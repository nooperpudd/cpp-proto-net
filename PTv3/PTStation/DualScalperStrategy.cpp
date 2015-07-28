#include "StdAfx.h"
#include "DualScalperStrategy.h"
#include "ManualOrderPlacer.h"
#include "globalmembers.h"
#include "Portfolio.h"
#include "DoubleCompare.h"
#include "OrderProcessor.h"

int TRANSITION_TABLE[15][10] = { 
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

CDualScalperStrategy::CDualScalperStrategy()
	: m_diffThreshold(0)
	, m_priceTick(0.2)
	, m_openOffset(0.6)
	, m_percentOffset(0.2)
	, m_closeOffset(0.4)
	, m_oppositeCloseThreshold(1)
	, m_longOrderPlacer(NULL)
	, m_shortOrderPlacer(NULL)
	, m_stopping(false)
{
}


CDualScalperStrategy::~CDualScalperStrategy()
{
}

void CDualScalperStrategy::Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers)
{
	if (withTriggers)
	{
		logger.Debug("Applying Dual ScalperStrategy...");
	}

	CStrategy::Apply(strategyItem, withTriggers);

	m_priceTick = strategyItem.ds_pricetick();
	m_diffThreshold = strategyItem.ds_diffthreshold();
	m_openOffset = strategyItem.ds_openoffset();
	m_percentOffset = strategyItem.ds_percentoffset();
	m_closeOffset = strategyItem.ds_closeoffset();
	m_oppositeCloseThreshold = strategyItem.ds_oppositeclosethreshold();

	logger.Debug(
		boost::str(boost::format("Portfolio(%s) DualScalper: PxTick = %.2f, Threshold = %.2f, OpenOffset = %.2f, Percent = %.2f, CloseOffset = %.2f")
		% pPortfolio->ID() % m_priceTick % m_diffThreshold % m_openOffset % m_percentOffset % m_closeOffset
		));

	m_longSideUserId = strategyItem.ds_longsideuserid();
	AddUserId(m_longSideUserId);
	m_shortSideUserId = strategyItem.ds_shortsideuserid();
	AddUserId(m_shortSideUserId);
}

void CDualScalperStrategy::Apply(const entity::StrategyItem& strategyItem, bool withTriggers)
{
	Apply(strategyItem, false);
}

void CDualScalperStrategy::Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	m_diff = pQuote->ask() - pQuote->bid();
	m_ask = pQuote->ask();
	m_askSize = pQuote->ask_size();
	m_bid = pQuote->bid();
	m_bidSize = pQuote->bid_size();

	if (m_askSize > 0 && m_bidSize > 0)
	{
		if (IsRunning() && !IsSuspending())
		{
			DualScalperState state = State();
#ifdef LOG_FOR_TRADE
			LOG_DEBUG(logger, boost::str(boost::format("DualScalper State: %d") % state));
#endif // LOG_FOR_TRADE

			if (m_stopping)
			{
				if (state == DS_BOTH_EMPTY || state == DS_ERROR)
				{
					// 1. Truly Stop Strategy
					CStrategy::Stop();
				}
				else if (state == DS_BOTH_HELD)
				{
					// 2. Try to close
					ClosePosition(pQuote, timestamp);
				}
			}
			else
			{
				// if price diff meets condition
				if (DoubleGreaterEqual(m_diff, m_diffThreshold))
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

void CDualScalperStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem)
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);
	pPortfUpdateItem->set_sc_diff(m_diff);
}

int CDualScalperStrategy::OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol)
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}

CPortfolioOrderPlacer* CDualScalperStrategy::CreateOrderPlacer()
{
	return new CDualScalperOrderPlacer;
}

void CDualScalperStrategy::OnBindedRoutes()
{
	m_longOrderPlacer = dynamic_cast<CDualScalperOrderPlacer*>(GetRoute(m_longSideUserId));
	assert(m_longOrderPlacer != NULL);
	m_longOrderPlacer->SetUserId(m_longSideUserId);
	m_longOrderPlacer->SetPortfolioTradedEventHandler(
		PortfolioTradedEvent(boost::bind(&CDualScalperStrategy::OnLongOrderPlacerDone, this, _1, _2, _3, _4)));

	m_shortOrderPlacer = dynamic_cast<CDualScalperOrderPlacer*>(GetRoute(m_shortSideUserId));
	assert(m_shortOrderPlacer != NULL);
	m_shortOrderPlacer->SetUserId(m_shortSideUserId);
	m_shortOrderPlacer->SetPortfolioTradedEventHandler(
		PortfolioTradedEvent(boost::bind(&CDualScalperStrategy::OnShortOrderPlacerDone, this, _1, _2, _3, _4)));
}

void CDualScalperStrategy::OpenPosition(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	double effectiveOffset = GetOffset(pQuote->ask() - pQuote->bid());
	double buyPx = pQuote->bid() + effectiveOffset;
	double sellPx = pQuote->ask() - effectiveOffset;

	if (buyPx > sellPx)
	{
		LOG_DEBUG(logger, boost::str(boost::format("DualScapler[Open] - Ignore Invalid buy/sell from inappropriate offset(%.2f)") % effectiveOffset));
		return;
	}

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

	LOG_DEBUG(logger, boost::str(boost::format("DualScapler - Open position @ B:%.2f - S:%.2f (A:%.2f, B:%.2f, %s %d)")
		% buyPx % sellPx 
		% pQuote->ask() % pQuote->bid() % pQuote->update_time() % pQuote->update_millisec()));
}

void CDualScalperStrategy::ClosePosition(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	double effectiveOffset = GetOffset(pQuote->ask() - pQuote->bid());
	double buyPx = pQuote->bid() + effectiveOffset;
	double sellPx = pQuote->ask() - effectiveOffset;

	if (buyPx > sellPx)
	{
		LOG_DEBUG(logger, boost::str(boost::format("DualScapler[Close] - Ignore Invalid buy/sell from inappropriate offset(%.2f)") % effectiveOffset));
		return;
	}

	Transition(EVT_CLOSING);
	if (pQuote->bid_size() >= pQuote->ask_size())
	{
		m_longOrderPlacer->CloseOrder(sellPx, true);
		m_shortOrderPlacer->CloseOrder(buyPx, true);
	}
	else
	{
		m_shortOrderPlacer->CloseOrder(buyPx, true);
		m_longOrderPlacer->CloseOrder(sellPx, true);
	}

	LOG_DEBUG(logger, boost::str(boost::format("DualScapler - Close position @ B:%.2f - S:%.2f ((A:%.2f, B:%.2f, %s %d))")
		% buyPx % sellPx 
		% pQuote->ask() % pQuote->bid() % pQuote->update_time() % pQuote->update_millisec()));
}

void CDualScalperStrategy::LongStopLoss(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	double stopLossPx = pQuote->ask() - m_closeOffset;  // Attempt use as big as possible value 
	// if it's still possible to get profit
	double cost = m_longOrderPlacer->CostPrice();
	if (DoubleGreaterEqual(pQuote->ask() - m_closeOffset, cost))
	{
		// do nothing here, value already gets correctly set
	}
	else if (DoubleGreaterEqual(pQuote->ask() - m_priceTick, cost))
	{
		stopLossPx = pQuote->ask() - m_priceTick;
	}
	// no possiblity of profit
	else if (DoubleLessEqual(m_diff, m_oppositeCloseThreshold)) // if price diff is small
	{
		stopLossPx = pQuote->bid(); // set opposite price
	}
	m_longOrderPlacer->CloseOrder(stopLossPx, false);
	LOG_DEBUG(logger, boost::str(boost::format("DualScapler - Long stop loss @ %.2f (A:%.2f, B:%.2f, %s %d)") 
		% stopLossPx % pQuote->ask() % pQuote->bid() % pQuote->update_time() % pQuote->update_millisec()));
}

void CDualScalperStrategy::ShortStopLoss(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	double stopLossPx = pQuote->bid() + m_closeOffset;  // Attempt use as small as possible value 
	// if it's still possible to get profit
	double cost = m_shortOrderPlacer->CostPrice();
	if (DoubleLessEqual(pQuote->bid() + m_closeOffset, cost))
	{
		// do nothing here, value already gets correctly set
	}
	else if (DoubleLessEqual(pQuote->bid() + m_priceTick, cost))
	{
		stopLossPx = pQuote->bid() + m_priceTick;
	}
	else if (DoubleLessEqual(m_diff, m_oppositeCloseThreshold)) // if price diff is small
	{
		stopLossPx = pQuote->ask(); // set opposite price
	}
	m_shortOrderPlacer->CloseOrder(stopLossPx, false);
	LOG_DEBUG(logger, boost::str(boost::format("DualScapler - Short stop loss @ %.2f (A:%.2f, B:%.2f, %s %d)")
		% stopLossPx % pQuote->ask() % pQuote->bid() % pQuote->update_time() % pQuote->update_millisec()));
}

void CDualScalperStrategy::OnLegFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume)
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

void CDualScalperStrategy::OnLegCanceled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction)
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

void CDualScalperStrategy::Transition(DualScalperEvent evt)
{
	boost::lock_guard<boost::mutex> l(m_mutFsm);
	DualScalperState current = State();
	int i_next = TRANSITION_TABLE[current][evt];
	
	if (i_next > -1)
	{
		SetState((DualScalperState)i_next);
#ifdef LOG_FOR_TRADE
		LOG_DEBUG(logger, boost::str(boost::format("DualScalper Transition Start: %d -> Event: %d -> Next: %d") 
			% current % evt % i_next));
#endif // LOG_FOR_TRADE
	}
	else
	{
		SetState(DS_ERROR);
		logger.Error(boost::str(boost::format("Transition ERROR --> Unexpected event(%d) for state (%d)")
			% evt % current));
		assert(i_next > -1);
	}
}

bool CDualScalperStrategy::OnStart()
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

void CDualScalperStrategy::OnStop()
{
	LOG_DEBUG(logger, "DualScapler - Truly Stopped");
	if (m_longOrderPlacer != NULL)
		m_longOrderPlacer->Cleanup();
	if (m_shortOrderPlacer != NULL)
		m_shortOrderPlacer->Cleanup();

	m_stopping = false;
}

void CDualScalperStrategy::Stop()
{
	LOG_DEBUG(logger, "DualScapler - Stopping");
	m_stopping = true;
}

void CDualScalperStrategy::AlreadyStarted()
{
	m_stopping = false;
}

void CDualScalperStrategy::OnLongOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded)
{
	if (doneState == PortfolioError)
	{
		SetState(DS_ERROR);
		OnStrategyError(m_longOrderPlacer->Portfolio(),
			boost::str(boost::format("%s 遇到错误策略停止") % m_longOrderPlacer->UserId()));
	}
}

void CDualScalperStrategy::OnShortOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded)
{
	if (doneState == PortfolioError)
	{
		SetState(DS_ERROR);
		OnStrategyError(m_shortOrderPlacer->Portfolio(),
			boost::str(boost::format("%s 遇到错误策略停止") % m_shortOrderPlacer->UserId()));
	}
}

void CDualScalperStrategy::OnStrategyError(CPortfolio* portf, const string& errorMsg)
{
	if (portf != NULL)
		boost::thread(boost::bind(&CPortfolio::StopStrategyDueTo, portf, errorMsg));
}

double CDualScalperStrategy::GetOffset(double gap)
{
	if (m_percentOffset > 0.001)
	{
		double offset1 = gap * m_percentOffset;
		double offset2 = (int)(offset1 / m_priceTick) * m_priceTick;
		double offset = (offset1 - offset2 - (m_priceTick / 2)) > 0.001 ? offset2 + m_priceTick : offset2;
		offset += m_openOffset;
		return offset;
	}
	else
	{
		return m_openOffset;
	}
}

void CMultiRouteStrategy::BindRoutes(CPortfolio* pPortfolio, OnBindingRouteHandler onBindingRouteHandler)
{
	for (vector<string>::iterator iter = m_userIds.begin(); iter != m_userIds.end(); ++iter)
	{
		const string& userId = (*iter);
		CPortfolioOrderPlacer* orderPlacer = CreateOrderPlacer();
		m_orderPlacerMap.insert(std::make_pair(userId, OrderPlacerPtr(orderPlacer)));
		COrderProcessor* pOrderProcessor = NULL;
		if (!onBindingRouteHandler.empty())
			onBindingRouteHandler(userId, &pOrderProcessor);
		orderPlacer->Initialize(pPortfolio, pOrderProcessor);
	
		
		if (pOrderProcessor == NULL)	// Must something wrong
		{
			logger.Error(boost::str(boost::format("Portfolio(%s) CANNOT bind OrderProcessor for userid(%s)") 
				% pPortfolio->ID() % userId ));
		}
		else
		{
			const string& ordProcUserId = pOrderProcessor->UserId();
			LOG_DEBUG(logger, boost::str(boost::format("Portfolio(%s) initialize an OrderPlacer with OrderProcessor(%s)") 
				% pPortfolio->ID() % ordProcUserId));
		}
	}

	OnBindedRoutes();
}

CPortfolioOrderPlacer* CMultiRouteStrategy::GetRoute(const string& userId)
{
	boost::unordered_map<string, OrderPlacerPtr>::iterator iter = m_orderPlacerMap.find(userId);
	if (iter != m_orderPlacerMap.end())
	{
		return (iter->second).get();
	}

	return NULL;
}
