#pragma once

#include "Strategy.h"
#include "Leg.h"
#include "entity/message.pb.h"
#include "entity/quote.pb.h"
#include "entity/trade.pb.h"
#include <boost/atomic.hpp>

enum DIFF_TYPE 
{
	LAST_DIFF, LONG_DIFF, SHORT_DIFF
};

enum CALC_DIFF_METHOD
{
	FAST_DEAL, BETTER_PRICE
};

struct ARBI_DIFF_CALC
{
	DIFF_TYPE DiffType;
	double Diff;
	double BuyPrice;
	double SellPrice;

	ARBI_DIFF_CALC(DIFF_TYPE diffType)
		: DiffType(diffType)
		, Diff(0), BuyPrice(0), SellPrice(0)
	{}
};

double CalcDiff(vector<LegPtr>& legs, DIFF_TYPE diffType);
int CalcSize(vector<LegPtr>& legs, DIFF_TYPE diffType);

class CQuoteRepositry;
class CSimpleQuoteReceiver;
class CQuoteFetcher;
class CAvatarClient;
class COrderProcessor;

class CPortfolio
{
public:
	CPortfolio(CAvatarClient* client, const entity::PortfolioItem& srcPortfolioItem);
	~CPortfolio(void);

	friend class CStrategy;

	// properties
	CAvatarClient* ParentAvatar(){ return m_avatar; }
	COrderProcessor* OrderProcessor();
	void ReleaseAvatar(){ m_avatar = NULL; }
	const string& ID(){ return m_portfolioItem.id(); }
	int Quantity(){ return m_portfolioItem.quantity(); }
	int Count(){ return m_legs.size(); }
	int TotalOpenTimes() { return m_totalOpenTimes; }
	int TotalCloseTimes() { return m_totalCloseTimes; }
	double Profit() { return m_profit; }
	double AvgCost() { return m_avgCost; }
	const string& InvestorId();
	CPortfolioOrderPlacer* OrderPlacer() { return m_strategy->OrderPlacer(); }

	entity::StrategyType StrategyType(){ return m_strategy->Type(); }
	trade::HedgeFlagType HedgeFlag(){ return m_hedgeFlag; }
	void SetHedgeFlag(trade::HedgeFlagType hedgeType);
	CStrategy* Strategy(){ return m_strategy.get(); }
	const entity::PortfolioUpdateItem& GetUpdated(){ return m_portfolioUpdate; }

	// operation
	void StartStrategy(int lastOrderId);
	void StopStrategy();
	void StopStrategyDueTo(const string& stopReason);
	void PauseStrategy();
	void ResumeStrategy();
	void EnableTrigger(int triggerIdx, bool enabled);
	void SetQuantity(int perOpenQty, int perStartQty, int totalOpenLimit, int maxCancelQty);
	void SetEndTimePoints(vector<string>& timepoints);
	void StrategyForceOpen();
	void StrategyForceClose(const string& mlOrderId, int quantity);
	void PushMessage(const string& msg);
	void SwitchStopLoss(bool enableStopLoss){ m_stopLoss.store(true, boost::memory_order_release); }
	void SwitchStopGain(bool enableStopGain){ m_stopGain.store(true, boost::memory_order_release); }
	bool StopLossEnabled() { return m_stopLoss.load(boost::memory_order_acquire); }
	bool StopGainEnabled() { return m_stopGain.load(boost::memory_order_acquire); }

	// legs
	vector<LegPtr>& Legs(){ return m_legs;}
	CLeg* GetLeg(int legId);
	CLeg* GetLeg(const string& symbol);

	// for quote
	void SubscribeQuotes(CQuoteRepositry* pQuoteRepo);
	void SubscribeQuotes(CSimpleQuoteReceiver* pQuoteReceiver);
	// Need to be done before remove from manager
	void Cleanup();

	// for order placer
	int NewOrderId(string& newId);
	void AddPosition(const trade::MultiLegOrder& openOrder, int actualTradedVol);
	void RemovePosition(const trade::MultiLegOrder& closeOrder, int actualTradedVol);
	void NotifyLegFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume, int execId);
	void NotifyLegCanceled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, int execId);

	int IncrementalCancelTimes(int amount){ m_cancelTimes += amount; return m_cancelTimes; }
	void UpdatePosition();
	void CheckOpenCancelLimit();

	double CalculateDiff(ARBI_DIFF_CALC* structArbiDiffCalc, CALC_DIFF_METHOD calcDiffMethod);
	bool LegsTimestampAligned();
	void PrintLegsQuote();
private:
	void AddLeg(const entity::LegItem& legItem);
	void PrepareTriggerUpdate();
	void InitOpenCancelLimit( const entity::PortfolioItem &srcPortfolioItem );
	void InitEndTimePoints( const entity::PortfolioItem& srcPortfolioItem);
	void CheckForStop(const string& quoteUpdateTime);
	void CheckForStart(const string& quoteUpdateTime);
	void StartStrategyDueTo(const string& startReason);
	void PauseStrategyDueTo(const string& pauseReason);
	
	void OnQuoteRecevied(boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote);
	
	void GetLegUpdate();
	void GetStatisticsUpdate();
	void PushUpdate();

	void AddProfit(double val){ m_profit += val; }
	void SetProfit(double val){ m_profit = val; }
	void SetAvgCost(double avgCost){ m_avgCost = avgCost; }
	int IncrementalOpenTimes(int opened){ m_openTimes += opened; m_totalOpenTimes += opened; return m_totalOpenTimes; }
	int IncrementalCloseTimes(int closed){ m_totalCloseTimes += closed; return m_totalCloseTimes; }
	
	// backup PortfolioItem which this is created from
	entity::PortfolioItem m_portfolioItem;
	entity::PortfolioUpdateItem m_portfolioUpdate;
	StrategyPtr m_strategy;
	vector<LegPtr> m_legs;

	// for quote subscription
	CQuoteRepositry* m_pQuoteRepo;
	vector<CQuoteFetcher*> m_quoteFetcherVec;
	CSimpleQuoteReceiver* m_pQuoteReceiver;

	// limits
	int m_maxOpenPerStart;
	int m_maxCancel;
	int m_totalOpenLimit;

	// HedgeFlag
	trade::HedgeFlagType m_hedgeFlag;
	boost::atomic<bool> m_stopGain;
	boost::atomic<bool> m_stopLoss;

	// end time points
	vector<string> m_endTimePoints;
	vector<string> m_beginTimePoints;
	string m_targetEnd;
	bool m_lastTargetEnd;
	boost::mutex m_endTimeMutex;

	// statistics
	int m_openTimes;
	int m_totalOpenTimes;
	int m_totalCloseTimes;
	int m_currentPosition;
	int m_cancelTimes;
	int m_serialOrderId;
	double m_profit;
	double m_avgCost;
	boost::mutex m_mutStat;
	boost::mutex m_mutPushingMsg;

	CAvatarClient* m_avatar;
};

typedef boost::shared_ptr<CPortfolio> PortfolioPtr;

