#pragma once

#include "Portfolio.h"
#include "PortfolioOrderPlacer.h"

enum ExecutorState
{
	EXECUTOR_IDLE,
	EMPTY_POSITION,
	PENDING_OPEN,
	HOLD_POSITION,
	PENDING_CLOSE
};

enum ExecutorEvent
{
	EXEC_OPEN,
	EXEC_CLOSE,
	EXEC_FILLED,
	EXEC_CANCELLED
};

class StrategyContext 
{
public:
	StrategyContext() : CurrentIndex(-1){}
	virtual ~StrategyContext(){}

	int CurrentIndex;
};

class CStrategyExecutor
{
public:
	CStrategyExecutor(int execId, int quantity);
	virtual ~CStrategyExecutor();

	int ExecId(){ return m_execId; }
	bool Prepare();
	void Cleanup();
	void Start();
	void FireEvent(ExecutorEvent execEvent);
	entity::PosiDirectionType PosiDirection();

	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc, PortfolioTradedEvent porfTradedEventHandler) = 0;

	virtual void OnWorking(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp){}
	virtual void OnFilled(int volumeTraded);
	virtual void OnCanceled();
	virtual void OnFinished();
	virtual bool TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp) = 0;
	virtual bool TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp) = 0;

	ExecutorState State(){ return m_currentState.load(boost::memory_order_consume); }
	void SetState(ExecutorState state){ m_currentState.store(state, boost::memory_order_release); }

protected:

	virtual void OpenPosition(entity::PosiDirectionType direction, ARBI_DIFF_CALC diffPrices, StrategyContext* pContext, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp) = 0;
	virtual bool ClosePosition(ARBI_DIFF_CALC diffPrices, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment, trade::SubmitReason reason) = 0;

	int m_execId;

	int m_quantity;
	boost::shared_ptr<void> m_fsm;
	boost::atomic<ExecutorState> m_currentState;

	OrderPlacerPtr m_orderPlacer;
	bool m_deferringCleanup;
};

typedef boost::shared_ptr<CStrategyExecutor> StrategyExecutorPtr;

