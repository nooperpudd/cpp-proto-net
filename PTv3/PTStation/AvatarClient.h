#pragma once

#include "QuoteAgentFacade.h"
#include "PortfolioManager.h"
#include "QuoteRepositry.h"
#include "OrderProcessor.h"
#include "TechDataRepo.h"
#include "entity/message.pb.h"

#ifndef USE_FEMAS_API
#ifdef USE_ZEUSING_API
#include "TradeAgent_Zeus.h"
#else
#include "TradeAgent.h"
#endif
#else
#include "TradeAgent_FM.h"
#endif


#include <boost/date_time.hpp>

class CAvatarClient : public LogicalConnection
{
public:
	CAvatarClient(const string& sessionId);
	virtual ~CAvatarClient(void);

	virtual bool MultiClient() { return false; }
	virtual const char* getKey(){ return m_sessionId.c_str(); }

	const string& Pseudo() const { return m_investorId; }
	void Pseudo(const string& val) { m_investorId = val; }

	virtual boost::tuple<bool, string> TradeLogin(const string& address, const string& brokerId, const string& investorId, vector<string>& userIds, const string& password);
	virtual void TradeLogout();
	virtual boost::tuple<bool, string> QuoteLogin(const string& address, const string& brokerId, const string& investorId, const string& userId, const string& password);
	virtual void QuoteLogout();

	const boost::gregorian::date& TradingDay(){ return m_tradeAgent.TradingDay(); }

	CPortfolioManager& PortfolioManager(){ return m_portfolioMgr; }
	COrderProcessor& OrderProcessor(){ return m_orderProcessor; }
	CTechDataRepo& TechDataRepo(){ return m_dataRepo; }

	void PublishPortfolioUpdate(const entity::PortfolioUpdateItem& portfolioUpdate);
	void PublishMultiLegOrderUpdate(trade::MultiLegOrder* pOrder);
	void PublishLegOrderUpdate(const string& portfId, const string& mlOrderId, trade::Order* legOrd);
	void PublishTradeUpdate(trade::Trade* pTrade);
	void PublishPositionDetail(trade::PositionDetailInfo* pPosiDetailInfo);

	bool m_destroyed;

protected:

	void UnderlyingPushPacket(OutgoingPacket* pPacket);
	void HandleAddingPortfolio(CPortfolio* pPortfolio);
	virtual void OnAddPortfolio(CPortfolio* pPortfolio);

	string				m_sessionId;
	string				m_investorId;
	CPortfolioManager	m_portfolioMgr;

private:
	string				m_userId;

	CTechDataRepo		m_dataRepo;
	CTradeAgent			m_tradeAgent;
	CQuoteRepositry		m_quoteRepositry;
	CQuoteAgentFacade	m_quoteAgent;
	COrderProcessor		m_orderProcessor;

	boost::mutex		m_mutConnection;
	
	bool m_tradeLogged;
	bool m_quoteLogged;

};

