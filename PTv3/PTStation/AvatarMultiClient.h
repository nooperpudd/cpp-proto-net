#pragma once

#include "AvatarClient.h"
#include "SimpleQuoteReceiver.h"

typedef boost::shared_ptr<CTradeAgent> TradeAgentPtr;
typedef boost::shared_ptr<COrderProcessor> OrderProcessorPtr;

class CAvatarMultiClient : public CAvatarClient
{
public:
	CAvatarMultiClient(const string& sessionId);
	~CAvatarMultiClient();

	bool MultiClient() { return true; }

	boost::tuple<bool, string> TradeLogin(const string& address, const string& brokerId, const string& investorId, vector<string>& userIds, const string& password);
	void TradeLogout();
	boost::tuple<bool, string> QuoteLogin(const string& address, const string& brokerId, const string& investorId, const string& userId, const string& password);
	void QuoteLogout();

protected:
	void OnAddPortfolio(CPortfolio* pPortfolio);

private:
	void RegisterQuote(CPortfolio* pPortfolio);
	void BindingOrderProcessor(CPortfolio* pPortfolio);
	void FindOrderProcessor(const string& userId, COrderProcessor** pGotOrderProc);

	string m_quoteAddress;
	string m_quoteBrokerId;
	string m_quoteInvestorId;
	string m_quoteUserId;
	string m_quotePassword;

	boost::unordered_map<string, SimpleQuoteReceiverPtr> m_quoteReceiverMap;

	vector<TradeAgentPtr> m_tradeAgentPool;
	boost::unordered_map<string, OrderProcessorPtr> m_orderProcessorPool;
};

