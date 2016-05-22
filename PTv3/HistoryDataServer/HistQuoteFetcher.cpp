#include "StdAfx.h"
#include "HistQuoteFetcher.h"
#include "HistQuoteStore.h"
#include "LogFactory.h"

log4cpp::Category& CHistQuoteFetcher::logger = CLogFactory::GetInstance().GetLogger("QuoteFetcher");

CHistQuoteFetcher::CHistQuoteFetcher(CHistQuoteStore* pStore):
m_pQuoteStore(pStore)
, m_isRunning(false)
{
}

CHistQuoteFetcher::~CHistQuoteFetcher(void)
{
	logger.debug(boost::str(boost::format("Destructing QuoteFetcher(%s) ...") % Symbol()));
	m_thread.join();
	logger.debug(boost::str(boost::format("Destructed QuoteFetcher(%s)") % Symbol()));
}

const string& CHistQuoteFetcher::Symbol()
{
	return m_pQuoteStore->Symbol();
}

void CHistQuoteFetcher::Run(QuoteUpdateFunc quoteUpdateFunc)
{
	m_cbFunc = quoteUpdateFunc;
	m_isRunning.store(true, boost::memory_order_relaxed);
	m_thread = boost::thread(boost::bind(&CHistQuoteFetcher::FetchingProc, this));
}

void CHistQuoteFetcher::Detach()
{
	logger.debug(boost::str(boost::format("QuoteFetcher(%s) is detached") % Symbol()));
	m_isRunning.store(false, boost::memory_order_release);
	m_cbFunc.clear();
}

void CHistQuoteFetcher::FetchingProc()
{
	while(m_isRunning.load(boost::memory_order_relaxed))
	{
		static boost::chrono::steady_clock::time_point zeroTP;
		CQuote quote;
		boost::chrono::steady_clock::time_point retTimestamp = 
			m_pQuoteStore->Get(m_timestamp, &quote);
		if(retTimestamp > zeroTP)
		{
			if(m_isRunning.load(boost::memory_order_acquire))
			{
				m_timestamp = retTimestamp;
				if(!m_cbFunc.empty())
				{
					m_cbFunc(m_timestamp, &quote);
				}
			}
			else
			{
				logger.debug("Quote fetcher has been stop.");
			}
		}
		else
		{
			logger.debug("Quote store passing ZERO timestamp for exit");
			break;
		}
	}
}

