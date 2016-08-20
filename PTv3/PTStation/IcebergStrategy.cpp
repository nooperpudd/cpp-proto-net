#include "stdafx.h"
#include "IcebergStrategy.h"
#include "globalmembers.h"
#include "SettingChangeTrace.h"
#include "PortfolioScalperOrderPlacer.h"
#include "Leg.h"
#include "Portfolio.h"

CIcebergStrategy::CIcebergStrategy()
	: m_priceTick(0.2)
	, m_pxDiffThreshold(2)
	, m_sizeDiffThreshold(10)
	, m_targetGainPercent(0.2)
{
}


CIcebergStrategy::~CIcebergStrategy()
{
}

void CIcebergStrategy::Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers)
{
	Apply(strategyItem, false);
}

void CIcebergStrategy::Apply(const entity::StrategyItem& strategyItem, bool withTriggers)
{
	if (withTriggers)
	{
		logger.Debug("Applying Iceberg Strategy...");
#ifdef LOG_FOR_TRADE
		PrintDoubleChange("Price Tick", m_priceTick, strategyItem.ic_pricetick());
		PrintDoubleChange("Price Diff Threshold", m_pxDiffThreshold, strategyItem.ic_pxdiffthreshold());
		PrintIntegerChange("Size Diff Threshold", m_sizeDiffThreshold, strategyItem.ic_sizediffthreshold());
		PrintDoubleChange("Target Gain Percent", m_targetGainPercent, strategyItem.ic_targetgain());
#endif
	}

	CStrategy::Apply(strategyItem, withTriggers);

	m_priceTick = strategyItem.ic_pricetick();
	m_pxDiffThreshold = strategyItem.ic_pxdiffthreshold();
	m_sizeDiffThreshold = strategyItem.ic_sizediffthreshold();
	m_targetGainPercent = strategyItem.ic_targetgain();
	m_userId = strategyItem.ic_userid();
}

double RoundByStep(double val, double step)
{
	double val1 = (int)(val / step) * step;
	double rounded = (val - val1 - (step / 2)) > 0.001 ? val1 + step : val1;
	return rounded;
}

bool CIcebergStrategy::GetPriceOffset(entity::PosiDirectionType direction, double* askOffset, double* bidOffset)
{
	bool succ = false;

	double halfGap = RoundByStep(m_priceGap / 2, m_priceTick);
	double targetGain = RoundByStep(m_priceGap * m_targetGainPercent, m_priceTick);

	if (halfGap - targetGain > 0.001)
	{
		if (direction == entity::LONG)
		{
			*askOffset = halfGap;
			*bidOffset = halfGap - targetGain;
		}
		else
		{
			*bidOffset = halfGap;
			*askOffset = halfGap - targetGain;
		}
		succ = true;
	}

	return succ;
}

void CIcebergStrategy::Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	m_priceGap = pQuote->ask() - pQuote->bid();
	m_ask = pQuote->ask();
	m_askSize = pQuote->ask_size();
	m_bid = pQuote->bid();
	m_bidSize = pQuote->bid_size();

	entity::PosiDirectionType direction = entity::NET;
	if (m_askSize > m_bidSize)
	{
		m_sizeGap = m_askSize - m_bidSize;
		if (m_sizeGap >= m_sizeDiffThreshold)
			direction = entity::LONG;
	}
	else if (m_askSize < m_bidSize)
	{
		m_sizeGap = m_bidSize - m_askSize;
		if (m_sizeGap >= m_sizeDiffThreshold)
			direction = entity::SHORT;
	}

	if (m_askSize > 0 && m_bidSize > 0)
	{
		CPortfolioOrderPlacer* pOrderPlacer = pPortfolio->OrderPlacer();

		if (IsRunning() && !IsSuspending())
		{
			if (!(pOrderPlacer->IsWorking()) && direction > entity::NET)
			{
				double askOffset = 0, bidOffset = 0;
				bool succ = GetPriceOffset(direction, &askOffset, &bidOffset);
				if (succ)
				{
					double lmtPrice[2];
					if (direction == entity::LONG)
					{
						lmtPrice[0] = m_bid + bidOffset;
						lmtPrice[1] = m_ask - askOffset;
					}
					else // Sell
					{
						lmtPrice[0] = m_ask - askOffset;
						lmtPrice[1] = m_bid + bidOffset;
					}
					pOrderPlacer->Run(direction, lmtPrice, 2, timestamp);
#ifdef LOG_FOR_TRADE
					logger.Info(boost::str(boost::format("[%s] A: %.2f, %d  vs  B: %.2f, %d => Ao: %.2f  Bo: %.2f")
						% (direction > entity::NET ? (direction == entity::LONG ? "LONG" : "SHORT") : "IGNORE")
						% m_ask % m_askSize % m_bid % m_bidSize % askOffset % bidOffset));
#endif
				}
			}
		}

		if (pOrderPlacer->IsWorking())
		{
			pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		}
	}

	// there is only ONE leg for scalper strategy 
	const LegPtr& leg = pPortfolio->Legs().at(0);
	leg->UpdateLast(pQuote->last());
	leg->UpdateAsk(pQuote->ask());
	leg->UpdateAskSize(pQuote->ask_size());
	leg->UpdateBid(pQuote->bid());
	leg->UpdateBidSize(pQuote->bid_size());
	leg->UpdateTimestamp(pQuote->update_time(), pQuote->update_millisec());
}

void CIcebergStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem)
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);
	pPortfUpdateItem->set_ic_pricegap(m_priceGap);
	pPortfUpdateItem->set_ic_sizegap(m_sizeGap);
}

int CIcebergStrategy::OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol)
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}

CPortfolioOrderPlacer* CIcebergStrategy::CreateOrderPlacer()
{
	return new CPortfolioScalperOrderPlacer;
}
