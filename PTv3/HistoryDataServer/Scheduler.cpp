#include "stdafx.h"
#include "Scheduler.h"
#include "LogFactory.h"

#include <boost/chrono/chrono_io.hpp>
#include <boost/date_time/local_time_adjustor.hpp>

log4cpp::Category& CScheduler::logger = CLogFactory::GetInstance().GetLogger("Scheduler");

CScheduler::CScheduler()
	: io_service()
	, m_startTimer(io_service)
	, m_endTimer(io_service)
	, m_nextStartTimePoint(-1), m_nextEndTimePoint(-1)
{
}


CScheduler::~CScheduler()
{
}

void CScheduler::Run(const string& startTimpoints, const string& endTimepoints)
{
	// 1. parse time points from string (including sort)
	int tpStarts = ParseTimePoints(startTimpoints, m_startTimePoints);
	logger.infoStream() << "Find " << tpStarts << " Start time points." << log4cpp::eol;
	int tpEnds = ParseTimePoints(endTimepoints, m_endTimePoints);
	logger.infoStream() << "Find " << tpEnds << " End time points." << log4cpp::eol;
	
	logger.infoStream() << "system_clock::now" << boost::chrono::system_clock::now() << log4cpp::eol;
	
	boost::chrono::system_clock::time_point nextStartPoint = FindNextChronoTimePointFromNow(m_startTimePoints, &m_nextStartTimePoint);
	logger.infoStream() << "The next START time point: " << m_nextStartTimePoint << ". "
		<< boost::chrono::time_fmt(boost::chrono::timezone::local, "%F %T")
		<< nextStartPoint << log4cpp::eol;
	m_startTimer.expires_at(nextStartPoint);

	boost::chrono::system_clock::time_point nextEndPoint = FindNextChronoTimePointFromNow(m_endTimePoints, &m_nextEndTimePoint);
	logger.infoStream() << "The next END time point: " << m_nextEndTimePoint << ". "
		<< boost::chrono::time_fmt(boost::chrono::timezone::local, "%F %T")
		<< nextEndPoint << log4cpp::eol;
	m_endTimer.expires_at(nextEndPoint);

	m_startTimer.async_wait(boost::bind(&CScheduler::OnStartTimeOut, this, _1));
	m_endTimer.async_wait(boost::bind(&CScheduler::OnEndTimeOut, this, _1));

	io_service.run();
}

void CScheduler::OnStartTimeOut(const boost::system::error_code& e)
{
	logger.infoStream() << "Reach No." << m_nextStartTimePoint << " START time point" << log4cpp::eol;
	
	boost::chrono::system_clock::time_point nextStartPoint = FindNextChronoTimePointFromNowIndex(
		m_startTimePoints, m_nextStartTimePoint, &m_nextStartTimePoint);
	logger.infoStream() << "The next START time point: " << m_nextStartTimePoint << ". "
		<< boost::chrono::time_fmt(boost::chrono::timezone::local, "%F %T")
		<< nextStartPoint << log4cpp::eol;
	m_startTimer.expires_at(nextStartPoint);
	m_startTimer.async_wait(boost::bind(&CScheduler::OnStartTimeOut, this, _1));
}

void CScheduler::OnEndTimeOut(const boost::system::error_code& e)
{
	logger.infoStream() << "Reach No." << m_nextEndTimePoint << " END time point" << log4cpp::eol;

	boost::chrono::system_clock::time_point nextEndPoint = FindNextChronoTimePointFromNowIndex(
		m_endTimePoints, m_nextEndTimePoint, &m_nextEndTimePoint);
	logger.infoStream() << "The next END time point: " << m_nextEndTimePoint << ". "
		<< boost::chrono::time_fmt(boost::chrono::timezone::local, "%F %T")
		<< nextEndPoint << log4cpp::eol;
	m_endTimer.expires_at(nextEndPoint);
	m_endTimer.async_wait(boost::bind(&CScheduler::OnEndTimeOut, this, _1));
}

boost::chrono::system_clock::time_point CScheduler::FindNextChronoTimePointFromNow(vector<boost::posix_time::time_duration>& durationTimePoints, int* outNextIdx)
{
	// 2. check the latest time point behind now
	boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
	boost::posix_time::time_duration nowTime = now.time_of_day();
	int nextIdx = 0;
	for (vector<boost::posix_time::time_duration>::iterator iter = durationTimePoints.begin();
		iter != durationTimePoints.end(); ++iter, ++nextIdx)
	{
		if (*iter > nowTime)
			break;
	}

	if(nextIdx < durationTimePoints.size())
	{
		*outNextIdx = nextIdx;
		return FindNextChronoTimePointByIndex(durationTimePoints, nextIdx);
	}

	*outNextIdx = 0; // reset to nextIdx to the first one, and add 24 hours
	boost::chrono::system_clock::time_point nextPoint = FindNextChronoTimePointByIndex(durationTimePoints, 0);
	return nextPoint + boost::chrono::hours(24);
}

boost::chrono::system_clock::time_point CScheduler::FindNextChronoTimePointFromNowIndex(vector<boost::posix_time::time_duration>& durationTimePoints, int nowIndex, int* outNextIdx)
{
	int totalCount = durationTimePoints.size();
	*outNextIdx = nowIndex + 1;
	if (*outNextIdx < totalCount)
		return FindNextChronoTimePointByIndex(durationTimePoints, *outNextIdx);
	
	*outNextIdx = 0; // return to the first time point
	boost::chrono::system_clock::time_point nextPoint = FindNextChronoTimePointByIndex(durationTimePoints, 0);
	return nextPoint + boost::chrono::hours(24);
}

boost::chrono::system_clock::time_point CScheduler::FindNextChronoTimePointByIndex(vector<boost::posix_time::time_duration>& durationTimePoints, int nextIdx)
{
	// 3. construct ptime of next time point
	boost::posix_time::time_duration nextTimepointDur = durationTimePoints[nextIdx];
	boost::posix_time::ptime nextPTime = boost::posix_time::ptime(boost::gregorian::day_clock::local_day(), nextTimepointDur);
	boost::posix_time::ptime utcNextPTime =
		boost::date_time::local_adjustor<boost::posix_time::ptime, 8, boost::posix_time::no_dst>
			 ::local_to_utc(nextPTime);
	// 4. convert to chrono time_point type
	time_t nextTTime = boost::posix_time::to_time_t(utcNextPTime);
	boost::chrono::system_clock::time_point expireTimePoint = boost::chrono::system_clock::from_time_t(nextTTime);

	return expireTimePoint;
}
