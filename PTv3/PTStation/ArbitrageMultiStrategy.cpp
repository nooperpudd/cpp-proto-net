#include "StdAfx.h"
#include "ArbitrageMultiStrategy.h"
#include "PriceBarDataProxy.h"
#include "Portfolio.h"
#include "PortfolioArbitrageOrderPlacer.h"
#include "DoubleCompare.h"
#include "globalmembers.h"

CArbitrageMultiStrategy::CArbitrageMultiStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio)
	: CMultiOpenStrategy(strategyItem, pAvatar, pPortfolio)
	, m_timeFrame(60)
	, m_bollPeriod(26)
	, m_stdDevMultiplier(2)
	, m_targetGain(0.8)
	, m_minStep(0.2)
	, m_useTargetGain(true)
	, m_allowPending(true)
	, m_stopLossType(entity::STOP_LOSS_Disabled)
{
}

CArbitrageMultiStrategy::~CArbitrageMultiStrategy()
{
}

void CArbitrageMultiStrategy::OnApply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers)
{
	GetMinPriceChange(pPortfolio);

	m_bollPeriod = (int)strategyItem.ar_bollperiod();
	m_stdDevMultiplier = (int)strategyItem.ar_stddevmultiplier();
	m_openTimeout = strategyItem.opentimeout();
	m_retryTimes = strategyItem.retrytimes();
	m_useTargetGain = strategyItem.ar_usetargetgain();
	m_targetGainTimes = strategyItem.ar_targetgain();
	m_stopLossType = strategyItem.ar_stoplosstype();
	m_targetGain = m_targetGainTimes * m_minStep;
	assert(m_targetGain > 0);

	if (!withTriggers)
	{
		// don't touch hist data source when editing strategy
		PrepareHistDataSrc(strategyItem);

		// Initialize Indicator set
		const vector<CPriceBarDataProxy*>& dataProxies = DataProxies();
		if (dataProxies.size() > 1)
		{
			m_diffRecordSet = DiffRecordSetPtr(
				new CDiffRecordSet(
				dataProxies[0]->GetOHLCRecordSet(),
				dataProxies[1]->GetOHLCRecordSet()
				));

			int histDataSize = dataProxies[0]->GetRecordSetSize();
			m_bollDataSet = BollDataSetPtr(new CBollDataSet(histDataSize, m_bollPeriod, m_stdDevMultiplier));

			CalculateEndBar(1, m_timeFrame, histDataSize);
		}
	}
}

void CArbitrageMultiStrategy::GetMinPriceChange(CPortfolio* pPortfolio)
{
	assert(pPortfolio != NULL);
	if (pPortfolio->Count() > 1)
	{
		m_minStep = pPortfolio->GetLeg(1)->MinPriceChange();
	}
}

bool CArbitrageMultiStrategy::Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context, boost::chrono::steady_clock::time_point& timestamp)
{
	bool prerequisiteOK = CMultiOpenStrategy::Prerequisite(pQuote, pPortfolio, context, timestamp);
	if (prerequisiteOK)
	{
		string symbol = pQuote->symbol();
		COHLCRecordSet* ohlc = GetRecordSet(symbol, m_timeFrame, timestamp);
		if (ohlc == NULL)
			return false;

		context.CurrentIndex = m_diffRecordSet->Calculate(ohlc);
		if (context.CurrentIndex < m_bollPeriod)
			return false;

		if (!pPortfolio->LegsTimestampAligned())
			return false;
	}

	return prerequisiteOK;
}

void CArbitrageMultiStrategy::CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	CALC_DIFF_METHOD calcMethod = m_allowPending ? BETTER_PRICE : FAST_DEAL;

	m_context.LastDiff = pPortfolio->CalculateDiff(&m_context.StructLastDiff, calcMethod);
	m_context.LongDiff = pPortfolio->CalculateDiff(&m_context.StructLongDiff, calcMethod);
	m_context.ShortDiff = pPortfolio->CalculateDiff(&m_context.StructShortDiff, calcMethod);

	m_context.LongDiffFast = pPortfolio->CalculateDiff(&m_context.StructLongDiffFast, FAST_DEAL);
	m_context.ShortDiffFast = pPortfolio->CalculateDiff(&m_context.StructShortDiffFast, FAST_DEAL);

	// TODO: will be figured out in the future
	m_context.LongDiffSize = 0;
	m_context.ShortDiffSize = 0;

	m_bollDataSet->Calculate(m_diffRecordSet.get(), pPortfolio);

	m_context.BollMid = m_bollDataSet->GetRef(IND_MID, 1);
	if (m_useTargetGain)
	{
		m_context.ActualMid = CalcBoundaryByTargetGain(m_context.BollMid, m_targetGain, m_minStep, &m_context.BollTop, &m_context.BollBottom);
	}
	else
	{
		m_context.BollTop = m_bollDataSet->GetRef(IND_TOP, 1);
		m_context.BollBottom = m_bollDataSet->GetRef(IND_BOTTOM, 1);
	}
}

double CArbitrageMultiStrategy::GetLowerValue(double val, double offset)
{
	int iVal = (int)(val * 10);
	int iOffset = (int)(offset * 10);
	int factor = iVal / iOffset;
	double ret = (double)(iOffset * factor) / 10.0;
	return ret;
}

double CArbitrageMultiStrategy::CalcBoundaryByTargetGain(double mid, double targetGain, double step, double* outUpper, double* outLower)
{
	double halfGain = targetGain / 2;
	double actualMid = GetLowerValue(mid, step);
	*outUpper = actualMid + halfGain;
	*outLower = actualMid - halfGain;

	return actualMid;
}

StrategyExecutorPtr CArbitrageMultiStrategy::CreateExecutor(int execId, int quantity)
{
	return StrategyExecutorPtr(new CArbitrageStrategyExecutor(execId, quantity, this));
}

void CArbitrageMultiStrategy::BeforeTestForTrade(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context)
{
	ArbitrageStrategyContext& arbitrageContext = dynamic_cast<ArbitrageStrategyContext&>(context);
	entity::PosiDirectionType direction = GetTradeDirection(arbitrageContext);
	entity::PosiDirectionType directionFast = GetFastTradeDirection(arbitrageContext);

	if (m_useTargetGain)
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Testing Direction - Mid:%.2f, longDiff:%.2f vs bottom:%.2f, shortDiff:%.2f vs top:%.2f -->> %s")
			% pPortfolio->InvestorId() % pPortfolio->ID() % arbitrageContext.ActualMid % arbitrageContext.LongDiff % arbitrageContext.BollBottom % arbitrageContext.ShortDiff % arbitrageContext.BollTop % GetPosiDirectionText(direction)));
	}
	else
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Testing trade direction - longDiff:%.2f vs bottom:%.2f, shortDiff:%.2f vs top:%.2f -->> %s")
			% pPortfolio->InvestorId() % pPortfolio->ID() % arbitrageContext.LongDiff % arbitrageContext.BollBottom % arbitrageContext.ShortDiff % arbitrageContext.BollTop % GetPosiDirectionText(direction)));
	}

	if (directionFast != entity::NET)
	{
		if (m_useTargetGain)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Testing Fast Direction - Mid:%.2f, longDiffFast:%.2f vs bottom:%.2f, shortDiffFast:%.2f vs top:%.2f -->> %s")
				% pPortfolio->InvestorId() % pPortfolio->ID() % arbitrageContext.ActualMid % arbitrageContext.LongDiffFast % arbitrageContext.BollBottom % arbitrageContext.ShortDiffFast % arbitrageContext.BollTop % GetPosiDirectionText(directionFast)));
		}
		else
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Testing Fast direction - longDiffFast:%.2f vs bottom:%.2f, shortDiffFast:%.2f vs top:%.2f -->> %s")
				% pPortfolio->InvestorId() % pPortfolio->ID() % arbitrageContext.LongDiffFast % arbitrageContext.BollBottom % arbitrageContext.ShortDiffFast % arbitrageContext.BollTop % GetPosiDirectionText(directionFast)));
		}
	}
}

entity::PosiDirectionType CArbitrageMultiStrategy::GetTradeDirection(ArbitrageStrategyContext& context)
{
	entity::PosiDirectionType direction = entity::NET;

	if (DoubleLessEqual(context.LongDiff, context.BollBottom))
		direction = entity::LONG;
	else if (DoubleGreaterEqual(context.ShortDiff, context.BollTop))
		direction = entity::SHORT;
	/*
	if(m_longDiff < m_bollBottom)
	direction = entity::LONG;
	else if(m_shortDiff > m_bollTop)
	direction = entity::SHORT;
	*/
	return direction;
}

entity::PosiDirectionType CArbitrageMultiStrategy::GetFastTradeDirection(ArbitrageStrategyContext& context)
{
	entity::PosiDirectionType direction = entity::NET;

	if (context.LongDiffFast < context.BollBottom)
		direction = entity::LONG;
	else if (context.ShortDiffFast > context.BollTop)
		direction = entity::SHORT;
	return direction;
}

void CArbitrageStrategyExecutor::OnWorking(entity::Quote* pQuote, StrategyContext* pContext)
{

}

bool CArbitrageStrategyExecutor::TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	ArbitrageStrategyContext* arbitrageContext = dynamic_cast<ArbitrageStrategyContext*>(pContext);
	if (arbitrageContext->DirectionFast > entity::NET)
	{
		entity::PosiDirectionType direction = arbitrageContext->DirectionFast;

		if (m_pParentStrategy->NotOpenInStopLossDirection()
			&& arbitrageContext->DirectionFast == arbitrageContext->LastStopLossDirection)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) does NOT open position same as last stop loss direction (%s)")
				% pPortfolio->InvestorId() % pPortfolio->ID() % GetPosiDirectionText(arbitrageContext->LastStopLossDirection)));
			return false;
		}
		
		//m_closePositionPurpose = CLOSE_POSITION_UNKNOWN;
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) FAST DEAL Opening position at bar %d")
			% pPortfolio->InvestorId() % pPortfolio->ID() % pContext->CurrentIndex));
		pPortfolio->PrintLegsQuote();

		OpenPosition(direction, (direction == entity::LONG ? arbitrageContext->StructLongDiffFast : arbitrageContext->StructShortDiffFast),
			arbitrageContext, pQuote, timestamp);

		return true;
	
	}
	else if (arbitrageContext->Direction > entity::NET)
	{
		entity::PosiDirectionType direction = arbitrageContext->Direction;

		if (m_pParentStrategy->NotOpenInStopLossDirection()
			&& arbitrageContext->DirectionFast == arbitrageContext->LastStopLossDirection)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) does NOT open position same as last stop loss direction (%s)")
				% pPortfolio->InvestorId() % pPortfolio->ID() % GetPosiDirectionText(arbitrageContext->LastStopLossDirection)));
			return false;
		}

		//m_closePositionPurpose = CLOSE_POSITION_UNKNOWN;
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Opening position at bar %d")
			% pPortfolio->InvestorId() % pPortfolio->ID() % pContext->CurrentIndex));
		pPortfolio->PrintLegsQuote();
		OpenPosition(direction, (direction == entity::LONG ? arbitrageContext->StructLongDiff : arbitrageContext->StructShortDiff),
			arbitrageContext, pQuote, timestamp);
		
		return true;
	}
	
	return false;
}

bool CArbitrageStrategyExecutor::TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	ArbitrageStrategyContext* arbitrageContext = dynamic_cast<ArbitrageStrategyContext*>(pContext);
	entity::PosiDirectionType side = PosiDirection();
	ARBI_DIFF_CALC& diffPrices = side == entity::LONG ? arbitrageContext->StructShortDiff : arbitrageContext->StructLongDiff;

	// Stop gain/loss logic in ArbitrageStrategy
	if (arbitrageContext->DirectionFast != entity::NET && side != arbitrageContext->DirectionFast)
	{
		diffPrices = side == entity::LONG ? arbitrageContext->StructShortDiffFast : arbitrageContext->StructLongDiffFast;

		if (side == entity::LONG)
		{
			// Fast Stop Gain
			//m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
			string logTxt = boost::str(boost::format("Fast StopGain: Fast Short diff(%.2f) above bollTop(%.2f) -> Stop Gain") 
				% arbitrageContext->ShortDiffFast % arbitrageContext->BollTop);
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();
			string comment = boost::str(boost::format("对价空价差(%.2f)大于上轨(%.2f) -> 止盈平仓") 
				% arbitrageContext->ShortDiffFast % arbitrageContext->BollTop);
			return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
		}
		else if (side == entity::SHORT)
		{
			// Fast Stop Gain
			//m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
			string logTxt = boost::str(boost::format("Fast StopGain: Fast Long diff(%.2f) below bollBottom(%.2f) -> Stop Gain") 
				% arbitrageContext->LongDiffFast % arbitrageContext->BollBottom);
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();
			string comment = boost::str(boost::format("对价多价差(%.2f)小于下轨(%.2f) -> 止盈平仓") 
				% arbitrageContext->LongDiffFast % arbitrageContext->BollBottom);
			return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
		}
	}
	else if (arbitrageContext->Direction != entity::NET)
	{
		if (side == entity::LONG)
		{
			if (arbitrageContext->BollTop <= m_costDiff)
			{
				// Stop Loss
				//m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
				string logTxt = boost::str(boost::format("bollTop(%.2f) <= costDiff(%.2f) -> Stop Loss") 
					% arbitrageContext->BollTop % m_costDiff);
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("上轨(%.2f)低于等于成本(%.2f) -> 止损平仓") 
					% arbitrageContext->BollTop % m_costDiff);
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopLoss);
			}
			else if (side != arbitrageContext->Direction)
			{
				// Stop Gain
				//m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
				string logTxt = boost::str(boost::format("Short diff(%.2f) above bollTop(%.2f) -> Stop Gain") 
					% arbitrageContext->ShortDiff % arbitrageContext->BollTop);
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("空价差(%.2f)大于上轨(%.2f) -> 止盈平仓") 
					% arbitrageContext->ShortDiff % arbitrageContext->BollTop);
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
			}
		}
		else if (side == entity::SHORT)
		{
			if (arbitrageContext->BollBottom >= m_costDiff)
			{
				// Stop Loss
				//m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
				string logTxt = boost::str(boost::format("bollBottom(%.2f) >= costDiff(%.2f) -> Stop Loss") 
					% arbitrageContext->BollBottom % m_costDiff);
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("下轨(%.2f)大于等于成本(%.2f) -> 止损平仓") 
					% arbitrageContext->BollBottom % m_costDiff);
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopLoss);
			}
			else if (side != arbitrageContext->Direction)
			{
				// Stop Gain
				//m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
				string logTxt = boost::str(boost::format("Long diff(%.2f) below bollBottom(%.2f) -> Stop Gain") 
					% arbitrageContext->LongDiff % arbitrageContext->BollBottom);
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("多价差(%.2f)小于下轨(%.2f) -> 止盈平仓") 
					% arbitrageContext->LongDiff % arbitrageContext->BollBottom);
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
			}
		}
	}

	return false;
}

void CArbitrageStrategyExecutor::InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc, PortfolioTradedEvent porfTradedEventHandler)
{
	m_orderPlacer = OrderPlacerPtr(new CPortfolioArbitrageOrderPlacer(m_execId));
	if (m_orderPlacer.get() != NULL)
	{
		m_orderPlacer->Initialize(pPortf, pOrderProc);
		m_orderPlacer->SetPortfolioTradedEventHandler(porfTradedEventHandler);
	}
}

void CArbitrageStrategyExecutor::OpenPosition(entity::PosiDirectionType direction, ARBI_DIFF_CALC diffPrices, StrategyContext* pContext, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	if (direction > entity::NET)
	{
		double lmtPrice[2];
		if (direction == entity::LONG)
		{
			lmtPrice[0] = diffPrices.BuyPrice;
			lmtPrice[1] = diffPrices.SellPrice;
		}
		else if (direction == entity::SHORT)
		{
			lmtPrice[0] = diffPrices.SellPrice;
			lmtPrice[1] = diffPrices.BuyPrice;
		}

		LOG_DEBUG(logger, boost::str(boost::format("Arbitrage Trend - %s Open position @ %.2f - %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % lmtPrice[1] % pQuote->update_time()));

		CPortfolioArbitrageOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioArbitrageOrderPlacer*>(m_orderPlacer.get());
		ArbitrageStrategyContext* arbiContext = dynamic_cast<ArbitrageStrategyContext*>(pContext);

		string openComment;
		if (direction == entity::LONG)
		{
			openComment = boost::str(boost::format("多价差 %.2f 低于下轨 %.2f 做多 (%s)") 
				% diffPrices.Diff % arbiContext->BollBottom % pQuote->update_time());
		}
		else if (direction == entity::SHORT)
		{
			openComment = boost::str(boost::format("空价差 %.2f 高于上轨 %.2f 做空 (%s)") 
				% diffPrices.Diff % arbiContext->BollTop % pQuote->update_time());
		}
		// TODO feed comment
		pOrderPlacer->SetMlOrderStatus(openComment);
		pOrderPlacer->OpenPosition(direction, lmtPrice, 2, timestamp, trade::SR_AutoOpen);
		m_costDiff = diffPrices.Diff;
		// todo resetforceopen
	}

}

bool CArbitrageStrategyExecutor::ClosePosition(ARBI_DIFF_CALC diffPrices, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment, trade::SubmitReason reason)
{
	entity::PosiDirectionType direction = PosiDirection();
	if (direction != entity::NET)
	{
		double lmtPrice[2];
		if (direction == entity::LONG)
		{
			lmtPrice[0] = diffPrices.SellPrice;
			lmtPrice[1] = diffPrices.BuyPrice;
		}
		else if (direction == entity::SHORT)
		{
			lmtPrice[0] = diffPrices.BuyPrice;
			lmtPrice[1] = diffPrices.SellPrice;
		}

		LOG_DEBUG(logger, boost::str(boost::format("Arbitrage Trend - %s Close position @ %.2f - %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % lmtPrice[1] % pQuote->update_time()));

		CPortfolioArbitrageOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioArbitrageOrderPlacer*>(m_orderPlacer.get());
		//pOrderPlacer->ClosePosition(m_volumeToClose, direction, lmtPrice, 2, timestamp, reason);

		//ResetForceClose();
		pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s 平仓 @ %.2f - %.2f")
			% comment % GetPosiDirectionText(direction, true) % lmtPrice[0] % lmtPrice[1]));

		return true;
	}

	return false;
}
