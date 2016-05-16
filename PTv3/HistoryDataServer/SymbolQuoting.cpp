#include "stdafx.h"
#include "SymbolQuoting.h"
#include "HistQuoteRepositry.h"
#include "HistQuoteFetcher.h"
#include "LogFactory.h"

log4cpp::Category& CSymbolQuoting::logger = CLogFactory::GetInstance().GetLogger("SymbolQuoting");

CSymbolQuoting::CSymbolQuoting(const string& symbol, const string& timeFrame)
	: m_symbol(symbol)
	, m_timeFrame(timeFrame)
	, m_pFetcher(NULL)
{
}

CSymbolQuoting::~CSymbolQuoting()
{
}

bool CSymbolQuoting::Subscribe(CHistQuoteRepositry* pQuoteRepo)
{
	m_pFetcher = pQuoteRepo->CreateFetcher(m_symbol);
	m_pFetcher->Run(boost::bind(&CSymbolQuoting::OnQuoteRecevied, this, _1, _2));
	return true;
}

void CSymbolQuoting::Unsubscribe(CHistQuoteRepositry* pQuoteRepo)
{
	if(m_pFetcher != NULL)
	{
		pQuoteRepo->DestoryFetcher(m_pFetcher);
		m_pFetcher = NULL;
	}
}

void CSymbolQuoting::OnQuoteRecevied(boost::chrono::steady_clock::time_point& timestamp, CQuote* pQuote)
{
	logger.info("In Quote: %s %.2f %s", 
		pQuote->symbol().c_str(), pQuote->last(), pQuote->update_time().c_str());
}
