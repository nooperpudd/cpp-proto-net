#pragma once
#include "ThostTraderApi/ThostFtdcMdApi.h"

#include <boost/atomic.hpp>

class CMarketDataConnection : public CThostFtdcMdSpi
{
public:
	CMarketDataConnection();
	~CMarketDataConnection();

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

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
	virtual void OnHeartBeatWarning(int nTimeLapse){}

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///��������Ӧ��
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///ȡ����������Ӧ��
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);

	bool Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password);
	void Logout();
	bool IsConnected() const
	{ return m_isConnected; }

	void SubscribeMarketData(char** symbolArr, int symCount);

	void UnsubscribeMarketData(char** symbolArr, int symCount);

private:
	enum { CONNECT_TIMEOUT_SECONDS = 3 };

	static log4cpp::Category& logger;
	static bool IsErrorRspInfo(CThostFtdcRspInfoField* pRspInfo);
	void RunThreadFunc(string address);
	void Login();

	int RequestIDIncrement() { return ++m_iRequestID; }

	// memeber variables
	CThostFtdcMdApi* m_pUserApi;
	boost::thread m_thQuoting;
	bool m_isWorking;

	boost::atomic<int> m_iRequestID;

	bool m_isConnected;
	bool m_isLoggedIn;
	string m_loginErr;

	string m_brokerId;
	string m_investorId;
	string m_userId;
	string m_password;

	boost::mutex m_mutConnecting;
	boost::condition_variable m_condConnecting;
	boost::mutex m_mutLogin;
	boost::condition_variable m_condLogin;
	boost::gregorian::date m_tradingDay;
	boost::mutex m_mutex;
};

