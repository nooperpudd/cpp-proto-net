#pragma once

class CQuote;

class CHistTradingTimeSpan
{
public:
	CHistTradingTimeSpan(const char* timeBegin, const char* timeEnd, bool trueEnd = false);

	const boost::chrono::seconds& Start() const { return m_Start; }
	const boost::chrono::seconds& End() const { return m_End; }

	bool TrueEnd() const { return m_trueEnd; }
	void SetTrueEnd(bool trueEnd) { m_trueEnd = trueEnd; }

	bool InScope (const boost::chrono::seconds& timePoint) const
	{
		return timePoint >= m_Start && timePoint <= m_End;
	}

	static int GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, int precision);
	 
private:

	boost::chrono::seconds m_Start;
	boost::chrono::seconds m_End;

	bool m_trueEnd;
};

typedef boost::shared_ptr<CHistTradingTimeSpan> TradingTimeSpanPtr;
typedef vector<TradingTimeSpanPtr> TimeSpanVec;
typedef TimeSpanVec::iterator TimeSpanVecIter;

class CHistTimeRange
{
public:
	CHistTimeRange(int idx, boost::chrono::seconds begin, boost::chrono::seconds end)
		: m_index(idx), m_begin(begin), m_end(end), m_spanEnd(false), m_notifyEnd(false)
	{
		BuildTimeStamp(end);
	}

	int Index() const { return m_index; }
	const boost::chrono::seconds& Begin() const { return m_begin; }
	const boost::chrono::seconds& End() const { return m_end; }
	const string& Timestamp() const { return m_timestamp; }

	void SetEnd(boost::chrono::seconds end)
	{
		m_end = end;
		BuildTimeStamp(end);
	}

	bool InScope(const boost::chrono::seconds& timePoint) const
	{
		return timePoint >= m_begin && (m_spanEnd ? timePoint <= m_end : timePoint < m_end);
	}

	void SetSpanEnd(bool spanEnd) { m_spanEnd = spanEnd; }
	bool SpanEnd() const { return m_spanEnd; }

	bool NotifyEnd() const { return m_notifyEnd; }
	void SetNotifyEnd(bool notify) { m_notifyEnd = notify; }

	static boost::chrono::hours DayHours;

private:
	void BuildTimeStamp(boost::chrono::seconds timeSeconds);

	int m_index;
	boost::chrono::seconds m_begin;
	boost::chrono::seconds m_end;
	string m_timestamp;

	bool m_spanEnd;
	bool m_notifyEnd;
};

typedef boost::shared_ptr<CHistTimeRange> TimeRangePtr;
typedef vector<TimeRangePtr> TimeRangeVec;
typedef TimeRangeVec::iterator TimeRangeVecIter;

int GetMarketSectionTimePoints(const string& symbol, TimeSpanVec& timeSpanVec);


