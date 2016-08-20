#include "stdafx.h"
#include "RtnOrderWrapper.h"
#include "charsetconvert.h"


CRtnOrderWrapper::CRtnOrderWrapper(CThostFtdcOrderField* pOrder)
{
	memcpy(&m_orderField, pOrder, sizeof(m_orderField));
	m_orderTimestamp = boost::chrono::steady_clock::now();
}


CRtnOrderWrapper::~CRtnOrderWrapper(void)
{
}

void CRtnOrderWrapper::ToEntity( trade::Order* pOrd )
{
	///���͹�˾����
	pOrd->set_brokerid(m_orderField.BrokerID);
	///Ͷ���ߴ���
	pOrd->set_investorid(m_orderField.InvestorID);
	///��Լ����
	pOrd->set_instrumentid(m_orderField.InstrumentID);
	///��������
	pOrd->set_orderref(m_orderField.OrderRef);
	///�û�����
	pOrd->set_userid(m_orderField.UserID);
	///�����۸�����
	pOrd->set_orderpricetype(static_cast	<trade::OrderPriceTypeType>(m_orderField.OrderPriceType));
	///��������
	pOrd->set_direction(static_cast<trade::TradeDirectionType>(m_orderField.Direction));
	///��Ͽ�ƽ��־
	pOrd->set_comboffsetflag(m_orderField.CombOffsetFlag);
	///���Ͷ���ױ���־
	pOrd->set_combhedgeflag(m_orderField.CombHedgeFlag);
	///�۸�
	pOrd->set_limitprice(m_orderField.LimitPrice);
	///����
	pOrd->set_volumetotaloriginal(m_orderField.VolumeTotalOriginal);
	///��Ч������
	pOrd->set_timecondition(static_cast<trade::TimeConditionType>(m_orderField.TimeCondition));
	///GTD����
	pOrd->set_gtddate(m_orderField.GTDDate);
	///�ɽ�������
	pOrd->set_volumecondition(static_cast<trade::VolumeConditionType>(m_orderField.VolumeCondition));
	///��С�ɽ���
	pOrd->set_minvolume(m_orderField.MinVolume);
	///��������
	pOrd->set_contingentcondition(static_cast<trade::ContingentConditionType>(m_orderField.ContingentCondition));
	///ֹ���
	pOrd->set_stopprice(m_orderField.StopPrice);
	///ǿƽԭ��
	pOrd->set_forceclosereason(static_cast<trade::ForceCloseReasonType>(m_orderField.ForceCloseReason));
	///�Զ������־
	pOrd->set_isautosuspend(m_orderField.IsAutoSuspend != 0);
	///ҵ��Ԫ
	pOrd->set_businessunit(m_orderField.BusinessUnit);
	///������
	pOrd->set_requestid(m_orderField.RequestID);
	// Above fields are same as InputOrder's
	////////////////////////////////////////////////////////////////////

	///���ر������
	pOrd->set_orderlocalid(m_orderField.OrderLocalID);
	///����������
	pOrd->set_exchangeid(m_orderField.ExchangeID);
	///��Ա����
	pOrd->set_participantid(m_orderField.ParticipantID);
	///�ͻ�����
	pOrd->set_clientid(m_orderField.ClientID);
	///��Լ�ڽ������Ĵ���
	pOrd->set_exchangeinstid(m_orderField.ExchangeInstID);
	///����������Ա����
	pOrd->set_traderid(m_orderField.TraderID);
	///��װ���
	pOrd->set_installid(m_orderField.InstallID);
	///�����ύ״̬
	pOrd->set_ordersubmitstatus(static_cast<trade::OrderSubmitStatusType>(m_orderField.OrderSubmitStatus));
	///������ʾ���
	pOrd->set_notifysequence(m_orderField.NotifySequence);
	///������
	pOrd->set_tradingday(m_orderField.TradingDay);
	///������
	pOrd->set_settlementid(m_orderField.SettlementID);
	///�������
	pOrd->set_ordersysid(m_orderField.OrderSysID);
	///������Դ
	// sometimes OrderSource could be 0 insteade of '0'
	if(m_orderField.OrderSource < trade::PARTICIPANT)
		pOrd->set_ordersource(trade::PARTICIPANT);
	else
		pOrd->set_ordersource(static_cast<trade::OrderSourceType>(m_orderField.OrderSource));
	///����״̬
	pOrd->set_orderstatus(static_cast<trade::OrderStatusType>(m_orderField.OrderStatus));
	///��������
	// Sometimes OrderType could be 0 instead of '0'
	if(m_orderField.OrderType < trade::NORMAL_ORDER)
		pOrd->set_ordertype(trade::NORMAL_ORDER);
	else
		pOrd->set_ordertype(static_cast<trade::OrderTypeType>(m_orderField.OrderType));
	///��ɽ�����
	pOrd->set_volumetraded(m_orderField.VolumeTraded);
	///ʣ������
	pOrd->set_volumetotal(m_orderField.VolumeTotal);
	///��������
	pOrd->set_insertdate(m_orderField.InsertDate);
	///ί��ʱ��
	pOrd->set_inserttime(m_orderField.InsertTime);
	///����ʱ��
	pOrd->set_activetime(m_orderField.ActiveTime);
	///����ʱ��
	pOrd->set_suspendtime(m_orderField.SuspendTime);
	///����޸�ʱ��
	pOrd->set_updatetime(m_orderField.UpdateTime);
	///����ʱ��
	pOrd->set_canceltime(m_orderField.CancelTime);
	///����޸Ľ���������Ա����
	pOrd->set_activetraderid(m_orderField.ActiveTraderID);
	///�����Ա���
	pOrd->set_clearingpartid(m_orderField.ClearingPartID);
	///���
	pOrd->set_sequenceno(m_orderField.SequenceNo);
	///ǰ�ñ��
	pOrd->set_frontid(m_orderField.FrontID);
	///�Ự���
	pOrd->set_sessionid(m_orderField.SessionID);
	///�û��˲�Ʒ��Ϣ
	pOrd->set_userproductinfo(m_orderField.UserProductInfo);
	///״̬��Ϣ
	string ordStatusMsg;
	GB2312ToUTF_8(ordStatusMsg, m_orderField.StatusMsg);
	pOrd->set_statusmsg(ordStatusMsg);
	///�û�ǿ����־
	pOrd->set_userforceclose(m_orderField.UserForceClose != 0);
	///�����û�����
	pOrd->set_activeuserid(m_orderField.ActiveUserID);
	///���͹�˾�������
	pOrd->set_brokerorderseq(m_orderField.BrokerOrderSeq);
	///��ر���
	pOrd->set_relativeordersysid(m_orderField.RelativeOrderSysID);
}

trade::OffsetFlagType CRtnOrderWrapper::Offset()
{
	if (strlen(m_orderField.CombOffsetFlag) > 0)
		return static_cast<trade::OffsetFlagType>(m_orderField.CombOffsetFlag[0]);

	return trade::OF_OPEN;
}
