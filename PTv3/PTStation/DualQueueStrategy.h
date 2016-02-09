#pragma once

#include "Strategy.h"

class CPortfolioQueueOrderPlacer;

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
	virtual void OnLegFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume);
	virtual void OnLegCanceled(int sendingIdx, const string & symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction);

protected:

	void OpenPosition(CPortfolioQueueOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening);
	
	CPortfolioOrderPlacer* CreateOrderPlacer();

	virtual bool OnStart();
	virtual void OnStop();

private:
	void OnStrategyError(CPortfolio * portf, const string & errorMsg) const;
	bool IfQuotingStable(entity::Quote* pQuote);

	enum DQ_STATUS 
	{
		DQ_UNOPENED = 0,
		DQ_IS_OPENING,
		DQ_OPENED,
		DQ_IS_CLOSING,
		DQ_CLOSED
	};

	double m_lastAsk;
	double m_lastBid;
	int m_stableQuoteCount;
	
	bool m_stableQuote;
	boost::atomic<DQ_STATUS> m_status;

	boost::mutex m_mut;

	double m_priceTick;
	int m_stableTickThreshold;
	int m_minWorkingSize;
	entity::PosiDirectionType m_direction;

	bool m_stopping;
};

