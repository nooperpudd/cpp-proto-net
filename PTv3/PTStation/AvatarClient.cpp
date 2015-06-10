#include "StdAfx.h"
#include "AvatarClient.h"
#include "ProtobufPacket.h"
#include "ProtocolIDs.h"
#include "globalmembers.h"

const int WAIT_CURRENT_SENDING_DONE_MILLISEC = 300;

CAvatarClient::CAvatarClient(const string& sessionId)
	: m_sessionId(sessionId)
	, m_tradeLogged(false)
	, m_quoteLogged(false)
	, m_destroyed(false)
{
	// Connect quoteAgent and Quote repository
	m_quoteRepositry.Init(&m_quoteAgent);
	m_quoteAgent.SetCallbackHanlder(&m_quoteRepositry);

	m_portfolioMgr.SetOnAddPortfolioHandler(boost::bind(&CAvatarClient::HandleAddingPortfolio, this, _1));
	m_orderProcessor.Initialize(this, &m_tradeAgent);
}

CAvatarClient::~CAvatarClient(void)
{
	m_orderProcessor.ReleaseAvatar();
	m_portfolioMgr.Clear();

	// wait 300 milliseconds for current sending done
	LOG_DEBUG(logger, boost::str(boost::format("Avatar %s waits %d milliseconds before destruction") 
		% m_investorId % WAIT_CURRENT_SENDING_DONE_MILLISEC));
	boost::this_thread::sleep_for(boost::chrono::milliseconds(WAIT_CURRENT_SENDING_DONE_MILLISEC));
	{
		boost::mutex::scoped_lock l(m_mutConnection);
		m_quoteRepositry.Init(NULL); // detach QuoteAgent
		m_quoteAgent.SetCallbackHanlder(NULL);
		m_destroyed = true;
	}
}

boost::tuple<bool, string> CAvatarClient::TradeLogin(const string& address, const string& brokerId, const string& investorId, vector<string>& userIds, const string& password)
{
	if(m_tradeLogged)
		return boost::make_tuple(false, "Trade already Logged in");
	m_investorId = investorId;
	m_userId = userIds[0];
	return m_tradeAgent.Login(address, brokerId, investorId, m_userId, password);
}

void CAvatarClient::TradeLogout()
{
	m_tradeAgent.Logout();
}

boost::tuple<bool, string> CAvatarClient::QuoteLogin(const string& address, const string& brokerId, const string& investorId, const string& userId, const string& password)
{
	if(m_quoteLogged)
		return boost::make_tuple(false, "Quote already Logged in");

	return m_quoteAgent.Login(address, brokerId, investorId, userId, password);
}

void CAvatarClient::QuoteLogout()
{
	m_quoteAgent.Logout();
}

void CAvatarClient::PublishPortfolioUpdate( const entity::PortfolioUpdateItem& portfolioUpdate )
{
	ProtobufPacket<entity::PortfolioUpdateItem> portfUpdateResp(PortfolioUpdateResponseID);
	portfUpdateResp.getData().CopyFrom(portfolioUpdate);
	
	UnderlyingPushPacket(&portfUpdateResp);
}

void CAvatarClient::PublishMultiLegOrderUpdate( trade::MultiLegOrder* pOrder )
{
	ProtobufPacket<trade::MultiLegOrder> mlOrderUpdateResp(MultilegOrderResponseID);
	mlOrderUpdateResp.getData().CopyFrom(*pOrder);

	UnderlyingPushPacket(&mlOrderUpdateResp);
}

void CAvatarClient::PublishLegOrderUpdate( const string& portfId, const string& mlOrderId, trade::Order* legOrd )
{
	ProtobufPacket<entity::LegOrderUpdateParam> legOrdUpdateResp(LegOrderResponseID);
	legOrdUpdateResp.getData().set_portfid(portfId);
	legOrdUpdateResp.getData().set_multilegorderid(mlOrderId);
	legOrdUpdateResp.getData().set_legorderref(legOrd->orderref());

	trade::Order* pOrd = legOrdUpdateResp.getData().mutable_legorder();
	pOrd->CopyFrom(*legOrd);

	UnderlyingPushPacket(&legOrdUpdateResp);
}

void CAvatarClient::PublishTradeUpdate( trade::Trade* pTrade )
{
	ProtobufPacket<trade::Trade> tradeUpdateResp(TradeResponseID);
	tradeUpdateResp.getData().CopyFrom(*pTrade);

	UnderlyingPushPacket(&tradeUpdateResp);
}

void CAvatarClient::PublishPositionDetail( trade::PositionDetailInfo* pPosiDetailInfo )
{

}

void CAvatarClient::UnderlyingPushPacket(OutgoingPacket* pPacket)
{
	boost::mutex::scoped_lock l(m_mutConnection);
	if (!IsInactive())
	{
		SendResult res = PushPacket(pPacket);
		if (res == SendResult_NotOK)
		{
			logger.Warning(boost::str(boost::format("Error sending packet for Avatar %s") % m_investorId));
		}
	}
}

void CAvatarClient::HandleAddingPortfolio(CPortfolio* pPortfolio)
{
	OnAddPortfolio(pPortfolio);
}

void CAvatarClient::OnAddPortfolio(CPortfolio* pPortfolio)
{
	if (pPortfolio != NULL)
		pPortfolio->SubscribeQuotes(&m_quoteRepositry);
}

