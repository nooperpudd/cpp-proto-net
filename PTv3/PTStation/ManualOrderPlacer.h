#pragma once
#include "PortfolioTrendOrderPlacer.h"
#include "BufferRunner.h"


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
	
	void AsyncRun(entity::PosiDirectionType posiDirection, double lmtPx,
		boost::chrono::steady_clock::time_point trigQuoteTimestamp)
	{
		m_threadWorker.QueueWorkItem(boost::bind(&CDualScalperOrderPlacer::Run2, this,
			posiDirection, lmtPx, trigQuoteTimestamp));
	}

	void Cleanup();

protected:
	void Run2(entity::PosiDirectionType posiDirection, double lmtPx,
		boost::chrono::steady_clock::time_point trigQuoteTimestamp)
	{
		double lmtPrice[2] = { lmtPx, 0.0 };
		Run(posiDirection, lmtPrice, 2, trigQuoteTimestamp);
	}

	virtual CLegOrderPlacer* CreateLegOrderPlacer(int openTimeout, int maxRetryTimes);
	trade::SubmitReason SubmitReason(){ return trade::SR_Scalpe; }
	int GetQuantity(){ return 1; }	// DualScalper strategy only support 1 so far
private:
	string m_userId;
	CThreadWorker m_threadWorker;
};
