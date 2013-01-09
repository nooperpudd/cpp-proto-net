#pragma once

#include "TradeAgentCallback.h"
#include "multilegorderptr.h"
#include "MLOrderStateMachine.h"
#include "SgOrderStateMachine.h"

#include "boost/thread.hpp"

class CPortfolio;

class COrderProcessor2 : public CTradeAgentCallback
{
public:
	COrderProcessor2(void);
	~COrderProcessor2(void);

	void SubmitPortfOrder(CPortfolio* pPortf, const MultiLegOrderPtr& multilegOrder);
	OrderPlacerPtr CreateSingleOrderPlacer(trade::MultiLegOrder* pMlOrder, const InputOrderPtr& pInputOrder, int retryTimes);
	void RaiseMLOrderPlacerEvent(const string& mlOrdPlacerId, COrderEvent& orderEvent);

	int GenerateOrderRef(char** ppOrdRef);

	void CancelOrder(const std::string& ordRef, 
					 const std::string& exchId, 
					 const std::string& ordSysId, 
					 const std::string& userId,
					 const std::string& symbol);

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

};

