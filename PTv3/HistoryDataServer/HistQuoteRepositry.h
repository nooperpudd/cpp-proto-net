#pragma once
#include "HistQuoteStore.h"
#include "QuoteAgentCallback.h"

#ifdef UDP_QUOTE
class CMarketDataUdp;
#else
class CMarketDataConnection;
#endif

class CHistQuoteFetcher;

class CHistQuoteRepositry : public CQuoteAgentCallback
{
public:
	CHistQuoteRepositry();
	~CHistQuoteRepositry();

#ifdef UDP_QUOTE
	void Init(CMarketDataUdp* pMarketDataConn, bool lazySubscribe = true)
#else
	void Init(CMarketDataConnection* pMarketDataConn, bool lazySubscribe = true)
#endif
	{
		m_pQuoteAgent = pMarketDataConn;
		m_lazySubscribe = lazySubscribe;
	}

	// CQuoteAgentCallback override virtual functions
	virtual void OnSubscribeCompleted() {}
	virtual void OnUnsubscribeCompleted() {}
	virtual void OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData, longlong timestamp);

	virtual void OnConnected(bool reconnected);

	CHistQuoteFetcher* CreateFetcher(const string& symbol);

	void DestoryFetcher(CHistQuoteFetcher* pFetcher);

	void SubmitSubscribe();
	bool IsLazySubscribe() const { return m_lazySubscribe; }

private:
	bool IsMarketReady();
	void SubscribeQuote(const string& symbol);
	void UnsubscribeQuote(const string& symbol);

	static log4cpp::Category& logger;

	typedef map<string, QuoteStorePtr> QuoteStoreMap;
	typedef QuoteStoreMap::iterator QuoteStoreMapIter;

	QuoteStoreMap m_quoteStoreMap;
	boost::mutex m_storeMapMutex;
#ifdef UDP_QUOTE
	CMarketDataUdp* m_pQuoteAgent;
#else
	CMarketDataConnection* m_pQuoteAgent;
#endif
	
	bool m_lazySubscribe;
	vector<string> m_sybmolsToSub;
};

