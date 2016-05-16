#pragma once
#include "MarketDataConnection.h"
#include "HistQuoteStore.h"

class CMarketDataConnection;
class CHistQuoteFetcher;

class CHistQuoteRepositry : public CQuoteAgentCallback
{
public:
	CHistQuoteRepositry();
	~CHistQuoteRepositry();

	void Init(CMarketDataConnection* pMarketDataConn) { m_pQuoteAgent = pMarketDataConn; }

	// CQuoteAgentCallback override virtual functions
	virtual void OnSubscribeCompleted() {}
	virtual void OnUnsubscribeCompleted() {}
	virtual void OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData, longlong timestamp);

	virtual void OnConnected(bool reconnected);

	CHistQuoteFetcher* CreateFetcher(const string& symbol);

	void DestoryFetcher(CHistQuoteFetcher* pFetcher);

private:
	bool IsMarketReady();
	void SubscribeQuote(const string& symbol);
	void UnsubscribeQuote(const string& symbol);

	static log4cpp::Category& logger;

	typedef map<string, QuoteStorePtr> QuoteStoreMap;
	typedef QuoteStoreMap::iterator QuoteStoreMapIter;

	QuoteStoreMap m_quoteStoreMap;
	boost::mutex m_storeMapMutex;

	CMarketDataConnection* m_pQuoteAgent;
};

