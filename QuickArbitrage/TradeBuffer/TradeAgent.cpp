#include "StdAfx.h"
#include "TradeAgent.h"
#include "Configuration.h"
#include "LogManager.h"
#include "OrderManager.h"
#include "protobuf_gen/trade.pb.h"

#include <sstream>
#include <boost/foreach.hpp>

#pragma comment(lib, "./ThostTraderApi/thosttraderapi.lib")

#define LOGOUT_TIMEOUT_SECOND 5
#define WAIT_SECONDS_FOR_FLOWCONTROL 1

extern CConfiguration config;
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

CTradeAgent::CTradeAgent(COrderManager* pOrderMgr):
	m_isConnected(false),
	m_maxOrderRef(-1),
	m_pUserApi(NULL)
{
	m_pOrderMgr = pOrderMgr;
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
		ss << "Try to trading front (" << config.GetTradeAddr() << ") ...";
		logger.Info(ss.str());

		m_thTrading = boost::thread(&TradingThreadFunc, m_pUserApi, config.GetTradeAddr());
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

		m_pOrderMgr->OnRspUserLogin(m_isConnected, errorMsg, -1);
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
	string errorMsg;
	if (bIsLast)
	{
		if(!IsErrorRspInfo(pRspInfo))
		{
			// Settlement confirm succeeded, then notify login success
			m_pOrderMgr->OnRspUserLogin(true, errorMsg, m_maxOrderRef);
		}
		else
		{
			errorMsg = pRspInfo->ErrorMsg;
			m_pOrderMgr->OnRspUserLogin(false, errorMsg, -1);
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
		boost::shared_ptr<protoc::AccountInfo> account(new protoc::AccountInfo);
		account->set_brokerid(pTradingAccount->BrokerID);
		account->set_accountid(pTradingAccount->AccountID);
		account->set_premortgage(pTradingAccount->PreMortgage);
		account->set_precredit(pTradingAccount->PreCredit);
		account->set_predeposit(pTradingAccount->PreDeposit);
		account->set_prebalance(pTradingAccount->PreBalance);
		account->set_premargin(pTradingAccount->PreMargin);
		account->set_interestbase(pTradingAccount->InterestBase);
		account->set_interest(pTradingAccount->Interest);
		account->set_deposit(pTradingAccount->Deposit);
		account->set_withdraw(pTradingAccount->Withdraw);
		account->set_frozenmargin(pTradingAccount->FrozenMargin);
		account->set_frozencash(pTradingAccount->FrozenCash);
		account->set_frozencommission(pTradingAccount->FrozenCommission);
		account->set_currmargin(pTradingAccount->CurrMargin);
		account->set_cashin(pTradingAccount->CashIn);
		account->set_commission(pTradingAccount->Commission);
		account->set_closeprofit(pTradingAccount->CloseProfit);
		account->set_positionprofit(pTradingAccount->PositionProfit);
		account->set_balance(pTradingAccount->Balance);
		account->set_available(pTradingAccount->Available);
		account->set_withdrawquota(pTradingAccount->WithdrawQuota);
		account->set_reserve(pTradingAccount->Reserve);
		account->set_tradingday(pTradingAccount->TradingDay);
		account->set_settlementid(pTradingAccount->SettlementID);
		account->set_credit(pTradingAccount->Credit);
		account->set_mortgage(pTradingAccount->Mortgage);
		account->set_exchangemargin(pTradingAccount->ExchangeMargin);
		account->set_deliverymargin(pTradingAccount->DeliveryMargin);
		account->set_exchangedeliverymargin(pTradingAccount->ExchangeDeliveryMargin);
	}
}

void CTradeAgent::OnRspQryInvestorPosition( CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}


void CTradeAgent::OnRspOrderAction( CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
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


void CTradeAgent::Buy()
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, m_brokerId.c_str());
	///Ͷ���ߴ���
	strcpy(req.InvestorID, m_userId.c_str());
	///��Լ����
	strcpy(req.InstrumentID, "cu1206");
	///��������
	strcpy(req.OrderRef, ORDER_REF);
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///�����۸�����: �޼�
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///��������: 
	req.Direction = THOST_FTDC_D_Buy;
	///��Ͽ�ƽ��־: ����
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///�۸�
	req.LimitPrice = 61000;
	///����: 1
	req.VolumeTotalOriginal = 1;
	///��Ч������: ������Ч
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD����
	//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	req.MinVolume = 1;
	///��������: ����
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ֹ���
	//	TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	req.IsAutoSuspend = 0;
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = 0;

	int iResult = m_pUserApi->ReqOrderInsert(&req, RequestIDIncrement());
	cerr << "--->>> ����¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}

void CTradeAgent::QueryAccount()
{
	if(m_isConnected)
	{
		logger.Warning("Didn't log in trading front!!!");
		return;
	}

	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId.c_str());
	strcpy(req.InvestorID, m_userId.c_str());
	while (true)
	{
		int iResult = m_pUserApi->ReqQryTradingAccount(&req, RequestIDIncrement());
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> �����ѯ�ʽ��˻�: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
			break;
		}
		else
		{
			cerr << "--->>> �����ѯ�ʽ��˻�: "  << iResult << ", �ܵ�����" << endl;
			boost::this_thread::sleep(boost::posix_time::seconds(WAIT_SECONDS_FOR_FLOWCONTROL));
		}
	} // while
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

bool CTradeAgent::SubmitOrder( const std::vector< boost::shared_ptr<protoc::InputOrder> >& orders )
{
	bool suc = true;

	BOOST_FOREACH(const boost::shared_ptr<protoc::InputOrder>& ord, orders)
	{
		if(!SubmitOrder(ord.get()))
			suc = false;
	}

	return suc;
}

bool CTradeAgent::SubmitOrder( protoc::InputOrder* pOrder )
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, pOrder->brokerid().c_str());
	///Ͷ���ߴ���
	strcpy(req.InvestorID, pOrder->brokerid().c_str());
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
	req.CombOffsetFlag[0] = (pOrder->combhedgeflag())[0];
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
	oss << "--->>> " << "OnRspOrderInsert ( for RequestID: " << nRequestID << " )" << endl;
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		oss << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	
	logger.Info(oss.str());

	m_pOrderMgr->OnRspOrderInsert(false, std::string(pRspInfo->ErrorMsg), NULL);
}

void CTradeAgent::OnRtnOrder( CThostFtdcOrderField *pOrder )
{
	ostringstream oss;
	oss << "--->>> " << "OnRtnOrder (OrdRef:"  << pOrder->OrderRef << ") Status:" << pOrder->StatusMsg;
	logger.Info(oss.str());

	protoc::Order* pOrd = new protoc::Order;

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
	pOrd->set_orderpricetype(static_cast	<protoc::OrderPriceTypeType>(pOrder->OrderPriceType));
	///��������
	pOrd->set_direction(static_cast<protoc::TradeDirectionType>(pOrder->Direction));
	///��Ͽ�ƽ��־
	pOrd->set_comboffsetflag(pOrder->CombOffsetFlag);
	///���Ͷ���ױ���־
	pOrd->set_combhedgeflag(pOrder->CombHedgeFlag);
	///�۸�
	pOrd->set_limitprice(pOrder->LimitPrice);
	///����
	pOrd->set_volumetotaloriginal(pOrder->VolumeTotalOriginal);
	///��Ч������
	pOrd->set_timecondition(static_cast<protoc::TimeConditionType>(pOrder->TimeCondition));
	///GTD����
	pOrd->set_gtddate(pOrder->GTDDate);
	///�ɽ�������
	pOrd->set_volumecondition(static_cast<protoc::VolumeConditionType>(pOrder->VolumeCondition));
	///��С�ɽ���
	pOrd->set_minvolume(pOrder->MinVolume);
	///��������
	pOrd->set_contingentcondition(static_cast<protoc::ContingentConditionType>(pOrder->ContingentCondition));
	///ֹ���
	pOrd->set_stopprice(pOrder->StopPrice);
	///ǿƽԭ��
	pOrd->set_forceclosereason(static_cast<protoc::ForceCloseReasonType>(pOrder->ForceCloseReason));
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
	pOrd->set_ordersubmitstatus(static_cast<protoc::OrderSubmitStatusType>(pOrder->OrderSubmitStatus));
	///������ʾ���
	pOrd->set_notifysequence(pOrder->NotifySequence);
	///������
	pOrd->set_tradingday(pOrder->TradingDay);
	///������
	pOrd->set_settlementid(pOrder->SettlementID);
	///�������
	pOrd->set_ordersysid(pOrder->OrderSysID);
	///������Դ
	pOrd->set_ordersource(static_cast<protoc::OrderSourceType>(pOrder->OrderSource));
	///����״̬
	pOrd->set_orderstatus(static_cast<protoc::OrderStatusType>(pOrder->OrderStatus));
	///��������
	pOrd->set_ordertype(static_cast<protoc::OrderTypeType>(pOrder->OrderType));
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

	m_pOrderMgr->OnRspOrderInsert(true, std::string(""), pOrd);
}

void CTradeAgent::OnRtnTrade( CThostFtdcTradeField *pTrade )
{
	ostringstream oss;
	oss << "--->>> " << "OnRtnTrade (OrdRef:"  << pTrade->OrderRef << ") TradeId:" << pTrade->TradeID;
	logger.Info(oss.str());

	protoc::Trade* pTd = new protoc::Trade;

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
	pTd->set_direction(static_cast<protoc::TradeDirectionType>(pTrade->Direction));
	///�������
	pTd->set_ordersysid(pTrade->OrderSysID);
	///��Ա����
	pTd->set_participantid(pTrade->ParticipantID);
	///�ͻ�����
	pTd->set_clientid(pTrade->ClientID);
	///���׽�ɫ
	pTd->set_tradingrole(static_cast<protoc::TradingRoleType>(pTrade->TradingRole));
	///��Լ�ڽ������Ĵ���
	pTd->set_exchangeinstid(pTrade->ExchangeInstID);
	///��ƽ��־
	pTd->set_offsetflag(static_cast<protoc::OffsetFlagType>(pTrade->OffsetFlag));
	///Ͷ���ױ���־
	pTd->set_hedgeflag(static_cast<protoc::HedgeFlagType>(pTrade->HedgeFlag));
	///�۸�
	pTd->set_price(pTrade->Price);
	///����
	pTd->set_volume(pTrade->Volume);
	///�ɽ�ʱ��
	pTd->set_tradedate(pTrade->TradeDate);
	///�ɽ�ʱ��
	pTd->set_tradetime(pTrade->TradeTime);
	///�ɽ�����
	pTd->set_tradetype(static_cast<protoc::TradeTypeType>(pTrade->TradeType));
	///�ɽ�����Դ
	pTd->set_pricesource(static_cast<protoc::PriceSourceType>(pTrade->PriceSource));
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

	m_pOrderMgr->OnRtnTrade(pTd);
}
