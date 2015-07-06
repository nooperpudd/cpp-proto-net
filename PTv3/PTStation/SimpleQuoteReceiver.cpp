#include "StdAfx.h"
#include "SimpleQuoteReceiver.h"
#ifdef _DEBUG
#include "SymbolTimeUtil.h"
#endif // _DEBUG

void SetQuoteFields(entity::Quote* outQuote, CThostFtdcDepthMarketDataField* marketData)
{
	outQuote->set_symbol(marketData->InstrumentID);
	outQuote->set_last(marketData->LastPrice);
	outQuote->set_update_time(marketData->UpdateTime);
	outQuote->set_update_millisec(marketData->UpdateMillisec);
	outQuote->set_bid(marketData->BidPrice1);
	outQuote->set_bid_size(marketData->BidVolume1);
	outQuote->set_ask(marketData->AskPrice1);
	outQuote->set_ask_size(marketData->AskVolume1);

/* Ignore unnecessary fields in Quote to increase speed	
	outQuote->set_trading_day(marketData->TradingDay);
	outQuote->set_prev_settlement_price(marketData->PreSettlementPrice);
	outQuote->set_prev_close(marketData->PreClosePrice);
	outQuote->set_prev_open_interest(marketData->PreOpenInterest);
	outQuote->set_open(marketData->OpenPrice);
	outQuote->set_high(marketData->HighestPrice);
	outQuote->set_low(marketData->LowestPrice);
	outQuote->set_volume(marketData->Volume);
	outQuote->set_turnover(marketData->Turnover);
	outQuote->set_open_interest(marketData->OpenInterest);
	outQuote->set_close(marketData->ClosePrice);
	outQuote->set_settlement_price(marketData->SettlementPrice);
	outQuote->set_upper_limit_price(marketData->UpperLimitPrice);
	outQuote->set_lower_limit_price(marketData->LowerLimitPrice);
	outQuote->set_prev_delta(marketData->PreDelta);
	outQuote->set_curr_delta(marketData->CurrDelta);
	outQuote->set_exchange_id(marketData->ExchangeID);
	outQuote->set_exchange_symbol_id(marketData->ExchangeInstID);
	outQuote->set_average_price(marketData->AveragePrice);
*/

#ifdef QUOTE_TIME_EMU

	boost::chrono::seconds tp = ParseTimeString(marketData->UpdateTime);
	//tp -= boost::chrono::hours(10);
	//tp -= boost::chrono::minutes(3);
	string updateTime = GetISOTimeString(tp);
	outQuote->set_update_time(updateTime);
	/*
	outQuote->set_update_time("09:15:58");
	outQuote->set_open(2308.6);
	outQuote->set_high(2309.8);
	outQuote->set_low(2305.4);
	outQuote->set_last(2306.6);
	*/

#endif // QUOTE_TIME_EMU

	/*
	outQuote->set_bid_2(marketData->BidPrice2);
	outQuote->set_bid_size_2(marketData->BidVolume2);
	outQuote->set_ask_2(marketData->AskPrice2);
	outQuote->set_ask_size_2(marketData->AskVolume2);
	outQuote->set_bid_3(marketData->BidPrice3);
	outQuote->set_bid_size_3(marketData->BidVolume3);
	outQuote->set_ask_3(marketData->AskPrice3);
	outQuote->set_ask_size_3(marketData->AskVolume3);
	outQuote->set_bid_4(marketData->BidPrice4);
	outQuote->set_bid_size_4(marketData->BidVolume4);
	outQuote->set_ask_4(marketData->AskPrice4);
	outQuote->set_ask_size_4(marketData->AskVolume4);
	outQuote->set_bid_5(marketData->BidPrice5);
	outQuote->set_bid_size_5(marketData->BidVolume5);
	outQuote->set_ask_5(marketData->AskPrice5);
	outQuote->set_ask_size_5(marketData->AskVolume5);
	*/
}



CSimpleQuoteReceiver::CSimpleQuoteReceiver()
	: m_quoteAgent(this)
{
}


CSimpleQuoteReceiver::~CSimpleQuoteReceiver()
{
}

void CSimpleQuoteReceiver::Subscribe(vector<string>& symbols, QuoteUpdateFunc cbQuoteUpdate)
{
	m_quoteAgent.SubscribesQuotes(symbols);
	m_cbFunc = cbQuoteUpdate;
}

void CSimpleQuoteReceiver::OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData, longlong timestamp)
{
	boost::chrono::steady_clock::duration dd(timestamp);
	boost::chrono::steady_clock::time_point tp(dd);

	SetQuoteFields(&m_quote, marketData);

	m_cbFunc(tp, &m_quote);
}

void CSimpleQuoteReceiver::OnConnected(bool reconnected)
{

}

boost::tuple<bool, string> CSimpleQuoteReceiver::QuoteLogin(const string& address, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
	return m_quoteAgent.Login(address, brokerId, investorId, userId, password);
}



