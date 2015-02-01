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
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Test(pQuote, pPortfolio, timestamp);

	if (!IsMarketOpen(pQuote))
		return;

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

void CMultiOpenStrategy::Apply(const entity::StrategyItem& strategyItem, bool withTriggers)
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);

	OnApply(strategyItem, withTriggers);

	// make sure following parameters having values
	if (m_openTimeout == 0)
		m_openTimeout = 350;
	if (m_retryTimes == 0)
		m_retryTimes = 8;

}

