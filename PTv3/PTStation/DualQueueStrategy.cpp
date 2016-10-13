#include "stdafx.h"
#include "DualQueueStrategy.h"
#include "PortfolioScalperOrderPlacer.h"
#include "globalmembers.h"
#include "Portfolio.h"
#include "DoubleCompare.h"
#include "TechAnalyStrategy.h"
#include "SymbolTimeUtil.h"

void PrintQuote(entity::Quote* pQuote)
{
	string quoteTxt = boost::str(boost::format("Display Quote: A:%.2f(%d), B:%.2f(%d)")
		% pQuote->ask() % pQuote->ask_size() % pQuote->bid() % pQuote->bid_size());
	LOG_DEBUG(logger, quoteTxt);
}

CLevelOrderPlacer::CLevelOrderPlacer(int lvlId, CPortfolioQueueOrderPlacer* pOrderPlacer)
	: m_levelId(lvlId)
	, m_levelPx(0)
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

void CLevelOrderPlacer::CancelPendingOpenOrder()
{
    CPortfolioQueueOrderPlacer* queueOrderPlacer = GetOrderPlacer();
	if (queueOrderPlacer == NULL)
		return;

	if (queueOrderPlacer->IsOpening())
		queueOrderPlacer->CancelPendingOrder();
}

void CLevelOrderPlacer::HandlePendingCloseOrder(boost::chrono::steady_clock::time_point& timestamp, entity::Quote * pQuote)
{
    CPortfolioQueueOrderPlacer* queueOrderPlacer = GetOrderPlacer();
	if (queueOrderPlacer == NULL)
		return;

    if (queueOrderPlacer->IsClosing())
		queueOrderPlacer->OnQuoteReceived(timestamp, pQuote);
}

CDualQueueStrategy::CDualQueueStrategy()
	: m_priceTick(0)
	, m_stableTickThreshold(6)
	, m_levelsNum(4)
	, m_profitCount(0)
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
    , m_orderQueued(false)
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
	//m_stableMinutesThreshold = strategyItem.dq_stableminutesthreshold();
	m_stableMinutes = boost::chrono::minutes(m_stableMinutesThreshold);

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
	for (int i = 0; i < m_levelsNum * 3; ++i)
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
	double ask = pQuote->ask();
	double bid = pQuote->bid();
	if ( ask > 0 && bid > 0)
	{
		
		if (IsRunning() && !IsSuspending())
		{
			if (m_stopping)
			{
				if (EnsureAllPlacerStop(pQuote))
				{
					// Truly Stop Strategy
                    m_orderQueued = false;
					CStrategy::Stop();
				}
			}
			else
			{
                if(!m_orderQueued)
                {
                    // If orders are not queued at levels, send orders
                    
                    // Send Long orders
                    for(int i = 1; i <= m_levelsNum; ++i)
                    {
                        CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
                        if(lvlOrdPlacer != NULL)
                        {
                            OpenPosition(lvlOrdPlacer, entity::LONG, pQuote->bid() - (i * m_priceTick), timestamp, pQuote);
                        }
                    }

                    // Send Short orders
                    for(int i = 1; i <= m_levelsNum; ++i)
                    {
                        CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
                        if(lvlOrdPlacer != NULL)
                        {
                            OpenPosition(lvlOrdPlacer, entity::SHORT, pQuote->ask() + (i * m_priceTick), timestamp, pQuote);
                        }
                    }
                }
				else
                {
                    // Handle pending closing order first
                    HandlePendingCloseOrder(timestamp, pQuote);

                    // Shift queued orders up/down
                    if(bid > m_lastBid) // bid is going up
                    {
                        // find out the lowest buy order
                        double buyPx = bid;
                        while(buyPx > m_lastBid + 0.1)
                        {
                            CLevelOrderPlacer* lowestOrdPlacer = FindLowestOrderPlacer(bid - (m_levelsNum * m_priceTick));
                            if(lowestOrdPlacer != NULL)
                            {
                                // Cancel Order and replace at buyPx
                                lowestOrdPlacer->CancelPendingOpenOrder();
                            }
                            buyPx -= m_priceTick;
                        }

                        double sellPx = ask;
                        while(sellPx > m_lastAsk + 0.1)
                        {
                            if(!IfLevelExists(sellPx))
                            {
                                CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
                                if(lvlOrdPlacer != NULL)
                                {
                                    OpenPosition(lvlOrdPlacer, entity::SHORT, sellPx, timestamp, pQuote);
                                }
                            }
                            sellPx -= m_priceTick;
                        } 
                    }

                    if(ask < m_lastAsk)
                    {
                        // find out the highest buy order
                        double sellPx = ask;
                        while(sellPx < m_lastAsk - 0.1)
                        {
                            CLevelOrderPlacer* highestOrdPlacer = FindHighestOrderPlacer(ask + (m_levelsNum * m_priceTick));
                            if(highestOrdPlacer != NULL)
                            {
                                // Cancel Order and replace at buyPx
                                highestOrdPlacer->CancelPendingOpenOrder();
                            }
                            sellPx += m_priceTick;
                        }

                        double buyPx = bid;
                        while(buyPx < m_lastBid - 0.1)
                        {
                            if(!IfLevelExists(buyPx))
                            {
                                CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
                                if(lvlOrdPlacer != NULL)
                                {
                                    OpenPosition(lvlOrdPlacer, entity::LONG, buyPx, timestamp, pQuote);
                                }
                            }
                            buyPx += m_priceTick;
                        } 
                    }
                }

				// Save lastAsk and lastBid
				CalculateContext(pQuote);
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


void CDualQueueStrategy::OpenPosition(CLevelOrderPlacer* pLevelPlacer, entity::PosiDirectionType direction, double openPx, boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote)
{
	if (direction > entity::NET)
	{
		double lmtPrice[2];
		if (direction == entity::LONG)
		{
			lmtPrice[0] = openPx;
			lmtPrice[1] = openPx + m_profitCount * m_priceTick;
		}
		else if (direction == entity::SHORT)
		{
			lmtPrice[0] = openPx;
			lmtPrice[1] = openPx - m_profitCount * m_priceTick;
		}
		
		if(pQuote != NULL)
		{
			LOG_DEBUG(logger, boost::str(boost::format("Double Queue - %s Open position @ %.2f (%s)")
				% GetPosiDirectionText(direction) % lmtPrice[0] % pQuote->update_time()));
		}
		else
		{
			LOG_DEBUG(logger, boost::str(boost::format("Double Queue - %s Open position @ %.2f (Replace Order)")
				% GetPosiDirectionText(direction) % lmtPrice[0]));
		}

		string openComment = boost::str(boost::format("排队 %s 开仓 @ %.2f") % GetPosiDirectionText(direction) % lmtPrice[0]);

		pLevelPlacer->SetLevelPx(openPx);
		pLevelPlacer->GetOrderPlacer()->SetMlOrderStatus(openComment);
		pLevelPlacer->SetStatus(DQ_IS_OPENING);
		pLevelPlacer->GetOrderPlacer()->QueueOrder(direction, lmtPrice[0], lmtPrice[1], timestamp);
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

			m_readyQueue.push(execId);
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

	if(execId > 0)
	{
		DQ_STATUS status = DQ_UNKNOWN;
		if (offset == trade::OF_OPEN && sendingIdx == 0)
		{
			status = DQ_UNOPENED;

			if(!m_stopping)
			{
				boost::chrono::steady_clock::time_point nowTime;
				// Resend the order at position near ask/bid
				if(direction == trade::BUY)
				{
					double buyPx = m_lastBid - m_priceTick;
					while (IfLevelExists(buyPx))
						buyPx -= m_priceTick;
					if(buyPx > 0) // still a valid price
					{
						boost::unordered_map<int, LevelOrderPlacerPtr>::iterator iter = m_levelOrderPlacers.find(execId);
						if (iter != m_levelOrderPlacers.end())
						{
							CLevelOrderPlacer* lvlOrdPlacer = (iter->second).get();
							if (lvlOrdPlacer != NULL)
							{
								OpenPosition(lvlOrdPlacer, entity::LONG, buyPx, nowTime, NULL);
							}
						}
					}
				}
				else if(direction == trade::SELL)
				{
					double sellPx = m_lastAsk + m_priceTick;
					while (IfLevelExists(sellPx))
						sellPx += m_priceTick;
					if (sellPx > 0) // still a valid price
					{
						boost::unordered_map<int, LevelOrderPlacerPtr>::iterator iter = m_levelOrderPlacers.find(execId);
						if (iter != m_levelOrderPlacers.end())
						{
							CLevelOrderPlacer* lvlOrdPlacer = (iter->second).get();
							if (lvlOrdPlacer != NULL)
							{
								OpenPosition(lvlOrdPlacer, entity::SHORT, sellPx, nowTime, NULL);
							}
						}
					}
				}
			}
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
		boost::chrono::seconds nearTime = m_latestHighTime > m_latestLowTime ? m_latestHighTime : m_latestLowTime;
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
	LOG_DEBUG(logger, boost::str(boost::format("Last High: %.2f, Last Low: %.2f, Last: %.2f")
		% m_latestHigh % m_latestLow % pQuote->last()));
	LOG_DEBUG(logger, boost::str(boost::format("Stable seconds: %d Secs vs Threshold %d Mins") 
		% m_stableSeconds % m_stableMinutes));
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

bool CDualQueueStrategy::IfLevelExists(double comparingPx)
{
	boost::mutex::scoped_lock l(m_mutLevels);

	for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		double levelPx = iter->second->GetLevelPx();
		if (DoubleEqual(comparingPx, levelPx))
			return true;
	}

	return false;
}

void CDualQueueStrategy::HandlePendingCloseOrder(boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote)
{
    for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		iter->second->HandlePendingCloseOrder(timestamp, pQuote);
	}
}

CLevelOrderPlacer* CDualQueueStrategy::FindLowestOrderPlacer(double lowestPx)
{
    for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		double levelPx = iter->second->GetLevelPx();
		if (lowestPx - levelPx > m_priceTick/10)
			return (iter->second).get();
	}

    return NULL;
}

CLevelOrderPlacer* CDualQueueStrategy::FindHighestOrderPlacer(double highestPx)
{
    for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		double levelPx = iter->second->GetLevelPx();
		if (levelPx - highestPx> m_priceTick/10)
			return (iter->second).get();
	}

    return NULL;
}
