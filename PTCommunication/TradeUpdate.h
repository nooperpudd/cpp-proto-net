#pragma once

#include "Enums.h"
#include "entity/trade.pb.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class TradeUpdate
{
public:
	TradeUpdate(const trade::Trade* pEntity);

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
	///����������
	property String ^ ExchangeID;
	///�ɽ����
	property String ^ TradeID;
	///��������
	property TradeDirectionType	Direction;
	///�������
	property String ^ OrderSysID;
	///��Ա����
	property String ^ ParticipantID;
	///�ͻ�����
	property String ^ ClientID;
	///���׽�ɫ
	property TradingRoleType	TradingRole;
	///��Լ�ڽ������Ĵ���
	property String ^ ExchangeInstID;
	///��ƽ��־
	property OffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	property HedgeFlagType	HedgeFlag;
	///�۸�
	property double	Price;
	///����
	property int	Volume;
	///�ɽ�ʱ��
	property String ^ TradeDate;
	///�ɽ�ʱ��
	property String ^ TradeTime;
	///�ɽ�����
	property TradeTypeType	TradeType;
	///�ɽ�����Դ
	property PriceSourceType	PriceSource;
	///����������Ա����
	property String ^ TraderID;
	///���ر������
	property String ^ OrderLocalID;
	///�����Ա���
	property String ^ ClearingPartID;
	///ҵ��Ԫ
	property String ^ BusinessUnit;
	///���
	property int	SequenceNo;
	///������
	property String ^ TradingDay;
	///������
	property int	SettlementID;
	///���͹�˾�������
	property int	BrokerOrderSeq;
};

}

