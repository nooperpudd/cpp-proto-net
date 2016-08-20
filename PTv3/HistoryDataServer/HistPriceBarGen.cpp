#include "stdafx.h"
#include "HistPriceBarGen.h"
#include "LogFactory.h"
#include "HistTimeUtil.h"
#include "Quote.h"

log4cpp::Category& CHistPriceBarGen::logger = CLogFactory::GetInstance().GetLogger("HistPriceBarGen");

CHistPriceBarGen::CHistPriceBarGen()
	: m_precision(0)
	, m_barCount(0)
	, m_currentIdx(-1)
	, m_open(0)
	, m_high(0)
	, m_low(0)
	, m_close(0)
	, m_currentTimeRange(NULL)
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
	int sectionCount = GetMarketSectionTimePoints(symbol, m_vecTimeSpan);
	logger.infoStream() << symbol << " has " << sectionCount << " timespans." << log4cpp::eol;
	m_barCount = PopulateTimeRange(precision);
	logger.info("%s has %d bars for precision %d seconds", symbol.c_str(), m_barCount, precision);
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

			if (m_currentTimeRange != NULL && m_currentTimeRange->NotifyEnd())
			{
				// In case the last bar of current span
				boost::thread(&CHistPriceBarGen::TriggerFinalizeLastBar, this);
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

	if (m_currentTimeRange == NULL || !m_currentTimeRange->InScope(quoteTimePoint))
	{
		for (TimeRangeVecIter iter = m_vecTimeRanges.begin();
			iter != m_vecTimeRanges.end(); ++iter)
		{
			if((*iter)->InScope(quoteTimePoint))
			{
				m_currentTimeRange = (*iter).get();
				break;
			}
		}
	}

	if (m_currentTimeRange != NULL)
	{
		*timestamp = m_currentTimeRange->Timestamp();
		return m_currentTimeRange->Index();
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

int CHistPriceBarGen::PopulateTimeRange(int precision)
{
	boost::chrono::seconds step(precision);
	int index = 0;
	CHistTimeRange* crossingRange = NULL;
	for (TimeSpanVecIter iter = m_vecTimeSpan.begin();
		iter != m_vecTimeSpan.end(); ++iter)
	{
		boost::chrono::seconds pos = (*iter)->Start();
		boost::chrono::seconds spanEnd = (*iter)->End();

		if(crossingRange != NULL)
		{
			boost::chrono::seconds gap = step - (crossingRange->End() - crossingRange->Begin());
			if(pos + gap <= spanEnd) // should be the most cases
			{
				boost::chrono::seconds rangeEnd = pos + gap;
				crossingRange->SetEnd(rangeEnd);
				pos = rangeEnd;
				// than clear crossingRange
				crossingRange = NULL;
			}
			else // occasionally be this case -- pos + gap > spanEnd
			{
				crossingRange->SetEnd(spanEnd);
				continue;
			}
		}

		while(pos < spanEnd)
		{
			TimeRangePtr range;
			if(pos + step <= spanEnd)
			{
				boost::chrono::seconds rangeEnd = pos + step;
				range = boost::make_shared<CHistTimeRange>(index, pos, rangeEnd);
				
				if(rangeEnd == spanEnd)
				{
					range->SetSpanEnd(true);
				}
			}
			else // pos + step > spanEnd
			{
				range = boost::make_shared<CHistTimeRange>(index, pos, spanEnd);
				crossingRange = range.get();
			}
			m_vecTimeRanges.push_back(range);
			pos = range->End();
			++index;
		}

		if((*iter)->TrueEnd())
		{
			TimeRangePtr& priorRange = m_vecTimeRanges.at(index - 1);
			priorRange->SetNotifyEnd(true);
		}
	}

	if (crossingRange != NULL)
	{
		crossingRange->SetSpanEnd(true);
	}

	return m_vecTimeRanges.size();
}
