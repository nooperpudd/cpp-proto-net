#include "stdafx.h"
#include "DualQueueStrategy.h"
#include "PortfolioScalperOrderPlacer.h"
#include "globalmembers.h"
#include "Portfolio.h"
#include "DoubleCompare.h"
#include "TechAnalyStrategy.h"
#include "SymbolTimeUtil.h"

#include <sstream>

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
	, m_direction(entity::NET)
	, m_cancelQuoteAsk(-1)
	, m_cancelQuoteBid(-1)
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

void CLevelOrderPlacer::CancelPendingOpenOrder(entity::Quote * pQuote)
{
	m_cancelQuoteAsk = pQuote->ask();
	m_cancelQuoteBid = pQuote->bid();

    CPortfolioQueueOrderPlacer* queueOrderPlacer = GetOrderPlacer();
	if (queueOrderPlacer == NULL)
		return;

	logger.Debug(boost::str(boost::format("Cancelling pending order (Level Id: %d) ... IsOpening: %d")
		% m_levelId % queueOrderPlacer->IsOpening()));
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

void CLevelOrderPlacer::GetStatus(string* status)
{
	*status = boost::str(boost::format("{id:%d,px:%.2f,l/s:%s,status:%d}") % m_levelId % m_levelPx % m_direction % m_status );
}

bool CLevelOrderPlacer::Prepare()
{
	CPortfolioQueueOrderPlacer* queueOrderPlacer = GetOrderPlacer();
	if (queueOrderPlacer == NULL)
		return false;

	return queueOrderPlacer->Prepare();
}

CDualQueueStrategy::CDualQueueStrategy()
	: m_lastAsk(0)
	, m_lastBid(0)
	, m_stableQuoteCount(0)
	, m_stableQuote(false)
	, m_priceTick(0)
	, m_stableTickThreshold(6)
	, m_minWorkingSize(0)
	, m_direction(entity::NET)
	, m_levelsNum(3)
	, m_profitCount(1)
	, m_lossCount(10)
	, m_interval(1)
	, m_openThresholdTimes(2)
	, m_stableMinutesThreshold(5)
	, m_latestHigh(-9999)
	, m_latestLow(9999999)
	, m_stopping(false)
    , m_orderQueued(false)
	, m_longGain(false)
	, m_shortGain(false)
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
	boost::mutex::scoped_lock lkSettings(m_mutSettings);

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
	for (int i = m_levelsNum * 4; i > 0; --i)
	{
		int execId = i;
		CPortfolioQueueOrderPlacer *pOrderPlacer = new CPortfolioQueueOrderPlacer(execId);
		pOrderPlacer->Initialize(pPortf, pOrderProc);
		pOrderPlacer->SetPortfolioTradedEventHandler(
			PortfolioTradedEvent(boost::bind(&CDualQueueStrategy::OnOrderPlacerDone, this, _1, _2, _3, _4, _5)));

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

	// empty executor pool
	while (!m_readyQueue.empty())
		m_readyQueue.pop();

	bool allReady = true;
	for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		bool indReady = iter->second->Prepare();
		if (!indReady)
		{
			allReady = false;
			logger.Warning(boost::str(boost::format("OrderPlacer Level %d is NOT ready yet") % iter->first));
			break;
		}
		int lvlId = iter->first;
		m_readyQueue.push(lvlId);
	}

	return allReady;
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
	boost::mutex::scoped_lock lkSettings(m_mutSettings);
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
					LOG_INFO(logger, "DualQueue - Queue orders >>>>");
                    // Send Long orders
                    for(int i = 1; i <= m_levelsNum; ++i)
                    {
                        CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
                        if(lvlOrdPlacer != NULL)
                        {
                            OpenPosition(lvlOrdPlacer, entity::LONG, pQuote->bid() - (i - 1) * m_priceTick * m_interval, timestamp, pQuote);
							m_longOrderQueue.push_back(lvlOrdPlacer);	// desc
                        }
                    }

                    // Send Short orders
                    for(int i = 1; i <= m_levelsNum; ++i)
                    {
                        CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
                        if(lvlOrdPlacer != NULL)
                        {
                            OpenPosition(lvlOrdPlacer, entity::SHORT, pQuote->ask() + (i - 1) * m_priceTick * m_interval, timestamp, pQuote);
							m_shortOrderQueue.push_back(lvlOrdPlacer);	// asc
                        }
                    }

					m_orderQueued = true;
                }
				else
                {
                    // Handle pending closing order first
                    HandlePendingCloseOrder(timestamp, pQuote);

					// Quote keeps unchanged
					if(DoubleEqual(bid, m_lastBid) && DoubleEqual(ask, m_lastAsk))
					{
						if(m_longGain && !IfLevelExists(bid))
						{
							CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
							if (lvlOrdPlacer != NULL)
							{
								m_longGain = false;
								LOG_INFO(logger,
									boost::str(boost::format("DualQueue - Ever gain and Re-Place LONG order @%.2f") % bid));
								OpenPosition(lvlOrdPlacer, entity::LONG, bid, timestamp, pQuote);
								m_longOrderQueue.push_front(lvlOrdPlacer);
							}
						}

						if(m_shortGain && !IfLevelExists(ask))
						{
							CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
							if (lvlOrdPlacer != NULL)
							{
								m_shortGain = false;
								LOG_INFO(logger,
									boost::str(boost::format("DualQueue - Ever gain and Re-Place SHORT order @%.2f") % ask));
								OpenPosition(lvlOrdPlacer, entity::SHORT, ask, timestamp, pQuote);
								m_shortOrderQueue.push_front(lvlOrdPlacer);
							}
						}
					}

                    // Shift queued orders up/down
                    if(bid > m_lastBid) // bid is going up
                    {
						boost::mutex::scoped_lock l(m_mutOrderQueue);

						LOG_INFO(logger, 
							boost::str(boost::format("DualQueue - bid is going UP %.2f -> %.2f.")
								% m_lastBid % bid));
                        
						// check if higher long order is needed
						if (m_longOrderQueue.empty())
						{
							double buyPx = bid;
							CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
							if (lvlOrdPlacer != NULL)
							{
								OpenPosition(lvlOrdPlacer, entity::LONG, buyPx, timestamp, pQuote);
								m_longOrderQueue.push_back(lvlOrdPlacer);
							}
						}
						else
						{
							double expectedHighestBuyPx = bid;
							// find out the highest sell order
							CLevelOrderPlacer* buyOrdPlacer = NULL;
							double buyDiff = -1;
							while (buyOrdPlacer == NULL || DoubleGreaterEqual(buyDiff, m_priceTick * m_interval))
							{
								if (buyOrdPlacer != NULL)
								{
									// Place highest buy first before cancelling order
									double buyPx = expectedHighestBuyPx;
									CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
									if (lvlOrdPlacer != NULL)
									{
										LOG_INFO(logger,
											boost::str(boost::format("DualQueue - Complement LONG order @%.2f (HIGHER)") % buyPx));
										OpenPosition(lvlOrdPlacer, entity::LONG, buyPx, timestamp, pQuote);
										m_longOrderQueue.push_front(lvlOrdPlacer);
									}
									else
										break;

									// Cancel Order at lowest buyPx
									if (m_longOrderQueue.size() > 0)
									{
										// find out the lowest buy order
										CLevelOrderPlacer* lowestOrdPlacer = m_longOrderQueue.back();
										LOG_INFO(logger,
											boost::str(boost::format("DualQueue - Cancel LONG order @%.2f [Level Id: %d]")
												% lowestOrdPlacer->GetLevelPx() % lowestOrdPlacer->Id()));
										lowestOrdPlacer->CancelPendingOpenOrder(pQuote);
										m_longOrderQueue.pop_back();
									}
									else
										break;
								}

								if (m_longOrderQueue.size() > 0)
								{
									buyOrdPlacer = m_longOrderQueue.front();
									double highestBuyPx = buyOrdPlacer->GetLevelPx();
									buyDiff = expectedHighestBuyPx - highestBuyPx;
								}
								else
									break;
							}
						}

						double nShifts = (m_levelsNum - 1) * m_priceTick * m_interval;
						double expectedHighestSellPx = ask + nShifts;
						// find out the highest sell order
						CLevelOrderPlacer* highestOrdPlacer = NULL;
						double highestDiff = -1;
						while(highestOrdPlacer == NULL || DoubleGreaterEqual(highestDiff, m_priceTick * m_interval))
						{
							if(highestOrdPlacer != NULL)
							{
								double sellPx = highestOrdPlacer->GetLevelPx() + m_priceTick * m_interval;
								CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
								if (lvlOrdPlacer != NULL)
								{
									LOG_INFO(logger,
										boost::str(boost::format("DualQueue - Re-queue SHORT order @%.2f (HIGHEST)") % sellPx));
									OpenPosition(lvlOrdPlacer, entity::SHORT, sellPx, timestamp, pQuote);
									m_shortOrderQueue.push_back(lvlOrdPlacer);
								}
								else
									break;
							}

							if (m_shortOrderQueue.size() > 0)
							{
								highestOrdPlacer = m_shortOrderQueue.back();
								double highestSellPx = highestOrdPlacer->GetLevelPx();
								highestDiff = expectedHighestSellPx - highestSellPx;
							}
							else
								break;
						}
                    }

                    if(ask < m_lastAsk)
                    {
						boost::mutex::scoped_lock l(m_mutOrderQueue);

						LOG_INFO(logger,
							boost::str(boost::format("DualQueue - ask is going DOWN %.2f -> %.2f")
								% m_lastAsk % ask));

						// check if lower short order is needed
						if (m_shortOrderQueue.empty())
						{
							double sellPx = ask;
							CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
							if (lvlOrdPlacer != NULL)
							{
								OpenPosition(lvlOrdPlacer, entity::SHORT, sellPx, timestamp, pQuote);
								m_shortOrderQueue.push_back(lvlOrdPlacer);
							}
						}
						else
						{
							double expectedLowestSellPx = ask;
							// find out the highest sell order
							CLevelOrderPlacer* sellOrdPlacer = NULL;
							double sellDiff = -1;
							while (sellOrdPlacer == NULL || DoubleGreaterEqual(sellDiff, m_priceTick * m_interval))
							{
								if (sellOrdPlacer != NULL)
								{
									double sellPx = expectedLowestSellPx;
									CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
									if (lvlOrdPlacer != NULL)
									{
										LOG_INFO(logger,
											boost::str(boost::format("DualQueue - Complement SHORT order @%.2f (LOWER)") % sellPx));
										OpenPosition(lvlOrdPlacer, entity::SHORT, sellPx, timestamp, pQuote);
										m_shortOrderQueue.push_front(lvlOrdPlacer);
									}
									else
										break;

									// Cancel Order at highest sellPx
									if (m_shortOrderQueue.size() > 0)
									{
										CLevelOrderPlacer* highestOrdPlacer = m_shortOrderQueue.back();
										LOG_INFO(logger,
											boost::str(boost::format("DualQueue - Cancel LONG order @%.2f [Level Id: %d]")
												% highestOrdPlacer->GetLevelPx() % highestOrdPlacer->Id()));
										highestOrdPlacer->CancelPendingOpenOrder(pQuote);
										m_shortOrderQueue.pop_back();
									}
									else
										break;
								}

								if (m_shortOrderQueue.size() > 0)
								{
									sellOrdPlacer = m_shortOrderQueue.front();
									double lowestSellPx = sellOrdPlacer->GetLevelPx();
									sellDiff = lowestSellPx - expectedLowestSellPx;
								}
								else
									break;
							}
						}

						double nShifts = (m_levelsNum - 1) * m_priceTick * m_interval;
						double expectedLowestBuyPx = bid - nShifts;
						// find out the lowest buy order
						CLevelOrderPlacer* lowestOrdPlacer = NULL;
						double lowestDiff = -1;
						while (lowestOrdPlacer == NULL || DoubleGreaterEqual(lowestDiff, m_priceTick * m_interval))
						{
							if (lowestOrdPlacer != NULL)
							{
								double buyPx = lowestOrdPlacer->GetLevelPx() - m_priceTick * m_interval;
								CLevelOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
								if (lvlOrdPlacer != NULL)
								{
									LOG_INFO(logger,
										boost::str(boost::format("DualQueue - Re-queue LONG order @%.2f (LOWEST)") % buyPx));
									OpenPosition(lvlOrdPlacer, entity::LONG, buyPx, timestamp, pQuote);
									m_longOrderQueue.push_back(lvlOrdPlacer);
								}
								else
									break;
							}

							if (m_longOrderQueue.size() > 0)
							{
								lowestOrdPlacer = m_longOrderQueue.back();
								double lowestBuyPx = lowestOrdPlacer->GetLevelPx();
								lowestDiff = lowestBuyPx - expectedLowestBuyPx;
							}
							else
								break;
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
	string dqStatus;
	GetLevelOrderPlacerStatus(&dqStatus);
	pPortfUpdateItem->set_dq_status(dqStatus);
}

void CDualQueueStrategy::GetLevelOrderPlacerStatus(string* outStatus)
{
	std::ostringstream statusStream;
	statusStream << "[";

	for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		if (iter != m_levelOrderPlacers.begin())
			statusStream << ";";

		string lvlStatus;
		iter->second->GetStatus(&lvlStatus);
		statusStream << lvlStatus;
	}

	statusStream << "]";

	*outStatus = statusStream.str();
}


void CDualQueueStrategy::OpenPosition(CLevelOrderPlacer* pLevelPlacer, entity::PosiDirectionType direction, double openPx, boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote, bool async)
{
	if (direction > entity::NET)
	{
		pLevelPlacer->Reset();	// Reset Last Cancel Quote info
		pLevelPlacer->SetDirection(direction);

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
			LOG_DEBUG(logger, boost::str(boost::format("Double Queue - %s Open position @ %.2f (%s) [Level Id: %d]")
				% GetPosiDirectionText(direction) % lmtPrice[0] % pQuote->update_time() % pLevelPlacer->Id()));
		}
		else
		{
			LOG_DEBUG(logger, boost::str(boost::format("Double Queue - %s Open position @ %.2f (Replace Order) [Level Id: %d]")
				% GetPosiDirectionText(direction) % lmtPrice[0] % pLevelPlacer->Id()));
		}

		string openComment = boost::str(boost::format("ÅÅ¶Ó %s ¿ª²Ö @ %.2f") % GetPosiDirectionText(direction) % lmtPrice[0]);

		pLevelPlacer->SetLevelPx(openPx);
		pLevelPlacer->GetOrderPlacer()->SetMlOrderStatus(openComment);
		pLevelPlacer->SetStatus(DQ_IS_OPENING);
		if(async)
			pLevelPlacer->GetOrderPlacer()->AsyncQueueOrder(direction, lmtPrice[0], lmtPrice[1], timestamp);
		else
			pLevelPlacer->GetOrderPlacer()->QueueOrder(direction, lmtPrice[0], lmtPrice[1], timestamp);
		ResetForceOpen();
	}
}

void CDualQueueStrategy::OnLegFilled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume, int execId)
{
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

void CDualQueueStrategy::OnOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded, entity::PosiDirectionType direction)
{
	if(execId > 0)
	{
		boost::mutex::scoped_lock l(m_mutOrderQueue);
		if (doneState == PortfolioFilled)
		{
			boost::mutex::scoped_lock lk(m_mutStopLoss);
			LOG_DEBUG(logger, boost::str(boost::format("Double Queue - %s Order(Level Id: %d) Finished")
				% (direction == trade::LONG ? "LONG" : "SHORT") % execId));
			ResetLevelOrderPlacer(execId);
			if(direction == trade::LONG)
			{
				deque<CLevelOrderPlacer*>::iterator iter = FindLevelOrderPlacer(m_longOrderQueue, execId);
				if(iter != m_longOrderQueue.end())
				{
					m_longOrderQueue.erase(iter);
				}
				boost::unordered_set<int>::iterator it = m_longStopLossPlacers.find(execId);
				if(it != m_longStopLossPlacers.end())
				{
					m_longStopLossPlacers.erase_return_void(it);
				}
			}
			else if(direction == trade::SHORT)
			{
				deque<CLevelOrderPlacer*>::iterator iter = FindLevelOrderPlacer(m_shortOrderQueue, execId);
				if (iter != m_shortOrderQueue.end())
				{
					m_shortOrderQueue.erase(iter);
				}
				boost::unordered_set<int>::iterator it = m_shortStopLossPlacers.find(execId);
				if (it != m_shortStopLossPlacers.end())
				{
					m_shortStopLossPlacers.erase_return_void(it);
				}
			}
		}
		else if (doneState == PortfolioCanceled)
		{
			LOG_DEBUG(logger, boost::str(boost::format("Double Queue - %s Open Order(Level Id: %d) Cancelled")
				% (direction == trade::LONG ? "LONG" : "SHORT") % execId));

			ResetLevelOrderPlacer(execId);
			if (direction == trade::LONG)
			{
				deque<CLevelOrderPlacer*>::iterator iter = FindLevelOrderPlacer(m_longOrderQueue, execId);
				if (iter != m_longOrderQueue.end())
				{
					m_longOrderQueue.erase(iter);
				}
			}
			else if (direction == trade::SHORT)
			{
				deque<CLevelOrderPlacer*>::iterator iter = FindLevelOrderPlacer(m_shortOrderQueue, execId);
				if (iter != m_shortOrderQueue.end())
				{
					m_shortOrderQueue.erase(iter);
				}
			}

			/*
			if (!m_stopping)
			{
				boost::unordered_map<int, LevelOrderPlacerPtr>::iterator iter = m_levelOrderPlacers.find(execId);
				if (iter != m_levelOrderPlacers.end())
				{
					CLevelOrderPlacer* lvlOrdPlacer = (iter->second).get();
					if (lvlOrdPlacer != NULL)
					{
						double cancelQuoteBid = lvlOrdPlacer->CancelQuoteBid();
						double cancelQuoteAsk = lvlOrdPlacer->CancelQuoteAsk();
						boost::chrono::steady_clock::time_point nowTime(boost::chrono::steady_clock::now());

						// Resend the order at position near ask/bid
						if (direction == entity::LONG)
						{
							boost::mutex::scoped_lock l(m_mutOrderQueue);

							double buyPx = cancelQuoteBid - (m_priceTick * m_interval);
							if(m_longOrderQueue.size() > 0)
							{
								CLevelOrderPlacer* highestBuyOrder = m_longOrderQueue.front();
								buyPx = highestBuyOrder->GetLevelPx() + m_priceTick * m_interval;
							}
							
							LOG_DEBUG(logger, boost::str(boost::format("Double Queue - Possible Replacing BUY Price @%.2f and Reference Bid @%.2f")
								% buyPx % cancelQuoteBid));
							if(DoubleLess(buyPx, cancelQuoteBid))
							{
								LOG_DEBUG(logger, boost::str(boost::format("Double Queue - Replace LONG order @%.2f (Level Id: %d)")
									% buyPx % execId));
								OpenPosition(lvlOrdPlacer, entity::LONG, buyPx, nowTime, NULL, true);
								m_longOrderQueue.push_front(lvlOrdPlacer);
							}
							else
							{
								ResetLevelOrderPlacer(execId, lvlOrdPlacer);
							}
						}
						else if (direction == entity::SHORT)
						{
							boost::mutex::scoped_lock l(m_mutOrderQueue);

							double sellPx = cancelQuoteAsk + (m_priceTick * m_interval);
							if(m_shortOrderQueue.size() > 0)
							{
								CLevelOrderPlacer* lowestSellOrder = m_shortOrderQueue.front();
								sellPx = lowestSellOrder->GetLevelPx() - m_priceTick * m_interval;
							}
							
							LOG_DEBUG(logger, boost::str(boost::format("Double Queue - Possible Replacing SELL Price @%.2f and Reference Ask @%.2f")
								% sellPx % cancelQuoteAsk));
							if (DoubleGreater(sellPx, cancelQuoteAsk)) // still a valid price
							{
								LOG_DEBUG(logger, boost::str(boost::format("Double Queue - Replace SHORT order @%.2f (Level Id: %d)")
									% sellPx % execId));
								OpenPosition(lvlOrdPlacer, entity::SHORT, sellPx, nowTime, NULL, true);
								m_shortOrderQueue.push_front(lvlOrdPlacer);
							}
							else
							{
								ResetLevelOrderPlacer(execId, lvlOrdPlacer);
							}
						}
					}
					else
					{
						logger.Warning(boost::str(boost::format("Double Queue - %d LevelOrderPlacer encounter NULL pointer") % execId));
					}
				}
				else
				{
					logger.Warning(boost::str(boost::format("Double Queue - Cannot find %d LevelOrderPlacer") % execId));
				}
			}
			*/
		}
	}
}

void CDualQueueStrategy::OnLegCanceled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, int execId)
{
	if(execId > 0)
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

	trade::PosiDirectionType gainDirection = GetOrderDirection(openOrder);
	if(ord_profit - 0.001 > 0)
	{
		if (gainDirection == trade::LONG)
			m_longGain = true;
		else if (gainDirection == trade::SHORT)
			m_shortGain = true;
	}
	else
	{
		if (gainDirection == trade::LONG)
			m_longGain = false;
		else if (gainDirection == trade::SHORT)
			m_shortGain = false;
	}


	return totalOpenTimes;
}

trade::PosiDirectionType CDualQueueStrategy::GetOrderDirection(const trade::MultiLegOrder& multiLegOrder)
{
	int legCount = multiLegOrder.legs_size();
	if(legCount > 0)
	{
		const trade::Order& legOrd = multiLegOrder.legs(0);
		if (legOrd.direction() == trade::BUY)
			return trade::LONG;
		else
			return trade::SHORT;
	}

	return trade::NET;
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
	boost::mutex::scoped_lock l(m_mutLevels);

	if (!m_readyQueue.empty())
	{
		int readyId = m_readyQueue.front();
		m_readyQueue.pop();

		LevelOrderPlacersIter iterFound = m_levelOrderPlacers.find(readyId);
		if (iterFound != m_levelOrderPlacers.end())
		{
			return (iterFound->second).get();
		}
	}
	else
		logger.Warning("Double Queue -- NO ready order placers any more!!!");
	return NULL;
}

bool CDualQueueStrategy::IfLevelExists(double comparingPx)
{
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
	boost::mutex::scoped_lock l(m_mutStopLoss);

    for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		CLevelOrderPlacer* placer = (iter->second).get();
		if(placer != NULL)
		{
			DQ_STATUS status = placer->GetStatus();
			if(DQ_IS_CLOSING == status)
			{
				if(placer->GetDirection() == entity::LONG)
				{
					if(DoubleGreaterEqual(placer->GetLevelPx() - pQuote->bid(), m_lossCount))
					{
						m_longStopLossPlacers.insert(placer->Id());
						iter->second->HandlePendingCloseOrder(timestamp, pQuote);
					}
				}else if(placer->GetDirection() == entity::SHORT)
				{
					if (DoubleGreaterEqual(pQuote->ask() - placer->GetLevelPx(), m_lossCount))
					{
						m_shortStopLossPlacers.insert(placer->Id());
						iter->second->HandlePendingCloseOrder(timestamp, pQuote);
					}
				}
			}
		}
	}
}

CLevelOrderPlacer* CDualQueueStrategy::FindLowestOrderPlacer(double lowestPx)
{
	LOG_DEBUG(logger, boost::str(boost::format("Looking for orderplacer with px < %.2f ...") % lowestPx));

    for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		if(iter->second->GetStatus() == DQ_IS_OPENING)
		{
			double levelPx = iter->second->GetLevelPx();
			if (DoubleLess(levelPx, lowestPx))
				return (iter->second).get();
		}
	}

    return NULL;
}

CLevelOrderPlacer* CDualQueueStrategy::FindHighestOrderPlacer(double highestPx)
{
	LOG_DEBUG(logger, boost::str(boost::format("Looking for orderplacer with px > %.2f ...") % highestPx));

    for (LevelOrderPlacersIter iter = m_levelOrderPlacers.begin(); iter != m_levelOrderPlacers.end(); ++iter)
	{
		if (iter->second->GetStatus() == DQ_IS_OPENING)
		{
			double levelPx = iter->second->GetLevelPx();
			if (DoubleGreater(levelPx, highestPx))
				return (iter->second).get();
		}
	}

    return NULL;
}

void CDualQueueStrategy::ResetLevelOrderPlacer(int execId, CLevelOrderPlacer* levelOrderPlacer)
{
	LOG_DEBUG(logger, boost::str(boost::format("Double Queue - Push back Level Id: %d to queue") % execId));
	boost::mutex::scoped_lock l(m_mutLevels);
	m_readyQueue.push(execId);

	if(NULL != levelOrderPlacer)
	{
		levelOrderPlacer->SetLevelPx(0);
	}
	else
	{
		boost::unordered_map<int, LevelOrderPlacerPtr>::iterator iter = m_levelOrderPlacers.find(execId);
		if (iter != m_levelOrderPlacers.end())
		{
			(iter->second)->SetLevelPx(0);	// reset level price
		}
	}
}

deque<CLevelOrderPlacer*>::iterator CDualQueueStrategy::FindLevelOrderPlacer(deque<CLevelOrderPlacer*>& queue, int execId)
{
	for (deque<CLevelOrderPlacer*>::iterator iter = queue.begin(); iter != queue.end(); ++ iter)
	{
		if (execId == (*iter)->Id())
			return iter;
	}

	return queue.end();
}
