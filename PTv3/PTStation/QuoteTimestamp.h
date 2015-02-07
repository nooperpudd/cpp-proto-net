#pragma once

#include <boost/unordered_map.hpp>
#include <boost/chrono.hpp>

class CQuoteTimestamp
{
public:
	CQuoteTimestamp(const char* updateTime, int milliseconds)
		: UpdateTime(updateTime), MilliSeconds(milliseconds)
	{}

	std::string UpdateTime;
	int	MilliSeconds;
	boost::chrono::steady_clock::time_point LastTime;
};

typedef boost::shared_ptr<CQuoteTimestamp> TimestampPtr;
typedef boost::unordered_map<std::string, TimestampPtr> QuoteTimestampMap;
typedef QuoteTimestampMap::iterator QuoteTimestampMapIter;