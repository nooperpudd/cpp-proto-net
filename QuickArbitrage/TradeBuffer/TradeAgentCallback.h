#pragma once

class CTradeAgentCallback
{
public:
	CTradeAgentCallback(void){}
	virtual ~CTradeAgentCallback(void){}

	///��¼������Ӧ
	virtual void OnRspUserLogin() = 0;

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm() = 0;

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument() = 0;

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount() = 0;

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition() = 0;

	///����¼��������Ӧ
	virtual void OnRspOrderInsert() = 0;

	///��������������Ӧ
	virtual void OnRspOrderAction() = 0;


	///����֪ͨ
	virtual void OnRtnOrder() = 0;

	///�ɽ�֪ͨ
	virtual void OnRtnTrade() = 0;
	
};