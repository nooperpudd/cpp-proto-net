#pragma once

#include "ArbitrageMultiStrategy.h"

class CArbitrageManualMultiStrategy : public CMultiOpenStrategy
{
public:
	CArbitrageManualMultiStrategy(CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CArbitrageManualMultiStrategy();

	virtual void CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	
	entity::PosiDirectionType Direction(){ return m_direction; }
	entity::PosiOffsetFlag Offset(){ return m_offset; }
	entity::CompareCondition OpenCondition(){ return m_openCondition; }
	double OpenThreshold(){ return m_openThreshold; }
	entity::CompareCondition CloseCondition(){ return m_closeCondition; }
	double CloseThreshold(){ return m_closeThreshold; }

	void TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	void TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp){}

protected:
	virtual void OnApply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual bool Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context, boost::chrono::steady_clock::time_point& timestamp);
	StrategyExecutorPtr CreateExecutor(int execId, int quantity);
	StrategyContext& GetContext(){ return m_context; }
	void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

private:

	ArbitrageStrategyContext m_context;

	entity::PosiDirectionType m_direction;
	entity::PosiOffsetFlag m_offset;
	entity::CompareCondition m_openCondition;
	double m_openThreshold;
	entity::CompareCondition m_closeCondition;
	double m_closeThreshold;
	string m_doneComment;

};

class CArbitrageManualStrategyExecutor : public CStrategyExecutor
{
public:
	CArbitrageManualStrategyExecutor(int execId, int quantity, CArbitrageManualMultiStrategy* parentStrategy)
		: CStrategyExecutor(execId, quantity), m_pParentStrategy(parentStrategy), m_volumeToClose(0)
	{}
	~CArbitrageManualStrategyExecutor(){}

	void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc, PortfolioTradedEvent porfTradedEventHandler);

	void OnWorking(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void OnFilled(int volumeTraded);
	bool TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	bool TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	bool GetLastOpenOrderId(string& outMlOrderId);

	void Cleanup();

protected:
	void OpenPosition(entity::PosiDirectionType direction, ARBI_DIFF_CALC diffPrices, StrategyContext* pContext, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp){}
	void OpenPosition(entity::PosiDirectionType direction, ARBI_DIFF_CALC diffPrices, StrategyContext* pContext, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment);
	bool ClosePosition(ARBI_DIFF_CALC diffPrices, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment, trade::SubmitReason reason);

private:
	bool TestForLongClose(double diffPx);
	bool TestForShortClose(double diffPx);
	bool TestForLongOpen(double diffPx);
	bool TestForShortOpen(double diffPx);

	CArbitrageManualMultiStrategy* m_pParentStrategy;

	int m_volumeToClose;
};

