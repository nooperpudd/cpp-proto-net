#pragma once

#include <boost/atomic.hpp>

#include "ThostTraderApi/ThostFtdcTraderApi.h"

class CTradingConnection : public CThostFtdcTraderSpi
{
public:
	CTradingConnection();
	~CTradingConnection();

	//////////////////////////////////////////////////////////////////////////
	// Response trading related api

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	virtual void OnFrontDisconnected(int nReason);

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	//////////////////////////////////////////////////////////////////////////

	bool Login(const string& frontAddr, const string& brokerId, const string& investorId, const string& userId, const string& password);
	void Logout();
	bool IsConnected() const
	{
		return m_isConnected;
	}

	const string& GetErrorMsg() const { return m_loginErr; }
	const boost::gregorian::date& GetTradingDay() const { return m_tradingDay; }

private:
	static log4cpp::Category& logger;
	static bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

	void RunThreadFunc(string address);
	void Login();
	void ReqSettlementInfoConfirm();
	
	void FakeLogin();

	int RequestIDIncrement() { return ++m_iRequestID; }

	enum{ CONNECT_TIMEOUT_SECONDS = 3};

	// memeber variables
	CThostFtdcTraderApi* m_pUserApi;
	boost::thread m_thTrading;
	bool m_isWorking;

	bool m_isConnected;
	bool m_isLoggedIn;
	bool m_isConfirmed;
	string m_loginErr;

	string m_brokerId;
	string m_investorId;
	string m_userId;
	string m_password;

	string FRONT_ID;
	string SESSION_ID;
	int m_maxOrderRef;
	boost::gregorian::date m_tradingDay;

	boost::atomic<int> m_iRequestID;

	boost::mutex m_mutConnecting;
	boost::condition_variable m_condConnecting;
	boost::mutex m_mutLogin;
	boost::condition_variable m_condLogin;
	boost::mutex m_mutConfirm;
	boost::condition_variable m_condConfirm;
};

