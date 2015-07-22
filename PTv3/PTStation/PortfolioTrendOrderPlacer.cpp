#include "StdAfx.h"
#include "PortfolioTrendOrderPlacer.h"
#include "Portfolio.h"
#include "OrderProcessor.h"

#include <boost/date_time.hpp>


CPortfolioTrendOrderPlacer::CPortfolioTrendOrderPlacer(void)
	: m_sentOrderIdx(-1)
{
	// This is important for TrendOrderPlacer. The next leg is always triggered explicitly.
	SendNextOnFilled(false);
}


CPortfolioTrendOrderPlacer::~CPortfolioTrendOrderPlacer(void)
{
}

int CPortfolioTrendOrderPlacer::GetQuantity()
{
	return m_pPortf->Quantity();
}

void CPortfolioTrendOrderPlacer::BuildTemplateOrder()
{
	static trade::OffsetFlagType OFFSET_FLAGS[2] = 
	{trade::OF_OPEN, trade::OF_CLOSE_TODAY};

	boost::shared_ptr<trade::MultiLegOrder> pMultiLegOrder(new trade::MultiLegOrder);

	pMultiLegOrder->set_quantity(GetQuantity());
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

		char CombOffset[2] = {0, 0};
		CombOffset[0] = OFFSET_FLAGS[i];

		order->set_comboffsetflag(CombOffset);

		char CombHedgeFlag[] = { static_cast<char>(m_pPortf->HedgeFlag()), 0 };
		order->set_combhedgeflag(CombHedgeFlag);

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

void CPortfolioTrendOrderPlacer::CloseOrder( double limitprice )
{
	if(m_multiLegOrderTemplate->legs_size() > 1)
	{
		// set limit price
		trade::Order* pOrd = m_multiLegOrderTemplate->mutable_legs(1);
		pOrd->set_limitprice(limitprice);
		
		m_legPlacers[1]->InputOrder().set_limitprice(limitprice);

		GotoNext();
	}
}

void CPortfolioTrendOrderPlacer::OnLegOrderSent( int orderPlacerIdx )
{
	m_sentOrderIdx = orderPlacerIdx;
}

void CPortfolioTrendOrderPlacer::OnPortfolioDone()
{
	m_sentOrderIdx = -1;
}

bool CPortfolioTrendOrderPlacer::IsOpened()
{
	return IsWorking() && m_activeOrdPlacer->SequenceNo() > 0;
}

bool CPortfolioTrendOrderPlacer::IsClosing()
{
	return IsWorking() && m_sentOrderIdx == 1;
}

void CPortfolioTrendOrderPlacer::OnAddingLegOrderPlacer( CLegOrderPlacer* pLegOrderPlacer )
{
	if(pLegOrderPlacer != NULL)
	{
		pLegOrderPlacer->ModifyPriceWay(BASED_ON_OPPOSITE);
	}
}

void CPortfolioTrendOrderPlacer::OnLegOrderFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume)
{
	m_pPortf->NotifyLegFilled(sendingIdx, symbol, offset, direction, price, volume);
}

void CPortfolioTrendOrderPlacer::OnLegOrderCanceled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction)
{
	m_pPortf->NotifyLegCanceled(sendingIdx, symbol, offset, direction);
}

double CPortfolioTrendOrderPlacer::CostPrice()
{
	if (m_multiLegOrderTemplate->legs_size() > 0)
	{
		// set limit price
		trade::Order* pOrd = m_multiLegOrderTemplate->mutable_legs(0);
		return pOrd->limitprice();
	}

	return 0;
}