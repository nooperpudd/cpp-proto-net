#include "stdafx.h"
#include "HistTradingTimeSpan.h"
#include "HistTimeUtil.h"

#include <boost/algorithm/string.hpp>

#define IF_TOTAL_TRADING_SECONDS (4 * 60 * 60 + 30 * 60)
#define NON_IF_TOTAL_TRADING_SECONDS (3 * 60 * 60 + 45 * 60)

#define IF_START_1 "09:30:00"
#define IF_END_1 "11:30:00"
#define IF_START_2 "13:00:00"
#define IF_END_2 "15:00:00"

#define COMMOD_START_1 "21:00:00"
#define COMMOD_END_1 "24:00:00"
#define COMMOD_START_2 "00:00:00"
#define COMMOD_END_2 "01:00:00"
#define COMMOD_START_3 "09:00:00"
#define COMMOD_END_3 "10:15:00"
#define COMMOD_START_4 "10:30:00"
#define COMMOD_END_4 "11:30:00"
#define COMMOD_START_5 "13:30:00"
#define COMMOD_END_5 "15:00:00"

#define IF_PREFIX "IF"
#define IC_PREFIX "IC"
#define IH_PREFIX "IH"


bool isSymbolIF(const string& symbol)
{
	return boost::starts_with(symbol, IF_PREFIX) ||
		boost::starts_with(symbol, IC_PREFIX) ||
		boost::starts_with(symbol, IH_PREFIX);
}

CHistTradingTimeSpan::CHistTradingTimeSpan(const char* timeBegin, const char* timeEnd, int precision) 
	: m_offset(0), m_precision(precision), m_trueEnd(true)
{
	m_Start = ParseTimeString(timeBegin);
	m_End = ParseTimeString(timeEnd);

	m_endIndex = GetIndexFromTime(m_Start, m_End, precision, false);

	if (m_End == DayHours)
		m_trueEnd = false;
}

int CHistTradingTimeSpan::GetIndex(const boost::chrono::seconds& timePoint) const
{
	if (timePoint <= m_End)
		return GetIndexFromTime(m_Start, timePoint, m_precision) + m_offset;
	else
		return EndIndex() - 1;
}

boost::chrono::hours CHistTradingTimeSpan::DayHours(24);

int CHistTradingTimeSpan::GetIndex(const boost::chrono::seconds& timePoint, string* outTimestamp) const
{
	if (timePoint <= m_End)
	{
		int idx = GetIndexFromTime(m_Start, timePoint, m_precision);
		boost::chrono::seconds timestampSeconds = m_Start + boost::chrono::seconds((idx + 1) * m_precision);
		if (timestampSeconds >= DayHours)
			timestampSeconds -= DayHours;
		*outTimestamp = GetISOTimeString(timestampSeconds);
		return  idx + m_offset;
	}
	else
	{
		*outTimestamp = GetISOTimeString(m_End);
		return EndIndex();
	}
}

int CHistTradingTimeSpan::GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, int precision, bool advance1s)
{
	boost::chrono::seconds diff = timePoint - baseTp;
	if (diff > boost::chrono::seconds::zero())
	{
		if (advance1s)
			diff -= boost::chrono::seconds(1);
		int idx = diff.count() / precision;
		return idx;
	}

	return 0;
}

int GetMarketSectionTimePoints(const string& symbol, vector<string>& startTimePoints, vector<string>& endTimePoints)
{
	if(isSymbolIF(symbol))
	{
		startTimePoints.push_back(IF_START_1);
		startTimePoints.push_back(IF_START_2);

		endTimePoints.push_back(IF_END_1);
		endTimePoints.push_back(IF_END_2);
	}
	else if(boost::starts_with(symbol, "rb")) // rb1610
	{
		startTimePoints.push_back(COMMOD_START_1);
		startTimePoints.push_back(COMMOD_START_3);
		startTimePoints.push_back(COMMOD_START_4);
		startTimePoints.push_back(COMMOD_START_5);

		endTimePoints.push_back("23:00:00");
		endTimePoints.push_back(COMMOD_END_3);
		endTimePoints.push_back(COMMOD_END_4);
		endTimePoints.push_back(COMMOD_END_5);
	}
	else if(boost::starts_with(symbol, "ag"))
	{
		startTimePoints.push_back(COMMOD_START_1);
		startTimePoints.push_back(COMMOD_START_2);
		startTimePoints.push_back(COMMOD_START_3);
		startTimePoints.push_back(COMMOD_START_4);
		startTimePoints.push_back(COMMOD_START_5);

		endTimePoints.push_back(COMMOD_END_1);
		endTimePoints.push_back("2:30:00");
		endTimePoints.push_back(COMMOD_END_3);
		endTimePoints.push_back(COMMOD_END_4);
		endTimePoints.push_back(COMMOD_END_5);
	}
	else
	{
		startTimePoints.push_back(COMMOD_START_1);
		startTimePoints.push_back(COMMOD_START_2);
		startTimePoints.push_back(COMMOD_START_3);
		startTimePoints.push_back(COMMOD_START_4);
		startTimePoints.push_back(COMMOD_START_5);

		endTimePoints.push_back(COMMOD_END_1);
		endTimePoints.push_back(COMMOD_END_2);
		endTimePoints.push_back(COMMOD_END_3);
		endTimePoints.push_back(COMMOD_END_4);
		endTimePoints.push_back(COMMOD_END_5);
	}

	return startTimePoints.size();
}
