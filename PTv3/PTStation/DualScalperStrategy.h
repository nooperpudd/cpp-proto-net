#pragma once

#include "Strategy.h"
#include "boost/unordered_map.hpp"

typedef boost::function<void(const string&, COrderProcessor**)> OnBindingRouteHandler;

class CMultiRouteStrategy : public CStrategy
{
public:
	CMultiRouteStrategy(){}
	virtual ~CMultiRouteStrategy(){}

	void BindRoutes(CPortfolio* pPortfolio, OnBindingRouteHandler onBindingRouteHandler);

private:

	vector<string> m_userIds;

	boost::unordered_map<string, OrderPlacerPtr> m_orderPlacerMap;
};

class CDualScalperStrategy : public CMultiRouteStrategy
{
public:
	CDualScalperStrategy();
	~CDualScalperStrategy();

	virtual void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc){}

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);

protected:
	CPortfolioOrderPlacer* CreateOrderPlacer();

private:

	double m_diffThreshold;

};

