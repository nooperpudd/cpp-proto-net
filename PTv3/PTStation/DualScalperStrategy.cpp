#include "StdAfx.h"
#include "DualScalperStrategy.h"
#include "PortfolioScalperOrderPlacer.h"
#include "globalmembers.h"
#include "Portfolio.h"

CDualScalperStrategy::CDualScalperStrategy()
{
}


CDualScalperStrategy::~CDualScalperStrategy()
{
}

void CDualScalperStrategy::Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers)
{
	if (withTriggers)
	{
		logger.Debug("Applying Dual ScalperStrategy...");
	}

	CStrategy::Apply(strategyItem, withTriggers);

	m_priceTick = strategyItem.ds_pricetick();
	m_diffThreshold = strategyItem.ds_diffthreshold();
	m_longSideUserId = strategyItem.ds_longsideuserid();
	m_shortSideUserId = strategyItem.ds_shortsideuserid();
}

void CDualScalperStrategy::Apply(const entity::StrategyItem& strategyItem, bool withTriggers)
{
	Apply(strategyItem, false);
}

void CDualScalperStrategy::Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{

}

void CDualScalperStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem)
{

}

int CDualScalperStrategy::OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol)
{
	return 0;
}

CPortfolioOrderPlacer* CDualScalperStrategy::CreateOrderPlacer()
{
	return new CPortfolioScalperOrderPlacer;
}

void CMultiRouteStrategy::BindRoutes(CPortfolio* pPortfolio, OnBindingRouteHandler onBindingRouteHandler)
{
	for (vector<string>::iterator iter = m_userIds.begin(); iter != m_userIds.end(); ++iter)
	{
		const string& userId = (*iter);
		CPortfolioOrderPlacer* orderPlacer = CreateOrderPlacer();
		m_orderPlacerMap.insert(std::make_pair(userId, OrderPlacerPtr(orderPlacer)));
		COrderProcessor* pOrderProcessor = NULL;
		if (!onBindingRouteHandler.empty())
			onBindingRouteHandler(userId, &pOrderProcessor);
		orderPlacer->Initialize(pPortfolio, pOrderProcessor);

		if (pOrderProcessor == NULL)	// Must something wrong
		{
			logger.Error(boost::str(boost::format("Portfolio(%s) CANNOT bind OrderProcessor for userid(%s)") 
				% pPortfolio->ID() % userId ));
		}
	}
}
