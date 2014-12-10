#pragma once

#include "FemasAPI/USTPFtdcTraderApi.h"
#include "TradeAgentCallback.h"
#include "SyncRequest.h"
#include "SymbolInfo.h"
#include "entity/quote.pb.h"

#ifdef FAKE_DEAL
#include "FakeDealer.h"
#endif

#include <boost/tuple/tuple.hpp>
#include <boost/date_time.hpp>
#include <boost/atomic.hpp>

class CTradeAgent : public CUstpFtdcTraderSpi
{
public:
	CTradeAgent(void);
	~CTradeAgent(void);

	boost::tuple<bool, string> Login(const string& frontAddr, const string& brokerId, const string& userId, const string& password);
	void Logout();

	void SetCallbackHanlder(CTradeAgentCallback* pCallback);

	bool SubmitOrder(trade::InputOrder* pInputOrder);
	bool SubmitOrder(CUstpFtdcInputOrderField& inputOrderField);
	bool SubmitOrderAction(trade::InputOrderAction* pInputOrderAction);

	void QueryAccount();
	void QueryOrders(const string& symbol);
	void QueryPositions();
	void QueryPositionDetails(const string& symbol);

	bool QuerySymbol(const string& symbol, entity::Quote** ppQuote);
	bool QuerySymbolAsync(const string& symbol, int nReqestId);
	bool QuerySymbolInfo(const string& symbol, CSymbolInfo** ppSymbolInfo);
	bool QuerySymbolInfoAsync( CSymbolInfo* pSymbolInfo, int nReqestId );

	const string& BrokerId(){ return m_brokerId; }
	const string& InvestorId(){ return m_investorId; }
	const boost::gregorian::date& TradingDay(){ return m_tradingDay; }

	//////////////////////////////////////////////////////////////////////////
	// Response trading related api

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected(int nReason);

	///��¼������Ӧ
	virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ͷ�����ʽ��˻���ѯӦ��
	virtual void OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ͷ���ֲֲ߳�ѯӦ��
	virtual void OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����¼��Ӧ��
	virtual void OnRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������Ӧ��
	virtual void OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	virtual void OnHeartBeatWarning(int nTimeLapse){}

	///�ɽ��ر�
	virtual void OnRtnTrade(CUstpFtdcTradeField *pTrade);

	///�����ر�
	virtual void OnRtnOrder(CUstpFtdcOrderField *pOrder);

	///����¼�����ر�
	virtual void OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo) {};

	///������������ر�
	virtual void OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo) {};
	
	//////////////////////////////////////////////////////////////////////////

private:

	void RunTradingFunc(string address);
	void Login();
	int RequestIDIncrement() { return ++m_iRequestID; }
	bool IsMyOrder(CUstpFtdcOrderField *pOrder)
	{ 
		//return pOrder->FrontID == FRONT_ID && pOrder->SessionID == SESSION_ID;
		return true;
	}

	string m_brokerId;
	string m_investorId;
	string m_password;

	// ������
	boost::atomic<int> m_iRequestID;

	// �Ự����
	//TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
	//TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
	int m_maxOrderRef;					//��������
	boost::gregorian::date m_tradingDay;
	
	bool m_isConnected;
	bool m_isLogged;
	bool m_isConfirmed;
	string m_loginErr;

	boost::thread m_thTrading;
	bool m_isWorking;
	CUstpFtdcTraderApi* m_pUserApi;

	boost::mutex m_mutConnecting;
	boost::condition_variable m_condConnecting;
	boost::mutex m_mutLogin;
	boost::condition_variable m_condLogin;
	boost::mutex m_mutConfirm;
	boost::condition_variable m_condConfirm;

	CSyncRequestFactory<CSymbolInfo> m_symbInfoReqFactory;

	CTradeAgentCallback* m_orderProcessor;
#ifdef FAKE_DEAL
	CFakeDealer m_fakeDealer;
#endif

};

