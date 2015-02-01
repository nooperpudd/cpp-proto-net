
#if !defined(ZEUSING_FTDCSTRUCT_H)
#define ZEUSING_FTDCSTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZeusingFtdcUserApiDataType.h"

#pragma pack(push)
#pragma pack(1)

///��Ϣ�ַ�
struct CZeusingFtdcDisseminationField
{
	///����ϵ�к�
	TZeusingFtdcSequenceSeriesType	SequenceSeries;
	///���к�
	TZeusingFtdcSequenceNoType	SequenceNo;
};

///�û���¼����
struct CZeusingFtdcReqUserLoginField
{
	///������
	TZeusingFtdcDateType	TradingDay;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///����
	TZeusingFtdcPasswordType	Password;
	///�û��˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	UserProductInfo;
	///�ӿڶ˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	InterfaceProductInfo;
	///Э����Ϣ
	TZeusingFtdcProtocolInfoType	ProtocolInfo;
	///Mac��ַ
	TZeusingFtdcMacAddressType	MacAddress;
	///��̬����
	TZeusingFtdcPasswordType	OneTimePassword;
	///�ն�IP��ַ
	TZeusingFtdcIPAddressType	ClientIPAddress;
};

///�û���¼Ӧ��
struct CZeusingFtdcRspUserLoginField
{
	///������
	TZeusingFtdcDateType	TradingDay;
	///��¼�ɹ�ʱ��
	TZeusingFtdcTimeType	LoginTime;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///����ϵͳ����
	TZeusingFtdcSystemNameType	SystemName;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///��󱨵�����
	TZeusingFtdcOrderRefType	MaxOrderRef;
	///������ʱ��
	TZeusingFtdcTimeType	SHFETime;
	///������ʱ��
	TZeusingFtdcTimeType	DCETime;
	///֣����ʱ��
	TZeusingFtdcTimeType	CZCETime;
	///�н���ʱ��
	TZeusingFtdcTimeType	FFEXTime;
};

///�û��ǳ�����
struct CZeusingFtdcUserLogoutField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
};

///ǿ�ƽ���Ա�˳�
struct CZeusingFtdcForceUserLogoutField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
};

///�ͻ�����֤����
struct CZeusingFtdcReqAuthenticateField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�û��˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	UserProductInfo;
	///��֤��
	TZeusingFtdcAuthCodeType	AuthCode;
};

///�ͻ�����֤��Ӧ
struct CZeusingFtdcRspAuthenticateField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�û��˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	UserProductInfo;
};

///�ͻ�����֤��Ϣ
struct CZeusingFtdcAuthenticationInfoField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�û��˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	UserProductInfo;
	///��֤��Ϣ
	TZeusingFtdcAuthInfoType	AuthInfo;
	///�Ƿ�Ϊ��֤���
	TZeusingFtdcBoolType	IsResult;
};

///����ת�ʱ���ͷ
struct CZeusingFtdcTransferHeaderField
{
	///�汾�ţ�������1.0
	TZeusingFtdcVersionType	Version;
	///���״��룬����
	TZeusingFtdcTradeCodeType	TradeCode;
	///�������ڣ������ʽ��yyyymmdd
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ�䣬�����ʽ��hhmmss
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ�ţ�N/A
	TZeusingFtdcTradeSerialType	TradeSerial;
	///�ڻ���˾���룬����
	TZeusingFtdcFutureIDType	FutureID;
	///���д��룬���ݲ�ѯ���еõ�������
	TZeusingFtdcBankIDType	BankID;
	///���з����Ĵ��룬���ݲ�ѯ���еõ�������
	TZeusingFtdcBankBrchIDType	BankBrchID;
	///����Ա��N/A
	TZeusingFtdcOperNoType	OperNo;
	///�����豸���ͣ�N/A
	TZeusingFtdcDeviceIDType	DeviceID;
	///��¼����N/A
	TZeusingFtdcRecordNumType	RecordNum;
	///�Ự��ţ�N/A
	TZeusingFtdcSessionIDType	SessionID;
	///�����ţ�N/A
	TZeusingFtdcRequestIDType	RequestID;
};

///�����ʽ�ת�ڻ�����TradeCode=202001
struct CZeusingFtdcTransferBankToFutureReqField
{
	///�ڻ��ʽ��˻�
	TZeusingFtdcAccountIDType	FutureAccount;
	///�����־
	TZeusingFtdcFuturePwdFlagType	FuturePwdFlag;
	///����
	TZeusingFtdcFutureAccPwdType	FutureAccPwd;
	///ת�˽��
	TZeusingFtdcMoneyType	TradeAmt;
	///�ͻ�������
	TZeusingFtdcMoneyType	CustFee;
	///���֣�RMB-����� USD-��Բ HKD-��Ԫ
	TZeusingFtdcCurrencyCodeType	CurrencyCode;
};

///�����ʽ�ת�ڻ�������Ӧ
struct CZeusingFtdcTransferBankToFutureRspField
{
	///��Ӧ����
	TZeusingFtdcRetCodeType	RetCode;
	///��Ӧ��Ϣ
	TZeusingFtdcRetInfoType	RetInfo;
	///�ʽ��˻�
	TZeusingFtdcAccountIDType	FutureAccount;
	///ת�ʽ��
	TZeusingFtdcMoneyType	TradeAmt;
	///Ӧ�տͻ�������
	TZeusingFtdcMoneyType	CustFee;
	///����
	TZeusingFtdcCurrencyCodeType	CurrencyCode;
};

///�ڻ��ʽ�ת��������TradeCode=202002
struct CZeusingFtdcTransferFutureToBankReqField
{
	///�ڻ��ʽ��˻�
	TZeusingFtdcAccountIDType	FutureAccount;
	///�����־
	TZeusingFtdcFuturePwdFlagType	FuturePwdFlag;
	///����
	TZeusingFtdcFutureAccPwdType	FutureAccPwd;
	///ת�˽��
	TZeusingFtdcMoneyType	TradeAmt;
	///�ͻ�������
	TZeusingFtdcMoneyType	CustFee;
	///���֣�RMB-����� USD-��Բ HKD-��Ԫ
	TZeusingFtdcCurrencyCodeType	CurrencyCode;
};

///�ڻ��ʽ�ת����������Ӧ
struct CZeusingFtdcTransferFutureToBankRspField
{
	///��Ӧ����
	TZeusingFtdcRetCodeType	RetCode;
	///��Ӧ��Ϣ
	TZeusingFtdcRetInfoType	RetInfo;
	///�ʽ��˻�
	TZeusingFtdcAccountIDType	FutureAccount;
	///ת�ʽ��
	TZeusingFtdcMoneyType	TradeAmt;
	///Ӧ�տͻ�������
	TZeusingFtdcMoneyType	CustFee;
	///����
	TZeusingFtdcCurrencyCodeType	CurrencyCode;
};

///��ѯ�����ʽ�����TradeCode=204002
struct CZeusingFtdcTransferQryBankReqField
{
	///�ڻ��ʽ��˻�
	TZeusingFtdcAccountIDType	FutureAccount;
	///�����־
	TZeusingFtdcFuturePwdFlagType	FuturePwdFlag;
	///����
	TZeusingFtdcFutureAccPwdType	FutureAccPwd;
	///���֣�RMB-����� USD-��Բ HKD-��Ԫ
	TZeusingFtdcCurrencyCodeType	CurrencyCode;
};

///��ѯ�����ʽ�������Ӧ
struct CZeusingFtdcTransferQryBankRspField
{
	///��Ӧ����
	TZeusingFtdcRetCodeType	RetCode;
	///��Ӧ��Ϣ
	TZeusingFtdcRetInfoType	RetInfo;
	///�ʽ��˻�
	TZeusingFtdcAccountIDType	FutureAccount;
	///�������
	TZeusingFtdcMoneyType	TradeAmt;
	///���п������
	TZeusingFtdcMoneyType	UseAmt;
	///���п�ȡ���
	TZeusingFtdcMoneyType	FetchAmt;
	///����
	TZeusingFtdcCurrencyCodeType	CurrencyCode;
};

///��ѯ���н�����ϸ����TradeCode=204999
struct CZeusingFtdcTransferQryDetailReqField
{
	///�ڻ��ʽ��˻�
	TZeusingFtdcAccountIDType	FutureAccount;
};

///��ѯ���н�����ϸ������Ӧ
struct CZeusingFtdcTransferQryDetailRspField
{
	///��������
	TZeusingFtdcDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///���״���
	TZeusingFtdcTradeCodeType	TradeCode;
	///�ڻ���ˮ��
	TZeusingFtdcTradeSerialNoType	FutureSerial;
	///�ڻ���˾����
	TZeusingFtdcFutureIDType	FutureID;
	///�ʽ��ʺ�
	TZeusingFtdcFutureAccountType	FutureAccount;
	///������ˮ��
	TZeusingFtdcTradeSerialNoType	BankSerial;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з����Ĵ���
	TZeusingFtdcBankBrchIDType	BankBrchID;
	///�����˺�
	TZeusingFtdcBankAccountType	BankAccount;
	///֤������
	TZeusingFtdcCertCodeType	CertCode;
	///���Ҵ���
	TZeusingFtdcCurrencyCodeType	CurrencyCode;
	///�������
	TZeusingFtdcMoneyType	TxAmount;
	///��Ч��־
	TZeusingFtdcTransferValidFlagType	Flag;
};

///��Ӧ��Ϣ
struct CZeusingFtdcRspInfoField
{
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///������
struct CZeusingFtdcExchangeField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///����������
	TZeusingFtdcExchangeNameType	ExchangeName;
	///����������
	TZeusingFtdcExchangePropertyType	ExchangeProperty;
};

///��Ʒ
struct CZeusingFtdcProductField
{
	///��Ʒ����
	TZeusingFtdcInstrumentIDType	ProductID;
	///��Ʒ����
	TZeusingFtdcProductNameType	ProductName;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ʒ����
	TZeusingFtdcProductClassType	ProductClass;
	///��Լ��������
	TZeusingFtdcVolumeMultipleType	VolumeMultiple;
	///��С�䶯��λ
	TZeusingFtdcPriceType	PriceTick;
	///�м۵�����µ���
	TZeusingFtdcVolumeType	MaxMarketOrderVolume;
	///�м۵���С�µ���
	TZeusingFtdcVolumeType	MinMarketOrderVolume;
	///�޼۵�����µ���
	TZeusingFtdcVolumeType	MaxLimitOrderVolume;
	///�޼۵���С�µ���
	TZeusingFtdcVolumeType	MinLimitOrderVolume;
	///�ֲ�����
	TZeusingFtdcPositionTypeType	PositionType;
	///�ֲ���������
	TZeusingFtdcPositionDateTypeType	PositionDateType;
	///ƽ�ִ�������
	TZeusingFtdcCloseDealTypeType	CloseDealType;
};

///��Լ
struct CZeusingFtdcInstrumentField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��Լ����
	TZeusingFtdcInstrumentNameType	InstrumentName;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///��Ʒ����
	TZeusingFtdcInstrumentIDType	ProductID;
	///��Ʒ����
	TZeusingFtdcProductClassType	ProductClass;
	///�������
	TZeusingFtdcYearType	DeliveryYear;
	///������
	TZeusingFtdcMonthType	DeliveryMonth;
	///�м۵�����µ���
	TZeusingFtdcVolumeType	MaxMarketOrderVolume;
	///�м۵���С�µ���
	TZeusingFtdcVolumeType	MinMarketOrderVolume;
	///�޼۵�����µ���
	TZeusingFtdcVolumeType	MaxLimitOrderVolume;
	///�޼۵���С�µ���
	TZeusingFtdcVolumeType	MinLimitOrderVolume;
	///��Լ��������
	TZeusingFtdcVolumeMultipleType	VolumeMultiple;
	///��С�䶯��λ
	TZeusingFtdcPriceType	PriceTick;
	///������
	TZeusingFtdcDateType	CreateDate;
	///������
	TZeusingFtdcDateType	OpenDate;
	///������
	TZeusingFtdcDateType	ExpireDate;
	///��ʼ������
	TZeusingFtdcDateType	StartDelivDate;
	///����������
	TZeusingFtdcDateType	EndDelivDate;
	///��Լ��������״̬
	TZeusingFtdcInstLifePhaseType	InstLifePhase;
	///��ǰ�Ƿ���
	TZeusingFtdcBoolType	IsTrading;
	///�ֲ�����
	TZeusingFtdcPositionTypeType	PositionType;
	///�ֲ���������
	TZeusingFtdcPositionDateTypeType	PositionDateType;
	///�Ƿ�ʹ�ô��߱�֤���㷨
	TZeusingFtdcMaxMarginSideAlgorithmType	MaxMarginSideAlgorithm;
	///������Ʒ����
	TZeusingFtdcInstrumentIDType	UnderlyingInstrID;
	///ִ�м�
	TZeusingFtdcPriceType	StrikePrice;
	///��Ȩ����
	TZeusingFtdcOptionsTypeType	OptionsType;
	///��Լ������Ʒ����
	TZeusingFtdcVolumeMultipleType	UnderlyingMultiple;
};

///���͹�˾
struct CZeusingFtdcBrokerField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///���͹�˾���
	TZeusingFtdcBrokerAbbrType	BrokerAbbr;
	///���͹�˾����
	TZeusingFtdcBrokerNameType	BrokerName;
	///�Ƿ��Ծ
	TZeusingFtdcBoolType	IsActive;
};

///����������Ա
struct CZeusingFtdcTraderField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///����
	TZeusingFtdcPasswordType	Password;
	///��װ����
	TZeusingFtdcInstallCountType	InstallCount;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
};

///Ͷ����
struct CZeusingFtdcInvestorField
{
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���߷������
	TZeusingFtdcInvestorIDType	InvestorGroupID;
	///Ͷ��������
	TZeusingFtdcPartyNameType	InvestorName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdentifiedCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�Ƿ��Ծ
	TZeusingFtdcBoolType	IsActive;
	///��ϵ�绰
	TZeusingFtdcTelephoneType	Telephone;
	///ͨѶ��ַ
	TZeusingFtdcAddressType	Address;
	///��������
	TZeusingFtdcDateType	OpenDate;
	///�ֻ�
	TZeusingFtdcMobileType	Mobile;
	///��������ģ�����
	TZeusingFtdcInvestorIDType	CommModelID;
	///����ֶ�
	TZeusingFtdcPadFieldType	PadField;
	///��֤����ģ�����
	TZeusingFtdcInvestorIDType	MarginModelID;
};

///���ױ���
struct CZeusingFtdcTradingCodeField
{
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///�Ƿ��Ծ
	TZeusingFtdcBoolType	IsActive;
	///���ױ�������
	TZeusingFtdcClientIDTypeType	ClientIDType;
};

///��Ա����;��͹�˾������ձ�
struct CZeusingFtdcPartBrokerField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�Ƿ��Ծ
	TZeusingFtdcBoolType	IsActive;
};

///�����û�
struct CZeusingFtdcSuperUserField
{
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�û�����
	TZeusingFtdcUserNameType	UserName;
	///����
	TZeusingFtdcPasswordType	Password;
	///�Ƿ��Ծ
	TZeusingFtdcBoolType	IsActive;
};

///�����û�����Ȩ��
struct CZeusingFtdcSuperUserFunctionField
{
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///���ܴ���
	TZeusingFtdcFunctionCodeType	FunctionCode;
};

///Ͷ������
struct CZeusingFtdcInvestorGroupField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���߷������
	TZeusingFtdcInvestorIDType	InvestorGroupID;
	///Ͷ���߷�������
	TZeusingFtdcInvestorGroupNameType	InvestorGroupName;
};

///�ʽ��˻�
struct CZeusingFtdcTradingAccountField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ϴ���Ѻ���
	TZeusingFtdcMoneyType	PreMortgage;
	///�ϴ����ö��
	TZeusingFtdcMoneyType	PreCredit;
	///�ϴδ���
	TZeusingFtdcMoneyType	PreDeposit;
	///�ϴν���׼����
	TZeusingFtdcMoneyType	PreBalance;
	///�ϴ�ռ�õı�֤��
	TZeusingFtdcMoneyType	PreMargin;
	///��Ϣ����
	TZeusingFtdcMoneyType	InterestBase;
	///��Ϣ����
	TZeusingFtdcMoneyType	Interest;
	///�����
	TZeusingFtdcMoneyType	Deposit;
	///������
	TZeusingFtdcMoneyType	Withdraw;
	///����ı�֤��
	TZeusingFtdcMoneyType	FrozenMargin;
	///������ʽ�
	TZeusingFtdcMoneyType	FrozenCash;
	///�����������
	TZeusingFtdcMoneyType	FrozenCommission;
	///��ǰ��֤���ܶ�
	TZeusingFtdcMoneyType	CurrMargin;
	///�ʽ���
	TZeusingFtdcMoneyType	CashIn;
	///������
	TZeusingFtdcMoneyType	Commission;
	///ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfit;
	///�ֲ�ӯ��
	TZeusingFtdcMoneyType	PositionProfit;
	///�ڻ�����׼����
	TZeusingFtdcMoneyType	Balance;
	///�����ʽ�
	TZeusingFtdcMoneyType	Available;
	///��ȡ�ʽ�
	TZeusingFtdcMoneyType	WithdrawQuota;
	///����׼����
	TZeusingFtdcMoneyType	Reserve;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///���ö��
	TZeusingFtdcMoneyType	Credit;
	///��Ѻ���
	TZeusingFtdcMoneyType	Mortgage;
	///��������֤��
	TZeusingFtdcMoneyType	ExchangeMargin;
	///Ͷ���߽��֤��
	TZeusingFtdcMoneyType	DeliveryMargin;
	///���������֤��
	TZeusingFtdcMoneyType	ExchangeDeliveryMargin;
	///�����ڻ�����׼����
	TZeusingFtdcMoneyType	ReserveBalance;
	///��Ȩƽ��ӯ��
	TZeusingFtdcMoneyType	OptionCloseProfit;
	///�ʻ�����
	TZeusingFtdcAccountLevelType	AccountLevel;
};

///Ͷ���ֲ߳�
struct CZeusingFtdcInvestorPositionField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///�ֲֶ�շ���
	TZeusingFtdcPosiDirectionType	PosiDirection;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///�ֲ�����
	TZeusingFtdcPositionDateType	PositionDate;
	///���ճֲ�
	TZeusingFtdcVolumeType	YdPosition;
	///���ճֲ�
	TZeusingFtdcVolumeType	Position;
	///��ͷ����
	TZeusingFtdcVolumeType	LongFrozen;
	///��ͷ����
	TZeusingFtdcVolumeType	ShortFrozen;
	///���ֶ�����
	TZeusingFtdcMoneyType	LongFrozenAmount;
	///���ֶ�����
	TZeusingFtdcMoneyType	ShortFrozenAmount;
	///������
	TZeusingFtdcVolumeType	OpenVolume;
	///ƽ����
	TZeusingFtdcVolumeType	CloseVolume;
	///���ֽ��
	TZeusingFtdcMoneyType	OpenAmount;
	///ƽ�ֽ��
	TZeusingFtdcMoneyType	CloseAmount;
	///ƽ��ֽ��
	TZeusingFtdcMoneyType	YdCloseAmount;
	///ƽ�������
	TZeusingFtdcVolumeType	YdCloseVolume;
	///�ֲֳɱ�
	TZeusingFtdcMoneyType	PositionCost;
	///�ϴ�ռ�õı�֤��
	TZeusingFtdcMoneyType	PreMargin;
	///ռ�õı�֤��
	TZeusingFtdcMoneyType	UseMargin;
	///����ı�֤��
	TZeusingFtdcMoneyType	FrozenMargin;
	///������ʽ�
	TZeusingFtdcMoneyType	FrozenCash;
	///�����������
	TZeusingFtdcMoneyType	FrozenCommission;
	///�ʽ���
	TZeusingFtdcMoneyType	CashIn;
	///������
	TZeusingFtdcMoneyType	Commission;
	///ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfit;
	///�ֲ�ӯ��
	TZeusingFtdcMoneyType	PositionProfit;
	///�ϴν����
	TZeusingFtdcPriceType	PreSettlementPrice;
	///���ν����
	TZeusingFtdcPriceType	SettlementPrice;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///���ֳɱ�
	TZeusingFtdcMoneyType	OpenCost;
	///��������֤��
	TZeusingFtdcMoneyType	ExchangeMargin;
	///��ϳɽ��γɵĳֲ�
	TZeusingFtdcVolumeType	CombPosition;
	///��϶�ͷ����
	TZeusingFtdcVolumeType	CombLongFrozen;
	///��Ͽ�ͷ����
	TZeusingFtdcVolumeType	CombShortFrozen;
	///���ն���ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfitByDate;
	///��ʶԳ�ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfitByTrade;
	///���ճֲ�
	TZeusingFtdcVolumeType	TodayPosition;
	///��֤����
	TZeusingFtdcRatioType	MarginRateByMoney;
	///��֤����(������)
	TZeusingFtdcRatioType	MarginRateByVolume;
	///ִ�ж���
	TZeusingFtdcVolumeType	StrikeFrozen;
	///ִ�ж�����
	TZeusingFtdcMoneyType	StrikeFrozenAmount;
	///����ִ�ж���
	TZeusingFtdcVolumeType	AbandonFrozen;
};

///��Լ��֤����
struct CZeusingFtdcInstrumentMarginRateField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///��ͷ��֤����
	TZeusingFtdcRatioType	LongMarginRatioByMoney;
	///��ͷ��֤���
	TZeusingFtdcMoneyType	LongMarginRatioByVolume;
	///��ͷ��֤����
	TZeusingFtdcRatioType	ShortMarginRatioByMoney;
	///��ͷ��֤���
	TZeusingFtdcMoneyType	ShortMarginRatioByVolume;
	///�Ƿ���Խ�������ȡ
	TZeusingFtdcBoolType	IsRelative;
};

///��Լ��������
struct CZeusingFtdcInstrumentCommissionRateField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///������������
	TZeusingFtdcRatioType	OpenRatioByMoney;
	///����������
	TZeusingFtdcRatioType	OpenRatioByVolume;
	///ƽ����������
	TZeusingFtdcRatioType	CloseRatioByMoney;
	///ƽ��������
	TZeusingFtdcRatioType	CloseRatioByVolume;
	///ƽ����������
	TZeusingFtdcRatioType	CloseTodayRatioByMoney;
	///ƽ��������
	TZeusingFtdcRatioType	CloseTodayRatioByVolume;
};

///�������
struct CZeusingFtdcDepthMarketDataField
{
	///������
	TZeusingFtdcDateType	TradingDay;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///���¼�
	TZeusingFtdcPriceType	LastPrice;
	///�ϴν����
	TZeusingFtdcPriceType	PreSettlementPrice;
	///������
	TZeusingFtdcPriceType	PreClosePrice;
	///��ֲ���
	TZeusingFtdcLargeVolumeType	PreOpenInterest;
	///����
	TZeusingFtdcPriceType	OpenPrice;
	///��߼�
	TZeusingFtdcPriceType	HighestPrice;
	///��ͼ�
	TZeusingFtdcPriceType	LowestPrice;
	///����
	TZeusingFtdcVolumeType	Volume;
	///�ɽ����
	TZeusingFtdcMoneyType	Turnover;
	///�ֲ���
	TZeusingFtdcLargeVolumeType	OpenInterest;
	///������
	TZeusingFtdcPriceType	ClosePrice;
	///���ν����
	TZeusingFtdcPriceType	SettlementPrice;
	///��ͣ���
	TZeusingFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TZeusingFtdcPriceType	LowerLimitPrice;
	///����ʵ��
	TZeusingFtdcRatioType	PreDelta;
	///����ʵ��
	TZeusingFtdcRatioType	CurrDelta;
	///����޸�ʱ��
	TZeusingFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TZeusingFtdcMillisecType	UpdateMillisec;
	///�����һ
	TZeusingFtdcPriceType	BidPrice1;
	///������һ
	TZeusingFtdcVolumeType	BidVolume1;
	///������һ
	TZeusingFtdcPriceType	AskPrice1;
	///������һ
	TZeusingFtdcVolumeType	AskVolume1;
	///����۶�
	TZeusingFtdcPriceType	BidPrice2;
	///��������
	TZeusingFtdcVolumeType	BidVolume2;
	///�����۶�
	TZeusingFtdcPriceType	AskPrice2;
	///��������
	TZeusingFtdcVolumeType	AskVolume2;
	///�������
	TZeusingFtdcPriceType	BidPrice3;
	///��������
	TZeusingFtdcVolumeType	BidVolume3;
	///��������
	TZeusingFtdcPriceType	AskPrice3;
	///��������
	TZeusingFtdcVolumeType	AskVolume3;
	///�������
	TZeusingFtdcPriceType	BidPrice4;
	///��������
	TZeusingFtdcVolumeType	BidVolume4;
	///��������
	TZeusingFtdcPriceType	AskPrice4;
	///��������
	TZeusingFtdcVolumeType	AskVolume4;
	///�������
	TZeusingFtdcPriceType	BidPrice5;
	///��������
	TZeusingFtdcVolumeType	BidVolume5;
	///��������
	TZeusingFtdcPriceType	AskPrice5;
	///��������
	TZeusingFtdcVolumeType	AskVolume5;
	///���վ���
	TZeusingFtdcPriceType	AveragePrice;
	///ҵ������
	TZeusingFtdcDateType	ActionDay;
};

///Ͷ���ߺ�Լ����Ȩ��
struct CZeusingFtdcInstrumentTradingRightField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����Ȩ��
	TZeusingFtdcTradingRightType	TradingRight;
	///���۽���Ȩ��
	TZeusingFtdcTradingRightType	QuoteTradingRight;
};

///���͹�˾�û�
struct CZeusingFtdcBrokerUserField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�û�����
	TZeusingFtdcUserNameType	UserName;
	///�û�����
	TZeusingFtdcUserTypeType	UserType;
	///�Ƿ��Ծ
	TZeusingFtdcBoolType	IsActive;
	///�Ƿ�ʹ������
	TZeusingFtdcBoolType	IsUsingOTP;
};

///���͹�˾�û�����
struct CZeusingFtdcBrokerUserPasswordField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///����
	TZeusingFtdcPasswordType	Password;
};

///���͹�˾�û�����Ȩ��
struct CZeusingFtdcBrokerUserFunctionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///���͹�˾���ܴ���
	TZeusingFtdcBrokerFunctionCodeType	BrokerFunctionCode;
};

///����������Ա���̻�
struct CZeusingFtdcTraderOfferField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///����
	TZeusingFtdcPasswordType	Password;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///����������Ա����״̬
	TZeusingFtdcTraderConnectStatusType	TraderConnectStatus;
	///�����������������
	TZeusingFtdcDateType	ConnectRequestDate;
	///�������������ʱ��
	TZeusingFtdcTimeType	ConnectRequestTime;
	///�ϴα�������
	TZeusingFtdcDateType	LastReportDate;
	///�ϴα���ʱ��
	TZeusingFtdcTimeType	LastReportTime;
	///�����������
	TZeusingFtdcDateType	ConnectDate;
	///�������ʱ��
	TZeusingFtdcTimeType	ConnectTime;
	///��������
	TZeusingFtdcDateType	StartDate;
	///����ʱ��
	TZeusingFtdcTimeType	StartTime;
	///������
	TZeusingFtdcDateType	TradingDay;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///��ϯλ���ɽ����
	TZeusingFtdcTradeIDType	MaxTradeID;
	///��ϯλ��󱨵�����
	TZeusingFtdcReturnCodeType	MaxOrderMessageReference;
};

///Ͷ���߽�����
struct CZeusingFtdcSettlementInfoField
{
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///���
	TZeusingFtdcSequenceNoType	SequenceNo;
	///��Ϣ����
	TZeusingFtdcContentType	Content;
};

///��Լ��֤���ʵ���
struct CZeusingFtdcInstrumentMarginRateAdjustField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///��ͷ��֤����
	TZeusingFtdcRatioType	LongMarginRatioByMoney;
	///��ͷ��֤���
	TZeusingFtdcMoneyType	LongMarginRatioByVolume;
	///��ͷ��֤����
	TZeusingFtdcRatioType	ShortMarginRatioByMoney;
	///��ͷ��֤���
	TZeusingFtdcMoneyType	ShortMarginRatioByVolume;
	///�Ƿ���Խ�������ȡ
	TZeusingFtdcBoolType	IsRelative;
};

///��������֤����
struct CZeusingFtdcExchangeMarginRateField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///��ͷ��֤����
	TZeusingFtdcRatioType	LongMarginRatioByMoney;
	///��ͷ��֤���
	TZeusingFtdcMoneyType	LongMarginRatioByVolume;
	///��ͷ��֤����
	TZeusingFtdcRatioType	ShortMarginRatioByMoney;
	///��ͷ��֤���
	TZeusingFtdcMoneyType	ShortMarginRatioByVolume;
};

///��������֤���ʵ���
struct CZeusingFtdcExchangeMarginRateAdjustField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///���潻����Ͷ���߶�ͷ��֤����
	TZeusingFtdcRatioType	LongMarginRatioByMoney;
	///���潻����Ͷ���߶�ͷ��֤���
	TZeusingFtdcMoneyType	LongMarginRatioByVolume;
	///���潻����Ͷ���߿�ͷ��֤����
	TZeusingFtdcRatioType	ShortMarginRatioByMoney;
	///���潻����Ͷ���߿�ͷ��֤���
	TZeusingFtdcMoneyType	ShortMarginRatioByVolume;
	///��������ͷ��֤����
	TZeusingFtdcRatioType	ExchLongMarginRatioByMoney;
	///��������ͷ��֤���
	TZeusingFtdcMoneyType	ExchLongMarginRatioByVolume;
	///��������ͷ��֤����
	TZeusingFtdcRatioType	ExchShortMarginRatioByMoney;
	///��������ͷ��֤���
	TZeusingFtdcMoneyType	ExchShortMarginRatioByVolume;
	///�����潻����Ͷ���߶�ͷ��֤����
	TZeusingFtdcRatioType	NoLongMarginRatioByMoney;
	///�����潻����Ͷ���߶�ͷ��֤���
	TZeusingFtdcMoneyType	NoLongMarginRatioByVolume;
	///�����潻����Ͷ���߿�ͷ��֤����
	TZeusingFtdcRatioType	NoShortMarginRatioByMoney;
	///�����潻����Ͷ���߿�ͷ��֤���
	TZeusingFtdcMoneyType	NoShortMarginRatioByVolume;
};

///��������
struct CZeusingFtdcSettlementRefField
{
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
};

///��ǰʱ��
struct CZeusingFtdcCurrentTimeField
{
	///��ǰ����
	TZeusingFtdcDateType	CurrDate;
	///��ǰʱ��
	TZeusingFtdcTimeType	CurrTime;
	///��ǰʱ�䣨���룩
	TZeusingFtdcMillisecType	CurrMillisec;
	///ҵ������
	TZeusingFtdcDateType	ActionDay;
};

///ͨѶ�׶�
struct CZeusingFtdcCommPhaseField
{
	///������
	TZeusingFtdcDateType	TradingDay;
	///ͨѶʱ�α��
	TZeusingFtdcCommPhaseNoType	CommPhaseNo;
	///ϵͳ���
	TZeusingFtdcSystemIDType	SystemID;
};

///��¼��Ϣ
struct CZeusingFtdcLoginInfoField
{
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��¼����
	TZeusingFtdcDateType	LoginDate;
	///��¼ʱ��
	TZeusingFtdcTimeType	LoginTime;
	///IP��ַ
	TZeusingFtdcIPAddressType	IPAddress;
	///�û��˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	UserProductInfo;
	///�ӿڶ˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	InterfaceProductInfo;
	///Э����Ϣ
	TZeusingFtdcProtocolInfoType	ProtocolInfo;
	///ϵͳ����
	TZeusingFtdcSystemNameType	SystemName;
	///����
	TZeusingFtdcPasswordType	Password;
	///��󱨵�����
	TZeusingFtdcOrderRefType	MaxOrderRef;
	///������ʱ��
	TZeusingFtdcTimeType	SHFETime;
	///������ʱ��
	TZeusingFtdcTimeType	DCETime;
	///֣����ʱ��
	TZeusingFtdcTimeType	CZCETime;
	///�н���ʱ��
	TZeusingFtdcTimeType	FFEXTime;
	///Mac��ַ
	TZeusingFtdcMacAddressType	MacAddress;
	///��̬����
	TZeusingFtdcPasswordType	OneTimePassword;
};

///��¼��Ϣ
struct CZeusingFtdcLogoutAllField
{
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///ϵͳ����
	TZeusingFtdcSystemNameType	SystemName;
};

///ǰ��״̬
struct CZeusingFtdcFrontStatusField
{
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�ϴα�������
	TZeusingFtdcDateType	LastReportDate;
	///�ϴα���ʱ��
	TZeusingFtdcTimeType	LastReportTime;
	///�Ƿ��Ծ
	TZeusingFtdcBoolType	IsActive;
};

///�û�������
struct CZeusingFtdcUserPasswordUpdateField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///ԭ���Ŀ���
	TZeusingFtdcPasswordType	OldPassword;
	///�µĿ���
	TZeusingFtdcPasswordType	NewPassword;
};

///���뱨��
struct CZeusingFtdcInputOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�����۸�����
	TZeusingFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///��Ͽ�ƽ��־
	TZeusingFtdcCombOffsetFlagType	CombOffsetFlag;
	///���Ͷ���ױ���־
	TZeusingFtdcCombHedgeFlagType	CombHedgeFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///����
	TZeusingFtdcVolumeType	VolumeTotalOriginal;
	///��Ч������
	TZeusingFtdcTimeConditionType	TimeCondition;
	///GTD����
	TZeusingFtdcDateType	GTDDate;
	///�ɽ�������
	TZeusingFtdcVolumeConditionType	VolumeCondition;
	///��С�ɽ���
	TZeusingFtdcVolumeType	MinVolume;
	///��������
	TZeusingFtdcContingentConditionType	ContingentCondition;
	///ֹ���
	TZeusingFtdcPriceType	StopPrice;
	///ǿƽԭ��
	TZeusingFtdcForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	TZeusingFtdcBoolType	IsAutoSuspend;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///�û�ǿ����־
	TZeusingFtdcBoolType	UserForceClose;
	///��������־
	TZeusingFtdcBoolType	IsSwapOrder;
};

///����
struct CZeusingFtdcOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�����۸�����
	TZeusingFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///��Ͽ�ƽ��־
	TZeusingFtdcCombOffsetFlagType	CombOffsetFlag;
	///���Ͷ���ױ���־
	TZeusingFtdcCombHedgeFlagType	CombHedgeFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///����
	TZeusingFtdcVolumeType	VolumeTotalOriginal;
	///��Ч������
	TZeusingFtdcTimeConditionType	TimeCondition;
	///GTD����
	TZeusingFtdcDateType	GTDDate;
	///�ɽ�������
	TZeusingFtdcVolumeConditionType	VolumeCondition;
	///��С�ɽ���
	TZeusingFtdcVolumeType	MinVolume;
	///��������
	TZeusingFtdcContingentConditionType	ContingentCondition;
	///ֹ���
	TZeusingFtdcPriceType	StopPrice;
	///ǿƽԭ��
	TZeusingFtdcForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	TZeusingFtdcBoolType	IsAutoSuspend;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�����ύ״̬
	TZeusingFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///������ʾ���
	TZeusingFtdcSequenceNoType	NotifySequence;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///������Դ
	TZeusingFtdcOrderSourceType	OrderSource;
	///����״̬
	TZeusingFtdcOrderStatusType	OrderStatus;
	///��������
	TZeusingFtdcOrderTypeType	OrderType;
	///��ɽ�����
	TZeusingFtdcVolumeType	VolumeTraded;
	///ʣ������
	TZeusingFtdcVolumeType	VolumeTotal;
	///��������
	TZeusingFtdcDateType	InsertDate;
	///ί��ʱ��
	TZeusingFtdcTimeType	InsertTime;
	///����ʱ��
	TZeusingFtdcTimeType	ActiveTime;
	///����ʱ��
	TZeusingFtdcTimeType	SuspendTime;
	///����޸�ʱ��
	TZeusingFtdcTimeType	UpdateTime;
	///����ʱ��
	TZeusingFtdcTimeType	CancelTime;
	///����޸Ľ���������Ա����
	TZeusingFtdcTraderIDType	ActiveTraderID;
	///�����Ա���
	TZeusingFtdcParticipantIDType	ClearingPartID;
	///���
	TZeusingFtdcSequenceNoType	SequenceNo;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///�û��˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	UserProductInfo;
	///״̬��Ϣ
	TZeusingFtdcErrorMsgType	StatusMsg;
	///�û�ǿ����־
	TZeusingFtdcBoolType	UserForceClose;
	///�����û�����
	TZeusingFtdcUserIDType	ActiveUserID;
	///���͹�˾�������
	TZeusingFtdcSequenceNoType	BrokerOrderSeq;
	///��ر���
	TZeusingFtdcOrderSysIDType	RelativeOrderSysID;
	///֣�����ɽ�����
	TZeusingFtdcVolumeType	ZCETotalTradedVolume;
	///��������־
	TZeusingFtdcBoolType	IsSwapOrder;
	///���ױ�������
	TZeusingFtdcClientIDTypeType	ClientIDType;
};

///����������
struct CZeusingFtdcExchangeOrderField
{
	///�����۸�����
	TZeusingFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///��Ͽ�ƽ��־
	TZeusingFtdcCombOffsetFlagType	CombOffsetFlag;
	///���Ͷ���ױ���־
	TZeusingFtdcCombHedgeFlagType	CombHedgeFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///����
	TZeusingFtdcVolumeType	VolumeTotalOriginal;
	///��Ч������
	TZeusingFtdcTimeConditionType	TimeCondition;
	///GTD����
	TZeusingFtdcDateType	GTDDate;
	///�ɽ�������
	TZeusingFtdcVolumeConditionType	VolumeCondition;
	///��С�ɽ���
	TZeusingFtdcVolumeType	MinVolume;
	///��������
	TZeusingFtdcContingentConditionType	ContingentCondition;
	///ֹ���
	TZeusingFtdcPriceType	StopPrice;
	///ǿƽԭ��
	TZeusingFtdcForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	TZeusingFtdcBoolType	IsAutoSuspend;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�����ύ״̬
	TZeusingFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///������ʾ���
	TZeusingFtdcSequenceNoType	NotifySequence;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///������Դ
	TZeusingFtdcOrderSourceType	OrderSource;
	///����״̬
	TZeusingFtdcOrderStatusType	OrderStatus;
	///��������
	TZeusingFtdcOrderTypeType	OrderType;
	///��ɽ�����
	TZeusingFtdcVolumeType	VolumeTraded;
	///ʣ������
	TZeusingFtdcVolumeType	VolumeTotal;
	///��������
	TZeusingFtdcDateType	InsertDate;
	///ί��ʱ��
	TZeusingFtdcTimeType	InsertTime;
	///����ʱ��
	TZeusingFtdcTimeType	ActiveTime;
	///����ʱ��
	TZeusingFtdcTimeType	SuspendTime;
	///����޸�ʱ��
	TZeusingFtdcTimeType	UpdateTime;
	///����ʱ��
	TZeusingFtdcTimeType	CancelTime;
	///����޸Ľ���������Ա����
	TZeusingFtdcTraderIDType	ActiveTraderID;
	///�����Ա���
	TZeusingFtdcParticipantIDType	ClearingPartID;
	///���
	TZeusingFtdcSequenceNoType	SequenceNo;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
};

///��������������ʧ��
struct CZeusingFtdcExchangeOrderInsertErrorField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///���뱨������
struct CZeusingFtdcInputOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///������������
	TZeusingFtdcOrderActionRefType	OrderActionRef;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///�����仯
	TZeusingFtdcVolumeType	VolumeChange;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///��������
struct CZeusingFtdcOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///������������
	TZeusingFtdcOrderActionRefType	OrderActionRef;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///�����仯
	TZeusingFtdcVolumeType	VolumeChange;
	///��������
	TZeusingFtdcDateType	ActionDate;
	///����ʱ��
	TZeusingFtdcTimeType	ActionTime;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�������ر��
	TZeusingFtdcOrderLocalIDType	ActionLocalID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///��������״̬
	TZeusingFtdcOrderActionStatusType	OrderActionStatus;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///״̬��Ϣ
	TZeusingFtdcErrorMsgType	StatusMsg;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///��������������
struct CZeusingFtdcExchangeOrderActionField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///�����仯
	TZeusingFtdcVolumeType	VolumeChange;
	///��������
	TZeusingFtdcDateType	ActionDate;
	///����ʱ��
	TZeusingFtdcTimeType	ActionTime;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�������ر��
	TZeusingFtdcOrderLocalIDType	ActionLocalID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///��������״̬
	TZeusingFtdcOrderActionStatusType	OrderActionStatus;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
};

///��������������ʧ��
struct CZeusingFtdcExchangeOrderActionErrorField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�������ر��
	TZeusingFtdcOrderLocalIDType	ActionLocalID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///�������ɽ�
struct CZeusingFtdcExchangeTradeField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�ɽ����
	TZeusingFtdcTradeIDType	TradeID;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///���׽�ɫ
	TZeusingFtdcTradingRoleType	TradingRole;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///��ƽ��־
	TZeusingFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///�۸�
	TZeusingFtdcPriceType	Price;
	///����
	TZeusingFtdcVolumeType	Volume;
	///�ɽ�ʱ��
	TZeusingFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TZeusingFtdcTimeType	TradeTime;
	///�ɽ�����
	TZeusingFtdcTradeTypeType	TradeType;
	///�ɽ�����Դ
	TZeusingFtdcPriceSourceType	PriceSource;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�����Ա���
	TZeusingFtdcParticipantIDType	ClearingPartID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///���
	TZeusingFtdcSequenceNoType	SequenceNo;
	///�ɽ���Դ
	TZeusingFtdcTradeSourceType	TradeSource;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///�ɽ�
struct CZeusingFtdcTradeField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�ɽ����
	TZeusingFtdcTradeIDType	TradeID;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///���׽�ɫ
	TZeusingFtdcTradingRoleType	TradingRole;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///��ƽ��־
	TZeusingFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///�۸�
	TZeusingFtdcPriceType	Price;
	///����
	TZeusingFtdcVolumeType	Volume;
	///�ɽ�ʱ��
	TZeusingFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TZeusingFtdcTimeType	TradeTime;
	///�ɽ�����
	TZeusingFtdcTradeTypeType	TradeType;
	///�ɽ�����Դ
	TZeusingFtdcPriceSourceType	PriceSource;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�����Ա���
	TZeusingFtdcParticipantIDType	ClearingPartID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///���
	TZeusingFtdcSequenceNoType	SequenceNo;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///���͹�˾�������
	TZeusingFtdcSequenceNoType	BrokerOrderSeq;
	///�ɽ���Դ
	TZeusingFtdcTradeSourceType	TradeSource;
};

///�û��Ự
struct CZeusingFtdcUserSessionField
{
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��¼����
	TZeusingFtdcDateType	LoginDate;
	///��¼ʱ��
	TZeusingFtdcTimeType	LoginTime;
	///IP��ַ
	TZeusingFtdcIPAddressType	IPAddress;
	///�û��˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	UserProductInfo;
	///�ӿڶ˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	InterfaceProductInfo;
	///Э����Ϣ
	TZeusingFtdcProtocolInfoType	ProtocolInfo;
	///Mac��ַ
	TZeusingFtdcMacAddressType	MacAddress;
};

///��ѯ��󱨵�����
struct CZeusingFtdcQueryMaxOrderVolumeField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///��ƽ��־
	TZeusingFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///�������������
	TZeusingFtdcVolumeType	MaxVolume;
};

///Ͷ���߽�����ȷ����Ϣ
struct CZeusingFtdcSettlementInfoConfirmField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///ȷ������
	TZeusingFtdcDateType	ConfirmDate;
	///ȷ��ʱ��
	TZeusingFtdcTimeType	ConfirmTime;
};

///�����ͬ��
struct CZeusingFtdcSyncDepositField
{
	///�������ˮ��
	TZeusingFtdcDepositSeqNoType	DepositSeqNo;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///�����
	TZeusingFtdcMoneyType	Deposit;
	///�Ƿ�ǿ�ƽ���
	TZeusingFtdcBoolType	IsForce;
};

///���͹�˾ͬ��
struct CZeusingFtdcBrokerSyncField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
};

///����ͬ���е�Ͷ����
struct CZeusingFtdcSyncingInvestorField
{
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���߷������
	TZeusingFtdcInvestorIDType	InvestorGroupID;
	///Ͷ��������
	TZeusingFtdcPartyNameType	InvestorName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdentifiedCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�Ƿ��Ծ
	TZeusingFtdcBoolType	IsActive;
	///��ϵ�绰
	TZeusingFtdcTelephoneType	Telephone;
	///ͨѶ��ַ
	TZeusingFtdcAddressType	Address;
	///��������
	TZeusingFtdcDateType	OpenDate;
	///�ֻ�
	TZeusingFtdcMobileType	Mobile;
	///��������ģ�����
	TZeusingFtdcInvestorIDType	CommModelID;
	///����ֶ�
	TZeusingFtdcPadFieldType	PadField;
	///��֤����ģ�����
	TZeusingFtdcInvestorIDType	MarginModelID;
};

///����ͬ���еĽ��״���
struct CZeusingFtdcSyncingTradingCodeField
{
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///�Ƿ��Ծ
	TZeusingFtdcBoolType	IsActive;
	///���ױ�������
	TZeusingFtdcClientIDTypeType	ClientIDType;
};

///����ͬ���е�Ͷ���߷���
struct CZeusingFtdcSyncingInvestorGroupField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���߷������
	TZeusingFtdcInvestorIDType	InvestorGroupID;
	///Ͷ���߷�������
	TZeusingFtdcInvestorGroupNameType	InvestorGroupName;
};

///����ͬ���еĽ����˺�
struct CZeusingFtdcSyncingTradingAccountField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ϴ���Ѻ���
	TZeusingFtdcMoneyType	PreMortgage;
	///�ϴ����ö��
	TZeusingFtdcMoneyType	PreCredit;
	///�ϴδ���
	TZeusingFtdcMoneyType	PreDeposit;
	///�ϴν���׼����
	TZeusingFtdcMoneyType	PreBalance;
	///�ϴ�ռ�õı�֤��
	TZeusingFtdcMoneyType	PreMargin;
	///��Ϣ����
	TZeusingFtdcMoneyType	InterestBase;
	///��Ϣ����
	TZeusingFtdcMoneyType	Interest;
	///�����
	TZeusingFtdcMoneyType	Deposit;
	///������
	TZeusingFtdcMoneyType	Withdraw;
	///����ı�֤��
	TZeusingFtdcMoneyType	FrozenMargin;
	///������ʽ�
	TZeusingFtdcMoneyType	FrozenCash;
	///�����������
	TZeusingFtdcMoneyType	FrozenCommission;
	///��ǰ��֤���ܶ�
	TZeusingFtdcMoneyType	CurrMargin;
	///�ʽ���
	TZeusingFtdcMoneyType	CashIn;
	///������
	TZeusingFtdcMoneyType	Commission;
	///ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfit;
	///�ֲ�ӯ��
	TZeusingFtdcMoneyType	PositionProfit;
	///�ڻ�����׼����
	TZeusingFtdcMoneyType	Balance;
	///�����ʽ�
	TZeusingFtdcMoneyType	Available;
	///��ȡ�ʽ�
	TZeusingFtdcMoneyType	WithdrawQuota;
	///����׼����
	TZeusingFtdcMoneyType	Reserve;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///���ö��
	TZeusingFtdcMoneyType	Credit;
	///��Ѻ���
	TZeusingFtdcMoneyType	Mortgage;
	///��������֤��
	TZeusingFtdcMoneyType	ExchangeMargin;
	///Ͷ���߽��֤��
	TZeusingFtdcMoneyType	DeliveryMargin;
	///���������֤��
	TZeusingFtdcMoneyType	ExchangeDeliveryMargin;
	///�����ڻ�����׼����
	TZeusingFtdcMoneyType	ReserveBalance;
	///��Ȩƽ��ӯ��
	TZeusingFtdcMoneyType	OptionCloseProfit;
	///�ʻ�����
	TZeusingFtdcAccountLevelType	AccountLevel;
};

///����ͬ���е�Ͷ���ֲ߳�
struct CZeusingFtdcSyncingInvestorPositionField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///�ֲֶ�շ���
	TZeusingFtdcPosiDirectionType	PosiDirection;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///�ֲ�����
	TZeusingFtdcPositionDateType	PositionDate;
	///���ճֲ�
	TZeusingFtdcVolumeType	YdPosition;
	///���ճֲ�
	TZeusingFtdcVolumeType	Position;
	///��ͷ����
	TZeusingFtdcVolumeType	LongFrozen;
	///��ͷ����
	TZeusingFtdcVolumeType	ShortFrozen;
	///���ֶ�����
	TZeusingFtdcMoneyType	LongFrozenAmount;
	///���ֶ�����
	TZeusingFtdcMoneyType	ShortFrozenAmount;
	///������
	TZeusingFtdcVolumeType	OpenVolume;
	///ƽ����
	TZeusingFtdcVolumeType	CloseVolume;
	///���ֽ��
	TZeusingFtdcMoneyType	OpenAmount;
	///ƽ�ֽ��
	TZeusingFtdcMoneyType	CloseAmount;
	///ƽ��ֽ��
	TZeusingFtdcMoneyType	YdCloseAmount;
	///ƽ�������
	TZeusingFtdcVolumeType	YdCloseVolume;
	///�ֲֳɱ�
	TZeusingFtdcMoneyType	PositionCost;
	///�ϴ�ռ�õı�֤��
	TZeusingFtdcMoneyType	PreMargin;
	///ռ�õı�֤��
	TZeusingFtdcMoneyType	UseMargin;
	///����ı�֤��
	TZeusingFtdcMoneyType	FrozenMargin;
	///������ʽ�
	TZeusingFtdcMoneyType	FrozenCash;
	///�����������
	TZeusingFtdcMoneyType	FrozenCommission;
	///�ʽ���
	TZeusingFtdcMoneyType	CashIn;
	///������
	TZeusingFtdcMoneyType	Commission;
	///ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfit;
	///�ֲ�ӯ��
	TZeusingFtdcMoneyType	PositionProfit;
	///�ϴν����
	TZeusingFtdcPriceType	PreSettlementPrice;
	///���ν����
	TZeusingFtdcPriceType	SettlementPrice;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///���ֳɱ�
	TZeusingFtdcMoneyType	OpenCost;
	///��������֤��
	TZeusingFtdcMoneyType	ExchangeMargin;
	///��ϳɽ��γɵĳֲ�
	TZeusingFtdcVolumeType	CombPosition;
	///��϶�ͷ����
	TZeusingFtdcVolumeType	CombLongFrozen;
	///��Ͽ�ͷ����
	TZeusingFtdcVolumeType	CombShortFrozen;
	///���ն���ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfitByDate;
	///��ʶԳ�ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfitByTrade;
	///���ճֲ�
	TZeusingFtdcVolumeType	TodayPosition;
	///��֤����
	TZeusingFtdcRatioType	MarginRateByMoney;
	///��֤����(������)
	TZeusingFtdcRatioType	MarginRateByVolume;
	///ִ�ж���
	TZeusingFtdcVolumeType	StrikeFrozen;
	///ִ�ж�����
	TZeusingFtdcMoneyType	StrikeFrozenAmount;
	///����ִ�ж���
	TZeusingFtdcVolumeType	AbandonFrozen;
};

///����ͬ���еĺ�Լ��֤����
struct CZeusingFtdcSyncingInstrumentMarginRateField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///��ͷ��֤����
	TZeusingFtdcRatioType	LongMarginRatioByMoney;
	///��ͷ��֤���
	TZeusingFtdcMoneyType	LongMarginRatioByVolume;
	///��ͷ��֤����
	TZeusingFtdcRatioType	ShortMarginRatioByMoney;
	///��ͷ��֤���
	TZeusingFtdcMoneyType	ShortMarginRatioByVolume;
	///�Ƿ���Խ�������ȡ
	TZeusingFtdcBoolType	IsRelative;
};

///����ͬ���еĺ�Լ��������
struct CZeusingFtdcSyncingInstrumentCommissionRateField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///������������
	TZeusingFtdcRatioType	OpenRatioByMoney;
	///����������
	TZeusingFtdcRatioType	OpenRatioByVolume;
	///ƽ����������
	TZeusingFtdcRatioType	CloseRatioByMoney;
	///ƽ��������
	TZeusingFtdcRatioType	CloseRatioByVolume;
	///ƽ����������
	TZeusingFtdcRatioType	CloseTodayRatioByMoney;
	///ƽ��������
	TZeusingFtdcRatioType	CloseTodayRatioByVolume;
};

///����ͬ���еĺ�Լ����Ȩ��
struct CZeusingFtdcSyncingInstrumentTradingRightField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����Ȩ��
	TZeusingFtdcTradingRightType	TradingRight;
	///���۽���Ȩ��
	TZeusingFtdcTradingRightType	QuoteTradingRight;
};

///��ѯ����
struct CZeusingFtdcQryOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///��ʼʱ��
	TZeusingFtdcTimeType	InsertTimeStart;
	///����ʱ��
	TZeusingFtdcTimeType	InsertTimeEnd;
};

///��ѯ�ɽ�
struct CZeusingFtdcQryTradeField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�ɽ����
	TZeusingFtdcTradeIDType	TradeID;
	///��ʼʱ��
	TZeusingFtdcTimeType	TradeTimeStart;
	///����ʱ��
	TZeusingFtdcTimeType	TradeTimeEnd;
};

///��ѯͶ���ֲ߳�
struct CZeusingFtdcQryInvestorPositionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///��ѯ�ʽ��˻�
struct CZeusingFtdcQryTradingAccountField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///��ѯͶ����
struct CZeusingFtdcQryInvestorField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///��ѯ���ױ���
struct CZeusingFtdcQryTradingCodeField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///���ױ�������
	TZeusingFtdcClientIDTypeType	ClientIDType;
};

///��ѯ���ױ���
struct CZeusingFtdcQryInvestorGroupField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
};

///��ѯ���ױ���
struct CZeusingFtdcQryInstrumentMarginRateField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
};

///��ѯ���ױ���
struct CZeusingFtdcQryInstrumentCommissionRateField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
};

///��ѯ���ױ���
struct CZeusingFtdcQryInstrumentTradingRightField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///��ѯ���͹�˾
struct CZeusingFtdcQryBrokerField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
};

///��ѯ����Ա
struct CZeusingFtdcQryTraderField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
};

///��ѯ���͹�˾��Ա����
struct CZeusingFtdcQryPartBrokerField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
};

///��ѯ�����û�����Ȩ��
struct CZeusingFtdcQrySuperUserFunctionField
{
	///�û�����
	TZeusingFtdcUserIDType	UserID;
};

///��ѯ�û��Ự
struct CZeusingFtdcQryUserSessionField
{
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
};

///��ѯǰ��״̬
struct CZeusingFtdcQryFrontStatusField
{
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
};

///��ѯ����������
struct CZeusingFtdcQryExchangeOrderField
{
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
};

///��ѯ��������
struct CZeusingFtdcQryOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///��ѯ��������������
struct CZeusingFtdcQryExchangeOrderActionField
{
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
};

///��ѯ�����û�
struct CZeusingFtdcQrySuperUserField
{
	///�û�����
	TZeusingFtdcUserIDType	UserID;
};

///��ѯ������
struct CZeusingFtdcQryExchangeField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///��ѯ��Ʒ
struct CZeusingFtdcQryProductField
{
	///��Ʒ����
	TZeusingFtdcInstrumentIDType	ProductID;
	///��Ʒ����
	TZeusingFtdcProductClassType	ProductClass;
};

///��ѯ��Լ
struct CZeusingFtdcQryInstrumentField
{
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///��Ʒ����
	TZeusingFtdcInstrumentIDType	ProductID;
};

///��ѯ����
struct CZeusingFtdcQryDepthMarketDataField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///��ѯ���͹�˾�û�
struct CZeusingFtdcQryBrokerUserField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
};

///��ѯ���͹�˾�û�Ȩ��
struct CZeusingFtdcQryBrokerUserFunctionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
};

///��ѯ����Ա���̻�
struct CZeusingFtdcQryTraderOfferField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
};

///��ѯ�������ˮ
struct CZeusingFtdcQrySyncDepositField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�������ˮ��
	TZeusingFtdcDepositSeqNoType	DepositSeqNo;
};

///��ѯͶ���߽�����
struct CZeusingFtdcQrySettlementInfoField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///������
	TZeusingFtdcDateType	TradingDay;
};

///��ѯ����
struct CZeusingFtdcQryHisOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///��ʼʱ��
	TZeusingFtdcTimeType	InsertTimeStart;
	///����ʱ��
	TZeusingFtdcTimeType	InsertTimeEnd;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
};

///��ǰ��Ȩ��Լ��С��֤��
struct CZeusingFtdcOptionInstrMiniMarginField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��λ���֣���Ȩ��Լ��С��֤��
	TZeusingFtdcMoneyType	MinMargin;
	///ȡֵ��ʽ
	TZeusingFtdcValueMethodType	ValueMethod;
	///�Ƿ���潻������ȡ
	TZeusingFtdcBoolType	IsRelative;
};

///���͹�˾��������Ȩ��֤���㷨
struct CZeusingFtdcBrokerExchOptMarginAlgoField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��ͱ���ϵ��
	TZeusingFtdcRatioType	GuaranteeRatio;
	///��ֵ���Żݱ���
	TZeusingFtdcRatioType	OutRatio;
};

///��ǰ��Ȩ��Լ��֤�����ϵ��
struct CZeusingFtdcOptionInstrMarginAdjustField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///��ͷ��֤�����ϵ��
	TZeusingFtdcRatioType	ShortMarginRatioByMoney;
	///��ͷ��֤�����ϵ��
	TZeusingFtdcMoneyType	ShortMarginRatioByVolume;
	///�Ƿ���潻������ȡ
	TZeusingFtdcBoolType	IsRelative;
};

///��ǰ��Ȩ��Լ�����ѵ���ϸ����
struct CZeusingFtdcOptionInstrCommRateField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///������������
	TZeusingFtdcRatioType	OpenRatioByMoney;
	///����������
	TZeusingFtdcRatioType	OpenRatioByVolume;
	///ƽ����������
	TZeusingFtdcRatioType	CloseRatioByMoney;
	///ƽ��������
	TZeusingFtdcRatioType	CloseRatioByVolume;
	///ƽ����������
	TZeusingFtdcRatioType	CloseTodayRatioByMoney;
	///ƽ��������
	TZeusingFtdcRatioType	CloseTodayRatioByVolume;
	///ִ����������
	TZeusingFtdcRatioType	StrikeRatioByMoney;
	///ִ��������
	TZeusingFtdcMoneyType	StrikeRatioByVolume;
};

///��Ȩ���׳ɱ�
struct CZeusingFtdcOptionInstrTradeCostField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///��Ȩ��Լ��֤��
	TZeusingFtdcMoneyType	Margin;
	///��Ȩ��ԼȨ����
	TZeusingFtdcMoneyType	Royalty;
};

///��Ȩ���׳ɱ���ѯ
struct CZeusingFtdcQryOptionInstrTradeCostField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///��Ȩ��Լ����
	TZeusingFtdcPriceType	InputPrice;
};

///��Ȩ�������ʲ�ѯ
struct CZeusingFtdcQryOptionInstrCommRateField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
};

///��ָ�ֻ�ָ��
struct CZeusingFtdcIndexPriceField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///�ֻ����̼�
	TZeusingFtdcPriceType	ClosePrice;
};

///�����ִ������
struct CZeusingFtdcInputExecOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///ִ����������
	TZeusingFtdcOrderRefType	ExecOrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///����
	TZeusingFtdcVolumeType	Volume;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///��ƽ��־
	TZeusingFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///ִ������
	TZeusingFtdcActionTypeType	ActionType;
	///����ͷ������ĳֲַ���
	TZeusingFtdcPosiDirectionType	PosiDirection;
	///��Ȩ��Ȩ���Ƿ����ڻ�ͷ��ı��
	TZeusingFtdcExecOrderPositionFlagType	ReservePositionFlag;
	///��Ȩ��Ȩ�����ɵ�ͷ���Ƿ��Զ�ƽ��
	TZeusingFtdcExecOrderCloseFlagType	CloseFlag;
};

///����ִ���������
struct CZeusingFtdcInputExecOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///ִ�������������
	TZeusingFtdcOrderActionRefType	ExecOrderActionRef;
	///ִ����������
	TZeusingFtdcOrderRefType	ExecOrderRef;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///ִ������������
	TZeusingFtdcExecOrderSysIDType	ExecOrderSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///ִ������
struct CZeusingFtdcExecOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///ִ����������
	TZeusingFtdcOrderRefType	ExecOrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///����
	TZeusingFtdcVolumeType	Volume;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///��ƽ��־
	TZeusingFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///ִ������
	TZeusingFtdcActionTypeType	ActionType;
	///����ͷ������ĳֲַ���
	TZeusingFtdcPosiDirectionType	PosiDirection;
	///��Ȩ��Ȩ���Ƿ����ڻ�ͷ��ı��
	TZeusingFtdcExecOrderPositionFlagType	ReservePositionFlag;
	///��Ȩ��Ȩ�����ɵ�ͷ���Ƿ��Զ�ƽ��
	TZeusingFtdcExecOrderCloseFlagType	CloseFlag;
	///����ִ��������
	TZeusingFtdcOrderLocalIDType	ExecOrderLocalID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///ִ�������ύ״̬
	TZeusingFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///������ʾ���
	TZeusingFtdcSequenceNoType	NotifySequence;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///ִ��������
	TZeusingFtdcExecOrderSysIDType	ExecOrderSysID;
	///��������
	TZeusingFtdcDateType	InsertDate;
	///����ʱ��
	TZeusingFtdcTimeType	InsertTime;
	///����ʱ��
	TZeusingFtdcTimeType	CancelTime;
	///ִ�н��
	TZeusingFtdcExecResultType	ExecResult;
	///�����Ա���
	TZeusingFtdcParticipantIDType	ClearingPartID;
	///���
	TZeusingFtdcSequenceNoType	SequenceNo;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///�û��˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	UserProductInfo;
	///״̬��Ϣ
	TZeusingFtdcErrorMsgType	StatusMsg;
	///�����û�����
	TZeusingFtdcUserIDType	ActiveUserID;
	///���͹�˾�������
	TZeusingFtdcSequenceNoType	BrokerExecOrderSeq;
	///���ױ�������
	TZeusingFtdcClientIDTypeType	ClientIDType;
};

///ִ���������
struct CZeusingFtdcExecOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///ִ�������������
	TZeusingFtdcOrderActionRefType	ExecOrderActionRef;
	///ִ����������
	TZeusingFtdcOrderRefType	ExecOrderRef;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///ִ������������
	TZeusingFtdcExecOrderSysIDType	ExecOrderSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///��������
	TZeusingFtdcDateType	ActionDate;
	///����ʱ��
	TZeusingFtdcTimeType	ActionTime;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///����ִ��������
	TZeusingFtdcOrderLocalIDType	ExecOrderLocalID;
	///�������ر��
	TZeusingFtdcOrderLocalIDType	ActionLocalID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///��������״̬
	TZeusingFtdcOrderActionStatusType	OrderActionStatus;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///״̬��Ϣ
	TZeusingFtdcErrorMsgType	StatusMsg;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///ִ�������ѯ
struct CZeusingFtdcQryExecOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///ִ��������
	TZeusingFtdcExecOrderSysIDType	ExecOrderSysID;
	///��ʼʱ��
	TZeusingFtdcTimeType	InsertTimeStart;
	///����ʱ��
	TZeusingFtdcTimeType	InsertTimeEnd;
};

///������ִ��������Ϣ
struct CZeusingFtdcExchangeExecOrderField
{
	///����
	TZeusingFtdcVolumeType	Volume;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///��ƽ��־
	TZeusingFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///ִ������
	TZeusingFtdcActionTypeType	ActionType;
	///����ͷ������ĳֲַ���
	TZeusingFtdcPosiDirectionType	PosiDirection;
	///��Ȩ��Ȩ���Ƿ����ڻ�ͷ��ı��
	TZeusingFtdcExecOrderPositionFlagType	ReservePositionFlag;
	///��Ȩ��Ȩ�����ɵ�ͷ���Ƿ��Զ�ƽ��
	TZeusingFtdcExecOrderCloseFlagType	CloseFlag;
	///����ִ��������
	TZeusingFtdcOrderLocalIDType	ExecOrderLocalID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///ִ�������ύ״̬
	TZeusingFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///������ʾ���
	TZeusingFtdcSequenceNoType	NotifySequence;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///ִ��������
	TZeusingFtdcExecOrderSysIDType	ExecOrderSysID;
	///��������
	TZeusingFtdcDateType	InsertDate;
	///����ʱ��
	TZeusingFtdcTimeType	InsertTime;
	///����ʱ��
	TZeusingFtdcTimeType	CancelTime;
	///ִ�н��
	TZeusingFtdcExecResultType	ExecResult;
	///�����Ա���
	TZeusingFtdcParticipantIDType	ClearingPartID;
	///���
	TZeusingFtdcSequenceNoType	SequenceNo;
};

///������ִ�������ѯ
struct CZeusingFtdcQryExchangeExecOrderField
{
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
};

///ִ�����������ѯ
struct CZeusingFtdcQryExecOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///������ִ���������
struct CZeusingFtdcExchangeExecOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///ִ������������
	TZeusingFtdcExecOrderSysIDType	ExecOrderSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///��������
	TZeusingFtdcDateType	ActionDate;
	///����ʱ��
	TZeusingFtdcTimeType	ActionTime;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///����ִ��������
	TZeusingFtdcOrderLocalIDType	ExecOrderLocalID;
	///�������ر��
	TZeusingFtdcOrderLocalIDType	ActionLocalID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///��������״̬
	TZeusingFtdcOrderActionStatusType	OrderActionStatus;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///������ִ�����������ѯ
struct CZeusingFtdcQryExchangeExecOrderActionField
{
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
};

///����ִ������
struct CZeusingFtdcErrExecOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///ִ����������
	TZeusingFtdcOrderRefType	ExecOrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///����
	TZeusingFtdcVolumeType	Volume;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///��ƽ��־
	TZeusingFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///ִ������
	TZeusingFtdcActionTypeType	ActionType;
	///����ͷ������ĳֲַ���
	TZeusingFtdcPosiDirectionType	PosiDirection;
	///��Ȩ��Ȩ���Ƿ����ڻ�ͷ��ı��
	TZeusingFtdcExecOrderPositionFlagType	ReservePositionFlag;
	///��Ȩ��Ȩ�����ɵ�ͷ���Ƿ��Զ�ƽ��
	TZeusingFtdcExecOrderCloseFlagType	CloseFlag;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///��ѯ����ִ������
struct CZeusingFtdcQryErrExecOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///����ִ���������
struct CZeusingFtdcErrExecOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///ִ�������������
	TZeusingFtdcOrderActionRefType	ExecOrderActionRef;
	///ִ����������
	TZeusingFtdcOrderRefType	ExecOrderRef;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///ִ������������
	TZeusingFtdcExecOrderSysIDType	ExecOrderSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///��ѯ����ִ���������
struct CZeusingFtdcQryErrExecOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///��������
struct CZeusingFtdcInputQuoteField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///������
	TZeusingFtdcVolumeType	BidVolumeTotalOriginal;
	///�۸�
	TZeusingFtdcPriceType	BidPrice;
	///����Ͽ�ƽ��־
	TZeusingFtdcCombOffsetFlagType	BidCombOffsetFlag;
	///�����Ͷ���ױ���־
	TZeusingFtdcCombHedgeFlagType	BidCombHedgeFlag;
	///��������
	TZeusingFtdcVolumeType	AskVolumeTotalOriginal;
	///�۸�
	TZeusingFtdcPriceType	AskPrice;
	///������Ͽ�ƽ��־
	TZeusingFtdcCombOffsetFlagType	AskCombOffsetFlag;
	///�������Ͷ���ױ���־
	TZeusingFtdcCombHedgeFlagType	AskCombHedgeFlag;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///���۲�������
struct CZeusingFtdcInputQuoteActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///������������
	TZeusingFtdcOrderActionRefType	QuoteActionRef;
	///��������
	TZeusingFtdcOrderRefType	QuoteRef;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�������
	TZeusingFtdcOrderSysIDType	QuoteSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///��ѯ����
struct CZeusingFtdcQryQuoteField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///���۱��
	TZeusingFtdcOrderSysIDType	QuoteSysID;
	///��ʼʱ��
	TZeusingFtdcTimeType	InsertTimeStart;
	///����ʱ��
	TZeusingFtdcTimeType	InsertTimeEnd;
};

///����
struct CZeusingFtdcQuoteField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///������
	TZeusingFtdcVolumeType	BidVolumeTotalOriginal;
	///�۸�
	TZeusingFtdcPriceType	BidPrice;
	///����Ͽ�ƽ��־
	TZeusingFtdcCombOffsetFlagType	BidCombOffsetFlag;
	///�����Ͷ���ױ���־
	TZeusingFtdcCombHedgeFlagType	BidCombHedgeFlag;
	///��������
	TZeusingFtdcVolumeType	AskVolumeTotalOriginal;
	///�۸�
	TZeusingFtdcPriceType	AskPrice;
	///������Ͽ�ƽ��־
	TZeusingFtdcCombOffsetFlagType	AskCombOffsetFlag;
	///�������Ͷ���ױ���־
	TZeusingFtdcCombHedgeFlagType	AskCombHedgeFlag;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�����ύ״̬
	TZeusingFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///������ʾ���
	TZeusingFtdcSequenceNoType	NotifySequence;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///�������
	TZeusingFtdcOrderSysIDType	QuoteSysID;
	///�򷽱������
	TZeusingFtdcOrderSysIDType	BidOrderSysID;
	///�����������
	TZeusingFtdcOrderSysIDType	AskOrderSysID;
	///����״̬
	TZeusingFtdcOrderStatusType	QuoteStatus;
	///��ʣ������
	TZeusingFtdcVolumeType	BidVolumeTotal;
	///����ʣ������
	TZeusingFtdcVolumeType	AskVolumeTotal;
	///�򷽳ɽ�����
	TZeusingFtdcVolumeType	BidVolumeTraded;
	///�����ɽ�����
	TZeusingFtdcVolumeType	AskVolumeTraded;
	///��������
	TZeusingFtdcDateType	InsertDate;
	///ί��ʱ��
	TZeusingFtdcTimeType	InsertTime;
	///����ʱ��
	TZeusingFtdcTimeType	ActiveTime;
	///����ʱ��
	TZeusingFtdcTimeType	SuspendTime;
	///����޸�ʱ��
	TZeusingFtdcTimeType	UpdateTime;
	///����ʱ��
	TZeusingFtdcTimeType	CancelTime;
	///����޸Ľ���������Ա����
	TZeusingFtdcTraderIDType	ActiveTraderID;
	///�����Ա���
	TZeusingFtdcParticipantIDType	ClearingPartID;
	///���
	TZeusingFtdcSequenceNoType	SequenceNo;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///�û��˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	UserProductInfo;
	///״̬��Ϣ
	TZeusingFtdcErrorMsgType	StatusMsg;
	///�����û�����
	TZeusingFtdcUserIDType	ActiveUserID;
	///���͹�˾�������
	TZeusingFtdcSequenceNoType	BrokerOrderSeq;
	///���ױ�������
	TZeusingFtdcClientIDTypeType	ClientIDType;
};

///�ʽ���ת
struct CZeusingFtdcInternalTransferField
{
	///�������ˮ��
	TZeusingFtdcDepositSeqNoType	DepositSeqNo;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///ת��Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///ת��Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InInvestorID;
	///��ת���
	TZeusingFtdcMoneyType	Deposit;
	///�Ƿ�ǿ�ƽ���
	TZeusingFtdcBoolType	IsForce;
};

///����
struct CZeusingFtdcQuoteActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///������������
	TZeusingFtdcOrderActionRefType	QuoteActionRef;
	///��������
	TZeusingFtdcOrderRefType	QuoteRef;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�������
	TZeusingFtdcOrderSysIDType	QuoteSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///��������
	TZeusingFtdcDateType	ActionDate;
	///����ʱ��
	TZeusingFtdcTimeType	ActionTime;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///���ر��۱��
	TZeusingFtdcOrderLocalIDType	QuoteLocalID;
	///�������ر��
	TZeusingFtdcOrderLocalIDType	ActionLocalID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///��������״̬
	TZeusingFtdcOrderActionStatusType	OrderActionStatus;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///״̬��Ϣ
	TZeusingFtdcErrorMsgType	StatusMsg;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///����
struct CZeusingFtdcQryQuoteActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///Ͷ���߲�Ʒ�鱣֤��
struct CZeusingFtdcInvestorProductGroupMarginField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///Ʒ��/��Ʒ�ֱ�ʾ
	TZeusingFtdcInstrumentIDType	ProductGroupID;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///����ı�֤��
	TZeusingFtdcMoneyType	FrozenMargin;
	///��ͷ����ı�֤��
	TZeusingFtdcMoneyType	LongFrozenMargin;
	///��ͷ����ı�֤��
	TZeusingFtdcMoneyType	ShortFrozenMargin;
	///ռ�õı�֤��
	TZeusingFtdcMoneyType	UseMargin;
	///��ͷ��֤��
	TZeusingFtdcMoneyType	LongUseMargin;
	///��ͷ��֤��
	TZeusingFtdcMoneyType	ShortUseMargin;
	///��������֤��
	TZeusingFtdcMoneyType	ExchMargin;
	///��������ͷ��֤��
	TZeusingFtdcMoneyType	LongExchMargin;
	///��������ͷ��֤��
	TZeusingFtdcMoneyType	ShortExchMargin;
	///ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfit;
	///�����������
	TZeusingFtdcMoneyType	FrozenCommission;
	///������
	TZeusingFtdcMoneyType	Commission;
	///������ʽ�
	TZeusingFtdcMoneyType	FrozenCash;
	///�ʽ���
	TZeusingFtdcMoneyType	CashIn;
	///�ֲ�ӯ��
	TZeusingFtdcMoneyType	PositionProfit;
	///�۵��ܽ��
	TZeusingFtdcMoneyType	OffsetAmount;
	///��ͷ�۵��ܽ��
	TZeusingFtdcMoneyType	LongOffsetAmount;
	///��ͷ�۵��ܽ��
	TZeusingFtdcMoneyType	ShortOffsetAmount;
	///�������۵��ܽ��
	TZeusingFtdcMoneyType	ExchOffsetAmount;
	///��������ͷ�۵��ܽ��
	TZeusingFtdcMoneyType	LongExchOffsetAmount;
	///��������ͷ�۵��ܽ��
	TZeusingFtdcMoneyType	ShortExchOffsetAmount;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
};

///Ͷ���߲�Ʒ�鱣֤��
struct CZeusingFtdcQryInvestorProductGroupMarginField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ʒ��/��Ʒ�ֱ�ʾ
	TZeusingFtdcInstrumentIDType	ProductGroupID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
};

///�г�����
struct CZeusingFtdcMarketDataField
{
	///������
	TZeusingFtdcDateType	TradingDay;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///���¼�
	TZeusingFtdcPriceType	LastPrice;
	///�ϴν����
	TZeusingFtdcPriceType	PreSettlementPrice;
	///������
	TZeusingFtdcPriceType	PreClosePrice;
	///��ֲ���
	TZeusingFtdcLargeVolumeType	PreOpenInterest;
	///����
	TZeusingFtdcPriceType	OpenPrice;
	///��߼�
	TZeusingFtdcPriceType	HighestPrice;
	///��ͼ�
	TZeusingFtdcPriceType	LowestPrice;
	///����
	TZeusingFtdcVolumeType	Volume;
	///�ɽ����
	TZeusingFtdcMoneyType	Turnover;
	///�ֲ���
	TZeusingFtdcLargeVolumeType	OpenInterest;
	///������
	TZeusingFtdcPriceType	ClosePrice;
	///���ν����
	TZeusingFtdcPriceType	SettlementPrice;
	///��ͣ���
	TZeusingFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TZeusingFtdcPriceType	LowerLimitPrice;
	///����ʵ��
	TZeusingFtdcRatioType	PreDelta;
	///����ʵ��
	TZeusingFtdcRatioType	CurrDelta;
	///����޸�ʱ��
	TZeusingFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TZeusingFtdcMillisecType	UpdateMillisec;
	///ҵ������
	TZeusingFtdcDateType	ActionDay;
};

///�����������
struct CZeusingFtdcMarketDataBaseField
{
	///������
	TZeusingFtdcDateType	TradingDay;
	///�ϴν����
	TZeusingFtdcPriceType	PreSettlementPrice;
	///������
	TZeusingFtdcPriceType	PreClosePrice;
	///��ֲ���
	TZeusingFtdcLargeVolumeType	PreOpenInterest;
	///����ʵ��
	TZeusingFtdcRatioType	PreDelta;
};

///���龲̬����
struct CZeusingFtdcMarketDataStaticField
{
	///����
	TZeusingFtdcPriceType	OpenPrice;
	///��߼�
	TZeusingFtdcPriceType	HighestPrice;
	///��ͼ�
	TZeusingFtdcPriceType	LowestPrice;
	///������
	TZeusingFtdcPriceType	ClosePrice;
	///��ͣ���
	TZeusingFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TZeusingFtdcPriceType	LowerLimitPrice;
	///���ν����
	TZeusingFtdcPriceType	SettlementPrice;
	///����ʵ��
	TZeusingFtdcRatioType	CurrDelta;
};

///�������³ɽ�����
struct CZeusingFtdcMarketDataLastMatchField
{
	///���¼�
	TZeusingFtdcPriceType	LastPrice;
	///����
	TZeusingFtdcVolumeType	Volume;
	///�ɽ����
	TZeusingFtdcMoneyType	Turnover;
	///�ֲ���
	TZeusingFtdcLargeVolumeType	OpenInterest;
};

///�������ż�����
struct CZeusingFtdcMarketDataBestPriceField
{
	///�����һ
	TZeusingFtdcPriceType	BidPrice1;
	///������һ
	TZeusingFtdcVolumeType	BidVolume1;
	///������һ
	TZeusingFtdcPriceType	AskPrice1;
	///������һ
	TZeusingFtdcVolumeType	AskVolume1;
};

///�����������������
struct CZeusingFtdcMarketDataBid23Field
{
	///����۶�
	TZeusingFtdcPriceType	BidPrice2;
	///��������
	TZeusingFtdcVolumeType	BidVolume2;
	///�������
	TZeusingFtdcPriceType	BidPrice3;
	///��������
	TZeusingFtdcVolumeType	BidVolume3;
};

///������������������
struct CZeusingFtdcMarketDataAsk23Field
{
	///�����۶�
	TZeusingFtdcPriceType	AskPrice2;
	///��������
	TZeusingFtdcVolumeType	AskVolume2;
	///��������
	TZeusingFtdcPriceType	AskPrice3;
	///��������
	TZeusingFtdcVolumeType	AskVolume3;
};

///���������ġ�������
struct CZeusingFtdcMarketDataBid45Field
{
	///�������
	TZeusingFtdcPriceType	BidPrice4;
	///��������
	TZeusingFtdcVolumeType	BidVolume4;
	///�������
	TZeusingFtdcPriceType	BidPrice5;
	///��������
	TZeusingFtdcVolumeType	BidVolume5;
};

///���������ġ�������
struct CZeusingFtdcMarketDataAsk45Field
{
	///��������
	TZeusingFtdcPriceType	AskPrice4;
	///��������
	TZeusingFtdcVolumeType	AskVolume4;
	///��������
	TZeusingFtdcPriceType	AskPrice5;
	///��������
	TZeusingFtdcVolumeType	AskVolume5;
};

///�������ʱ������
struct CZeusingFtdcMarketDataUpdateTimeField
{
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����޸�ʱ��
	TZeusingFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TZeusingFtdcMillisecType	UpdateMillisec;
};

///���齻������������
struct CZeusingFtdcMarketDataExchangeField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///ָ���ĺ�Լ
struct CZeusingFtdcSpecificInstrumentField
{
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///��Լ״̬
struct CZeusingFtdcInstrumentStatusField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///���������
	TZeusingFtdcSettlementGroupIDType	SettlementGroupID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��Լ����״̬
	TZeusingFtdcInstrumentStatusType	InstrumentStatus;
	///���׽׶α��
	TZeusingFtdcTradingSegmentSNType	TradingSegmentSN;
	///���뱾״̬ʱ��
	TZeusingFtdcTimeType	EnterTime;
	///���뱾״̬ԭ��
	TZeusingFtdcInstStatusEnterReasonType	EnterReason;
};

///��ѯ��Լ״̬
struct CZeusingFtdcQryInstrumentStatusField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
};

///Ͷ�����˻�
struct CZeusingFtdcInvestorAccountField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
};

///����ӯ���㷨
struct CZeusingFtdcPositionProfitAlgorithmField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///ӯ���㷨
	TZeusingFtdcAlgorithmType	Algorithm;
	///��ע
	TZeusingFtdcMemoType	Memo;
};

///��Ա�ʽ��ۿ�
struct CZeusingFtdcDiscountField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///�ʽ��ۿ۱���
	TZeusingFtdcRatioType	Discount;
};

///��ѯת������
struct CZeusingFtdcQryTransferBankField
{
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з����Ĵ���
	TZeusingFtdcBankBrchIDType	BankBrchID;
};

///ת������
struct CZeusingFtdcTransferBankField
{
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з����Ĵ���
	TZeusingFtdcBankBrchIDType	BankBrchID;
	///��������
	TZeusingFtdcBankNameType	BankName;
	///�Ƿ��Ծ
	TZeusingFtdcBoolType	IsActive;
};

///��ѯͶ���ֲ߳���ϸ
struct CZeusingFtdcQryInvestorPositionDetailField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///Ͷ���ֲ߳���ϸ
struct CZeusingFtdcInvestorPositionDetailField
{
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///����
	TZeusingFtdcDirectionType	Direction;
	///��������
	TZeusingFtdcDateType	OpenDate;
	///�ɽ����
	TZeusingFtdcTradeIDType	TradeID;
	///����
	TZeusingFtdcVolumeType	Volume;
	///���ּ�
	TZeusingFtdcPriceType	OpenPrice;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///�ɽ�����
	TZeusingFtdcTradeTypeType	TradeType;
	///��Ϻ�Լ����
	TZeusingFtdcInstrumentIDType	CombInstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///���ն���ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfitByDate;
	///��ʶԳ�ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfitByTrade;
	///���ն��гֲ�ӯ��
	TZeusingFtdcMoneyType	PositionProfitByDate;
	///��ʶԳ�ֲ�ӯ��
	TZeusingFtdcMoneyType	PositionProfitByTrade;
	///Ͷ���߱�֤��
	TZeusingFtdcMoneyType	Margin;
	///��������֤��
	TZeusingFtdcMoneyType	ExchMargin;
	///��֤����
	TZeusingFtdcRatioType	MarginRateByMoney;
	///��֤����(������)
	TZeusingFtdcRatioType	MarginRateByVolume;
	///������
	TZeusingFtdcPriceType	LastSettlementPrice;
	///�����
	TZeusingFtdcPriceType	SettlementPrice;
	///ƽ����
	TZeusingFtdcVolumeType	CloseVolume;
	///ƽ�ֽ��
	TZeusingFtdcMoneyType	CloseAmount;
};

///�ʽ��˻�������
struct CZeusingFtdcTradingAccountPasswordField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///����
	TZeusingFtdcPasswordType	Password;
};

///���������鱨�̻�
struct CZeusingFtdcMDTraderOfferField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///����
	TZeusingFtdcPasswordType	Password;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///����������Ա����״̬
	TZeusingFtdcTraderConnectStatusType	TraderConnectStatus;
	///�����������������
	TZeusingFtdcDateType	ConnectRequestDate;
	///�������������ʱ��
	TZeusingFtdcTimeType	ConnectRequestTime;
	///�ϴα�������
	TZeusingFtdcDateType	LastReportDate;
	///�ϴα���ʱ��
	TZeusingFtdcTimeType	LastReportTime;
	///�����������
	TZeusingFtdcDateType	ConnectDate;
	///�������ʱ��
	TZeusingFtdcTimeType	ConnectTime;
	///��������
	TZeusingFtdcDateType	StartDate;
	///����ʱ��
	TZeusingFtdcTimeType	StartTime;
	///������
	TZeusingFtdcDateType	TradingDay;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///��ϯλ���ɽ����
	TZeusingFtdcTradeIDType	MaxTradeID;
	///��ϯλ��󱨵�����
	TZeusingFtdcReturnCodeType	MaxOrderMessageReference;
};

///��ѯ���鱨�̻�
struct CZeusingFtdcQryMDTraderOfferField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
};

///��ѯ�ͻ�֪ͨ
struct CZeusingFtdcQryNoticeField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
};

///�ͻ�֪ͨ
struct CZeusingFtdcNoticeField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///��Ϣ����
	TZeusingFtdcContentType	Content;
	///���͹�˾֪ͨ�������к�
	TZeusingFtdcSequenceLabelType	SequenceLabel;
};

///�û�Ȩ��
struct CZeusingFtdcUserRightField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�ͻ�Ȩ������
	TZeusingFtdcUserRightTypeType	UserRightType;
	///�Ƿ��ֹ
	TZeusingFtdcBoolType	IsForbidden;
};

///��ѯ������Ϣȷ����
struct CZeusingFtdcQrySettlementInfoConfirmField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///װ�ؽ�����Ϣ
struct CZeusingFtdcLoadSettlementInfoField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
};

///���͹�˾�����ʽ��㷨��
struct CZeusingFtdcBrokerWithdrawAlgorithmField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�����ʽ��㷨
	TZeusingFtdcAlgorithmType	WithdrawAlgorithm;
	///�ʽ�ʹ����
	TZeusingFtdcRatioType	UsingRatio;
	///�����Ƿ����ƽ��ӯ��
	TZeusingFtdcIncludeCloseProfitType	IncludeCloseProfit;
	///�����޲����޳ɽ��ͻ��Ƿ��ܿ����������
	TZeusingFtdcAllWithoutTradeType	AllWithoutTrade;
	///�����Ƿ����ƽ��ӯ��
	TZeusingFtdcIncludeCloseProfitType	AvailIncludeCloseProfit;
	///�Ƿ������û��¼�
	TZeusingFtdcBoolType	IsBrokerUserEvent;
};

///�ʽ��˻���������
struct CZeusingFtdcTradingAccountPasswordUpdateV1Field
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///ԭ���Ŀ���
	TZeusingFtdcPasswordType	OldPassword;
	///�µĿ���
	TZeusingFtdcPasswordType	NewPassword;
};

///�ʽ��˻���������
struct CZeusingFtdcTradingAccountPasswordUpdateField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///ԭ���Ŀ���
	TZeusingFtdcPasswordType	OldPassword;
	///�µĿ���
	TZeusingFtdcPasswordType	NewPassword;
};

///��ѯ��Ϻ�Լ����
struct CZeusingFtdcQryCombinationLegField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ϻ�Լ����
	TZeusingFtdcInstrumentIDType	CombInstrumentID;
	///���ȱ��
	TZeusingFtdcLegIDType	LegID;
	///���Ⱥ�Լ����
	TZeusingFtdcInstrumentIDType	LegInstrumentID;
};

///��ѯ��Ϻ�Լ����
struct CZeusingFtdcQrySyncStatusField
{
	///������
	TZeusingFtdcDateType	TradingDay;
};

///��Ͻ��׺�Լ�ĵ���
struct CZeusingFtdcCombinationLegField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ϻ�Լ����
	TZeusingFtdcInstrumentIDType	CombInstrumentID;
	///���ȱ��
	TZeusingFtdcLegIDType	LegID;
	///���Ⱥ�Լ����
	TZeusingFtdcInstrumentIDType	LegInstrumentID;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///���ȳ���
	TZeusingFtdcLegMultipleType	LegMultiple;
	///��������
	TZeusingFtdcImplyLevelType	ImplyLevel;
};

///����ͬ��״̬
struct CZeusingFtdcSyncStatusField
{
	///������
	TZeusingFtdcDateType	TradingDay;
	///����ͬ��״̬
	TZeusingFtdcDataSyncStatusType	DataSyncStatus;
};

///��ѯ��ϵ��
struct CZeusingFtdcQryLinkManField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///��ϵ��
struct CZeusingFtdcLinkManField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��ϵ������
	TZeusingFtdcPersonTypeType	PersonType;
	///֤������
	TZeusingFtdcIdCardTypeType	IdentifiedCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///����
	TZeusingFtdcPartyNameType	PersonName;
	///��ϵ�绰
	TZeusingFtdcTelephoneType	Telephone;
	///ͨѶ��ַ
	TZeusingFtdcAddressType	Address;
	///��������
	TZeusingFtdcZipCodeType	ZipCode;
	///���ȼ�
	TZeusingFtdcPriorityType	Priority;
};

///��ѯ���͹�˾�û��¼�
struct CZeusingFtdcQryBrokerUserEventField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�û��¼�����
	TZeusingFtdcUserEventTypeType	UserEventType;
};

///��ѯ���͹�˾�û��¼�
struct CZeusingFtdcBrokerUserEventField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�û��¼�����
	TZeusingFtdcUserEventTypeType	UserEventType;
	///�û��¼����
	TZeusingFtdcSequenceNoType	EventSequenceNo;
	///�¼���������
	TZeusingFtdcDateType	EventDate;
	///�¼�����ʱ��
	TZeusingFtdcTimeType	EventTime;
	///�û��¼���Ϣ
	TZeusingFtdcUserEventInfoType	UserEventInfo;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///��ѯǩԼ��������
struct CZeusingFtdcQryContractBankField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з����Ĵ���
	TZeusingFtdcBankBrchIDType	BankBrchID;
};

///��ѯǩԼ������Ӧ
struct CZeusingFtdcContractBankField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з����Ĵ���
	TZeusingFtdcBankBrchIDType	BankBrchID;
	///��������
	TZeusingFtdcBankNameType	BankName;
};

///Ͷ������ϳֲ���ϸ
struct CZeusingFtdcInvestorPositionCombineDetailField
{
	///������
	TZeusingFtdcDateType	TradingDay;
	///��������
	TZeusingFtdcDateType	OpenDate;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��ϱ��
	TZeusingFtdcTradeIDType	ComTradeID;
	///��ϱ��
	TZeusingFtdcTradeIDType	TradeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///����
	TZeusingFtdcDirectionType	Direction;
	///�ֲ���
	TZeusingFtdcVolumeType	TotalAmt;
	///Ͷ���߱�֤��
	TZeusingFtdcMoneyType	Margin;
	///��������֤��
	TZeusingFtdcMoneyType	ExchMargin;
	///��֤����
	TZeusingFtdcRatioType	MarginRateByMoney;
	///��֤����(������)
	TZeusingFtdcRatioType	MarginRateByVolume;
	///���ȱ��
	TZeusingFtdcLegIDType	LegID;
	///���ȳ���
	TZeusingFtdcLegMultipleType	LegMultiple;
	///��ϳֲֺ�Լ����
	TZeusingFtdcInstrumentIDType	CombInstrumentID;
	///�ɽ����
	TZeusingFtdcTradeGroupIDType	TradeGroupID;
};

///Ԥ��
struct CZeusingFtdcParkedOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�����۸�����
	TZeusingFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///��Ͽ�ƽ��־
	TZeusingFtdcCombOffsetFlagType	CombOffsetFlag;
	///���Ͷ���ױ���־
	TZeusingFtdcCombHedgeFlagType	CombHedgeFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///����
	TZeusingFtdcVolumeType	VolumeTotalOriginal;
	///��Ч������
	TZeusingFtdcTimeConditionType	TimeCondition;
	///GTD����
	TZeusingFtdcDateType	GTDDate;
	///�ɽ�������
	TZeusingFtdcVolumeConditionType	VolumeCondition;
	///��С�ɽ���
	TZeusingFtdcVolumeType	MinVolume;
	///��������
	TZeusingFtdcContingentConditionType	ContingentCondition;
	///ֹ���
	TZeusingFtdcPriceType	StopPrice;
	///ǿƽԭ��
	TZeusingFtdcForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	TZeusingFtdcBoolType	IsAutoSuspend;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///�û�ǿ����־
	TZeusingFtdcBoolType	UserForceClose;
	///��������־
	TZeusingFtdcBoolType	IsSwapOrder;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///Ԥ�񱨵����
	TZeusingFtdcParkedOrderIDType	ParkedOrderID;
	///�û�����
	TZeusingFtdcUserTypeType	UserType;
	///Ԥ��״̬
	TZeusingFtdcParkedOrderStatusType	Status;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///����Ԥ�񵥲���
struct CZeusingFtdcParkedOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///������������
	TZeusingFtdcOrderActionRefType	OrderActionRef;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///�����仯
	TZeusingFtdcVolumeType	VolumeChange;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///Ԥ�񳷵������
	TZeusingFtdcParkedOrderActionIDType	ParkedOrderActionID;
	///�û�����
	TZeusingFtdcUserTypeType	UserType;
	///Ԥ�񳷵�״̬
	TZeusingFtdcParkedOrderStatusType	Status;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///��ѯԤ��
struct CZeusingFtdcQryParkedOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///��ѯԤ�񳷵�
struct CZeusingFtdcQryParkedOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///ɾ��Ԥ��
struct CZeusingFtdcRemoveParkedOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ԥ�񱨵����
	TZeusingFtdcParkedOrderIDType	ParkedOrderID;
};

///ɾ��Ԥ�񳷵�
struct CZeusingFtdcRemoveParkedOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///Ԥ�񳷵����
	TZeusingFtdcParkedOrderActionIDType	ParkedOrderActionID;
};

///���͹�˾�����ʽ��㷨��
struct CZeusingFtdcInvestorWithdrawAlgorithmField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///�����ʽ����
	TZeusingFtdcRatioType	UsingRatio;
};

///��ѯ��ϳֲ���ϸ
struct CZeusingFtdcQryInvestorPositionCombineDetailField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��ϳֲֺ�Լ����
	TZeusingFtdcInstrumentIDType	CombInstrumentID;
};

///�ɽ�����
struct CZeusingFtdcMarketDataAveragePriceField
{
	///���վ���
	TZeusingFtdcPriceType	AveragePrice;
};

///У��Ͷ��������
struct CZeusingFtdcVerifyInvestorPasswordField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����
	TZeusingFtdcPasswordType	Password;
};

///�û�IP
struct CZeusingFtdcUserIPField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///IP��ַ
	TZeusingFtdcIPAddressType	IPAddress;
	///IP��ַ����
	TZeusingFtdcIPAddressType	IPMask;
	///Mac��ַ
	TZeusingFtdcMacAddressType	MacAddress;
};

///�û��¼�֪ͨ��Ϣ
struct CZeusingFtdcTradingNoticeInfoField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����ʱ��
	TZeusingFtdcTimeType	SendTime;
	///��Ϣ����
	TZeusingFtdcContentType	FieldContent;
	///����ϵ�к�
	TZeusingFtdcSequenceSeriesType	SequenceSeries;
	///���к�
	TZeusingFtdcSequenceNoType	SequenceNo;
};

///�û��¼�֪ͨ
struct CZeusingFtdcTradingNoticeField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���߷�Χ
	TZeusingFtdcInvestorRangeType	InvestorRange;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����ϵ�к�
	TZeusingFtdcSequenceSeriesType	SequenceSeries;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///����ʱ��
	TZeusingFtdcTimeType	SendTime;
	///���к�
	TZeusingFtdcSequenceNoType	SequenceNo;
	///��Ϣ����
	TZeusingFtdcContentType	FieldContent;
};

///��ѯ�����¼�֪ͨ
struct CZeusingFtdcQryTradingNoticeField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///��ѯ���󱨵�
struct CZeusingFtdcQryErrOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///���󱨵�
struct CZeusingFtdcErrOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�����۸�����
	TZeusingFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///��Ͽ�ƽ��־
	TZeusingFtdcCombOffsetFlagType	CombOffsetFlag;
	///���Ͷ���ױ���־
	TZeusingFtdcCombHedgeFlagType	CombHedgeFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///����
	TZeusingFtdcVolumeType	VolumeTotalOriginal;
	///��Ч������
	TZeusingFtdcTimeConditionType	TimeCondition;
	///GTD����
	TZeusingFtdcDateType	GTDDate;
	///�ɽ�������
	TZeusingFtdcVolumeConditionType	VolumeCondition;
	///��С�ɽ���
	TZeusingFtdcVolumeType	MinVolume;
	///��������
	TZeusingFtdcContingentConditionType	ContingentCondition;
	///ֹ���
	TZeusingFtdcPriceType	StopPrice;
	///ǿƽԭ��
	TZeusingFtdcForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	TZeusingFtdcBoolType	IsAutoSuspend;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///�û�ǿ����־
	TZeusingFtdcBoolType	UserForceClose;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
	///��������־
	TZeusingFtdcBoolType	IsSwapOrder;
};

///��ѯ���󱨵�����
struct CZeusingFtdcErrorConditionalOrderField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�����۸�����
	TZeusingFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///��Ͽ�ƽ��־
	TZeusingFtdcCombOffsetFlagType	CombOffsetFlag;
	///���Ͷ���ױ���־
	TZeusingFtdcCombHedgeFlagType	CombHedgeFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///����
	TZeusingFtdcVolumeType	VolumeTotalOriginal;
	///��Ч������
	TZeusingFtdcTimeConditionType	TimeCondition;
	///GTD����
	TZeusingFtdcDateType	GTDDate;
	///�ɽ�������
	TZeusingFtdcVolumeConditionType	VolumeCondition;
	///��С�ɽ���
	TZeusingFtdcVolumeType	MinVolume;
	///��������
	TZeusingFtdcContingentConditionType	ContingentCondition;
	///ֹ���
	TZeusingFtdcPriceType	StopPrice;
	///ǿƽԭ��
	TZeusingFtdcForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	TZeusingFtdcBoolType	IsAutoSuspend;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///��Լ�ڽ������Ĵ���
	TZeusingFtdcExchangeInstIDType	ExchangeInstID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�����ύ״̬
	TZeusingFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///������ʾ���
	TZeusingFtdcSequenceNoType	NotifySequence;
	///������
	TZeusingFtdcDateType	TradingDay;
	///������
	TZeusingFtdcSettlementIDType	SettlementID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///������Դ
	TZeusingFtdcOrderSourceType	OrderSource;
	///����״̬
	TZeusingFtdcOrderStatusType	OrderStatus;
	///��������
	TZeusingFtdcOrderTypeType	OrderType;
	///��ɽ�����
	TZeusingFtdcVolumeType	VolumeTraded;
	///ʣ������
	TZeusingFtdcVolumeType	VolumeTotal;
	///��������
	TZeusingFtdcDateType	InsertDate;
	///ί��ʱ��
	TZeusingFtdcTimeType	InsertTime;
	///����ʱ��
	TZeusingFtdcTimeType	ActiveTime;
	///����ʱ��
	TZeusingFtdcTimeType	SuspendTime;
	///����޸�ʱ��
	TZeusingFtdcTimeType	UpdateTime;
	///����ʱ��
	TZeusingFtdcTimeType	CancelTime;
	///����޸Ľ���������Ա����
	TZeusingFtdcTraderIDType	ActiveTraderID;
	///�����Ա���
	TZeusingFtdcParticipantIDType	ClearingPartID;
	///���
	TZeusingFtdcSequenceNoType	SequenceNo;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///�û��˲�Ʒ��Ϣ
	TZeusingFtdcProductInfoType	UserProductInfo;
	///״̬��Ϣ
	TZeusingFtdcErrorMsgType	StatusMsg;
	///�û�ǿ����־
	TZeusingFtdcBoolType	UserForceClose;
	///�����û�����
	TZeusingFtdcUserIDType	ActiveUserID;
	///���͹�˾�������
	TZeusingFtdcSequenceNoType	BrokerOrderSeq;
	///��ر���
	TZeusingFtdcOrderSysIDType	RelativeOrderSysID;
	///֣�����ɽ�����
	TZeusingFtdcVolumeType	ZCETotalTradedVolume;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
	///��������־
	TZeusingFtdcBoolType	IsSwapOrder;
};

///��ѯ���󱨵�����
struct CZeusingFtdcQryErrOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///���󱨵�����
struct CZeusingFtdcErrOrderActionField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///������������
	TZeusingFtdcOrderActionRefType	OrderActionRef;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///ǰ�ñ��
	TZeusingFtdcFrontIDType	FrontID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�������
	TZeusingFtdcOrderSysIDType	OrderSysID;
	///������־
	TZeusingFtdcActionFlagType	ActionFlag;
	///�۸�
	TZeusingFtdcPriceType	LimitPrice;
	///�����仯
	TZeusingFtdcVolumeType	VolumeChange;
	///��������
	TZeusingFtdcDateType	ActionDate;
	///����ʱ��
	TZeusingFtdcTimeType	ActionTime;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�������ر��
	TZeusingFtdcOrderLocalIDType	ActionLocalID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
	///��������״̬
	TZeusingFtdcOrderActionStatusType	OrderActionStatus;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///״̬��Ϣ
	TZeusingFtdcErrorMsgType	StatusMsg;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///��ѯ������״̬
struct CZeusingFtdcQryExchangeSequenceField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///������״̬
struct CZeusingFtdcExchangeSequenceField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///���
	TZeusingFtdcSequenceNoType	SequenceNo;
	///��Լ����״̬
	TZeusingFtdcInstrumentStatusType	MarketStatus;
};

///���ݼ۸��ѯ��󱨵�����
struct CZeusingFtdcQueryMaxOrderVolumeWithPriceField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///��ƽ��־
	TZeusingFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///�������������
	TZeusingFtdcVolumeType	MaxVolume;
	///�����۸�
	TZeusingFtdcPriceType	Price;
};

///��ѯ���͹�˾���ײ���
struct CZeusingFtdcQryBrokerTradingParamsField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///���͹�˾���ײ���
struct CZeusingFtdcBrokerTradingParamsField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��֤��۸�����
	TZeusingFtdcMarginPriceTypeType	MarginPriceType;
	///ӯ���㷨
	TZeusingFtdcAlgorithmType	Algorithm;
	///�����Ƿ����ƽ��ӯ��
	TZeusingFtdcIncludeCloseProfitType	AvailIncludeCloseProfit;
};

///��ѯ���͹�˾�����㷨
struct CZeusingFtdcQryBrokerTradingAlgosField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///���͹�˾�����㷨
struct CZeusingFtdcBrokerTradingAlgosField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///�ֲִ����㷨���
	TZeusingFtdcHandlePositionAlgoIDType	HandlePositionAlgoID;
	///Ѱ�ұ�֤�����㷨���
	TZeusingFtdcFindMarginRateAlgoIDType	FindMarginRateAlgoID;
	///�ʽ����㷨���
	TZeusingFtdcHandleTradingAccountAlgoIDType	HandleTradingAccountAlgoID;
};

///��ѯ���͹�˾�ʽ�
struct CZeusingFtdcQueryBrokerDepositField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
};

///���͹�˾�ʽ�
struct CZeusingFtdcBrokerDepositField
{
	///��������
	TZeusingFtdcTradeDateType	TradingDay;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///�ϴν���׼����
	TZeusingFtdcMoneyType	PreBalance;
	///��ǰ��֤���ܶ�
	TZeusingFtdcMoneyType	CurrMargin;
	///ƽ��ӯ��
	TZeusingFtdcMoneyType	CloseProfit;
	///�ڻ�����׼����
	TZeusingFtdcMoneyType	Balance;
	///�����
	TZeusingFtdcMoneyType	Deposit;
	///������
	TZeusingFtdcMoneyType	Withdraw;
	///�����ʽ�
	TZeusingFtdcMoneyType	Available;
	///����׼����
	TZeusingFtdcMoneyType	Reserve;
	///����ı�֤��
	TZeusingFtdcMoneyType	FrozenMargin;
};

///��ѯ��֤����ϵͳ���͹�˾��Կ
struct CZeusingFtdcQryCFMMCBrokerKeyField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
};

///��֤����ϵͳ���͹�˾��Կ
struct CZeusingFtdcCFMMCBrokerKeyField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///���͹�˾ͳһ����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///��Կ��������
	TZeusingFtdcDateType	CreateDate;
	///��Կ����ʱ��
	TZeusingFtdcTimeType	CreateTime;
	///��Կ���
	TZeusingFtdcSequenceNoType	KeyID;
	///��̬��Կ
	TZeusingFtdcCFMMCKeyType	CurrentKey;
	///��̬��Կ����
	TZeusingFtdcCFMMCKeyKindType	KeyKind;
};

///��֤����ϵͳ���͹�˾�ʽ��˻���Կ
struct CZeusingFtdcCFMMCTradingAccountKeyField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///���͹�˾ͳһ����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///��Կ���
	TZeusingFtdcSequenceNoType	KeyID;
	///��̬��Կ
	TZeusingFtdcCFMMCKeyType	CurrentKey;
};

///�����ѯ��֤����ϵͳ���͹�˾�ʽ��˻���Կ
struct CZeusingFtdcQryCFMMCTradingAccountKeyField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
};

///�û���̬���Ʋ���
struct CZeusingFtdcBrokerUserOTPParamField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��̬�����ṩ��
	TZeusingFtdcOTPVendorsIDType	OTPVendorsID;
	///��̬�������к�
	TZeusingFtdcSerialNumberType	SerialNumber;
	///������Կ
	TZeusingFtdcAuthKeyType	AuthKey;
	///Ư��ֵ
	TZeusingFtdcLastDriftType	LastDrift;
	///�ɹ�ֵ
	TZeusingFtdcLastSuccessType	LastSuccess;
	///��̬��������
	TZeusingFtdcOTPTypeType	OTPType;
};

///�ֹ�ͬ���û���̬����
struct CZeusingFtdcManualSyncBrokerUserOTPField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///��̬��������
	TZeusingFtdcOTPTypeType	OTPType;
	///��һ����̬����
	TZeusingFtdcPasswordType	FirstOTP;
	///�ڶ�����̬����
	TZeusingFtdcPasswordType	SecondOTP;
};

///Ͷ������������ģ��
struct CZeusingFtdcCommRateModelField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///��������ģ�����
	TZeusingFtdcInvestorIDType	CommModelID;
	///ģ������
	TZeusingFtdcCommModelNameType	CommModelName;
};

///�����ѯͶ������������ģ��
struct CZeusingFtdcQryCommRateModelField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///��������ģ�����
	TZeusingFtdcInvestorIDType	CommModelID;
};

///Ͷ���߱�֤����ģ��
struct CZeusingFtdcMarginModelField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///��֤����ģ�����
	TZeusingFtdcInvestorIDType	MarginModelID;
	///ģ������
	TZeusingFtdcCommModelNameType	MarginModelName;
};

///�ֵ��۵���Ϣ
struct CZeusingFtdcEWarrantOffsetField
{
	///��������
	TZeusingFtdcTradeDateType	TradingDay;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///��������
	TZeusingFtdcDirectionType	Direction;
	///Ͷ���ױ���־
	TZeusingFtdcHedgeFlagType	HedgeFlag;
	///����
	TZeusingFtdcVolumeType	Volume;
};

///��ѯ�ֵ��۵���Ϣ
struct CZeusingFtdcQryEWarrantOffsetField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
};

///ת�ʿ�������
struct CZeusingFtdcReqOpenAccountField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�Ա�
	TZeusingFtdcGenderType	Gender;
	///���Ҵ���
	TZeusingFtdcCountryCodeType	CountryCode;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///��ַ
	TZeusingFtdcAddressType	Address;
	///�ʱ�
	TZeusingFtdcZipCodeType	ZipCode;
	///�绰����
	TZeusingFtdcTelephoneType	Telephone;
	///�ֻ�
	TZeusingFtdcMobilePhoneType	MobilePhone;
	///����
	TZeusingFtdcFaxType	Fax;
	///�����ʼ�
	TZeusingFtdcEMailType	EMail;
	///�ʽ��˻�״̬
	TZeusingFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///�㳮��־
	TZeusingFtdcCashExchangeCodeType	CashExchangeCode;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
};

///ת����������
struct CZeusingFtdcReqCancelAccountField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�Ա�
	TZeusingFtdcGenderType	Gender;
	///���Ҵ���
	TZeusingFtdcCountryCodeType	CountryCode;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///��ַ
	TZeusingFtdcAddressType	Address;
	///�ʱ�
	TZeusingFtdcZipCodeType	ZipCode;
	///�绰����
	TZeusingFtdcTelephoneType	Telephone;
	///�ֻ�
	TZeusingFtdcMobilePhoneType	MobilePhone;
	///����
	TZeusingFtdcFaxType	Fax;
	///�����ʼ�
	TZeusingFtdcEMailType	EMail;
	///�ʽ��˻�״̬
	TZeusingFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///�㳮��־
	TZeusingFtdcCashExchangeCodeType	CashExchangeCode;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
};

///��������˻�����
struct CZeusingFtdcReqChangeAccountField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�Ա�
	TZeusingFtdcGenderType	Gender;
	///���Ҵ���
	TZeusingFtdcCountryCodeType	CountryCode;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///��ַ
	TZeusingFtdcAddressType	Address;
	///�ʱ�
	TZeusingFtdcZipCodeType	ZipCode;
	///�绰����
	TZeusingFtdcTelephoneType	Telephone;
	///�ֻ�
	TZeusingFtdcMobilePhoneType	MobilePhone;
	///����
	TZeusingFtdcFaxType	Fax;
	///�����ʼ�
	TZeusingFtdcEMailType	EMail;
	///�ʽ��˻�״̬
	TZeusingFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///�������ʺ�
	TZeusingFtdcBankAccountType	NewBankAccount;
	///����������
	TZeusingFtdcPasswordType	NewBankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///����ID
	TZeusingFtdcTIDType	TID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
};

///ת������
struct CZeusingFtdcReqTransferField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TZeusingFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///ת�ʽ��
	TZeusingFtdcTradeAmountType	TradeAmount;
	///�ڻ���ȡ���
	TZeusingFtdcTradeAmountType	FutureFetchAmount;
	///����֧����־
	TZeusingFtdcFeePayFlagType	FeePayFlag;
	///Ӧ�տͻ�����
	TZeusingFtdcCustFeeType	CustFee;
	///Ӧ���ڻ���˾����
	TZeusingFtdcFutureFeeType	BrokerFee;
	///���ͷ������շ�����Ϣ
	TZeusingFtdcAddInfoType	Message;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///ת�˽���״̬
	TZeusingFtdcTransferStatusType	TransferStatus;
};

///���з��������ʽ�ת�ڻ���Ӧ
struct CZeusingFtdcRspTransferField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TZeusingFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///ת�ʽ��
	TZeusingFtdcTradeAmountType	TradeAmount;
	///�ڻ���ȡ���
	TZeusingFtdcTradeAmountType	FutureFetchAmount;
	///����֧����־
	TZeusingFtdcFeePayFlagType	FeePayFlag;
	///Ӧ�տͻ�����
	TZeusingFtdcCustFeeType	CustFee;
	///Ӧ���ڻ���˾����
	TZeusingFtdcFutureFeeType	BrokerFee;
	///���ͷ������շ�����Ϣ
	TZeusingFtdcAddInfoType	Message;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///ת�˽���״̬
	TZeusingFtdcTransferStatusType	TransferStatus;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///��������
struct CZeusingFtdcReqRepealField
{
	///����ʱ����
	TZeusingFtdcRepealTimeIntervalType	RepealTimeInterval;
	///�Ѿ���������
	TZeusingFtdcRepealedTimesType	RepealedTimes;
	///���г�����־
	TZeusingFtdcBankRepealFlagType	BankRepealFlag;
	///���̳�����־
	TZeusingFtdcBrokerRepealFlagType	BrokerRepealFlag;
	///������ƽ̨��ˮ��
	TZeusingFtdcPlateSerialType	PlateRepealSerial;
	///������������ˮ��
	TZeusingFtdcBankSerialType	BankRepealSerial;
	///�������ڻ���ˮ��
	TZeusingFtdcFutureSerialType	FutureRepealSerial;
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TZeusingFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///ת�ʽ��
	TZeusingFtdcTradeAmountType	TradeAmount;
	///�ڻ���ȡ���
	TZeusingFtdcTradeAmountType	FutureFetchAmount;
	///����֧����־
	TZeusingFtdcFeePayFlagType	FeePayFlag;
	///Ӧ�տͻ�����
	TZeusingFtdcCustFeeType	CustFee;
	///Ӧ���ڻ���˾����
	TZeusingFtdcFutureFeeType	BrokerFee;
	///���ͷ������շ�����Ϣ
	TZeusingFtdcAddInfoType	Message;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///ת�˽���״̬
	TZeusingFtdcTransferStatusType	TransferStatus;
};

///������Ӧ
struct CZeusingFtdcRspRepealField
{
	///����ʱ����
	TZeusingFtdcRepealTimeIntervalType	RepealTimeInterval;
	///�Ѿ���������
	TZeusingFtdcRepealedTimesType	RepealedTimes;
	///���г�����־
	TZeusingFtdcBankRepealFlagType	BankRepealFlag;
	///���̳�����־
	TZeusingFtdcBrokerRepealFlagType	BrokerRepealFlag;
	///������ƽ̨��ˮ��
	TZeusingFtdcPlateSerialType	PlateRepealSerial;
	///������������ˮ��
	TZeusingFtdcBankSerialType	BankRepealSerial;
	///�������ڻ���ˮ��
	TZeusingFtdcFutureSerialType	FutureRepealSerial;
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TZeusingFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///ת�ʽ��
	TZeusingFtdcTradeAmountType	TradeAmount;
	///�ڻ���ȡ���
	TZeusingFtdcTradeAmountType	FutureFetchAmount;
	///����֧����־
	TZeusingFtdcFeePayFlagType	FeePayFlag;
	///Ӧ�տͻ�����
	TZeusingFtdcCustFeeType	CustFee;
	///Ӧ���ڻ���˾����
	TZeusingFtdcFutureFeeType	BrokerFee;
	///���ͷ������շ�����Ϣ
	TZeusingFtdcAddInfoType	Message;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///ת�˽���״̬
	TZeusingFtdcTransferStatusType	TransferStatus;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///��ѯ�˻���Ϣ����
struct CZeusingFtdcReqQueryAccountField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///�ڻ���˾��ˮ��
	TZeusingFtdcFutureSerialType	FutureSerial;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
};

///��ѯ�˻���Ϣ��Ӧ
struct CZeusingFtdcRspQueryAccountField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///�ڻ���˾��ˮ��
	TZeusingFtdcFutureSerialType	FutureSerial;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///���п��ý��
	TZeusingFtdcTradeAmountType	BankUseAmount;
	///���п�ȡ���
	TZeusingFtdcTradeAmountType	BankFetchAmount;
};

///����ǩ��ǩ��
struct CZeusingFtdcFutureSignIOField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
};

///����ǩ����Ӧ
struct CZeusingFtdcRspFutureSignInField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
	///PIN��Կ
	TZeusingFtdcPasswordKeyType	PinKey;
	///MAC��Կ
	TZeusingFtdcPasswordKeyType	MacKey;
};

///����ǩ������
struct CZeusingFtdcReqFutureSignOutField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
};

///����ǩ����Ӧ
struct CZeusingFtdcRspFutureSignOutField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///��ѯָ����ˮ�ŵĽ��׽������
struct CZeusingFtdcReqQueryTradeResultBySerialField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///��ˮ��
	TZeusingFtdcSerialType	Reference;
	///����ˮ�ŷ����ߵĻ�������
	TZeusingFtdcInstitutionTypeType	RefrenceIssureType;
	///����ˮ�ŷ����߻�������
	TZeusingFtdcOrganCodeType	RefrenceIssure;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///ת�ʽ��
	TZeusingFtdcTradeAmountType	TradeAmount;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
};

///��ѯָ����ˮ�ŵĽ��׽����Ӧ
struct CZeusingFtdcRspQueryTradeResultBySerialField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
	///��ˮ��
	TZeusingFtdcSerialType	Reference;
	///����ˮ�ŷ����ߵĻ�������
	TZeusingFtdcInstitutionTypeType	RefrenceIssureType;
	///����ˮ�ŷ����߻�������
	TZeusingFtdcOrganCodeType	RefrenceIssure;
	///ԭʼ���ش���
	TZeusingFtdcReturnCodeType	OriginReturnCode;
	///ԭʼ����������
	TZeusingFtdcDescrInfoForReturnCodeType	OriginDescrInfoForReturnCode;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///ת�ʽ��
	TZeusingFtdcTradeAmountType	TradeAmount;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
};

///�����ļ���������
struct CZeusingFtdcReqDayEndFileReadyField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ļ�ҵ����
	TZeusingFtdcFileBusinessCodeType	FileBusinessCode;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
};

///���ؽ��
struct CZeusingFtdcReturnResultField
{
	///���ش���
	TZeusingFtdcReturnCodeType	ReturnCode;
	///����������
	TZeusingFtdcDescrInfoForReturnCodeType	DescrInfoForReturnCode;
};

///��֤�ڻ��ʽ�����
struct CZeusingFtdcVerifyFuturePasswordField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///����ID
	TZeusingFtdcTIDType	TID;
};

///��֤�ͻ���Ϣ
struct CZeusingFtdcVerifyCustInfoField
{
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
};

///��֤�ڻ��ʽ�����Ϳͻ���Ϣ
struct CZeusingFtdcVerifyFuturePasswordAndCustInfoField
{
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
};

///��֤�ڻ��ʽ�����Ϳͻ���Ϣ
struct CZeusingFtdcDepositResultInformField
{
	///�������ˮ�ţ�����ˮ��Ϊ���ڱ��̷��ص���ˮ��
	TZeusingFtdcDepositSeqNoType	DepositSeqNo;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///�����
	TZeusingFtdcMoneyType	Deposit;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///���ش���
	TZeusingFtdcReturnCodeType	ReturnCode;
	///����������
	TZeusingFtdcDescrInfoForReturnCodeType	DescrInfoForReturnCode;
};

///���׺��������ڱ��̷�����Կͬ������
struct CZeusingFtdcReqSyncKeyField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///���׺��ĸ����ڱ��̵���Ϣ
	TZeusingFtdcAddInfoType	Message;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
};

///���׺��������ڱ��̷�����Կͬ����Ӧ
struct CZeusingFtdcRspSyncKeyField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///���׺��ĸ����ڱ��̵���Ϣ
	TZeusingFtdcAddInfoType	Message;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///��ѯ�˻���Ϣ֪ͨ
struct CZeusingFtdcNotifyQueryAccountField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///�ڻ���˾��ˮ��
	TZeusingFtdcFutureSerialType	FutureSerial;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///���п��ý��
	TZeusingFtdcTradeAmountType	BankUseAmount;
	///���п�ȡ���
	TZeusingFtdcTradeAmountType	BankFetchAmount;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///����ת�˽�����ˮ��
struct CZeusingFtdcTransferSerialField
{
	///ƽ̨��ˮ��
	TZeusingFtdcPlateSerialType	PlateSerial;
	///���׷�������
	TZeusingFtdcTradeDateType	TradeDate;
	///��������
	TZeusingFtdcDateType	TradingDay;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///���״���
	TZeusingFtdcTradeCodeType	TradeCode;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///���б���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///�ڻ���˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///�ڻ���˾�ʺ�����
	TZeusingFtdcFutureAccTypeType	FutureAccType;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///�ڻ���˾��ˮ��
	TZeusingFtdcFutureSerialType	FutureSerial;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///���׽��
	TZeusingFtdcTradeAmountType	TradeAmount;
	///Ӧ�տͻ�����
	TZeusingFtdcCustFeeType	CustFee;
	///Ӧ���ڻ���˾����
	TZeusingFtdcFutureFeeType	BrokerFee;
	///��Ч��־
	TZeusingFtdcAvailabilityFlagType	AvailabilityFlag;
	///����Ա
	TZeusingFtdcOperatorCodeType	OperatorCode;
	///�������ʺ�
	TZeusingFtdcBankAccountType	BankNewAccount;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///�����ѯת����ˮ
struct CZeusingFtdcQryTransferSerialField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///���б���
	TZeusingFtdcBankIDType	BankID;
};

///����ǩ��֪ͨ
struct CZeusingFtdcNotifyFutureSignInField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
	///PIN��Կ
	TZeusingFtdcPasswordKeyType	PinKey;
	///MAC��Կ
	TZeusingFtdcPasswordKeyType	MacKey;
};

///����ǩ��֪ͨ
struct CZeusingFtdcNotifyFutureSignOutField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///���׺��������ڱ��̷�����Կͬ����������֪ͨ
struct CZeusingFtdcNotifySyncKeyField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///���׺��ĸ����ڱ��̵���Ϣ
	TZeusingFtdcAddInfoType	Message;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///������
	TZeusingFtdcRequestIDType	RequestID;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///�����ѯ����ǩԼ��ϵ
struct CZeusingFtdcQryAccountregisterField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///���б���
	TZeusingFtdcBankIDType	BankID;
};

///�ͻ���������Ϣ��
struct CZeusingFtdcAccountregisterField
{
	///��������
	TZeusingFtdcTradeDateType	TradeDay;
	///���б���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///�ڻ���˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�ڻ���˾��֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///���������
	TZeusingFtdcOpenOrDestroyType	OpenOrDestroy;
	///ǩԼ����
	TZeusingFtdcTradeDateType	RegDate;
	///��Լ����
	TZeusingFtdcTradeDateType	OutDate;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
};

///���ڿ�����Ϣ
struct CZeusingFtdcOpenAccountField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�Ա�
	TZeusingFtdcGenderType	Gender;
	///���Ҵ���
	TZeusingFtdcCountryCodeType	CountryCode;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///��ַ
	TZeusingFtdcAddressType	Address;
	///�ʱ�
	TZeusingFtdcZipCodeType	ZipCode;
	///�绰����
	TZeusingFtdcTelephoneType	Telephone;
	///�ֻ�
	TZeusingFtdcMobilePhoneType	MobilePhone;
	///����
	TZeusingFtdcFaxType	Fax;
	///�����ʼ�
	TZeusingFtdcEMailType	EMail;
	///�ʽ��˻�״̬
	TZeusingFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///�㳮��־
	TZeusingFtdcCashExchangeCodeType	CashExchangeCode;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///����������Ϣ
struct CZeusingFtdcCancelAccountField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�Ա�
	TZeusingFtdcGenderType	Gender;
	///���Ҵ���
	TZeusingFtdcCountryCodeType	CountryCode;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///��ַ
	TZeusingFtdcAddressType	Address;
	///�ʱ�
	TZeusingFtdcZipCodeType	ZipCode;
	///�绰����
	TZeusingFtdcTelephoneType	Telephone;
	///�ֻ�
	TZeusingFtdcMobilePhoneType	MobilePhone;
	///����
	TZeusingFtdcFaxType	Fax;
	///�����ʼ�
	TZeusingFtdcEMailType	EMail;
	///�ʽ��˻�״̬
	TZeusingFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///�㳮��־
	TZeusingFtdcCashExchangeCodeType	CashExchangeCode;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///������־
	TZeusingFtdcDeviceIDType	DeviceID;
	///�ڻ���λ�ʺ�����
	TZeusingFtdcBankAccTypeType	BankSecuAccType;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TZeusingFtdcBankAccountType	BankSecuAcc;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///���׹�Ա
	TZeusingFtdcOperNoType	OperNo;
	///����ID
	TZeusingFtdcTIDType	TID;
	///�û���ʶ
	TZeusingFtdcUserIDType	UserID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///���ڱ�������˺���Ϣ
struct CZeusingFtdcChangeAccountField
{
	///ҵ������
	TZeusingFtdcTradeCodeType	TradeCode;
	///���д���
	TZeusingFtdcBankIDType	BankID;
	///���з�֧��������
	TZeusingFtdcBankBrchIDType	BankBranchID;
	///���̴���
	TZeusingFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TZeusingFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TZeusingFtdcTradeDateType	TradeDate;
	///����ʱ��
	TZeusingFtdcTradeTimeType	TradeTime;
	///������ˮ��
	TZeusingFtdcBankSerialType	BankSerial;
	///����ϵͳ���� 
	TZeusingFtdcTradeDateType	TradingDay;
	///����ƽ̨��Ϣ��ˮ��
	TZeusingFtdcSerialType	PlateSerial;
	///����Ƭ��־
	TZeusingFtdcLastFragmentType	LastFragment;
	///�Ự��
	TZeusingFtdcSessionIDType	SessionID;
	///�ͻ�����
	TZeusingFtdcIndividualNameType	CustomerName;
	///֤������
	TZeusingFtdcIdCardTypeType	IdCardType;
	///֤������
	TZeusingFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�Ա�
	TZeusingFtdcGenderType	Gender;
	///���Ҵ���
	TZeusingFtdcCountryCodeType	CountryCode;
	///�ͻ�����
	TZeusingFtdcCustTypeType	CustType;
	///��ַ
	TZeusingFtdcAddressType	Address;
	///�ʱ�
	TZeusingFtdcZipCodeType	ZipCode;
	///�绰����
	TZeusingFtdcTelephoneType	Telephone;
	///�ֻ�
	TZeusingFtdcMobilePhoneType	MobilePhone;
	///����
	TZeusingFtdcFaxType	Fax;
	///�����ʼ�
	TZeusingFtdcEMailType	EMail;
	///�ʽ��˻�״̬
	TZeusingFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///�����ʺ�
	TZeusingFtdcBankAccountType	BankAccount;
	///��������
	TZeusingFtdcPasswordType	BankPassWord;
	///�������ʺ�
	TZeusingFtdcBankAccountType	NewBankAccount;
	///����������
	TZeusingFtdcPasswordType	NewBankPassWord;
	///Ͷ�����ʺ�
	TZeusingFtdcAccountIDType	AccountID;
	///�ڻ�����
	TZeusingFtdcPasswordType	Password;
	///�����ʺ�����
	TZeusingFtdcBankAccTypeType	BankAccType;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///��֤�ͻ�֤�������־
	TZeusingFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///���ִ���
	TZeusingFtdcCurrencyIDType	CurrencyID;
	///�ڻ���˾���б���
	TZeusingFtdcBankCodingForFutureType	BrokerIDByBank;
	///���������־
	TZeusingFtdcPwdFlagType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TZeusingFtdcPwdFlagType	SecuPwdFlag;
	///����ID
	TZeusingFtdcTIDType	TID;
	///ժҪ
	TZeusingFtdcDigestType	Digest;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///�ֱ����Ľ���Ȩ��
struct CZeusingFtdcUserRightsAssignField
{
	///Ӧ�õ�Ԫ����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///�������Ĵ���
	TZeusingFtdcDRIdentityIDType	DRIdentityID;
};

///���ù�˾�Ƿ����ڱ���ʾ�Ľ���Ȩ��
struct CZeusingFtdcBrokerUserRightAssignField
{
	///Ӧ�õ�Ԫ����
	TZeusingFtdcBrokerIDType	BrokerID;
	///�������Ĵ���
	TZeusingFtdcDRIdentityIDType	DRIdentityID;
	///�ܷ���
	TZeusingFtdcBoolType	Tradeable;
};

///�ֱ�����ת������
struct CZeusingFtdcDRTransferField
{
	///ԭ�������Ĵ���
	TZeusingFtdcDRIdentityIDType	OrigDRIdentityID;
	///Ŀ�꽻�����Ĵ���
	TZeusingFtdcDRIdentityIDType	DestDRIdentityID;
	///ԭӦ�õ�Ԫ����
	TZeusingFtdcBrokerIDType	OrigBrokerID;
	///Ŀ�����õ�Ԫ����
	TZeusingFtdcBrokerIDType	DestBrokerID;
};

///ѯ��
struct CZeusingFtdcForQuoteField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�û�����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///ѯ��ʱ��
	TZeusingFtdcTimeType	ReqForQuoteTime;
	///ѯ�۱��
	TZeusingFtdcOrderSysIDType	ReqForQuoteID;
	///������
	TZeusingFtdcDateType	TradingDay;
};

///ѯ������
struct CZeusingFtdcInputForQuoteField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�û�����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///ѯ��ʱ��
	TZeusingFtdcTimeType	ReqForQuoteTime;
};

///������ѯ������
struct CZeusingFtdcExchRspForQuoteField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�û�����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///ѯ��ʱ��
	TZeusingFtdcTimeType	ReqForQuoteTime;
};

///������ѯ�۴���
struct CZeusingFtdcErrRtnExchRtnForQuoteField
{
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///��Լ����
	TZeusingFtdcInstrumentIDType	InstrumentID;
	///�Ự���
	TZeusingFtdcSessionIDType	SessionID;
	///��������
	TZeusingFtdcOrderRefType	OrderRef;
	///���ر������
	TZeusingFtdcOrderLocalIDType	OrderLocalID;
	///�û�����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///�û�����
	TZeusingFtdcUserIDType	UserID;
	///ѯ��ʱ��
	TZeusingFtdcTimeType	ReqForQuoteTime;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
};

///���������۴���
struct CZeusingFtdcExchangeQuoteInsertErrorField
{
	///���͹�˾����
	TZeusingFtdcBrokerIDType	BrokerID;
	///����������
	TZeusingFtdcExchangeIDType	ExchangeID;
	///��Ա����
	TZeusingFtdcParticipantIDType	ParticipantID;
	///����������Ա����
	TZeusingFtdcTraderIDType	TraderID;
	///��װ���
	TZeusingFtdcInstallIDType	InstallID;
	///���ر������
	TZeusingFtdcOrderLocalIDType	QuoteLocalID;
	///�������
	TZeusingFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZeusingFtdcErrorMsgType	ErrorMsg;
	///�ͻ�����
	TZeusingFtdcClientIDType	ClientID;
	///Ͷ���ߴ���
	TZeusingFtdcInvestorIDType	InvestorID;
	///ҵ��Ԫ
	TZeusingFtdcBusinessUnitType	BusinessUnit;
};


#pragma pack(pop)

#endif
