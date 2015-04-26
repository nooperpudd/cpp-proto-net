#pragma once

#include "BufferRunner.h"
#include "ThostTraderApi/ThostFtdcTraderApi.h"

#include <sstream>
#include <boost/date_time.hpp>
#include <boost/atomic.hpp>
#include <boost/unordered_map.hpp>

using namespace std;

typedef boost::function< void(CThostFtdcOrderField *pOrder) > RtnOrderFunc;
typedef boost::function< void(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )> RspOrderInsertFunc;
typedef boost::function< void(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)> RspOrderActionFunc;

enum FakeMsgType { MSG_RTN_ORDER, MSG_RSP_ORDER_INSERT, MSG_RSP_ORDER_ACTION };

class CFakeMessage
{
public:
	virtual ~CFakeMessage(){}
	virtual FakeMsgType MsgType() = 0;
};

template<typename T>
class CFakeOrderMessage : public CFakeMessage
{
public:
	CFakeOrderMessage(int nReq):m_nReq(nReq)
	{
		memset(&m_msg, 0, sizeof(m_msg));
	}
	virtual ~CFakeOrderMessage(){}
	T* Msg(){ return &m_msg; }
	int RequestId() { return m_nReq; }
protected:
	T m_msg;
	int m_nReq;
};

class CFakeRtnOrder : public CFakeOrderMessage<CThostFtdcOrderField>
{
public:
	CFakeRtnOrder(int nReq):CFakeOrderMessage<CThostFtdcOrderField>(nReq){}
	FakeMsgType MsgType() { return MSG_RTN_ORDER; }
};
class CFakeRspOrderInsert : public CFakeOrderMessage<CThostFtdcInputOrderField>
{
public:
	CFakeRspOrderInsert(int nReq):CFakeOrderMessage<CThostFtdcInputOrderField>(nReq){}
	FakeMsgType MsgType() { return MSG_RSP_ORDER_INSERT; }
};
class CFakeRspOrderAction : public CFakeOrderMessage<CThostFtdcInputOrderActionField>
{
public:
	CFakeRspOrderAction(int nReq):CFakeOrderMessage<CThostFtdcInputOrderActionField>(nReq){}
	FakeMsgType MsgType() { return MSG_RSP_ORDER_ACTION; }
};

typedef boost::shared_ptr<CFakeMessage> FakeMsgPtr;

typedef boost::shared_ptr<CThostFtdcInputOrderField> InputOrderPtr;

class CFakeDealer
{
public:
	CFakeDealer(void);
	~CFakeDealer(void);

	void Init(RtnOrderFunc funcRtnOrd, RspOrderInsertFunc funcRspOrdIns, RspOrderActionFunc funcRspOrdAct)
	{
		m_funcRtnOrder = funcRtnOrd;
		m_funcRspOrderInsert = funcRspOrdIns;
		m_funcRspOrderAction = funcRspOrdAct;
	}

	int ReqOrderInsert(CThostFtdcInputOrderField *pInputOrder, int nRequestID);
	int ReqOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID);

	void DispatchMsg(FakeMsgPtr& fakeMsg);

	void SetSessionParams(int frontId, int sessionId)
	{
		FRONT_ID = frontId;
		SESSION_ID = sessionId;
	}

private:
	CFakeRtnOrder* CreateOrderTemplate(CThostFtdcInputOrderField * pInputOrder, int nRequestID);

	CFakeRtnOrder* GetAcceptOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID);
	CFakeRtnOrder* GetPendingOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId, int amount = 0);
	CFakeRtnOrder* GetFilledOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId);
	CFakeRtnOrder* GetPartiallyFilledOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId, int amount);
	CFakeRtnOrder* GetCanceledOrder(CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId, int amount = 0);

	void FullFillOrder(boost::shared_ptr<CThostFtdcInputOrderField> pInputOrder, int nRequestID);
	void PartiallyFillOrder( boost::shared_ptr<CThostFtdcInputOrderField> pInputOrder, int nRequestID );
	void PendingOrder(boost::shared_ptr<CThostFtdcInputOrderField> pInputOrder, int nRequestID);
	void CancelOrder(boost::shared_ptr<CThostFtdcInputOrderActionField> pInputOrderAction, int nRequestID);
	
	void SetDateField(CThostFtdcOrderField* pRtnOrder);
	CThostFtdcInputOrderField* CreatePendingOrder(CThostFtdcInputOrderField * pInputOrder);

	RtnOrderFunc m_funcRtnOrder;
	RspOrderInsertFunc m_funcRspOrderInsert;
	RspOrderActionFunc m_funcRspOrderAction;

	CBufferRunner<FakeMsgPtr> m_msgPump;
	boost::atomic<int> m_orderNum;

	// �Ự����
	TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
	TThostFtdcSessionIDType	SESSION_ID;	//�Ự���

	string m_tradingDay;
	
	CThostFtdcInputOrderField m_pendingInputOrder;
	boost::unordered_map<string, InputOrderPtr> m_pendingOrderMap;
	boost::mutex m_mutPendingOrder;
	int m_pendingOrdSysId;

	std::ostringstream m_timeStream;
	boost::mutex m_mutTimeFormat;

	int m_partiallyFilledAmount;
	bool m_testPartiallyFill;
};

