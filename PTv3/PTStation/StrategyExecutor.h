#pragma once

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
	EXEC_FILLED
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
	void Start();
	void FireEvent(ExecutorEvent execEvent);

	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc, PortfolioTradedEvent porfTradedEventHandler) = 0;

	virtual void OnWorking(entity::Quote* pQuote, StrategyContext* pContext){}
	virtual void OnFinished(){}
	virtual bool TestForOpen(entity::Quote* pQuote, StrategyContext* pContext) = 0;
	virtual bool TestForClose(entity::Quote* pQuote, StrategyContext* pContext) = 0;

	ExecutorState State(){ return m_currentState.load(boost::memory_order_consume); }
	void SetState(ExecutorState state){ m_currentState.store(state, boost::memory_order_release); }

protected:

	void OpenPosition(entity::PosiDirectionType direction, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp){}

	int m_execId;

	int m_quantity;
	boost::shared_ptr<void> m_fsm;
	boost::atomic<ExecutorState> m_currentState;

	OrderPlacerPtr m_orderPlacer;
};

typedef boost::shared_ptr<CStrategyExecutor> StrategyExecutorPtr;

