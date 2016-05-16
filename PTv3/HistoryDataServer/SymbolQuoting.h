#pragma once
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

private:
	void OnQuoteRecevied(boost::chrono::steady_clock::time_point& timestamp, CQuote* pQuote);

	static log4cpp::Category& logger;

	string m_symbol;
	string m_timeFrame;
	CHistQuoteFetcher* m_pFetcher;
};

typedef boost::shared_ptr<CSymbolQuoting> SymbolQuotingPtr;
typedef vector<SymbolQuotingPtr> SymbolQuotingVector;
typedef SymbolQuotingVector::iterator SymbolQuotingVecIter;

