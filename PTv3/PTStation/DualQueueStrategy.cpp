#include "stdafx.h"
#include "DualQueueStrategy.h"
#include "ManualOrderPlacer.h"
#include "globalmembers.h"
#include "Portfolio.h"
#include "DoubleCompare.h"
#include "OrderProcessor.h"


CDualQueueStrategy::CDualQueueStrategy()
	: m_priceTick(0)
	, m_longOrderPlacer(NULL)
	, m_shortOrderPlacer(NULL)
	, m_stopping(false)
	, m_stableTickThreshold(6)
	, m_minSize(0)
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

	//m_priceTick = strategyItem.dq_pricetick();
	//m_stableTickThreshold = strategyItem.dq_stabletickthreshold();

	logger.Debug(
		boost::str(boost::format("Portfolio(%s) DualQueue: PxTick = %.2f, StableTickThreshold = %d")
			% pPortfolio->ID() % m_priceTick % m_stableTickThreshold 
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

void CDualQueueStrategy::Stop()
{
}

void CDualQueueStrategy::AlreadyStarted()
{
}

void CDualQueueStrategy::Test(entity::Quote * pQuote, CPortfolio * pPortfolio, boost::chrono::steady_clock::time_point & timestamp)
{
}

void CDualQueueStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem * pPortfUpdateItem)
{
}

void CDualQueueStrategy::OnLegFilled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume)
{
}

void CDualQueueStrategy::OnLegCanceled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction)
{
}

int CDualQueueStrategy::OnPortfolioAddPosition(CPortfolio * pPortfolio, const trade::MultiLegOrder & openOrder, int actualTradedVol)
{
	return 0;
}

void CDualQueueStrategy::OnStrategyError(CPortfolio * portf, const string & errorMsg)
{
}

bool CDualQueueStrategy::StopOnTimepoint()
{
	return false;
}

void CDualQueueStrategy::OnBindedRoutes()
{
}

CPortfolioOrderPlacer * CDualQueueStrategy::CreateOrderPlacer()
{
	return nullptr;
}

bool CDualQueueStrategy::OnStart()
{
	return false;
}

void CDualQueueStrategy::OnStop()
{
}

void CDualQueueStrategy::Transition(DualScalperEvent evt)
{
}

void CDualQueueStrategy::OpenPosition(entity::Quote * pQuote, boost::chrono::steady_clock::time_point & timestamp)
{
}

void CDualQueueStrategy::ClosePosition(entity::Quote * pQuote, boost::chrono::steady_clock::time_point & timestamp)
{
}

void CDualQueueStrategy::LongStopLoss(entity::Quote * pQuote, boost::chrono::steady_clock::time_point & timestamp)
{
}

void CDualQueueStrategy::ShortStopLoss(entity::Quote * pQuote, boost::chrono::steady_clock::time_point & timestamp)
{
}

void CDualQueueStrategy::OnLongOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded)
{
}

void CDualQueueStrategy::OnShortOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded)
{
}

double CDualQueueStrategy::GetOffset(double gap)
{
	return 0.0;
}
