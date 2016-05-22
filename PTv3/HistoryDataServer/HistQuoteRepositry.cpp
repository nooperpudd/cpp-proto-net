#include "stdafx.h"
#include "HistQuoteRepositry.h"
#include "LogFactory.h"
#include "HistQuoteFetcher.h"

log4cpp::Category& CHistQuoteRepositry::logger = CLogFactory::GetInstance().GetLogger("QuoteRepositry");

CHistQuoteRepositry::CHistQuoteRepositry()
	: m_pQuoteAgent(NULL)
{
}

CHistQuoteRepositry::~CHistQuoteRepositry()
{
}

void CHistQuoteRepositry::OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData, longlong timestamp)
{
	boost::unique_lock<boost::mutex> lock(m_storeMapMutex);

	boost::chrono::steady_clock::duration dd(timestamp);
	boost::chrono::steady_clock::time_point tp(dd);

	const string& symbol = marketData->InstrumentID;
	QuoteStoreMapIter iterStore = m_quoteStoreMap.find(symbol);
	if (iterStore != m_quoteStoreMap.end())
	{
		(iterStore->second)->Set(marketData, tp);
	}
	else
	{
		logger.warn(boost::str(boost::format("Receiving unexpected quotes for %s")
			% symbol));
	}
}

void CHistQuoteRepositry::OnConnected(bool reconnected)
{
}

CHistQuoteFetcher* CHistQuoteRepositry::CreateFetcher(const string& symbol)
{
	boost::unique_lock<boost::mutex> lock(m_storeMapMutex);

	CHistQuoteStore* pQuoteStore;
	// check if corresponding quote store exists
	QuoteStoreMapIter iterStore = m_quoteStoreMap.find(symbol);
	if (iterStore != m_quoteStoreMap.end())
	{
		// quote store already existing
		pQuoteStore = (iterStore->second).get();
	}
	else
	{
		QuoteStorePtr quoteStore(new CHistQuoteStore(symbol));
		m_quoteStoreMap.insert(make_pair(symbol, quoteStore));

		SubscribeQuote(symbol);
		pQuoteStore = quoteStore.get();
	}

	return pQuoteStore->GetFetcher();
}

void CHistQuoteRepositry::DestoryFetcher(CHistQuoteFetcher* pFetcher)
{
	boost::unique_lock<boost::mutex> lock(m_storeMapMutex);

	if (pFetcher == NULL)
		return;

	const string& symbol = pFetcher->Symbol();

	QuoteStoreMapIter iterStore = m_quoteStoreMap.find(symbol);
	if (iterStore != m_quoteStoreMap.end())
	{
		(iterStore->second)->EndIfOnlyOneLeft();
		int remainedFetcher = (iterStore->second)->ReclaimFetcher(pFetcher);
		logger.debug(boost::str(boost::format("Remaining fetcher of quote store(%s): %d.")
			% symbol % remainedFetcher));
		if (remainedFetcher < 1)
		{
			UnsubscribeQuote(symbol);
			m_quoteStoreMap.erase(iterStore);
		}
	}
}

void CHistQuoteRepositry::SubmitSubscribe()
{
	if(m_lazySubscribe && IsMarketReady() && m_sybmolsToSub.size() > 0)
	{
		m_pQuoteAgent->SubscribesQuotes(m_sybmolsToSub);
	}
}

bool CHistQuoteRepositry::IsMarketReady()
{
	return m_pQuoteAgent->IsConnected();
}

void CHistQuoteRepositry::SubscribeQuote(const string& symbol)
{
	if(m_lazySubscribe)
	{
		m_sybmolsToSub.push_back(symbol);
	}
	else if (IsMarketReady())
	{
		vector<string> symbols;
		symbols.push_back(symbol);
		m_pQuoteAgent->SubscribesQuotes(symbols);
	}
}

void CHistQuoteRepositry::UnsubscribeQuote(const string& symbol)
{
	if (IsMarketReady())
	{
		vector<string> symbols;

		symbols.push_back(symbol);
		m_pQuoteAgent->UnSubscribesQuotes(symbols);
	}
}
