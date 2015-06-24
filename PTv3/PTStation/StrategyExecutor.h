#pragma once

#include "Portfolio.h"
#include "PortfolioOrderPlacer.h"

enum ExecutorState
{
	EXECUTOR_IDLE,
	EMPTY_POSITION,
	PENDING_OPEN,
	HOLD_POSITION,
	PENDING_CLOSE,
	EXECUTOR_ERROR
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
	int Quantity() { return m_quantity; }
	bool Prepare();
	virtual void Cleanup();
	void Start();
	void FireEvent(ExecutorEvent execEvent);
	entity::PosiDirectionType PosiDirection();
	void VirtualFill();

	void SetForceClose(){ m_forceClosing.store(true, boost::memory_order_release); };
	void ResetForceClose(){ m_forceClosing.store(false, boost::memory_order_release); };
	bool IsForceClosing(){ return m_forceClosing.load(boost::memory_order_acquire); };

	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc, PortfolioTradedEvent porfTradedEventHandler) = 0;
	void UpdateOrderProcessor(COrderProcessor* pOrderProc);

	virtual void OnWorking(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp){}
	virtual void OnFilled(int volumeTraded);
	virtual void OnCanceled();
	virtual void OnFinished();
	virtual void OnError();
	virtual bool TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp) = 0;
	virtual bool TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp) = 0;
	virtual bool GetLastOpenOrderId(string& outMlOrderId);

	ExecutorState State(){ return m_currentState.load(boost::memory_order_acquire); }
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

	boost::atomic<bool> m_forceClosing;
};

typedef boost::shared_ptr<CStrategyExecutor> StrategyExecutorPtr;

