#pragma once

#include "TechAnalyStrategy.h"
#include "StrategyExecutor.h"

#include <stack>

class CMultiOpenStrategy : public CTechAnalyStrategy
{
public:
	CMultiOpenStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CMultiOpenStrategy();

	void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp){}

	void TestWorkingExecutors(entity::Quote* pQuote);
	void TestForOpen(entity::Quote* pQuote){}
	void TestForClose(entity::Quote* pQuote){}

protected:

	virtual void OnApply(const entity::StrategyItem& strategyItem, bool withTriggers){}

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
	CStrategyExecutor* m_activeExecutors;


	boost::mutex m_mut;

};

