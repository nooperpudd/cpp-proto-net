#pragma once

class CQuote;

class CHistTradingTimeSpan
{
public:
	CHistTradingTimeSpan(const char* timeBegin, const char* timeEnd, int precision);

	const boost::chrono::seconds& Start() const { return m_Start; }
	const boost::chrono::seconds& End() const { return m_End; }

	void SetOffset(int offset) { m_offset = offset; }
	int Offset() const { return m_offset; }

	int StartIndex() const { return m_offset; }
	int EndIndex() const { return m_offset + m_endIndex; }

	bool TrueEnd() const { return m_trueEnd; }
	void SetTrueEnd(bool trueEnd) { m_trueEnd = trueEnd; }

	bool InScope (const boost::chrono::seconds& timePoint) const
	{
		return timePoint >= m_Start && timePoint <= m_End;
	}
	// return is 0 based. if incoming time point equals to m_End, regard it as last point before m_End
	int GetIndex(const boost::chrono::seconds& timePoint) const;
	// return is 0 based
	int GetIndex(const boost::chrono::seconds& timePoint, string* outTimestamp) const;

	static int GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, int precision);
	 
private:

	boost::chrono::seconds m_Start;
	boost::chrono::seconds m_End;
	int m_offset;
	int m_endIndex;
	int m_precision;

	static boost::chrono::hours DayHours;
	bool m_trueEnd;
	bool m_sharpAligning;
};

int GetMarketSectionTimePoints(const string& symbol, vector<string>& startTimePoints, vector<string>& endTimePoints);


