#pragma once

#include "QuoteAgentCallback.h"
#include "QuoteUdpAgent.h"
#include "QuoteUpdateFunc.h"

class CSimpleQuoteReceiver : public CQuoteAgentCallback
{
public:
	CSimpleQuoteReceiver();
	~CSimpleQuoteReceiver();

	boost::tuple<bool, string> QuoteLogin(const string& address, const string& brokerId, const string& investorId, const string& userId, const string& password);

	void Subscribe(vector<string>& symbols, QuoteUpdateFunc cbQuoteUpdate);
	void Unsubscribe(){}

	// CQuoteAgentCallback override virtual functions
	virtual void OnSubscribeCompleted(){}
	virtual void OnUnsubscribeCompleted(){}
	virtual void OnQuoteReceived(CThostFtdcDepthMarketDataField* marketData, longlong timestamp);

	virtual void OnConnected(bool reconnected);

private:

	CQuoteUdpSingleAgent m_quoteAgent;
	QuoteUpdateFunc m_cbFunc;
	entity::Quote m_quote;
};

typedef boost::shared_ptr<CSimpleQuoteReceiver> SimpleQuoteReceiverPtr;

