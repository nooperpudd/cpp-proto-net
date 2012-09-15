#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include "TradeAgentCallback.h"
#include "TradeAgent.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

using namespace std;

typedef boost::shared_ptr<trade::MultiLegOrder> MultiLegOrderPtr;
typedef boost::function<void(trade::MultiLegOrder*)> PushMultiLegOrderFunc;
typedef boost::function<void( const string&, const string&, trade::Order* legOrd)> PushLegOrderFunc;
typedef boost::function<void(trade::Trade*)> PushTradeFunc;

class COrderProcessor : public CTradeAgentCallback
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	void Initialize(CTradeAgent* pTradeAgent);

	void SubmitOrder(MultiLegOrderPtr multilegOrder);
	void CancelOrder(const string& orderId);

	bool QueryAccountInfo(string* outSerializedAcctInfo);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///��¼������Ӧ
	virtual void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID);

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg);

	///��������������Ӧ
	virtual void OnRspOrderAction(bool succ, const std::string& orderRef, const std::string& msg){}

	///����֪ͨ
	virtual void OnRtnOrder(trade::Order* order);

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(trade::Trade* pTrade);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(){}

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(){}

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(const trade::AccountInfo& accountInfo);

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(){}

	//////////////////////////////////////////////////////////////////////////

	void PublishMultiLegOrderUpdate(trade::MultiLegOrder* pOrder)
	{
		if(!m_pushMultiOrdFunc.empty())
		{
			m_pushMultiOrdFunc(pOrder);
		}
	}
	void SetPushPortfolioFunc(PushMultiLegOrderFunc funcPushMLOrder)
	{
		m_pushMultiOrdFunc = funcPushMLOrder;
	}

	void SetPushOrderFunc(PushLegOrderFunc funcPushOrder)
	{
		m_pushLegOrderFunc = funcPushOrder;
	}

	void PublishOrderUpdate(const string& portfId, const string& mlOrderId, trade::Order* legOrd)
	{
		if(!m_pushLegOrderFunc.empty())
		{
			m_pushLegOrderFunc(portfId, mlOrderId, legOrd);
		}
	}

	void PublishTradeUpdate(trade::Trade* pTrade)
	{
		if(!m_pushTradeFunc.empty())
		{
			m_pushTradeFunc(pTrade);
		}
	}
	void SetPushTradeFunc(PushTradeFunc funcPushTrade)
	{
		m_pushTradeFunc = funcPushTrade;
	}

private:
	int IncrementalOrderRef(trade::MultiLegOrder* pMlOrder, int maxOrderRef);
	void RemoveFromPending(trade::MultiLegOrder* pMlOrder);

	map<string, MultiLegOrderPtr> m_pendingMultiLegOrders;
	typedef map<string, MultiLegOrderPtr>::iterator MultiLegOrderIter;
	map<string /* orderRef */, string /* mlOrderId */> m_pendingTicketOrderMap;
	typedef map<string, string>::iterator PendingTktOrdMapIter;
	boost::mutex m_mutTicketOrderMap;
	 
	CTradeAgent* m_pTradeAgent;

	PushMultiLegOrderFunc m_pushMultiOrdFunc;
	PushLegOrderFunc m_pushLegOrderFunc;
	PushTradeFunc m_pushTradeFunc;

	int m_maxOrderRef;
	boost::mutex m_mutOrdRefIncr;

	boost::condition_variable m_condQryAcct;
	boost::mutex m_mutQryAcct;
	string m_serializedQryAcctInfo;
};

