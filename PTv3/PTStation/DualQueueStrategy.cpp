#include "stdafx.h"
#include "DualQueueStrategy.h"
#include "ManualOrderPlacer.h"
#include "globalmembers.h"
#include "Portfolio.h"
#include "DoubleCompare.h"

CDualQueueStrategy::CDualQueueStrategy()
	: m_priceTick(0)
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
}

void CDualQueueStrategy::Apply(const entity::StrategyItem & strategyItem, bool withTriggers)
{
	Apply(strategyItem, false);
}

CPortfolioOrderPlacer * CDualQueueStrategy::CreateOrderPlacer()
{
	return new CManualOrderPlacer;
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
	if (pQuote->ask() > 0 && pQuote->bid() > 0)
	{
		if (IsRunning() && !IsSuspending())
		{
			if (m_stopping)
			{
				// Cancel order in pending state 

				// If own position, closing position
			}
			else
			{
				// Open position if empty

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

void CDualQueueStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem * pPortfUpdateItem)
{
}

void CDualQueueStrategy::OpenPosition(entity::Quote * pQuote, boost::chrono::steady_clock::time_point & timestamp)
{
	double buyPx = pQuote->bid();
	double sellPx = pQuote->ask();

	double longLmtPrice[2] = { buyPx, 0.0 };
	double shortLmtPrice[2] = { sellPx, 0.0 };

	if (pQuote->bid_size() <= pQuote->ask_size())
	{
		//m_shortOrderPlacer->AsyncRun(entity::SHORT, sellPx, timestamp);
	}
	else
	{
		//m_longOrderPlacer->AsyncRun(entity::LONG, buyPx, timestamp);
	}

	LOG_DEBUG(logger, boost::str(boost::format("DualQueue - Open position @ B:%.2f - S:%.2f (A:%.2f, B:%.2f, %s %d)")
		% buyPx % sellPx
		% pQuote->ask() % pQuote->bid() % pQuote->update_time() % pQuote->update_millisec()));
}

void CDualQueueStrategy::ClosePosition(entity::Quote * pQuote, boost::chrono::steady_clock::time_point & timestamp)
{
	double buyPx = pQuote->bid();
	double sellPx = pQuote->ask();

	if (pQuote->bid_size() <= pQuote->ask_size())
	{
	}
	else
	{
	}

	LOG_DEBUG(logger, boost::str(boost::format("DualQueue - Close position @ B:%.2f - S:%.2f ((A:%.2f, B:%.2f, %s %d))")
		% buyPx % sellPx
		% pQuote->ask() % pQuote->bid() % pQuote->update_time() % pQuote->update_millisec()));
}

void CDualQueueStrategy::OnLegFilled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume)
{
	// if opened position, close position in the other side
}

void CDualQueueStrategy::OnLegCanceled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction)
{
	
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


