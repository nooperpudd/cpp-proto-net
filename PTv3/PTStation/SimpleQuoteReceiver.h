#pragma once

#include "QuoteAgentCallback.h"

class CQuoteUdpMultiAgent;

class CSimpleQuoteReceiver : public CQuoteAgentCallback
{
public:
	CSimpleQuoteReceiver();
	~CSimpleQuoteReceiver();

	// CQuoteAgentCallback override virtual functions
	virtual void OnSubscribeCompleted(){}
	virtual void OnUnsubscribeCompleted(){}
	virtual void OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData, longlong timestamp);

	virtual void OnConnected(bool reconnected);

	void Init(CQuoteUdpMultiAgent* pQuoteAgent){ m_pQuoteAgent = pQuoteAgent; }

private:

	CQuoteUdpMultiAgent* m_pQuoteAgent;
};

