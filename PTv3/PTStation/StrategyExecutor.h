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

class CStrategyExecutor
{
public:
	CStrategyExecutor();
	~CStrategyExecutor();

	void OnWorking(entity::Quote* pQuote){}
	void OnTestForOpen(entity::Quote* pQuote){}
	void OnTestForClose(entity::Quote* pQuote){}

	ExecutorState State(){ return m_currentState.load(boost::memory_order_consume); }
	void SetState(ExecutorState state){ m_currentState.store(state, boost::memory_order_release); }
private:

	boost::shared_ptr<void> m_fsm;
	boost::atomic<ExecutorState> m_currentState;

	OrderPlacerPtr m_orderPlacer;
};

typedef boost::shared_ptr<CStrategyExecutor> StrategyExecutorPtr;

