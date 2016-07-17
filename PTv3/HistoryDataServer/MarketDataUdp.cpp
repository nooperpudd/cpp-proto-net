#include "stdafx.h"
#include "MarketDataUdp.h"
#include "LogFactory.h"

log4cpp::Category& CMarketDataUdp::logger = CLogFactory::GetInstance().GetLogger("MarketDataUdp");

CMarketDataUdp::CMarketDataUdp()
{
}


CMarketDataUdp::~CMarketDataUdp()
{
}

bool CMarketDataUdp::Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
	Start(41115);
	return true;
}

void CMarketDataUdp::Logout()
{
	Stop();
}

bool CMarketDataUdp::SubscribesQuotes(vector<string>& subscribeArr)
{
	boost::mutex::scoped_lock lock(m_mutSymbol);

	for (vector<string>::iterator iter = subscribeArr.begin(); iter != subscribeArr.end(); ++iter)
	{
		m_symbols.insert(*iter);
		logger.debug(boost::str(boost::format("Udp Subscribe %s") % (*iter)));
	}
	return true;
}

bool CMarketDataUdp::UnSubscribesQuotes(vector<string>& unSubscribeArr)
{
	boost::mutex::scoped_lock lock(m_mutSymbol);
	for (vector<string>::iterator iter = unSubscribeArr.begin(); iter != unSubscribeArr.end(); ++iter)
	{
		boost::unordered_set<string>::iterator iterFound = m_symbols.find(*iter);
		if (iterFound != m_symbols.end())
		{
			m_symbols.erase(iterFound);
			logger.debug(boost::str(boost::format("Udp UnSubscribe %s") % (*iter)));
		}
	}
	return true;
}

void CMarketDataUdp::OnUdpDataReceived(char* pData, std::size_t nSize)
{
	boost::mutex::scoped_lock lock(m_mutSymbol);
	CThostFtdcDepthMarketDataField *pDepthMarketData = reinterpret_cast<CThostFtdcDepthMarketDataField*>(pData);
	logger.debug(boost::str(boost::format("Symbol coming %s") % pDepthMarketData->InstrumentID));
	boost::unordered_set<string>::iterator iterFound = m_symbols.find(pDepthMarketData->InstrumentID);
	if (iterFound != m_symbols.end())
	{
		longlong timestamp = boost::chrono::steady_clock::now().time_since_epoch().count();
		m_pCallback->OnQuoteReceived(pDepthMarketData, timestamp);
	}
}
