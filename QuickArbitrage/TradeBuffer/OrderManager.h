#pragma once

#include "QuoteListener.h"
#include "RemoteClient.h"
#include "TradeAgent.h"

#include <string>
#include <vector>

class COrderManager : public QuoteListener
{
public:
	COrderManager(void);
	~COrderManager(void);

	void Initialize();

	void Register(RemoteClient* pClient, std::string& brokerId, std::string& userId, std::string& password);
	void Unregister(std::string& brokerId, std::string& userId);

	void Subscribe(std::vector<std::string>& symbols);
	void UnSubscribe();

	virtual void OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///��¼������Ӧ
	void OnRspUserLogin(bool succ, std::string& msg)
	{
		if(m_pClient != NULL)
			m_pClient->OnRegisterResult(succ, msg);
	}

	///Ͷ���߽�����ȷ����Ӧ
	void OnRspSettlementInfoConfirm(){}

	///�����ѯ��Լ��Ӧ
	void OnRspQryInstrument(){}

	///�����ѯ�ʽ��˻���Ӧ
	void OnRspQryTradingAccount(){}

	///�����ѯͶ���ֲ߳���Ӧ
	void OnRspQryInvestorPosition(){}

	///����¼��������Ӧ
	void OnRspOrderInsert(){}

	///��������������Ӧ
	void OnRspOrderAction(){}


	///����֪ͨ
	void OnRtnOrder(){}

	///�ɽ�֪ͨ
	void OnRtnTrade(){}
	//////////////////////////////////////////////////////////////////////////
private:

	CTradeAgent	m_tradeAgent;
	
	RemoteClient* m_pClient;
};

