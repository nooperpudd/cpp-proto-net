#pragma once

#include "QuoteAgentBase.h"
#include "UdpQuoteListener.h"
#include "QuoteTimestamp.h"

#include <boost/unordered_set.hpp>

class CQuoteUdpMultiAgent;

class CQuoteUdpAgent
{
public:
	CQuoteUdpAgent(CQuoteUdpMultiAgent* pParentAgent) 
		: m_parentAgent(pParentAgent)
		, m_nPort(-1)
	{}
	~CQuoteUdpAgent(){}

	void Start(int nPort)
	{
		m_udpListener = UdpQuoteListenerPtr(
			new CUdpQuoteListener(nPort,
			boost::bind(&CQuoteUdpAgent::OnUdpDataReceived, this, _1, _2)));
		m_nPort = nPort;
	}
	void Stop(){ m_udpListener.reset(); }

	bool SubscribesQuotes(vector<string>& subscribeArr);
	bool UnSubscribesQuotes(vector<string>& unSubscribeArr);

private:
	void OnUdpDataReceived(char* pData, std::size_t nSize);

	CQuoteUdpMultiAgent* m_parentAgent;
	UdpQuoteListenerPtr m_udpListener;
	
	boost::unordered_set<string> m_symbols;
	boost::mutex m_mutSymbol;
	int m_nPort;
};

typedef boost::shared_ptr<CQuoteUdpAgent> QuoteUdpAgentPtr;
typedef vector<QuoteUdpAgentPtr> QuoteUdpAgentVector;
typedef QuoteUdpAgentVector::iterator QuoteUdpAgentVectorIter;

class CQuoteUdpMultiAgent : public CQuoteAgentBase
{
public:
	CQuoteUdpMultiAgent(CQuoteAgentCallback* pCallback);
	virtual ~CQuoteUdpMultiAgent();

	boost::tuple<bool, string> Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password);
	void Logout();

	bool SubscribesQuotes(vector<string>& subscribeArr)
	{
		for (QuoteUdpAgentVectorIter iter = m_childAgents.begin(); iter != m_childAgents.end(); ++iter)
		{
			(*iter)->SubscribesQuotes(subscribeArr);
		}
		return true;
	}
	bool UnSubscribesQuotes(vector<string>& unSubscribeArr)
	{
		for (QuoteUdpAgentVectorIter iter = m_childAgents.begin(); iter != m_childAgents.end(); ++iter)
		{
			(*iter)->UnSubscribesQuotes(unSubscribeArr);
		}
		return true;
	}

	virtual void OnQuoteReceived(int nPort, CThostFtdcDepthMarketDataField* pDepthMarketData, longlong timestamp);

private:
	QuoteUdpAgentVector m_childAgents;

	QuoteTimestampMap m_lastQuoteTimestamp;
	boost::mutex m_mutex;
};

class CQuoteUdpSingleAgent : public CQuoteUdpMultiAgent
{
public:
	CQuoteUdpSingleAgent(CQuoteAgentCallback* pCallback)
		: CQuoteUdpMultiAgent(pCallback)
	{}
	~CQuoteUdpSingleAgent(){}

	void OnQuoteReceived(int nPort, CThostFtdcDepthMarketDataField* pDepthMarketData, longlong timestamp)
	{
		m_pCallback->OnQuoteReceived(pDepthMarketData, timestamp);
	}
};
