#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include "TradeAgentCallback.h"
#include "TradeAgent.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

using namespace std;

typedef boost::shared_ptr<trade::MultiLegOrder> MultiLegOrderPtr;

class COrderProcessor : public CTradeAgentCallback
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	void Initialize(CTradeAgent* pTradeAgent);

	void OpenOrder(MultiLegOrderPtr multilegOrder);
	void CloseOrder(const string& orderId);
	void CancelOrder(const string& orderId);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///��¼������Ӧ
	virtual void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID){}

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg){}

	///��������������Ӧ
	virtual void OnRspOrderAction(bool succ, const std::string& orderRef, const std::string& msg){}

	///����֪ͨ
	virtual void OnRtnOrder(trade::Order* order){}

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(trade::Trade* pTrade){}

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(){}

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(){}

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(){}

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(){}

	//////////////////////////////////////////////////////////////////////////

private:
	map<string, MultiLegOrderPtr> m_pendingMultiLegOrders;

	CTradeAgent* m_pTradeAgent;
};

