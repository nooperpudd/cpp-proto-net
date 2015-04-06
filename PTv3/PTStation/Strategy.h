#pragma once

#include "Trigger.h"
#include "PortfolioOrderPlacer.h"

#include "entity/message.pb.h"
#include "entity/quote.pb.h"
#include "entity/trade.pb.h"

#include <boost/chrono.hpp>
#include <boost/atomic.hpp>

class CPortfolio;
class COrderProcessor;

class CStrategy
{
public:
	CStrategy();
	virtual ~CStrategy(void);

	entity::StrategyType Type(){ return m_type; }
	int RetryTimes(){ return m_retryTimes; }
	int OpenTimeout(){ return m_openTimeout; }
	vector<TriggerPtr>& Triggers() { return m_triggers; }

	virtual void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	void ApplyTrigger(const entity::StrategyItem& strategyItem);
	virtual void Cleanup(){}

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);
	
	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);
	virtual int OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder, int actualTradedVol);
	virtual void OnLegFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume){}

	bool IsRunning(){ return m_running.load(boost::memory_order_acquire); }
	virtual void Start();
	virtual void Stop();

	static double CalcOrderProfit(const trade::MultiLegOrder& openOrder);
	static int CalcOffsetBarsBeforeMktCls(int minutesBeforeMktCls, int timeFrame);

	void SetForceOpen(){ m_forceOpening.store(true, boost::memory_order_release); }
	void ResetForceOpen(){ m_forceOpening.store(false, boost::memory_order_release); }
	bool IsForceOpening(){ return m_forceOpening.load(boost::memory_order_acquire); }

	void SetForceClose(){ m_forceClosing.store(true, boost::memory_order_release); };
	void ResetForceClose(){ m_forceClosing.store(false, boost::memory_order_release);};
	bool IsForceClosing(){ return m_forceClosing.load(boost::memory_order_acquire); };

	bool IsMarketOpen(entity::Quote* pQuote);

	CPortfolioOrderPlacer* OrderPlacer() { return m_orderPlacer.get(); }
	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc);

protected:

	virtual CPortfolioOrderPlacer* CreateOrderPlacer() = 0;
	virtual bool OnStart(){ return true; }
	virtual void OnStop(){}

	void AddProfit(CPortfolio* pPortfolio, double profit);
	void SetAvgCost(CPortfolio* pPortfolio, double avgCost);
	int IncrementOpenTimes( CPortfolio* pPortfolio, int qty);
	int IncrementCloseTimes( CPortfolio* pPortfolio, int qty);
	int PositionQuantity(CPortfolio* pPortfolio);
	double AvgCost(CPortfolio* pPortfolio);

	void CalculateEndBar(int offset, int timeFrame, int histDataSize);
	bool OutOfTradingWindow(int currentBarIdx);

	vector<TriggerPtr> m_triggers;
	entity::StrategyType m_type;
	boost::atomic<bool> m_running;
	int m_retryTimes;
	int m_openTimeout;

	boost::atomic<bool> m_forceOpening;
	boost::atomic<bool> m_forceClosing;

	bool m_marketOpen;
	int m_endTradingBar;

	OrderPlacerPtr m_orderPlacer;
};

typedef boost::shared_ptr<CStrategy> StrategyPtr;
typedef vector<TriggerPtr>::iterator TriggerIter;


