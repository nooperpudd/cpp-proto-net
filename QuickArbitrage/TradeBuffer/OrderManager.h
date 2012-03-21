#pragma once

#include "QuoteListener.h"
#include "RemoteClient.h"
#include "TradeAgent.h"
#include "Portfolio.h"

#include <string>
#include <vector>
#include <boost/thread.hpp>

typedef std::vector< boost::shared_ptr< CPortfolio > > PortfolioVector;
typedef PortfolioVector::iterator PortfolioVecIter;

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

	void AddPortfolio(CPortfolio* pPortfolio);

	void RemovePortfolio(const boost::uuids::uuid& pid);

	int GetPortfolioCount() { return m_portfolioVec.size(); }

	bool Portfolio_OpenPosition(const boost::uuids::uuid& pid);
	bool Portfolio_ClosePosition(const boost::uuids::uuid& pid);

	CPortfolio* GetPortfolio(const boost::uuids::uuid& pid);

	void Buy()
	{
		m_tradeAgent.Buy();
	}

	virtual void OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///��¼������Ӧ
	void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID)
	{
		if(m_pClient != NULL)
		{
			m_pClient->OnRegisterResult(succ, msg);
		}

		if(succ){
			m_orderRefID = initOrderRefID;
		}
		else{
			SetCurrentClient(NULL);
		}
	}

	///����¼��������Ӧ
	void OnRspOrderInsert(bool succ, std::string& msg, protoc::Order* order);

	///�ɽ�֪ͨ
	void OnRtnTrade(protoc::Trade* pTrade);

	///Ͷ���߽�����ȷ����Ӧ
	void OnRspSettlementInfoConfirm(){}

	///�����ѯ��Լ��Ӧ
	void OnRspQryInstrument(){}

	///�����ѯ�ʽ��˻���Ӧ
	void OnRspQryTradingAccount(){}

	///�����ѯͶ���ֲ߳���Ӧ
	void OnRspQryInvestorPosition(){}

	

	///��������������Ӧ
	void OnRspOrderAction(){}


	///����֪ͨ
	void OnRtnOrder(){}

	
	//////////////////////////////////////////////////////////////////////////
private:

	const char* NextOrderRef();

	PortfolioVecIter FindPortfolio(const boost::uuids::uuid& pid);

	CTradeAgent	m_tradeAgent;
	
	ClientBase* m_pClient;

	PortfolioVector m_portfolioVec;

	int m_orderRefID;
	boost::mutex m_mutOrderRef;
	TThostFtdcOrderRefType ORDER_REF_BUF;
};

