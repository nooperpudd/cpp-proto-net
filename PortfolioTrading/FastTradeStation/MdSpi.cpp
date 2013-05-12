#include "MdSpi.h"
#include "QS_Configuration.h"

#ifndef WIN32
#define strcpy_s strcpy
#define _strdup strdup
#endif

extern CQSConfiguration qsConfig;

CMdSpi::CMdSpi(CThostFtdcMdApi* pUserApi)
	: m_pUserApi(pUserApi)
	, m_iRequestId(0)
{
}

CMdSpi::~CMdSpi(void)
{
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
	int iResult = m_pUserApi->ReqUserLogin(&req, ++m_iRequestId);
	cout << "--->>> �����û���¼����: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///��ȡ��ǰ������
		cout << "--->>> ��ȡ��ǰ������ = " << m_pUserApi->GetTradingDay() << endl;
		// ����������
		SubscribeMarketData();	
	}
}

// Following is only for test
char *ppInstrumentID[] = { "IF1305" };			// ���鶩���б�
int iInstrumentID = 1;	

void CMdSpi::SubscribeMarketData()
{
	int iResult = m_pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cout << "--->>> �������鶩������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "OnRspSubMarketData" << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "OnRspUnSubMarketData" << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	cout << "OnRtnDepthMarketData" << endl;
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}
