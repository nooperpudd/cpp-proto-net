#include "stdafx.h"
#include "DualQueueStrategy.h"
#include "ManualOrderPlacer.h"
#include "globalmembers.h"
#include "Portfolio.h"
#include "DoubleCompare.h"


int DQ_TRANSITION_TABLE[15][10] = {
	//		--- COMMON				--- LONG														--- SHORT
	//	[OPENING, CLOSING] [OPENED, OPEN_CANCELED, CLOSED, CLOSE_CANCELED] [OPENED, OPEN_CANCELED, CLOSED, CLOSE_CANCELED]
	//		0		1		  2			3			 4			5			  6			 7			  8			9
	/*0 DS_BOTH_EMPTY*/{ DS_BOTH_OPENING, -2, -3, -4, -5, -6, -7, -8, -9, -10 },
	/*1 DS_BOTH_OPENING*/{ -1, -2, DS_SHORT_OPENING_HOLD_LONG, DS_SHORT_OPENING_ONLY, -5, -6, DS_LONG_OPENING_HOLD_SHORT, DS_LONG_OPENING_ONLY, -9, -10 },
	/*2 DS_LONG_OPENING_HOLD_SHORT*/{ -1, -2, DS_BOTH_HELD, DS_HOLD_SHORT, -5, -6, -7, -8, -9, -10 },
	/*3 DS_LONG_OPENING_ONLY*/{ -1, -2, DS_HOLD_LONG, DS_BOTH_EMPTY, -5, -6, -7, -8, -9, -10 },
	/*4 DS_SHORT_OPENING_HOLD_LONG*/{ -1, -2, -3, -4, -5, -6, DS_BOTH_HELD, DS_HOLD_LONG, -9, -10 },
	/*5 DS_SHORT_OPENING_ONLY*/{ -1, -2, -3, -4, -5, -6, DS_HOLD_SHORT, DS_BOTH_EMPTY, -9, -10 },
	/*6 DS_HOLD_LONG*/{ -1, -2, -3, -4, DS_BOTH_EMPTY, DS_HOLD_LONG, -7, -8, -9, -10 },
	/*7 DS_HOLD_SHORT*/{ -1, -2, -3, -4, -5, -6, -7, -8, DS_BOTH_EMPTY, DS_HOLD_SHORT },
	/*8 DS_BOTH_HELD*/{ -1, DS_BOTH_CLOSING, -3, -4, -5, -6, -7, -8, -9, -10 },
	/*9 DS_BOTH_CLOSING*/{ -1, -2, -3, -4, DS_SHORT_CLOSING_ONLY, DS_SHORT_CLOSING_HOLD_LONG, -7, -8, DS_LONG_CLOSING_ONLY, DS_LONG_CLOSING_HOLD_SHORT },
	/*10 DS_LONG_CLOSING_ONLY*/{ -1, -2, -3, -4, DS_BOTH_EMPTY, DS_HOLD_LONG, -7, -8, -9, -10 },
	/*11 DS_LONG_CLOSING_HOLD_SHORT*/{ -1, -2, -3, -4, DS_HOLD_SHORT, DS_BOTH_HELD, -7, -8, -9, -10 },
	/*12 DS_SHORT_CLOSING_ONLY*/{ -1, -2, -3, -4, -5, -6, -7, -8, DS_BOTH_EMPTY, DS_HOLD_SHORT },
	/*13 DS_SHORT_CLOSING_HOLD_LONG*/{ -1, -2, -3, -4, -5, -6, -7, -8, DS_HOLD_LONG, DS_BOTH_HELD },
	/*14 DS_ERROR*/{ DS_ERROR, DS_ERROR, DS_ERROR, DS_ERROR, DS_ERROR, DS_ERROR, DS_ERROR, DS_ERROR, DS_ERROR, DS_ERROR }
};

CDualQueueStrategy::CDualQueueStrategy()
	: m_priceTick(0)
	, m_longOrderPlacer(NULL)
	, m_shortOrderPlacer(NULL)
	, m_stableTickThreshold(6)
	, m_minWorkingSize(0)
	, m_stopping(false)
{
}


CDualQueueStrategy::~CDualQueueStrategy()
{
}

void CDualQueueStrategy::Apply(const entity::StrategyItem & strategyItem, CPortfolio * pPortfolio, bool withTriggers)
{
	if (withTriggers)
	{
		logger.Debug("Applying Dual ScalperStrategy...");
	}

	CStrategy::Apply(strategyItem, withTriggers);

	m_priceTick = strategyItem.dq_pricetick();
	m_stableTickThreshold = strategyItem.dq_stabletickthreshold();
	m_minWorkingSize = strategyItem.dq_minworkingsize();

	logger.Debug(
		boost::str(boost::format("Portfolio(%s) DualQueue: PxTick = %.2f, StableTickThreshold = %d, MinWorkingSize = %d")
			% pPortfolio->ID() % m_priceTick % m_stableTickThreshold  % m_minWorkingSize
			));

	m_longSideUserId = strategyItem.ds_longsideuserid();
	AddUserId(m_longSideUserId);
	m_shortSideUserId = strategyItem.ds_shortsideuserid();
	AddUserId(m_shortSideUserId);
}

void CDualQueueStrategy::Apply(const entity::StrategyItem & strategyItem, bool withTriggers)
{
	Apply(strategyItem, false);
}

void CDualQueueStrategy::OnBindedRoutes()
{
	m_longOrderPlacer = dynamic_cast<CDualScalperOrderPlacer*>(GetRoute(m_longSideUserId));
	assert(m_longOrderPlacer != NULL);
	m_longOrderPlacer->SetUserId(m_longSideUserId);
	m_longOrderPlacer->SetPortfolioTradedEventHandler(
		PortfolioTradedEvent(boost::bind(&CDualQueueStrategy::OnLongOrderPlacerDone, this, _1, _2, _3, _4)));

	m_shortOrderPlacer = dynamic_cast<CDualScalperOrderPlacer*>(GetRoute(m_shortSideUserId));
	assert(m_shortOrderPlacer != NULL);
	m_shortOrderPlacer->SetUserId(m_shortSideUserId);
	m_shortOrderPlacer->SetPortfolioTradedEventHandler(
		PortfolioTradedEvent(boost::bind(&CDualQueueStrategy::OnShortOrderPlacerDone, this, _1, _2, _3, _4)));
}

CPortfolioOrderPlacer * CDualQueueStrategy::CreateOrderPlacer()
{
	return new CDualScalperOrderPlacer;
}

bool CDualQueueStrategy::OnStart()
{
	m_stopping = false;

	if (IsRunning())
	{
		return false;
	}

	if (m_longOrderPlacer != NULL)
		m_longOrderPlacer->Prepare();
	if (m_shortOrderPlacer != NULL)
		m_shortOrderPlacer->Prepare();

	SetState(DS_BOTH_EMPTY);

	return true;
}

void CDualQueueStrategy::AlreadyStarted()
{
	m_stopping = false;
}

bool CDualQueueStrategy::StopOnTimepoint()
{
	DualScalperState state = State();
	return state != DS_BOTH_HELD;
}

void CDualQueueStrategy::OnStop()
{
	LOG_DEBUG(logger, "DualScapler - Truly Stopped");
	if (m_longOrderPlacer != NULL)
		m_longOrderPlacer->Cleanup();
	if (m_shortOrderPlacer != NULL)
		m_shortOrderPlacer->Cleanup();

	m_stopping = false;
}

void CDualQueueStrategy::Stop()
{
	LOG_DEBUG(logger, "DualScapler - Stopping");
	m_stopping = true;
}

void CDualQueueStrategy::Test(entity::Quote * pQuote, CPortfolio * pPortfolio, boost::chrono::steady_clock::time_point & timestamp)
{
	if (pQuote->ask() > 0 && pQuote->bid() > 0)
	{
		if (IsRunning() && !IsSuspending())
		{
			DualScalperState state = State();
#ifdef LOG_FOR_TRADE
			LOG_DEBUG(logger, boost::str(boost::format("DualQueue State: %d") % state));
#endif // LOG_FOR_TRADE

			if (m_stopping)
			{
				if (state == DS_BOTH_EMPTY || state == DS_ERROR)
				{
					// 1. Truly Stop Strategy
					CStrategy::Stop();
				}
				else if(state == DS_BOTH_OPENING)
				{
					// 2. Both sides are queuing for opening position -> Cancel Orders
				
				}
				else if(state == DS_BOTH_HELD)
				{
					// 3. Try to close
					ClosePosition(pQuote, timestamp);
				}
				else if(state == DS_BOTH_CLOSING)
				{
					// 4. Both sides are queuing for closing position -> Cancel Orders, Close Position
					ClosePosition(pQuote, timestamp);
				}
			}
			else
			{
				// if price diff meets condition
				if (1/*DoubleGreaterEqual(m_diff, m_diffThreshold)*/)
				{
					if (state == DS_BOTH_EMPTY)
					{
						// 1. Try to open
						OpenPosition(pQuote, timestamp);
					}
					else if (state == DS_BOTH_HELD)
					{
						// 2. Try to close
						ClosePosition(pQuote, timestamp);
					}
				}
			}

			if (state == DS_HOLD_LONG)
			{
				if (m_longOrderPlacer->IsOnPending())
					m_longOrderPlacer->OnQuoteReceived(timestamp, pQuote);
				else if (m_longOrderPlacer->IsOpened())
					LongStopLoss(pQuote, timestamp);
			}
			else if (state == DS_HOLD_SHORT)
			{
				if (m_shortOrderPlacer->IsOnPending())
					m_shortOrderPlacer->OnQuoteReceived(timestamp, pQuote);
				else if (m_shortOrderPlacer->IsOpened())
					ShortStopLoss(pQuote, timestamp);
			}
		}
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

void CDualQueueStrategy::OpenPosition(entity::Quote * pQuote, boost::chrono::steady_clock::time_point & timestamp)
{
	double buyPx = pQuote->bid();
	double sellPx = pQuote->ask();

	double longLmtPrice[2] = { buyPx, 0.0 };
	double shortLmtPrice[2] = { sellPx, 0.0 };

	Transition(EVT_OPENING);
	if (pQuote->bid_size() <= pQuote->ask_size())
	{
		//m_shortOrderPlacer->AsyncRun(entity::SHORT, sellPx, timestamp);
		m_longOrderPlacer->AsyncRun(entity::LONG, buyPx, timestamp);
		m_shortOrderPlacer->Run(entity::SHORT, shortLmtPrice, 2, timestamp);
	}
	else
	{
		//m_longOrderPlacer->AsyncRun(entity::LONG, buyPx, timestamp);
		m_shortOrderPlacer->AsyncRun(entity::SHORT, sellPx, timestamp);
		m_longOrderPlacer->Run(entity::LONG, longLmtPrice, 2, timestamp);
	}

	LOG_DEBUG(logger, boost::str(boost::format("DualQueue - Open position @ B:%.2f - S:%.2f (A:%.2f, B:%.2f, %s %d)")
		% buyPx % sellPx
		% pQuote->ask() % pQuote->bid() % pQuote->update_time() % pQuote->update_millisec()));
}

void CDualQueueStrategy::ClosePosition(entity::Quote * pQuote, boost::chrono::steady_clock::time_point & timestamp)
{
	double buyPx = pQuote->bid();
	double sellPx = pQuote->ask();

	Transition(EVT_CLOSING);
	if (pQuote->bid_size() <= pQuote->ask_size())
	{
		m_longOrderPlacer->CloseOrder(sellPx, true);
		m_shortOrderPlacer->CloseOrder(buyPx, true);
	}
	else
	{
		m_shortOrderPlacer->CloseOrder(buyPx, true);
		m_longOrderPlacer->CloseOrder(sellPx, true);
	}

	LOG_DEBUG(logger, boost::str(boost::format("DualQueue - Close position @ B:%.2f - S:%.2f ((A:%.2f, B:%.2f, %s %d))")
		% buyPx % sellPx
		% pQuote->ask() % pQuote->bid() % pQuote->update_time() % pQuote->update_millisec()));
}

void CDualQueueStrategy::LongStopLoss(entity::Quote * pQuote, boost::chrono::steady_clock::time_point & timestamp) const
{
	double stopLossPx = pQuote->ask(); 
	
	m_longOrderPlacer->CloseOrder(stopLossPx, false);
	LOG_DEBUG(logger, boost::str(boost::format("DualScapler - Long stop loss @ %.2f (A:%.2f, B:%.2f, %s %d)")
		% stopLossPx % pQuote->ask() % pQuote->bid() % pQuote->update_time() % pQuote->update_millisec()));
}

void CDualQueueStrategy::ShortStopLoss(entity::Quote * pQuote, boost::chrono::steady_clock::time_point & timestamp) const
{
	double stopLossPx = pQuote->bid();
	
	m_shortOrderPlacer->CloseOrder(stopLossPx, false);
	LOG_DEBUG(logger, boost::str(boost::format("DualScapler - Short stop loss @ %.2f (A:%.2f, B:%.2f, %s %d)")
		% stopLossPx % pQuote->ask() % pQuote->bid() % pQuote->update_time() % pQuote->update_millisec()));
}

void CDualQueueStrategy::Transition(DualScalperEvent evt)
{
	boost::lock_guard<boost::mutex> l(m_mutFsm);
	DualScalperState current = State();
	int i_next = DQ_TRANSITION_TABLE[current][evt];

	if (i_next > -1)
	{
		SetState(DualScalperState(i_next));
#ifdef LOG_FOR_TRADE
		LOG_DEBUG(logger, boost::str(boost::format("DualQueue Transition Start: %d -> Event: %d -> Next: %d")
			% current % evt % i_next));
#endif // LOG_FOR_TRADE
	}
	else
	{
		SetState(DS_ERROR);
		logger.Error(boost::str(boost::format("DualQueue Transition ERROR --> Unexpected event(%d) for state (%d)")
			% evt % current));
		assert(i_next > -1);
	}
}

void CDualQueueStrategy::OnLegFilled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume)
{
	if (offset == trade::OF_OPEN && direction == trade::BUY && sendingIdx == 0)
	{
		Transition(EVT_LONG_OPENED);
	}
	else if ((offset == trade::OF_CLOSE || offset == trade::OF_CLOSE_TODAY) && direction == trade::SELL && sendingIdx == 1)
	{
		Transition(EVT_LONG_CLOSED);
	}
	else if (offset == trade::OF_OPEN && direction == trade::SELL && sendingIdx == 0)
	{
		Transition(EVT_SHORT_OPENED);
	}
	else if ((offset == trade::OF_CLOSE || offset == trade::OF_CLOSE_TODAY) && direction == trade::BUY && sendingIdx == 1)
	{
		Transition(EVT_SHORT_CLOSED);
	}
}

void CDualQueueStrategy::OnLegCanceled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction)
{
	if (offset == trade::OF_OPEN && direction == trade::BUY && sendingIdx == 0)
	{
		Transition(EVT_LONG_OPEN_CANCELED);
	}
	else if ((offset == trade::OF_CLOSE || offset == trade::OF_CLOSE_TODAY) && direction == trade::SELL && sendingIdx == 1)
	{
		Transition(EVT_LONG_CLOSE_CANCELED);
	}
	else if (offset == trade::OF_OPEN && direction == trade::SELL && sendingIdx == 0)
	{
		Transition(EVT_SHORT_OPEN_CANCELED);
	}
	else if ((offset == trade::OF_CLOSE || offset == trade::OF_CLOSE_TODAY) && direction == trade::BUY && sendingIdx == 1)
	{
		Transition(EVT_SHORT_CLOSE_CANCELED);
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

void CDualQueueStrategy::OnLongOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded)
{
	if (doneState == PortfolioError)
	{
		SetState(DS_ERROR);
		OnStrategyError(m_longOrderPlacer->Portfolio(),
			boost::str(boost::format("%s Óöµ½´íÎó²ßÂÔÍ£Ö¹") % m_longOrderPlacer->UserId()));
	}
}

void CDualQueueStrategy::OnShortOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded)
{
	if (doneState == PortfolioError)
	{
		SetState(DS_ERROR);
		OnStrategyError(m_shortOrderPlacer->Portfolio(),
			boost::str(boost::format("%s Óöµ½´íÎó²ßÂÔÍ£Ö¹") % m_shortOrderPlacer->UserId()));
	}
}
