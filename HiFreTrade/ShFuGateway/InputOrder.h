#pragma once

#include "TradeEnumDefinition.h"

#include <string>

using namespace std;

class CInputOrder
{
public:
	CInputOrder(void);
	~CInputOrder(void);

	///���͹�˾����
	const string& broderid(){ return m_brokerId; }
	void set_brokerid(const string& brokerId){ m_brokerId = brokerId; }

	///Ͷ���ߴ���
	const string& investorid() {return m_investorId; }
	void set_investorid(const string& investorId) { m_investorId = investorId; }

	///��Լ����
	const string& instrumentid() { return m_instrumentId; }
	void set_instrumentid(const string& instrumentId) { m_instrumentId = instrumentId; }

	///��������
	const string& orderref() { return m_orderRef; }
	void set_orderref(const string& ordRef){ m_orderRef = ordRef; }

	///�û�����
	const string& userid() {return m_userId;}
	void set_userid(const string& userId) {m_userId = userId;}

	///�����۸�����
	OrderPriceTypeType orderpricetype(){return m_orderPriceType;}
	void set_orderpricetype(OrderPriceTypeType orderPriceType){m_orderPriceType = orderPriceType;}

	///��������
	TradeDirectionType direction(){return m_direction;}
	void set_direction(TradeDirectionType tradeDirection){m_direction = tradeDirection;}

	///��Ͽ�ƽ��־
	const string& comboffsetflag() {return m_combOffsetFlag;}
	void set_comboffsetflag(const string& combOffsetFlag) {m_combOffsetFlag = combOffsetFlag;}

	///���Ͷ���ױ���־
	const string& combhedgeflag() {return m_combHedgeFlag;}
	void set_combhedgeflag(const string& combHedgeFlag) {m_combHedgeFlag = combHedgeFlag;}
	
	///�۸�
	double limitprice() {return m_limitPrice;}
	void set_limitprice(double limitPrice){m_limitPrice = limitPrice;}

	///����
	int volumetotaloriginal(){return m_volumeTotalOriginal;}
	void set_volumetotaloriginal(int volTotalOrig){m_volumeTotalOriginal = volTotalOrig;}

	///��Ч������
	TimeConditionType timecondition(){return m_timeCondition;}
	void set_timecondition(TimeConditionType timeCond){m_timeCondition = timeCond;}

	///GTD����
	const string& gtddate(){return m_gtdDate;}
	void set_gtddate(const string& gtdDate){m_gtdDate = gtdDate;}

	///�ɽ�������
	VolumeConditionType volumecondition(){return m_volumeCondition;}
	void set_volumecondition(VolumeConditionType volCond){m_volumeCondition = volCond;}

	///��С�ɽ���
	int minvolume(){return m_minVolume;}
	void set_minvolume(int minVol){m_minVolume = minVol;}

	///��������
	ContingentConditionType contingentcondition(){return m_contingentCondition;}
	void set_contingentcondition(ContingentConditionType triggerCond){m_contingentCondition = triggerCond;}

	///ֹ���
	double stopprice(){return m_stopPrice;}
	void set_stopprice(double stopPx){m_stopPrice = stopPx;}

	///ǿƽԭ��
	ForceCloseReasonType forceclosereason(){return m_forceCloseReason;}
	void set_forceclosereason(ForceCloseReasonType closeReason){m_forceCloseReason = closeReason;}

	///�Զ������־
	bool isautosuspend(){return m_isAutoSuspend;}
	void set_isautosuspend(bool autoSuspend){m_isAutoSuspend = autoSuspend;}

	///ҵ��Ԫ
	const string& businessunit(){return m_businessUnit;}
	void set_businessunit(const string& buszUnit){m_businessUnit = buszUnit;}

	///������
	int requestid(){return m_requestId;}
	void set_requestid(int reqId){m_requestId = reqId;}

	///�û�ǿ����־
	bool userforceclose(){return m_userForceClose;}
	void set_userforceclose(bool userForceClose){m_userForceClose = userForceClose;}

private:
	string m_brokerId;
	string m_investorId;
	string m_instrumentId;
	string m_orderRef;
	string m_userId;
	OrderPriceTypeType m_orderPriceType;
	TradeDirectionType m_direction;
	string m_combOffsetFlag;
	string m_combHedgeFlag;
	double m_limitPrice;
	int m_volumeTotalOriginal;
	TimeConditionType m_timeCondition;
	string m_gtdDate;
	VolumeConditionType m_volumeCondition;
	int m_minVolume;
	ContingentConditionType m_contingentCondition;
	double m_stopPrice;
	ForceCloseReasonType m_forceCloseReason;
	bool m_isAutoSuspend;
	string m_businessUnit;
	int m_requestId;
	bool m_userForceClose;
};

