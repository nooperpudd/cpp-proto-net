#include "stdafx.h"
#include "SymbolQuoting.h"
#include "HistQuoteRepositry.h"
#include "HistQuoteFetcher.h"
#include "LogFactory.h"

#include <boost/algorithm/string.hpp>

log4cpp::Category& CSymbolQuoting::logger = CLogFactory::GetInstance().GetLogger("SymbolQuoting");

CSymbolQuoting::CSymbolQuoting(const string& symbol, const string& timeFrame)
	: m_symbol(symbol)
	, m_timeFrame(timeFrame)
	, m_precision(0)
	, m_pFetcher(NULL)
{
	m_precision = ParseTimeFrame(m_timeFrame);
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

void CSymbolQuoting::Init(const boost::gregorian::date& tradingDay)
{
	m_priceBarGen.Init(m_symbol, m_precision);
	m_priceBarGen.SetBarFinalizedHandler(boost::bind(&CSymbolQuoting::OnBarFinalized, this, _1, _2, _3, _4, _5, _6));

	m_histDataWriter.Init(m_symbol, m_precision, tradingDay);
	bool writerReady = m_histDataWriter.Open();
	if (!writerReady)
	{
		logger.error(boost::str(boost::format("Cannot open HistDataWriter for %s-%u") % m_symbol % m_precision));
	}
}

void CSymbolQuoting::OnQuoteRecevied(boost::chrono::steady_clock::time_point& timestamp, CQuote* pQuote)
{
	logger.info("In Quote: %s %.2f %s", 
		pQuote->symbol().c_str(), pQuote->last(), pQuote->update_time().c_str());

	m_priceBarGen.Calculate(pQuote);
}

void CSymbolQuoting::OnBarFinalized(int barIdx, double open, double high, double low, double close, const string& timestamp)
{
	logger.info("%s - barIdx: %d, close: %.2f, timestatmp: %s", m_symbol.c_str(), barIdx, close, timestamp.c_str());
	m_histDataWriter.Write(timestamp, open, high, low, close);
}

int CSymbolQuoting::ParseTimeFrame(const string& timeFrame)
{
	string digit_part = boost::trim_copy_if(timeFrame, boost::is_alpha());
	string alpha_part = boost::trim_left_copy_if(timeFrame, boost::is_digit());

	int digit = boost::lexical_cast<int>(digit_part);

	if(alpha_part == "m" || alpha_part == "M")
	{
		return digit * 60;
	}
	
	if(alpha_part == "h" || alpha_part == "H")
	{
		return digit * 60 * 60;
	}

	return digit;
}
