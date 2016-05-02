#pragma once

#include "TradingConnection.h"
#include "MarketDataConnection.h"
#include "HistQuoteRepositry.h"

class CHistAvatar
{
public:
	CHistAvatar();
	~CHistAvatar();

	CTradingConnection m_tradingConnection;
	CMarketDataConnection m_marketDataConnection;

	CHistQuoteRepositry m_quoteRepositry;

};

