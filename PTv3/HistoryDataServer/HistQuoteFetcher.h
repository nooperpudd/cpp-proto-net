#pragma once

#include <boost/atomic.hpp>
#include "Quote.h"

class CHistQuoteStore;

class CHistQuoteFetcher
{
public:
	CHistQuoteFetcher(CHistQuoteStore* pStore);
	~CHistQuoteFetcher(void);

	const string& Symbol();
	void Run(QuoteUpdateFunc quoteUpdateFunc);
	void Detach();

private:
	void FetchingProc();

	static log4cpp::Category& logger;

	CHistQuoteStore* m_pQuoteStore;
	boost::thread m_thread;
	boost::atomic<bool> m_isRunning;
	
	QuoteUpdateFunc m_cbFunc;
	boost::chrono::steady_clock::time_point m_timestamp;
};

