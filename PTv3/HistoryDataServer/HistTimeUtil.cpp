#include "stdafx.h"
#include "HistTimeUtil.h"

#include <algorithm>    // std::sort

#include <boost/algorithm/string.hpp>
#include <boost/date_time.hpp>


boost::chrono::seconds ParseTimeString(const char* time)
{
	boost::chrono::seconds finalVal;
	unsigned int hour = 0, min = 0, sec = 0;
	int got = sscanf(time, "%u:%u:%u", &hour, &min, &sec);
	if (got == 3)
	{
		finalVal = boost::chrono::hours(hour)
			+ boost::chrono::minutes(min)
			+ boost::chrono::seconds(sec);
	}
	// else must be something wrong
	return finalVal;
}

boost::chrono::seconds ParseTimeString(const string& time)
{
	return ParseTimeString(time.c_str());
}

char TIME_POINT_DELIMITER[] = ",";

int ParseTimePoints(const string& timePointExpr, vector<boost::posix_time::time_duration>& outTimePoints)
{
	vector<string> timeStrings;
	boost::split(timeStrings, timePointExpr, boost::is_any_of(TIME_POINT_DELIMITER));
	for (vector<string>::iterator iter = timeStrings.begin(); iter != timeStrings.end(); ++iter)
	{
		boost::trim(*iter);
		boost::posix_time::time_duration dur = boost::posix_time::duration_from_string(*iter);
		outTimePoints.push_back(dur);
	}

	if (!outTimePoints.empty())
		sort(outTimePoints.begin(), outTimePoints.end());

	return outTimePoints.size();
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
