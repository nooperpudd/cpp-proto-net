#pragma once

#include "Strategy.h"
#include "boost/unordered_map.hpp"

class CDualScalperOrderPlacer;

typedef boost::function<void(const string&, COrderProcessor**)> OnBindingRouteHandler;

class CMultiRouteStrategy : public CStrategy
{
public:
	CMultiRouteStrategy(){}
	virtual ~CMultiRouteStrategy(){}

	void BindRoutes(CPortfolio* pPortfolio, OnBindingRouteHandler onBindingRouteHandler);
	void AddUserId(const string& userId){ m_userIds.push_back(userId); }
	CPortfolioOrderPlacer* GetRoute(const string& userId);

protected:
	virtual void OnBindedRoutes(){}

private:

	vector<string> m_userIds;

	boost::unordered_map<string, OrderPlacerPtr> m_orderPlacerMap;
};

enum DualScalperState
{
	DUAL_SCALPER_BOTH_EMPTY,
	DUAL_SCALPER_HOLD_LONG,
	DUAL_SCALPER_HOLD_SHORT,
	DUAL_SCALPER_BOTH_HELD,
	DUAL_SCALPER_ERROR,
};

class CDualScalperStrategy : public CMultiRouteStrategy
{
public:
	CDualScalperStrategy();
	~CDualScalperStrategy();

	virtual void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);

	virtual void Stop();

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc){}

	virtual void OnLegFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume);
	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);

	DualScalperState State(){ return m_currentState.load(boost::memory_order_acquire); }
	void SetState(DualScalperState state){ m_currentState.store(state, boost::memory_order_release); }

protected:

	void OnBindedRoutes();
	CPortfolioOrderPlacer* CreateOrderPlacer();

	virtual bool OnStart();
	virtual void OnStop();

private:
	void OpenPosition(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void ClosePosition(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void LongStopLoss(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void ShortStopLoss(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);


	double m_diffThreshold;
	double m_priceTick;
	double m_openOffset;
	double m_closeOffset;
	double m_oppositeCloseThreshold;
	string m_longSideUserId;
	string m_shortSideUserId;

	CDualScalperOrderPlacer* m_longOrderPlacer;
	CDualScalperOrderPlacer* m_shortOrderPlacer;

	double m_diff;
	double m_ask;
	int m_askSize;
	double m_bid;
	int m_bidSize;

	boost::shared_ptr<void> m_fsm;
	boost::atomic<DualScalperState> m_currentState;
	boost::mutex m_mutFsm;

	bool m_stopping;
};

