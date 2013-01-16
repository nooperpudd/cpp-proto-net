#pragma once

#include "TradeAgentCallback.h"
#include "multilegorderptr.h"
#include "MLOrderStateMachine.h"
#include "SgOrderStateMachine.h"
#include "OrderProcPushFuncDef.h"

#include "boost/thread.hpp"

class CPortfolio;

class COrderProcessor2 : public CTradeAgentCallback
{
public:
	COrderProcessor2(void);
	~COrderProcessor2(void);

	void SubmitPortfOrder(CPortfolio* pPortf, const MultiLegOrderPtr& multilegOrder);
	COrderPlacer* CreateSingleOrderPlacer(CPortfolio* pPortf, trade::MultiLegOrder* pMlOrder, const InputOrderPtr& pInputOrder, int retryTimes);
	void RaiseMLOrderPlacerEvent(const string& mlOrdPlacerId, COrderEvent& orderEvent);
	
	int LockForSubmit(string& outOrdRef);
	bool SubmitAndUnlock(trade::InputOrder* pOrder);
	int GenerateOrderRef(string& outOrdRef);
	bool SubmitOrderToTradeAgent(trade::InputOrder* pOrder);

	void CancelOrder(const std::string& ordRef, 
					 const std::string& exchId, 
					 const std::string& ordSysId, 
					 const std::string& userId,
					 const std::string& symbol);

	void PublishMultiLegOrderUpdate(trade::MultiLegOrder* pOrder);
	void SetPushPortfolioFunc(PushMultiLegOrderFunc funcPushMLOrder);

	void PublishOrderUpdate(const string& portfId, const string& mlOrderId, trade::Order* legOrd);
	void SetPushOrderFunc(PushLegOrderFunc funcPushOrder);

	void PublishTradeUpdate(trade::Trade* pTrade);
	void SetPushTradeFunc(PushTradeFunc funcPushTrade);

	void PublishPositionDetail(trade::PositionDetailInfo* pPosiDetailInfo);
	void SetPushPositionDetailFunc(PushPositionDetailFunc funcPushPosiDetail);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///��¼������Ӧ
	virtual void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID);

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg);

	///��������������Ӧ
	virtual void OnRspOrderAction(bool succ, const std::string& orderRef, const std::string& msg);

	///����֪ͨ
	virtual void OnRtnOrder(trade::Order* order);

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(trade::Trade* pTrade);

	///�����ѯͶ���ֲ߳���ϸ��Ӧ
	virtual void OnRspQryInvestorPositionDetail(trade::PositionDetailInfo* pPositionDetail);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(){}

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(){}

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(const trade::AccountInfo& accountInfo);

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(){}

	//////////////////////////////////////////////////////////////////////////

private:

	static void PrintOrderStatus(trade::Order* order);
	static bool GetOrderEvent(trade::Order* order, COrderEvent** ppOrderEvt);

	CMLOrderStateMachine m_mlOrderStateMachine;
	CSgOrderStateMachine m_sgOrderStateMachine;

	int m_maxOrderRef;
	boost::mutex m_mutOrdRefIncr;
	boost::condition_variable m_condSubmit;

	PushMultiLegOrderFunc m_pushMultiOrdFunc;
	PushLegOrderFunc m_pushLegOrderFunc;
	PushTradeFunc m_pushTradeFunc;
	PushPositionDetailFunc m_pushPosiDetailFunc;
};

