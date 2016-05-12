#include "stdafx.h"
#include "SymbolQuoting.h"


CSymbolQuoting::CSymbolQuoting(const string& symbol, const string& timeFrame)
{
}


CSymbolQuoting::~CSymbolQuoting()
{
}

bool CSymbolQuoting::Subscribe(CHistQuoteRepositry* pQuoteRepo)
{
	return true;
}

void CSymbolQuoting::Unsubscribe(CHistQuoteRepositry* pQuoteRepo)
{}
