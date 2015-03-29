#pragma once

#include "TechAnalyStrategy.h"
#include "StrategyExecutor.h"

#include <queue>
#include <boost/unordered_map.hpp>

class CMultiOpenStrategy : public CTechAnalyStrategy
{
public:
	CMultiOpenStrategy(CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CMultiOpenStrategy();

	void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp) = 0;
	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);
	virtual int OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder, int actualTradedVol);

	void TestWorkingExecutors(entity::Quote* pQuote, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	void TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	void TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);

protected:

	CPortfolioOrderPlacer* CreateOrderPlacer();
	void OnStart();

	virtual StrategyContext& GetContext() = 0;
	virtual StrategyExecutorPtr CreateExecutor(int execId, int quantity) = 0;
	virtual void BeforeTestForTrade(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context){}
	virtual void OnApply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers){}
	virtual bool Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context, boost::chrono::steady_clock::time_point& timestamp);

	void InitializeExecutors();
	bool GetReadyExecutor(CStrategyExecutor** pOutExector);
	void OnPortfolioTraded(int execId, entity::PosiOffsetFlag offsetFlag, int volumeTraded);

	static double CalcMlOrderCost(const trade::MultiLegOrder& openOrder);
	static entity::PosiDirectionType GetMlOrderDirection(const trade::MultiLegOrder& mlOrder);

	int m_perOpenQuantity;
	int m_maxQuantity;

	int m_longPosition;
	double m_longAvgCost;
	int m_shortPosition;
	double m_shortAvgCost;

	vector<StrategyExecutorPtr> m_strategyExecutors;
	
	queue<CStrategyExecutor*> m_executorsPool;
	
	// store the executors that are opening or closing position
	boost::unordered_map<int, CStrategyExecutor*> m_workingExecutors;
	
	// store the executors that are testing for closing poistion
	boost::unordered_map<int, CStrategyExecutor*> m_OpenedExecutors;
	
	// the current executor that is waiting for opening position
	CStrategyExecutor* m_activeExecutor;
	
	boost::mutex m_mut;

};

