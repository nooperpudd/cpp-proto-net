#include "StdAfx.h"
#include "ArbitrageMultiStrategy.h"
#include "PriceBarDataProxy.h"
#include "Portfolio.h"
#include "PortfolioArbitrageOrderPlacer.h"
#include "DoubleCompare.h"
#include "globalmembers.h"

CArbitrageMultiStrategy::CArbitrageMultiStrategy(CAvatarClient* pAvatar, CPortfolio* pPortfolio)
	: CMultiOpenStrategy(pAvatar, pPortfolio)
	, m_timeFrame(60)
	, m_bollPeriod(26)
	, m_stdDevMultiplier(2)
	, m_targetGain(0.8)
	, m_minStep(0.2)
	, m_specifyBandRange(false)
	, m_bandRange(0.8)
	, m_useTargetGain(true)
	, m_absoluteGain(false)
	, m_allowPending(true)
	, m_stopLossType(entity::STOP_LOSS_Disabled)
	, m_stopLossThreshold(5)
	, m_stopLossComparison(entity::GREATER_EQUAL_THAN)
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

	if (m_openTimeout == 0)
		m_openTimeout = 350;
	if (m_retryTimes == 0)
		m_retryTimes = 8;

	m_specifyBandRange = strategyItem.ar_specifybandrange();
	m_bandRangeTimes = strategyItem.ar_bandrange();
	if (m_bandRangeTimes < 1)
		m_bandRangeTimes = 4;
	m_useTargetGain = strategyItem.ar_usetargetgain();
	m_absoluteGain = strategyItem.ar_absolutegain();
	m_targetGainTimes = strategyItem.ar_targetgain();
	if (m_targetGainTimes < 1)
		m_targetGainTimes = 4;
	m_stopLossType = strategyItem.ar_stoplosstype();
	m_stopLossThreshold = strategyItem.ar_stoplossthreshold();
	m_stopLossComparison = strategyItem.ar_stoplosscondition();
	m_targetGain = m_targetGainTimes * m_minStep;
	m_bandRange = m_bandRangeTimes * m_minStep;
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

void CArbitrageMultiStrategy::OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg)
{
	if (pHistSrcCfg != NULL)
	{
		if (pHistSrcCfg->Precision == m_timeFrame)
			pHistSrcCfg->HistData = true;
	}
}

void CArbitrageMultiStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem)
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_ar_diff(m_context.LastDiff);
	pPortfUpdateItem->set_ar_longdiff(m_context.LongDiff);
	pPortfUpdateItem->set_ar_longsize(m_context.LongDiffSize);
	pPortfUpdateItem->set_ar_shortdiff(m_context.ShortDiff);
	pPortfUpdateItem->set_ar_shortsize(m_context.ShortDiffSize);

	pPortfUpdateItem->set_ar_bolltop(m_context.BollTop);
	pPortfUpdateItem->set_ar_bollbottom(m_context.BollBottom);
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
		int nbElems = m_diffRecordSet->NbElements();
		if (nbElems < m_bollPeriod)
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
	if (m_specifyBandRange)
	{
		m_context.ActualMid = CalcBoundaryByTargetGain(m_context.BollMid, m_bandRange, m_minStep, &m_context.BollTop, &m_context.BollBottom);
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

	if (m_specifyBandRange)
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
		if (m_specifyBandRange)
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

	arbitrageContext.Direction = direction;
	arbitrageContext.DirectionFast = directionFast;
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

void CArbitrageStrategyExecutor::OnWorking(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	
	CPortfolio* pPortfolio = m_orderPlacer->Portfolio();
	LOG_DEBUG(logger, boost::str(boost::format("[%s - %d] Arbitrage Strategy - Check and likely retry submit order %s:%.2f") 
		% (pPortfolio != NULL ? pPortfolio->InvestorId() : "") % m_execId % pQuote->symbol() % pQuote->last()));
	m_orderPlacer->OnQuoteReceived(timestamp, pQuote);
}

bool CArbitrageStrategyExecutor::TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	ArbitrageStrategyContext* arbitrageContext = dynamic_cast<ArbitrageStrategyContext*>(pContext);
	if (arbitrageContext->DirectionFast > entity::NET)
	{
		entity::PosiDirectionType direction = arbitrageContext->DirectionFast;

		if (m_pParentStrategy->NotOpenInStopLossDirection()
			&& direction == arbitrageContext->LastStopLossDirection)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) does NOT open position same as last stop loss direction (%s)")
				% pPortfolio->InvestorId() % pPortfolio->ID() % GetPosiDirectionText(arbitrageContext->LastStopLossDirection)));
			return false;
		}
		
		m_closePositionPurpose = CLOSE_POSITION_UNKNOWN;
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
			&& direction == arbitrageContext->LastStopLossDirection)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) does NOT open position same as last stop loss direction (%s)")
				% pPortfolio->InvestorId() % pPortfolio->ID() % GetPosiDirectionText(arbitrageContext->LastStopLossDirection)));
			return false;
		}

		m_closePositionPurpose = CLOSE_POSITION_UNKNOWN;
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

	// Check if force closing
	bool forceClosing = IsForceClosing();
	if (forceClosing)
	{
		if(State() == HOLD_POSITION)
		{
			// Fast Stop Gain
			m_closePositionPurpose = CLOSE_POSITION_FORCE;
			string logTxt = boost::str(boost::format("Manual Close Portfolio(%s) - Executor (%d) at %s")
				% pPortfolio->ID() % ExecId() % pQuote->update_time());
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();
			string comment = "强制平仓";
			return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_Manual);
		}
		else
		{
			ResetForceClose();	// ignore and reset unexpected force closing
		}
		return false;
	}

	if (m_pParentStrategy->UseTargetGain())
	{
		return m_pParentStrategy->AbsoluteGain() ?
			TestForCloseUseTargetGainToCost(pQuote, pPortfolio, pContext, timestamp) :
			TestForCloseUseTargetGainToBoundary(pQuote, pPortfolio, pContext, timestamp);
	}
	
	// Stop gain/loss logic in ArbitrageStrategy
	if (arbitrageContext->DirectionFast != entity::NET && side != arbitrageContext->DirectionFast)
	{
		diffPrices = side == entity::LONG ? arbitrageContext->StructShortDiffFast : arbitrageContext->StructLongDiffFast;

		if (side == entity::LONG)
		{
			// Fast Stop Gain
			m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
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
			m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
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
			string comment;
			if (side != arbitrageContext->Direction)
			{
				// Stop Gain
				m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
				string logTxt = boost::str(boost::format("Short diff(%.2f) above bollTop(%.2f) -> Stop Gain")
					% arbitrageContext->ShortDiff % arbitrageContext->BollTop);
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("空价差(%.2f)大于上轨(%.2f) -> 止盈平仓")
					% arbitrageContext->ShortDiff % arbitrageContext->BollTop);
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
			}
			else if (StopLossLong(arbitrageContext, &comment))
			{
				pPortfolio->PrintLegsQuote();
				// Stop Loss
				m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopLoss);
			}
		}
		else if (side == entity::SHORT)
		{
			string comment;
			if (side != arbitrageContext->Direction)
			{
				// Stop Gain
				m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
				string logTxt = boost::str(boost::format("Long diff(%.2f) below bollBottom(%.2f) -> Stop Gain") 
					% arbitrageContext->LongDiff % arbitrageContext->BollBottom);
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("多价差(%.2f)小于下轨(%.2f) -> 止盈平仓") 
					% arbitrageContext->LongDiff % arbitrageContext->BollBottom);
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
			}
			else if (StopLossShort(arbitrageContext, &comment))
			{
				pPortfolio->PrintLegsQuote();
				// Stop Loss
				m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopLoss);
			}
		}
	}

	return false;
}

entity::PosiDirectionType GetStopGainDirectionOnOffset(double targetGain, entity::PosiDirectionType posiDirection, ArbitrageStrategyContext* arbitrageContext, bool* fastDeal)
{

	*fastDeal = false;
	double targetThreshold = 0;
	if (posiDirection == entity::LONG)
	{
		targetThreshold = arbitrageContext->BollBottom + targetGain;
		if (arbitrageContext->ShortDiffFast > targetThreshold)
		{
			*fastDeal = true;
			return entity::SHORT;
		}
		// normal gain
		if (arbitrageContext->ShortDiff > targetThreshold)
		{
			return entity::SHORT;
		}
	}
	else if (posiDirection == entity::SHORT)
	{
		targetThreshold = arbitrageContext->BollTop - targetGain;
		if (arbitrageContext->LongDiffFast < targetThreshold)
		{
			*fastDeal = true;
			return entity::LONG;
		}
		// normal gain
		if (arbitrageContext->LongDiff < targetThreshold)
		{
			return entity::LONG;
		}
	}
	return entity::NET;
}

entity::PosiDirectionType GetStopGainDirectionOnCost(double cost, double targetGain, entity::PosiDirectionType posiDirection, ArbitrageStrategyContext* arbitrageContext, bool* fastDeal)
{
	*fastDeal = false;

	if (posiDirection == entity::LONG)
	{
		double gain = arbitrageContext->ShortDiffFast - cost;
		if (DoubleGreaterEqual(gain, targetGain))
		{
			*fastDeal = true;
			return entity::SHORT;
		}
		// normal gain
		gain = arbitrageContext->ShortDiff - cost;
		if (DoubleGreaterEqual(gain, targetGain))
		{
			return entity::SHORT;
		}
	}
	else if (posiDirection == entity::SHORT)
	{
		double gain = cost - arbitrageContext->LongDiffFast;
		if (DoubleGreaterEqual(gain, targetGain))
		{
			*fastDeal = true;
			return entity::LONG;
		}
		// normal gain
		gain = cost - arbitrageContext->LongDiff;
		if (DoubleGreaterEqual(gain, targetGain))
		{
			return entity::LONG;
		}
	}

	return entity::NET;
}

bool CArbitrageStrategyExecutor::TestForCloseUseTargetGainToBoundary(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	ArbitrageStrategyContext* arbitrageContext = dynamic_cast<ArbitrageStrategyContext*>(pContext);
	entity::PosiDirectionType side = PosiDirection();
	ARBI_DIFF_CALC& diffPrices = side == entity::LONG ? arbitrageContext->StructShortDiff : arbitrageContext->StructLongDiff;

	bool fastDeal = false;
	entity::PosiDirectionType stopGainDirection = GetStopGainDirectionOnOffset(m_pParentStrategy->TargetGain(), side, arbitrageContext, &fastDeal);

	if (stopGainDirection != entity::NET && stopGainDirection != side)
	{
		if (fastDeal)
			diffPrices = side == entity::LONG ? arbitrageContext->StructShortDiffFast : arbitrageContext->StructLongDiffFast;

		if (side == entity::LONG)
		{
			// Fast/Normal Stop Gain
			m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
			string logTxt = boost::str(boost::format("%s StopGain: Short diff(%.2f) > Bottom(%.2f) + Target Gain(%.2f)")
				% (fastDeal ? "FAST" : "")
				% (fastDeal ? arbitrageContext->ShortDiffFast : arbitrageContext->ShortDiff)
				% arbitrageContext->BollBottom % m_pParentStrategy->TargetGain());
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();

			string comment = boost::str(boost::format("%s空价差(%.2f) > 下轨(%.2f) + 目标盈利(%.2f) -> 止盈平仓")
				% (fastDeal ? "对价" : "")
				% (fastDeal ? arbitrageContext->ShortDiffFast : arbitrageContext->ShortDiff)
				% arbitrageContext->BollBottom % m_pParentStrategy->TargetGain());

			return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
		}
		else if (side == entity::SHORT)
		{
			// Fast/Normal Stop Gain
			m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
			string logTxt = boost::str(boost::format("%s StopGain: Long diff(%.2f) < Top(%.2f) - Target Gain(%.2f)")
				% (fastDeal ? "FAST" : "")
				% (fastDeal ? arbitrageContext->LongDiffFast : arbitrageContext->LongDiff)
				% arbitrageContext->BollTop
				% m_pParentStrategy->TargetGain());
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();
			string comment = boost::str(boost::format("%s多价差(%.2f) < 上轨(%.2f) - 目标盈利(%.2f) -> 止盈平仓")
				% (fastDeal ? "对价" : "")
				% (fastDeal ? arbitrageContext->LongDiffFast : arbitrageContext->LongDiff)
				% arbitrageContext->BollTop
				% m_pParentStrategy->TargetGain());
			return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
		}
	}

	// Stop loss logic in ArbitrageStrategy
	if (arbitrageContext->Direction != entity::NET)
	{
		if (side == entity::LONG)
		{
			string comment;
			if (StopLossLong(arbitrageContext, &comment))
			{
				pPortfolio->PrintLegsQuote();
				// Stop Loss
				m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopLoss);
			}
		}
		else if (side == entity::SHORT)
		{
			string comment;
			if (StopLossShort(arbitrageContext, &comment))
			{
				pPortfolio->PrintLegsQuote();
				// Stop Loss
				m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopLoss);
			}
		}
	}

	return false;
}

bool CArbitrageStrategyExecutor::TestForCloseUseTargetGainToCost(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	ArbitrageStrategyContext* arbitrageContext = dynamic_cast<ArbitrageStrategyContext*>(pContext);
	entity::PosiDirectionType side = PosiDirection();
	ARBI_DIFF_CALC& diffPrices = side == entity::LONG ? arbitrageContext->StructShortDiff : arbitrageContext->StructLongDiff;

	bool fastDeal = false;
	entity::PosiDirectionType stopGainDirection = GetStopGainDirectionOnCost(m_costDiff, m_pParentStrategy->TargetGain(), side, arbitrageContext, &fastDeal);

	if (stopGainDirection != entity::NET && stopGainDirection != side)
	{
		if (fastDeal)
			diffPrices = side == entity::LONG ? arbitrageContext->StructShortDiffFast : arbitrageContext->StructLongDiffFast;

		if (side == entity::LONG)
		{
			// Fast/Normal Stop Gain
			m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
			string logTxt = boost::str(boost::format("%s StopGain: Short diff(%.2f) - cost(%.2f) >= Target Gain(%.2f)")
				% (fastDeal ? "FAST" : "")
				% (fastDeal ? arbitrageContext->ShortDiffFast : arbitrageContext->ShortDiff)
				% m_costDiff % m_pParentStrategy->TargetGain());
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();

			string comment = boost::str(boost::format("%s空价差(%.2f) - 成本(%.2f) >= %.2f -> 止盈平仓")
				% (fastDeal ? "对价" : "")
				% (fastDeal ? arbitrageContext->ShortDiffFast : arbitrageContext->ShortDiff)
				% m_costDiff % m_pParentStrategy->TargetGain());

			return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
		}
		else if (side == entity::SHORT)
		{
			// Fast/Normal Stop Gain
			m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
			string logTxt = boost::str(boost::format("%s StopGain: cost(%.2f) - Long diff(%.2f) >= Target Gain(%.2f)")
				% (fastDeal ? "FAST" : "")
				% m_costDiff
				% (fastDeal ? arbitrageContext->LongDiffFast : arbitrageContext->LongDiff)
				% m_pParentStrategy->TargetGain());
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();
			string comment = boost::str(boost::format("成本(%.2f) - %s多价差(%.2f) >= %.2f -> 止盈平仓")
				% m_costDiff
				% (fastDeal ? "对价" : "")
				% (fastDeal ? arbitrageContext->LongDiffFast : arbitrageContext->LongDiff)
				% m_pParentStrategy->TargetGain());
			return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
		}
	}

	// Stop loss logic in ArbitrageStrategy
	if (arbitrageContext->Direction != entity::NET)
	{
		if (side == entity::LONG)
		{
			string comment;
			if (StopLossLong(arbitrageContext, &comment))
			{
				pPortfolio->PrintLegsQuote();
				// Stop Loss
				m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopLoss);
			}
		}
		else if (side == entity::SHORT)
		{
			string comment;
			if (StopLossShort(arbitrageContext, &comment))
			{
				pPortfolio->PrintLegsQuote();
				// Stop Loss
				m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
				return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_StopLoss);
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
		pOrderPlacer->SetMlOrderStatus(openComment);

		FireEvent(EXEC_OPEN);
		pOrderPlacer->OpenPosition(direction, lmtPrice, 2, timestamp, trade::SR_AutoOpen);
		m_costDiff = diffPrices.Diff;
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
		
		FireEvent(EXEC_CLOSE);
		CPortfolioArbitrageOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioArbitrageOrderPlacer*>(m_orderPlacer.get());
		pOrderPlacer->ClosePosition(m_volumeToClose, direction, lmtPrice, 2, timestamp, reason);

		ResetForceClose();
		pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s 平仓 @ %.2f - %.2f")
			% comment % GetPosiDirectionText(direction, true) % lmtPrice[0] % lmtPrice[1]));

		return true;
	}

	return false;
}

void CArbitrageStrategyExecutor::OnFilled(int volumeTraded)
{
	ExecutorState state = State();
	if (state == PENDING_OPEN)
		m_volumeToClose = volumeTraded;
	else if (state == PENDING_CLOSE)
	{
		if (m_closePositionPurpose == CLOSE_POSITION_STOP_LOSS)
		{
			m_lastStopLossDirection = PosiDirection();
			CPortfolio* pPortfolio = m_orderPlacer->Portfolio();
			if (pPortfolio != NULL)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) SAVING last stop loss direction (%s)")
					% pPortfolio->InvestorId() % pPortfolio->ID() % GetPosiDirectionText(m_lastStopLossDirection)));
			}
		}
	}

	CStrategyExecutor::OnFilled(volumeTraded);
}

bool CArbitrageStrategyExecutor::StopLossLong(ArbitrageStrategyContext* arbitrageContext, string* outComment)
{
	bool ret = false;
	if (m_pParentStrategy->StopLossType() == entity::STOP_LOSS_Auto)
	{
		ret = arbitrageContext->BollTop <= m_costDiff;
		if (ret)
		{
			string logTxt = boost::str(boost::format("bollTop(%.2f) <= costDiff(%.2f) -> Stop Loss")
				% arbitrageContext->BollTop % m_costDiff);
			LOG_DEBUG(logger, logTxt);
			
			*outComment = boost::str(boost::format("上轨(%.2f)低于等于成本(%.2f) -> 止损平仓")
				% arbitrageContext->BollTop % m_costDiff);
		}
	}
	else if (m_pParentStrategy->StopLossType() == entity::STOP_LOSS_Fixed)
	{
		double lost = m_costDiff - arbitrageContext->ShortDiffFast;
		if (m_pParentStrategy->StopLossComparison() == entity::GREATER_THAN)
		{
			ret = lost - m_pParentStrategy->StopLossThreshold() > 0.1;
		}
		else if (m_pParentStrategy->StopLossComparison() == entity::GREATER_EQUAL_THAN)
		{
			ret = DoubleGreaterEqual(lost, m_pParentStrategy->StopLossThreshold());
		}
		if (ret)
		{
			string logTxt = boost::str(boost::format("Current Lost(%.2f) %s Threshold(%.2f) -> Stop Loss")
				% lost % (m_pParentStrategy->StopLossComparison() == entity::GREATER_THAN ? ">" : ">=") 
				% m_pParentStrategy->StopLossThreshold());
			LOG_DEBUG(logger, logTxt);

			*outComment = boost::str(boost::format("对价亏损(%.2f)%s设定值(%.2f) -> 止损平仓")
				% lost % (m_pParentStrategy->StopLossComparison() == entity::GREATER_THAN ? "大于" : "大于等于")
				% m_pParentStrategy->StopLossThreshold());
		}
	}
	else // entity::STOP_LOSS_Disabled
	{
		return false;
	}

	return ret;
}

bool CArbitrageStrategyExecutor::StopLossShort(ArbitrageStrategyContext* arbitrageContext, string* outComment)
{
	bool ret = false;
	if (m_pParentStrategy->StopLossType() == entity::STOP_LOSS_Auto)
	{
		ret = arbitrageContext->BollBottom >= m_costDiff;
		if (ret)
		{
			// Stop Loss
			m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
			string logTxt = boost::str(boost::format("bollBottom(%.2f) >= costDiff(%.2f) -> Stop Loss")
				% arbitrageContext->BollBottom % m_costDiff);
			LOG_DEBUG(logger, logTxt);
			string comment = boost::str(boost::format("下轨(%.2f)大于等于成本(%.2f) -> 止损平仓")
				% arbitrageContext->BollBottom % m_costDiff);
		}
	}
	else if (m_pParentStrategy->StopLossType() == entity::STOP_LOSS_Fixed)
	{
		double lost = arbitrageContext->LongDiffFast - m_costDiff;
		if (m_pParentStrategy->StopLossComparison() == entity::GREATER_THAN)
		{
			ret = lost - m_pParentStrategy->StopLossThreshold() > 0.1;
		}
		else if (m_pParentStrategy->StopLossComparison() == entity::GREATER_EQUAL_THAN)
		{
			ret = DoubleGreaterEqual(lost, m_pParentStrategy->StopLossThreshold());
		}
		if (ret)
		{
			string logTxt = boost::str(boost::format("Current Lost(%.2f) %s Threshold(%.2f) -> Stop Loss")
				% lost % (m_pParentStrategy->StopLossComparison() == entity::GREATER_THAN ? ">" : ">=")
				% m_pParentStrategy->StopLossThreshold());
			LOG_DEBUG(logger, logTxt);

			*outComment = boost::str(boost::format("对价亏损(%.2f)%s设定值(%.2f) -> 止损平仓")
				% lost % (m_pParentStrategy->StopLossComparison() == entity::GREATER_THAN ? "大于" : "大于等于")
				% m_pParentStrategy->StopLossThreshold());
		}
	}
	else // entity::STOP_LOSS_Disabled
	{
		return false;
	}

	return ret;
}

bool CArbitrageStrategyExecutor::GetLastOpenOrderId(string& outMlOrderId)
{
	if (State() == HOLD_POSITION)
	{
		CPortfolioArbitrageOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioArbitrageOrderPlacer*>(m_orderPlacer.get());
		if (pOrderPlacer->IsOpened())
		{
			outMlOrderId = pOrderPlacer->LastOpenOrderId();
			return true;
		}
	}
	return false;
}
