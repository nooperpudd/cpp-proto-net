#pragma once
class CHistQuoteRepositry;

class CSymbolQuoting
{
public:
	CSymbolQuoting(const string& symbol, const string& timeFrame);
	~CSymbolQuoting();

	bool Subscribe(CHistQuoteRepositry* pQuoteRepo);
	void Unsubscribe(CHistQuoteRepositry* pQuoteRepo);
};

typedef boost::shared_ptr<CSymbolQuoting> SymbolQuotingPtr;
typedef vector<SymbolQuotingPtr> SymbolQuotingVector;
typedef SymbolQuotingVector::iterator SymbolQuotingVecIter;

