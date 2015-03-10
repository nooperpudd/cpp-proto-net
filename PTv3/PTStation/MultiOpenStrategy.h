#pragma once

#include "TechAnalyStrategy.h"
#include "StrategyExecutor.h"

#include <queue>
#include <boost/unordered_map.hpp>

class CMultiOpenStrategy : public CTechAnalyStrategy
{
public:
	CMultiOpenStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CMultiOpenStrategy();

	void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual StrategyContext* CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp) = 0;
	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);
	virtual int OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder, int actualTradedVol);

	void TestWorkingExecutors(entity::Quote* pQuote, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	void TestForOpen(entity::Quote* pQuote, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	void TestForClose(entity::Quote* pQuote, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);

protected:

	virtual StrategyExecutorPtr CreateExecutor(int execId, int quantity) = 0;
	virtual void OnApply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers){}
	virtual bool Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

	void InitializeExecutors();
	bool GetReadyExecutor(CStrategyExecutor** pOutExector);
	void OnPortfolioTraded(int execId, entity::PosiOffsetFlag offsetFlag);

	int m_perOpenQuantity;
	int m_maxQuantity;

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

