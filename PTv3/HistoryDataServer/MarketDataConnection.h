#pragma once
#include "ThostTraderApi/ThostFtdcMdApi.h"

#include <boost/atomic.hpp>

class CMarketDataConnection : public CThostFtdcMdSpi
{
public:
	CMarketDataConnection();
	~CMarketDataConnection();

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse){}

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
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

