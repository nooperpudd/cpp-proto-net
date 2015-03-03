#include "StdAfx.h"
#include "MultiOpenStrategy.h"
#include "Portfolio.h"


CMultiOpenStrategy::CMultiOpenStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_activeExecutor(NULL)
{
	Apply(strategyItem, pPortfolio, false);
}

CMultiOpenStrategy::~CMultiOpenStrategy()
{
}

void CMultiOpenStrategy::Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Test(pQuote, pPortfolio, timestamp);

	if (!Prerequisite(pQuote, pPortfolio, timestamp))
		return;

	StrategyContext* pContext = CalculateContext(pQuote, pPortfolio, timestamp);

	// 1. Feed quote to strategy executors that is opening/closing position
	TestWorkingExecutors(pQuote, pContext);
	// 2. Feed quote to active strategy executor to check for opening position
	TestForOpen(pQuote, pContext);
	// 3. Feed quote to strategy executors that are opened and check for closing position
	TestForClose(pQuote, pContext);
}

void CMultiOpenStrategy::Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers)
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);

	OnApply(strategyItem, pPortfolio, withTriggers);

	m_maxQuantity = strategyItem.maxposition();
	m_perOpenQuantity = pPortfolio->Quantity();

	if (m_perOpenQuantity > 0 && m_maxQuantity >= m_perOpenQuantity)
		InitializeExecutors();
}

void CMultiOpenStrategy::TestWorkingExecutors(entity::Quote* pQuote, StrategyContext* pContext)
{
	boost::mutex::scoped_lock l(m_mutWorking);
	for (vector<CStrategyExecutor*>::iterator iter = m_workingExecutors.begin(); iter != m_workingExecutors.end(); ++iter)
	{
		(*iter)->OnWorking(pQuote, pContext);
	}
}

void CMultiOpenStrategy::TestForOpen(entity::Quote* pQuote, StrategyContext* pContext)
{
	boost::mutex::scoped_lock l(m_mutActive);
	if (m_activeExecutor != NULL)
	{
		m_activeExecutor->OnTestForOpen(pQuote, pContext);
	}
}

void CMultiOpenStrategy::TestForClose(entity::Quote* pQuote, StrategyContext* pContext)
{
	boost::mutex::scoped_lock l(m_mutOpened);
	for (vector<CStrategyExecutor*>::iterator iter = m_OpenedExecutors.begin(); iter != m_OpenedExecutors.end(); ++iter)
	{
		(*iter)->OnTestForClose(pQuote, pContext);
	}
}

bool CMultiOpenStrategy::Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	return IsMarketOpen(pQuote);
}

void CMultiOpenStrategy::InitializeExecutors()
{
	int remainingQty = m_maxQuantity;
	while (remainingQty > 0)
	{
		int execQty = remainingQty >= m_perOpenQuantity ? m_perOpenQuantity : remainingQty;
		StrategyExecutorPtr executorPtr = CreateExecutor(execQty);
		m_strategyExecutors.push_back(executorPtr);
		remainingQty -= m_perOpenQuantity;
	}
}

