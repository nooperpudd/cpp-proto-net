#include "StdAfx.h"
#include "ArbitrageStrategy.h"
#include "ArbitrageTrigger.h"
#include "PriceBarDataProxy.h"
#include "globalmembers.h"
#include "PortfolioArbitrageOrderPlacer.h"
#include "DoubleCompare.h"

enum  { TIMES_OF_PRICE_TICK = 4 };

entity::PosiDirectionType GetMlOrderDirection(const trade::MultiLegOrder& mlOrder)
{
	if(mlOrder.offset() == trade::ML_OF_OPEN)
	{
		if(mlOrder.legs(0).direction() == trade::BUY)
			return entity::LONG;
		else
			return entity::SHORT;
	}
	else if(mlOrder.offset() == trade::ML_OF_CLOSE)
	{
		if(mlOrder.legs(0).direction() == trade::SELL)
			return entity::LONG;
		else
			return entity::SHORT;
	}
	else
		return entity::NET;
}

CArbitrageStrategy::CArbitrageStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_timeFrame(60)
	, m_bollPeriod(26)
	, m_stdDevMultiplier(2)
	, m_lastDiff(0)
	, m_longDiff(0)
	, m_longDiffFast(0)
	, m_longDiffSize(0)
	, m_shortDiff(0)
	, m_shortDiffFast(0)
	, m_shortDiffSize(0)
	, m_bollTop(0)
	, m_bollBottom(0)
	, m_costDiff(0)
	, m_targetGain(0.8)
	, m_minStep(0.2)
	, m_useTargetGain(true)
	, m_allowPending(true)
	, m_longPosition(0)
	, m_longAvgCost(0)
	, m_shortPosition(0)
	, m_shortAvgCost(0)
	, m_lastStopLossDirection(entity::NET)
	, m_notOpenInStopLossDirection(true)
	, m_closePositionPurpose(CLOSE_POSITION_UNKNOWN)
	, m_volumeToClose(0)
{
	InitForTargetGain(pPortfolio);

	Apply(strategyItem, false);

	CreateTriggers(strategyItem);
}


CArbitrageStrategy::~CArbitrageStrategy(void)
{
}

void CArbitrageStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{
	for (int i = 0; i < strategyItem.triggers_size(); ++i)
	{
		TriggerPtr trigger(new CArbitrageTrigger(strategyItem.triggers(i)));
		m_triggers.push_back(trigger);
	}
}

void CArbitrageStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);

	// TODO
	m_bollPeriod = (int)strategyItem.ar_bollperiod();
	m_stdDevMultiplier = (int)strategyItem.ar_stddevmultiplier();
	m_openTimeout = strategyItem.opentimeout();
	m_retryTimes = strategyItem.retrytimes();

	// make sure following parameters having values
	if(m_openTimeout == 0)
		m_openTimeout = 350;
	if(m_retryTimes == 0)
		m_retryTimes = 8;

	if(!withTriggers)
	{
		// don't touch hist data source when editing strategy
		PrepareHistDataSrc(strategyItem);

		// Initialize Indicator set
		const vector<CPriceBarDataProxy*>& dataProxies = DataProxies();
		if(dataProxies.size() > 1)
		{
			m_diffRecordSet = DiffRecordSetPtr(
				new CDiffRecordSet(
				dataProxies[0]->GetOHLCRecordSet(),
				dataProxies[1]->GetOHLCRecordSet()
			));

			int histDataSize = dataProxies[0]->GetRecordSetSize();
			m_bollDataSet = BollDataSetPtr(new CBollDataSet(histDataSize, m_bollPeriod, m_stdDevMultiplier));

			CalculateEndBar(1, m_timeFrame, histDataSize);
		}
	}
}

void CArbitrageStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	// a mutex to protect from unexpected applying strategy settings concurrently
	boost::mutex::scoped_lock l(m_mut);
	
	/*
	pQuote->set_update_time("09:15:00");
	if(pQuote->symbol() == "IF1408")
	{
		pQuote->set_last(2242.6);
	}
	else
	{
		pQuote->set_last(2246.6);
	}
	*/

	CTechAnalyStrategy::Test(pQuote, pPortfolio, timestamp);

	if (!IsMarketOpen(pQuote))
		return;

	string symbol = pQuote->symbol();
	COHLCRecordSet* ohlc = GetRecordSet(symbol, m_timeFrame, timestamp);
	if (ohlc == NULL)
		return;

	int currentBarIdx = m_diffRecordSet->Calculate(ohlc);
	if (currentBarIdx < m_bollPeriod)
		return;

	ARBI_DIFF_CALC structLastDiff = { LAST_DIFF, 0, 0, 0 };
	ARBI_DIFF_CALC structLongDiff = { LONG_DIFF, 0, 0, 0 };
	ARBI_DIFF_CALC structShortDiff = { SHORT_DIFF, 0, 0, 0 };
	ARBI_DIFF_CALC structLongDiffFast = { LONG_DIFF, 0, 0, 0 };
	ARBI_DIFF_CALC structShortDiffFast = { SHORT_DIFF, 0, 0, 0 };

	CALC_DIFF_METHOD calcMethod = m_allowPending ? BETTER_PRICE : FAST_DEAL;

	m_lastDiff = pPortfolio->CalculateDiff(&structLastDiff, calcMethod);
	m_longDiff = pPortfolio->CalculateDiff(&structLongDiff, calcMethod);
	m_shortDiff = pPortfolio->CalculateDiff(&structShortDiff, calcMethod);

	m_longDiffFast = pPortfolio->CalculateDiff(&structLongDiffFast, FAST_DEAL);
	m_shortDiffFast = pPortfolio->CalculateDiff(&structShortDiffFast, FAST_DEAL);

	// TODO: will be figured out in the future
	m_longDiffSize = 0;
	m_shortDiffSize = 0;

	m_bollDataSet->Calculate(m_diffRecordSet.get());

	double actualMid = 0;
	if (m_useTargetGain)
	{
		double bollMid = m_bollDataSet->GetRef(IND_MID, 1);
		actualMid = CalcBoundaryByTargetGain(bollMid, m_targetGain, m_minStep, &m_bollTop, &m_bollBottom);
	}
	else
	{
		m_bollTop = m_bollDataSet->GetRef(IND_TOP, 1);
		m_bollBottom = m_bollDataSet->GetRef(IND_BOTTOM, 1);
	}

	if (!IsRunning())
		return;

	CPortfolioArbitrageOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioArbitrageOrderPlacer*>(pPortfolio->OrderPlacer());

	// if working with order, don't need to test strategy but check for retry
	if (pOrderPlacer->IsWorking())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Strategy - Check and likely retry submit order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	if (!pPortfolio->LegsTimestampAligned())
		return;

	entity::PosiDirectionType direction = GetTradeDirection();
	entity::PosiDirectionType directionFast = GetFastTradeDirection();

	if(m_useTargetGain)
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Testing Direction - Mid:%.2f, longDiff:%.2f vs bottom:%.2f, shortDiff:%.2f vs top:%.2f -->> %s")
			% pPortfolio->InvestorId() % pPortfolio->ID() % actualMid % m_longDiff % m_bollBottom % m_shortDiff % m_bollTop % GetPosiDirectionText(direction)));
	}
	else
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Testing trade direction - longDiff:%.2f vs bottom:%.2f, shortDiff:%.2f vs top:%.2f -->> %s")
			% pPortfolio->InvestorId() % pPortfolio->ID() % m_longDiff % m_bollBottom % m_shortDiff % m_bollTop % GetPosiDirectionText(direction)));
	}

	if (directionFast != entity::NET)
	{
		if (m_useTargetGain)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Testing Fast Direction - Mid:%.2f, longDiffFast:%.2f vs bottom:%.2f, shortDiffFast:%.2f vs top:%.2f -->> %s")
				% pPortfolio->InvestorId() % pPortfolio->ID() % actualMid % m_longDiffFast % m_bollBottom % m_shortDiffFast % m_bollTop % GetPosiDirectionText(directionFast)));
		}
		else
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Testing Fast direction - longDiffFast:%.2f vs bottom:%.2f, shortDiffFast:%.2f vs top:%.2f -->> %s")
				% pPortfolio->InvestorId() % pPortfolio->ID() % m_longDiffFast % m_bollBottom % m_shortDiffFast % m_bollTop % GetPosiDirectionText(directionFast)));
		}
	}

	if (pOrderPlacer->IsOpened())
	{
		bool meetCloseCondition = false;
		entity::PosiDirectionType side = pOrderPlacer->PosiDirection();
		ARBI_DIFF_CALC& diffPrices = side == entity::LONG ? structShortDiff : structLongDiff;
		bool forceClosing = IsForceClosing() || OutOfTradingWindow(currentBarIdx);
		if(forceClosing) // This close condition check is only effective on the bar after open
		{
			m_closePositionPurpose = CLOSE_POSITION_FORCE;
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Forcibly Closing position")
				% pPortfolio->InvestorId() % pPortfolio->ID()));
			ARBI_DIFF_CALC forceClosePx = { side == entity::LONG ? SHORT_DIFF : LONG_DIFF, 0, 0, 0};
			pPortfolio->CalculateDiff(&forceClosePx, FAST_DEAL);
			pPortfolio->PrintLegsQuote();
			ClosePosition(pOrderPlacer, forceClosePx, pQuote, timestamp, "�ֶ�ƽ��", trade::SR_Manual); 
			return;
		}
		
		// Stop gain/loss logic in ArbitrageStrategy
		if (directionFast != entity::NET && side != directionFast)
		{
			diffPrices = side == entity::LONG ? structShortDiffFast : structLongDiffFast;
			
			if (side == entity::LONG)
			{
				// Fast Stop Gain
				m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
				string logTxt = boost::str(boost::format("Fast StopGain: Fast Short diff(%.2f) above bollTop(%.2f) -> Stop Gain") % m_shortDiffFast % m_bollTop);
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("�Լۿռ۲�(%.2f)�����Ϲ�(%.2f) -> ֹӯƽ��") % m_shortDiffFast % m_bollTop);
				ClosePosition(pOrderPlacer, diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
			}
			else if (side == entity::SHORT)
			{
				// Fast Stop Gain
				m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
				string logTxt = boost::str(boost::format("Fast StopGain: Fast Long diff(%.2f) below bollBottom(%.2f) -> Stop Gain") % m_longDiffFast % m_bollBottom);
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("�Լ۶�۲�(%.2f)С���¹�(%.2f) -> ֹӯƽ��") % m_longDiffFast % m_bollBottom);
				ClosePosition(pOrderPlacer, diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
			}
		}
		else if(direction != entity::NET)
		{
			if(side == entity::LONG)
			{
				if(m_bollTop <= m_costDiff)
				{
					// Stop Loss
					m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
					string logTxt = boost::str(boost::format("bollTop(%.2f) <= costDiff(%.2f) -> Stop Loss") % m_bollTop % m_costDiff);
					LOG_DEBUG(logger, logTxt);
					pPortfolio->PrintLegsQuote();
					string comment = boost::str(boost::format("�Ϲ�(%.2f)���ڵ��ڳɱ�(%.2f) -> ֹ��ƽ��") % m_bollTop % m_costDiff);
					ClosePosition(pOrderPlacer, diffPrices, pQuote, timestamp, comment, trade::SR_StopLoss);
				}
				else if(side != direction)
				{
					// Stop Gain
					m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
					string logTxt = boost::str(boost::format("Short diff(%.2f) above bollTop(%.2f) -> Stop Gain") % m_shortDiff % m_bollTop);
					LOG_DEBUG(logger, logTxt);
					pPortfolio->PrintLegsQuote();
					string comment = boost::str(boost::format("�ռ۲�(%.2f)�����Ϲ�(%.2f) -> ֹӯƽ��") % m_shortDiff % m_bollTop);
					ClosePosition(pOrderPlacer, diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
				}
			}
			else if(side == entity::SHORT)
			{
				if(m_bollBottom >= m_costDiff)
				{
					// Stop Loss
					m_closePositionPurpose = CLOSE_POSITION_STOP_LOSS;
					string logTxt = boost::str(boost::format("bollBottom(%.2f) >= costDiff(%.2f) -> Stop Loss") % m_bollBottom % m_costDiff);
					LOG_DEBUG(logger, logTxt);
					pPortfolio->PrintLegsQuote();
					string comment = boost::str(boost::format("�¹�(%.2f)���ڵ��ڳɱ�(%.2f) -> ֹ��ƽ��") % m_bollBottom % m_costDiff);
					ClosePosition(pOrderPlacer, diffPrices, pQuote, timestamp, comment, trade::SR_StopLoss);
				}
				else if(side != direction)
				{
					// Stop Gain
					m_closePositionPurpose = CLOSE_POSITION_STOP_GAIN;
					string logTxt = boost::str(boost::format("Long diff(%.2f) below bollBottom(%.2f) -> Stop Gain") % m_longDiff % m_bollBottom);
					LOG_DEBUG(logger, logTxt);
					pPortfolio->PrintLegsQuote();
					string comment = boost::str(boost::format("��۲�(%.2f)С���¹�(%.2f) -> ֹӯƽ��") % m_longDiff % m_bollBottom);
					ClosePosition(pOrderPlacer, diffPrices, pQuote, timestamp, comment, trade::SR_StopGain);
				}
			}
		}

		return;
	}

	// Test for open position of portfolio
	//bool forceOpening = IsForceOpening();
	if(!OutOfTradingWindow(currentBarIdx))
	{
		if (directionFast > entity::NET)
		{
			if (m_notOpenInStopLossDirection && directionFast == m_lastStopLossDirection)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) does NOT open position same as last stop loss direction (%s)")
					% pPortfolio->InvestorId() % pPortfolio->ID() % GetPosiDirectionText(m_lastStopLossDirection)));
				return;
			}

			if (!pOrderPlacer->IsWorking())
			{
				m_closePositionPurpose = CLOSE_POSITION_UNKNOWN;
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) FAST DEAL Opening position at bar %d")
					% pPortfolio->InvestorId() % pPortfolio->ID() % currentBarIdx));
				pPortfolio->PrintLegsQuote();
				OpenPosition(direction, pOrderPlacer, (direction == entity::LONG ? structLongDiffFast : structShortDiffFast),
					pQuote, timestamp);
				return;
			}
		}
		else if (direction > entity::NET)
		{
			if (m_notOpenInStopLossDirection && direction == m_lastStopLossDirection)
			{
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) does NOT open position same as last stop loss direction (%s)")
					% pPortfolio->InvestorId() % pPortfolio->ID() % GetPosiDirectionText(m_lastStopLossDirection)));
				return;
			}

			if (!pOrderPlacer->IsWorking())
			{
				m_closePositionPurpose = CLOSE_POSITION_UNKNOWN;
				LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) Opening position at bar %d")
					% pPortfolio->InvestorId() % pPortfolio->ID() % currentBarIdx));
				pPortfolio->PrintLegsQuote();
				OpenPosition(direction, pOrderPlacer, (direction == entity::LONG ? structLongDiff : structShortDiff),
					pQuote, timestamp);
				return;
			}
		}
	}
}

void CArbitrageStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_ar_diff(m_lastDiff);
	pPortfUpdateItem->set_ar_longdiff(m_longDiff);
	pPortfUpdateItem->set_ar_longsize(m_longDiffSize);
	pPortfUpdateItem->set_ar_shortdiff(m_shortDiff);
	pPortfUpdateItem->set_ar_shortsize(m_shortDiffSize);

	pPortfUpdateItem->set_ar_bolltop(m_bollTop);
	pPortfUpdateItem->set_ar_bollbottom(m_bollBottom);
}

int CArbitrageStrategy::OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol)
{
	int qty = actualTradedVol;
	m_volumeToClose = actualTradedVol;
	m_lastStopLossDirection = entity::NET;
	double cost = CalcMlOrderCost(openOrder);

	entity::PosiDirectionType posiDirection = GetMlOrderDirection(openOrder);
	if(posiDirection == entity::LONG)
	{
		int origQty = m_longPosition;
		double newAvgCost = (m_longAvgCost * origQty + cost * qty) / (origQty + qty);
		m_longAvgCost = newAvgCost;
		m_longPosition += qty;
	}
	else if(posiDirection == entity::SHORT)
	{
		int origQty = m_shortPosition;
		double newAvgCost = (m_shortAvgCost * origQty + cost * qty) / (origQty + qty);
		m_shortAvgCost = newAvgCost;
		m_shortPosition += qty;
	}
	
	return IncrementOpenTimes(pPortfolio, qty);
}

int CArbitrageStrategy::OnPortfolioRemovePosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& closeOrder, int actualTradedVol)
{
	int qty = actualTradedVol;
	m_volumeToClose = 0;
	double cost = CalcMlOrderCost(closeOrder);
	LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Strategy - Portfolio(%s) Removed position and Reset CostDiff")
		% pPortfolio->InvestorId() % pPortfolio->ID()));
	m_costDiff = 0;

	entity::PosiDirectionType posiDirection = GetMlOrderDirection(closeOrder);
	if(posiDirection == entity::LONG)
	{
		double orderProfit = (cost - m_longAvgCost) * qty;
		AddProfit(pPortfolio, orderProfit);

		int origQty = m_longPosition;
		int remaing = origQty - qty;
		if(remaing > 0)
		{
			double newAvgCost = (m_longAvgCost * origQty - cost * qty) / remaing;
			m_longAvgCost = newAvgCost;
		}
		else
		{
			m_longAvgCost = 0;
		}
		m_longPosition = remaing;
	}
	else if(posiDirection == entity::SHORT)
	{
		double orderProfit = (m_shortAvgCost - cost) * qty;
		AddProfit(pPortfolio, orderProfit);

		int origQty = m_shortPosition;
		int remaing = origQty - qty;
		if(remaing > 0)
		{
			double newAvgCost = (m_shortAvgCost * origQty - cost * qty) / remaing;
			m_shortAvgCost = newAvgCost;
		}
		else
		{
			m_shortAvgCost = 0;
		}
		m_shortPosition = remaing;
	}

	if (IfStopLossClosePosition())
	{
		m_lastStopLossDirection = posiDirection;
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Arbitrage Trend - Portfolio(%s) SAVING last stop loss direction (%s)")
			% pPortfolio->InvestorId() % pPortfolio->ID() % GetPosiDirectionText(m_lastStopLossDirection)));
	}

	return IncrementCloseTimes(pPortfolio, qty);
}

bool CArbitrageStrategy::IfStopLossClosePosition()
{
	return m_closePositionPurpose == CLOSE_POSITION_STOP_LOSS;
}

double CArbitrageStrategy::CalcMlOrderCost( const trade::MultiLegOrder& openOrder )
{
	double cost = 0;
	int legCount = openOrder.legs_size();
	if(legCount > 1)
	{
		const trade::Order& firstLeg = openOrder.legs(0);
		double firstLegPx = firstLeg.limitprice();

		const trade::Order& secondLeg = openOrder.legs(1);
		double secondLegPx = secondLeg.limitprice();

		cost = firstLegPx - secondLegPx;
	}

	return cost;
}

const string& CArbitrageStrategy::GetAnotherLegSymbol( const string& symb, const vector<LegPtr>& legs )
{
	static string emptySymb;
	BOOST_FOREACH(const LegPtr& l, legs)
	{
		if(l->Symbol() != symb)
			return l->Symbol();
	}
	return emptySymb;
}

entity::PosiDirectionType CArbitrageStrategy::GetTradeDirection()
{
	entity::PosiDirectionType direction = entity::NET;

	if(DoubleLessEqual(m_longDiff, m_bollBottom))
		direction = entity::LONG;
	else if(DoubleGreaterEqual(m_shortDiff, m_bollTop))
		direction = entity::SHORT;
	/*
	if(m_longDiff < m_bollBottom)
		direction = entity::LONG;
	else if(m_shortDiff > m_bollTop)
		direction = entity::SHORT;
	*/
	return direction;
}

entity::PosiDirectionType CArbitrageStrategy::GetFastTradeDirection()
{
	entity::PosiDirectionType direction = entity::NET;

	if (m_longDiffFast < m_bollBottom)
		direction = entity::LONG;
	else if (m_shortDiffFast > m_bollTop)
		direction = entity::SHORT;
	return direction;
}

void CArbitrageStrategy::OpenPosition( entity::PosiDirectionType direction, CPortfolioArbitrageOrderPlacer* pOrderPlacer, ARBI_DIFF_CALC diffPrices, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	if(direction > entity::NET)
	{
		double lmtPrice[2];
		if(direction == entity::LONG)
		{
			lmtPrice[0] = diffPrices.BuyPrice;
			lmtPrice[1] = diffPrices.SellPrice;
		}
		else if(direction == entity::SHORT)
		{
			lmtPrice[0] = diffPrices.SellPrice;
			lmtPrice[1] = diffPrices.BuyPrice;
		}

		LOG_DEBUG(logger, boost::str(boost::format("Arbitrage Trend - %s Open position @ %.2f - %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % lmtPrice[1] % pQuote->update_time()));

		string openComment;
		if(direction == entity::LONG)
		{
			openComment = boost::str(boost::format("��۲� %.2f �����¹� %.2f ���� (%s)") % diffPrices.Diff % m_bollBottom % pQuote->update_time());
		}
		else if(direction == entity::SHORT)
		{
			openComment = boost::str(boost::format("�ռ۲� %.2f �����Ϲ� %.2f ���� (%s)") % diffPrices.Diff % m_bollTop % pQuote->update_time());
		}
		// TODO feed comment
		pOrderPlacer->SetMlOrderStatus(openComment);
		pOrderPlacer->OpenPosition(direction, lmtPrice, 2, timestamp, trade::SR_AutoOpen);
		m_costDiff = diffPrices.Diff;
		ResetForceOpen();
	}
}

void CArbitrageStrategy::ClosePosition( CPortfolioArbitrageOrderPlacer* pOrderPlacer, ARBI_DIFF_CALC diffPrices, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment, trade::SubmitReason reason )
{
	if(pOrderPlacer != NULL)
	{
		entity::PosiDirectionType direction = pOrderPlacer->PosiDirection();

		double lmtPrice[2];
		if(direction == entity::LONG)
		{
			lmtPrice[0] = diffPrices.SellPrice;
			lmtPrice[1] = diffPrices.BuyPrice;
		}
		else if(direction == entity::SHORT)
		{
			lmtPrice[0] = diffPrices.BuyPrice;
			lmtPrice[1] = diffPrices.SellPrice;
		}

		LOG_DEBUG(logger, boost::str(boost::format("Arbitrage Trend - %s Close position @ %.2f - %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % lmtPrice[1]  % pQuote->update_time()));

		pOrderPlacer->ClosePosition(m_volumeToClose, direction, lmtPrice, 2, timestamp, reason);

		ResetForceClose();
		pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s ƽ�� @ %.2f - %.2f")
			% comment % GetPosiDirectionText(direction, true) % lmtPrice[0] % lmtPrice[1]));

	}
}

void CArbitrageStrategy::OnBeforeAddingHistSrcConfig( CHistSourceCfg* pHistSrcCfg )
{
	if(pHistSrcCfg != NULL)
	{
		if(pHistSrcCfg->Precision == m_timeFrame)
			pHistSrcCfg->HistData = true;
	}
}

double GetLowerValue(double val, double offset)
{
	int iVal = (int)(val * 10);
	int iOffset = (int)(offset * 10);
	int factor = iVal / iOffset;
	double ret = (double)(iOffset * factor) / 10.0;
	return ret;
}

const double CArbitrageStrategy::CalcBoundaryByTargetGain( double mid, double targetGain, double step, double* outUpper, double* outLower )
{
	double halfGain = targetGain / 2;
	double actualMid = GetLowerValue(mid, step);
	*outUpper = actualMid + halfGain;
	*outLower = actualMid - halfGain;

	return actualMid;
}

void CArbitrageStrategy::InitForTargetGain(CPortfolio* pPortfolio)
{
	assert(pPortfolio != NULL);
	if (pPortfolio->Count() > 1)
	{
		m_minStep = pPortfolio->GetLeg(1)->MinPriceChange();
		m_targetGain = TIMES_OF_PRICE_TICK * m_minStep;
	}
}




