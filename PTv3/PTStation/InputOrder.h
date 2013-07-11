#pragma once

#include "entity/trade.pb.h"
#include "ThostTraderApi/ThostFtdcTraderApi.h"

class CInputOrder
{
public:
	CInputOrder(void);
	~CInputOrder(void);

	const string& Symbol(){ return m_symbol; }

	void set_brokerid(const string& brokerId);
	///Ͷ���ߴ���
	void set_investorid(const string& investorId);
	///��Լ����
	void set_symbol(const string& symbol);
	///��������
	void set_orderref(const string& orderRef);
	const string& OrderRef(){ return m_orderRef; }
	///�û�����
	void set_userid(const string& userId){}
	///�����۸�����: �޼�
	void set_orderpricetype(trade::OrderPriceTypeType ordPriceType);
	///��������: 
	void set_direction(trade::TradeDirectionType tradeDirection);
	trade::TradeDirectionType Direction(){ return m_direction; }
	///��Ͽ�ƽ��־: ����
	void set_comboffsetflag(const string& offsetFlag);
	const string& OffsetFlag(){ return m_offsetFlag; }
	///���Ͷ���ױ���־
	void set_combhedgeflag(const string& hedgeFlag);
	///�۸�
	void set_limitprice(double lmtPx);
	double LimitPrice(){ return m_innerOrderItem.LimitPrice; }
	///����: 1
	void set_volumetotaloriginal(int volTotal);
	int VolumeTotalOriginal(){ return m_innerOrderItem.VolumeTotalOriginal; }
	///��Ч������: ������Ч
	void set_timecondition(trade::TimeConditionType timeConditionType);
	///GTD����
	void set_gtddate(const string& gtdDate){}
	///�ɽ�������: �κ�����
	void set_volumecondition(trade::VolumeConditionType volConditionType);
	///��С�ɽ���: 1
	void set_minvolume(int minVol);
	///��������: ����
	void set_contingentcondition(trade::ContingentConditionType contingentCond);
	///ֹ���
	void set_stopprice(double stopPx){}
	///ǿƽԭ��: ��ǿƽ
	void set_forceclosereason(trade::ForceCloseReasonType forceCloseReason);
	///�Զ������־: ��
	void set_isautosuspend(bool isAutoSuspend);
	///ҵ��Ԫ
	void set_businessunit(const string& busiUnit){}
	///������
	void set_requestid(int reqId);
	///�û�ǿ����־: ��
	void set_userforceclose(bool userForceClose);

	CThostFtdcInputOrderField& InnerOrder(){ return m_innerOrderItem; }

private:
	string m_symbol;
	string m_orderRef;
	string m_offsetFlag;
	trade::TradeDirectionType m_direction;

	CThostFtdcInputOrderField m_innerOrderItem;
};



