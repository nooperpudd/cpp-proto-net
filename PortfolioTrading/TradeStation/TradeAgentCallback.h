#pragma once

#include <string>

#include "../Entity/gen/cpp/trade.pb.h"

class CTradeAgentCallback
{
public:
	CTradeAgentCallback(void){}
	virtual ~CTradeAgentCallback(void){}

	///��¼������Ӧ
	virtual void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID) = 0;

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg) = 0;

	///��������������Ӧ
	virtual void OnRspOrderAction(bool succ, const std::string& orderRef, const std::string& msg) = 0;

	///����֪ͨ
	virtual void OnRtnOrder(trade::Order* order) = 0;

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(trade::Trade* pTrade) = 0;

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(){}

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(){}

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(const trade::AccountInfo& accountInfo){}

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(){}
};