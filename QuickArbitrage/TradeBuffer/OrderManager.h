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

	bool Register(ClientBase* pClient, const std::string& brokerId, const std::string& userId, const std::string& password);
	void Unregister(std::string& brokerId, std::string& userId);

	ClientBase* GetCurrentClient(){ return m_pClient; }
	void SetCurrentClient(ClientBase* pClient) { m_pClient = pClient; }

	void Subscribe(std::vector<std::string>& symbols);
	void UnSubscribe();

	void Buy()
	{
		m_tradeAgent.Buy();
	}

	virtual void OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///��¼������Ӧ
	void OnRspUserLogin(bool succ, std::string& msg)
	{
		if(m_pClient != NULL)
		{
			m_pClient->OnRegisterResult(succ, msg);
		}

		if(!succ) SetCurrentClient(NULL);
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
	
	ClientBase* m_pClient;

};

