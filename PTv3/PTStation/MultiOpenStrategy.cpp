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

	StrategyContext& context = GetContext();

	if (!Prerequisite(pQuote, pPortfolio, context, timestamp))
		return;

	CalculateContext(pQuote, pPortfolio, timestamp);

	// 1. Feed quote to strategy executors that is opening/closing position
	TestWorkingExecutors(pQuote, &context, timestamp);

	BeforeTestForTrade(pQuote, pPortfolio, context);

	// 2. Feed quote to active strategy executor to check for opening position
	if (IsRunning() && !OutOfTradingWindow(context.CurrentIndex))
	{
		TestForOpen(pQuote, pPortfolio, &context, timestamp);
	}

	// 3. Feed quote to strategy executors that are opened and check for closing position
	TestForClose(pQuote, pPortfolio, &context, timestamp);
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
		(iter->second)->OnWorking(pQuote, pContext, timestamp);
	}
}

void CMultiOpenStrategy::TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	if (m_activeExecutor != NULL)
	{
		bool open = m_activeExecutor->TestForOpen(pQuote, pPortfolio, pContext, timestamp);
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

void CMultiOpenStrategy::TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	for (boost::unordered_map<int, CStrategyExecutor*>::iterator iter = m_OpenedExecutors.begin(); iter != m_OpenedExecutors.end();)
	{
		CStrategyExecutor* closingExecutor = iter->second;
		bool close = closingExecutor->TestForClose(pQuote, pPortfolio, pContext, timestamp);
		if (close)
		{
			// move active executor to working pool
			int execId = closingExecutor->ExecId();
			LOG_DEBUG(logger, boost::str(boost::format("Moving executor(%d) to working pool due to Pending Close") % execId));
			m_workingExecutors.insert(std::make_pair(execId, closingExecutor));
			iter = m_OpenedExecutors.erase(iter);
		}
		else
			++iter;
	}
}

void CMultiOpenStrategy::OnPortfolioTraded(int execId, entity::PosiOffsetFlag offsetFlag, int volumeTraded)
{
	boost::mutex::scoped_lock l(m_mut);
	boost::unordered_map<int, CStrategyExecutor*>::iterator iterFound = m_workingExecutors.find(execId);
	if (iterFound != m_workingExecutors.end())
	{
		CStrategyExecutor* pExecutor = iterFound->second;

		m_workingExecutors.erase(iterFound);

		if (offsetFlag == entity::OPEN)
		{
			ExecutorState execState = pExecutor->State();
			assert(execState == PENDING_OPEN);
			m_OpenedExecutors.insert(std::make_pair(execId, pExecutor));
		}
		else if (offsetFlag == entity::CLOSE)
		{
			ExecutorState execState = pExecutor->State();
			assert(execState == PENDING_CLOSE);
			m_executorsPool.push(pExecutor);
		}
		pExecutor->OnFilled(volumeTraded);
	}
}

bool CMultiOpenStrategy::Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context, boost::chrono::steady_clock::time_point& timestamp)
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
		GetReadyExecutor(&m_activeExecutor);
	}
}

void CMultiOpenStrategy::InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc)
{
	for (vector<StrategyExecutorPtr>::iterator iter = m_strategyExecutors.begin();
		iter != m_strategyExecutors.end(); ++iter)
	{
		(*iter)->InitOrderPlacer(pPortf, pOrderProc, 
			boost::bind(&CMultiOpenStrategy::OnPortfolioTraded, this, _1, _2, _3));
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


