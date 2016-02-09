#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioScalperOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioScalperOrderPlacer(void);
	virtual ~CPortfolioScalperOrderPlacer(void);

	virtual void BuildTemplateOrder();

	virtual void OnAddingLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer);
};

class CPortfolioQueueOrderPlacer : public CPortfolioScalperOrderPlacer
{
public:
	CPortfolioQueueOrderPlacer(): m_openPrice(0), m_closePrice(0)
	{}
	~CPortfolioQueueOrderPlacer(){}

	void QueueOrder(entity::PosiDirectionType posiDirection, double openPx, double closePx,
		boost::chrono::steady_clock::time_point trigQuoteTimestamp)
	{
		m_openPrice = openPx;
		m_closePrice = closePx;
		double lmtPrice[2] = { openPx, closePx };
		Run(posiDirection, lmtPrice, 2, trigQuoteTimestamp);
	}

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

