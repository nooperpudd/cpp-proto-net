#include "StdAfx.h"
#include "OHLCRecordSet.h"
#include "TechStrategyDefs.h"
#include "globalmembers.h"

#include <boost/algorithm/string.hpp>

void COHLCRecordSet::ResetArray(double arr[], int length)
{
	for(int i = 0; i < length; ++i)
	{
		arr[i] = 0;
	}
}

COHLCRecordSet::COHLCRecordSet(const string& symbol, int precision, HISTORY_DATA_MODE histDataMode, bool weightAvg)
	: m_symbol(symbol)
	, m_precision(precision)
	, m_histDataMode(histDataMode)
	, m_weightAvg(weightAvg)
	, m_nbElements(0)
	, m_countInBar(0)
	, m_totalInBar(0.0)
{
	bool isIF = boost::starts_with(symbol, "IF") || 
				boost::starts_with(symbol, "IC") || 
				boost::starts_with(symbol, "IH");
	int tradingTime = isIF ? IF_TOTAL_TRADING_SECONDS : NON_IF_TOTAL_TRADING_SECONDS ;
	assert(precision != 0);
	int countPerDay = tradingTime / precision;
	
	switch(histDataMode)
	{
	case NONE:
		m_historyDataSize = 0;
		break;
	case ONE_BAR:
		m_historyDataSize = 1;
		break;
	case ONE_DAY:
		m_historyDataSize = countPerDay;
		break;
	}
	m_totalCount = countPerDay + m_historyDataSize;

	OpenSeries = boost::shared_array<double>(new double[m_totalCount]);
	ResetArray(OpenSeries.get(), m_totalCount);
	HighSeries = boost::shared_array<double>(new double[m_totalCount]);
	ResetArray(HighSeries.get(), m_totalCount);
	LowSeries = boost::shared_array<double>(new double[m_totalCount]);
	ResetArray(LowSeries.get(), m_totalCount);
	CloseSeries = boost::shared_array<double>(new double[m_totalCount]);
	ResetArray(CloseSeries.get(), m_totalCount);

	if(m_weightAvg)
	{
		WeightAvgSeries = boost::shared_array<double>(new double[m_totalCount]);
		ResetArray(WeightAvgSeries.get(), m_totalCount);
	}

	m_endIndex = m_historyDataSize - 1;
}


COHLCRecordSet::~COHLCRecordSet(void)
{
}

// barIdx is 0 based
void COHLCRecordSet::SetToday(int barIdx, double open, double high, double low, double close, const string& timestamp)
{
	int settingIdx = m_historyDataSize + barIdx;
	if(settingIdx > m_endIndex)
	{
		// if input bar index is NOT continuous, reset nbElements to 0 
		if(settingIdx != m_endIndex + 1)
			m_nbElements = 0;

		m_endIndex = settingIdx;
#ifdef TEST_TODAY_HIST
		if(m_nbElements < 0)
			m_nbElements = barIdx + 1;
		else
#endif // TEST_TODAY_HIST
		++m_nbElements;

		if(m_weightAvg)
		{
			if(m_nbElements > 1)
			{
				m_countInBar = 1;
				m_totalInBar = WeightAvgSeries[settingIdx - 1];
			}
			else
			{
				m_countInBar = 0;
				m_totalInBar = 0.0;
			}
		}
	}

	// protect from out of array boundary
	if(settingIdx < m_totalCount)
	{
		OpenSeries[settingIdx] = open;
		HighSeries[settingIdx] = high;
		LowSeries[settingIdx] = low;
		CloseSeries[settingIdx] = close;

		if(m_weightAvg)
		{
			m_totalInBar += close;
			WeightAvgSeries[settingIdx] = m_totalInBar / ++m_countInBar;
		}
	}

#ifdef LOG_FOR_TECH_CALC
	logger.Info(boost::str(boost::format("Setting Today %s [%s]: last Index - %d, last close - %.2f")
		% m_symbol % timestamp % m_endIndex % close));
#endif
}

void COHLCRecordSet::SetHistory( int barIdx, double open, double high, double low, double close )
{
	OpenSeries[barIdx] = open;
	HighSeries[barIdx] = high;
	LowSeries[barIdx] = low;
	CloseSeries[barIdx] = close;

	// make sure m_nbElements not larger than m_historyDataSize
	if (barIdx == m_nbElements)
		++m_nbElements;

#ifdef LOG_FOR_TECH_CALC
	logger.Info(boost::str(boost::format("Setting History: bar Index - %d, close - %f")
		% barIdx % close));
#endif
}
