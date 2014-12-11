#pragma once

#include "MarketLoginWaiter.h"
#include "FemasAPI/USTPFtdcMduserApi.h"

#include <iostream>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;

class CQuoteProxy;

class CMdSpi : public CUstpFtdcMduserSpi
{
public:
	CMdSpi(CQuoteProxy* pQuoteProxy);
	~CMdSpi(void);

	///����Ӧ��
	virtual void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

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
	virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�û��˳�Ӧ��
	virtual void OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///���ĺ�Լ�������Ϣ
	virtual void OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///���ĺ�Լ�������Ϣ
	virtual void OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	// �������֪ͨ�����������������֪ͨ�ͻ���
	virtual void OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pDepthMarketData);
	

private:
	void ReqUserLogin();
	bool IsErrorRspInfo(CUstpFtdcRspInfoField *pRspInfo);

	CQuoteProxy* m_pQuoteProxy;
	CMarketLoginWaiter m_loginWaiter;

	boost::atomic<int> m_iRequestId;
	bool m_isReady;
};

