#include "stdafx.h"
#include "HistPriceBarGen.h"
#include "LogFactory.h"
#include "HistTimeUtil.h"
#include "Quote.h"

#include <boost/algorithm/string.hpp>


log4cpp::Category& CHistPriceBarGen::logger = CLogFactory::GetInstance().GetLogger("HistPriceBarGen");

CHistPriceBarGen::CHistPriceBarGen()
	: m_precision(0)
	, m_barCount(0)
	, m_currentIdx(-1)
	, m_open(0)
	, m_high(0)
	, m_low(0)
	, m_close(0)
	, m_currentTimeSpan(NULL)
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

	vector<string> marketSectionStartVec, marketSectionEndVec;
	int sectionCount = GetMarketSectionTimePoints(symbol, marketSectionStartVec, marketSectionEndVec);

	for (int i = 0; i < sectionCount; ++i)
	{
		TradingTimeSpanPtr span(new CHistTradingTimeSpan(
			marketSectionStartVec[i].c_str(), marketSectionEndVec[i].c_str(), precision));
		m_vecTimeSpan.push_back(span);

		if(i > 0)
		{
			span->SetOffset(m_vecTimeSpan[i - 1]->EndIndex());
		}
	}

	m_barCount = m_vecTimeSpan[sectionCount - 1]->EndIndex();
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

			if (m_currentTimeSpan != NULL)
			{
				// In case the last bar of current span
				if (barIdx == m_currentTimeSpan->EndIndex() - 1 && m_currentTimeSpan->TrueEnd())
				{
					boost::thread(&CHistPriceBarGen::TriggerFinalizeLastBar, this);
				}
			}
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
		logger.warn("Encounter weird quote Idx %d (%s,%d %.2f). Ignored", 
			barIdx, pQuote->update_time().c_str(), pQuote->update_millisec(), pQuote->last());
	}
}

int CHistPriceBarGen::GetIndex(const string& quoteTime, string* timestamp)
{
	boost::chrono::seconds quoteTimePoint = ParseTimeString(quoteTime);
#ifndef _DEBUG
	boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
	int nowTimeSeconds = now.time_of_day().total_seconds();
	if (abs(quoteTimePoint.count() - nowTimeSeconds) > 3600)
		return -1;	// Ignore Quote once timestamp is more than 1 hour away from now time.
#endif
	if (m_currentTimeSpan == NULL || !m_currentTimeSpan->InScope(quoteTimePoint))
	{
		for (TimeSpanVecIter iter = m_vecTimeSpan.begin();
			iter != m_vecTimeSpan.end(); ++iter)
		{
			if((*iter)->InScope(quoteTimePoint))
			{
				m_currentTimeSpan = (*iter).get();
				break;
			}
		}
	}

	if (m_currentTimeSpan != NULL)
	{
		return m_currentTimeSpan->GetIndex(quoteTimePoint, timestamp);
	}

	// input quote time is NOT in valid trading time
	return -1;
}

int CHistPriceBarGen::GetIndex(const string& quoteTime)
{
	boost::chrono::seconds quoteTimePoint = ParseTimeString(quoteTime);

	if(m_currentTimeSpan == NULL || !m_currentTimeSpan->InScope(quoteTimePoint))
	{
		for (TimeSpanVecIter iter = m_vecTimeSpan.begin();
			iter != m_vecTimeSpan.end(); ++iter)
		{
			if ((*iter)->InScope(quoteTimePoint))
			{
				m_currentTimeSpan = (*iter).get();
				break;
			}
		}
	}

	if(m_currentTimeSpan != NULL)
	{
		return m_currentTimeSpan->GetIndex(quoteTimePoint);
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

void CHistPriceBarGen::TriggerFinalizeLastBar() const
{
	int secondsToWait = m_precision + 60;
	logger.info("Wait %d seconds to finialize the last bar", secondsToWait);
	// Sleep for one more 1 mintues
	boost::this_thread::sleep_for(boost::chrono::seconds(secondsToWait));
	logger.info("Trigger to finialize the last bar.");
	RaiseBarFinalizedEvent();
}
