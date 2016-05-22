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

};

typedef boost::function<void(int, double, double, double, double, const string&)> BarChangeFunc;

class CHistPriceBarGen
{
public:
	CHistPriceBarGen();
	~CHistPriceBarGen();

	void Init(const string& symbol, int precision);
	void Calculate(CQuote* pQuote);

	void SetBarChangedHandler(BarChangeFunc onBarChanged)
	{
		m_onBarChanged = onBarChanged;
	}

	void SetBarFinalizedHandler(BarChangeFunc onBarFinalized)
	{
		m_onBarFinalized = onBarFinalized;
	}

	int GetIndex(const string& quoteTime);
	int GetIndex(const string& quoteTime, string* timestamp);

private:
	typedef boost::shared_ptr<CHistTradingTimeSpan> TradingTimeSpanPtr;

	static log4cpp::Category& logger;

	void RaiseBarChangeEvent(int barIdx, const string& timestamp) const
	{
		if (!m_onBarChanged.empty())
			m_onBarChanged(barIdx, m_open, m_high, m_low, m_close, timestamp);
	}

	void RaiseBarFinalizedEvent() const;

	string m_symbol;
	int m_precision;
	int m_barCount;

	int m_currentIdx;
	string m_currentTimestamp;

	double m_open;
	double m_high;
	double m_low;
	double m_close;

	BarChangeFunc m_onBarChanged;
	BarChangeFunc m_onBarFinalized;

	typedef vector<TradingTimeSpanPtr> TimeSpanVec;
	typedef TimeSpanVec::iterator TimeSpanVecIter;
	TimeSpanVec m_vecTimeSpan;
};

