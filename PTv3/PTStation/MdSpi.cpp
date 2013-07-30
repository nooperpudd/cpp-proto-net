#include "StdAfx.h"
#include "MdSpi.h"
#include "QS_Configuration.h"
#include "QuoteProxy.h"

#include <boost/interprocess/shared_memory_object.hpp>  
#include <boost/interprocess/mapped_region.hpp>  
#include <boost/interprocess/sync/scoped_lock.hpp> 

#ifndef WIN32
#define strcpy_s strcpy
#define _strdup strdup
#endif

using namespace boost::interprocess;

extern CQSConfiguration qsConfig;

CMdSpi::CMdSpi(CQuoteProxy* pQuoteProxy)
	: m_pQuoteProxy(pQuoteProxy)
	, m_loginWaiter(pQuoteProxy)
	, m_iRequestId(0)
{
	// Wait Login success for 10 seconds 
	m_loginWaiter.BeginWait(10);
}

CMdSpi::~CMdSpi(void)
{
	cout << "CMdSpi destructing..." << endl;
	m_pQuoteProxy = NULL;
}

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cout << "--->>> " << "OnHeartBeatWarning" << endl;
	cout << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	cout << "--->>> " << "OnFrontConnected" << endl;
	///�û���¼����
	ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, qsConfig.BrokerId().c_str());
	strcpy_s(req.UserID, qsConfig.Username().c_str());
	strcpy_s(req.Password, qsConfig.Password().c_str());
	int iResult = m_pQuoteProxy->MdApi()->ReqUserLogin(&req, ++m_iRequestId);
	cout << "--->>> �����û���¼����: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		m_loginWaiter.Cancel();

		///��ȡ��ǰ������
		cout << "--->>> ��ȡ��ǰ������ = " << m_pQuoteProxy->MdApi()->GetTradingDay() << endl;
		m_pQuoteProxy->GetReady();
	}
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "OnRspSubMarketData - " << pSpecificInstrument->InstrumentID << 
		(pRspInfo->ErrorID == 0 ? " Succeeded" : " Failed") << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "OnRspUnSubMarketData - " << pSpecificInstrument->InstrumentID << 
		(pRspInfo->ErrorID == 0 ? " Succeeded" : " Failed") << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	//cout << "CMdSpi::OnRtnDepthMarketData : " << pDepthMarketData->InstrumentID << ", "
	//	<< pDepthMarketData->LastPrice << ", "
	//	<< pDepthMarketData->UpdateTime << ", "
	//	<< pDepthMarketData->UpdateMillisec << endl;
	m_pQuoteProxy->OnQuoteReceived(pDepthMarketData);
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}
