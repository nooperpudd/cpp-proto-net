#include "stdafx.h"
#include "Quote.h"


CQuote::CQuote(): m_last(0), m_open(0), m_high(0), m_low(0), m_volume(0), m_close(0), m_updateMillisec(0), m_bid(0), m_bidSize(0), m_ask(0), m_askSize(0), m_averagePrice(0)
{
}


CQuote::~CQuote()
{
}
