#include "FakeDealer.h"

#include <boost/date_time.hpp>

#ifndef WIN32
#define strcpy_s strcpy
#endif

void CopyCommonField(CThostFtdcInputOrderField * pInputOrder, CThostFtdcOrderField* pRtnOrder);


CFakeDealer::CFakeDealer(void)
{
	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	m_tradingDay = boost::gregorian::to_iso_string(d);

	m_msgPump.Init(boost::bind(&CFakeDealer::DispatchMsg, this, _1));
	m_msgPump.Start();
}

CFakeDealer::~CFakeDealer(void)
{
	m_msgPump.Stop();
}

int CFakeDealer::ReqOrderInsert( CThostFtdcInputOrderField *pInputOrder, int nRequestID )
{
	FullFillOrder(pInputOrder, nRequestID);
	return 0;
}

int CFakeDealer::ReqOrderAction( CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID )
{
	return 0;
}

void CFakeDealer::DispatchMsg( FakeMsgPtr& fakeMsg )
{
	FakeMsgType msgType = fakeMsg->MsgType();
	if(msgType == MSG_RTN_ORDER)
	{
		CFakeRtnOrder* fakeOrder = dynamic_cast<CFakeRtnOrder*>(fakeMsg.get());
		if(fakeOrder != NULL)
		{
			m_funcRtnOrder(fakeOrder->Msg());
		}
	}
	else if(msgType == MSG_RSP_ORDER_INSERT)
	{
		CFakeRspOrderAction* fakeOrderAct = dynamic_cast<CFakeRspOrderAction*>(fakeMsg.get());
		if(fakeOrderAct != NULL)
		{
			CThostFtdcRspInfoField rspInfo;
			memset(&rspInfo, 0, sizeof(rspInfo));
			int req = 0;
			m_funcRspOrderAction(fakeOrderAct->Msg(), &rspInfo, req, true);
		}
	}
	else if(msgType == MSG_RSP_ORDER_ACTION)
	{
		CFakeRspOrderInsert* fakeOrderIns = dynamic_cast<CFakeRspOrderInsert*>(fakeMsg.get());
		if(fakeOrderIns != NULL)
		{
			CThostFtdcRspInfoField rspInfo;
			memset(&rspInfo, 0, sizeof(rspInfo));
			int req = 0;

			m_funcRspOrderInsert(fakeOrderIns->Msg(), &rspInfo, req, true);
		}
	}
}

CFakeRtnOrder* CFakeDealer::GetAcceptOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID )
{
	CFakeRtnOrder* pFakeOrder = CreateOrderTemplate(pInputOrder, nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();

	pOrdField->OrderSubmitStatus = THOST_FTDC_OSS_InsertSubmitted;
	pOrdField->OrderStatus = THOST_FTDC_OST_Unknown;
	pOrdField->VolumeTraded = 0;
	pOrdField->VolumeTotal = pInputOrder->VolumeTotalOriginal;
	strcpy_s(pOrdField->StatusMsg, "�������ύ");
	
	return pFakeOrder;
}

CFakeRtnOrder* CFakeDealer::GetPendingOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId)
{
	CFakeRtnOrder* pFakeOrder = CreateOrderTemplate(pInputOrder, nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();

	sprintf(pOrdField->OrderSysID, "%d", orderSysId);
	
	pOrdField->OrderSubmitStatus = THOST_FTDC_OSS_Accepted;
	pOrdField->OrderStatus = THOST_FTDC_OST_NoTradeQueueing;
	pOrdField->VolumeTraded = 0;
	pOrdField->VolumeTotal = pInputOrder->VolumeTotalOriginal;
	strcpy_s(pOrdField->StatusMsg, "δ�ɽ�");

	return pFakeOrder;
}

CFakeRtnOrder* CFakeDealer::GetFilledOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID, int orderSysId )
{
	CFakeRtnOrder* pFakeOrder = CreateOrderTemplate(pInputOrder, nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();
	sprintf(pOrdField->OrderSysID, "%d", orderSysId);

	pOrdField->OrderSubmitStatus = THOST_FTDC_OSS_Accepted;
	pOrdField->OrderStatus = THOST_FTDC_OST_AllTraded;
	pOrdField->VolumeTraded = pInputOrder->VolumeTotalOriginal;
	pOrdField->VolumeTotal = pInputOrder->VolumeTotalOriginal;
	strcpy_s(pOrdField->StatusMsg, "ȫ���ɽ�");

	return pFakeOrder;
}

void CFakeDealer::FullFillOrder( CThostFtdcInputOrderField * pInputOrder, int nRequestID )
{
	FakeMsgPtr msgAccept(GetAcceptOrder(pInputOrder, nRequestID));
	m_msgPump.Enqueue(msgAccept);

	int orderSysId = ++m_orderNum;

	FakeMsgPtr msgPending(GetPendingOrder(pInputOrder, nRequestID, orderSysId));
	m_msgPump.Enqueue(msgPending);

	FakeMsgPtr msgPending2(GetPendingOrder(pInputOrder, nRequestID, orderSysId));
	m_msgPump.Enqueue(msgPending2);

	FakeMsgPtr msgFilled(GetFilledOrder(pInputOrder, nRequestID, orderSysId));
	m_msgPump.Enqueue(msgFilled);
}

void CFakeDealer::SetDateField( CThostFtdcOrderField* pRtnOrder )
{
	strcpy_s(pRtnOrder->InsertDate, m_tradingDay.c_str());
	strcpy_s(pRtnOrder->TradingDay, m_tradingDay.c_str());
}

CFakeRtnOrder* CFakeDealer::CreateOrderTemplate( CThostFtdcInputOrderField * pInputOrder, int nRequestID )
{
	CFakeRtnOrder* pFakeOrder = new CFakeRtnOrder(nRequestID);
	CThostFtdcOrderField* pOrdField = pFakeOrder->Msg();
	CopyCommonField(pInputOrder, pOrdField);
	SetDateField(pOrdField);
	return pFakeOrder;
}

void CopyCommonField(CThostFtdcInputOrderField * pInputOrder, CThostFtdcOrderField* pRtnOrder)
{
	strcpy_s(pRtnOrder->BrokerID, pInputOrder->BrokerID);
	///Ͷ���ߴ���
	strcpy_s(pRtnOrder->InvestorID, pInputOrder->InvestorID);
	///��Լ����
	strcpy_s(pRtnOrder->InstrumentID, pInputOrder->InstrumentID);
	///��������
	strcpy_s(pRtnOrder->OrderRef, pInputOrder->OrderRef);
	///�û�����
	strcpy_s(pRtnOrder->UserID, pInputOrder->UserID);
	///�����۸�����
	pRtnOrder->OrderPriceType = pInputOrder->OrderPriceType;
	///��������
	pRtnOrder->Direction = pInputOrder->Direction;
	///��Ͽ�ƽ��־
	strcpy_s(pRtnOrder->CombOffsetFlag, pInputOrder->CombOffsetFlag);
	///���Ͷ���ױ���־
	strcpy_s(pRtnOrder->CombHedgeFlag, pInputOrder->CombHedgeFlag);
	///�۸�
	pRtnOrder->LimitPrice = pInputOrder->LimitPrice;
	///����
	pRtnOrder->VolumeTotalOriginal = pInputOrder->VolumeTotalOriginal;
	///��Ч������
	pRtnOrder->TimeCondition = pInputOrder->TimeCondition;
	///GTD����
	strcpy_s(pRtnOrder->GTDDate, pInputOrder->GTDDate);
	///�ɽ�������
	pRtnOrder->VolumeCondition = pInputOrder->VolumeCondition;
	///��С�ɽ���
	pRtnOrder->MinVolume = pInputOrder->MinVolume;
	///��������
	pRtnOrder->ContingentCondition = pInputOrder->ContingentCondition;
	///ֹ���
	pRtnOrder->StopPrice = pInputOrder->StopPrice;
	///ǿƽԭ��
	pRtnOrder->ForceCloseReason = pInputOrder->ForceCloseReason;
	///�Զ������־
	pRtnOrder->IsAutoSuspend = pInputOrder->IsAutoSuspend;
	///ҵ��Ԫ
	strcpy_s(pRtnOrder->BusinessUnit, pInputOrder->BusinessUnit);
	///������
	pRtnOrder->RequestID = pRtnOrder->RequestID;
	///�û�ǿ����־
	pRtnOrder->UserForceClose = pRtnOrder->UserForceClose;
	///��������־
	pRtnOrder->IsSwapOrder = pRtnOrder->IsSwapOrder;
}
