#pragma once

#include "ShmQuoteSubscribe.h"
#include "ShmQuoteFeed.h"
#include "MarketLoginWaiter.h"
#include "ThostTraderApi/ThostFtdcMdApi.h"

#include <iostream>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;

class CMdSpi : public CThostFtdcMdSpi
{
public:
	CMdSpi(CThostFtdcMdApi* pUserApi);
	~CMdSpi(void);

	int ExitCode(){ return m_exitCode; }

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(int nReason);

	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������Ӧ��
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ȡ����������Ӧ��
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

private:
	void ReqUserLogin();
	void SubscribeMarketData(char** symbolArr, int symCount);
	void UnsubscribeMarketData(char** symbolArr, int symCount);
	void OnTerminateNotified();
	// 
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

	int m_exitCode;
	CThostFtdcMdApi* m_pUserApi;
	CMarketLoginWaiter m_loginWaiter;

	boost::atomic<int> m_iRequestId;
	boost::shared_ptr<CShmQuoteSubscribeConsumer> m_quoteSubscriber;
	boost::shared_ptr<CShmQuoteFeedProducer> m_quoteFeeder;
};

