#include "stdafx.h"
#include "DualQueueStrategy.h"
#include "PortfolioScalperOrderPlacer.h"
#include "globalmembers.h"
#include "Portfolio.h"
#include "DoubleCompare.h"
#include "TechAnalyStrategy.h"
#include "PortfolioArbitrageOrderPlacer.h"
#include "SymbolTimeUtil.h"

void PrintQuote(entity::Quote* pQuote)
{
	string quoteTxt = boost::str(boost::format("Display Quote: A:%.2f(%d), B:%.2f(%d)")
		% pQuote->ask() % pQuote->ask_size() % pQuote->bid() % pQuote->bid_size());
	LOG_DEBUG(logger, quoteTxt);
}

CLevelOrderPlacer::CLevelOrderPlacer(int lvlId, CPortfolioQueueOrderPlacer* pOrderPlacer)
	: m_levelId(lvlId)
	, m_orderPlacer(pOrderPlacer)
	, m_status(DQ_UNOPENED)
{
}

CPortfolioQueueOrderPlacer* CLevelOrderPlacer::GetOrderPlacer()
{
	CPortfolioOrderPlacer* placer = m_orderPlacer.get();
	return placer != NULL ? dynamic_cast<CPortfolioQueueOrderPlacer*>(placer) : NULL;
}

bool CLevelOrderPlacer::IsStop(entity::Quote * pQuote)
{
	bool working = m_orderPlacer->IsWorking();
	if (working)
		CancelPendingAndClosePosition(pQuote);
	return !working;
}

void CLevelOrderPlacer::CancelPendingAndClosePosition(entity::Quote * pQuote)
{
	CPortfolioQueueOrderPlacer* queueOrderPlacer = GetOrderPlacer();
	if (queueOrderPlacer == NULL)
		return;

	if (queueOrderPlacer->IsOpening())
		queueOrderPlacer->CancelPendingOrder();
	// If own position, closing position
	else if (queueOrderPlacer->IsClosing())
		queueOrderPlacer->CancelPendingAndClosePosition(pQuote);
}

CDualQueueStrategy::CDualQueueStrategy()
	: m_priceTick(0)
	, m_stableTickThreshold(6)
	, m_levelsNum(5)
	, m_stableMinutesThreshold(5)
	, m_openThresholdTimes(2)
	, m_latestHigh(-9999)
	, m_latestLow(9999999)
	, m_minWorkingSize(0)
	, m_direction(entity::NET)
	, m_stopping(false)
	, m_lastAsk(0)
	, m_lastBid(0)
	, m_stableQuoteCount(0)
	, m_stableQuote(false)
{
}


CDualQueueStrategy::~CDualQueueStrategy()
{
}

void CDualQueueStrategy::Apply(const entity::StrategyItem & strategyItem, CPortfolio * pPortfolio, bool withTriggers)
{
	if (withTriggers)
	{
		logger.Debug("Applying Dual Queue Strategy...");
	}
	boost::mutex::scoped_lock l(m_mut);

	CStrategy::Apply(strategyItem, withTriggers);

	m_priceTick = strategyItem.dq_pricetick();
	m_direction = strategyItem.dq_direction();
	m_stableTickThreshold = strategyItem.dq_stabletickthreshold();
	m_minWorkingSize = strategyItem.dq_minworkingsize();

	logger.Debug(
		boost::str(boost::format("Portfolio(%s) DualQueue: PxTick = %.2f, StableTickThreshold = %d, MinWorkingSize = %d")
			% pPortfolio->ID() % m_priceTick % m_stableTickThreshold  % m_minWorkingSize
			));
}

void CDualQueueStrategy::Apply(const entity::StrategyItem & strategyItem, bool withTriggers)
{
	Apply(strategyItem, false);
}

CPortfolioOrderPlacer * CDualQueueStrategy::CreateOrderPlacer()
{
	return new CPortfolioQueueOrderPlacer(0);
}

void CDualQueueStrategy::InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc)
{
	for (int i = 0; i < m_levelsNum; ++i)
	{
		int execId = i + 1;
		CPortfolioQueueOrderPlacer *pOrderPlacer = new CPortfolioQueueOrderPlacer(execId);
		pOrderPlacer->Initialize(pPortf, pOrderProc);
		LevelOrderPlacerPtr levelOrderPlacer(new CLevelOrderPlacer(execId, pOrderPlacer));
		m_levelOrderPlacers.insert(make_pair(execId, levelOrderPlacer));
	}
}

bool CDualQueueStrategy::OnStart()
{
	m_stopping = false;

	if (IsRunning())
	{
		return false;
	}

	return true;
}

void CDualQueueStrategy::AlreadyStarted()
{
	m_stopping = false;
}

void CDualQueueStrategy::OnStop()
{
	LOG_DEBUG(logger, "DualQueue - Truly Stopped");

	m_stopping = false;
}

void CDualQueueStrategy::Stop()
{
	LOG_DEBUG(logger, "DualQueue - Stopping");
	m_stopping = true;
}

void CDualQueueStrategy::Test(entity::Quote * pQuote, CPortfolio * pPortfolio, boost::chrono::steady_clock::time_point & timestamp)
{
	boost::mutex::scoped_lock l(m_mut);

	if (pQuote->ask() > 0 && pQuote->bid() > 0)
	{

		if (IsRunning() && !IsSuspending())
		{
			if (m_stopping)
			{
				if (EnsureAllPlacerStop(pQuote))
				{
					// Truly Stop Strategy
					CStrategy::Stop();
				}
			}
			else
			{
				if (OperatingConditionCheck(pQuote))
				{
					// including list order size check
					entity::PosiDirectionType direction = DecideOpenDirection(pQuote);
					if(direction != entity::NET && !IfLevelExists(pQuote))
					{
						CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
						if (lvlOrdPlacer != NULL)
						{
							OpenPosition(lvlOrdPlacer, direction, pQuote, timestamp, false);
						}
						else
						{
							// There is no available order placer for opening position
						}
					}
					
				}
			}
		}

		/*
		CPortfolioQueueOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioQueueOrderPlacer*>(pPortfolio->OrderPlacer());

		m_stableQuote = IfQuotingStable(pQuote);
		PrintQuote(pQuote);
		LOG_DEBUG(logger, boost::str(boost::format("DualQueue - Quote %s Stable. Stable Quote Count:%d") 
			% (m_stableQuote ? "IS" : "IS NOT") % m_stableQuoteCount));

		if (IsRunning() && !IsSuspending())
		{
			if (m_stopping)
			{
				if (!pOrderPlacer->IsWorking())
				{
					// Truly Stop Strategy
					CStrategy::Stop();
				}
				// Cancel order in pending state
				else if(pOrderPlacer->IsOpening())
					pOrderPlacer->CancelPendingOrder();
				// If own position, closing position
				else if (pOrderPlacer->IsClosing())
					pOrderPlacer->CancelPendingAndClosePosition(pQuote);
			}
			else
			{
				if(m_stableQuote)
				{
					// Open position if empty
					if (!pOrderPlacer->IsWorking())
					{
						OpenPosition(pOrderPlacer, pQuote, timestamp, false);
					}
					else if(pOrderPlacer->IsOnPending())
					{
						pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
					}
				}
				
				bool forceClosing = IsForceClosing();
				if (forceClosing)
				{
					if (pOrderPlacer->IsClosing())
					{
						pOrderPlacer->CancelPendingAndClosePosition(pQuote);
					}
					else
					{
						ResetForceClose();	// ignore and reset unexpected force closing
					}
				}
			}
		}
		*/
	}

	// there is only ONE leg for scalper strategy 
	const LegPtr& leg = pPortfolio->Legs().at(0);
	leg->UpdateLast(pQuote->last());
	leg->UpdateAsk(pQuote->ask());
	leg->UpdateAskSize(pQuote->ask_size());
	leg->UpdateBid(pQuote->bid());
	leg->UpdateBidSize(pQuote->bid_size());
	leg->UpdateTimestamp(pQuote->update_time(), pQuote->update_millisec());
}

void CDualQueueStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem * pPortfUpdateItem)
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);
	pPortfUpdateItem->set_dq_stablequote(m_stableQuote);
	//pPortfUpdateItem->set_dq_status(static_cast<entity::LegStatus>(m_status.load()));
}


void CDualQueueStrategy::OpenPosition(CLevelOrderPlacer* pLevelPlacer, entity::PosiDirectionType direction, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening)
{
	if (direction > entity::NET)
	{
		double lmtPrice[2];
		if (direction == entity::LONG)
		{
			lmtPrice[0] = pQuote->bid();
			lmtPrice[1] = pQuote->ask();
		}
		else if (direction == entity::SHORT)
		{
			lmtPrice[0] = pQuote->ask();
			lmtPrice[1] = pQuote->bid();
		}
		
		LOG_DEBUG(logger, boost::str(boost::format("Double Queue - %s Open position @ %.2f (%s)")
			% GetPosiDirectionText(m_direction) % lmtPrice[0] % pQuote->update_time()));

		string openComment = boost::str(boost::format("ÅÅ¶Ó %s ¿ª²Ö @ %.2f") % GetPosiDirectionText(direction) % lmtPrice[0]);

		pLevelPlacer->SetLevelPx(pQuote->bid());
		pLevelPlacer->GetOrderPlacer()->SetMlOrderStatus(openComment);
		pLevelPlacer->SetStatus(DQ_IS_OPENING);
		pLevelPlacer->GetOrderPlacer()->QueueOrder(m_direction, lmtPrice[0], lmtPrice[1], timestamp);
		ResetForceOpen();
	}
}

void CDualQueueStrategy::OnLegFilled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume, int execId)
{
	boost::mutex::scoped_lock l(m_mutLevels);

	if(execId > 0)
	{
		DQ_STATUS status = DQ_UNKNOWN;
		if (offset == trade::OF_OPEN && sendingIdx == 0)
		{
			status = DQ_IS_CLOSING;
		}
		else if ((offset == trade::OF_CLOSE || offset == trade::OF_CLOSE_TODAY) && sendingIdx == 1)
		{
			if (m_stopping)
				status = DQ_UNOPENED;
			else
				status = DQ_CLOSED;

			//ResetForceClose();
		}

		if (status > DQ_UNKNOWN)
		{
			boost::unordered_map<int, LevelOrderPlacerPtr>::iterator iter = m_levelOrderPlacers.find(execId);
			if (iter != m_levelOrderPlacers.end())
			{
				(iter->second)->SetStatus(status);
			}
		}
	}
}

void CDualQueueStrategy::OnLegCanceled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, int execId)
{
	boost::mutex::scoped_lock l(m_mutLevels);

	if(m_stopping)
	{
		DQ_STATUS status = DQ_UNKNOWN;
		if (offset == trade::OF_OPEN && sendingIdx == 0)
		{
			status = DQ_UNOPENED;
		}
		else if ((offset == trade::OF_CLOSE || offset == trade::OF_CLOSE_TODAY) && sendingIdx == 1)
		{
			status = DQ_IS_CLOSING;
		}

		if (status > DQ_UNKNOWN)
		{
			boost::unordered_map<int, LevelOrderPlacerPtr>::iterator iter = m_levelOrderPlacers.find(execId);
			if (iter != m_levelOrderPlacers.end())
			{
				(iter->second)->SetStatus(status);
			}
		}
	}
}

int CDualQueueStrategy::OnPortfolioAddPosition(CPortfolio * pPortfolio, const trade::MultiLegOrder & openOrder, int actualTradedVol)
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}

void CDualQueueStrategy::OnStrategyError(CPortfolio * portf, const string & errorMsg) const
{
	if (portf != NULL)
		boost::thread(boost::bind(&CPortfolio::StopStrategyDueTo, portf, errorMsg));
}

bool CDualQueueStrategy::IfQuotingStable(entity::Quote * pQuote)
{
	double ask = pQuote->ask();
	double bid = pQuote->bid();
	if(DoubleEqual(ask, m_lastAsk) && DoubleEqual(bid, m_lastBid))
	{
		++m_stableQuoteCount;
	}else
	{
		m_stableQuoteCount = 0;
	}
	m_lastAsk = ask;
	m_lastBid = bid;

	bool stable = m_stableQuoteCount >= m_stableTickThreshold;
	if (m_minWorkingSize > 0)
		stable = stable && (pQuote->ask_size() >= m_minWorkingSize && pQuote->bid_size() >= m_minWorkingSize);
	return stable;
}

void CDualQueueStrategy::CalculateContext(entity::Quote* pQuote)
{
	m_lastAsk = pQuote->ask();
	m_lastBid = pQuote->bid();
	double last = pQuote->last();
	boost::chrono::seconds currentTime = ParseTimeString(pQuote->update_time());
	if (DoubleLessEqual(last, m_latestHigh) && DoubleGreaterEqual(last, m_latestLow))
	{	// between upper and lower
		boost::chrono::seconds nearTime = m_latestHighTime < m_latestLowTime ? m_latestHighTime : m_latestLowTime;
		m_stableSeconds = currentTime - nearTime;
	}
	else
	{
		if (last > m_latestHigh)	// going up
		{
			m_latestHigh = last;
			m_latestHighTime = currentTime;
		}
		if (last < m_latestLow)		// going down
		{
			m_latestLow = last;
			m_latestLowTime = currentTime;
		}
	}
}

bool CDualQueueStrategy::OperatingConditionCheck(entity::Quote * pQuote)
{
	return m_stableSeconds > m_stableMinutes;
}

entity::PosiDirectionType CDualQueueStrategy::DecideOpenDirection(entity::Quote * pQuote)
{
	if (pQuote->ask() > m_lastAsk)
		return entity::SHORT;

	if (pQuote->bid() < m_lastBid)
		return entity::LONG;

	if (pQuote->ask_size() >= pQuote->bid_size())
		return entity::SHORT;
	
	return entity::LONG;
}

bool CDualQueueStrategy::EnsureAllPlacerStop(entity::Quote * pQuote)
{
	for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		if (!iter->second->IsStop(pQuote))
			return false;
	}
	return true;
}

CLevelOrderPlacer* CDualQueueStrategy::GetReadyOrderPlacer()
{
	if(!m_readyQueue.empty())
	{
		int readyId = m_readyQueue.front();
		m_readyQueue.pop();

		LevelOrderPlacersIter iterFound = m_levelOrderPlacers.find(readyId);
		if(iterFound != m_levelOrderPlacers.end())
		{
			return (iterFound->second).get();
		}
	}
	return NULL;
}

bool CDualQueueStrategy::IfLevelExists(entity::Quote* pQuote)
{
	boost::mutex::scoped_lock l(m_mutLevels);

	double comparingPx = pQuote->bid();
	for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		double levelPx = iter->second->GetLevelPx();
		if (DoubleEqual(comparingPx, levelPx))
			return true;
	}

	return false;
}
