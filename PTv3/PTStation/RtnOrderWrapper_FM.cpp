#include "StdAfx.h"
#include "RtnOrderWrapper_FM.h"
#include "charsetconvert.h"


CRtnOrderWrapper::CRtnOrderWrapper(CUstpFtdcOrderField* pOrder)
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
	pOrd->set_orderref(m_orderField.UserOrderLocalID);
	///�û�����
	pOrd->set_userid(m_orderField.UserID);
	///�����۸�����
	pOrd->set_orderpricetype(static_cast	<trade::OrderPriceTypeType>(m_orderField.OrderPriceType));
	///��������
	pOrd->set_direction(static_cast<trade::TradeDirectionType>(m_orderField.Direction));
	string a;
	
	char tmp[2] = { 0, 0 };
	///��ƽ��־
	tmp[0] = m_orderField.OffsetFlag;
	pOrd->set_comboffsetflag(tmp);
	///Ͷ���ױ���־
	tmp[0] = m_orderField.HedgeFlag;
	pOrd->set_combhedgeflag(tmp);
	///�۸�
	pOrd->set_limitprice(m_orderField.LimitPrice);
	///����
	pOrd->set_volumetotaloriginal(m_orderField.Volume);
	///��Ч������
	pOrd->set_timecondition(static_cast<trade::TimeConditionType>(m_orderField.TimeCondition));
	///GTD����
	pOrd->set_gtddate(m_orderField.GTDDate);
	///�ɽ�������
	pOrd->set_volumecondition(static_cast<trade::VolumeConditionType>(m_orderField.VolumeCondition));
	///��С�ɽ���
	pOrd->set_minvolume(m_orderField.MinVolume);
	///��������
	pOrd->set_contingentcondition(trade::IMMEDIATELY);
	///ֹ���
	pOrd->set_stopprice(m_orderField.StopPrice);
	///ǿƽԭ��
	pOrd->set_forceclosereason(static_cast<trade::ForceCloseReasonType>(m_orderField.ForceCloseReason));
	///�Զ������־
	pOrd->set_isautosuspend(m_orderField.IsAutoSuspend != 0);
	///ҵ��Ԫ
	pOrd->set_businessunit(m_orderField.BusinessUnit);
	///������
	pOrd->set_requestid(0);
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
	pOrd->set_exchangeinstid("");
	///����������Ա����
	pOrd->set_traderid("");
	///��װ���
	pOrd->set_installid(0);
	///�����ύ״̬
	pOrd->set_ordersubmitstatus(OrderSubmitStatus());
	///������ʾ���
	pOrd->set_notifysequence(0);
	///������
	pOrd->set_tradingday(m_orderField.TradingDay);
	///������
	pOrd->set_settlementid(0);
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
	pOrd->set_ordertype(trade::NORMAL_ORDER);
	///��ɽ�����
	pOrd->set_volumetraded(m_orderField.VolumeTraded);
	///ʣ������
	pOrd->set_volumetotal(m_orderField.VolumeRemain);
	///��������
	pOrd->set_insertdate("");
	///ί��ʱ��
	pOrd->set_inserttime(m_orderField.InsertTime);
	///����ʱ��
	pOrd->set_activetime("");
	///����ʱ��
	pOrd->set_suspendtime("");
	///����޸�ʱ��
	pOrd->set_updatetime("");
	///����ʱ��
	pOrd->set_canceltime(m_orderField.CancelTime);
	///����޸Ľ���������Ա����
	pOrd->set_activetraderid("");
	///�����Ա���
	pOrd->set_clearingpartid("");
	///���
	pOrd->set_sequenceno(0);
	///ǰ�ñ��
	pOrd->set_frontid(0);
	///�Ự���
	pOrd->set_sessionid(0);
	///�û��˲�Ʒ��Ϣ
	pOrd->set_userproductinfo("");
	///״̬��Ϣ
	pOrd->set_statusmsg("");
	///�û�ǿ����־
	pOrd->set_userforceclose(false);
	///�����û�����
	pOrd->set_activeuserid("");
	///���͹�˾�������
	pOrd->set_brokerorderseq(0);
	///��ر���
	pOrd->set_relativeordersysid("");
}
