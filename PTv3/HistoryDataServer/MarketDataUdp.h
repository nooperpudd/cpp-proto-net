#pragma once

#include "UdpListener.h"
#include "QuoteAgentCallback.h"

#include <boost/unordered_set.hpp>

class CMarketDataUdp
{
public:
	CMarketDataUdp();
	~CMarketDataUdp();

	void Init(CQuoteAgentCallback* pCallback) { m_pCallback = pCallback; }
	bool Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password);
	void Logout();
	bool IsConnected() const
	{
		return m_isConnected;
	}

	void Start(int nPort)
	{
		m_udpListener = UdpListenerPtr(
			new CUdpListener(nPort,
				boost::bind(&CMarketDataUdp::OnUdpDataReceived, this, _1, _2)));
		m_nPort = nPort;
		m_isConnected = true;
	}
	void Stop()
	{
		m_udpListener.reset();
		m_isConnected = false;
	}

	bool SubscribesQuotes(vector<string>& subscribeArr);
	bool UnSubscribesQuotes(vector<string>& unSubscribeArr);

private:
	void OnUdpDataReceived(char* pData, std::size_t nSize);

	static log4cpp::Category& logger;

	CQuoteAgentCallback* m_pCallback;
	UdpListenerPtr m_udpListener;

	boost::unordered_set<string> m_symbols;
	boost::mutex m_mutSymbol;
	int m_nPort;
	bool m_isConnected;
};

