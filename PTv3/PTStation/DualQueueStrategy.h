#pragma once

#include "Strategy.h"

#include <boost/unordered_map.hpp>
#include <queue>

class CPortfolioQueueOrderPlacer;

enum DQ_STATUS
{
	DQ_UNKNOWN = -1,
	DQ_UNOPENED = 0,
	DQ_IS_OPENING,
	DQ_OPENED,
	DQ_IS_CLOSING,
	DQ_CLOSED
};

class CLevelOrderPlacer
{
public:
	CLevelOrderPlacer(int lvlId, CPortfolioQueueOrderPlacer* pOrderPlacer);
		
	CPortfolioQueueOrderPlacer* GetOrderPlacer();

	void SetStatus(DQ_STATUS status)
	{
		m_status = status;
	}

	DQ_STATUS GetStatus() { return m_status; }

	void SetLevelPx(double px) { m_levelPx = px; }
	double GetLevelPx() { return m_levelPx; }

	bool IsStop(entity::Quote * pQuote);

	void CancelPendingAndClosePosition(entity::Quote * pQuote);
    void CancelPendingOpenOrder();
    void HandlePendingCloseOrder(boost::chrono::steady_clock::time_point& timestamp, entity::Quote * pQuote);


private:
	int m_levelId;
	double m_levelPx;
	OrderPlacerPtr m_orderPlacer;
	boost::atomic<DQ_STATUS> m_status;
};

typedef boost::shared_ptr<CLevelOrderPlacer> LevelOrderPlacerPtr;

class CDualQueueStrategy : public CStrategy
{
public:
	CDualQueueStrategy();
	~CDualQueueStrategy();

	virtual void Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers);
	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem);

	virtual void Stop();
	virtual void AlreadyStarted();

	virtual int OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol);
	virtual void OnLegFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume, int execId);
	virtual void OnLegCanceled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, int execId);

	virtual void InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc);

protected:

	void OpenPosition(CLevelOrderPlacer* pLevelPlacer, entity::PosiDirectionType direction, double openPx, boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote);
	
	CPortfolioOrderPlacer* CreateOrderPlacer();

	virtual bool OnStart();
	virtual void OnStop();

private:
	void OnStrategyError(CPortfolio * portf, const string & errorMsg) const;
	bool IfQuotingStable(entity::Quote* pQuote);

	void CalculateContext(entity::Quote* pQuote);
	bool OperatingConditionCheck(entity::Quote * pQuote);
	entity::PosiDirectionType DecideOpenDirection(entity::Quote * pQuote);
	bool EnsureAllPlacerStop(entity::Quote * pQuote);
	CLevelOrderPlacer* GetReadyOrderPlacer();
	bool IfLevelExists(double comparingPx);
    void HandlePendingCloseOrder(boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote);
	CLevelOrderPlacer* FindLowestOrderPlacer(double lowestPx);
	CLevelOrderPlacer* FindHighestOrderPlacer(double highestPx);
	double m_lastAsk;
	double m_lastBid;
	int m_stableQuoteCount;
	
	bool m_stableQuote;

	boost::mutex m_mut;
	boost::mutex m_mutLevels;

	double m_priceTick;
	int m_stableTickThreshold;
	int m_minWorkingSize;
	entity::PosiDirectionType m_direction;
	int m_levelsNum;
	int m_profitCount;
	double m_openThresholdTimes;
	int m_stableMinutesThreshold;
	boost::chrono::minutes m_stableMinutes;
	boost::chrono::seconds m_stableSeconds;

	double m_latestHigh;
	boost::chrono::seconds m_latestHighTime;
	double m_latestLow;
	boost::chrono::seconds m_latestLowTime;


	boost::unordered_map<int, LevelOrderPlacerPtr> m_levelOrderPlacers;
	typedef boost::unordered_map<int, LevelOrderPlacerPtr>::iterator LevelOrderPlacersIter;
	queue<int> m_readyQueue;

	bool m_stopping;
    bool m_orderQueued;
};


