#pragma once

#include "HistDataPersistence.h"
#include "HistPriceBarGen.h"

class CHistQuoteRepositry;
class CHistQuoteFetcher;
class CQuote;

class CSymbolQuoting
{
public:
	CSymbolQuoting(const string& symbol, const string& timeFrame);
	~CSymbolQuoting();

	bool Subscribe(CHistQuoteRepositry* pQuoteRepo);
	void Unsubscribe(CHistQuoteRepositry* pQuoteRepo);

	void Init(const boost::gregorian::date& tradingDay);

private:
	void OnQuoteRecevied(boost::chrono::steady_clock::time_point& timestamp, CQuote* pQuote);

	void OnBarFinalized(int barIdx, double open, double high, double low, double close, const string& timestamp);

	static log4cpp::Category& logger;
	static int ParseTimeFrame(const string& timeFrame);

	string m_symbol;
	string m_timeFrame;
	int m_precision;
	CHistQuoteFetcher* m_pFetcher;

	CHistPriceBarGen m_priceBarGen;
	CHistDataPersistence m_histDataWriter;
};

typedef boost::shared_ptr<CSymbolQuoting> SymbolQuotingPtr;
typedef vector<SymbolQuotingPtr> SymbolQuotingVector;
typedef SymbolQuotingVector::iterator SymbolQuotingVecIter;

