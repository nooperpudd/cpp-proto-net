#include "StdAfx.h"
#include "MultiOpenStrategy.h"


CMultiOpenStrategy::CMultiOpenStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio)
	: CTechAnalyStrategy(strategyItem, pAvatar)
{
}


CMultiOpenStrategy::~CMultiOpenStrategy()
{
}

void CMultiOpenStrategy::Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	CTechAnalyStrategy::Test(pQuote, pPortfolio, timestamp);

	// 1. Feed quote to strategy executors that is opening/closing position

	// 2. Feed quote to active strategy executor to check for opening position

	// 3. Feed quote to strategy executors that are opened and check for closing position
}

void CMultiOpenStrategy::TestWorkingExecutors(entity::Quote* pQuote)
{
	boost::mutex::scoped_lock l(m_mutWorking);
	for (vector<CStrategyExecutor*>::iterator iter = m_workingExecutors.begin(); iter != m_workingExecutors.end(); ++iter)
	{
		(*iter)->OnWorking(pQuote);
	}
}
