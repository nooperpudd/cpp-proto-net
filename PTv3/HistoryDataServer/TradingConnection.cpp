#include "stdafx.h"
#include "TradingConnection.h"
#include "LogFactory.h"
#include "FileSystemUtil.h"

#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>

#if defined(WIN32)
#pragma comment(lib, "./ThostTraderApi/thosttraderapi.lib")
#else
#define strcpy_s strcpy
#define _strdup strdup
#endif

log4cpp::Category& CTradingConnection::logger = CLogFactory::GetInstance().GetLogger("TradingConnection");

CTradingConnection::CTradingConnection()
	: m_pUserApi(NULL)
	, m_isConnected(false)
	, m_isLoggedIn(false)
	, m_isConfirmed(false)
	, m_isWorking(false)
	, m_maxOrderRef(0)
{
}

CTradingConnection::~CTradingConnection()
{
}

void CTradingConnection::OnFrontConnected()
{
	boost::lock_guard<boost::mutex> lock(m_mutConnecting);
	m_isConnected = true;
	logger.info("Trade Server connected");
	m_condConnecting.notify_one();
}

void CTradingConnection::OnFrontDisconnected(int nReason)
{
	if (nReason == 0)
	{
		logger.info(boost::str(boost::format("%s Trade normally disconnected.") % m_investorId));
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

void CTradingConnection::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	boost::mutex::scoped_lock lock(m_mutLogin);

	m_isLoggedIn = (pRspInfo->ErrorID == 0);
	string loginInfo = boost::str(
		boost::format(
			"Trade login response (ReqId:%d): %s")
		% nRequestID
		% (m_isLoggedIn ? "Succeeded" : "Failed"));
	logger.info(loginInfo);

	if (m_isLoggedIn)
	{
		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		m_maxOrderRef = boost::lexical_cast<int>(pRspUserLogin->MaxOrderRef);
		//m_maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);

		string ds(pRspUserLogin->TradingDay);
		m_tradingDay = boost::gregorian::from_undelimited_string(ds);

		logger.infoStream() << "Trading day: " << pRspUserLogin->TradingDay << log4cpp::eol;
		logger.infoStream() << "Login Time: " << pRspUserLogin->LoginTime << log4cpp::eol;
		logger.infoStream() << "Broker ID: " << pRspUserLogin->BrokerID << log4cpp::eol;
		logger.infoStream() << "User ID: " << pRspUserLogin->UserID << log4cpp::eol;
		logger.infoStream() << "System name: " << pRspUserLogin->SystemName << log4cpp::eol;
		logger.infoStream() << "Front ID: " << pRspUserLogin->FrontID << log4cpp::eol;
		logger.infoStream() << "Session ID: " << pRspUserLogin->SessionID << log4cpp::eol;
		logger.infoStream() << "Maximum order ref: " << pRspUserLogin->MaxOrderRef << log4cpp::eol;
		logger.infoStream() << "SHFE time: " << pRspUserLogin->SHFETime << log4cpp::eol;
		logger.infoStream() << "DCE time: " << pRspUserLogin->DCETime << log4cpp::eol;
		logger.infoStream() << "CZCE time: " << pRspUserLogin->CZCETime << log4cpp::eol;
		logger.infoStream() << "FFEX time: " << pRspUserLogin->FFEXTime << log4cpp::eol;
	}
	else
	{
		logger.error(boost::str(boost::format("Error Message:%s") % pRspInfo->ErrorMsg));
	}

	m_condLogin.notify_one();
}

void CTradingConnection::OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	
}

void CTradingConnection::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast)
	{
		boost::unique_lock<boost::mutex> lock(m_mutConfirm);

		bool loginSuccess;
		string errorMsg;
		
		if (!IsErrorRspInfo(pRspInfo))
		{
			// Settlement confirm succeeded, then notify login success
			loginSuccess = true;
		}
		else
		{
			loginSuccess = false;
			errorMsg = pRspInfo->ErrorMsg;
		}

		m_isConfirmed = loginSuccess;
		m_loginErr = errorMsg;
		m_condConfirm.notify_one();
	}
}

bool CTradingConnection::Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
	try {
		string streamFolder = investorId + "/Td/";
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
		m_pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(streamFolder.c_str());
		// 注册事件类
		m_pUserApi->RegisterSpi(this);

		m_pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);
		m_pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);

		logger.info("Try to connect trade server (%s) ...", frontAddr.c_str());

#ifdef UDP_QUOTE
		logger.info("NOW FAKE Loging in TradingConnection.");
		FakeLogin();
#else
		// wait for connected event
		{
			boost::unique_lock<boost::mutex> lock(m_mutConnecting);
			m_thTrading = boost::thread(&CTradingConnection::RunThreadFunc, this, frontAddr);

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

		// wait for confirm
		{
			boost::unique_lock<boost::mutex> lock(m_mutConfirm);
			ReqSettlementInfoConfirm();

			if (!m_condConfirm.timed_wait(lock, boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS)))
			{
				m_loginErr = boost::str(boost::format("%s Confirm time out") % userId);
				logger.warn(m_loginErr);
				return false;
			}
			if (!m_isConfirmed)
			{
				return false;
			}
		}
#endif
		return true;
	}
	catch (std::exception& ex)
	{
		m_loginErr = "Failed to connect to trade server, details is following\n";
		m_loginErr += ex.what();
		logger.error(m_loginErr);
	}
	catch (...)
	{
		m_loginErr = "Unknown error encountered while connecting trade server";
		logger.error(m_loginErr);
	}

	return false;
}

void CTradingConnection::Logout()
{
	logger.info("%s Begin logging OUT...", m_investorId);
	if (m_isWorking)
	{
		m_pUserApi->Release();
		m_thTrading.join();
	}
	logger.info("%s Logged OUT.", m_investorId);
}

bool CTradingConnection::IsErrorRspInfo(CThostFtdcRspInfoField* pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
	{
		logger.debug(boost::str(boost::format("--->>> ErrorID=%d, ErrorMsg=%s")
			% pRspInfo->ErrorID % pRspInfo->ErrorMsg));
	}
	return bResult;
}

void CTradingConnection::RunThreadFunc(string address)
{
	// duplicate address string and use boost.smart_ptr to manage its life cycle
	boost::scoped_array<char> front_addr(_strdup(address.c_str()));
	m_isWorking = true;
	m_pUserApi->RegisterFront(front_addr.get());					// connect
	m_pUserApi->Init();

	m_pUserApi->Join();
	m_isWorking = false;
}

void CTradingConnection::Login()
{
	string traceInfo = boost::str(boost::format("Log in trade server (%s, %s, %s)")
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
			string loginInfo = boost::str(boost::format("Sending login %s, RequestID: %d")
				% (reqSucc ? "Succeeded" : "Failed") % requestId);
			logger.info(loginInfo);
		}
	}
	catch (...)
	{
		m_loginErr = "Encounter error while logging in trade server";
		logger.error(m_loginErr);
	}

	if (!reqSucc)
	{
		boost::lock_guard<boost::mutex> lock(m_mutConnecting);
		m_isConnected = false;
		m_condConnecting.notify_one();
	}
}

void CTradingConnection::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, m_brokerId.c_str());
	strcpy_s(req.InvestorID, m_investorId.c_str());
	int iResult = m_pUserApi->ReqSettlementInfoConfirm(&req, RequestIDIncrement());

	logger.infoStream() << "--->>> [" << m_investorId << "] 请求投资者结算结果确认: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败");
}

void CTradingConnection::FakeLogin()
{
	FRONT_ID = "123456";
	SESSION_ID = "654321";

	m_maxOrderRef = 1;
	//string ds(pRspUserLogin->TradingDay);
	string ds("20160710");
	m_tradingDay = boost::gregorian::from_undelimited_string(ds);
	string emptyStr;
	//m_orderProcessor->OnRspUserLogin(true, emptyStr, m_maxOrderRef);
}