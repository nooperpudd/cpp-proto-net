#pragma once

#include "ThostTraderApi/ThostFtdcMdApi.h"
#include "Quote.h"

#include <boost/unordered_set.hpp>

class CHistQuoteFetcher;

class CHistQuoteStore
{
public:
	CHistQuoteStore(const string& symbol);
	~CHistQuoteStore(void);

	const string& Symbol() const
	{ return m_symbol; }

	CHistQuoteFetcher* GetFetcher();
	int ReclaimFetcher(CHistQuoteFetcher* pFetcher);

	void Set(CThostFtdcDepthMarketDataField* pQuoteData, boost::chrono::steady_clock::time_point& tpTimestamp);
	
	boost::chrono::steady_clock::time_point Get(
		boost::chrono::steady_clock::time_point timestamp, CQuote* outQuote);

	void EndIfOnlyOneLeft();

private:
	void GetQuote(CQuote* outQuote);

	static log4cpp::Category& logger;

	string m_symbol;

	boost::unordered_set<CHistQuoteFetcher*> m_fetcherSet;
	vector<CHistQuoteFetcher*> m_reclaimedFetcher;
	boost::mutex m_fetcherSetMutex;

	boost::condition_variable m_cond;
	boost::mutex m_quoteMutex;

	CThostFtdcDepthMarketDataField m_cachedQuoteData;
	
	boost::chrono::steady_clock::time_point m_quoteTimestamp;
	bool m_isEnd;
};

typedef boost::shared_ptr<CHistQuoteStore> QuoteStorePtr;



