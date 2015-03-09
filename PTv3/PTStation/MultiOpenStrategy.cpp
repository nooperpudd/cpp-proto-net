#include "StdAfx.h"
#include "MultiOpenStrategy.h"
#include "Portfolio.h"
#include "globalmembers.h"


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
	TestWorkingExecutors(pQuote, pContext, timestamp);
	// 2. Feed quote to active strategy executor to check for opening position
	TestForOpen(pQuote, pContext, timestamp);
	// 3. Feed quote to strategy executors that are opened and check for closing position
	TestForClose(pQuote, pContext, timestamp);
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

void CMultiOpenStrategy::TestWorkingExecutors(entity::Quote* pQuote, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	for (boost::unordered_map<int, CStrategyExecutor*>::iterator iter = m_workingExecutors.begin(); iter != m_workingExecutors.end(); ++iter)
	{
		(iter->second)->OnWorking(pQuote, pContext);
	}
}

void CMultiOpenStrategy::TestForOpen(entity::Quote* pQuote, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	boost::mutex::scoped_lock l(m_mutActive);
	if (m_activeExecutor != NULL)
	{
		bool open = m_activeExecutor->TestForOpen(pQuote, pContext);
		if (open)
		{
			// move active executor to working pool
			int execId = m_activeExecutor->ExecId();
			LOG_DEBUG(logger, boost::str(boost::format("Moving executor(%d) to working pool due to Pending Open") % execId));
			m_workingExecutors.insert(std::make_pair(execId, m_activeExecutor));

			// get next ready executor;
			bool succ = GetReadyExecutor(&m_activeExecutor);
			if (succ)
			{
				execId = m_activeExecutor != NULL ? m_activeExecutor->ExecId() : -1;
				LOG_DEBUG(logger, boost::str(boost::format("Successfully Get next ready executor(%d)") % execId));
			}
			else
			{
				LOG_DEBUG(logger, "There is NO ready executor so far");
			}
		}
	}
}

void CMultiOpenStrategy::TestForClose(entity::Quote* pQuote, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	boost::mutex::scoped_lock l(m_mutOpened);
	for (boost::unordered_map<int, CStrategyExecutor*>::iterator iter = m_OpenedExecutors.begin(); iter != m_OpenedExecutors.end(); ++iter)
	{
		CStrategyExecutor* closingExecutor = iter->second;
		bool close = closingExecutor->TestForClose(pQuote, pContext);
		if (close)
		{
			// move active executor to working pool
			int execId = closingExecutor->ExecId();
			LOG_DEBUG(logger, boost::str(boost::format("Moving executor(%d) to working pool due to Pending Close") % execId));
			m_workingExecutors.insert(std::make_pair(execId, closingExecutor));
		}
	}
}

bool CMultiOpenStrategy::Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	return IsMarketOpen(pQuote);
}

void CMultiOpenStrategy::InitializeExecutors()
{
	int remainingQty = m_maxQuantity;
	int execId = 1;
	while (remainingQty > 0)
	{
		int execQty = remainingQty >= m_perOpenQuantity ? m_perOpenQuantity : remainingQty;
		StrategyExecutorPtr executorPtr = CreateExecutor(execId++, execQty);
		m_strategyExecutors.push_back(executorPtr);
		m_executorsPool.push(executorPtr.get());
		remainingQty -= execQty;
	}

	if (m_strategyExecutors.size() > 0)
	{
		boost::mutex::scoped_lock l(m_mutActive);
		GetReadyExecutor(&m_activeExecutor);
	}
}

void CMultiOpenStrategy::InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc)
{
	for (vector<StrategyExecutorPtr>::iterator iter = m_strategyExecutors.begin();
		iter != m_strategyExecutors.end(); ++iter)
	{
		(*iter)->InitOrderPlacer(pPortf, pOrderProc);
	}
}

bool CMultiOpenStrategy::GetReadyExecutor(CStrategyExecutor** pOutExector)
{
	bool hasElem = !m_executorsPool.empty();
	if (hasElem)
	{
		*pOutExector = m_executorsPool.front();
		(*pOutExector)->Start();
		m_executorsPool.pop();
	}
	else
		*pOutExector = NULL;
	return hasElem;
}

int CMultiOpenStrategy::OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol)
{
	return 0;
}

int CMultiOpenStrategy::OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder, int actualTradedVol)
{
	return 0;
}

