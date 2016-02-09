#include "stdafx.h"
#include "DualQueueStrategy.h"
#include "PortfolioScalperOrderPlacer.h"
#include "globalmembers.h"
#include "Portfolio.h"
#include "DoubleCompare.h"
#include "TechAnalyStrategy.h"

CDualQueueStrategy::CDualQueueStrategy()
	: m_priceTick(0)
	, m_stableTickThreshold(6)
	, m_minWorkingSize(0)
	, m_stopping(false)
	, m_lastAsk(0)
	, m_lastBid(0)
	, m_stableQuoteCount(0)
	, m_stableQuote(false)
	, m_status(DQ_UNOPENED)
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
	return new CPortfolioQueueOrderPlacer();
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
		CPortfolioQueueOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioQueueOrderPlacer*>(pPortfolio->OrderPlacer());

		m_stableQuote = IfQuotingStable(pQuote);
		LOG_DEBUG(logger, boost::str(boost::format("DualQueue - Quote %s Stable. Stable Quote Count:%d") 
			% (m_stableQuote ? "IS" : "IS NOT") % m_stableQuoteCount));

		if (IsRunning() && !IsSuspending())
		{
			if (m_stopping)
			{
				// Cancel order in pending state 

				// If own position, closing position
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
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);
	pPortfUpdateItem->set_dq_stablequote(m_stableQuote);
	pPortfUpdateItem->set_dq_status(static_cast<entity::LegStatus>(m_status.load()));
}


void CDualQueueStrategy::OpenPosition(CPortfolioQueueOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening)
{
	if (m_direction > entity::NET)
	{
		double lmtPrice[2];
		if (m_direction == entity::LONG)
		{
			lmtPrice[0] = pQuote->bid();
			lmtPrice[1] = pQuote->ask();
		}
		else if (m_direction == entity::SHORT)
		{
			lmtPrice[0] = pQuote->ask();
			lmtPrice[1] = pQuote->bid();
		}

		LOG_DEBUG(logger, boost::str(boost::format("Double Queue - %s Open position @ %.2f (%s)")
			% GetPosiDirectionText(m_direction) % lmtPrice[0] % pQuote->update_time()));

		string openComment = boost::str(boost::format("ÅÅ¶Ó %s ¿ª²Ö @ %.2f") % GetPosiDirectionText(m_direction) % lmtPrice[0]);

		pOrderPlacer->SetMlOrderStatus(openComment);
		m_status = DQ_IS_OPENING;
		pOrderPlacer->QueueOrder(m_direction, lmtPrice[0], lmtPrice[1], timestamp);
		ResetForceOpen();
	}
}

void CDualQueueStrategy::OnLegFilled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume)
{
	// if opened position, close position in the other side
	//ClosePosition();
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


