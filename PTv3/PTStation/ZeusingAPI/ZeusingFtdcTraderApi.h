
#if !defined(ZEUSING_FTDCTRADERAPI_H)
#define ZEUSING_FTDCTRADERAPI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZeusingFtdcUserApiStruct.h"

#if defined(ISLIB) && defined(WIN32)
#ifdef LIB_TRADER_API_EXPORT
#define TRADER_API_EXPORT __declspec(dllexport)
#else
#define TRADER_API_EXPORT __declspec(dllimport)
#endif
#else
#define TRADER_API_EXPORT 
#endif

class CZeusingFtdcTraderSpi
{
public:
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected(){};
	
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(int nReason){};
		
	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��
	virtual void OnHeartBeatWarning(int nTimeLapse){};
	
	///�ͻ�����֤��Ӧ
	virtual void OnRspAuthenticate(CZeusingFtdcRspAuthenticateField *pRspAuthenticateField, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};


	///��¼������Ӧ
	virtual void OnRspUserLogin(CZeusingFtdcRspUserLoginField *pRspUserLogin, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CZeusingFtdcUserLogoutField *pUserLogout, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�û��������������Ӧ
	virtual void OnRspUserPasswordUpdate(CZeusingFtdcUserPasswordUpdateField *pUserPasswordUpdate, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�ʽ��˻��������������Ӧ
	virtual void OnRspTradingAccountPasswordUpdate(CZeusingFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(CZeusingFtdcInputOrderField *pInputOrder, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///Ԥ��¼��������Ӧ
	virtual void OnRspParkedOrderInsert(CZeusingFtdcParkedOrderField *pParkedOrder, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///Ԥ�񳷵�¼��������Ӧ
	virtual void OnRspParkedOrderAction(CZeusingFtdcParkedOrderActionField *pParkedOrderAction, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///��������������Ӧ
	virtual void OnRspOrderAction(CZeusingFtdcInputOrderActionField *pInputOrderAction, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///��ѯ��󱨵�������Ӧ
	virtual void OnRspQueryMaxOrderVolume(CZeusingFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CZeusingFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///ɾ��Ԥ����Ӧ
	virtual void OnRspRemoveParkedOrder(CZeusingFtdcRemoveParkedOrderField *pRemoveParkedOrder, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///ɾ��Ԥ�񳷵���Ӧ
	virtual void OnRspRemoveParkedOrderAction(CZeusingFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///ִ������¼��������Ӧ
	virtual void OnRspExecOrderInsert(CZeusingFtdcInputExecOrderField *pInputExecOrder, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///ִ���������������Ӧ
	virtual void OnRspExecOrderAction(CZeusingFtdcInputExecOrderActionField *pInputExecOrderAction, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///����¼��������Ӧ
	virtual void OnRspQuoteInsert(CZeusingFtdcInputQuoteField *pInputQuote, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///���۲���������Ӧ
	virtual void OnRspQuoteAction(CZeusingFtdcInputQuoteActionField *pInputQuoteAction, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///ѯ�۲���Ӧ��
	virtual void OnRspForQuote(CZeusingFtdcInputForQuoteField *pInputForQuote, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ʽ���ת��Ӧ
	virtual void OnRspInternalTransfer(CZeusingFtdcInternalTransferField *pInternalTransfer, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ������Ӧ
	virtual void OnRspQryOrder(CZeusingFtdcOrderField *pOrder, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ�ɽ���Ӧ
	virtual void OnRspQryTrade(CZeusingFtdcTradeField *pTrade, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(CZeusingFtdcInvestorPositionField *pInvestorPosition, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CZeusingFtdcTradingAccountField *pTradingAccount, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯͶ������Ӧ
	virtual void OnRspQryInvestor(CZeusingFtdcInvestorField *pInvestor, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ���ױ�����Ӧ
	virtual void OnRspQryTradingCode(CZeusingFtdcTradingCodeField *pTradingCode, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ��Լ��֤������Ӧ
	virtual void OnRspQryInstrumentMarginRate(CZeusingFtdcInstrumentMarginRateField *pInstrumentMarginRate, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ��Լ����������Ӧ
	virtual void OnRspQryInstrumentCommissionRate(CZeusingFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ��������Ӧ
	virtual void OnRspQryExchange(CZeusingFtdcExchangeField *pExchange, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CZeusingFtdcInstrumentField *pInstrument, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ������Ӧ
	virtual void OnRspQryDepthMarketData(CZeusingFtdcDepthMarketDataField *pDepthMarketData, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯͶ���߽�������Ӧ
	virtual void OnRspQrySettlementInfo(CZeusingFtdcSettlementInfoField *pSettlementInfo, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯת��������Ӧ
	virtual void OnRspQryTransferBank(CZeusingFtdcTransferBankField *pTransferBank, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯͶ���ֲ߳���ϸ��Ӧ
	virtual void OnRspQryInvestorPositionDetail(CZeusingFtdcInvestorPositionDetailField *pInvestorPositionDetail, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯͶ���ֲ߳���ϸ��Ӧ
	virtual void OnRspQryInvestorPositionCombineDetail(CZeusingFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///��ѯ��֤����ϵͳ���͹�˾�ʽ��˻���Կ��Ӧ
	virtual void OnRspQryCFMMCTradingAccountKey(CZeusingFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ�ֵ��۵���Ϣ��Ӧ
	virtual void OnRspQryEWarrantOffset(CZeusingFtdcEWarrantOffsetField *pEWarrantOffset, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯִ��������Ӧ
	virtual void OnRspQryExecOrder(CZeusingFtdcExecOrderField *pExecOrder, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯִ�����������Ӧ
	virtual void OnRspQryExecOrderAction(CZeusingFtdcExecOrderActionField *pExecOrderAction, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ����ִ��������Ӧ
	virtual void OnRspQryErrExecOrder(CZeusingFtdcErrExecOrderField *pErrExecOrder, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ����ִ�����������Ӧ
	virtual void OnRspQryErrExecOrderAction(CZeusingFtdcErrExecOrderActionField *pErrExecOrderAction, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯת����ˮ��Ӧ
	virtual void OnRspQryTransferSerial(CZeusingFtdcTransferSerialField *pTransferSerial, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ����ǩԼ��ϵ��Ӧ
	virtual void OnRspQryAccountregister(CZeusingFtdcAccountregisterField *pAccountregister, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///����Ӧ��
	virtual void OnRspError(CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///����֪ͨ
	virtual void OnRtnOrder(CZeusingFtdcOrderField *pOrder) {};

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CZeusingFtdcTradeField *pTrade) {};

	///����¼�����ر�
	virtual void OnErrRtnOrderInsert(CZeusingFtdcInputOrderField *pInputOrder, CZeusingFtdcRspInfoField *pRspInfo) {};

	///������������ر�
	virtual void OnErrRtnOrderAction(CZeusingFtdcOrderActionField *pOrderAction, CZeusingFtdcRspInfoField *pRspInfo) {};

	///��Լ����״̬֪ͨ
	virtual void OnRtnInstrumentStatus(CZeusingFtdcInstrumentStatusField *pInstrumentStatus) {};

	///����֪ͨ
	virtual void OnRtnTradingNotice(CZeusingFtdcTradingNoticeInfoField *pTradingNoticeInfo) {};

	///��ʾ������У�����
	virtual void OnRtnErrorConditionalOrder(CZeusingFtdcErrorConditionalOrderField *pErrorConditionalOrder) {};

	///����¼�����ر�
	virtual void OnErrRtnQuoteInsert(CZeusingFtdcInputQuoteField *pInputQuote, CZeusingFtdcRspInfoField *pRspInfo) {};

	///���۲�������ر�
	virtual void OnErrRtnQuoteAction(CZeusingFtdcQuoteActionField *pQuoteAction, CZeusingFtdcRspInfoField *pRspInfo) {};

	///ѯ��֪ͨ
	virtual void OnRtnForQuote(CZeusingFtdcForQuoteField *pForQuote) {};

	///ѯ�ۻر�
	virtual void OnRtnExchRspForQuote(CZeusingFtdcExchRspForQuoteField *pExchRspForQuote) {};

	///��ʾ������ѯ��ʧ��
	virtual void OnRtnErrExchRtnForQuote(CZeusingFtdcErrRtnExchRtnForQuoteField *pErrRtnExchRtnForQuote) {};

	///�����ѯǩԼ������Ӧ
	virtual void OnRspQryContractBank(CZeusingFtdcContractBankField *pContractBank, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ����֪ͨ��Ӧ
	virtual void OnRspQryTradingNotice(CZeusingFtdcTradingNoticeField *pTradingNotice, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ���͹�˾���ײ�����Ӧ
	virtual void OnRspQryBrokerTradingParams(CZeusingFtdcBrokerTradingParamsField *pBrokerTradingParams, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�����ѯ���͹�˾�����㷨��Ӧ
	virtual void OnRspQryBrokerTradingAlgos(CZeusingFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///���з��������ʽ�ת�ڻ�֪ͨ
	virtual void OnRtnFromBankToFutureByBank(CZeusingFtdcRspTransferField *pRspTransfer) {};

	///���з����ڻ��ʽ�ת����֪ͨ
	virtual void OnRtnFromFutureToBankByBank(CZeusingFtdcRspTransferField *pRspTransfer) {};

	///���з����������ת�ڻ�֪ͨ
	virtual void OnRtnRepealFromBankToFutureByBank(CZeusingFtdcRspRepealField *pRspRepeal) {};

	///���з�������ڻ�ת����֪ͨ
	virtual void OnRtnRepealFromFutureToBankByBank(CZeusingFtdcRspRepealField *pRspRepeal) {};

	///�ڻ����������ʽ�ת�ڻ�֪ͨ
	virtual void OnRtnFromBankToFutureByFuture(CZeusingFtdcRspTransferField *pRspTransfer) {};

	///�ڻ������ڻ��ʽ�ת����֪ͨ
	virtual void OnRtnFromFutureToBankByFuture(CZeusingFtdcRspTransferField *pRspTransfer) {};

	///ϵͳ����ʱ�ڻ����ֹ������������ת�ڻ��������д�����Ϻ��̷��ص�֪ͨ
	virtual void OnRtnRepealFromBankToFutureByFutureManual(CZeusingFtdcRspRepealField *pRspRepeal) {};

	///ϵͳ����ʱ�ڻ����ֹ���������ڻ�ת�����������д�����Ϻ��̷��ص�֪ͨ
	virtual void OnRtnRepealFromFutureToBankByFutureManual(CZeusingFtdcRspRepealField *pRspRepeal) {};

	///�ڻ������ѯ�������֪ͨ
	virtual void OnRtnQueryBankBalanceByFuture(CZeusingFtdcNotifyQueryAccountField *pNotifyQueryAccount) {};

	///�ڻ����������ʽ�ת�ڻ�����ر�
	virtual void OnErrRtnBankToFutureByFuture(CZeusingFtdcReqTransferField *pReqTransfer, CZeusingFtdcRspInfoField *pRspInfo) {};

	///�ڻ������ڻ��ʽ�ת���д���ر�
	virtual void OnErrRtnFutureToBankByFuture(CZeusingFtdcReqTransferField *pReqTransfer, CZeusingFtdcRspInfoField *pRspInfo) {};

	///ϵͳ����ʱ�ڻ����ֹ������������ת�ڻ�����ر�
	virtual void OnErrRtnRepealBankToFutureByFutureManual(CZeusingFtdcReqRepealField *pReqRepeal, CZeusingFtdcRspInfoField *pRspInfo) {};

	///ϵͳ����ʱ�ڻ����ֹ���������ڻ�ת���д���ر�
	virtual void OnErrRtnRepealFutureToBankByFutureManual(CZeusingFtdcReqRepealField *pReqRepeal, CZeusingFtdcRspInfoField *pRspInfo) {};

	///�ڻ������ѯ����������ر�
	virtual void OnErrRtnQueryBankBalanceByFuture(CZeusingFtdcReqQueryAccountField *pReqQueryAccount, CZeusingFtdcRspInfoField *pRspInfo) {};

	///�ڻ������������ת�ڻ��������д�����Ϻ��̷��ص�֪ͨ
	virtual void OnRtnRepealFromBankToFutureByFuture(CZeusingFtdcRspRepealField *pRspRepeal) {};

	///�ڻ���������ڻ�ת�����������д�����Ϻ��̷��ص�֪ͨ
	virtual void OnRtnRepealFromFutureToBankByFuture(CZeusingFtdcRspRepealField *pRspRepeal) {};

	///�ڻ����������ʽ�ת�ڻ�Ӧ��
	virtual void OnRspFromBankToFutureByFuture(CZeusingFtdcReqTransferField *pReqTransfer, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�ڻ������ڻ��ʽ�ת����Ӧ��
	virtual void OnRspFromFutureToBankByFuture(CZeusingFtdcReqTransferField *pReqTransfer, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�ڻ������ѯ�������Ӧ��
	virtual void OnRspQueryBankAccountMoneyByFuture(CZeusingFtdcReqQueryAccountField *pReqQueryAccount, CZeusingFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///���з������ڿ���֪ͨ
	virtual void OnRtnOpenAccountByBank(CZeusingFtdcOpenAccountField *pOpenAccount) {};

	///���з�����������֪ͨ
	virtual void OnRtnCancelAccountByBank(CZeusingFtdcCancelAccountField *pCancelAccount) {};

	///���з����������˺�֪ͨ
	virtual void OnRtnChangeAccountByBank(CZeusingFtdcChangeAccountField *pChangeAccount) {};
};

#ifndef WINDOWS
	#if __GNUC__ >= 4
		#pragma GCC visibility push(default)
	#endif
#endif
class TRADER_API_EXPORT CZeusingFtdcTraderApi
{
public:
	///����TraderApi
	///@param pszFlowPath ����������Ϣ�ļ���Ŀ¼��Ĭ��Ϊ��ǰĿ¼
	///@return ��������UserApi
	//modify for udp marketdata
	static CZeusingFtdcTraderApi *CreateFtdcTraderApi(const char *pszFlowPath = "", const bool bIsUsingUdp=false);
	
	///ɾ���ӿڶ�����
	///@remark ����ʹ�ñ��ӿڶ���ʱ,���øú���ɾ���ӿڶ���
	virtual void Release() = 0;
	
	///��ʼ��
	///@remark ��ʼ�����л���,ֻ�е��ú�,�ӿڲſ�ʼ����
	virtual void Init() = 0;
	
	///�ȴ��ӿ��߳̽�������
	///@return �߳��˳�����
	virtual int Join() = 0;
	
	///��ȡ��ǰ������
	///@retrun ��ȡ���Ľ�����
	///@remark ֻ�е�¼�ɹ���,���ܵõ���ȷ�Ľ�����
	virtual const char *GetTradingDay() = 0;
	
	///ע��ǰ�û������ַ
	///@param pszFrontAddress��ǰ�û������ַ��
	///@remark �����ַ�ĸ�ʽΪ����protocol://ipaddress:port�����磺��tcp://127.0.0.1:17001���� 
	///@remark ��tcp��������Э�飬��127.0.0.1�������������ַ����17001������������˿ںš�
	virtual void RegisterFront(char *pszFrontAddress, unsigned int priority = 0) = 0;
	
	///ע��ص��ӿ�
	///@param pSpi �����Իص��ӿ����ʵ��
	virtual void RegisterSpi(CZeusingFtdcTraderSpi *pSpi) = 0;
	
	///����˽������
	///@param nResumeType ˽�����ش���ʽ  
	///        ZEUSING_TERT_RESTART:�ӱ������տ�ʼ�ش�
	///        ZEUSING_TERT_RESUME:���ϴ��յ�������
	///        ZEUSING_TERT_QUICK:ֻ���͵�¼��˽����������
	///@remark �÷���Ҫ��Init����ǰ���á����������򲻻��յ�˽���������ݡ�
	virtual void SubscribePrivateTopic(ZEUSING_TE_RESUME_TYPE nResumeType) = 0;
	
	///���Ĺ�������
	///@param nResumeType �������ش���ʽ  
	///        ZEUSING_TERT_RESTART:�ӱ������տ�ʼ�ش�
	///        ZEUSING_TERT_RESUME:���ϴ��յ�������
	///        ZEUSING_TERT_QUICK:ֻ���͵�¼�󹫹���������
	///@remark �÷���Ҫ��Init����ǰ���á����������򲻻��յ������������ݡ�
	virtual void SubscribePublicTopic(ZEUSING_TE_RESUME_TYPE nResumeType) = 0;

  ///�ͻ�����֤����
	virtual int ReqAuthenticate(CZeusingFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID) = 0;
	
	///�û���¼����
	virtual int ReqUserLogin(CZeusingFtdcReqUserLoginField *pReqUserLoginField, int nRequestID) = 0;
	

	///�ǳ�����
	virtual int ReqUserLogout(CZeusingFtdcUserLogoutField *pUserLogout, int nRequestID) = 0;

	///�û������������
	virtual int ReqUserPasswordUpdate(CZeusingFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID) = 0;

	///�ʽ��˻������������
	virtual int ReqTradingAccountPasswordUpdate(CZeusingFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID) = 0;

	///����¼������
	virtual int ReqOrderInsert(CZeusingFtdcInputOrderField *pInputOrder, int nRequestID) = 0;

	///Ԥ��¼������
	virtual int ReqParkedOrderInsert(CZeusingFtdcParkedOrderField *pParkedOrder, int nRequestID) = 0;

	///Ԥ�񳷵�¼������
	virtual int ReqParkedOrderAction(CZeusingFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID) = 0;

	///������������
	virtual int ReqOrderAction(CZeusingFtdcInputOrderActionField *pInputOrderAction, int nRequestID) = 0;

	///��ѯ��󱨵���������
	virtual int ReqQueryMaxOrderVolume(CZeusingFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID) = 0;

	///Ͷ���߽�����ȷ��
	virtual int ReqSettlementInfoConfirm(CZeusingFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID) = 0;

	///����ɾ��Ԥ��
	virtual int ReqRemoveParkedOrder(CZeusingFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID) = 0;

	///����ɾ��Ԥ�񳷵�
	virtual int ReqRemoveParkedOrderAction(CZeusingFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID) = 0;

	///ִ������¼������
	virtual int ReqExecOrderInsert(CZeusingFtdcInputExecOrderField *pInputExecOrder, int nRequestID) = 0;

	///ִ�������������
	virtual int ReqExecOrderAction(CZeusingFtdcInputExecOrderActionField *pInputExecOrderAction, int nRequestID) = 0;

	///����¼������
	virtual int ReqQuoteInsert(CZeusingFtdcInputQuoteField *pInputQuote, int nRequestID) = 0;

	///���۲�������
	virtual int ReqQuoteAction(CZeusingFtdcInputQuoteActionField *pInputQuoteAction, int nRequestID) = 0;

	///ѯ�۲�������
	virtual int ReqForQuote(CZeusingFtdcInputForQuoteField *pInputForQuote, int nRequestID) = 0;

	///�����ʽ���ת
	virtual int ReqInternalTransfer(CZeusingFtdcInternalTransferField *pInternalTransfer, int nRequestID) = 0;

	///�����ѯ����
	virtual int ReqQryOrder(CZeusingFtdcQryOrderField *pQryOrder, int nRequestID) = 0;

	///�����ѯ�ɽ�
	virtual int ReqQryTrade(CZeusingFtdcQryTradeField *pQryTrade, int nRequestID) = 0;

	///�����ѯͶ���ֲ߳�
	virtual int ReqQryInvestorPosition(CZeusingFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID) = 0;

	///�����ѯ�ʽ��˻�
	virtual int ReqQryTradingAccount(CZeusingFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID) = 0;

	///�����ѯͶ����
	virtual int ReqQryInvestor(CZeusingFtdcQryInvestorField *pQryInvestor, int nRequestID) = 0;

	///�����ѯ���ױ���
	virtual int ReqQryTradingCode(CZeusingFtdcQryTradingCodeField *pQryTradingCode, int nRequestID) = 0;

	///�����ѯ��Լ��֤����
	virtual int ReqQryInstrumentMarginRate(CZeusingFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID) = 0;

	///�����ѯ��Լ��������
	virtual int ReqQryInstrumentCommissionRate(CZeusingFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID) = 0;

	///�����ѯ������
	virtual int ReqQryExchange(CZeusingFtdcQryExchangeField *pQryExchange, int nRequestID) = 0;

	///�����ѯ��Լ
	virtual int ReqQryInstrument(CZeusingFtdcQryInstrumentField *pQryInstrument, int nRequestID) = 0;

	///�����ѯ����
	virtual int ReqQryDepthMarketData(CZeusingFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID) = 0;

	///�����ѯͶ���߽�����
	virtual int ReqQrySettlementInfo(CZeusingFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID) = 0;

	///�����ѯת������
	virtual int ReqQryTransferBank(CZeusingFtdcQryTransferBankField *pQryTransferBank, int nRequestID) = 0;

	///�����ѯͶ���ֲ߳���ϸ
	virtual int ReqQryInvestorPositionDetail(CZeusingFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID) = 0;

	///�����ѯͶ���ֲ߳���ϸ
	virtual int ReqQryInvestorPositionCombineDetail(CZeusingFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID) = 0;

	///�����ѯ��֤����ϵͳ���͹�˾�ʽ��˻���Կ
	virtual int ReqQryCFMMCTradingAccountKey(CZeusingFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID) = 0;

	///�����ѯ�ֵ��۵���Ϣ
	virtual int ReqQryEWarrantOffset(CZeusingFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID) = 0;

	///�����ѯִ������
	virtual int ReqQryExecOrder(CZeusingFtdcQryExecOrderField *pQryExecOrder, int nRequestID) = 0;

	///�����ѯִ���������
	virtual int ReqQryExecOrderAction(CZeusingFtdcQryExecOrderActionField *pQryExecOrderAction, int nRequestID) = 0;

	///�����ѯ����ִ������
	virtual int ReqQryErrExecOrder(CZeusingFtdcQryErrExecOrderField *pQryErrExecOrder, int nRequestID) = 0;

	///�����ѯ����ִ���������
	virtual int ReqQryErrExecOrderAction(CZeusingFtdcQryErrExecOrderActionField *pQryErrExecOrderAction, int nRequestID) = 0;

	///�����ѯת����ˮ
	virtual int ReqQryTransferSerial(CZeusingFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID) = 0;

	///�����ѯ����ǩԼ��ϵ
	virtual int ReqQryAccountregister(CZeusingFtdcQryAccountregisterField *pQryAccountregister, int nRequestID) = 0;

	///�����ѯǩԼ����
	virtual int ReqQryContractBank(CZeusingFtdcQryContractBankField *pQryContractBank, int nRequestID) = 0;

	///�����ѯ����֪ͨ
	virtual int ReqQryTradingNotice(CZeusingFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID) = 0;

	///�����ѯ���͹�˾���ײ���
	virtual int ReqQryBrokerTradingParams(CZeusingFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID) = 0;

	///�����ѯ���͹�˾�����㷨
	virtual int ReqQryBrokerTradingAlgos(CZeusingFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID) = 0;

	///�ڻ����������ʽ�ת�ڻ�����
	virtual int ReqFromBankToFutureByFuture(CZeusingFtdcReqTransferField *pReqTransfer, int nRequestID) = 0;

	///�ڻ������ڻ��ʽ�ת��������
	virtual int ReqFromFutureToBankByFuture(CZeusingFtdcReqTransferField *pReqTransfer, int nRequestID) = 0;

	///�ڻ������ѯ�����������
	virtual int ReqQueryBankAccountMoneyByFuture(CZeusingFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID) = 0;
protected:
	~CZeusingFtdcTraderApi(){};
};

#ifndef WINDOWS
	#if __GNUC__ >= 4
		#pragma GCC visibility pop
	#endif
#endif
#endif
