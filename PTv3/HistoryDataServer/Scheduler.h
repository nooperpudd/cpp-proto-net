#pragma once

#include "HistTimeUtil.h"

#include <boost/asio/system_timer.hpp>

class CScheduler
{
public:
	CScheduler();
	~CScheduler();

	void Run(const string& startTimpoints, const string& endTimepoints);

private:
	void OnStartTimeOut(const boost::system::error_code& e);
	void OnEndTimeOut(const boost::system::error_code& e);

	static boost::chrono::system_clock::time_point FindNextChronoTimePointFromNow(vector<boost::posix_time::time_duration>& durationTimePoints, int* outNextIdx);
	static boost::chrono::system_clock::time_point FindNextChronoTimePointFromNowIndex(vector<boost::posix_time::time_duration>& durationTimePoints, int nowIndex, int* outNextIdx);
	static boost::chrono::system_clock::time_point FindNextChronoTimePointByIndex(vector<boost::posix_time::time_duration>& durationTimePoints, int nextIdx);

	static log4cpp::Category& logger;

	boost::asio::io_service io_service;
	boost::asio::system_timer m_startTimer;
	boost::asio::system_timer m_endTimer;

	vector<boost::posix_time::time_duration> m_startTimePoints;
	int m_nextStartTimePoint;
	vector<boost::posix_time::time_duration> m_endTimePoints;
	int m_nextEndTimePoint;
};

