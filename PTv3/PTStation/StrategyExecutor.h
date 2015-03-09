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

class StrategyContext 
{
public:
	virtual ~StrategyContext(){}
};

class CStrategyExecutor
{
public:
	CStrategyExecutor(int execId, int quantity);
	virtual ~CStrategyExecutor();

	int ExecId(){ return m_execId; }
	void Start();
	void Transit(){}

	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc) = 0;

	virtual void OnWorking(entity::Quote* pQuote, StrategyContext* pContext){}
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

