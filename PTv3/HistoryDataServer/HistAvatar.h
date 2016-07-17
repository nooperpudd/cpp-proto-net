#pragma once

#include "TradingConnection.h"
#ifdef UDP_QUOTE
#include "MarketDataUdp.h"
#else
#include "MarketDataConnection.h"
#endif

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
#ifdef UDP_QUOTE
	CMarketDataUdp m_marketDataConnection;
#else
	CMarketDataConnection m_marketDataConnection;
#endif
	
	CHistQuoteRepositry m_quoteRepositry;
	CHistTechDataRepo m_techDataRepositry;

	SymbolQuotingVector m_vecSymbolQuoting;
};

