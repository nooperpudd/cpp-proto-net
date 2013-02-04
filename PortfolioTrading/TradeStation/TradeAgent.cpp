#include "StdAfx.h"
#include "TradeAgent.h"
#include "globalmembers.h"
#include "FileOperations.h"

#include <sstream>
#include <boost/format.hpp>

#pragma comment(lib, "./ThostTraderApi/thosttraderapi.lib")

#define CONNECT_TIMEOUT_SECONDS 15
#define DISCONNECT_TIMEOUT_SECOND 5
#define LOGIN_TIMEOUT_SECONDS 15
#define QUERY_QUOTE_RETRY_TIMES 5

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
	{
		logger.Debug(boost::str(boost::format("--->>> ErrorID=%d, ErrorMsg=%s")
			% pRspInfo->ErrorID % pRspInfo->ErrorMsg)); 
	}
	return bResult;
}

void PrintRtnOrder(CThostFtdcOrderField* pOrder)
{
	ostringstream oss;
	oss << "\n///////////////////////\n";
	oss << "��Լ����      \t" << pOrder->InstrumentID << "\n";
	oss << "��������      \t" << pOrder->OrderRef << "\n";
	oss << "�����۸�����   \t" << pOrder->OrderPriceType << "\n";	
	oss << "��������      \t" << pOrder->Direction << "\n";	
	oss << "��Ͽ�ƽ��־   \t" << pOrder->CombOffsetFlag << "\n";
	oss << "���Ͷ���ױ���־\t" << pOrder->CombHedgeFlag << "\n";
	oss << "�۸�          \t" << pOrder->LimitPrice << "\n";
	oss << "����          \t" << pOrder->VolumeTotalOriginal << "\n";
	oss << "��Ч������     \t" << pOrder->TimeCondition << "\n";
	oss << "�ɽ�������     \t" << pOrder->VolumeCondition << "\n";
	oss << "������       \t" << pOrder->RequestID << "\n";
	oss << "///////////////////////\n";

	logger.Debug(oss.str());
}

CTradeAgent::CTradeAgent(void):
m_loginSuccess(false),
m_pUserApi(NULL),
m_bIsConnected(false),
m_maxOrderRef(0),
m_iRequestID(0)
{
}

void RunTradingFunc(CThostFtdcTraderApi* pUserApi, const char* address)
{
	// duplicate address string and use boost.smart_ptr to manage its lifecycle
	boost::scoped_array<char> front_addr(_strdup(address));

	pUserApi->RegisterFront(front_addr.get());					// connect
	pUserApi->Init();

	pUserApi->Join();
}


CTradeAgent::~CTradeAgent(void)
{
}

boost::tuple<bool, string> CTradeAgent::Open( const string& address, const string& streamDir )
{
	string errMsg;
	try{
		string streamFolder = streamDir + "\\Td";
		if(!CreateFolderIfNotExists(streamFolder))
		{
			errMsg = boost::str(boost::format("Cannot create stream folder (%s) for trading") % streamFolder);
			return boost::make_tuple(false, errMsg);
		}
		streamFolder += "\\";
		// ��ʼ��UserApi
		m_pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(streamFolder.c_str());			// ����UserApi
		
		m_pUserApi->RegisterSpi(this);						// ע���¼���

		m_pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);
		m_pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);

		logger.Info(boost::str(boost::format("Try to connect trade server (%s) ...") % address));

		m_thQuoting = boost::thread(&RunTradingFunc, m_pUserApi, address.c_str());

		// wait 15 seconds for connected event
		{
			boost::unique_lock<boost::mutex> lock(m_mutex);
			if(!m_condConnectDone.timed_wait(lock, boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS)))
			{
				errMsg = "Connecting time out";
				logger.Warning(errMsg);
				return boost::make_tuple(false, errMsg);
			}

			m_bIsConnected = true;
		}
		return boost::make_tuple(true, errMsg);
	}
	catch(std::exception& ex)
	{
		errMsg = "Failed to connect to trade server, details is following\n";
		errMsg = ex.what();
		logger.Error(errMsg);
	}
	catch(...)
	{
		errMsg = "Unknown error encounted while connecting trade server";
		logger.Error(errMsg);
	}

	return boost::make_tuple(false, errMsg);
}

void CTradeAgent::OnFrontConnected()
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	logger.Info("Trade connected");
	m_condConnectDone.notify_all();
}

void CTradeAgent::Close()
{
	if(!m_bIsConnected)
		return;

	if(m_pUserApi != NULL)
	{
		//m_pUserApi->RegisterSpi(NULL);
		m_pUserApi->Release();

		{
			boost::unique_lock<boost::mutex> lock(m_mutex);

			if(m_bIsConnected)
			{
				if(!m_condConnectDone.timed_wait(lock, boost::posix_time::seconds(DISCONNECT_TIMEOUT_SECOND)))
				{
					logger.Warning("Disconnecting time out");
					m_bIsConnected = false;
				}
			}
		}
	}
}

void CTradeAgent::OnFrontDisconnected( int nReason )
{
	boost::lock_guard<boost::mutex> lock(m_mutex);

	if(!m_bIsConnected)
		return;	// disconnect already time out 

	if(nReason == 0)
	{
		logger.Info("Trade normaly disconnected.");
	}
	else
	{
		string reasonTxt = "Disconnected from trade server due to ";
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

	m_bIsConnected = false;
	m_condConnectDone.notify_all();
}

boost::tuple<bool, string> CTradeAgent::Login( const string& brokerId, const string& userId, const string& password )
{
	string traceInfo = boost::str(boost::format("Log in trade (%s, %s, %s)") 
		% brokerId.c_str() % userId.c_str() % password.c_str());
	logger.Trace(traceInfo);

	m_brokerID = brokerId;
	m_userID = userId;

	try
	{
		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy_s(req.BrokerID, brokerId.c_str());
		strcpy_s(req.UserID, userId.c_str());
		strcpy_s(req.Password, password.c_str());
		if(m_pUserApi != NULL)
		{
			boost::unique_lock<boost::mutex> lock(m_mutLogin);

			int requestId = RequestIDIncrement();
			int iResult = m_pUserApi->ReqUserLogin(&req, requestId);
			bool reqSucc = iResult == 0;
			string loginInfo = boost::str(boost::format("Sending login %s, RequestID: %d")
				% (reqSucc ? "Succeeded" : "Failed") % requestId);
			logger.Info(loginInfo);

			if(reqSucc)
			{
				if(!m_condLogin.timed_wait(lock, boost::posix_time::seconds(LOGIN_TIMEOUT_SECONDS)))
				{
					m_sLoginError = "Trade Login timeout";
					logger.Error(m_sLoginError);
				}
			}
			else
			{
				m_sLoginError = loginInfo;
			}
		}
	}
	catch (...)
	{
		m_sLoginError = "Encouter error while logging in trade server";
		logger.Error(m_sLoginError);
	}

	return boost::make_tuple(m_loginSuccess, m_sLoginError);
}

void CTradeAgent::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	boost::mutex::scoped_lock lock(m_mutLogin);

	m_loginSuccess = (pRspInfo->ErrorID == 0);
	string loginInfo = boost::str(
		boost::format(
		"Trade login response (ReqId:%d): %s") 
		% nRequestID 
		% (m_loginSuccess ? "Succeeded" : "Failed"));
	logger.Info(loginInfo);

	if(m_loginSuccess)
	{
		// ����Ự����
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		m_maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);

		ostringstream ss;
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

		logger.Info(ss.str());

		ReqSettlementInfoConfirm();
	}
	else
	{
		m_sLoginError = pRspInfo->ErrorMsg;
		logger.Error(boost::str(boost::format("Error Message:%s") % pRspInfo->ErrorMsg));
	}

	m_condLogin.notify_one();
}

void CTradeAgent::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerID.c_str());
	strcpy(req.InvestorID, m_userID.c_str());
	int iResult = m_pUserApi->ReqSettlementInfoConfirm(&req, RequestIDIncrement());

	ostringstream debugSS;
	debugSS << "--->>> ����Ͷ���߽�����ȷ��: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��");
	logger.Info(debugSS.str());
}

void CTradeAgent::OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if (bIsLast)
	{
		CRspUserLogin* pRspLogin= new CRspUserLogin;
		MessagePacketPtr packet(new CTradeMessagePacket(pRspLogin, RSP_LOGIN));
		if(!IsErrorRspInfo(pRspInfo))
		{
			// Settlement confirm succeeded, then notify login success
			pRspLogin->LoginSuccess = true;
			pRspLogin->InitOrderRef = m_maxOrderRef;
		}
		else
		{
			pRspLogin->LoginSuccess = false;
			pRspLogin->ErrorMsg = pRspInfo->ErrorMsg;
			pRspLogin->InitOrderRef = -1;
		}
		m_messagePump.Enqueue(packet);
	}
}

void CTradeAgent::Logout()
{
	logger.Trace("Trade Logging out");

	if(!m_bIsConnected || !m_loginSuccess)
		return;

	int nResult = -1;
	try{
		CThostFtdcUserLogoutField req;
		memset(&req, 0, sizeof(req));
		strcpy_s(req.BrokerID, m_brokerID.c_str());
		strcpy_s(req.UserID, m_userID.c_str());

		if(m_pUserApi != NULL)
			nResult = m_pUserApi->ReqUserLogout(&req, RequestIDIncrement());

		if(nResult == 0)
		{
			m_loginSuccess = false;
			logger.Info("Sending Trade logout successfully");
		}
		else
		{
			logger.Error("Sending Trade logout failed");
		}
	}
	catch(...)
	{
		logger.Error("Unknown error happent while Trade logging out");
	}
}

void CTradeAgent::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	logger.Info(boost::str(boost::format("Trade Logout Response (ReqID:%d)") % nRequestID));

	string logoutInfo;
	if(pRspInfo->ErrorID == 0)
	{
		logoutInfo = boost::str(boost::format("Trade Logout succeeded. BrokerId:%s, UserId:%s") 
			% pUserLogout->BrokerID % pUserLogout->UserID);
	}
	else
	{
		// login failed
		logoutInfo = boost::str(boost::format("Trade Logout failed due to %s") % pRspInfo->ErrorMsg);
	}

	logger.Info(logoutInfo);
}


void CTradeAgent::OnRspQryInstrument( CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CTradeAgent::QueryAccount()
{
	_ASSERT(!m_brokerID.empty(), "Broker Id cannot be empty");
	_ASSERT(!m_userID.empty(), "Account Id cannot be empty");

	if(m_brokerID.empty() || m_userID.empty())
		return;

	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, m_brokerID.c_str());
	strcpy_s(req.InvestorID, m_userID.c_str());
	while (true)
	{
		int iResult = m_pUserApi->ReqQryTradingAccount(&req, RequestIDIncrement());
		if (!IsFlowControl(iResult))
		{
			std::string infoText = boost::str(boost::format("Query trading account: %d, %s") % iResult % ((iResult == 0) ? "�ɹ�" : "ʧ��"));
			logger.Info(infoText);
			break;
		}
		else
		{
			logger.Warning(boost::str(boost::format("--->>> Query trading account: %d, �ܵ�����") % iResult));
			boost::this_thread::sleep(boost::posix_time::seconds(1)); 
		}
	} // while
}

void CTradeAgent::OnRspQryTradingAccount( CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	logger.Debug("--->>> OnRspQryTradingAccount");
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		trade::AccountInfo* account = new trade::AccountInfo;
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
		
		MessagePacketPtr packet(new CTradeMessagePacket(account, RSP_ACCOUNT));
		m_messagePump.Enqueue(packet);
	}
}

void CTradeAgent::OnRspError( CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}


void CTradeAgent::OnHeartBeatWarning( int nTimeLapse )
{

}

int CTradeAgent::RequestIDIncrement()
{
	boost::mutex::scoped_lock lock(m_mutReqIdInc);
	return ++m_iRequestID;
}

bool CTradeAgent::SubmitOrder( trade::InputOrder* pInputOrder )
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy_s(req.BrokerID, pInputOrder->brokerid().c_str());
	///Ͷ���ߴ���
	strcpy_s(req.InvestorID, pInputOrder->investorid().c_str());
	///��Լ����
	strcpy_s(req.InstrumentID, pInputOrder->instrumentid().c_str());
	///��������
	strcpy_s(req.OrderRef, pInputOrder->orderref().c_str());
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///�����۸�����: �޼�
	req.OrderPriceType = pInputOrder->orderpricetype();
	///��������: 
	req.Direction = pInputOrder->direction();
	///��Ͽ�ƽ��־: ����
	req.CombOffsetFlag[0] = (pInputOrder->comboffsetflag())[0];
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = (pInputOrder->combhedgeflag())[0];
	///�۸�
	req.LimitPrice = pInputOrder->limitprice();
	///����: 1
	req.VolumeTotalOriginal = pInputOrder->volumetotaloriginal();
	///��Ч������: ������Ч
	req.TimeCondition = pInputOrder->timecondition();
	///GTD����
	//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = pInputOrder->volumecondition();
	///��С�ɽ���: 1
	req.MinVolume = pInputOrder->minvolume();
	///��������: ����
	req.ContingentCondition = pInputOrder->contingentcondition();
	///ֹ���
	//	TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = pInputOrder->forceclosereason();
	///�Զ������־: ��
	req.IsAutoSuspend = pInputOrder->isautosuspend();
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	int iRequestID = RequestIDIncrement();
	req.RequestID = iRequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = pInputOrder->userforceclose();

	int iResult = m_pUserApi->ReqOrderInsert(&req, iRequestID);

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> ����¼������(OrdRef:" << pInputOrder->orderref() << ", ReqestID:" << iRequestID << "): " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��");
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

	CRspOrderInsert* pOrdIns = new CRspOrderInsert;
	pOrdIns->InsertSucess = false;
	pOrdIns->OrderRef = pInputOrder->OrderRef;
	pOrdIns->ErrorMsg = pRspInfo->ErrorMsg;
	MessagePacketPtr packet(new CTradeMessagePacket(pOrdIns, RSP_ORDER_INSERT));
	m_messagePump.Enqueue(packet);
}

void CTradeAgent::OnRtnOrder( CThostFtdcOrderField *pOrder )
{
	ostringstream oss;
	oss << "--->>> " << "OnRtnOrder (OrdRef:"  << pOrder->OrderRef << ") Status:" << pOrder->StatusMsg;
	logger.Info(oss.str());

	trade::Order* pOrd(new trade::Order);

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
	pOrd->set_orderpricetype(static_cast	<trade::OrderPriceTypeType>(pOrder->OrderPriceType));
	///��������
	pOrd->set_direction(static_cast<trade::TradeDirectionType>(pOrder->Direction));
	///��Ͽ�ƽ��־
	pOrd->set_comboffsetflag(pOrder->CombOffsetFlag);
	///���Ͷ���ױ���־
	pOrd->set_combhedgeflag(pOrder->CombHedgeFlag);
	///�۸�
	pOrd->set_limitprice(pOrder->LimitPrice);
	///����
	pOrd->set_volumetotaloriginal(pOrder->VolumeTotalOriginal);
	///��Ч������
	pOrd->set_timecondition(static_cast<trade::TimeConditionType>(pOrder->TimeCondition));
	///GTD����
	pOrd->set_gtddate(pOrder->GTDDate);
	///�ɽ�������
	pOrd->set_volumecondition(static_cast<trade::VolumeConditionType>(pOrder->VolumeCondition));
	///��С�ɽ���
	pOrd->set_minvolume(pOrder->MinVolume);
	///��������
	pOrd->set_contingentcondition(static_cast<trade::ContingentConditionType>(pOrder->ContingentCondition));
	///ֹ���
	pOrd->set_stopprice(pOrder->StopPrice);
	///ǿƽԭ��
	pOrd->set_forceclosereason(static_cast<trade::ForceCloseReasonType>(pOrder->ForceCloseReason));
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
	pOrd->set_ordersubmitstatus(static_cast<trade::OrderSubmitStatusType>(pOrder->OrderSubmitStatus));
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
	if(pOrder->OrderSource < trade::PARTICIPANT)
		pOrd->set_ordersource(trade::PARTICIPANT);
	else
		pOrd->set_ordersource(static_cast<trade::OrderSourceType>(pOrder->OrderSource));
	///����״̬
	pOrd->set_orderstatus(static_cast<trade::OrderStatusType>(pOrder->OrderStatus));
	///��������
	// Sometimes OrderType could be 0 instead of '0'
	if(pOrder->OrderType < trade::NORMAL_ORDER)
		pOrd->set_ordertype(trade::NORMAL_ORDER);
	else
		pOrd->set_ordertype(static_cast<trade::OrderTypeType>(pOrder->OrderType));
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

	// Print order detail once rejected
	if(pOrder->OrderSubmitStatus >= THOST_FTDC_OSS_InsertRejected)
		PrintRtnOrder(pOrder);

	MessagePacketPtr packet(new CTradeMessagePacket(pOrd, RTN_ORDER));
	m_messagePump.Enqueue(packet);
}

void CTradeAgent::OnRtnTrade( CThostFtdcTradeField *pTrade )
{
	ostringstream oss;
	oss << "--->>> " << "OnRtnTrade (OrdRef:"  << pTrade->OrderRef << ") TradeId:" << pTrade->TradeID;
	logger.Info(oss.str());

	trade::Trade* pTd(new trade::Trade);

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
	pTd->set_direction(static_cast<trade::TradeDirectionType>(pTrade->Direction));
	///�������
	pTd->set_ordersysid(pTrade->OrderSysID);
	///��Ա����
	pTd->set_participantid(pTrade->ParticipantID);
	///�ͻ�����
	pTd->set_clientid(pTrade->ClientID);
	///���׽�ɫ
	if(pTrade->TradingRole < trade::ER_BROKER)
	{
		pTd->set_tradingrole(trade::ER_BROKER);
	}
	else
	{
		pTd->set_tradingrole(static_cast<trade::TradingRoleType>(pTrade->TradingRole));
	}
	///��Լ�ڽ������Ĵ���
	pTd->set_exchangeinstid(pTrade->ExchangeInstID);
	///��ƽ��־
	pTd->set_offsetflag(static_cast<trade::OffsetFlagType>(pTrade->OffsetFlag));
	///Ͷ���ױ���־
	pTd->set_hedgeflag(static_cast<trade::HedgeFlagType>(pTrade->HedgeFlag));
	///�۸�
	pTd->set_price(pTrade->Price);
	///����
	pTd->set_volume(pTrade->Volume);
	///�ɽ�ʱ��
	pTd->set_tradedate(pTrade->TradeDate);
	///�ɽ�ʱ��
	pTd->set_tradetime(pTrade->TradeTime);
	///�ɽ�����
	if(pTrade->TradeType < trade::TRDT_COMMON)
		pTd->set_tradetype(trade::TRDT_COMMON);
	else
		pTd->set_tradetype(static_cast<trade::TradeTypeType>(pTrade->TradeType));
	///�ɽ�����Դ
	if(pTrade->PriceSource < trade::PSRC_LAST_PRICE)
		pTd->set_pricesource(trade::PSRC_LAST_PRICE);
	else
		pTd->set_pricesource(static_cast<trade::PriceSourceType>(pTrade->PriceSource));
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

	MessagePacketPtr packet(new CTradeMessagePacket(pTd,  RTN_TRADE));
	m_messagePump.Enqueue(packet);
}


bool CTradeAgent::SubmitOrderAction( trade::InputOrderAction* pInputOrderAction )
{
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy_s(req.BrokerID, m_brokerID.c_str());
	///Ͷ���ߴ���
	strcpy_s(req.InvestorID, m_userID.c_str());
	///������������
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///��������
	strcpy_s(req.OrderRef, pInputOrderAction->orderref().c_str());
	///������
	int iRequestID = RequestIDIncrement();
	req.RequestID = iRequestID;
	///ǰ�ñ��
	req.FrontID = FRONT_ID;
	///�Ự���
	req.SessionID = SESSION_ID;
	///����������
	strcpy_s(req.ExchangeID, pInputOrderAction->exchangeid().c_str());
	///�������
	strcpy_s(req.OrderSysID, pInputOrderAction->ordersysid().c_str());
	///������־
	req.ActionFlag = THOST_FTDC_AF_Delete;	// Cancel order
	///�۸�
	//	TThostFtdcPriceType	LimitPrice;
	///�����仯
	//	TThostFtdcVolumeType	VolumeChange;
	///�û�����
	strcpy_s(req.UserID, pInputOrderAction->userid().c_str());
	///��Լ����
	strcpy_s(req.InstrumentID, pInputOrderAction->instrumentid().c_str());

	int iResult = m_pUserApi->ReqOrderAction(&req, iRequestID);

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> ������������ ( Canel OrdRef:" << pInputOrderAction->orderref() << ", ReqestID:" << iRequestID << "): " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��");
	logger.Debug(oss.str());
#endif

	return iResult == 0;
}

void CTradeAgent::OnRspOrderAction( CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	CRspOrderInsert* pOrdIns = new CRspOrderInsert;
	pOrdIns->InsertSucess = IsErrorRspInfo(pRspInfo);
	pOrdIns->OrderRef = pInputOrderAction->OrderRef;
	pOrdIns->ErrorMsg = pRspInfo->ErrorMsg;
	MessagePacketPtr packet(new CTradeMessagePacket(pOrdIns, RSP_ORDER_INSERT));
	m_messagePump.Enqueue(packet);
}

void CTradeAgent::QueryPositionDetails( const std::string& symbol )
{
	CThostFtdcQryInvestorPositionDetailField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, m_brokerID.c_str());
	strcpy_s(req.InvestorID, m_userID.c_str());
	strcpy_s(req.InstrumentID, symbol.c_str());

	while (true)
	{
		int iResult = m_pUserApi->ReqQryInvestorPositionDetail(&req, RequestIDIncrement());
		if (!IsFlowControl(iResult))
		{
			std::string infoText = boost::str(boost::format("Query investor position details: %d, %s") % iResult % ((iResult == 0) ? "�ɹ�" : "ʧ��"));
			logger.Info(infoText);
			break;
		}
		else
		{
			logger.Warning(boost::str(boost::format("--->>> Query trading account: %d, �ܵ�����") % iResult));
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
	} // while
}

void CTradeAgent::OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		if(pInvestorPositionDetail == NULL) return;

		trade::PositionDetailInfo* posiDetail = new trade::PositionDetailInfo;
		posiDetail->set_instrumentid(pInvestorPositionDetail->InstrumentID);
		posiDetail->set_brokerid(pInvestorPositionDetail->BrokerID);
		posiDetail->set_investorid(pInvestorPositionDetail->BrokerID);
		posiDetail->set_hedgeflag(static_cast<trade::HedgeFlagType>(pInvestorPositionDetail->HedgeFlag));
		posiDetail->set_direction(static_cast<trade::TradeDirectionType>(pInvestorPositionDetail->Direction));
		posiDetail->set_opendate(pInvestorPositionDetail->OpenDate);
		posiDetail->set_tradeid(pInvestorPositionDetail->TradeID);
		posiDetail->set_volume(pInvestorPositionDetail->Volume);
		posiDetail->set_openprice(pInvestorPositionDetail->OpenPrice);
		posiDetail->set_tradingday(pInvestorPositionDetail->TradingDay);
		posiDetail->set_settlementid(pInvestorPositionDetail->SettlementID);
		if(pInvestorPositionDetail->TradeType < trade::TRDT_COMMON)
			posiDetail->set_tradetype(trade::TRDT_COMMON);
		else
			posiDetail->set_tradetype(static_cast<trade::TradeTypeType>(pInvestorPositionDetail->TradeType));
		posiDetail->set_combinstrumentid(pInvestorPositionDetail->CombInstrumentID);
		posiDetail->set_exchangeid(pInvestorPositionDetail->ExchangeID);
		posiDetail->set_closeprofitbydate(pInvestorPositionDetail->CloseProfitByDate);
		posiDetail->set_closeprofitbytrade(pInvestorPositionDetail->CloseProfitByTrade);
		posiDetail->set_positionprofitbydate(pInvestorPositionDetail->PositionProfitByDate);
		posiDetail->set_positionprofitbytrade(pInvestorPositionDetail->PositionProfitByTrade);
		posiDetail->set_margin(pInvestorPositionDetail->Margin);
		posiDetail->set_exchmargin(pInvestorPositionDetail->ExchMargin);
		posiDetail->set_marginratebymoney(pInvestorPositionDetail->MarginRateByMoney);
		posiDetail->set_marginratebyvolume(pInvestorPositionDetail->MarginRateByVolume);
		posiDetail->set_lastsettlementprice(pInvestorPositionDetail->LastSettlementPrice);
		posiDetail->set_settlementprice(pInvestorPositionDetail->SettlementPrice);
		posiDetail->set_closevolume(pInvestorPositionDetail->CloseVolume);
		posiDetail->set_closeamount(pInvestorPositionDetail->CloseAmount);

		MessagePacketPtr packet(new CTradeMessagePacket(posiDetail, RSP_POSITION));
		m_messagePump.Enqueue(packet);
	}

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> ��ѯ��λ��ϸ (ReqestID: " << nRequestID << ") -- " << (bIsLast ? "End" : "Remaining");
	logger.Debug(oss.str());
#endif
}

bool CTradeAgent::QuerySymbol( const std::string& symbol, entity::Quote** ppQuote )
{
	bool querySucc = false;
	int retry = QUERY_QUOTE_RETRY_TIMES;
	while(retry > 0 && !querySucc)
	{
		int reqId = RequestIDIncrement();
		boost::shared_ptr< CSyncRequest<entity::Quote> > req = m_requestFactory.Create(reqId);
		bool succ = req->Invoke(boost::bind(&CTradeAgent::QuerySymbolAsync, this, symbol, _1));
		if(succ)
		{
			querySucc = req->GetResult(ppQuote);
			m_requestFactory.Remove(reqId);
			return querySucc;
		}
		else
		{
			m_requestFactory.Remove(reqId);
			boost::this_thread::sleep(boost::posix_time::milliseconds(200));
		}
		--retry;
	}
	return querySucc;
}

bool CTradeAgent::QuerySymbolAsync( const std::string& symbol, int nReqestId )
{
	CThostFtdcQryDepthMarketDataField req;
	strcpy_s(req.InstrumentID, symbol.c_str());
	int iResult = m_pUserApi->ReqQryDepthMarketData(&req, nReqestId);

	std::string infoText = boost::str(boost::format("Query %s Quote: %d, %s") % symbol.c_str() % iResult % ((iResult == 0) ? "�ɹ�" : "ʧ��"));
	logger.Info(infoText);
	return (iResult == 0);
}

void CTradeAgent::OnRspQryDepthMarketData( CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	entity::Quote* quote = new entity::Quote();	// it will be managed by quote aggregator

	quote->set_symbol(pDepthMarketData->InstrumentID);
	quote->set_trading_day(pDepthMarketData->TradingDay);
	quote->set_exchange_id(pDepthMarketData->ExchangeID);
	quote->set_exchange_symbol_id(pDepthMarketData->ExchangeInstID);
	quote->set_last(pDepthMarketData->LastPrice);
	quote->set_prev_settlement_price(pDepthMarketData->PreSettlementPrice);
	quote->set_prev_close(pDepthMarketData->PreClosePrice);
	quote->set_prev_open_interest(pDepthMarketData->PreOpenInterest);
	quote->set_open(pDepthMarketData->OpenPrice);
	quote->set_high(pDepthMarketData->HighestPrice);
	quote->set_low(pDepthMarketData->LowestPrice);
	quote->set_volume(pDepthMarketData->Volume);
	quote->set_turnover(pDepthMarketData->Turnover);
	quote->set_open_interest(pDepthMarketData->OpenInterest);
	quote->set_close(pDepthMarketData->ClosePrice);
	quote->set_settlement_price(pDepthMarketData->SettlementPrice);
	quote->set_upper_limit_price(pDepthMarketData->UpperLimitPrice);
	quote->set_lower_limit_price(pDepthMarketData->LowerLimitPrice);
	quote->set_prev_delta(pDepthMarketData->PreDelta);
	quote->set_curr_delta(pDepthMarketData->CurrDelta);
	quote->set_update_time(pDepthMarketData->UpdateTime);
	quote->set_update_millisec(pDepthMarketData->UpdateMillisec);

	quote->set_bid(pDepthMarketData->BidPrice1);
	quote->set_bid_size(pDepthMarketData->BidVolume1);
	quote->set_ask(pDepthMarketData->AskPrice1);
	quote->set_ask_size(pDepthMarketData->AskVolume1);
	quote->set_bid_2(pDepthMarketData->BidPrice2);
	quote->set_bid_size_2(pDepthMarketData->BidVolume2);
	quote->set_ask_2(pDepthMarketData->AskPrice2);
	quote->set_ask_size_2(pDepthMarketData->AskVolume2);
	quote->set_bid_3(pDepthMarketData->BidPrice3);
	quote->set_bid_size_3(pDepthMarketData->BidVolume3);
	quote->set_ask_3(pDepthMarketData->AskPrice3);
	quote->set_ask_size_3(pDepthMarketData->AskVolume3);
	quote->set_bid_4(pDepthMarketData->BidPrice4);
	quote->set_bid_size_4(pDepthMarketData->BidVolume4);
	quote->set_ask_4(pDepthMarketData->AskPrice4);
	quote->set_ask_size_4(pDepthMarketData->AskVolume4);
	quote->set_bid_5(pDepthMarketData->BidPrice5);
	quote->set_bid_size_5(pDepthMarketData->BidVolume5);
	quote->set_ask_5(pDepthMarketData->AskPrice5);
	quote->set_ask_size_5(pDepthMarketData->AskVolume5);
	quote->set_average_price(pDepthMarketData->AveragePrice);

	m_requestFactory.Response(nRequestID, true, quote);
}

void CTradeAgent::SetCallbackHanlder( CTradeAgentCallback* pCallback )
{
	m_messagePump.Init(pCallback);
}

