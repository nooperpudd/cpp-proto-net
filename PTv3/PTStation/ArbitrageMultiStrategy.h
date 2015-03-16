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
		, BollTop(0), BollBottom(0), BollMid(0), ActualMid(0)
		, Direction(entity::NET), DirectionFast(entity::NET), LastStopLossDirection(entity::NET)
		, StructLastDiff({ LAST_DIFF, 0, 0, 0 })
		, StructLongDiff({ LONG_DIFF, 0, 0, 0 })
		, StructShortDiff({ SHORT_DIFF, 0, 0, 0 })
		, StructLongDiffFast({ LONG_DIFF, 0, 0, 0 })
		, StructShortDiffFast({ SHORT_DIFF, 0, 0, 0 })
	{}
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
	double BollMid;
	double ActualMid;

	ARBI_DIFF_CALC StructLastDiff;
	ARBI_DIFF_CALC StructLongDiff;
	ARBI_DIFF_CALC StructShortDiff;
	ARBI_DIFF_CALC StructLongDiffFast;
	ARBI_DIFF_CALC StructShortDiffFast;
	
	entity::PosiDirectionType Direction;
	entity::PosiDirectionType DirectionFast;
	entity::PosiDirectionType LastStopLossDirection;
};

class CArbitrageMultiStrategy : public CMultiOpenStrategy
{
public:
	CArbitrageMultiStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CArbitrageMultiStrategy();

	bool NotOpenInStopLossDirection(){ return m_notOpenInStopLossDirection; }

	virtual void CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

protected:
	StrategyExecutorPtr CreateExecutor(int execId, int quantity);
	StrategyContext& GetContext(){ return m_context; }
	void BeforeTestForTrade(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context);

	virtual void OnApply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual bool Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context, boost::chrono::steady_clock::time_point& timestamp);

private:

	void GetMinPriceChange(CPortfolio* pPortfolio);
	static double GetLowerValue(double val, double offset);
	static double CalcBoundaryByTargetGain(double mid, double targetGain, double step, double* outUpper, double* outLower);
	static entity::PosiDirectionType GetTradeDirection(ArbitrageStrategyContext& context);
	static entity::PosiDirectionType GetFastTradeDirection(ArbitrageStrategyContext& context);

	int m_timeFrame;
	int m_bollPeriod;
	int m_stdDevMultiplier;
	int m_targetGainTimes;
	double m_targetGain;
	double m_minStep;
	bool m_useTargetGain;
	entity::ArbitrageStopLossType m_stopLossType;
	bool m_allowPending;

	bool m_notOpenInStopLossDirection;

	DiffRecordSetPtr m_diffRecordSet;
	BollDataSetPtr m_bollDataSet;

	ArbitrageStrategyContext m_context;
};


class CArbitrageStrategyExecutor : public CStrategyExecutor
{
public:
	CArbitrageStrategyExecutor(int execId, int quantity, CArbitrageMultiStrategy* parentStrategy) 
		: CStrategyExecutor(execId, quantity), m_pParentStrategy(parentStrategy), m_costDiff(0), m_volumeToClose(0)
	{}
	~CArbitrageStrategyExecutor(){}

	void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc, PortfolioTradedEvent porfTradedEventHandler);

	void OnWorking(entity::Quote* pQuote, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	void OnFilled(int volumeTraded);
	bool TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	bool TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);

protected:
	void OpenPosition(entity::PosiDirectionType direction, ARBI_DIFF_CALC diffPrices, StrategyContext* pContext, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	bool ClosePosition(ARBI_DIFF_CALC diffPrices, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment, trade::SubmitReason reason);

private:

	CArbitrageMultiStrategy* m_pParentStrategy;

	double m_costDiff;
	int m_volumeToClose;

	CLOSE_POSITION_PURPOSE m_closePositionPurpose;
	entity::PosiDirectionType m_lastStopLossDirection;
};
