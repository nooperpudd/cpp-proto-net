#include "StdAfx.h"
#include "PortfolioScalperOrderPlacer.h"
#include "Portfolio.h"
#include "OrderProcessor.h"
#include "ScalperStrategy.h"

#ifndef USE_FEMAS_API

#ifdef USE_ZEUSING_API
#include "InputOrder_Zeus.h"
#else
#include "InputOrder.h"
#endif

#else
#include "InputOrder_FM.h"
#endif

#include <boost/date_time.hpp>

CPortfolioScalperOrderPlacer::CPortfolioScalperOrderPlacer(void)
{
}


CPortfolioScalperOrderPlacer::~CPortfolioScalperOrderPlacer(void)
{
}

void CPortfolioScalperOrderPlacer::BuildTemplateOrder()
{
	static trade::OffsetFlagType OFFSET_FLAGS[2] = 
				{trade::OF_OPEN, trade::OF_CLOSE_TODAY};

	boost::shared_ptr<trade::MultiLegOrder> pMultiLegOrder(new trade::MultiLegOrder);
	
	pMultiLegOrder->set_quantity(m_pPortf->Quantity());
	pMultiLegOrder->set_portfolioid(m_pPortf->ID());
	pMultiLegOrder->set_offset(trade::ML_OF_OTHER);
	pMultiLegOrder->set_haswarn(false);
	pMultiLegOrder->set_statusmsg("");

	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	pMultiLegOrder->set_opendate(boost::gregorian::to_iso_string(d));
	pMultiLegOrder->set_reason(SubmitReason());

	assert(m_pPortf->Count() == 1);

	CLeg* leg = m_pPortf->GetLeg(1);

	for(int i = 0; i < 2; ++i)
	{
		trade::Order* order = pMultiLegOrder->add_legs();
		order->set_brokerid(m_pOrderProcessor->BrokerId());
		order->set_investorid(m_pOrderProcessor->InvestorId());
		order->set_instrumentid(leg->Symbol());
		order->set_orderref("");

		order->set_orderpricetype(trade::LIMIT_PRICE);

		char CombOffset[2] = { 0, 0 };
		CombOffset[0] = OFFSET_FLAGS[i];

		order->set_comboffsetflag(CombOffset);

		char CombHedgeFlag[] = { static_cast<char>(m_pPortf->HedgeFlag()), 0 };
		order->set_combhedgeflag(std::string(CombHedgeFlag));

		int qty = pMultiLegOrder->quantity() * leg->Ratio();
		order->set_volumetotaloriginal(qty);

		order->set_timecondition(trade::TC_GFD);
		
		order->set_volumecondition(trade::VC_AV);
		order->set_minvolume(1);

		order->set_contingentcondition(trade::IMMEDIATELY);
		order->set_forceclosereason(trade::NOT_FORCE_CLOSE);
		order->set_isautosuspend(false);
		order->set_userforceclose(false);

		order->set_ordersubmitstatus(trade::NOT_SUBMITTED);
		order->set_orderstatus(trade::STATUS_UNKNOWN);

		order->set_preferred(i == 0);
	}

	m_multiLegOrderTemplate = pMultiLegOrder;
}

void CPortfolioScalperOrderPlacer::OnAddingLegOrderPlacer( CLegOrderPlacer* pLegOrderPlacer )
{
	CScalperStrategy* scalperStrategy = dynamic_cast<CScalperStrategy*>(m_pPortf->Strategy());
	if(scalperStrategy != NULL)
	{
		double priceTick = scalperStrategy->PriceTick();
		pLegOrderPlacer->SetPriceTick(priceTick);
	}
}

void CPortfolioQueueOrderPlacer::QueueOrder(entity::PosiDirectionType posiDirection, double openPx, double closePx, boost::chrono::steady_clock::time_point trigQuoteTimestamp)
{
	m_openPrice = openPx;
	m_closePrice = closePx;
	double lmtPrice[2] = { openPx, closePx };
	Run(posiDirection, lmtPrice, 2, trigQuoteTimestamp);
}

bool CPortfolioQueueOrderPlacer::IsOpening()
{
	return IsWorking() && m_activeOrdPlacer->SequenceNo() == 0;
}

bool CPortfolioQueueOrderPlacer::IsClosing()
{
	return IsWorking() && m_activeOrdPlacer->SequenceNo() == 1;
}

void CPortfolioQueueOrderPlacer::CancelPendingOrder()
{
	// Go to canceling event
	OnPendingTimeUp();
}

void CPortfolioQueueOrderPlacer::CancelPendingAndClosePosition(entity::Quote* pQuote)
{
	m_activeOrdPlacer->ModifyPriceBasedOnOpposite(pQuote);
	// Go to canceling event
	OnPendingTimeUp();
}

CLegOrderPlacer* CPortfolioQueueOrderPlacer::CreateLegOrderPlacer(int openTimeout, int maxRetryTimes)
{
	return new CQueueLegOrderPlacer(this);
}

void CPortfolioQueueOrderPlacer::OnLegOrderFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume, int execId)
{
	m_pPortf->NotifyLegFilled(sendingIdx, symbol, offset, direction, price, volume, execId);
}

void CPortfolioQueueOrderPlacer::OnLegOrderCanceled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, int execId)
{
	m_pPortf->NotifyLegCanceled(sendingIdx, symbol, offset, direction, execId);
}
