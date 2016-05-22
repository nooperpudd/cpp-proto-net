#include "stdafx.h"
#include "HistPriceBarGen.h"
#include "LogFactory.h"
#include "HistTimeUtil.h"
#include "Quote.h"

#include <boost/algorithm/string.hpp>

#define IF_TOTAL_TRADING_SECONDS (4 * 60 * 60 + 30 * 60)
#define NON_IF_TOTAL_TRADING_SECONDS (3 * 60 * 60 + 45 * 60)

#define IF_START_1 "09:15:00"
#define IF_END_1 "11:30:00"
#define IF_START_2 "13:00:00"
#define IF_END_2 "15:15:00"

#define NON_IF_START_1 "09:00:00"
#define NON_IF_END_1 "10:15:00"
#define NON_IF_START_2 "10:30:00"
#define NON_IF_END_2 "11:30:00"
#define NON_IF_START_3 "13:30:00"
#define NON_IF_END_3 "15:00:00"

#define IF_PREFIX "IF"
#define IC_PREFIX "IC"
#define IH_PREFIX "IH"


log4cpp::Category& CHistPriceBarGen::logger = CLogFactory::GetInstance().GetLogger("HistDataPersistence");

bool isSymbolIF(const string& symbol)
{
	return boost::starts_with(symbol, IF_PREFIX) ||
		boost::starts_with(symbol, IC_PREFIX) ||
		boost::starts_with(symbol, IH_PREFIX);
}

CHistTradingTimeSpan::CHistTradingTimeSpan(const char* timeBegin, const char* timeEnd, int precision) 
	: m_offset(0), m_precision(precision)
{
	m_Start = ParseTimeString(timeBegin);
	m_End = ParseTimeString(timeEnd);

	m_endIndex = GetIndexFromTime(m_Start, m_End, precision);
}

int CHistTradingTimeSpan::GetIndex(const boost::chrono::seconds& timePoint) const
{
	if (timePoint < m_End)
		return GetIndexFromTime(m_Start, timePoint, m_precision) + m_offset;
	else
		return EndIndex() - 1;
}

int CHistTradingTimeSpan::GetIndex(const boost::chrono::seconds& timePoint, string* outTimestamp) const
{
	if (timePoint < m_End)
	{
		int idx = GetIndexFromTime(m_Start, timePoint, m_precision);
		*outTimestamp = GetISOTimeString(m_Start + boost::chrono::seconds(idx * m_precision));
		return  idx + m_offset;
	}
	else
	{
		*outTimestamp = GetISOTimeString(m_End);
		return EndIndex();
	}
}

int CHistTradingTimeSpan::GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, int precision)
{
	boost::chrono::seconds diff = timePoint - baseTp;
	if (diff > boost::chrono::seconds::zero())
	{
		int idx = diff.count() / precision;
		return idx;
	}

	return 0;
}

CHistPriceBarGen::CHistPriceBarGen()
	: m_precision(0)
	, m_barCount(0)
	, m_currentIdx(-1)
	, m_open(0)
	, m_high(0)
	, m_low(0)
	, m_close(0)
{
}


CHistPriceBarGen::~CHistPriceBarGen()
{
	// Write the last quote to file
	RaiseBarFinalizedEvent();
}


void CHistPriceBarGen::Init(const string& symbol, int precision)
{
	m_symbol = symbol;
	m_precision = precision;

	bool isIF = isSymbolIF(symbol);
	if (isIF)
	{
		TradingTimeSpanPtr if_span_1(new CHistTradingTimeSpan(IF_START_1, IF_END_1, precision));
		m_vecTimeSpan.push_back(if_span_1);
		TradingTimeSpanPtr if_span_2(new CHistTradingTimeSpan(IF_START_2, IF_END_2, precision));
		m_vecTimeSpan.push_back(if_span_2);
		if_span_2->SetOffset(if_span_1->EndIndex());
		m_barCount = if_span_2->EndIndex();
	}
	else
	{
		TradingTimeSpanPtr non_if_span_1(new CHistTradingTimeSpan(NON_IF_START_1, NON_IF_END_1, precision));
		m_vecTimeSpan.push_back(non_if_span_1);
		TradingTimeSpanPtr non_if_span_2(new CHistTradingTimeSpan(NON_IF_START_2, NON_IF_END_2, precision));
		m_vecTimeSpan.push_back(non_if_span_2);
		non_if_span_2->SetOffset(non_if_span_1->EndIndex());
		TradingTimeSpanPtr non_if_span_3(new CHistTradingTimeSpan(NON_IF_START_3, NON_IF_END_3, precision));
		m_vecTimeSpan.push_back(non_if_span_3);
		non_if_span_3->SetOffset(non_if_span_2->EndIndex());
		m_barCount = non_if_span_3->EndIndex();
	}
}

void CHistPriceBarGen::Calculate(CQuote* pQuote)
{
	string timestamp;
	int barIdx = GetIndex(pQuote->update_time(), &timestamp);
	if (barIdx < m_barCount && barIdx > -1)
	{
		double last = pQuote->last();
		if (barIdx > m_currentIdx)
		{
			// if not the first one, finalize the last bar
			RaiseBarFinalizedEvent();

			m_currentIdx = barIdx;
			m_currentTimestamp = timestamp;
			m_close = m_low = m_high = m_open = last;
		}
		else if (barIdx == m_currentIdx)
		{
			m_close = last;
			if (last > m_high)
				m_high = last;
			if (last < m_low)
				m_low = last;
		}
		else
		{	// barIdx < m_currentIdx ???
			logger.error(boost::str(boost::format("!!! %s -> barIdx(%d) < m_currentIdx(%d) !!!") % pQuote->update_time() % barIdx % m_currentIdx));

			//assert(barIdx > m_currentIdx);
			return;	// ignore the quote with invalid calculated index
		}

		RaiseBarChangeEvent(barIdx, timestamp);
	}
	else if (barIdx >= m_barCount)
	{
		if (barIdx > m_currentIdx)
		{
			m_close = pQuote->last();
			RaiseBarFinalizedEvent();
			m_currentIdx = barIdx;
		}
	}
}

int CHistPriceBarGen::GetIndex(const string& quoteTime, string* timestamp)
{
	boost::chrono::seconds quoteTimePoint = ParseTimeString(quoteTime);

	for (TimeSpanVecIter iter = m_vecTimeSpan.begin();
	iter != m_vecTimeSpan.end(); ++iter)
	{
		if ((*iter)->InScope(quoteTimePoint))
		{
			return (*iter)->GetIndex(quoteTimePoint, timestamp);
		}
	}

	// input quote time is NOT in valid trading time
	return -1;
}

int CHistPriceBarGen::GetIndex(const string& quoteTime)
{
	boost::chrono::seconds quoteTimePoint = ParseTimeString(quoteTime);

	for (TimeSpanVecIter iter = m_vecTimeSpan.begin();
	iter != m_vecTimeSpan.end(); ++iter)
	{
		if ((*iter)->InScope(quoteTimePoint))
		{
			return (*iter)->GetIndex(quoteTimePoint);
		}
	}

	// input quote time is NOT in valid trading time
	return -1;
}

void CHistPriceBarGen::RaiseBarFinalizedEvent() const
{
	if (m_currentIdx >= 0 && !m_onBarFinalized.empty())
	{
		m_onBarFinalized(m_currentIdx, m_open, m_high, m_low, m_close, m_currentTimestamp);
	}
}
