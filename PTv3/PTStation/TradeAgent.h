#pragma once

#include "ThostTraderApi/ThostFtdcTraderApi.h"
#include "TradeAgentCallback.h"

#include <boost/tuple/tuple.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/atomic.hpp>

class CTradeAgent : public CThostFtdcTraderSpi
{
public:
	CTradeAgent(void);
	~CTradeAgent(void);

	boost::tuple<bool, string> Login(const string& frontAddr, const string& brokerId, const string& userId, const string& password);
	void Logout();

	void SetCallbackHanlder(CTradeAgentCallback* pCallback);

	//////////////////////////////////////////////////////////////////////////
	// Response trading related api

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected(int nReason);

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){}

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){}

	///�����ѯͶ���ֲ߳���ϸ��Ӧ
	virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){}

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){}

	///��������������Ӧ
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){}

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){}

	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	virtual void OnHeartBeatWarning(int nTimeLapse){}

	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder){}

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade){}

	///�����ѯ������Ӧ
	virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){}

	//////////////////////////////////////////////////////////////////////////

private:

	void RunTradingFunc(string address);
	void Login();
	void ReqSettlementInfoConfirm();
	int RequestIDIncrement() { return ++m_iRequestID; }

	string m_brokerId;
	string m_investorId;
	string m_password;

	// ������
	boost::atomic<int> m_iRequestID;

	// �Ự����
	TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
	TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
	int m_maxOrderRef;					//��������
	boost::gregorian::date m_tradingDay;
	
	bool m_isConnected;
	bool m_isLogged;
	bool m_isConfirmed;
	string m_loginErr;

	boost::thread m_thTrading;
	bool m_isWorking;
	CThostFtdcTraderApi* m_pUserApi;

	boost::mutex m_mutConnecting;
	boost::condition_variable m_condConnecting;
	boost::mutex m_mutLogin;
	boost::condition_variable m_condLogin;
	boost::mutex m_mutConfirm;
	boost::condition_variable m_condConfirm;

	CTradeAgentCallback* m_orderProcessor;
};

