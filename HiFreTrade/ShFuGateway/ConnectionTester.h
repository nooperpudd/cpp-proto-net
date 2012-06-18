#pragma once

#include "ThostFtdcMdApi.h"

#include <string>
#include <boost/thread.hpp>

class CConnectionTester : public CThostFtdcMdSpi
{
public:
	CConnectionTester(void);
	~CConnectionTester(void);

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

	bool Validate(const std::string& address, int* delay);

private:

	void ConnectMarketFunc(char* pAddress);

	CThostFtdcMdApi* m_pUserApi;
	boost::thread m_thConnect;
	boost::condition_variable m_condConnectDone;
	boost::mutex m_mutex;
};

