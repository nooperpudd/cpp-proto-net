#pragma once
#include "PortfolioTrendOrderPlacer.h"


class CManualOrderPlacer :
	public CPortfolioTrendOrderPlacer
{
public:
	CManualOrderPlacer();
	virtual ~CManualOrderPlacer();

	virtual bool Prepare();

	virtual void Cleanup(){}

	virtual bool IfPortfolioCanceled(){ return false; }

protected:

	virtual CLegOrderPlacer* CreateLegOrderPlacer(int openTimeout, int maxRetryTimes);
};


class CDualScalperOrderPlacer : public CPortfolioTrendOrderPlacer
{
public:
	CDualScalperOrderPlacer(){}
	~CDualScalperOrderPlacer(){}

	void CloseOrder(double closePrice, bool cancelOnTimeout);

	void SetUserId(const string& userId){ m_userId = userId; }
	const string& UserId(){ return m_userId; }

protected:
	virtual CLegOrderPlacer* CreateLegOrderPlacer(int openTimeout, int maxRetryTimes);
	trade::SubmitReason SubmitReason(){ return trade::SR_Scalpe; }

private:
	string m_userId;
};
