#pragma once

#include "MultiRouteStrategy.h"
#include "DualTradingEnums.h"

class CDualScalperOrderPlacer;

class CDualQueueStrategy : public CMultiRouteStrategy
{
public:
	CDualQueueStrategy();
	~CDualQueueStrategy();

	virtual void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);

	virtual void Stop();
	virtual void AlreadyStarted();

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc) {}

	virtual void OnLegFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume);
	virtual void OnLegCanceled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction);
	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);

	DualScalperState State() { return m_currentState.load(boost::memory_order_acquire); }
	void SetState(DualScalperState state) { m_currentState.store(state, boost::memory_order_release); }

	void OnStrategyError(CPortfolio* portf, const string& errorMsg) const;
	virtual bool StopOnTimepoint();

protected:

	void OnBindedRoutes();
	CPortfolioOrderPlacer* CreateOrderPlacer();

	virtual bool OnStart();
	virtual void OnStop();

private:
	void Transition(DualScalperEvent evt);

	void OpenPosition(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void ClosePosition(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	void LongStopLoss(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp) const;
	void ShortStopLoss(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp) const;

	void OnLongOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded);
	void OnShortOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded);

	double m_priceTick;
	string m_longSideUserId;
	string m_shortSideUserId;

	CDualScalperOrderPlacer* m_longOrderPlacer;
	CDualScalperOrderPlacer* m_shortOrderPlacer;

	//double m_diff;
	//double m_ask;
	//int m_askSize;
	//double m_bid;
	//int m_bidSize;
	int m_stableTickThreshold;
	int m_minWorkingSize;

	boost::atomic<DualScalperState> m_currentState;
	boost::mutex m_mutFsm;

	bool m_stopping;
};

