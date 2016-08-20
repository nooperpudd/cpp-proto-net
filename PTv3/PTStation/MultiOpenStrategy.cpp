#include "stdafx.h"
#include "MultiOpenStrategy.h"
#include "Portfolio.h"
#include "globalmembers.h"


CMultiOpenStrategy::CMultiOpenStrategy(CAvatarClient* pAvatar, CPortfolio* pPortfolio)
	: CTechAnalyStrategy(pAvatar)
	, m_activeExecutor(NULL)
	, m_pPortfolio(pPortfolio)
	, m_perOpenQuantity(1)
	, m_maxQuantity(3)
	, m_longPosition(0)
	, m_longAvgCost(0)
	, m_shortPosition(0)
	, m_shortAvgCost(0)
	, m_closeYesterday(false)
	, m_reInitProc(NULL)
{
	m_portfTradedEvtPump.Init(boost::bind(&CMultiOpenStrategy::HandlePortfolioDone, this, _1));
}

CMultiOpenStrategy::~CMultiOpenStrategy()
{
	m_pPortfolio = NULL;
	m_activeExecutor = NULL;
}

void CMultiOpenStrategy::Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Test(pQuote, pPortfolio, timestamp);

	StrategyContext& context = GetContext();

	// 1. Feed quote to strategy executors that is opening/closing position
	TestWorkingExecutors(pQuote, timestamp);

	if (!Prerequisite(pQuote, pPortfolio, context, timestamp))
		return;

	CalculateContext(pQuote, pPortfolio, timestamp);

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
	if (strategyItem.has_closeyesterday())
	{
		m_closeYesterday = strategyItem.closeyesterday();
		LOG_INFO(logger, boost::str(boost::format("CMultiOpenStrategy setting CloseYesterday %s") 
			% (m_closeYesterday ? "Yes" : "No")));
	}

	m_perOpenQuantity = pPortfolio->Quantity();
}

int CMultiOpenStrategy::GetCurrentPreparedQuantity()
{
	int currentMaxQuantity = 0;
	for (vector<StrategyExecutorPtr>::iterator iter = m_strategyExecutors.begin();
		iter != m_strategyExecutors.end(); ++iter)
	{
		currentMaxQuantity += (*iter)->Quantity();
	}

	return currentMaxQuantity;
}

bool CMultiOpenStrategy::PrepareExecutors()
{
	if (m_perOpenQuantity > 0
		&& m_maxQuantity >= m_perOpenQuantity)
	{
		int currentQty = GetCurrentPreparedQuantity();
		if (m_maxQuantity != currentQty) // Current quantity is not different than set m_maxQuantity
		{
			int idleExecutors = m_activeExecutor != NULL ? m_executorsPool.size() + 1 : m_executorsPool.size();
			idleExecutors += m_errorExecutors.size();

			if (m_strategyExecutors.size() == idleExecutors)
			{
				// Only when all executors are idle or there is no executor at all 
				InitializeExecutors();

				// Must call InitOrderPlacer before use
				InitOrderPlacer(m_pPortfolio, GetEffectiveProcessor());

				return true;
			}
			else{
				logger.Warning(boost::str(boost::format("Portfolio(%s) cannot prepare executors due to possiblity of unfinished executor existing. Total Idle = %d")
					% m_pPortfolio->ID() % idleExecutors));
			}
		}
		else
		{
			// Desired max quantity is already met by existing executors
			return true;
		}
	}

	return false;
}

void CMultiOpenStrategy::TestWorkingExecutors(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	for (boost::unordered_map<int, CStrategyExecutor*>::iterator iter = m_workingExecutors.begin(); iter != m_workingExecutors.end(); ++iter)
	{
		(iter->second)->OnWorking(pQuote, timestamp);
	}
}

void CMultiOpenStrategy::TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	if (m_activeExecutor == NULL)
	{
		bool succ = GetReadyExecutor(&m_activeExecutor);
		if (succ)
		{
			int execId = m_activeExecutor != NULL ? m_activeExecutor->ExecId() : -1;
			LOG_DEBUG(logger, boost::str(boost::format("Portfolio(%s) Successfully Get next ready executor(%d)") % pPortfolio->ID() % execId));
		}
		else
		{
			LOG_DEBUG(logger, boost::str(boost::format("Portfolio(%s) already ran out of executors") % pPortfolio->ID()));
		}
	}

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
				logger.Warning("There is NO ready executor so far");
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
			break; // Just trigger one orderplacer to close position in one round of test close.
		}
		else
			++iter;
	}
}

void CMultiOpenStrategy::HandlePortfolioDone(PortfolioDoneMsgPtr msgPtr)
{
	int execId = msgPtr->ExecId();
	PortfolioFinishState doneState = msgPtr->DoneState();
	entity::PosiOffsetFlag offsetFlag = msgPtr->OffsetFlag();
	int volumeTraded = msgPtr->VolumeTraded();

	boost::mutex::scoped_lock l(m_mut);
	
	boost::unordered_map<int, CStrategyExecutor*>::iterator iterFound = m_workingExecutors.find(execId);
	if (iterFound != m_workingExecutors.end())
	{
		CStrategyExecutor* pExecutor = iterFound->second;
		LOG_DEBUG(logger, boost::str(boost::format("Remove Executor(%d) from working list") % pExecutor->ExecId()));
		m_workingExecutors.erase(iterFound);

		if (doneState == PortfolioFilled)
		{
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
				LOG_DEBUG(logger, boost::str(boost::format("Return Executor(%d) to executorPool") % pExecutor->ExecId()));
				m_executorsPool.push(pExecutor);
			}
			pExecutor->OnFilled(volumeTraded);
		}
		else if (doneState == PortfolioCanceled)
		{
			if (offsetFlag == entity::OPEN)
			{
				LOG_DEBUG(logger, boost::str(boost::format("OPEN Canceled and RETURN Executor(%d) to executorPool") % pExecutor->ExecId()));
				m_executorsPool.push(pExecutor);
			}
			else
			{
				LOG_DEBUG(logger, boost::str(boost::format("CLOSE Canceled and RETURN Executor(%d) to OpenedExecutors list") % pExecutor->ExecId()));
				m_OpenedExecutors.insert(std::make_pair(execId, pExecutor));
			}
			pExecutor->OnCanceled();
		}
		else // got error
		{
			if (offsetFlag == entity::OPEN)
			{
				logger.Warning(boost::str(boost::format("OPEN position and Got ERROR then RETURN Executor(%d) to executorPool") % pExecutor->ExecId()));
				m_executorsPool.push(pExecutor);
				pExecutor->OnCanceled();
			}
			else // error occurs when closing position
			{
				logger.Warning(boost::str(boost::format("CLOSE position and Got ERROR then put Executor(%d) to Error list") % pExecutor->ExecId()));
				PutErrorList(pExecutor);
				pExecutor->OnError();
			}
		}
	}
}

void CMultiOpenStrategy::OnPortfolioDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded)
{
	PortfolioDoneMsgPtr msg(new PortfolioDoneMsg(execId, doneState, offsetFlag, volumeTraded));
	m_portfTradedEvtPump.Enqueue(msg);
}

bool CMultiOpenStrategy::Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context, boost::chrono::steady_clock::time_point& timestamp)
{
	return IsMarketOpen(pQuote) && !IsSuspending();
}

void CMultiOpenStrategy::InitializeExecutors()
{
	m_errorExecutors.clear();
	m_activeExecutor = NULL;
	m_strategyExecutors.clear();
	
	int remainingQty = m_maxQuantity;
	int execId = 1;
	while (remainingQty > 0)
	{
		int execQty = remainingQty >= m_perOpenQuantity ? m_perOpenQuantity : remainingQty;
		StrategyExecutorPtr executorPtr = CreateExecutor(execId++, execQty);
		m_strategyExecutors.push_back(executorPtr);
		remainingQty -= execQty;
	}
}

void CMultiOpenStrategy::InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc)
{
	for (vector<StrategyExecutorPtr>::iterator iter = m_strategyExecutors.begin();
		iter != m_strategyExecutors.end(); ++iter)
	{
		(*iter)->InitOrderPlacer(pPortf, pOrderProc, 
			boost::bind(&CMultiOpenStrategy::OnPortfolioDone, this, _1, _2, _3, _4));
	}
}

COrderProcessor* CMultiOpenStrategy::GetEffectiveProcessor()
{
	return m_reInitProc != NULL ? m_reInitProc : m_pPortfolio->OrderProcessor();
}

void CMultiOpenStrategy::ReinitOrderPlacer(COrderProcessor* pOrderProc)
{
	m_reInitProc = pOrderProc;
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
	int qty = actualTradedVol;
	double cost = CalcMlOrderCost(openOrder);

	entity::PosiDirectionType posiDirection = GetMlOrderDirection(openOrder);
	if (posiDirection == entity::LONG)
	{
		int origQty = m_longPosition;
		double newAvgCost = (m_longAvgCost * origQty + cost * qty) / (origQty + qty);
		m_longAvgCost = newAvgCost;
		m_longPosition += qty;
	}
	else if (posiDirection == entity::SHORT)
	{
		int origQty = m_shortPosition;
		double newAvgCost = (m_shortAvgCost * origQty + cost * qty) / (origQty + qty);
		m_shortAvgCost = newAvgCost;
		m_shortPosition += qty;
	}

	return IncrementOpenTimes(pPortfolio, qty);
}

int CMultiOpenStrategy::OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder, int actualTradedVol)
{
	int qty = actualTradedVol;
	double cost = CalcMlOrderCost(closeOrder);
	LOG_DEBUG(logger, boost::str(boost::format("[%s] MultiOpen Strategy - Portfolio(%s) Removed position and Reset CostDiff")
		% pPortfolio->InvestorId() % pPortfolio->ID()));
	
	entity::PosiDirectionType posiDirection = GetMlOrderDirection(closeOrder);
	if (posiDirection == entity::LONG)
	{
		double orderProfit = (cost - m_longAvgCost) * qty;
		AddProfit(pPortfolio, orderProfit);

		int origQty = m_longPosition;
		int remaing = origQty - qty;
		if (remaing > 0)
		{
			double newAvgCost = (m_longAvgCost * origQty - cost * qty) / remaing;
			m_longAvgCost = newAvgCost;
		}
		else
		{
			m_longAvgCost = 0;
		}
		m_longPosition = remaing;
	}
	else if (posiDirection == entity::SHORT)
	{
		double orderProfit = (m_shortAvgCost - cost) * qty;
		AddProfit(pPortfolio, orderProfit);

		int origQty = m_shortPosition;
		int remaing = origQty - qty;
		if (remaing > 0)
		{
			double newAvgCost = (m_shortAvgCost * origQty - cost * qty) / remaing;
			m_shortAvgCost = newAvgCost;
		}
		else
		{
			m_shortAvgCost = 0;
		}
		m_shortPosition = remaing;
	}

	return IncrementCloseTimes(pPortfolio, qty);
}

double CMultiOpenStrategy::CalcMlOrderCost(const trade::MultiLegOrder& openOrder)
{
	double cost = 0;
	int legCount = openOrder.legs_size();
	if (legCount > 1)
	{
		const trade::Order& firstLeg = openOrder.legs(0);
		double firstLegPx = firstLeg.limitprice();

		const trade::Order& secondLeg = openOrder.legs(1);
		double secondLegPx = secondLeg.limitprice();

		cost = firstLegPx - secondLegPx;
	}

	return cost;
}

entity::PosiDirectionType CMultiOpenStrategy::GetMlOrderDirection(const trade::MultiLegOrder& mlOrder)
{
	if (mlOrder.offset() == trade::ML_OF_OPEN)
	{
		if (mlOrder.legs(0).direction() == trade::BUY)
			return entity::LONG;
		else
			return entity::SHORT;
	}
	else if (mlOrder.offset() == trade::ML_OF_CLOSE)
	{
		if (mlOrder.legs(0).direction() == trade::SELL)
			return entity::LONG;
		else
			return entity::SHORT;
	}
	else
		return entity::NET;
}

CPortfolioOrderPlacer* CMultiOpenStrategy::CreateOrderPlacer()
{
	// MultiOpenStrategy won't create a single order placer. Instead, it override InitOrderPlacer directly
	assert(false);
	return NULL;
}

bool CMultiOpenStrategy::OnStart()
{
	boost::mutex::scoped_lock l(m_mut);

	if (!PrepareExecutors())
		return false;

	// empty executor pool
	while (!m_executorsPool.empty())
		m_executorsPool.pop();

	bool allReady = true;
	for (vector<StrategyExecutorPtr>::iterator iter = m_strategyExecutors.begin();
		iter != m_strategyExecutors.end(); ++iter)
	{
		bool indReady = (*iter)->Prepare(m_closeYesterday);
		if (!indReady)
		{
			allReady = false;
			break;
		}
		m_executorsPool.push((*iter).get());
	}

	if (allReady)
	{
		// ensure all other containers empty
		m_workingExecutors.clear();
		m_errorExecutors.clear();
		m_OpenedExecutors.clear();


		if (m_strategyExecutors.size() > 0)
		{
			GetReadyExecutor(&m_activeExecutor);
		}
	}
	else
	{
		LOG_DEBUG(logger, "CMultiOpenStrategy preparing executors fail, Cleanup executors.");
		for (vector<StrategyExecutorPtr>::iterator iter = m_strategyExecutors.begin();
			iter != m_strategyExecutors.end(); ++iter)
		{
			(*iter)->Cleanup();
		}
	}
	
	return allReady;
}

void CMultiOpenStrategy::OnStop()
{
	LOG_DEBUG(logger, "CMultiOpenStrategy enter OnStop ...");
	boost::mutex::scoped_lock l(m_mut);

	if (m_activeExecutor != NULL)
	{
		m_executorsPool.push(m_activeExecutor);
		m_activeExecutor = NULL;
	}

	for (vector<StrategyExecutorPtr>::iterator iter = m_strategyExecutors.begin();
		iter != m_strategyExecutors.end(); ++iter)
	{
		(*iter)->Cleanup();
	}

	CleanupOnStop();
	LOG_DEBUG(logger, "CMultiOpenStrategy exit OnStop ...");
}

void CMultiOpenStrategy::PutErrorList(CStrategyExecutor* pErrExecutor)
{
	m_errorExecutors.push_back(pErrExecutor);
	if (m_errorExecutors.size() == m_strategyExecutors.size())
	{
		if (m_pPortfolio != NULL)
		{
			logger.Warning(boost::str(boost::format("All executors of Portfolio (%d) in Error and Stop Strategy") % m_pPortfolio->ID()));
			boost::thread(boost::bind(&CPortfolio::StopStrategy, m_pPortfolio));
		}
	}
}

void CMultiOpenStrategy::SetForceClose(int quantity)
{
	boost::mutex::scoped_lock l(m_mut);

	if (m_OpenedExecutors.size() > 0)
	{
		int count = quantity > 0 && quantity < (int)m_OpenedExecutors.size() ? quantity : m_OpenedExecutors.size();

		for (boost::unordered_map<int, CStrategyExecutor*>::iterator iter = m_OpenedExecutors.begin();
			iter != m_OpenedExecutors.end() && count > 0; ++iter, --count)
		{
			CStrategyExecutor* closingExecutor = iter->second;
			closingExecutor->SetForceClose();
		}
	}
}

void CMultiOpenStrategy::SetForceClose(const string& mlOrderId)
{
	boost::mutex::scoped_lock l(m_mut);

	for (boost::unordered_map<int, CStrategyExecutor*>::iterator iter = m_OpenedExecutors.begin(); iter != m_OpenedExecutors.end(); ++iter)
	{
		CStrategyExecutor* closingExecutor = iter->second;

		string openedOrderId;
		if (closingExecutor->GetLastOpenOrderId(openedOrderId))
		{
			if (openedOrderId == mlOrderId)
			{
				closingExecutor->SetForceClose();
				break;
			}
		}
	}
}


