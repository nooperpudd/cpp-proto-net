#pragma once

#include "Strategy.h"

class CIcebergStrategy : public CStrategy
{
public:
	CIcebergStrategy();
	~CIcebergStrategy();

	virtual void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);

protected:
	CPortfolioOrderPlacer* CreateOrderPlacer();

	bool GetPriceOffset(entity::PosiDirectionType direction, double* askOffset, double* bidOffset);

	// Settings
	double m_priceTick;
	double m_pxDiffThreshold;
	int m_sizeDiffThreshold;
	double m_targetGainPercent;

	// Update properties
	double m_priceGap;
	int m_sizeGap;

	double m_ask;
	int m_askSize;
	double m_bid;
	int m_bidSize;
};

