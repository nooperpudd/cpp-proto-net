#pragma once

#include "MultiOpenStrategy.h"
#include "DiffRecordSet.h"
#include "BollDataSet.h"

struct ArbitrageStrategyContext : public StrategyContext
{
	double LastDiff;
	double LongDiff;
	double LongDiffFast;
	int LongDiffSize;
	double ShortDiff;
	double ShortDiffFast;
	int ShortDiffSize;
	double BollTop;
	double BollBottom;
};

class CArbitrageStrategyExecutor : public CStrategyExecutor
{
public:
	CArbitrageStrategyExecutor(){}
	~CArbitrageStrategyExecutor(){}

	void OnWorking(entity::Quote* pQuote, const StrategyContext* pContext);
	void OnTestForOpen(entity::Quote* pQuote, const StrategyContext* pContext);
	void OnTestForClose(entity::Quote* pQuote, const StrategyContext* pContext);

private:

};

class CArbitrageMultiStrategy : public CMultiOpenStrategy
{
public:
	CArbitrageMultiStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CArbitrageMultiStrategy();

	
	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc);
	virtual void CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

protected:
	virtual void OnApply(const entity::StrategyItem& strategyItem, bool withTriggers);

private:
	void GetMinPriceChange(CPortfolio* pPortfolio);

	int m_timeFrame;
	int m_bollPeriod;
	int m_stdDevMultiplier;
	int m_targetGainTimes;
	double m_targetGain;
	double m_minStep;
	bool m_useTargetGain;
	bool m_allowPending;

	DiffRecordSetPtr m_diffRecordSet;
	BollDataSetPtr m_bollDataSet;

};

