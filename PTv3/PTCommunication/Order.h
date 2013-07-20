#pragma once

#include "Enums.h"
#include "entity/trade.pb.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class Order
{
public:
	Order(const trade::Order* pEntityOrd);

	///���͹�˾����
	property String ^ BrokerID;
	///Ͷ���ߴ���
	property String ^ InvestorID;
	///��Լ����
	property String ^ InstrumentID;
	///��������
	property String ^ OrderRef;
	///�û�����
	property String ^ UserID;
	///�����۸�����
	property OrderPriceTypeType	OrderPriceType;
	///��������
	property TradeDirectionType	Direction;
	///��Ͽ�ƽ��־
	property String ^ CombOffsetFlag;
	///���Ͷ���ױ���־
	property String ^ CombHedgeFlag;
	///�۸�
	property double	LimitPrice;
	///����
	property int	VolumeTotalOriginal;
	///��Ч������
	property TimeConditionType	TimeCondition;
	///GTD����
	property String ^ GTDDate;
	///�ɽ�������
	property VolumeConditionType	VolumeCondition;
	///��С�ɽ���
	property int	MinVolume;
	///��������
	property ContingentConditionType	ContingentCondition;
	///ֹ���
	property double	StopPrice;
	///ǿƽԭ��
	property ForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	property bool	IsAutoSuspend;
	///ҵ��Ԫ
	property String ^ BusinessUnit;
	///������
	property int	RequestID;
	// Above fields are same as InputOrder's
	////////////////////////////////////////////////////////////////////

	///���ر������
	property String ^ OrderLocalID;
	///����������
	property String ^ ExchangeID;
	///��Ա����
	property String ^ ParticipantID;
	///�ͻ�����
	property String ^ ClientID;
	///��Լ�ڽ������Ĵ���
	property String ^ ExchangeInstID;
	///����������Ա����
	property String ^ TraderID;
	///��װ���
	property int	InstallID;
	///�����ύ״̬
	property OrderSubmitStatusType	OrderSubmitStatus;
	///������ʾ���
	property int	NotifySequence;
	///������
	property String ^ TradingDay;
	///������
	property int	SettlementID;
	///�������
	property String ^ OrderSysID;
	///������Դ
	property OrderSourceType	OrderSource;
	///����״̬
	property OrderStatusType	OrderStatus;
	///��������
	property OrderTypeType	OrderType;
	///��ɽ�����
	property int	VolumeTraded;
	///ʣ������
	property int	VolumeTotal;
	///��������
	property String ^ InsertDate;
	///ί��ʱ��
	property String ^ InsertTime;
	///����ʱ��
	property String ^ ActiveTime;
	///����ʱ��
	property String ^ SuspendTime;
	///����޸�ʱ��
	property String ^ UpdateTime;
	///����ʱ��
	property String ^ CancelTime;
	///����޸Ľ���������Ա����
	property String ^ ActiveTraderID;
	///�����Ա���
	property String ^ ClearingPartID;
	///���
	property int	SequenceNo;
	///ǰ�ñ��
	property int	FrontID;
	///�Ự���
	property int	SessionID;
	///�û��˲�Ʒ��Ϣ
	property String ^ UserProductInfo;
	///״̬��Ϣ
	property String ^ StatusMsg;
	///�û�ǿ����־
	property bool	UserForceClose;
	///�����û�����
	property String ^ ActiveUserID;
	///���͹�˾�������
	property int	BrokerOrderSeq;
	///��ر���
	property String ^ RelativeOrderSysID;

	///���ȳɽ�
	property bool Preferred;
};

}

