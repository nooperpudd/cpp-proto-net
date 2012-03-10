#pragma once

#include "ThostFtdcMdApi.h"
#include "QuoteListener.h"
#include "MarketAgentCallback.h"

#include <vector>
#include <string>
#include <boost/thread.hpp>

class CMarketAgent : public CThostFtdcMdSpi
{
public:
	CMarketAgent(void);
	~CMarketAgent(void);

	bool Connect();
	void Disconnect();
	bool Login(const char* brokerID, const char* userID, const char* password);
	void Logout( const char* brokerID, const char* userID );

	bool SubscribesQuotes( std::vector<std::string>& subscribeArr );
	bool UnSubscribesQuotes( std::vector<std::string>& unSubscribeArr );

	void SetCallbackHanlder(CMarketAgentCallback* pCallback){ m_pCallback = pCallback; }

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

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


	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������Ӧ��
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ȡ����������Ӧ��
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

private:
	int RequestIDIncrement();

	CThostFtdcMdApi* m_pUserApi;

	CMarketAgentCallback* m_pCallback;
	
	bool m_bIsConnected;
	// ������
	int m_iRequestID;

	boost::condition_variable m_condConnectDone;
	boost::mutex m_mutex;
	boost::thread m_thQuoting;
};

