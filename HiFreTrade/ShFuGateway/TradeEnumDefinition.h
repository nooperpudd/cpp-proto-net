#pragma once

///TFtdcOrderPriceTypeType��һ�������۸���������
enum OrderPriceTypeType
{
	///�����
	ANY_PRICE = 49,
	///�޼�
	LIMIT_PRICE = 50,
	///���ż�
	BEST_PRICE = 51,
	///���¼�
	LAST_PRICE = 52,
	///���¼۸����ϸ�1��ticks
	LAST_PRICE_PLUS_ONE_TICKS = 53,
	///���¼۸����ϸ�2��ticks
	LAST_PRICE_PLUS_TWO_TICKS = 54,
	///���¼۸����ϸ�3��ticks
	LAST_PRICE_PLUS_THREE_TICKS = 55,
	///��һ��
	ASK_PRICE1 = 56,
	///��һ�۸����ϸ�1��ticks
	ASK_PRICE1_PLUS_ONE_TICKS = 57,
	///��һ�۸����ϸ�2��ticks
	ASK_PRICE1_PLUS_TWO_TICKS = 65,
	///��һ�۸����ϸ�3��ticks
	ASK_PRICE1_PLUS_THREE_TICKS = 66,
	///��һ��
	BID_PRICE1 = 67,
	///��һ�۸����ϸ�1��ticks
	BID_PRICE1_PLUS_ONE_TICKS = 68,
	///��һ�۸����ϸ�2��ticks
	BID_PRICE1_PLUS_TWO_TICKS = 69,
	///��һ�۸����ϸ�3��ticks
	BID_PRICE1_PLUS_THREE_TICKS = 70
};

///TFtdcDirectionType��һ��������������
enum TradeDirectionType
{
	///��
	BUY = 48,
	///��
	SELL = 49
};

///TFtdcTimeConditionType��һ����Ч����������
enum TimeConditionType
{
	///������ɣ�������
	TC_IOC = 49,
	///������Ч
	TC_GFS = 50,
	///������Ч
	TC_GFD = 51,
	///ָ������ǰ��Ч
	TC_GTD = 52,
	///����ǰ��Ч
	TC_GTC = 53,
	///���Ͼ�����Ч
	TC_GFA = 54
};

///TFtdcVolumeConditionType��һ���ɽ�����������
enum VolumeConditionType
{
	///�κ�����
	VC_AV = 49,
	///��С����
	VC_MV = 50,
	///ȫ������
	VC_CV = 51
};

///TFtdcContingentConditionType��һ��������������
enum ContingentConditionType
{
	///����
	IMMEDIATELY = 49,
	///ֹ��
	TOUCH = 50,
	///ֹӮ
	TOUCH_PROFIT = 51,
	///Ԥ��
	PARKED_ORDER = 52,
	///���¼۴���������
	LAST_PRICE_GREATER_THAN_STOP_PRICE = 53,
	///���¼۴��ڵ���������
	LAST_PRICE_GREATER_EQUAL_STOP_PRICE = 54,
	///���¼�С��������
	LAST_PRICE_LESSER_THAN_STOP_PRICE = 55,
	///���¼�С�ڵ���������
	LAST_PRICE_LESSER_EQUAL_STOP_PRICE = 56,
	///��һ�۴���������
	ASK_PRICE_GREATER_THAN_STOP_PRICE = 57,
	///��һ�۴��ڵ���������
	ASK_PRICE_GREATER_EQUAL_STOP_PRICE = 65,
	///��һ��С��������
	ASK_PRICE_LESSER_THAN_STOP_PRICE = 66,
	///��һ��С�ڵ���������
	ASK_PRICE_LESSER_EQUAL_STOP_PRICE = 67,
	///��һ�۴���������
	BID_PRICE_GREATER_THAN_STOP_PRICE = 68,
	///��һ�۴��ڵ���������
	BID_PRICE_GREATER_EQUAL_STOP_PRICE = 69,
	///��һ��С��������
	BID_PRICE_LESSER_THAN_STOP_PRICE = 70,
	///��һ��С�ڵ���������
	BID_PRICE_LESSER_EQUAL_STOP_PRICE = 71
};

///TFtdcForceCloseReasonType��һ��ǿƽԭ������
enum ForceCloseReasonType
{
	///��ǿƽ
	NOT_FORCE_CLOSE = 48,
	///�ʽ���
	LACK_DEPOSIT = 49,
	///�ͻ�����
	CLIENT_OVER_POSITION_LIMIT = 50,
	///��Ա����
	MEMBER_OVER_POSITION_LIMIT = 51,
	///�ֲַ�������
	NOT_MULTIPLE = 52,
	///Υ��
	VIOLATION = 53,
	///����
	FCC_OTHER = 54,
	///��Ȼ���ٽ�����
	PERSON_DELIV = 55
};

///TFtdcOrderSubmitStatusType��һ�������ύ״̬����
enum OrderSubmitStatusType
{
	///�Ѿ��ύ
	INSERT_SUBMITTED = 48,
	///�����Ѿ��ύ
	CANCEL_SUBMITTED = 49,
	///�޸��Ѿ��ύ
	MODIFY_SUBMITTED = 50,
	///�Ѿ�����
	ACCEPTED = 51,
	///�����Ѿ����ܾ�
	INSERT_REJECTED = 52,
	///�����Ѿ����ܾ�
	CANCEL_REJECTED = 53,
	///�ĵ��Ѿ����ܾ�
	MODIFY_REJECTED = 54
};

///TFtdcOrderSourceType��һ��������Դ����
enum OrderSourceType
{
	///���Բ�����
	PARTICIPANT = 48,
	///���Թ���Ա
	ADMINISTRATOR = 49
};

///TFtdcOrderStatusType��һ������״̬����
enum OrderStatusType
{
	///ȫ���ɽ�
	ALL_TRADED = 48,
	///���ֳɽ����ڶ�����
	PART_TRADED_QUEUEING = 49,
	///���ֳɽ����ڶ�����
	PART_TRADED_NOT_QUEUEING = 50,
	///δ�ɽ����ڶ�����
	NO_TRADE_QUEUEING = 51,
	///δ�ɽ����ڶ�����
	NO_TRADE_NOT_QUEUEING = 52,
	///����
	ORDER_CANCELED = 53,
	///δ֪
	STATUS_UNKNOWN = 97,
	///��δ����
	ORDER_NOT_TOUCHED = 98,
	///�Ѵ���
	ORDER_TOUCHED = 99
};

///TFtdcOrderTypeType��һ��������������
enum OrderTypeType
{
	///����
	NORMAL_ORDER = 48,
	///��������
	DERIVE_FROM_QUOTE = 49,
	///�������
	DERIVE_FROM_COMBINATION = 50,
	///��ϱ���
	COMBINATION = 51,
	///������
	CONDITIONAL_ORDER = 52,
	///������
	SWAP_ORDER = 53
};

///TFtdcTradingRoleType��һ�����׽�ɫ����
enum TradingRoleType
{
	///����
	ER_BROKER = 49,
	///��Ӫ
	ER_HOST = 50,
	///������
	ER_MAKER = 51
};

///TFtdcOffsetFlagType��һ����ƽ��־����
enum OffsetFlagType
{
	///����
	OF_OPEN = 48,
	///ƽ��
	OF_CLOSE = 49,
	///ǿƽ
	OF_FORCE_CLOSE = 50,
	///ƽ��
	OF_CLOSE_TODAY = 51,
	///ƽ��
	OF_CLOSE_YESTERDAY = 52,
	///ǿ��
	OF_FORCE_OFF = 53,
	///����ǿƽ
	OF_LOCAL_FORCE_CLOSE = 54
};

///TFtdcTradeTypeType��һ���ɽ���������
enum TradeTypeType
{
	///��ͨ�ɽ�
	TRDT_COMMON = 48,
	///��Ȩִ��
	TRDT_OPTIONS_EXECUTION = 49,
	///OTC�ɽ�
	TRDT_OTC = 50,
	///��ת�������ɽ�
	TRDT_EFP_DERIVED = 51,
	///��������ɽ�
	TRDT_COMBINATION_DERIVED = 52
};

///TFtdcPriceSourceType��һ���ɽ�����Դ����
enum PriceSourceType
{
	///ǰ�ɽ���
	PSRC_LAST_PRICE = 48,
	///��ί�м�
	PSRC_BUY = 49,
	///��ί�м�
	PSRC_SELL = 50
};

///TFtdcActionFlagType��һ��������־����
enum ActionFlagType
{
	///ɾ��
	AF_Delete = 48,
	///�޸�
	AF_Modify = 51
};

enum HedgeFlagType {
	SPECULATION = 49,
	ARBITRAGE = 50,
	HEDGE = 51
};
