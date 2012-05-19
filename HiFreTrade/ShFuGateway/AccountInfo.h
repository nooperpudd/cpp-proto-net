#pragma once

#include <string>

using namespace std;

class CAccountInfo
{
public:
	CAccountInfo(void);
	~CAccountInfo(void);

	///���͹�˾����
	const std::string brokerid() const { return m_brokerId; }
	void set_brokerid(std::string val) { m_brokerId = val; }

	///Ͷ�����ʺ�
	std::string accountid() const { return m_accountid; }
	void set_accountid(std::string val) { m_accountid = val; }

	///�ϴ���Ѻ���
	double premortgage() const { return m_premortgage; }
	void set_premortgage(double val) { m_premortgage = val; }

	///�ϴ����ö��
	double precredit() const { return m_precredit; }
	void set_precredit(double val) { m_precredit = val; }

	///�ϴδ���
	double predeposit() const { return m_predeposit; }
	void set_predeposit(double val) { m_predeposit = val; }

	///�ϴν���׼����
	double prebalance() const { return m_prebalance; }
	void set_prebalance(double val) { m_prebalance = val; }

	///�ϴ�ռ�õı�֤��
	double premargin() const { return m_premargin; }
	void set_premargin(double val) { m_premargin = val; }

	///��Ϣ����
	double interestbase() const { return m_interestbase; }
	void set_interestbase(double val) { m_interestbase = val; }

	///��Ϣ����
	double interest() const { return m_interest; }
	void set_interest(double val) { m_interest = val; }

	///�����
	double deposit() const { return m_deposit; }
	void set_deposit(double val) { m_deposit = val; }

	///������
	double withdraw() const { return m_withdraw; }
	void set_withdraw(double val) { m_withdraw = val; }

	///����ı�֤��
	double frozenmargin() const { return m_frozenmargin; }
	void set_frozenmargin(double val) { m_frozenmargin = val; }

	///������ʽ�
	double frozencash() const { return m_frozencash; }
	void set_frozencash(double val) { m_frozencash = val; }

	///�����������
	double frozencommission() const { return m_frozencommission; }
	void set_frozencommission(double val) { m_frozencommission = val; }

	///��ǰ��֤���ܶ�
	double currmargin() const { return m_currmargin; }
	void set_currmargin(double val) { m_currmargin = val; }

	///�ʽ���
	double cashin() const { return m_cashin; }
	void set_cashin(double val) { m_cashin = val; }

	///������
	double commission() const { return m_commission; }
	void set_commission(double val) { m_commission = val; }

	///ƽ��ӯ��
	double closeprofit() const { return m_closeprofit; }
	void set_closeprofit(double val) { m_closeprofit = val; }

	///�ֲ�ӯ��
	double positionprofit() const { return m_positionprofit; }
	void set_positionprofit(double val) { m_positionprofit = val; }

	///�ڻ�����׼����
	double balance() const { return m_balance; }
	void set_balance(double val) { m_balance = val; }
	
	///�����ʽ�
	double available() const { return m_available; }
	void set_available(double val) { m_available = val; }

	///��ȡ�ʽ�
	double withdrawquota() const { return m_withdrawquota; }
	void set_withdrawquota(double val) { m_withdrawquota = val; }

	///����׼����
	double reserve() const { return m_reserve; }
	void set_reserve(double val) { m_reserve = val; }

	///������
	const std::string tradingday() const { return m_tradingday; }
	void set_tradingday(std::string val) { m_tradingday = val; }

	///������
	int settlementid() const { return m_settlementid; }
	void set_settlementid(int val) { m_settlementid = val; }

	///���ö��
	double credit() const { return m_credit; }
	void set_credit(double val) { m_credit = val; }

	///��Ѻ���
	double mortgage() const { return m_mortgage; }
	void set_mortgage(double val) { m_mortgage = val; }

	///��������֤��
	double exchangemargin() const { return m_exchangemargin; }
	void set_exchangemargin(double val) { m_exchangemargin = val; }

	///Ͷ���߽��֤��
	double deliverymargin() const { return m_deliverymargin; }
	void set_deliverymargin(double val) { m_deliverymargin = val; }

	///���������֤��
	double exchangedeliverymargin() const { return m_exchangedeliverymargin; }
	void set_exchangedeliverymargin(double val) { m_exchangedeliverymargin = val; }

private:
	string m_brokerId;
	string m_accountid;
	double m_premortgage;
	double	m_precredit;
	double	m_predeposit;
	double	m_prebalance;
	double	m_premargin;
	double	m_interestbase;
	double	m_interest;
	double	m_deposit;
	double	m_withdraw;
	double	m_frozenmargin;
	double	m_frozencash;
	double	m_frozencommission;
	double	m_currmargin;
	double	m_cashin;
	double	m_commission;
	double	m_closeprofit;
	double	m_positionprofit;
	double	m_balance;
	double	m_available;
	double	m_withdrawquota;
	double	m_reserve;
	string	m_tradingday;
	int	m_settlementid;
	double	m_credit;
	double	m_mortgage;
	double	m_exchangemargin;
	double	m_deliverymargin;
	double	m_exchangedeliverymargin;
};

