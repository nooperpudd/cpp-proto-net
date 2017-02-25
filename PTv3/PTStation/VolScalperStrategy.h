#pragma once

#include "Strategy.h"

#include <boost/unordered_map.hpp>
#include <queue>

class CPortfolioTrendOrderPlacer;
class CStrategyTrigger;

class CVolScalperStrategy : public CStrategy
{
public:
	CVolScalperStrategy();
	~CVolScalperStrategy();

	double PriceTick() { return m_priceTick; }

	virtual void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	void BeforeTest(entity::Quote* pQuote);
	void AfterTest(entity::Quote* pQuote);
	void UpdateLeg(entity::Quote* pQuote, CPortfolio* pPortfolio);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);

	void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc);
	virtual void Stop();
	virtual void AlreadyStarted();

	friend class CSpringV51Trigger;
protected:
	CPortfolioOrderPlacer* CreateOrderPlacer();

	virtual bool OnStart();
	virtual void OnStop();

private:
	void CreateTriggers(const entity::StrategyItem& strategyItem){}
	void OnOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded, entity::PosiDirectionType direction);

	CPortfolioTrendOrderPlacer* GetReadyOrderPlacer();
	void ResetLevelOrderPlacer(int execId);
	void HandleOpenedPosition(boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote);
	void OpenPosition(CPortfolioTrendOrderPlacer* lvlOrdPlacer, entity::PosiDirectionType direction, boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote, const string& triggerComment) const;
	void ClosePosition(CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const string& triggerComment) const;

	double m_priceTick;
	int m_lot;
	bool m_stopping;

	boost::mutex m_mutSettings;
	
	double m_lastTurnover;
	int m_lastVolume;
	double m_lastAsk;
	double m_lastBid;

	double m_ask;
	int m_askSize;
	double m_bid;
	int m_bidSize;

	// **************
	// For Spring V5-1
	double m_vwapLast;
	double m_vwapNow;
	double m_speedLast;
	double m_speedNow;
	double m_enterPrice;
	double m_openThreshold;
	double m_closeThreshold;
	double m_lossThreshold;
	int m_pxOffset;
	int m_maxPosition;
	// **************

	int m_actTurnover;
	int m_actVolume;
	int m_buyVolume;
	int m_sellVolume;

	int m_gainCount;
	int m_lossCount;

	typedef boost::shared_ptr<CPortfolioTrendOrderPlacer> TrendOrderPlacerPtr;

	boost::unordered_map<int, TrendOrderPlacerPtr> m_levelOrderPlacers;
	typedef boost::unordered_map<int, TrendOrderPlacerPtr>::iterator LevelOrderPlacersIter;

	queue<int> m_readyQueue;
	boost::mutex m_mutReadyQueue;

	vector<TrendOrderPlacerPtr> m_openedOrdersVector;
	boost::mutex m_mutOpenedVector;

	boost::shared_ptr<CStrategyTrigger> m_trigger;
};

class CStrategyTrigger
{
public:
	CStrategyTrigger(CVolScalperStrategy* strategy): m_strategy(strategy)
	{}
	virtual ~CStrategyTrigger() {}

	virtual entity::PosiDirectionType TestForOpen(entity::Quote * pQuote, string& outComment) = 0;
	virtual bool TestForClose(entity::Quote * pQuote, double entryPrice, entity::PosiDirectionType direction, string& outComment) = 0;

protected:
	CVolScalperStrategy* m_strategy;
};

class CSpringV51Trigger : public CStrategyTrigger
{
public:
	CSpringV51Trigger(CVolScalperStrategy* strategy): CStrategyTrigger(strategy){}
	
	entity::PosiDirectionType TestForOpen(entity::Quote* pQuote, string& outComment);
	bool TestForClose(entity::Quote* pQuote, double entryPrice, entity::PosiDirectionType direction, string& outComment);
};


