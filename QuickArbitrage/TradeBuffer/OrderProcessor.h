#pragma once

#include "tradeagentcallback.h"

#include <string>

using namespace std;

class COrderProcessor :
	public CTradeAgentCallback
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	bool Login(const string& brokerID, const string& userID, const string& password);
	void Logout( const string& brokerID, const string& userID );


	///��¼������Ӧ
	virtual void OnRspUserLogin(){}

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(){}

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(){}

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(){}

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(){}

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(){}

	///��������������Ӧ
	virtual void OnRspOrderAction(){}


	///����֪ͨ
	virtual void OnRtnOrder(){}

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(){}
};

