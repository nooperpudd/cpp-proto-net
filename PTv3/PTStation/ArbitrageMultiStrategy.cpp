#include "StdAfx.h"
#include "ArbitrageMultiStrategy.h"
#include "Portfolio.h"
#include "PriceBarDataProxy.h"
#include "PortfolioArbitrageOrderPlacer.h"

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

bool CArbitrageMultiStrategy::Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	bool prerequisiteOK = CMultiOpenStrategy::Prerequisite(pQuote, pPortfolio, timestamp);
	if (prerequisiteOK)
	{
		string symbol = pQuote->symbol();
		COHLCRecordSet* ohlc = GetRecordSet(symbol, m_timeFrame, timestamp);
		if (ohlc == NULL)
			return false;

		int currentBarIdx = m_diffRecordSet->Calculate(ohlc);
		if (currentBarIdx < m_bollPeriod)
			return false;

		if (!pPortfolio->LegsTimestampAligned())
			return false;
	}

	return prerequisiteOK;
}

StrategyContext* CArbitrageMultiStrategy::CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	ARBI_DIFF_CALC structLastDiff = { LAST_DIFF, 0, 0, 0 };
	ARBI_DIFF_CALC structLongDiff = { LONG_DIFF, 0, 0, 0 };
	ARBI_DIFF_CALC structShortDiff = { SHORT_DIFF, 0, 0, 0 };
	ARBI_DIFF_CALC structLongDiffFast = { LONG_DIFF, 0, 0, 0 };
	ARBI_DIFF_CALC structShortDiffFast = { SHORT_DIFF, 0, 0, 0 };

	CALC_DIFF_METHOD calcMethod = m_allowPending ? BETTER_PRICE : FAST_DEAL;

	m_context.LastDiff = pPortfolio->CalculateDiff(&structLastDiff, calcMethod);
	m_context.LongDiff = pPortfolio->CalculateDiff(&structLongDiff, calcMethod);
	m_context.ShortDiff = pPortfolio->CalculateDiff(&structShortDiff, calcMethod);

	m_context.LongDiffFast = pPortfolio->CalculateDiff(&structLongDiffFast, FAST_DEAL);
	m_context.ShortDiffFast = pPortfolio->CalculateDiff(&structShortDiffFast, FAST_DEAL);

	// TODO: will be figured out in the future
	m_context.LongDiffSize = 0;
	m_context.ShortDiffSize = 0;

	m_bollDataSet->Calculate(m_diffRecordSet.get(), pPortfolio);

	double actualMid = 0;
	if (m_useTargetGain)
	{
		double bollMid = m_bollDataSet->GetRef(IND_MID, 1);
		actualMid = CalcBoundaryByTargetGain(bollMid, m_targetGain, m_minStep, &m_context.BollTop, &m_context.BollBottom);
	}
	else
	{
		m_context.BollTop = m_bollDataSet->GetRef(IND_TOP, 1);
		m_context.BollBottom = m_bollDataSet->GetRef(IND_BOTTOM, 1);
	}

	return &m_context;
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
	return StrategyExecutorPtr(new CArbitrageStrategyExecutor(execId, quantity));
}

void CArbitrageStrategyExecutor::OnWorking(entity::Quote* pQuote, StrategyContext* pContext)
{

}

bool CArbitrageStrategyExecutor::TestForOpen(entity::Quote* pQuote, StrategyContext* pContext)
{
	return true;
}

bool CArbitrageStrategyExecutor::TestForClose(entity::Quote* pQuote, StrategyContext* pContext)
{
	return true;
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

