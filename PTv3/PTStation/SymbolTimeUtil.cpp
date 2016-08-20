#include "stdafx.h"
#include "SymbolTimeUtil.h"
#include "TechStrategyDefs.h"

#include <cstdio>
#include <boost/algorithm/string.hpp>


boost::chrono::seconds ParseTimeString(const char* time)
{
	boost::chrono::seconds finalVal;
	unsigned int hour = 0, min = 0, sec = 0;
	int got = sscanf(time, "%u:%u:%u", &hour, &min, &sec);
	if(got == 3)
	{
		finalVal = boost::chrono::hours(hour)
			+ boost::chrono::minutes(min)
			+ boost::chrono::seconds(sec);
	}
	// else must be something wrong
	return finalVal;
}

bool ParseTimeString(const char* time, unsigned int* hour, unsigned int* min, unsigned int* sec)
{
	*hour = 0;
	*min = 0;
	*sec = 0;
	int got = sscanf(time, "%u:%u:%u", hour, min, sec);
	if(got == 3)
	{
		return true;
	}
	// else must be something wrong
	return false;
}

boost::chrono::seconds ParseTimeString(const string& time)
{
	return ParseTimeString(time.c_str());
}

bool ParseTimeString(const string& time, unsigned int* hour, unsigned int* min, unsigned int* sec)
{
	return ParseTimeString(time.c_str(), hour, min, sec);
}

bool isSymbolIF(const string& symbol)
{
	return boost::starts_with(symbol, IF_PREFIX) ||
		boost::starts_with(symbol, IC_PREFIX) ||
		boost::starts_with(symbol, IH_PREFIX);
}

// returned index is 0 based
int GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, int precision)
{
	boost::chrono::seconds diff = timePoint - baseTp;
	if(diff > boost::chrono::seconds::zero())
	{
		int idx = diff.count() / precision;
		return idx;
	}

	return 0;
}

string GetISOTimeString(const boost::chrono::seconds& timepoint)
{
	boost::chrono::seconds s = timepoint;
	// separate minutes from seconds
	boost::chrono::minutes m = boost::chrono::duration_cast<boost::chrono::minutes>(s);
	s -= m;
	// separate hours from minutes
	boost::chrono::hours h = boost::chrono::duration_cast<boost::chrono::hours>(m);
	m -= h;
	return boost::str(boost::format("%02d:%02d:%02d") % h.count() % m.count() % s.count());
}

void GetStandardTimeString(const string& srcTime, string& targetTime)
{
	boost::chrono::seconds sec = ParseTimeString(srcTime);
	targetTime = GetISOTimeString(sec);
}
