#pragma once

#include "Strategy.h"
#include "boost/unordered_map.hpp"

typedef boost::function<void(const string&, COrderProcessor**)> OnBindingRouteHandler;

class CMultiRouteStrategy : public CStrategy
{
public:
	CMultiRouteStrategy() {}
	virtual ~CMultiRouteStrategy() {}

	void BindRoutes(CPortfolio* pPortfolio, OnBindingRouteHandler onBindingRouteHandler);
	void AddUserId(const string& userId) { m_userIds.push_back(userId); }
	CPortfolioOrderPlacer* GetRoute(const string& userId);

protected:
	virtual void OnBindedRoutes() {}

private:

	vector<string> m_userIds;

	boost::unordered_map<string, OrderPlacerPtr> m_orderPlacerMap;
};