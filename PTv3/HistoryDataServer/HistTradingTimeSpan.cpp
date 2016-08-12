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

CHistTradingTimeSpan::CHistTradingTimeSpan(const char* timeBegin, const char* timeEnd, bool trueEnd) 
	: m_trueEnd(trueEnd)
{
	m_Start = ParseTimeString(timeBegin);
	m_End = ParseTimeString(timeEnd);
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

boost::chrono::hours CHistTimeRange::DayHours(24);

void CHistTimeRange::BuildTimeStamp(boost::chrono::seconds timeSeconds)
{
	if (timeSeconds >= DayHours)
		timeSeconds -= DayHours;
	m_timestamp = GetISOTimeString(timeSeconds);
}

int GetMarketSectionTimePoints(const string& symbol, TimeSpanVec& timeSpanVec)
{
	if(isSymbolIF(symbol))
	{
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(IF_START_1, IF_END_1, false));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(IF_START_2, IF_END_2, true));
	}
	else if(boost::starts_with(symbol, "rb")) // rb1610
	{
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_1, "23:00:00", true));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_3, COMMOD_END_3, false));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_4, COMMOD_END_4, false));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_5, COMMOD_END_5, true));
	}
	else if(boost::starts_with(symbol, "ag"))
	{
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_1, COMMOD_END_1, false));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_2, "2:30:00", true));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_3, COMMOD_END_3, false));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_4, COMMOD_END_4, false));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_5, COMMOD_END_5, true));
	}
	else
	{
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_1, COMMOD_END_1, false));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_2, COMMOD_END_2, true));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_3, COMMOD_END_3, false));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_4, COMMOD_END_4, false));
		timeSpanVec.push_back(boost::make_shared<CHistTradingTimeSpan>(COMMOD_START_5, COMMOD_END_5, true));
	}

	return timeSpanVec.size();
}
