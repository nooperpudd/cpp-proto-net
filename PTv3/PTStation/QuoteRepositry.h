#pragma once

#include "QuoteAgentCallback.h"
#include "QuoteFetcher.h"
#include "QuoteStore.h"

class CQuoteAgent;

class CQuoteRepositry : public CQuoteAgentCallback
{
public:
	CQuoteRepositry(void);
	~CQuoteRepositry(void);

	// CQuoteAgentCallback override virtual functions
	virtual void OnSubscribeCompleted(){}
	virtual void OnUnsubscribeCompleted(){}
#ifndef USE_FEMAS_API
	virtual void OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData, longlong timestamp);
#else
	virtual void OnQuoteReceived(CUstpFtdcDepthMarketDataField* marketData, longlong timestamp);
#endif
	
	virtual void OnConnected(bool reconnected);

	void Init(CQuoteAgent* pQuoteAgent){ m_pQuoteAgent = pQuoteAgent; }
	CQuoteFetcher* CreateFetcher(const string& symbol);
	void DestoryFetcher(CQuoteFetcher* pFetcher);

private:
	bool IsMarketReady();
	void SubscribeQuote(const string& symbol);
	void UnsubscribeQuote(const string& symbol);

	typedef map<string, QuoteStorePtr> QuoteStoreMap;
	typedef QuoteStoreMap::iterator QuoteStoreMapIter;
	
	QuoteStoreMap m_quoteStoreMap;
	boost::mutex m_storeMapMutex;

	CQuoteAgent* m_pQuoteAgent;
	
};

