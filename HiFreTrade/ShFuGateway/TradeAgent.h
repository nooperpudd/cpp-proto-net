#pragma once

#include "ThostFtdcTraderApi.h"
#include "InputOrder.h"
#include "InputOrderAction.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

class COrderProcessor;

class CTradeAgent : public CThostFtdcTraderSpi
{
public:
	CTradeAgent(void);
	~CTradeAgent(void);

	void Initialize(COrderProcessor* pOrderProcessor)
	{
		m_pOrderProcessor = pOrderProcessor;
	}

	bool Login(const char* brokerId, const char* userId, const char* password);
	void Logout();

	bool SubmitOrder( const std::vector< boost::shared_ptr<CInputOrder> >& orders );
	bool SubmitOrder( CInputOrder* pOrder );

	bool SubmitOrderAction( CInputOrderAction* pOrderAction );

	//////////////////////////////////////////////////////////////////////////
	// Response trading related api

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������������Ӧ
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected(int nReason);

	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	//////////////////////////////////////////////////////////////////////////

private:
	int RequestIDIncrement(){ return ++m_iRequestID; }
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);
	void ReqSettlementInfoConfirm();

	CThostFtdcTraderApi* m_pUserApi;
	boost::thread m_thTrading;

	std::string m_brokerId;
	std::string m_userId;
	std::string m_password;

	bool m_isConnected;
	// ������
	int m_iRequestID;

	boost::mutex m_mutex;
	boost::condition_variable m_condLogout;

	// �Ự����
	TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
	TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
	TThostFtdcOrderRefType	ORDER_REF;	//��������

	int m_maxOrderRef;

	COrderProcessor* m_pOrderProcessor;

};

