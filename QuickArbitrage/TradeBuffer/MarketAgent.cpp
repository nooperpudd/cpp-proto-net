#include "StdAfx.h"
#include "MarketAgent.h"

// ���ò���
char FRONT_ADDR[] = "tcp://asp-sim2-md1.financial-trading-platform.com:26213";		// ǰ�õ�ַ
TThostFtdcBrokerIDType	BROKER_ID = "2030";				// ���͹�˾����
TThostFtdcInvestorIDType INVESTOR_ID = "00092";			// Ͷ���ߴ���
TThostFtdcPasswordType  PASSWORD = "888888";			// �û�����
char *ppInstrumentID[] = {"cu1010", "cu1009"};			// ���鶩���б�
int iInstrumentID = 2;									// ���鶩������

// ������
int iRequestID = 0;


CMarketAgent::CMarketAgent(void):
	m_pUserApi(NULL)
{
}

CMarketAgent::~CMarketAgent(void)
{
	if(m_pUserApi != NULL)
		m_pUserApi->Release();
}

void CMarketAgent::Startup()
{
	// ��ʼ��UserApi
	m_pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// ����UserApi
	//CThostFtdcMdSpi* pUserSpi = new CMdSpi();
	m_pUserApi->RegisterSpi(&m_mktResponse);						// ע���¼���
	m_pUserApi->RegisterFront(FRONT_ADDR);					// connect
	m_pUserApi->Init();
}
