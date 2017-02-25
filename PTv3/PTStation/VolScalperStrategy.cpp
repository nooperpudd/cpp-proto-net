#include "stdafx.h"
#include "VolScalperStrategy.h"
#include "globalmembers.h"
#include "SettingChangeTrace.h"
#include "Portfolio.h"
#include "DoubleCompare.h"
#include "PortfolioTrendOrderPlacer.h"


CVolScalperStrategy::CVolScalperStrategy() : CStrategy()
, m_priceTick(1)
, m_lot(10)
, m_stopping(false)
, m_lastTurnover(0)
, m_lastVolume(0)
, m_lastAsk(0)
, m_lastBid(0)
, m_ask(0)
, m_askSize(0)
, m_bid(0)
, m_bidSize(0)
, m_vwapLast(0)
, m_vwapNow(0)
, m_speedLast(0)
, m_speedNow(0)
, m_enterPrice(0)
, m_openThreshold(0)
, m_closeThreshold(0)
, m_lossThreshold(0)
, m_pxOffset(0)
, m_maxPosition(0)
, m_actTurnover(0)
, m_actVolume(0)
, m_buyVolume(0)
, m_sellVolume(0)
, m_gainCount(0)
, m_lossCount(0)
{
}


CVolScalperStrategy::~CVolScalperStrategy()
{
}

void CVolScalperStrategy::Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers)
{
	Apply(strategyItem, false);
	CreateTriggers(strategyItem);
}

void CVolScalperStrategy::Apply(const entity::StrategyItem& strategyItem, bool withTriggers)
{
	if (withTriggers)
	{
		logger.Debug("Applying VolScalperStrategy...");
#ifdef LOG_FOR_TRADE
		PrintDoubleChange("Price Tick", m_priceTick, strategyItem.sc_pricetick());
#endif
	}
	boost::mutex::scoped_lock lkSettings(m_mutSettings);

	CStrategy::Apply(strategyItem, withTriggers);

	m_trigger = boost::make_shared<CSpringV51Trigger>(this);
}

void CVolScalperStrategy::BeforeTest(entity::Quote* pQuote)
{
	m_ask = pQuote->ask();
	m_askSize = pQuote->ask_size();
	m_bid = pQuote->bid();
	m_bidSize = pQuote->bid_size();
	m_actVolume = pQuote->volume() - m_lastVolume;
	m_actTurnover = pQuote->turnover() - m_lastTurnover;
	
	if(DoubleEqual(m_ask, pQuote->last()))
	{
		m_sellVolume = (m_ask * m_actVolume - m_actTurnover / m_lot) / (m_ask - m_bid);
		m_buyVolume = m_actVolume - m_sellVolume;
	}else if(DoubleEqual(m_bid, pQuote->last()))
	{
		m_buyVolume = (m_actTurnover / m_lot - m_bid * m_actVolume) / (m_ask - m_bid);
		m_sellVolume = m_actVolume - m_buyVolume;
	}
	
	// Calculate volume weighted average price
	m_vwapNow = m_actVolume / m_lot / m_actTurnover;
	if(m_vwapLast > 0)
	{
		m_speedNow = m_vwapNow - m_vwapLast;
	}
}

void CVolScalperStrategy::AfterTest(entity::Quote* pQuote)
{
	m_lastAsk = m_ask;
	m_lastBid = m_bid;
	m_lastVolume = pQuote->volume();
	m_lastTurnover = pQuote->turnover();

	m_vwapLast = m_vwapNow;
	m_speedLast = m_speedNow;
}

void CVolScalperStrategy::Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	boost::mutex::scoped_lock lkSettings(m_mutSettings);

	BeforeTest(pQuote);

	if (m_askSize > 0 && m_bidSize > 0)
	{
		if (IsRunning())
		{
			HandleOpenedPosition(timestamp, pQuote);

			// Open order if condition is meet
			if(!IsSuspending())
			{
				string triggerComment;
				entity::PosiDirectionType direction = m_trigger->TestForOpen(pQuote, triggerComment);
				if(direction > entity::NET)
				{
					CPortfolioTrendOrderPlacer* lvlOrdPlacer = GetReadyOrderPlacer();
					if (lvlOrdPlacer != NULL)
					{
						OpenPosition(lvlOrdPlacer, direction, timestamp, pQuote, triggerComment);
					}
				}
			}
		}
	}

	AfterTest(pQuote);

	// there is only ONE leg for scalper strategy 
	UpdateLeg(pQuote, pPortfolio);
}

void CVolScalperStrategy::UpdateLeg(entity::Quote* pQuote, CPortfolio* pPortfolio)
{
	const LegPtr& leg = pPortfolio->Legs().at(0);
	leg->UpdateLast(pQuote->last());
	leg->UpdateAsk(pQuote->ask());
	leg->UpdateAskSize(pQuote->ask_size());
	leg->UpdateBid(pQuote->bid());
	leg->UpdateBidSize(pQuote->bid_size());
	leg->UpdateTimestamp(pQuote->update_time(), pQuote->update_millisec());
}

void CVolScalperStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem)
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);
}

int CVolScalperStrategy::OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol)
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}


CPortfolioOrderPlacer* CVolScalperStrategy::CreateOrderPlacer()
{
	return new CPortfolioTrendOrderPlacer;
}

void CVolScalperStrategy::InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc)
{
	for (int i = m_maxPosition; i > 0; --i)
	{
		int execId = i;
		CPortfolioTrendOrderPlacer *pOrderPlacer = new CPortfolioTrendOrderPlacer(execId);
		pOrderPlacer->Initialize(pPortf, pOrderProc);
		pOrderPlacer->SetPortfolioTradedEventHandler(
			PortfolioTradedEvent(boost::bind(&CVolScalperStrategy::OnOrderPlacerDone, this, _1, _2, _3, _4, _5)));

		TrendOrderPlacerPtr levelOrderPlacer(pOrderPlacer);
		m_levelOrderPlacers.insert(make_pair(execId, levelOrderPlacer));
	}
}

void CVolScalperStrategy::Stop()
{
	LOG_DEBUG(logger, "VolScalper - Stopping");
	m_stopping = true;
}

void CVolScalperStrategy::AlreadyStarted()
{
	m_stopping = false;
}

bool CVolScalperStrategy::OnStart()
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

CPortfolioTrendOrderPlacer* CVolScalperStrategy::GetReadyOrderPlacer()
{
	boost::mutex::scoped_lock l(m_mutReadyQueue);

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

void CVolScalperStrategy::ResetLevelOrderPlacer(int execId)
{
	LOG_DEBUG(logger, boost::str(boost::format("VolScalper - Push back Level Id: %d to queue") % execId));

	boost::mutex::scoped_lock l(m_mutReadyQueue);

	m_readyQueue.push(execId);

	boost::unordered_map<int, TrendOrderPlacerPtr>::iterator iter = m_levelOrderPlacers.find(execId);
	if (iter != m_levelOrderPlacers.end())
	{
		// Do something for reset 
		(iter->second)->SetEntryPrice(0);
	}
}

void CVolScalperStrategy::HandleOpenedPosition(boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote)
{
	boost::mutex::scoped_lock l(m_mutOpenedVector);

	for (vector<TrendOrderPlacerPtr>::iterator iter = m_openedOrdersVector.begin(); 
		iter != m_openedOrdersVector.end(); ++iter)
	{
		CPortfolioTrendOrderPlacer* placer = iter->get();
		if (placer != NULL)
		{
			// Handle closing order first
			if(placer->IsClosing())
			{
				placer->OnQuoteReceived(timestamp, pQuote);
			}
			else if(placer->IsOpened())
			{
				string triggerComment;
				if(m_trigger->TestForClose(pQuote, placer->EntryPrice(), placer->PosiDirection(), triggerComment))
				{
					ClosePosition(placer, pQuote, triggerComment);
				}
			}
		}
	}
}

void CVolScalperStrategy::OpenPosition(CPortfolioTrendOrderPlacer* lvlOrdPlacer, entity::PosiDirectionType direction, boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote, const string& triggerComment) const
{
	if (direction > entity::NET && lvlOrdPlacer != NULL)
	{
		double openPx = direction == entity::LONG ?
			pQuote->bid() + m_priceTick * m_pxOffset : pQuote->ask() - m_priceTick * m_pxOffset;
		LOG_INFO(logger,
			boost::str(boost::format("VolScalper - Open %s Order @%.2f ")
				% (direction == entity::LONG ? "LONG" : "SHORT")
				% openPx));

		double lmtPrice[2] = { openPx, 0 };

		lvlOrdPlacer->SetMlOrderStatus(triggerComment);
		lvlOrdPlacer->SetEntryPrice(m_vwapNow);
		lvlOrdPlacer->Run(direction, lmtPrice, 2, timestamp);
	}
}

void CVolScalperStrategy::ClosePosition(CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const string& triggerComment) const
{
	if (pOrderPlacer != NULL)
	{
		entity::PosiDirectionType posiDirection = pOrderPlacer->PosiDirection();

		double closePx = 0.0;
		if (posiDirection == entity::LONG)
		{
			closePx = pQuote->ask() - m_priceTick * m_pxOffset;
		}
		else if (posiDirection == entity::SHORT)
		{
			closePx = pQuote->bid() + m_priceTick * m_pxOffset;
		}

		LOG_INFO(logger,
			boost::str(boost::format("VolScalper - Close %s Order @%.2f ")
				% (posiDirection == entity::LONG ? "LONG" : "SHORT")
				% closePx));

		pOrderPlacer->CloseOrder(closePx);
		pOrderPlacer->OutputStatus(triggerComment);
	}
}

entity::PosiDirectionType CSpringV51Trigger::TestForOpen(entity::Quote* pQuote, string& outComment)
{
	if(m_strategy->m_speedLast > 0 && m_strategy->m_speedNow > 0)
	{
		if(DoubleGreater(m_strategy->m_speedLast, m_strategy->m_openThreshold)
			&& DoubleGreater(m_strategy->m_speedNow, m_strategy->m_openThreshold))
		{
			LOG_INFO(logger,
				boost::str(boost::format("VolScalper - continuous Px Chg Speed (%.2f, %.2f) > %.2f --> Open LONG position")
					% m_strategy->m_speedLast % m_strategy->m_speedNow
					% m_strategy->m_openThreshold));
			outComment = boost::str(boost::format("连续两个成交均价变动(%.2f, %.2f)大于%.2f - 多头开仓 @ %.2f")
				% m_strategy->m_speedLast % m_strategy->m_speedNow % m_strategy->m_openThreshold
				% pQuote->last());
			return entity::LONG;
		}
	}
	else if(m_strategy->m_speedLast < 0 && m_strategy->m_speedNow < 0)
	{
		if (DoubleLess(m_strategy->m_speedLast, -m_strategy->m_openThreshold)
			&& DoubleLess(m_strategy->m_speedNow, -m_strategy->m_openThreshold))
		{
			LOG_INFO(logger,
				boost::str(boost::format("VolScalper - continuous Px Chg Speed (%.2f, %.2f) < %.2f --> Open SHORT position")
					% m_strategy->m_speedLast % m_strategy->m_speedNow
					% m_strategy->m_openThreshold));
			outComment = boost::str(boost::format("连续两个成交均价变动(%.2f, %.2f)小于%.2f - 空头开仓 @ %.2f")
				% m_strategy->m_speedLast % m_strategy->m_speedNow % m_strategy->m_openThreshold
				% pQuote->last());
			return entity::SHORT;
		}
	}
	return entity::NET;
}

bool CSpringV51Trigger::TestForClose(entity::Quote* pQuote, double entryPrice, entity::PosiDirectionType direction, string& outComment)
{
	if (direction == entity::LONG)
	{
		if (DoubleLess(m_strategy->m_speedLast, m_strategy->m_closeThreshold)
			&& DoubleLess(m_strategy->m_speedNow, m_strategy->m_closeThreshold))
		{
			LOG_INFO(logger,
				boost::str(boost::format("VolScalper - continuous Px Chg Speed (%.2f, %.2f) < %.2f --> Close LONG position")
					% m_strategy->m_speedLast % m_strategy->m_speedNow
					% m_strategy->m_closeThreshold));
			outComment = boost::str(boost::format("连续两个成交均价变动(%.2f, %.2f)小于%.2f - 多头平仓 @ %.2f")
				% m_strategy->m_speedLast % m_strategy->m_speedNow % m_strategy->m_closeThreshold
				% pQuote->last());
			return true;
		}
	}
	else if (direction == entity::SHORT)
	{
		if (DoubleGreater(m_strategy->m_speedLast, -m_strategy->m_closeThreshold)
			&& DoubleGreater(m_strategy->m_speedNow, -m_strategy->m_closeThreshold))
		{
			LOG_INFO(logger,
				boost::str(boost::format("VolScalper - continuous Px Chg Speed (%.2f, %.2f) > %.2f --> Close SHORT position")
					% m_strategy->m_speedLast % m_strategy->m_speedNow
					% m_strategy->m_closeThreshold));
			outComment = boost::str(boost::format("连续两个成交均价变动(%.2f, %.2f)大于%.2f - 空头平仓 @ %.2f")
				% m_strategy->m_speedLast % m_strategy->m_speedNow % m_strategy->m_closeThreshold
				% pQuote->last());
			return true;
		}
	}
	return false;
}

void CVolScalperStrategy::OnStop()
{
	LOG_DEBUG(logger, "VolScalper - Truly Stopped");

	m_stopping = false;
}

void CVolScalperStrategy::OnOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded, entity::PosiDirectionType direction)
{
	if (execId > 0)
	{
		if (doneState == PortfolioFilled)
		{
			LOG_DEBUG(logger, boost::str(boost::format("Double Queue - %s Order(Level Id: %d) Finished")
				% (direction == trade::LONG ? "LONG" : "SHORT") % execId));
			
			ResetLevelOrderPlacer(execId);
		}
		else if (doneState == PortfolioCanceled)
		{
			LOG_DEBUG(logger, boost::str(boost::format("Double Queue - %s Open Order(Level Id: %d) Cancelled")
				% (direction == trade::LONG ? "LONG" : "SHORT") % execId));

			ResetLevelOrderPlacer(execId);
		}
	}
}


