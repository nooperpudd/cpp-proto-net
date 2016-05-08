#pragma once

#include "HistTimeUtil.h"

#include <boost/asio/system_timer.hpp>

typedef boost::function<void(int)> TimeOutHandlerFunc;

class CScheduler
{
public:
	CScheduler();
	~CScheduler();

	void Run(const string& startTimpoints, const string& endTimepoints);
	void SetStartTimeOutHandler(TimeOutHandlerFunc& handler) { m_onStartTimeOutHandler = handler; }
	void SetEndTimeoutHandler(TimeOutHandlerFunc& handler) { m_onEndTimeOutHandler = handler; }

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

	TimeOutHandlerFunc m_onStartTimeOutHandler;
	TimeOutHandlerFunc m_onEndTimeOutHandler;
};

