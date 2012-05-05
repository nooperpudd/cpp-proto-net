#pragma once

#include <string>

using namespace std;

class CInputOrderAction
{
public:
	CInputOrderAction(void);
	~CInputOrderAction(void);

	///���͹�˾����
	const string& broderid(){ return m_brokerId; }
	void set_brokerid(const string& brokerId){ m_brokerId = brokerId; }

	///Ͷ���ߴ���
	const string& investorid() {return m_investorId; }
	void set_investorid(const string& investorId) { m_investorId = investorId; }

	///������������
	int orderactionref(){return m_orderActionRef;}
	void set_orderactionref(int ordActionRef){m_orderActionRef = ordActionRef;}

	///��������
	const string& orderref() { return m_orderRef; }
	void set_orderref(const string& ordRef){ m_orderRef = ordRef; }

	///������
	int requestid(){return m_requestId;}
	void set_requestid(int reqId){m_requestId = reqId;}

	///ǰ�ñ��
	int frontid(){return m_frontId;}
	void set_frontid(int frontId){m_frontId = frontId;}

	///�Ự���
	int sessionid(){return m_sessionId;}
	void set_sessionid(int sessionId){m_sessionId = sessionId;}

	///����������
	const string& exchangeid(){return m_exchangeId;}
	void set_exchangeid(const string& exchId){m_exchangeId = exchId;}

	///�������
	const string& ordersysid(){return m_orderSysId;}
	void set_ordersysid(const string& ordSysId){m_orderSysId = ordSysId;}

	///������־
	ActionFlagType actionflag(){return m_actionFlag;}
	void set_actionflag(ActionFlagType flagType){m_actionFlag = flagType;}

	///�۸�
	double limitprice(){return m_limitPrice;}
	void set_limitprice(double limitPrice){m_limitPrice = limitPrice;}

	///�����仯
	int volumechange(){return m_volumeChange;}
	void set_volumechange(int volChange){m_volumeChange = volChange;}

	///�û�����
	const string& userid() {return m_userId;}
	void set_userid(const string& userId) {m_userId = userId;}

	///��Լ����
	const string& instrumentid() { return m_instrumentId; }
	void set_instrumentid(const string& instrumentId) { m_instrumentId = instrumentId; }


private:

	string m_brokerId;
	string m_investorId;
	int m_orderActionRef;
	string m_orderRef;

	int m_requestId;
	int m_frontId;
	int m_sessionId;

	string m_exchangeId;
	string m_orderSysId;

	ActionFlagType m_actionFlag;
	double m_limitPrice;
	int m_volumeChange;
	string m_userId;
	string m_instrumentId;
};

