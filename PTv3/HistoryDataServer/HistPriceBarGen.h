#pragma once

#include "HistTradingTimeSpan.h"

class CQuote;

typedef boost::function<void(int, double, double, double, double, const string&)> BarChangeFunc;

class CHistPriceBarGen
{
public:
	CHistPriceBarGen();
	~CHistPriceBarGen();

	int PopulateTimeRange(int precision);
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
	
	static log4cpp::Category& logger;

	void RaiseBarChangeEvent(int barIdx, const string& timestamp) const
	{
		if (!m_onBarChanged.empty())
			m_onBarChanged(barIdx, m_open, m_high, m_low, m_close, timestamp);
	}

	void RaiseBarFinalizedEvent() const;
	void TriggerFinalizeLastBar() const;

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

	typedef boost::shared_ptr<CHistTradingTimeSpan> TradingTimeSpanPtr;
	typedef vector<TradingTimeSpanPtr> TimeSpanVec;
	typedef TimeSpanVec::iterator TimeSpanVecIter;
	TimeSpanVec m_vecTimeSpan;

	typedef boost::shared_ptr<CHistTimeRange> TimeRangePtr;
	typedef vector<TimeRangePtr> TimeRangeVec;
	typedef TimeRangeVec::iterator TimeRangeVecIter;
	TimeRangeVec m_vecTimeRanges;
	CHistTimeRange* m_currentTimeRange;

	CHistTradingTimeSpan* m_currentTimeSpan;
};

