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
		, StructLastDiff(LAST_DIFF)
		, StructLongDiff(LONG_DIFF)
		, StructShortDiff(SHORT_DIFF)
		, StructLongDiffFast(LONG_DIFF)
		, StructShortDiffFast(SHORT_DIFF)
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
	CArbitrageMultiStrategy(CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CArbitrageMultiStrategy();

	bool NotOpenInStopLossDirection(){ return m_notOpenInStopLossDirection; }
	entity::ArbitrageStopLossType StopLossType(){ return m_stopLossType; }
	entity::CompareCondition StopLossComparison(){ return m_stopLossComparison; }
	double StopLossThreshold(){ return m_stopLossThreshold; }

	virtual void CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

	bool UseTargetGain(){ return m_useTargetGain; }
	double TargetGain(){ return m_targetGain; }

	bool AbsoluteGain(){ return m_absoluteGain;  }

protected:
	StrategyExecutorPtr CreateExecutor(int execId, int quantity);
	StrategyContext& GetContext(){ return m_context; }
	void BeforeTestForTrade(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context);
	void OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg);
	void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);
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
	int m_bandRangeTimes;
	double m_targetGain;
	double m_minStep;
	bool m_useTargetGain;
	bool m_absoluteGain;
	bool m_specifyBandRange;
	double m_bandRange;
	double m_stopLossThreshold;
	entity::CompareCondition m_stopLossComparison;
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

	void OnWorking(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void OnFilled(int volumeTraded);
	bool TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	bool TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);

protected:
	void OpenPosition(entity::PosiDirectionType direction, ARBI_DIFF_CALC diffPrices, StrategyContext* pContext, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	bool ClosePosition(ARBI_DIFF_CALC diffPrices, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment, trade::SubmitReason reason);

private:
	bool TestForCloseUseTargetGainToCost(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	bool TestForCloseUseTargetGainToBoundary(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);

	bool StopLossLong(ArbitrageStrategyContext* arbitrageContext, string* outComment);
	bool StopLossShort(ArbitrageStrategyContext* arbitrageContext, string* outComment);


	CArbitrageMultiStrategy* m_pParentStrategy;

	double m_costDiff;
	int m_volumeToClose;

	CLOSE_POSITION_PURPOSE m_closePositionPurpose;
	entity::PosiDirectionType m_lastStopLossDirection;
};
