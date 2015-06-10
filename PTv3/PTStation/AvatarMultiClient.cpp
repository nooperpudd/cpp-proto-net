#include "stdafx.h"
#include "AvatarMultiClient.h"


CAvatarMultiClient::CAvatarMultiClient(const string& sessionId)
	: CAvatarClient(sessionId)
{
	m_quoteAgent = boost::shared_ptr<CQuoteUdpMultiAgent>(new CQuoteUdpMultiAgent(&m_simpleQuoteReceiver));
}


CAvatarMultiClient::~CAvatarMultiClient()
{
}

boost::tuple<bool, string> CAvatarMultiClient::TradeLogin(const string& address, const string& brokerId, const string& investorId, vector<string>& userIds, const string& password)
{
	m_investorId = investorId;

	bool loginSucc = true;
	string errMsg;
	for (vector<string>::iterator iter = userIds.begin(); iter != userIds.end(); ++iter)
	{
		TradeAgentPtr tradeAgent(new CTradeAgent);
		boost::tuple<bool, string> res = tradeAgent->Login(address, brokerId, investorId, (*iter), password);
		loginSucc = boost::get<0>(res);
		if (loginSucc)
		{
			OrderProcessorPtr orderProc(new COrderProcessor);
			orderProc->Initialize(this, tradeAgent.get());
			m_tradeAgentPool.push_back(tradeAgent);
			m_orderProcessorPool.insert(std::make_pair((*iter), orderProc));
		}
		else
		{
			errMsg = boost::str(boost::format("UserId %s Failed to log in due to %s") % (*iter) % boost::get<1>(res));
			break;
		}
	}
	return boost::make_tuple<bool, string>(loginSucc, errMsg);
}

void CAvatarMultiClient::TradeLogout()
{
	for (vector<TradeAgentPtr>::iterator iter = m_tradeAgentPool.begin(); iter != m_tradeAgentPool.end(); ++iter)
	{
		(*iter)->Logout();
	}
}

boost::tuple<bool, string> CAvatarMultiClient::QuoteLogin(const string& address, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
	return boost::make_tuple<bool, string>(true, "");
}

void CAvatarMultiClient::QuoteLogout()
{

}

void CAvatarMultiClient::OnAddPortfolio(CPortfolio* pPortfolio)
{
	// 1. Register quote

	// 2. Binding OrderProcessor
}
