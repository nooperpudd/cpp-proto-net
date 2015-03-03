#pragma once

#include "TechAnalyStrategy.h"
#include "StrategyExecutor.h"

#include <stack>

class CMultiOpenStrategy : public CTechAnalyStrategy
{
public:
	CMultiOpenStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CMultiOpenStrategy();

	void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual StrategyContext* CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp) = 0;

	void TestWorkingExecutors(entity::Quote* pQuote, StrategyContext* pContext);
	void TestForOpen(entity::Quote* pQuote, StrategyContext* pContext);
	void TestForClose(entity::Quote* pQuote, StrategyContext* pContext);

protected:

	virtual StrategyExecutorPtr CreateExecutor(int quantity) = 0;
	virtual void OnApply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers){}
	virtual bool Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

	void InitializeExecutors();

	int m_perOpenQuantity;
	int m_maxQuantity;

	vector<StrategyExecutorPtr> m_strategyExecutors;
	
	stack<CStrategyExecutor*> m_executorsPool;
	boost::mutex m_mutPool;

	// store the executors that are opening or closing position
	vector<CStrategyExecutor*> m_workingExecutors;
	boost::mutex m_mutWorking;

	// store the executors that are testing for closing poistion
	vector<CStrategyExecutor*> m_OpenedExecutors;
	boost::mutex m_mutOpened;

	// the current executor that is waiting for opening position
	CStrategyExecutor* m_activeExecutor;
	boost::mutex m_mutActive;

	boost::mutex m_mut;

};

