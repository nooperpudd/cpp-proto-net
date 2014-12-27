#pragma once
using namespace System;

namespace PTEntity {
	
	public enum class TradeDirectionType : int 
	{ 
		BUY = 48, 
		SELL = 49
	};

	public enum class ServerType : int
	{
		SERV_QUOTE = 0,
		SERV_TRADE = 1
	};

	public enum class PosiDirectionType : int
	{
		NET = 49,
		LONG = 50,
		SHORT = 51
	};

	public enum class LegStatus : int
	{
		UNOPENED = 0,
		IS_OPENING = 1,
		OPENED = 2,
		IS_CLOSING = 3,
		CLOSED = 4
	};

	public enum class StrategyType : int
	{
		ARBITRAGE = 0,
		CHANGE_POSITION = 1,
		SCALPER = 2,
		HIST_SLOPE = 3,
		WMA_TREND = 4,
		LINER_REGRESSION = 5,
		ASC_TREND = 6,
		RANGE_TREND = 7,
		MANUAL = 8,
		QUICK_SCORE = 9,
		TREND_REVERT = 10
	};

	public enum class DirectionDepends : int
	{
		IGNORE_THIS = 0,
		ON_SMALL_SIZE = 1,
		ON_BIG_SIZE = 2,
		ON_SMALL_CHANGE = 3,
		ON_BIG_CHANGE = 4
	};

	public enum class StopPriceType : int
	{
		LOSS_STOP = 0,
		TRAILING_STOP = 1
	};

	public enum class StopLossCloseMethods : int
	{
		BASED_ON_NEXT_QUOTE = 0,
		BASED_ON_INPUT_LIMIT = 1
	};

	public enum class CompareCondition : int
	{
		GREATER_THAN = 0,
		GREATER_EQUAL_THAN = 1,
		LESS_THAN = 2,
		LESS_EQUAL_THAN = 3
	};

	public enum class PosiOffsetFlag : int
	{
		OPEN = 0,
		CLOSE = 1
	};

	public enum class SubmitReason : int
	{
		SR_Manual = 0,
		SR_AutoOpen,
		SR_StopGain,
		SR_StopLoss,
		SR_AutoSwitch,
		SR_Scalpe,
		SR_Trend
	};

	public enum class MlOrderOffset : int
	{
		ML_OF_OPEN = 0,
		ML_OF_CLOSE,
		ML_OF_OTHER
	};

	//TFtdcHedgeFlagType��һ��Ͷ���ױ���־����
	public enum class HedgeFlagType : int 
	{
		SPECULATION = 49,
		ARBITRAGE = 50,
		HEDGE = 51
	};

	public enum class SlopeDirection : int
	{
		NO_DIRECTION = 0,
		GOING_UP = 1,
		GOING_DOWN = 2
	};

	//TFtdcOrderPriceTypeType��һ�������۸���������
	public enum class OrderPriceTypeType : int
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

	//TFtdcTimeConditionType��һ����Ч����������
	public enum class TimeConditionType : int
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

	//TFtdcVolumeConditionType��һ���ɽ�����������
	public enum class VolumeConditionType : int
	{
		///�κ�����
		VC_AV = 49,
		///��С����
		VC_MV = 50,
		///ȫ������
		VC_CV = 51
	};

	//TFtdcContingentConditionType��һ��������������
	public enum class ContingentConditionType : int
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

	//TFtdcForceCloseReasonType��һ��ǿƽԭ������
	public enum class ForceCloseReasonType : int
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

	//TFtdcOrderSubmitStatusType��һ�������ύ״̬����
	public enum class OrderSubmitStatusType : int
	{
		///δ�ύ
		NOT_SUBMITTED = 47,
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

	//TFtdcOrderSourceType��һ��������Դ����
	public enum class OrderSourceType : int
	{
		///���Բ�����
		PARTICIPANT = 48,
		///���Թ���Ա
		ADMINISTRATOR = 49
	};

	//TFtdcOrderStatusType��һ������״̬����
	public enum class OrderStatusType : int
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

	//TFtdcOrderTypeType��һ��������������
	public enum class OrderTypeType : int
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

	//TFtdcTradingRoleType��һ�����׽�ɫ����
	public enum class TradingRoleType : int
	{
		///����
		ER_BROKER = 49,
		///��Ӫ
		ER_HOST = 50,
		///������
		ER_MAKER = 51
	};

	//TFtdcOffsetFlagType��һ����ƽ��־����
	public enum class OffsetFlagType : int
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

	//TFtdcTradeTypeType��һ���ɽ���������
	public enum class TradeTypeType : int
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

	//TFtdcPriceSourceType��һ���ɽ�����Դ����
	public enum class PriceSourceType : int
	{
		///ǰ�ɽ���
		PSRC_LAST_PRICE = 48,
		///��ί�м�
		PSRC_BUY = 49,
		///��ί�м�
		PSRC_SELL = 50
	};

	//TFtdcActionFlagType��һ��������־����
	public enum class ActionFlagType : int
	{
		///ɾ��
		AF_Delete = 48,
		///�޸�
		AF_Modify = 51
	};
}