#pragma once

#include "HistTimeUtil.h"

#include <boost/asio/system_timer.hpp>

typedef boost::function<void(int)> TimeOutHandlerFunc;
typedef boost::function<void(bool, int)> BeforeWaitingHandlerFunc;

class CScheduler
{
public:
	CScheduler();
	~CScheduler();

	void Run(const string& startTimpoints, const string& endTimepoints);
	void SetStartTimeOutHandler(TimeOutHandlerFunc& handler) { m_onStartTimeOutHandler = handler; }
	void SetEndTimeoutHandler(TimeOutHandlerFunc& handler) { m_onEndTimeOutHandler = handler; }
	void SetBeforeWaitingHandler(BeforeWaitingHandlerFunc& handler) { m_onBeforeWaitingHandler = handler; }

	int NextStartTimePoint() const { return m_nextStartTimePoint; }
	int NextEndTimePoint() const { return m_nextEndTimePoint; }

private:
	void OnStartTimeOut(const boost::system::error_code& e);
	void OnEndTimeOut(const boost::system::error_code& e);

	static bool NowDuringTimePeriod(int startTimePoint, int endTimePoint);

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
	BeforeWaitingHandlerFunc m_onBeforeWaitingHandler;
};

