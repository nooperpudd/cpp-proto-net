#pragma once

#include "TradingConnection.h"
#include "MarketDataConnection.h"
#include "HistQuoteRepositry.h"
#include "HistTechDataRepo.h"
#include "SymbolQuoting.h"

class CHistAvatar
{
public:
	CHistAvatar();
	~CHistAvatar();

	bool Start();
	void Stop();

private:
	static log4cpp::Category& logger;
	void SubscribeQuotes();
	void UnsubscribeQuotes();

	CTradingConnection m_tradingConnection;
	CMarketDataConnection m_marketDataConnection;

	CHistQuoteRepositry m_quoteRepositry;
	CHistTechDataRepo m_techDataRepositry;

	SymbolQuotingVector m_vecSymbolQuoting;
};

