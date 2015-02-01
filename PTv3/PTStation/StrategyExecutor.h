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

struct StrategyContext {};

class CStrategyExecutor
{
public:
	CStrategyExecutor();
	virtual ~CStrategyExecutor();

	virtual void OnWorking(entity::Quote* pQuote, const StrategyContext* pContext){}
	virtual void OnTestForOpen(entity::Quote* pQuote, const StrategyContext* pContext){}
	virtual void OnTestForClose(entity::Quote* pQuote, const StrategyContext* pContext){}

	ExecutorState State(){ return m_currentState.load(boost::memory_order_consume); }
	void SetState(ExecutorState state){ m_currentState.store(state, boost::memory_order_release); }

protected:

	boost::shared_ptr<void> m_fsm;
	boost::atomic<ExecutorState> m_currentState;

	OrderPlacerPtr m_orderPlacer;
};

typedef boost::shared_ptr<CStrategyExecutor> StrategyExecutorPtr;

