#include "stdafx.h"
#include "MarketDataConnection.h"
#include "LogFactory.h"
#include "FileSystemUtil.h"
#include <boost/smart_ptr/shared_array.hpp>
#include <boost/date_time.hpp>


#if defined(WIN32)
#pragma comment(lib, "./ThostTraderApi/thostmduserapi.lib")
#else
#define strcpy_s strcpy
#define _strdup strdup
#endif

log4cpp::Category& CMarketDataConnection::logger = CLogFactory::GetInstance().GetLogger("MarketDataConnection");

CMarketDataConnection::CMarketDataConnection()
	: m_pCallback(NULL)
	, m_pUserApi(NULL)
	, m_isWorking(false)
	, m_isConnected(false)
	, m_isLoggedIn(false)
{
}


CMarketDataConnection::~CMarketDataConnection()
{
}

void CMarketDataConnection::OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	logger.errorStream() << "--->>> " << "OnRspError" << log4cpp::eol;
	IsErrorRspInfo(pRspInfo);
}

void CMarketDataConnection::OnFrontDisconnected(int nReason)
{
	if (nReason == 0)
	{
		logger.info(boost::str(boost::format("%s Market Data normally disconnected.") % m_investorId));
	}
	else
	{
		string reasonTxt = boost::str(boost::format("%s disconnected from trade server due to ") % m_investorId);
		switch (nReason)
		{
		case 0x1001:
			reasonTxt.append("Cannot read from network");
			break;
		case 0x1002:
			reasonTxt.append("Cannot write to network");
			break;
		case 0x2001:
			reasonTxt.append("Receiving heart beat time out");
			break;
		case 0x2002:
			reasonTxt.append("Sending heart beat time out");
			break;
		case 0x2003:
			reasonTxt.append("Invalid packets received");
			break;
		}
		reasonTxt.append(" (will reconnect automatically).");
		logger.warn(reasonTxt);
	}

	m_isLoggedIn = false;
	m_isConnected = false;
}

void CMarketDataConnection::OnFrontConnected()
{
	boost::lock_guard<boost::mutex> lock(m_mutConnecting);
	m_isConnected = true;
	logger.info("Market Data Server connected");
	m_condConnecting.notify_one();
}

void CMarketDataConnection::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	boost::mutex::scoped_lock lock(m_mutLogin);

	m_isLoggedIn = (pRspInfo->ErrorID == 0);
	string loginInfo = boost::str(
		boost::format(
			"Market Data login response (ReqId:%d): %s")
		% nRequestID
		% (m_isLoggedIn ? "Succeeded" : "Failed"));
	logger.info(loginInfo);

	if (m_isLoggedIn)
	{
		// 保存会话参数

		string ds(pRspUserLogin->TradingDay);
		m_tradingDay = boost::gregorian::from_undelimited_string(ds);

		logger.infoStream() << "Trading day: " << pRspUserLogin->TradingDay << log4cpp::eol;
	}
	else
	{
		logger.error(boost::str(boost::format("Error Message:%s") % pRspInfo->ErrorMsg));
	}

	m_condLogin.notify_one();
}

void CMarketDataConnection::OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	logger.infoStream() << "OnRspSubMarketData - " << pSpecificInstrument->InstrumentID <<
		(pRspInfo->ErrorID == 0 ? " Succeeded" : " Failed") << log4cpp::eol;
}

void CMarketDataConnection::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	logger.infoStream() << "OnRspUnSubMarketData - " << pSpecificInstrument->InstrumentID <<
		(pRspInfo->ErrorID == 0 ? " Succeeded" : " Failed") << log4cpp::eol;
}

void CMarketDataConnection::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	longlong timestamp = boost::chrono::steady_clock::now().time_since_epoch().count();
	m_pCallback->OnQuoteReceived(pDepthMarketData, timestamp);
}

bool CMarketDataConnection::Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
	try {
		string streamFolder = investorId + "/Md/";
		if (!CreateFolderIfNotExists(streamFolder))
		{
			m_loginErr = boost::str(boost::format("Cannot create stream folder (%s) for trading") % streamFolder);
			return false;
		}

		m_brokerId = brokerId;
		m_investorId = investorId;
		m_userId = userId;
		m_password = password;

		// 创建UserApi
		m_pUserApi = CThostFtdcMdApi::CreateFtdcMdApi(streamFolder.c_str(), false);
		assert(m_pUserApi != NULL);
		// 注册事件类
		m_pUserApi->RegisterSpi(this);

		logger.info("Try to connect trade server (%s) ...", frontAddr.c_str());

		// wait for connected event
		{
			boost::unique_lock<boost::mutex> lock(m_mutConnecting);
			m_thQuoting = boost::thread(&CMarketDataConnection::RunThreadFunc, this, frontAddr);

			if (!m_condConnecting.timed_wait(lock, boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS)))
			{
				m_loginErr = boost::str(boost::format("Connecting %s time out") % frontAddr);
				logger.warn(m_loginErr);
				return false;
			}

			if (!m_isConnected)
			{
				logger.error(boost::str(boost::format("Connecting %s encounter unknown error") % frontAddr));
				return false;
			}
		}

		// wait for login
		{
			boost::unique_lock<boost::mutex> lock(m_mutLogin);
			Login();

			if (!m_condLogin.timed_wait(lock, boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS)))
			{
				m_loginErr = boost::str(boost::format("Login %s time out") % userId);
				logger.warn(m_loginErr);
				return false;
			}
			if (!m_isLoggedIn)
			{
				return false;
			}
		}

		return true;
	}
	catch (std::exception& ex)
	{
		m_loginErr = "Failed to connect to Market Data server, details is following\n";
		m_loginErr += ex.what();
		logger.error(m_loginErr);
	}
	catch (...)
	{
		m_loginErr = "Unknown error encountered while connecting Market Data server";
		logger.error(m_loginErr);
	}

	return false;
}

void CMarketDataConnection::Logout()
{
	logger.info("%s Begin logging OUT Market Data...", m_investorId.c_str());
	if (m_isWorking)
	{
		m_pUserApi->Release();
		m_thQuoting.join();
	}
	logger.info("%s Logged OUT Market Data.", m_investorId.c_str());
}

void CMarketDataConnection::SubscribeMarketData(char** symbolArr, int symCount)
{
	if (symbolArr != NULL && symCount > 0)
	{
		boost::unique_lock<boost::mutex> l(m_mutex);
		if (m_isWorking)
		{
			logger.infoStream() << m_investorId << " Subscribing " << symCount 
				<< " symbol(s). The first is " << symbolArr[0] << log4cpp::eol;
			int iResult = m_pUserApi->SubscribeMarketData(symbolArr, symCount);
			logger.infoStream() << "The result of SubscribeMarketData " << iResult << log4cpp::eol;
		}
	}
}

void CMarketDataConnection::UnsubscribeMarketData(char** symbolArr, int symCount)
{
	if (symbolArr != NULL && symCount > 0)
	{
		boost::unique_lock<boost::mutex> l(m_mutex);
		if(m_isWorking)
		{
			logger.infoStream() << m_investorId << " Un-Subscribing " << symCount 
				<< " symbol(s). The first is " << symbolArr[0] << log4cpp::eol;
			int iResult = m_pUserApi->UnSubscribeMarketData(symbolArr, symCount);
			logger.infoStream() << "The result of UnSubscribeMarketData " << iResult << log4cpp::eol;
		}
	}
}

void CMarketDataConnection::SubscribesQuotes(vector<string>& symbols)
{
	int size = symbols.size();
	boost::shared_array<char*> symbolArray(new char*[size]);
	for (int i = 0; i < size; ++i)
	{
		symbolArray[i] = const_cast<char*>(symbols[i].c_str());
	}
	
	SubscribeMarketData(symbolArray.get(), size);
}

void CMarketDataConnection::UnSubscribesQuotes(vector<string>& symbols)
{
	int size = symbols.size();
	boost::shared_array<char*> symbolArray(new char*[size]);
	for (int i = 0; i < size; ++i)
	{
		symbolArray[i] = const_cast<char*>(symbols[i].c_str());
	}

	UnsubscribeMarketData(symbolArray.get(), size);
}

bool CMarketDataConnection::IsErrorRspInfo(CThostFtdcRspInfoField* pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		logger.errorStream() << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << log4cpp::eol;
	return bResult;
}

void CMarketDataConnection::RunThreadFunc(string address)
{
	int conStrLen = address.length();
	boost::shared_array<char> FRONT_ADDR(new char[conStrLen + 1]);
	strncpy(FRONT_ADDR.get(), address.c_str(), conStrLen);

	m_isWorking = true;
	m_pUserApi->RegisterFront(FRONT_ADDR.get());
	m_pUserApi->Init();

	m_pUserApi->Join();
	m_isWorking = false;
}

void CMarketDataConnection::Login()
{
	string traceInfo = boost::str(boost::format("Log in Market Data server (%s, %s, %s)")
		% m_brokerId % m_investorId % m_password);
	logger.info(traceInfo);

	bool reqSucc = false;
	try
	{
		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy_s(req.BrokerID, m_brokerId.c_str());
		strcpy_s(req.UserID, m_investorId.c_str());
		strcpy_s(req.Password, m_password.c_str());
		if (m_pUserApi != NULL)
		{
			int requestId = RequestIDIncrement();
			int iResult = m_pUserApi->ReqUserLogin(&req, requestId);
			reqSucc = iResult == 0;
			string loginInfo = boost::str(boost::format("Requesting log in Market Data Server %s, RequestID: %d")
				% (reqSucc ? "Succeeded" : "Failed") % requestId);
			logger.info(loginInfo);
		}
	}
	catch (...)
	{
		m_loginErr = "Encounter error while logging in Market Data server";
		logger.error(m_loginErr);
	}

	if (!reqSucc)
	{
		boost::lock_guard<boost::mutex> lock(m_mutConnecting);
		m_isConnected = false;
		m_condConnecting.notify_one();
	}
}
