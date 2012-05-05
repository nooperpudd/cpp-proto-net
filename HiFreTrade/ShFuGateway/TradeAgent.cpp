#include "StdAfx.h"
#include "TradeAgent.h"
#include "LogManager.h"
#include "ReturnOrder.h"
#include "Trade.h"

#include <sstream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

#pragma comment(lib, "./ThostTraderApi/thosttraderapi.lib")

#define LOGOUT_TIMEOUT_SECOND 5
#define WAIT_SECONDS_FOR_FLOWCONTROL 1

const char* TradeAddress = "tcp://asp-sim2-front1.financial-trading-platform.com:26205";

extern CLogManager	logger;

using namespace std;

// �����ж�
bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

CTradeAgent::CTradeAgent(void):
	m_isConnected(false),
	m_maxOrderRef(-1),
	m_pUserApi(NULL)
{
}

CTradeAgent::~CTradeAgent(void)
{
}

void TradingThreadFunc(CThostFtdcTraderApi* pUserApi, const char* address)
{
	// duplicate address string and use boost.smart_ptr to manage its lifecycle
	boost::scoped_array<char> front_addr(strdup(address));

	pUserApi->RegisterFront(front_addr.get());					// connect
	pUserApi->Init();

	pUserApi->Join();
}

void CTradeAgent::Login( const std::string& brokerId, const std::string& userId, const std::string& password )
{
	try{
		// ��ʼ��UserApi

		m_brokerId = brokerId;
		m_userId = userId;
		m_password = password;

		m_pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\Td\\");			// ����UserApi
		m_pUserApi->RegisterSpi(this);						// ע���¼���
		m_pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				// ע�ṫ����
		m_pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);	

		std::ostringstream ss;
		ss << "Try to trading front (" << TradeAddress << ") ...";
		logger.Info(ss.str());

		m_thTrading = boost::thread(&TradingThreadFunc, m_pUserApi, TradeAddress);
	}
	catch(std::exception& ex)
	{
		logger.Error("Failed to log in to trading front, details is following");
		logger.Error(ex.what());
	}
	catch(...)
	{
		logger.Error("Unknown error encounted while logging in trading front");
	}

}

void CTradeAgent::OnFrontConnected()
{
	if(m_userId.length() > 0)
	{
		logger.Debug("Trading front connected, then begin logging in");

		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, m_brokerId.c_str());
		strcpy(req.UserID, m_userId.c_str());
		strcpy(req.Password, m_password.c_str());

		int iResult = m_pUserApi->ReqUserLogin(&req, RequestIDIncrement());
		std::ostringstream info;
		info << "--->>> Send request of log in trading front (Result:" << iResult << ((iResult == 0) ? "), Succeeded" : ", Failed");
		logger.Info(info.str());
	}
	else
	{
		logger.Debug("Unexpected front connected event");
	}
}

void CTradeAgent::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	std::string errorMsg;
	std::ostringstream ss;
	ss << "Login Response (ID:" << nRequestID <<")" << endl;
	if(pRspInfo->ErrorID == 0)
	{
		m_isConnected = true;

		// ����Ự����
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		m_maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		//sprintf(ORDER_REF, "%d", iNextOrderRef);

		// login succeeded
		ss << "Login succeeded." << endl;
		ss << "Trading day: " << pRspUserLogin->TradingDay << endl;
		ss << "Login Time: " << pRspUserLogin->LoginTime << endl;
		ss << "Broker ID: " << pRspUserLogin->BrokerID << endl;
		ss << "User ID: " << pRspUserLogin->UserID << endl;
		ss << "System name: " << pRspUserLogin->SystemName << endl;
		ss << "Front ID: " << pRspUserLogin->FrontID << endl;
		ss << "Session ID: " << pRspUserLogin->SessionID << endl;
		ss << "Maximum order ref: " << pRspUserLogin->MaxOrderRef << endl;
		ss << "SHFE time: " << pRspUserLogin->SHFETime << endl;
		ss << "DCE time: " << pRspUserLogin->DCETime << endl;
		ss << "CZCE time: " << pRspUserLogin->CZCETime << endl;
		ss << "FFEX time: " << pRspUserLogin->FFEXTime << endl;

		ReqSettlementInfoConfirm();
	}
	else
	{
		// login failed
		errorMsg = pRspInfo->ErrorMsg;
		ss << "Login failed due to " << pRspInfo->ErrorMsg << endl;
	}

	logger.Info(ss.str());
}

void CTradeAgent::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId.c_str());
	strcpy(req.InvestorID, m_userId.c_str());
	int iResult = m_pUserApi->ReqSettlementInfoConfirm(&req, RequestIDIncrement());

	ostringstream debugSS;
	debugSS << "--->>> ����Ͷ���߽�����ȷ��: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��");
	logger.Info(debugSS.str());
}

void CTradeAgent::OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if (bIsLast)
	{
		if(!IsErrorRspInfo(pRspInfo))
		{
			// Settlement confirm succeeded, then notify login success
			logger.Info(boost::str(boost::format("SettlementInfo Confirm Succeeded. MaxOrderRef:%d") % m_maxOrderRef));
		}
		else
		{
			string errorMsg = pRspInfo->ErrorMsg;
			logger.Info(boost::str(boost::format("SettlementInfo Confirm Failed. Error:%s") % errorMsg));
		}
	}
}

void CTradeAgent::Logout()
{
	logger.Trace("Logging out...");

	if(!m_isConnected)
		return;

	int nResult = -1;
	try{
		CThostFtdcUserLogoutField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, m_brokerId.c_str());
		strcpy(req.UserID, m_userId.c_str());

		m_brokerId.clear();
		m_userId.clear();

		{
			boost::unique_lock<boost::mutex> lock(m_mutex);

			if(m_pUserApi != NULL)
				nResult = m_pUserApi->ReqUserLogout(&req, RequestIDIncrement());

			if(nResult == 0)
			{
				logger.Info("Sending logout from trading front successfully");
				if(!m_condLogout.timed_wait(lock, boost::posix_time::seconds(LOGOUT_TIMEOUT_SECOND)))
				{
					logger.Warning("logging out from trade front time out.");
					m_isConnected = false;
				}
			}
			else
			{
				logger.Error("Sending logout from trading front failed");
			}
		}

		m_pUserApi->Release();
		m_pUserApi = NULL;
	}
	catch(...)
	{
		logger.Error("Unknown error happent while logging out from trading front");
	}
}

void CTradeAgent::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	boost::lock_guard<boost::mutex> lock(m_mutex);

	if(!m_isConnected)
		return;

	ostringstream ss;
	ss << "Log out from trading front. Response (ID:" << nRequestID <<")" << endl;
	if(pRspInfo->ErrorID == 0)
	{
		// login succeeded
		ss << "Logout succeeded." << endl;
		ss << "Broker ID: " << pUserLogout->BrokerID << endl;
		ss << "User ID: " << pUserLogout->UserID << endl;
	}
	else
	{
		// login failed
		ss << "Logout failed due to " << pRspInfo->ErrorMsg << endl;
	}

	m_isConnected = false;
	m_condLogout.notify_one();

	logger.Info(ss.str());
}

void CTradeAgent::OnRspQryInstrument( CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnRspQryTradingAccount( CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	cerr << "--->>> " << "OnRspQryTradingAccount" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		
	}
}

void CTradeAgent::OnRspQryInvestorPosition( CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnRspError( CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::OnFrontDisconnected( int nReason )
{
	if(!m_isConnected)
		return;	// disconnect already time out 

	if(nReason == 0)
	{
		logger.Info("Market normaly disconnected.");
	}
	else
	{
		string reasonTxt = "Disconnected with market due to ";
		switch (nReason)
		{
		case 0x1001:
			reasonTxt.append("Cannot read from network");
			break;
		case 0x1002:
			reasonTxt.append("Cannot write to network");
			break;
		case 0x2001:
			reasonTxt.append("Receiving heart beat time out");
			break;
		case 0x2002:
			reasonTxt.append("Sending heart beat time out");
			break;
		case 0x2003:
			reasonTxt.append("Invalid packets received");
			break;
		}
		reasonTxt.append(" (will reconnect automatically).");
		logger.Warning(reasonTxt);
	}
}

void CTradeAgent::OnHeartBeatWarning( int nTimeLapse )
{

}

bool CTradeAgent::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONT_ID) &&
		(pOrder->SessionID == SESSION_ID) &&
		(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

bool CTradeAgent::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

bool CTradeAgent::SubmitOrder( const std::vector< boost::shared_ptr<CInputOrder> >& orders )
{
	bool suc = true;

	BOOST_FOREACH(const boost::shared_ptr<CInputOrder>& ord, orders)
	{
		if(!SubmitOrder(ord.get()))
			suc = false;
	}

	return suc;
}

bool CTradeAgent::SubmitOrder( CInputOrder* pOrder )
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, m_brokerId.c_str());
	///Ͷ���ߴ���
	strcpy(req.InvestorID, m_userId.c_str());
	///��Լ����
	strcpy(req.InstrumentID, pOrder->instrumentid().c_str());
	///��������
	strcpy(req.OrderRef, pOrder->orderref().c_str());
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///�����۸�����: �޼�
	req.OrderPriceType = pOrder->orderpricetype();
	///��������: 
	req.Direction = pOrder->direction();
	///��Ͽ�ƽ��־: ����
	req.CombOffsetFlag[0] = (pOrder->comboffsetflag())[0];
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = (pOrder->combhedgeflag())[0];
	///�۸�
	req.LimitPrice = pOrder->limitprice();
	///����: 1
	req.VolumeTotalOriginal = pOrder->volumetotaloriginal();
	///��Ч������: ������Ч
	req.TimeCondition = pOrder->timecondition();
	///GTD����
	//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = pOrder->volumecondition();
	///��С�ɽ���: 1
	req.MinVolume = pOrder->minvolume();
	///��������: ����
	req.ContingentCondition = pOrder->contingentcondition();
	///ֹ���
	//	TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = pOrder->forceclosereason();
	///�Զ������־: ��
	req.IsAutoSuspend = pOrder->isautosuspend();
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	int iRequestID = RequestIDIncrement();
	req.RequestID = iRequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = pOrder->userforceclose();

	int iResult = m_pUserApi->ReqOrderInsert(&req, iRequestID);

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> ����¼������(OrdRef:" << pOrder->orderref() << ", ReqestID:" << iRequestID << "): " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��");
	logger.Debug(oss.str());
#endif

	return iResult == 0;
}

void CTradeAgent::OnRspOrderInsert( CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	ostringstream oss;
	oss << "--->>> " << "OnRspOrderInsert for order ( OrderRef: " << pInputOrder->OrderRef << " ) with RequestID: " << nRequestID <<  endl;
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		oss << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;

	logger.Info(oss.str());
}

void CTradeAgent::OnRtnOrder( CThostFtdcOrderField *pOrder )
{
	ostringstream oss;
	oss << "--->>> " << "OnRtnOrder (OrdRef:"  << pOrder->OrderRef << ") Status:" << pOrder->StatusMsg;
	logger.Info(oss.str());

	CReturnOrder* pOrd = new CReturnOrder;

	///���͹�˾����
	pOrd->set_brokerid(pOrder->BrokerID);
	///Ͷ���ߴ���
	pOrd->set_investorid(pOrder->InvestorID);
	///��Լ����
	pOrd->set_instrumentid(pOrder->InstrumentID);
	///��������
	pOrd->set_orderref(pOrder->OrderRef);
	///�û�����
	pOrd->set_userid(pOrder->UserID);
	///�����۸�����
	pOrd->set_orderpricetype(static_cast	<OrderPriceTypeType>(pOrder->OrderPriceType));
	///��������
	pOrd->set_direction(static_cast<TradeDirectionType>(pOrder->Direction));
	///��Ͽ�ƽ��־
	pOrd->set_comboffsetflag(pOrder->CombOffsetFlag);
	///���Ͷ���ױ���־
	pOrd->set_combhedgeflag(pOrder->CombHedgeFlag);
	///�۸�
	pOrd->set_limitprice(pOrder->LimitPrice);
	///����
	pOrd->set_volumetotaloriginal(pOrder->VolumeTotalOriginal);
	///��Ч������
	pOrd->set_timecondition(static_cast<TimeConditionType>(pOrder->TimeCondition));
	///GTD����
	pOrd->set_gtddate(pOrder->GTDDate);
	///�ɽ�������
	pOrd->set_volumecondition(static_cast<VolumeConditionType>(pOrder->VolumeCondition));
	///��С�ɽ���
	pOrd->set_minvolume(pOrder->MinVolume);
	///��������
	pOrd->set_contingentcondition(static_cast<:ContingentConditionType>(pOrder->ContingentCondition));
	///ֹ���
	pOrd->set_stopprice(pOrder->StopPrice);
	///ǿƽԭ��
	pOrd->set_forceclosereason(static_cast<ForceCloseReasonType>(pOrder->ForceCloseReason));
	///�Զ������־
	pOrd->set_isautosuspend(pOrder->IsAutoSuspend);
	///ҵ��Ԫ
	pOrd->set_businessunit(pOrder->BusinessUnit);
	///������
	pOrd->set_requestid(pOrder->RequestID);
	// Above fields are same as InputOrder's
	////////////////////////////////////////////////////////////////////

	///���ر������
	pOrd->set_orderlocalid(pOrder->OrderLocalID);
	///����������
	pOrd->set_exchangeid(pOrder->ExchangeID);
	///��Ա����
	pOrd->set_participantid(pOrder->ParticipantID);
	///�ͻ�����
	pOrd->set_clientid(pOrder->ClientID);
	///��Լ�ڽ������Ĵ���
	pOrd->set_exchangeinstid(pOrder->ExchangeInstID);
	///����������Ա����
	pOrd->set_traderid(pOrder->TraderID);
	///��װ���
	pOrd->set_installid(pOrder->InstallID);
	///�����ύ״̬
	pOrd->set_ordersubmitstatus(static_cast<OrderSubmitStatusType>(pOrder->OrderSubmitStatus));
	///������ʾ���
	pOrd->set_notifysequence(pOrder->NotifySequence);
	///������
	pOrd->set_tradingday(pOrder->TradingDay);
	///������
	pOrd->set_settlementid(pOrder->SettlementID);
	///�������
	pOrd->set_ordersysid(pOrder->OrderSysID);
	///������Դ
	// sometimes OrderSource could be 0 insteade of '0'
	if(pOrder->OrderSource < PARTICIPANT)
		pOrd->set_ordersource(PARTICIPANT);
	else
		pOrd->set_ordersource(static_cast<OrderSourceType>(pOrder->OrderSource));
	///����״̬
	pOrd->set_orderstatus(static_cast<OrderStatusType>(pOrder->OrderStatus));
	///��������
	// Sometimes OrderType could be 0 instead of '0'
	if(pOrder->OrderType < NORMAL_ORDER)
		pOrd->set_ordertype(NORMAL_ORDER);
	else
		pOrd->set_ordertype(static_cast<OrderTypeType>(pOrder->OrderType));
	///��ɽ�����
	pOrd->set_volumetraded(pOrder->VolumeTraded);
	///ʣ������
	pOrd->set_volumetotal(pOrder->VolumeTotal);
	///��������
	pOrd->set_insertdate(pOrder->InsertDate);
	///ί��ʱ��
	pOrd->set_inserttime(pOrder->InsertTime);
	///����ʱ��
	pOrd->set_activetime(pOrder->ActiveTime);
	///����ʱ��
	pOrd->set_suspendtime(pOrder->SuspendTime);
	///����޸�ʱ��
	pOrd->set_updatetime(pOrder->UpdateTime);
	///����ʱ��
	pOrd->set_canceltime(pOrder->CancelTime);
	///����޸Ľ���������Ա����
	pOrd->set_activetraderid(pOrder->ActiveTraderID);
	///�����Ա���
	pOrd->set_clearingpartid(pOrder->ClearingPartID);
	///���
	pOrd->set_sequenceno(pOrder->SequenceNo);
	///ǰ�ñ��
	pOrd->set_frontid(pOrder->FrontID);
	///�Ự���
	pOrd->set_sessionid(pOrder->SessionID);
	///�û��˲�Ʒ��Ϣ
	pOrd->set_userproductinfo(pOrder->UserProductInfo);
	///״̬��Ϣ
	pOrd->set_statusmsg(pOrder->StatusMsg);
	///�û�ǿ����־
	pOrd->set_userforceclose(pOrder->UserForceClose);
	///�����û�����
	pOrd->set_activeuserid(pOrder->ActiveUserID);
	///���͹�˾�������
	pOrd->set_brokerorderseq(pOrder->BrokerOrderSeq);
	///��ر���
	pOrd->set_relativeordersysid(pOrder->RelativeOrderSysID);
}

void CTradeAgent::OnRtnTrade( CThostFtdcTradeField *pTrade )
{
	ostringstream oss;
	oss << "--->>> " << "OnRtnTrade (OrdRef:"  << pTrade->OrderRef << ") TradeId:" << pTrade->TradeID;
	logger.Info(oss.str());

	CTrade* pTd = new CTrade;

	///���͹�˾����
	pTd->set_brokerid(pTrade->BrokerID);
	///Ͷ���ߴ���
	pTd->set_investorid(pTrade->InvestorID);
	///��Լ����
	pTd->set_instrumentid(pTrade->InstrumentID);
	///��������
	pTd->set_orderref(pTrade->OrderRef);
	///�û�����
	pTd->set_userid(pTrade->UserID);
	///����������
	pTd->set_exchangeid(pTrade->ExchangeID);
	///�ɽ����
	pTd->set_tradeid(pTrade->TradeID);
	///��������
	pTd->set_direction(static_cast<TradeDirectionType>(pTrade->Direction));
	///�������
	pTd->set_ordersysid(pTrade->OrderSysID);
	///��Ա����
	pTd->set_participantid(pTrade->ParticipantID);
	///�ͻ�����
	pTd->set_clientid(pTrade->ClientID);
	///���׽�ɫ
	if(pTrade->TradingRole < ER_BROKER)
	{
		pTd->set_tradingrole(ER_BROKER);
	}
	else
	{
		pTd->set_tradingrole(static_cast<TradingRoleType>(pTrade->TradingRole));
	}
	///��Լ�ڽ������Ĵ���
	pTd->set_exchangeinstid(pTrade->ExchangeInstID);
	///��ƽ��־
	pTd->set_offsetflag(static_cast<OffsetFlagType>(pTrade->OffsetFlag));
	///Ͷ���ױ���־
	pTd->set_hedgeflag(static_cast<HedgeFlagType>(pTrade->HedgeFlag));
	///�۸�
	pTd->set_price(pTrade->Price);
	///����
	pTd->set_volume(pTrade->Volume);
	///�ɽ�ʱ��
	pTd->set_tradedate(pTrade->TradeDate);
	///�ɽ�ʱ��
	pTd->set_tradetime(pTrade->TradeTime);
	///�ɽ�����
	if(pTrade->TradeType < TRDT_COMMON)
		pTd->set_tradetype(TRDT_COMMON);
	else
		pTd->set_tradetype(static_cast<TradeTypeType>(pTrade->TradeType));
	///�ɽ�����Դ
	if(pTrade->PriceSource < PSRC_LAST_PRICE)
		pTd->set_pricesource(PSRC_LAST_PRICE);
	else
		pTd->set_pricesource(static_cast<PriceSourceType>(pTrade->PriceSource));
	///����������Ա����
	pTd->set_traderid(pTrade->TraderID);
	///���ر������
	pTd->set_orderlocalid(pTrade->OrderLocalID);
	///�����Ա���
	pTd->set_clearingpartid(pTrade->ClearingPartID);
	///ҵ��Ԫ
	pTd->set_businessunit(pTrade->BusinessUnit);
	///���
	pTd->set_sequenceno(pTrade->SequenceNo);
	///������
	pTd->set_tradingday(pTrade->TradingDay);
	///������
	pTd->set_settlementid(pTrade->SettlementID);
	///���͹�˾�������
	pTd->set_brokerorderseq(pTrade->BrokerOrderSeq);

	//m_pOrderMgr->OnRtnTrade(pTd);
}

bool CTradeAgent::SubmitOrderAction( CInputOrderAction* pOrderAction )
{
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, m_brokerId.c_str());
	///Ͷ���ߴ���
	strcpy(req.InvestorID, m_userId.c_str());
	///������������
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///��������
	strcpy(req.OrderRef, pOrderAction->orderref().c_str());
	///������
	int iRequestID = RequestIDIncrement();
	req.RequestID = iRequestID;
	///ǰ�ñ��
	req.FrontID = FRONT_ID;
	///�Ự���
	req.SessionID = SESSION_ID;
	///����������
	strcpy(req.ExchangeID, pOrderAction->exchangeid().c_str());
	///�������
	strcpy(req.OrderSysID, pOrderAction->ordersysid().c_str());
	///������־
	req.ActionFlag = THOST_FTDC_AF_Delete;	// Cancel order
	///�۸�
	//	TThostFtdcPriceType	LimitPrice;
	///�����仯
	//	TThostFtdcVolumeType	VolumeChange;
	///�û�����
	strcpy(req.UserID, pOrderAction->userid().c_str());
	///��Լ����
	strcpy(req.InstrumentID, pOrderAction->instrumentid().c_str());

	int iResult = m_pUserApi->ReqOrderAction(&req, iRequestID);

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> ������������ ( Canel OrdRef:" << pOrderAction->orderref() << ", ReqestID:" << iRequestID << "): " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��");
	logger.Debug(oss.str());
#endif

	return iResult == 0;
}

void CTradeAgent::OnRspOrderAction( CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	//m_pOrderMgr->OnRspOrderInsert(IsErrorRspInfo(pRspInfo), std::string(pInputOrderAction->OrderRef), std::string(pRspInfo->ErrorMsg));
}
