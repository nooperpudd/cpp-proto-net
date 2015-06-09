#pragma once

#include "TechAnalyStrategy.h"
#include "StrategyExecutor.h"
#include "BufferRunner.h"

#include <queue>
#include <boost/unordered_map.hpp>

class PortfolioDoneMsg
{
public:
	PortfolioDoneMsg(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded)
		: m_execId(execId), m_doneState(doneState), m_offsetFlag(offsetFlag), m_volumeTraded(volumeTraded)
	{}

	int ExecId(){ return m_execId; }
	PortfolioFinishState DoneState() { return m_doneState; }
	entity::PosiOffsetFlag OffsetFlag(){ return m_offsetFlag; }
	int VolumeTraded(){ return m_volumeTraded; }

private:
	int m_execId;
	entity::PosiOffsetFlag m_offsetFlag;
	PortfolioFinishState m_doneState;
	int m_volumeTraded;
};

typedef boost::shared_ptr<PortfolioDoneMsg> PortfolioDoneMsgPtr;

class CMultiOpenStrategy : public CTechAnalyStrategy
{
public:
	CMultiOpenStrategy(CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CMultiOpenStrategy();

	void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp) = 0;
	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);
	virtual int OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder, int actualTradedVol);

	void TestWorkingExecutors(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	virtual void TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);
	virtual void TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp);

	void SetForceClose(const string& mlOrderId);
	void SetForceClose(int quantity);
	void SetForceClose(){ SetForceClose(0); }
protected:

	CPortfolioOrderPlacer* CreateOrderPlacer();
	virtual bool OnStart();
	void OnStop();

	virtual StrategyContext& GetContext() = 0;
	virtual StrategyExecutorPtr CreateExecutor(int execId, int quantity) = 0;
	virtual void BeforeTestForTrade(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context){}
	virtual void OnApply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers){}
	virtual bool Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context, boost::chrono::steady_clock::time_point& timestamp);

	bool PrepareExecutors();
	int GetCurrentPreparedQuantity();
	void InitializeExecutors();
	bool GetReadyExecutor(CStrategyExecutor** pOutExector);
	void OnPortfolioDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded);
	void HandlePortfolioDone(PortfolioDoneMsgPtr msgPtr);
	void PutErrorList(CStrategyExecutor* pErrExecutor);

	static double CalcMlOrderCost(const trade::MultiLegOrder& openOrder);
	static entity::PosiDirectionType GetMlOrderDirection(const trade::MultiLegOrder& mlOrder);

	int m_perOpenQuantity;
	int m_maxQuantity;

	int m_longPosition;
	double m_longAvgCost;
	int m_shortPosition;
	double m_shortAvgCost;

	vector<StrategyExecutorPtr> m_strategyExecutors;
	
	queue<CStrategyExecutor*> m_executorsPool;
	vector<CStrategyExecutor*> m_errorExecutors;
	
	// store the executors that are opening or closing position
	boost::unordered_map<int, CStrategyExecutor*> m_workingExecutors;
	
	// store the executors that are testing for closing poistion
	boost::unordered_map<int, CStrategyExecutor*> m_OpenedExecutors;
	
	// the current executor that is waiting for opening position
	CStrategyExecutor* m_activeExecutor;
	CPortfolio* m_pPortfolio;
	
	boost::mutex m_mut;

	CBufferRunner<PortfolioDoneMsgPtr> m_portfTradedEvtPump;
};

