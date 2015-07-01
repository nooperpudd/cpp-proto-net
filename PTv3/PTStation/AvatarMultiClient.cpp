#include "StdAfx.h"
#include "AvatarMultiClient.h"
#include "DualScalperStrategy.h"


CAvatarMultiClient::CAvatarMultiClient(const string& sessionId)
	: CAvatarClient(sessionId)
{
}

CAvatarMultiClient::~CAvatarMultiClient()
{
}

boost::tuple<bool, string> CAvatarMultiClient::TradeLogin(const string& address, const string& brokerId, const string& investorId, vector<string>& userIds, const string& password)
{
	if (m_tradeLogged)
		return boost::make_tuple(false, "Trade already Logged in");

	m_investorId = investorId;

	bool loginSucc = true;
	string errMsg;
	int orderProcIndex = 0;
	for (vector<string>::iterator iter = userIds.begin(); iter != userIds.end(); ++iter)
	{
		const string& userId = (*iter);
		if (!userId.empty())
		{
			TradeAgentPtr tradeAgent(new CTradeAgent);
#ifdef USE_FEMAS_API
			tradeAgent->SetIndex(orderProcIndex);
#endif			
			OrderProcessorPtr orderProc(new COrderProcessor(orderProcIndex));
			orderProcIndex++;
			orderProc->Initialize(this, tradeAgent.get());
			boost::tuple<bool, string> res = tradeAgent->Login(address, brokerId, investorId, userId, password);
			loginSucc = boost::get<0>(res);
			if (loginSucc)
			{
				m_tradeAgentPool.push_back(tradeAgent);
				m_orderProcessorPool.insert(std::make_pair(userId, orderProc));
			}
			else
			{
				errMsg = boost::str(boost::format("UserId %s Failed to log in due to %s") % userId % boost::get<1>(res));
				break;
			}
			
		}
	}

	m_tradeLogged = loginSucc;

	return boost::make_tuple<bool, string>(loginSucc, errMsg);
}

void CAvatarMultiClient::TradeLogout()
{
	for (vector<TradeAgentPtr>::iterator iter = m_tradeAgentPool.begin(); iter != m_tradeAgentPool.end(); ++iter)
	{
		(*iter)->Logout();
	}
	m_tradeLogged = false;
}

boost::tuple<bool, string> CAvatarMultiClient::QuoteLogin(const string& address, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
	m_quoteAddress = address;
	m_quoteBrokerId = brokerId;
	m_quoteInvestorId = investorId;
	m_quoteUserId = userId;
	m_quotePassword = password;

	return boost::make_tuple(true, "");
}

void CAvatarMultiClient::QuoteLogout()
{
	
}

void CAvatarMultiClient::OnAddPortfolio(CPortfolio* pPortfolio)
{
	// 1. Register quote
	RegisterQuote(pPortfolio);
	// 2. Binding OrderProcessor
	BindingOrderProcessor(pPortfolio);
}

void CAvatarMultiClient::RegisterQuote(CPortfolio* pPortfolio)
{
	const string& portfId = pPortfolio->ID();
	SimpleQuoteReceiverPtr quoteReceiver(new CSimpleQuoteReceiver);
	boost::tuple<bool, string> res = quoteReceiver->QuoteLogin(
		m_quoteAddress, m_quoteBrokerId, m_quoteInvestorId, m_quoteUserId, m_quotePassword);
	if (boost::get<0>(res))
	{
		pPortfolio->SubscribeQuotes(quoteReceiver.get());
		m_quoteReceiverMap.insert(std::make_pair(portfId, quoteReceiver));
	}
	else
	{
		logger.Error(boost::str(boost::format("Cannot subscribe SimpleQuoteReceiver for portfolio(%s)") % pPortfolio->ID()));
	}
}

void CAvatarMultiClient::BindingOrderProcessor(CPortfolio* pPortfolio)
{
	if (pPortfolio->StrategyType() == entity::DUAL_SCALPER)
	{
		CMultiRouteStrategy* multiRouteStrategy = dynamic_cast<CMultiRouteStrategy*>(pPortfolio->Strategy());
		if (multiRouteStrategy != NULL)
			multiRouteStrategy->BindRoutes(pPortfolio, 
			boost::bind(&CAvatarMultiClient::FindOrderProcessor, this, _1, _2));
	}
	else
	{
		COrderProcessor* pOrderProc = NULL;
		FindOrderProcessor("", &pOrderProc);
		pPortfolio->Strategy()->ReinitOrderPlacer(pOrderProc);
	}
}

void CAvatarMultiClient::FindOrderProcessor(const string& userId, COrderProcessor** pGotOrderProc)
{
	boost::unordered_map<string, OrderProcessorPtr>::iterator iter = m_orderProcessorPool.find(userId);
	if (iter == m_orderProcessorPool.end())
	{
		// in case not find desired order processor, return the first one
		iter = m_orderProcessorPool.begin();
	}

	if (iter != m_orderProcessorPool.end())
	{
		*pGotOrderProc = (iter->second).get();
	}
}
