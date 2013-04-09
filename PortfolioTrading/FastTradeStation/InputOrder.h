#pragma once

#include "entity/trade.pb.h"
#include "ThostTraderApi/ThostFtdcTraderApi.h"

#include <string>

using namespace std;

class CInputOrder
{
public:
	CInputOrder(void);
	~CInputOrder(void);

	const string& Symbol(){ return string(m_innerOrderItem.InstrumentID); }

	void set_brokerid(const string& brokerId);
	///Ͷ���ߴ���
	void set_investorid(const string& investorId);
	///��Լ����
	void set_symbol(const string& symbol);
	///��������
	void set_orderref(const string& orderRef);
	///�û�����
	void set_userid(const string& userId){}
	///�����۸�����: �޼�
	void set_orderpricetype(trade::OrderPriceTypeType ordPriceType);
	///��������: 
	void set_direction(trade::TradeDirectionType tradeDirection);
	///��Ͽ�ƽ��־: ����
	void set_comboffsetflag(const string& offsetFlag);
	///���Ͷ���ױ���־
	void set_combhedgeflag(const string& hedgeFlag);
	///�۸�
	void set_limitprice(double lmtPx);
	///����: 1
	void set_volumetotaloriginal(int volTotal);
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

private:

	CThostFtdcInputOrderField m_innerOrderItem;
};



