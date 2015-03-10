#pragma once

#include "MultiOpenStrategy.h"
#include "DiffRecordSet.h"
#include "BollDataSet.h"

class ArbitrageStrategyContext : public StrategyContext
{
public:
	ArbitrageStrategyContext()
		: LastDiff(0), LongDiff(0), LongDiffFast(0), LongDiffSize(0)
		, ShortDiff(0), ShortDiffFast(0), ShortDiffSize(0)
		, BollTop(0), BollBottom(0){}
	~ArbitrageStrategyContext(){}

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
	CArbitrageStrategyExecutor(int execId, int quantity) :CStrategyExecutor(execId, quantity){}
	~CArbitrageStrategyExecutor(){}

	void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc, PortfolioTradedEvent porfTradedEventHandler);

	void OnWorking(entity::Quote* pQuote, StrategyContext* pContext);
	bool TestForOpen(entity::Quote* pQuote, StrategyContext* pContext);
	bool TestForClose(entity::Quote* pQuote, StrategyContext* pContext);

private:

};

class CArbitrageMultiStrategy : public CMultiOpenStrategy
{
public:
	CArbitrageMultiStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CArbitrageMultiStrategy();

	virtual StrategyContext* CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

protected:
	StrategyExecutorPtr CreateExecutor(int execId, int quantity);
	virtual void OnApply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual bool Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
private:
	void GetMinPriceChange(CPortfolio* pPortfolio);
	static double GetLowerValue(double val, double offset);
	static double CalcBoundaryByTargetGain(double mid, double targetGain, double step, double* outUpper, double* outLower);

	int m_timeFrame;
	int m_bollPeriod;
	int m_stdDevMultiplier;
	int m_targetGainTimes;
	double m_targetGain;
	double m_minStep;
	bool m_useTargetGain;
	entity::ArbitrageStopLossType m_stopLossType;
	bool m_allowPending;

	DiffRecordSetPtr m_diffRecordSet;
	BollDataSetPtr m_bollDataSet;

	ArbitrageStrategyContext m_context;
};

