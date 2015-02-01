#include "StdAfx.h"
#include "ArbitrageMultiStrategy.h"
#include "Portfolio.h"
#include "PriceBarDataProxy.h"

CArbitrageMultiStrategy::CArbitrageMultiStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio)
	: CMultiOpenStrategy(strategyItem, pAvatar, pPortfolio)
	, m_timeFrame(60)
	, m_bollPeriod(26)
	, m_stdDevMultiplier(2)
	, m_targetGain(0.8)
	, m_minStep(0.2)
	, m_useTargetGain(true)
	, m_allowPending(true)
{
	GetMinPriceChange(pPortfolio);
	Apply(strategyItem, false);
	
}

CArbitrageMultiStrategy::~CArbitrageMultiStrategy()
{
}

void CArbitrageMultiStrategy::OnApply(const entity::StrategyItem& strategyItem, bool withTriggers)
{
	m_bollPeriod = (int)strategyItem.ar_bollperiod();
	m_stdDevMultiplier = (int)strategyItem.ar_stddevmultiplier();
	m_openTimeout = strategyItem.opentimeout();
	m_retryTimes = strategyItem.retrytimes();
	m_useTargetGain = strategyItem.ar_usetargetgain();
	m_targetGainTimes = strategyItem.ar_targetgain();
	m_targetGain = m_targetGainTimes * m_minStep;
	assert(m_targetGain > 0);

	// make sure following parameters having values
	if (m_openTimeout == 0)
		m_openTimeout = 350;
	if (m_retryTimes == 0)
		m_retryTimes = 8;

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

void CArbitrageMultiStrategy::CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{

}

void CArbitrageMultiStrategy::InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc)
{

}

void CArbitrageStrategyExecutor::OnWorking(entity::Quote* pQuote, const StrategyContext* pContext)
{

}

void CArbitrageStrategyExecutor::OnTestForOpen(entity::Quote* pQuote, const StrategyContext* pContext)
{

}

void CArbitrageStrategyExecutor::OnTestForClose(entity::Quote* pQuote, const StrategyContext* pContext)
{

}

