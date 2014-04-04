#include "StdAfx.h"
#include "RangeTrendStrategy.h"
#include "PriceBarDataProxy.h"
#include "globalmembers.h"
#include "SymbolTimeUtil.h"
#include "TechStrategyDefs.h"
#include "Portfolio.h"
#include "PortfolioTrendOrderPlacer.h"
#include "OHLCRecordSet.h"
#include "DoubleCompare.h"



CRangeTrendStrategy::CRangeTrendStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CTechAnalyStrategy(strategyItem, pAvatar)
	, m_timeFrame(300)
	, m_marketOpen(false)
	, m_openPeriod(20)
	, m_closePeriod(10)
	, m_stopLossFactor(2.0)
	, m_trendFactor(2.0)
	, m_upperBoundOpen(DOUBLE_MAX_PRICE)
	, m_lowerBoundOpen(DOUBLE_MIN_PRICE)
	, m_upperBoundClose(DOUBLE_MAX_PRICE)
	, m_lowerBoundClose(DOUBLE_MIN_PRICE)
	, m_currentHigh(0.0)
	, m_currentLow(0.0)
	, m_NATR(0.0)
	, m_StopLoss(0.0)
	, m_fireAtNextBar(false)
	, m_lastBarIdx(-1)
{
}


CRangeTrendStrategy::~CRangeTrendStrategy(void)
{
}

void CRangeTrendStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	boost::mutex::scoped_lock l(m_mut);

	CTechAnalyStrategy::Apply(strategyItem, withTriggers);

	//m_timeFrame = strategyItem.rt_timeframe();
	//m_openPeriod = strategyItem.rt_openperiod();
	//m_closePeriod = strategyItem.rt_closeperiod();
	//m_stopLossFactor = strategyItem.rt_stoplossfactor();
	//m_trendFactor = strategyItem.rt_trendfactor();
	
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
		for(vector<CPriceBarDataProxy*>::const_iterator iter = dataProxies.begin(); iter != dataProxies.end(); ++iter)
		{
			if((*iter)->Precision() == m_timeFrame)
			{
				m_openDonchianDataSet = DonchianDataSetPtr(new CDonchianDataSet((*iter)->GetRecordSetSize(), m_openPeriod));
				m_closeDonchianDataSet = DonchianDataSetPtr(new CDonchianDataSet((*iter)->GetRecordSetSize(), m_closePeriod));
				m_atrDataSet = ATRDataSetPtr(new CATRDataSet((*iter)->GetRecordSetSize(), m_closePeriod));
			}
		}
	}
}

void CRangeTrendStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	// a mutex to protect from unexpected applying strategy settings concurrently
	boost::mutex::scoped_lock l(m_mut);

	//pQuote->set_last(2466);
	//pQuote->set_update_time("09:15:12");

	CTechAnalyStrategy::Test(pQuote, pPortfolio, timestamp);

	if(!IsRunning())
		return;

	if(!m_marketOpen)
	{
		string symbol = pQuote->symbol();
		string quoteUpdateTime = pQuote->update_time();
		bool isIF = isSymbolIF(symbol);
		string targetBeginTime = isIF ? IF_START_1 : NON_IF_START_1;
		if(quoteUpdateTime.compare(targetBeginTime) >= 0)
		{
			m_marketOpen = true;
		}
		else
		{
			return;
		}
	}

	string symbol = pQuote->symbol();

	COHLCRecordSet* ohlc = GetRecordSet(symbol, m_timeFrame, timestamp);
	if(ohlc == NULL)
		return;

	//int forceCloseBar = ohlc->GetSize() - m_forceCloseOffset;

	int currentBarIdx = ohlc->GetEndIndex();

	if(currentBarIdx <= m_openPeriod)
		return;	// too few bars to test

	m_openDonchianDataSet->Calculate(ohlc);
	m_upperBoundOpen = m_openDonchianDataSet->GetRef(IND_DONCHIAN_Hi, 1);
	m_lowerBoundOpen = m_openDonchianDataSet->GetRef(IND_DONCHIAN_Lo, 1);

	m_closeDonchianDataSet->Calculate(ohlc);
	m_upperBoundClose = m_closeDonchianDataSet->GetRef(IND_DONCHIAN_Hi, 1);
	m_lowerBoundClose = m_closeDonchianDataSet->GetRef(IND_DONCHIAN_Lo, 1);

	m_atrDataSet->Calculate(ohlc);
	m_NATR = m_atrDataSet->GetRef(IND_ATR, 1);

	if(currentBarIdx > m_lastBarIdx)
	{
		m_lastBarIdx = currentBarIdx;
		if(m_fireAtNextBar)
		{
			// Fire trigger
			m_fireAtNextBar = false;
			return;
		}
	}

	CPortfolioTrendOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioTrendOrderPlacer*>(pPortfolio->OrderPlacer());

	if(pOrderPlacer->IsClosing())
	{
		LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Check for modifying closing order") % pPortfolio->InvestorId()));
		pOrderPlacer->OnQuoteReceived(timestamp, pQuote);
		return;
	}

	if (pOrderPlacer->IsOpened())
	{
		bool meetCloseCondition = false;
		bool forceClosing = IsForceClosing();
		if(forceClosing // This close condition check is only effective on the bar after open
			//|| currentBarIdx >= forceCloseBar
			)
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Portfolio(%s) Closing position due to Force close")
				% pPortfolio->InvestorId() % pPortfolio->ID()));
			ClosePosition(pPortfolio, pOrderPlacer, pQuote, "�ֶ�ƽ��");
			return;
		}
	}
	
	/*
	// Testing for Open position
	double last = pQuote->last();
	LOG_DEBUG(logger, boost::str(boost::format("[%s] Range Trend - Portfolio(%s) Testing for OPEN - Last: %.2f, Hi: %.2f, Lo: %.2f")
		% pPortfolio->InvestorId() % pPortfolio->ID() 
		% last % m_upperBoundOpen % m_lowerBoundOpen));

	entity::PosiDirectionType direction = TestForOpen(pQuote, m_williamsR, m_donchianHi, m_donchianLo, trend);
	if(currentBarIdx < forceCloseBar &&
		direction > entity::NET && 
		(currentBarIdx > m_lastCloseBarIdx))		// In general, don't open position at the bar just closing position
		//|| direction != m_lastPositionOffset))	// unless the direction is different
	{
		if(!pOrderPlacer->IsWorking())
		{
			LOG_DEBUG(logger, boost::str(boost::format("[%s] ASC Trend - Portfolio(%s) Opening position at bar %d")
				% pPortfolio->InvestorId() % pPortfolio->ID() % currentBarIdx ));
			OpenPosition(direction, pOrderPlacer, pQuote, timestamp, false, 
				boost::str(boost::format("WR(%.2f)��������") % m_williamsR).c_str());
			m_lastOpenBarIdx = currentBarIdx;

			// The stopPx at the current bar in this moment will be initial stop price as backup stop price.
			// in case the preceding trend is different than this one, this initial stop price will be effective.
			m_initStopPx = m_watrStopIndSet->GetRef(IND_WATR_STOP, 0);
			return;
		}
	}
	*/
}

void CRangeTrendStrategy::GetStrategyUpdate( entity::PortfolioUpdateItem* pPortfUpdateItem )
{

}

int CRangeTrendStrategy::OnPortfolioAddPosition( CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder )
{
	return 0;
}

void CRangeTrendStrategy::CreateTriggers( const entity::StrategyItem& strategyItem )
{

}

void CRangeTrendStrategy::OpenPosition( entity::PosiDirectionType direction, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, bool forceOpening, const char* noteText )
{
	/*
	if(direction > entity::NET)
	{
		double lmtPrice[2];
		if(direction == entity::LONG)
		{
			lmtPrice[0] = pQuote->ask();
		}
		else if(direction == entity::SHORT)
		{
			lmtPrice[0] = pQuote->bid();
		}
		lmtPrice[1] = 0.0;

		LOG_DEBUG(logger, boost::str(boost::format("Range Trend - %s Open position @ %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % pQuote->update_time()));

		string openComment = forceOpening ? 
			boost::str(boost::format("�ֶ� %s ���� @ %.2f") % GetPosiDirectionText(direction) % lmtPrice[0])
			: 
		boost::str(boost::format("%s - %s ���� @ %.2f")
			% noteText % GetPosiDirectionText(direction) % lmtPrice[0]);

		pOrderPlacer->SetMlOrderStatus(openComment);

		pOrderPlacer->Run(direction, lmtPrice, 2, timestamp);

		//m_lastPositionOffset = direction;
		//m_isRealSignal = m_allowFakeSignal ? true : false; // When opening position, not sure current bar is real signal or not
		ResetForceOpen();
	}
	*/
}

void CRangeTrendStrategy::ClosePosition( CPortfolio* pPortfolio, CPortfolioTrendOrderPlacer* pOrderPlacer, entity::Quote* pQuote, const char* noteText )
{
	/*
	if(pOrderPlacer != NULL)
	{
		entity::PosiDirectionType posiDirection = pOrderPlacer->PosiDirection();

		double closePx = 0.0;
		if(posiDirection == entity::LONG)
		{
			closePx = pQuote->bid();
		}
		else if(posiDirection == entity::SHORT)
		{
			closePx = pQuote->ask();
		}

		LOG_DEBUG(logger, boost::str(boost::format("Range Trend - %s Close position @ %.2f (%s)")
			% GetPosiDirectionText(posiDirection) % closePx  % pQuote->update_time()));

		pOrderPlacer->CloseOrder(closePx);

		ResetForceClose();
		// reset initStopPx
		//m_initStopPx = -1.0;

		pPortfolio->PushMessage(boost::str(boost::format("%s - %s ƽ�� @ %.2f")
			% noteText % GetPosiDirectionText(posiDirection, true) % closePx));
	}
	*/
}

void CRangeTrendStrategy::OnBeforeAddingHistSrcConfig( CHistSourceCfg* pHistSrcCfg )
{
	if(pHistSrcCfg != NULL)
	{
		if(pHistSrcCfg->Precision == m_timeFrame)
			pHistSrcCfg->HistData = true;
	}
}
