#include "StdAfx.h"
#include "TradeAgent_FM.h"
#include "RtnOrderWrapper_FM.h"
#include "FileOperations.h"
#include "globalmembers.h"
#include "charsetconvert.h"
#include "SymbolInfoRepositry.h"

#include <sstream>

#if defined(WIN32)
#pragma comment(lib, "./FemasAPI/USTPtraderapi.lib")
#else
#define strcpy_s strcpy
#define _strdup strdup
#endif

enum {
	DISCONNECT_TIMEOUT_SECOND = 1,
	CONNECT_TIMEOUT_SECONDS = 30,
	QUERY_QUOTE_RETRY_TIMES = 5
};

// �����ж�
bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

bool IsErrorRspInfo(CUstpFtdcRspInfoField *pRspInfo)
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

void PrintRtnOrder(CUstpFtdcOrderField* pOrder)
{
	ostringstream oss;
	oss << "\n///////////////////////\n";
	oss << "��Լ����      \t" << pOrder->InstrumentID << "\n";
	oss << "��������      \t" << pOrder->UserOrderLocalID << "\n";
	oss << "�����۸�����   \t" << pOrder->OrderPriceType << "\n";	
	oss << "��������      \t" << pOrder->Direction << "\n";	
	oss << "��Ͽ�ƽ��־   \t" << pOrder->OffsetFlag << "\n";
	oss << "���Ͷ���ױ���־\t" << pOrder->HedgeFlag << "\n";
	oss << "�۸�          \t" << pOrder->LimitPrice << "\n";
	oss << "����          \t" << pOrder->Volume << "\n";
	oss << "��Ч������     \t" << pOrder->TimeCondition << "\n";
	oss << "�ɽ�������     \t" << pOrder->VolumeCondition << "\n";
	oss << "///////////////////////\n";

	logger.Debug(oss.str());
}

CSymbolInfoRepositry symbolInfoRepo;

CTradeAgent::CTradeAgent(void)
	: m_pUserApi(NULL)
	, m_isConnected(false)
	, m_isLogged(false)
	, m_isConfirmed(false)
	, m_isWorking(false)
	, m_maxOrderRef(1)
	, m_iRequestID(0)
	, m_orderProcessor(NULL)
{
#ifdef FAKE_DEAL
	m_fakeDealer.Init(boost::bind(&CTradeAgent::OnRtnOrder, this, _1)
		, boost::bind(&CTradeAgent::OnRspOrderInsert, this, _1, _2, _3, _4)
		, boost::bind(&CTradeAgent::OnRspOrderAction, this, _1, _2, _3, _4));
#endif
}


CTradeAgent::~CTradeAgent(void)
{
	Logout();
	m_pUserApi = NULL;
}

void CTradeAgent::RunTradingFunc(string address)
{
	// duplicate address string and use boost.smart_ptr to manage its life cycle
	boost::scoped_array<char> front_addr(_strdup(address.c_str()));
	m_isWorking = true;
	m_pUserApi->RegisterFront(front_addr.get());					// connect
	m_pUserApi->Init();

	m_pUserApi->Join();
	m_isWorking = false;
}

boost::tuple<bool, string> CTradeAgent::Login(const string& frontAddr, const string& brokerId, const string& InvestorId, const string& userId, const string& password)
{
	try{
		string streamFolder = userId + "/Td/";
		if(!CreateFolderIfNotExists(streamFolder))
		{
			m_loginErr = boost::str(boost::format("Cannot create stream folder (%s) for trading") % streamFolder);
			return boost::make_tuple(false, m_loginErr);
		}

		m_brokerId = brokerId;
		m_investorId = InvestorId;
		m_userId = userId;
		m_password = password;

		// ����UserApi
		m_pUserApi = CUstpFtdcTraderApi::CreateFtdcTraderApi(streamFolder.c_str());
		// ע���¼���
		m_pUserApi->RegisterSpi(this);

		m_pUserApi->SubscribePrivateTopic(USTP_TERT_QUICK);
		m_pUserApi->SubscribePublicTopic(USTP_TERT_QUICK);
		
		logger.Info(boost::str(boost::format("Try to connect trade server (%s) ...") % frontAddr));

#ifdef FAKE_LOGIN
		return boost::make_tuple(true, m_loginErr);
#endif

		// wait for connected event
		{
			boost::unique_lock<boost::mutex> lock(m_mutConnecting);
			m_thTrading = boost::thread(&CTradeAgent::RunTradingFunc, this, frontAddr);

			if(!m_condConnecting.timed_wait(lock, boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS)))
			{
				m_loginErr = boost::str(boost::format("Connecting %s time out") % frontAddr);
				logger.Warning(m_loginErr);
				return boost::make_tuple(false, m_loginErr);
			}
			if(!m_isConnected)
			{
				logger.Error(boost::str(boost::format("Connecting %s encounter unknown error") % frontAddr));
				return boost::make_tuple(false, m_loginErr);
			}
		}
		
		// wait for login
		{
			boost::unique_lock<boost::mutex> lock(m_mutLogin);
			Login();

			if(!m_condLogin.timed_wait(lock, boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS)))
			{
				m_loginErr = boost::str(boost::format("Login %s time out") % userId);
				logger.Warning(m_loginErr);
				return boost::make_tuple(false, m_loginErr);
			}
			if(!m_isLogged)
			{
				return boost::make_tuple(false, m_loginErr);
			}
		}

		return boost::make_tuple(true, m_loginErr);
	}
	catch(std::exception& ex)
	{
		m_loginErr = "Failed to connect to trade server, details is following\n";
		m_loginErr += ex.what();
		logger.Error(m_loginErr);
	}
	catch(...)
	{
		m_loginErr = "Unknown error encountered while connecting trade server";
		logger.Error(m_loginErr);
	}

	return boost::make_tuple(false, m_loginErr);
}

void CTradeAgent::Login()
{
	string traceInfo = boost::str(boost::format("Log in trade (%s, %s, %s)") 
		% m_brokerId % m_userId % m_password);
	logger.Trace(traceInfo);

	bool reqSucc = false;
	try
	{
		CUstpFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy_s(req.BrokerID, m_brokerId.c_str());
		strcpy_s(req.UserID, m_userId.c_str());
		strcpy_s(req.Password, m_password.c_str());
		strcpy_s(req.UserProductInfo, "TradeStationFM 4.2");
		if(m_pUserApi != NULL)
		{
			int requestId = RequestIDIncrement();
			int iResult = m_pUserApi->ReqUserLogin(&req, requestId);
			reqSucc = iResult == 0;
			string loginInfo = boost::str(boost::format("Sending login %s, RequestID: %d")
				% (reqSucc ? "Succeeded" : "Failed") % requestId);
			logger.Info(loginInfo);
		}
	}
	catch (...)
	{
		m_loginErr = "Encounter error while logging in trade server";
		logger.Error(m_loginErr);
	}

	if(!reqSucc)
	{
		boost::lock_guard<boost::mutex> lock(m_mutConnecting);
		m_isConnected = false;
		m_condConnecting.notify_one();
	}
}

void CTradeAgent::Logout()
{
	LOG_INFO(logger, boost::str(boost::format("%s Begin logging OUT...") % m_investorId));
	if(m_isWorking)
	{
		m_pUserApi->Release();
		m_thTrading.join();
	}
	LOG_INFO(logger, boost::str(boost::format("%s Logged OUT.") % m_investorId));
}

void CTradeAgent::OnFrontConnected()
{
	boost::lock_guard<boost::mutex> lock(m_mutConnecting);
	m_isConnected = true;
	logger.Info("Trade connected");
	m_condConnecting.notify_one();
}

void CTradeAgent::OnFrontDisconnected( int nReason )
{
	if(nReason == 0)
	{
		logger.Info(boost::str(boost::format("%s Trade normally disconnected.") % m_investorId));
	}
	else
	{
		string reasonTxt = boost::str(boost::format("%s disconnected from trade server due to ") % m_investorId);
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

	m_isConnected = false;
}

void CTradeAgent::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	boost::mutex::scoped_lock lock(m_mutLogin);

	m_isLogged = (pRspInfo->ErrorID == 0);
	string loginInfo = boost::str(
		boost::format(
		"Trade login response (ReqId:%d): %s") 
		% nRequestID 
		% (m_isLogged ? "Succeeded" : "Failed"));
	logger.Info(loginInfo);

	if(m_isLogged)
	{
#ifdef FAKE_DEAL
		m_fakeDealer.SetSessionParams(FRONT_ID, SESSION_ID);
#endif
		if (pRspUserLogin->MaxOrderLocalID == NULL || strcmp(pRspUserLogin->MaxOrderLocalID, "") == 0)
			m_maxOrderRef = 1;
		else 
			m_maxOrderRef = atoi(pRspUserLogin->MaxOrderLocalID) + 1;

		string ds(pRspUserLogin->TradingDay);
		m_tradingDay = boost::gregorian::from_undelimited_string(ds);

		ostringstream ss;
		ss << "Trading day: " << pRspUserLogin->TradingDay << endl;
		ss << "Login Time: " << pRspUserLogin->LoginTime << endl;
		ss << "Broker ID: " << pRspUserLogin->BrokerID << endl;
		ss << "User ID: " << pRspUserLogin->UserID << endl;
		ss << "System name: " << pRspUserLogin->TradingSystemName << endl;
		ss << "Data Center ID: " << pRspUserLogin->DataCenterID << endl;
		ss << "Maximum order ref: " << pRspUserLogin->MaxOrderLocalID << endl;
		
		logger.Info(ss.str());
	}
	else
	{
		m_maxOrderRef = -1;
		logger.Error(boost::str(boost::format("Error Message:%s") % pRspInfo->ErrorMsg));
		GB2312ToUTF_8(m_loginErr, pRspInfo->ErrorMsg);
	}

	m_orderProcessor->OnRspUserLogin(m_isLogged, m_loginErr, m_maxOrderRef);

	m_condLogin.notify_one();
}


void CTradeAgent::OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

}

void CTradeAgent::SetCallbackHanlder( CTradeAgentCallback* pCallback )
{
	m_orderProcessor = pCallback;
}

bool CTradeAgent::SubmitOrder( trade::InputOrder* pInputOrder )
{
	CUstpFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy_s(req.BrokerID, pInputOrder->brokerid().c_str());
	///Ͷ���ߴ���
	strcpy_s(req.InvestorID, pInputOrder->investorid().c_str());
	///��Լ����
	strcpy_s(req.InstrumentID, pInputOrder->instrumentid().c_str());
	///��������
	strcpy_s(req.UserOrderLocalID, pInputOrder->orderref().c_str());
	strcpy_s(req.ExchangeID, "CFFEX");
	strcpy_s(req.UserID, m_userId.c_str());
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///�����۸�����: �޼�
	req.OrderPriceType = pInputOrder->orderpricetype();
	///��������: 
	req.Direction = pInputOrder->direction();
	///��Ͽ�ƽ��־: ����
	req.OffsetFlag = (pInputOrder->comboffsetflag())[0];
	///���Ͷ���ױ���־
	req.HedgeFlag = (pInputOrder->combhedgeflag())[0];
	///�۸�
	req.LimitPrice = pInputOrder->limitprice();
	///����: 1
	req.Volume = pInputOrder->volumetotaloriginal();
	///��Ч������: ������Ч
	req.TimeCondition = pInputOrder->timecondition();
	///GTD����
	//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = pInputOrder->volumecondition();
	///��С�ɽ���: 1
	req.MinVolume = pInputOrder->minvolume();
	///ֹ���
	//	TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = pInputOrder->forceclosereason();
	///�Զ������־: ��
	req.IsAutoSuspend = pInputOrder->isautosuspend();
	int iRequestID = RequestIDIncrement();
	int iResult = m_pUserApi->ReqOrderInsert(&req, iRequestID);

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> ����¼������(OrdRef:" << pInputOrder->orderref() << ", ReqestID:" << iRequestID << "): " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��");
	logger.Debug(oss.str());
#endif

	return iResult == 0;
}

bool CTradeAgent::SubmitOrder( CUstpFtdcInputOrderField& inputOrderField )
{
	int iRequestID = RequestIDIncrement();
	strcpy_s(inputOrderField.ExchangeID, "CFFEX");
	strcpy_s(inputOrderField.UserID, m_userId.c_str());
#ifdef FAKE_DEAL
	int iResult = m_fakeDealer.ReqOrderInsert(&inputOrderField, iRequestID);
#else
	int iResult = m_pUserApi->ReqOrderInsert(&inputOrderField, iRequestID);
#endif

	return iResult == 0;
}


void CTradeAgent::OnRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	ostringstream oss;
	oss << "--->>> " << m_investorId <<" OnRspOrderInsert for order ( OrderRef: " << pInputOrder->UserOrderLocalID << " ) with RequestID: " << nRequestID <<  endl;
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
	{
		oss << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
		logger.Info(oss.str());

		/*	Don't publish error insert order any longer, instead return a fake reject order built from inputOrder
		string errorMsg = pRspInfo->ErrorMsg;
		string ordRef = pInputOrder->UserOrderLocalID;
		m_orderProcessor->OnRspOrderInsert(false, ordRef, errorMsg);
		*/

		RtnOrderWrapperPtr orderWrapper(CRtnOrderWrapper::MakeFakeRejectOrder(*pInputOrder, pRspInfo->ErrorMsg));
		// Order will be actually managed by underlying evtOrder
		m_orderProcessor->OnRtnOrder(orderWrapper);
	}
}

void CTradeAgent::OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo)
{
	/*
	ostringstream oss;
	oss << "--->>> " << m_investorId << " OnRspOrderInsert for order ( OrderRef: " << pInputOrder->UserOrderLocalID << endl;
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		oss << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;

	logger.Info(oss.str());

	string ordRef = pInputOrder->UserOrderLocalID;
	string errorMsg = pRspInfo->ErrorMsg;
	m_orderProcessor->OnRspOrderInsert(false, ordRef, errorMsg);
	*/
}


void CTradeAgent::OnRtnOrder(CUstpFtdcOrderField *pOrder)
{
	if(!IsMyOrder(pOrder))
		return;

	ostringstream oss;
	oss << "--->>> " << m_investorId << "(" << m_userId << ") OnRtnOrder (OrdRef:" << pOrder->UserOrderLocalID << ") Status:" << pOrder->OrderStatus;
	logger.Info(oss.str());

	RtnOrderWrapperPtr orderWrapper(new CRtnOrderWrapper(pOrder));

	// Order will be actually managed by underlying evtOrder
	m_orderProcessor->OnRtnOrder(orderWrapper);
}

void CTradeAgent::OnRtnTrade(CUstpFtdcTradeField *pTrade)
{
	if (!IsMyTrade(pTrade))
		return;

	ostringstream oss;
	oss << "--->>> " << m_investorId << "(" << m_userId << ") OnRtnTrade (OrdRef:" << pTrade->UserOrderLocalID << ") TradeId:" << pTrade->TradeID;
	oss << " " << pTrade->InstrumentID;
	oss << " " << pTrade->TradeTime;
	oss << " @" << pTrade->TradePrice;
	oss << " " << pTrade->TradeVolume << " traded.";
	logger.Info(oss.str());

	boost::shared_ptr<trade::Trade> pTd(new trade::Trade);

	///���͹�˾����
	pTd->set_brokerid(pTrade->BrokerID);
	///Ͷ���ߴ���
	pTd->set_investorid(pTrade->InvestorID);
	///��Լ����
	pTd->set_instrumentid(pTrade->InstrumentID);
	///��������
	pTd->set_orderref(pTrade->UserOrderLocalID);
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
	pTd->set_tradingrole(trade::ER_BROKER);
	///��Լ�ڽ������Ĵ���
	pTd->set_exchangeinstid("");
	///��ƽ��־
	pTd->set_offsetflag(static_cast<trade::OffsetFlagType>(pTrade->OffsetFlag));
	///Ͷ���ױ���־
	pTd->set_hedgeflag(static_cast<trade::HedgeFlagType>(pTrade->HedgeFlag));
	///�۸�
	pTd->set_price(pTrade->TradePrice);
	///����
	pTd->set_volume(pTrade->TradeVolume);
	///�ɽ�ʱ��
	pTd->set_tradedate(pTrade->TradingDay);
	///�ɽ�ʱ��
	pTd->set_tradetime(pTrade->TradeTime);
	///�ɽ�����
	pTd->set_tradetype(trade::TRDT_COMMON);
	///�ɽ�����Դ
	pTd->set_pricesource(trade::PSRC_LAST_PRICE);
	///����������Ա����
	pTd->set_traderid("");
	///���ر������
	pTd->set_orderlocalid("");
	///�����Ա���
	pTd->set_clearingpartid(pTrade->ClearingPartID);
	///ҵ��Ԫ
	pTd->set_businessunit("");
	///���
	pTd->set_sequenceno(0);
	///������
	pTd->set_tradingday(pTrade->TradingDay);
	///������
	pTd->set_settlementid(0);
	///���͹�˾�������
	pTd->set_brokerorderseq(0);

	m_orderProcessor->OnRtnTrade(pTd.get());


}

bool CTradeAgent::SubmitOrderAction( trade::InputOrderAction* pInputOrderAction )
{
	CUstpFtdcOrderActionField req;
	memset(&req, 0, sizeof(req));
	/// �н���
	strcpy_s(req.ExchangeID, "CFFEX");
	///���͹�˾����
	strcpy_s(req.BrokerID, m_brokerId.c_str());
	///�û�����
	strcpy_s(req.UserID, m_userId.c_str());
	///�������
	strcpy_s(req.OrderSysID, pInputOrderAction->ordersysid().c_str());
	///Ͷ���߱��
	strcpy_s(req.InvestorID, m_investorId.c_str());
	req.ActionFlag = USTP_FTDC_AF_Delete;
	
	strcpy_s(req.UserOrderActionLocalID, pInputOrderAction->orderactionref().c_str());
	strcpy_s(req.UserOrderLocalID, pInputOrderAction->orderref().c_str());

	///������
	int iRequestID = RequestIDIncrement();
	
#ifdef FAKE_DEAL
	int iResult = m_fakeDealer.ReqOrderAction(&req, iRequestID);
#else
	int iResult = m_pUserApi->ReqOrderAction(&req, iRequestID);
#endif

	ostringstream oss;
	oss << "--->>> ������������ ( Canel OrdRef:" << pInputOrderAction->orderref() << ", ReqestID:" << iRequestID << "): " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��");
	logger.Debug(oss.str());

	return iResult == 0;
}

void CTradeAgent::OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool cancelError = IsErrorRspInfo(pRspInfo);
	if (cancelError)
	{
		string orderRef = pOrderAction->UserOrderLocalID;
		string errorMsg = pRspInfo->ErrorMsg;

		m_orderProcessor->OnRspOrderAction(false, orderRef, pRspInfo->ErrorID, errorMsg);
	}
}

void CTradeAgent::QueryAccount()
{
	assert(!m_brokerId.empty()); // , "Broker Id cannot be empty");
	assert(!m_investorId.empty());	//, "Account Id cannot be empty");

	if(m_brokerId.empty() || m_investorId.empty())
		return;
	
	CUstpFtdcQryInvestorAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, m_brokerId.c_str());
	strcpy_s(req.InvestorID, m_investorId.c_str());
	while (true)
	{
		int iResult = m_pUserApi->ReqQryInvestorAccount(&req, RequestIDIncrement());
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


void CTradeAgent::OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	logger.Debug("--->>> OnRspQryTradingAccount");
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		boost::shared_ptr<trade::AccountInfo> account(new trade::AccountInfo);
		account->set_brokerid(pRspInvestorAccount->BrokerID);
		account->set_accountid(pRspInvestorAccount->AccountID);
		account->set_prebalance(pRspInvestorAccount->PreBalance);
		account->set_deposit(pRspInvestorAccount->Deposit);
		account->set_withdraw(pRspInvestorAccount->Withdraw);
		account->set_frozenmargin(pRspInvestorAccount->FrozenMargin);
		account->set_frozencommission(pRspInvestorAccount->FrozenFee);
		account->set_frozencash(pRspInvestorAccount->FrozenPremium);
		account->set_commission(pRspInvestorAccount->Fee);
		account->set_closeprofit(pRspInvestorAccount->CloseProfit);
		account->set_positionprofit(pRspInvestorAccount->PositionProfit);
		account->set_available(pRspInvestorAccount->Available);

		m_orderProcessor->OnRspQryTradingAccount(*account);
	}
}

void CTradeAgent::QueryOrders( const string& symbol )
{
	// to do
}

void CTradeAgent::QueryPositions()
{
	// to do
}

void CTradeAgent::QueryPositionDetails( const string& symbol )
{
	CUstpFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, m_brokerId.c_str());
	strcpy_s(req.InvestorID, m_investorId.c_str());
	strcpy_s(req.InstrumentID, symbol.c_str());

	while (true)
	{
		int iResult = m_pUserApi->ReqQryInvestorPosition(&req, RequestIDIncrement());
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


void CTradeAgent::OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		if (pRspInvestorPosition == NULL) return;

		boost::shared_ptr<trade::PositionDetailInfo> posiDetail(new trade::PositionDetailInfo);
		posiDetail->set_instrumentid(pRspInvestorPosition->InstrumentID);
		posiDetail->set_brokerid(pRspInvestorPosition->BrokerID);
		posiDetail->set_investorid(pRspInvestorPosition->BrokerID);
		posiDetail->set_hedgeflag(static_cast<trade::HedgeFlagType>(pRspInvestorPosition->HedgeFlag));
		posiDetail->set_direction(static_cast<trade::TradeDirectionType>(pRspInvestorPosition->Direction));
		/* TODO
		posiDetail->set_opendate(pRspInvestorPosition->OpenDate);
		posiDetail->set_tradeid(pRspInvestorPosition->TradeID);
		posiDetail->set_volume(pRspInvestorPosition->Volume);
		posiDetail->set_openprice(pRspInvestorPosition->OpenPrice);
		posiDetail->set_tradingday(pRspInvestorPosition->TradingDay);
		posiDetail->set_settlementid(pRspInvestorPosition->SettlementID);
		if (pRspInvestorPosition->TradeType < trade::TRDT_COMMON)
			posiDetail->set_tradetype(trade::TRDT_COMMON);
		else
			posiDetail->set_tradetype(static_cast<trade::TradeTypeType>(pRspInvestorPosition->TradeType));
		posiDetail->set_combinstrumentid(pRspInvestorPosition->CombInstrumentID);
		posiDetail->set_exchangeid(pRspInvestorPosition->ExchangeID);
		posiDetail->set_closeprofitbydate(pRspInvestorPosition->CloseProfitByDate);
		posiDetail->set_closeprofitbytrade(pRspInvestorPosition->CloseProfitByTrade);
		posiDetail->set_positionprofitbydate(pRspInvestorPosition->PositionProfitByDate);
		posiDetail->set_positionprofitbytrade(pRspInvestorPosition->PositionProfitByTrade);
		posiDetail->set_margin(pRspInvestorPosition->Margin);
		posiDetail->set_exchmargin(pRspInvestorPosition->ExchMargin);
		posiDetail->set_marginratebymoney(pRspInvestorPosition->MarginRateByMoney);
		posiDetail->set_marginratebyvolume(pRspInvestorPosition->MarginRateByVolume);
		posiDetail->set_lastsettlementprice(pRspInvestorPosition->LastSettlementPrice);
		posiDetail->set_settlementprice(pRspInvestorPosition->SettlementPrice);
		posiDetail->set_closevolume(pRspInvestorPosition->CloseVolume);
		posiDetail->set_closeamount(pRspInvestorPosition->CloseAmount);
		*/
		m_orderProcessor->OnRspQryInvestorPositionDetail(posiDetail.get());
	}

#ifdef _DEBUG
	ostringstream oss;
	oss << "--->>> ��ѯ��λ��ϸ (ReqestID: " << nRequestID << ") -- " << (bIsLast ? "End" : "Remaining");
	logger.Debug(oss.str());
#endif
}

bool CTradeAgent::QuerySymbolInfo( const string& symbol, CSymbolInfo** ppSymbolInfo )
{
	bool querySucc = false;
	int reqId = RequestIDIncrement();
	boost::shared_ptr< CSyncRequest<CSymbolInfo> > req = m_symbInfoReqFactory.Create(reqId);

	CSymbolInfo* pSymbolInfo = symbolInfoRepo.Get(symbol.c_str());
	assert(pSymbolInfo != NULL);

	if(pSymbolInfo != NULL)
	{
		if(pSymbolInfo->Ready())
		{
			*ppSymbolInfo = pSymbolInfo;
			return true;
		}
		else
		{
			bool succ = req->Invoke(boost::bind(&CTradeAgent::QuerySymbolInfoAsync, this, pSymbolInfo, _1));
			if(succ)
			{
				querySucc = req->GetResult(ppSymbolInfo);
				m_symbInfoReqFactory.Remove(reqId);
			}
		}
	}

	return querySucc;
}

bool CTradeAgent::QuerySymbolInfoAsync( CSymbolInfo* pSymbolInfo, int nReqestId )
{
	CUstpFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.InstrumentID, (pSymbolInfo->Instrument()).c_str());
	strcpy_s(req.ExchangeID, (pSymbolInfo->ExchangeID()).c_str());
	strcpy_s(req.ProductID, (pSymbolInfo->ProductID()).c_str());
	int iResult = m_pUserApi->ReqQryInstrument(&req, nReqestId);

	std::string infoText = boost::str(boost::format("Query %s information: %d, %s") 
		% pSymbolInfo->Instrument() % iResult % ((iResult == 0) ? "�ɹ�" : "ʧ��"));
	logger.Info(infoText);
	return (iResult == 0);
}

void CTradeAgent::OnRspQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInstrument != NULL)
	{
		string symbol = pRspInstrument->InstrumentID;

		CSymbolInfo* pSymbInfo = symbolInfoRepo.Get(symbol.c_str());
		if(pSymbInfo != NULL)
		{
			pSymbInfo->PriceTick(pRspInstrument->PriceTick);
			pSymbInfo->VolumeMultiple(pRspInstrument->VolumeMultiple);
			pSymbInfo->Ready(true);
			m_symbInfoReqFactory.Response(nRequestID, true, pSymbInfo);
			return;
		}
	}
	// something wrong
	m_symbInfoReqFactory.Response(nRequestID, false, NULL);
}

bool CTradeAgent::QuerySymbol(const string& symbol, entity::Quote** ppQuote)
{
	return false;
}

bool CTradeAgent::QuerySymbolAsync(const string& symbol, int nReqestId)
{
	return false;
}

