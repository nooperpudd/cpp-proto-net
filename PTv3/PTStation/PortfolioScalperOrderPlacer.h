#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioScalperOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioScalperOrderPlacer(void);
	virtual ~CPortfolioScalperOrderPlacer(void);

	virtual trade::SubmitReason SubmitReason() { return trade::SR_Scalpe; }
	virtual void BuildTemplateOrder();

	virtual void OnAddingLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer);
};

class CPortfolioQueueOrderPlacer : public CPortfolioScalperOrderPlacer
{
public:
	CPortfolioQueueOrderPlacer(int execId): m_openPrice(0), m_closePrice(0)
	{
		SetExecId(execId);
	}
	~CPortfolioQueueOrderPlacer(){}

	void QueueOrder(entity::PosiDirectionType posiDirection, double openPx, double closePx,
	                boost::chrono::steady_clock::time_point trigQuoteTimestamp);

	bool IsOpening();
	bool IsClosing();
	void CancelPendingOrder();
	void CancelPendingAndClosePosition(entity::Quote* pQuote);
	virtual trade::SubmitReason SubmitReason() { return trade::SR_Queuing; }
	virtual void OnLegOrderFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume, int execId);
	virtual void OnLegOrderCanceled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, int execId);

	double GetOpenPrice() const
	{ return m_openPrice; }
	double GetClosePrice() const
	{ return m_closePrice; }

protected:

	virtual CLegOrderPlacer* CreateLegOrderPlacer(int openTimeout, int maxRetryTimes);

private:
	double m_openPrice;
	double m_closePrice;
};

