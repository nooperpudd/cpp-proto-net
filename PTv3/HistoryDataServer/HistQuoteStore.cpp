#include "stdafx.h"
#include "HistQuoteStore.h"
#include "HistQuoteFetcher.h"
#include "LogFactory.h"

log4cpp::Category& CHistQuoteStore::logger = CLogFactory::GetInstance().GetLogger("QuoteStore");

CHistQuoteStore::CHistQuoteStore(const string& symbol)
	: m_symbol(symbol)
	, m_isEnd(false)
{
	memset(&m_cachedQuoteData, 0, sizeof(m_cachedQuoteData));
}

CHistQuoteStore::~CHistQuoteStore(void)
{
	logger.debug(boost::str(boost::format("Destructing Quote Store(%s) with %d fetcher ... ") 
		% m_symbol % m_reclaimedFetcher.size()));

	for(vector<CHistQuoteFetcher*>::iterator iter = m_reclaimedFetcher.begin();
	iter != m_reclaimedFetcher.end(); ++iter)
	{
		if(*iter != NULL)
			delete *iter;
	}

	logger.debug(boost::str(boost::format("Destructed Quote Store(%s)") % m_symbol));
}

void CHistQuoteStore::Set(CThostFtdcDepthMarketDataField* pQuoteData, boost::chrono::steady_clock::time_point& tpTimestamp)
{
	// as struct is value type, this should be correct
	boost::unique_lock<boost::mutex> lock(m_quoteMutex);
	m_quoteTimestamp = tpTimestamp;
	m_cachedQuoteData = *pQuoteData;
	m_cond.notify_all();
}

boost::chrono::steady_clock::time_point CHistQuoteStore::Get( 
	boost::chrono::steady_clock::time_point timestamp, CQuote* outQuote )
{
	static boost::chrono::steady_clock::time_point zeroPoint;
	if(timestamp == zeroPoint 
		&& m_quoteTimestamp > zeroPoint)
	{
		// It's the first time of Fetcher getting quote and 
		// store already has cached quote
		GetQuote(outQuote);
	}
	else
	{
		boost::unique_lock<boost::mutex> lock(m_quoteMutex);
		if(!m_isEnd)
		{
			m_cond.wait(lock);

			if(m_quoteTimestamp > timestamp)
				GetQuote(outQuote);
		}
	}
	
	return m_quoteTimestamp;
}

CHistQuoteFetcher* CHistQuoteStore::GetFetcher()
{
	boost::unique_lock<boost::mutex> lock(m_fetcherSetMutex);

	CHistQuoteFetcher* fetcher = new CHistQuoteFetcher(this);
	m_fetcherSet.insert(fetcher);
	return fetcher;
}

int CHistQuoteStore::ReclaimFetcher(CHistQuoteFetcher* pFetcher )
{
	boost::unique_lock<boost::mutex> lock(m_fetcherSetMutex);
	pFetcher->Detach();
	m_fetcherSet.erase(pFetcher);
	m_reclaimedFetcher.push_back(pFetcher);
	
	return m_fetcherSet.size();
}

void CHistQuoteStore::GetQuote( CQuote* outQuote )
{
	outQuote->set_symbol(m_cachedQuoteData.InstrumentID);
	outQuote->set_trading_day(m_cachedQuoteData.TradingDay);

	outQuote->set_last(m_cachedQuoteData.LastPrice);
	outQuote->set_open(m_cachedQuoteData.OpenPrice);
	outQuote->set_high(m_cachedQuoteData.HighestPrice);
	outQuote->set_low(m_cachedQuoteData.LowestPrice);
	outQuote->set_volume(m_cachedQuoteData.Volume);
	outQuote->set_close(m_cachedQuoteData.ClosePrice);

	outQuote->set_update_time(m_cachedQuoteData.UpdateTime);
#ifdef QUOTE_TIME_EMU
	
	boost::chrono::seconds tp = ParseTimeString(m_cachedQuoteData.UpdateTime);
	//tp -= boost::chrono::hours(10);
	//tp -= boost::chrono::minutes(3);
	string updateTime = GetISOTimeString(tp);
	outQuote->set_update_time(updateTime);
	/*
	outQuote->set_update_time("09:15:58");
	outQuote->set_open(2308.6);
	outQuote->set_high(2309.8);
	outQuote->set_low(2305.4);
	outQuote->set_last(2306.6);
	*/

#endif // QUOTE_TIME_EMU
	
	outQuote->set_update_millisec(m_cachedQuoteData.UpdateMillisec);

	outQuote->set_bid(m_cachedQuoteData.BidPrice1);
	outQuote->set_bid_size(m_cachedQuoteData.BidVolume1);
	outQuote->set_ask(m_cachedQuoteData.AskPrice1);
	outQuote->set_ask_size(m_cachedQuoteData.AskVolume1);
	/*
	outQuote->set_bid_2(m_cachedQuoteData.BidPrice2);
	outQuote->set_bid_size_2(m_cachedQuoteData.BidVolume2);
	outQuote->set_ask_2(m_cachedQuoteData.AskPrice2);
	outQuote->set_ask_size_2(m_cachedQuoteData.AskVolume2);
	outQuote->set_bid_3(m_cachedQuoteData.BidPrice3);
	outQuote->set_bid_size_3(m_cachedQuoteData.BidVolume3);
	outQuote->set_ask_3(m_cachedQuoteData.AskPrice3);
	outQuote->set_ask_size_3(m_cachedQuoteData.AskVolume3);
	outQuote->set_bid_4(m_cachedQuoteData.BidPrice4);
	outQuote->set_bid_size_4(m_cachedQuoteData.BidVolume4);
	outQuote->set_ask_4(m_cachedQuoteData.AskPrice4);
	outQuote->set_ask_size_4(m_cachedQuoteData.AskVolume4);
	outQuote->set_bid_5(m_cachedQuoteData.BidPrice5);
	outQuote->set_bid_size_5(m_cachedQuoteData.BidVolume5);
	outQuote->set_ask_5(m_cachedQuoteData.AskPrice5);
	outQuote->set_ask_size_5(m_cachedQuoteData.AskVolume5);
	*/
	outQuote->set_exchange_id(m_cachedQuoteData.ExchangeID);
	outQuote->set_exchange_symbol_id(m_cachedQuoteData.ExchangeInstID);
	outQuote->set_average_price(m_cachedQuoteData.AveragePrice);
	
}

void CHistQuoteStore::EndIfOnlyOneLeft()
{
	boost::unique_lock<boost::mutex> lock(m_quoteMutex);
	if(m_fetcherSet.size() == 1)
	{
		m_quoteTimestamp = boost::chrono::steady_clock::time_point();
		m_isEnd = true;
		m_cond.notify_all();
		logger.debug(boost::str(boost::format("QuoteStore(%s) Notify all QuoteFetcher to stop with zero timestamp") % m_symbol));
	}
}
