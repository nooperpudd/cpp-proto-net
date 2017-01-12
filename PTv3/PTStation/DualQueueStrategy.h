#pragma once

#include "Strategy.h"

#include <boost/unordered_map.hpp>
#include <queue>
#include <deque>

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

	int Id() const { return m_levelId; }

	void SetDirection(entity::PosiDirectionType direction) { m_direction = direction; }
	entity::PosiDirectionType GetDirection() { return m_direction; }

	void SetLevelPx(double px) { m_levelPx = px; }
	double GetLevelPx() const { return m_levelPx; }

	bool IsStop(entity::Quote * pQuote);

	void CancelPendingAndClosePosition(entity::Quote * pQuote);
    void CancelPendingOpenOrder(entity::Quote * pQuote);
    void HandlePendingCloseOrder(boost::chrono::steady_clock::time_point& timestamp, entity::Quote * pQuote);
	void GetStatus(string* status);

	bool Prepare();
	double CancelQuoteAsk() const { return m_cancelQuoteAsk; }
	double CancelQuoteBid() const { return m_cancelQuoteBid; }
	void Reset() { m_cancelQuoteAsk = -1; m_cancelQuoteBid = -1; };

private:
	int m_levelId;
	double m_levelPx;
	OrderPlacerPtr m_orderPlacer;
	boost::atomic<DQ_STATUS> m_status;
	entity::PosiDirectionType m_direction;
	
	double m_cancelQuoteAsk;
	double m_cancelQuoteBid;
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

	void OpenPosition(CLevelOrderPlacer* pLevelPlacer, entity::PosiDirectionType direction, double openPx, boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote, bool async = false);
	
	CPortfolioOrderPlacer* CreateOrderPlacer();

	virtual bool OnStart();
	virtual void OnStop();

private:
	void OnOrderPlacerDone(int execId, PortfolioFinishState doneState, entity::PosiOffsetFlag offsetFlag, int volumeTraded, entity::PosiDirectionType direction);
	void OnStrategyError(CPortfolio * portf, const string & errorMsg) const;
	bool IfQuotingStable(entity::Quote* pQuote);
	void GetLevelOrderPlacerStatus(string* outStatus);
	void CalculateContext(entity::Quote* pQuote);
	bool OperatingConditionCheck(entity::Quote * pQuote);
	entity::PosiDirectionType DecideOpenDirection(entity::Quote * pQuote);
	bool EnsureAllPlacerStop(entity::Quote * pQuote);
	CLevelOrderPlacer* GetReadyOrderPlacer();
	bool IfLevelExists(double comparingPx);
    void HandlePendingCloseOrder(boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote);
	CLevelOrderPlacer* FindLowestOrderPlacer(double lowestPx);
	CLevelOrderPlacer* FindHighestOrderPlacer(double highestPx);
	void ResetLevelOrderPlacer(int execId, CLevelOrderPlacer* levelOrderPlacer = NULL);
	static deque<CLevelOrderPlacer*>::iterator FindLevelOrderPlacer(deque<CLevelOrderPlacer*>& queue, int execId);


	double m_lastAsk;
	double m_lastBid;
	int m_stableQuoteCount;
	
	bool m_stableQuote;

	boost::mutex m_mutSettings;
	boost::mutex m_mutLevels;

	double m_priceTick;
	int m_stableTickThreshold;
	int m_minWorkingSize;
	entity::PosiDirectionType m_direction;
	int m_levelsNum;
	int m_profitCount;
	int m_lossCount;
	int m_interval;
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
	deque<CLevelOrderPlacer*> m_longOrderQueue;
	deque<CLevelOrderPlacer*> m_shortOrderQueue;
	boost::mutex m_mutOrderQueue;

	bool m_stopping;
    bool m_orderQueued;
};


