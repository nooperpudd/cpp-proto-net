#include "StdAfx.h"
#include "QuoteUdpAgent.h"
#include "globalmembers.h"

#ifdef USE_ZEUSING_API
#include "ParseZeusingMarketData.h"
#endif // USE_ZEUSING_API

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

const int ZEUSING_QUOTE_PORTS_COUNT = 3;
int ZEUSING_QUOTE_PORTS[ZEUSING_QUOTE_PORTS_COUNT] = { 28444, 55521, 28333 };

CQuoteUdpMultiAgent::CQuoteUdpMultiAgent(CQuoteAgentCallback* pCallback)
	: CQuoteAgentBase(pCallback)
{
}

CQuoteUdpMultiAgent::~CQuoteUdpMultiAgent()
{
}

int GetQuoteBroadcastPort(const string& frontAddr)
{
	bool isUdp = boost::istarts_with(frontAddr, "udp://255.255.255.255");
	int nPort = -1;
	if (isUdp)
	{
		string portStr = frontAddr.substr(22);
		try
		{
			nPort = boost::lexical_cast<int>(portStr);
		}
		catch (const boost::bad_lexical_cast &)
		{
		}
	}
	return nPort;
}

boost::tuple<bool, string> CQuoteUdpMultiAgent::Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
	m_brokerID = brokerId;
	m_userID = investorId;

	int nPort = GetQuoteBroadcastPort(frontAddr);
	if (nPort > 0)
	{
		boost::unordered_set<int> ports;
		ports.insert(nPort);

#ifdef USE_ZEUSING_API
		for (int i = 0; i < ZEUSING_QUOTE_PORTS_COUNT; ++i)
		{
			ports.insert(ZEUSING_QUOTE_PORTS[i]);
		}
#endif

		for (boost::unordered_set<int>::iterator iter = ports.begin(); iter != ports.end(); ++iter)
		{
			QuoteUdpAgentPtr quoteAgent(new CQuoteUdpAgent(this));
			m_childAgents.push_back(quoteAgent);
			int port = *iter;
			quoteAgent->Start(port);
		}

		m_bIsConnected = true;
		return boost::make_tuple(true, "");
	}
	else
	{
		return boost::make_tuple(false, "Cannot find port from connection string");
	}
}

void CQuoteUdpMultiAgent::Logout()
{
	if (m_bIsConnected)
	{
		m_bIsConnected = false;
		for (QuoteUdpAgentVectorIter iter = m_childAgents.begin(); iter != m_childAgents.end(); ++iter)
		{
			(*iter)->Stop();
		}
		m_childAgents.clear();
	}
}

void CQuoteUdpMultiAgent::OnQuoteReceived(int nPort, CThostFtdcDepthMarketDataField* pDepthMarketData, longlong timestamp)
{
	boost::mutex::scoped_lock l(m_mutex);

	long delay = 0;
	string symbol = pDepthMarketData->InstrumentID;
	QuoteTimestampMapIter iter = m_lastQuoteTimestamp.find(symbol);
	if (iter == m_lastQuoteTimestamp.end())
	{
		TimestampPtr ts(new CQuoteTimestamp(pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec));
		boost::chrono::steady_clock::duration d(timestamp);
		ts->LastTime = boost::chrono::steady_clock::time_point(d);
		m_lastQuoteTimestamp.insert(make_pair(symbol, ts));
		m_pCallback->OnQuoteReceived(pDepthMarketData, timestamp);
	}
	else
	{
		if (iter->second->UpdateTime == pDepthMarketData->UpdateTime &&
			iter->second->MilliSeconds == pDepthMarketData->UpdateMillisec)
		{
			boost::chrono::steady_clock::duration elapsed = boost::chrono::steady_clock::now() - iter->second->LastTime;
			delay = boost::chrono::duration_cast<boost::chrono::microseconds>(elapsed).count();
		}
		else
		{
			iter->second->UpdateTime = pDepthMarketData->UpdateTime;
			iter->second->MilliSeconds = pDepthMarketData->UpdateMillisec;
			boost::chrono::steady_clock::duration d(timestamp);
			iter->second->LastTime = boost::chrono::steady_clock::time_point(d);
			m_pCallback->OnQuoteReceived(pDepthMarketData, timestamp);
		}
	}

	/*
	if (delay == 0)
	{
		pLogger->Info(boost::str(boost::format("Quote(%s): %s\t%d\t%.1f\t%.1f\t%.1f\t%.1f")
			% symbol % pDepthMarketData->UpdateTime % pDepthMarketData->UpdateMillisec
			% pDepthMarketData->OpenPrice % pDepthMarketData->HighestPrice % pDepthMarketData->LowestPrice % pDepthMarketData->LastPrice));
	}
	*/
}

bool CQuoteUdpAgent::SubscribesQuotes(vector<string>& subscribeArr)
{
	boost::mutex::scoped_lock lock(m_mutSymbol);

	for (vector<string>::iterator iter = subscribeArr.begin(); iter != subscribeArr.end(); ++iter)
	{
		m_symbols.insert(*iter);
		LOG_DEBUG(logger, boost::str(boost::format("Subscribe %s") % (*iter)));
	}
	return true;
}

bool CQuoteUdpAgent::UnSubscribesQuotes(vector<string>& unSubscribeArr)
{
	boost::mutex::scoped_lock lock(m_mutSymbol);
	for (vector<string>::iterator iter = unSubscribeArr.begin(); iter != unSubscribeArr.end(); ++iter)
	{
		boost::unordered_set<string>::iterator iterFound = m_symbols.find(*iter);
		if (iterFound != m_symbols.end())
		{
			m_symbols.erase(iterFound);
		}
	}
	return true;
}

#ifdef USE_ZEUSING_API
CThostFtdcDepthMarketDataField g_DepthMarketData;
#endif

void CQuoteUdpAgent::OnUdpDataReceived(char* pData, std::size_t nSize)
{
	boost::mutex::scoped_lock lock(m_mutSymbol);

#ifdef USE_ZEUSING_API
	ParseMarketData(pData, nSize, &g_DepthMarketData);
	CThostFtdcDepthMarketDataField *pDepthMarketData = &g_DepthMarketData;
#else
	CThostFtdcDepthMarketDataField *pDepthMarketData = reinterpret_cast<CThostFtdcDepthMarketDataField*>(pData);
#endif
	//LOG_DEBUG(logger, boost::str(boost::format("Symbol coming %s") % pDepthMarketData->InstrumentID));
	boost::unordered_set<string>::iterator iterFound = m_symbols.find(pDepthMarketData->InstrumentID);
	if (iterFound != m_symbols.end())
	{
		longlong timestamp = boost::chrono::steady_clock::now().time_since_epoch().count();
		m_parentAgent->OnQuoteReceived(m_nPort, pDepthMarketData, timestamp);
		//LOG_DEBUG(logger, boost::str(boost::format("Passed down quote for Symbol %s") % pDepthMarketData->InstrumentID));
	}
}


