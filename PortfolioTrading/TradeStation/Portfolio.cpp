#include "StdAfx.h"
#include "Portfolio.h"
#include "PortfolioManager.h"
#include "StrategyFactory.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

CPortfolio::CPortfolio(void):
m_porfMgr(NULL),
m_openedOrderCount(0),
m_strategyEnabled(false),
m_isAutoOpen(false),
m_isAutoStopGain(false),
m_isAutoStopLoss(false)
{
}


CPortfolio::~CPortfolio(void)
{
	Cleanup();
}

CLeg* CPortfolio::AddLeg(entity::LegItem* legItem)
{
	int legCount = m_vecLegs.size();
	LegPtr leg(new CLeg(legCount + 1));
	leg->SetInnerItem(legItem);
	m_vecLegs.push_back(leg);
	return leg.get();
}

int CPortfolio::Count()
{
	return m_vecLegs.size();
}

CLeg* CPortfolio::GetLeg( int legId )
{
	if(legId <= m_vecLegs.size())
		return m_vecLegs[legId - 1].get();
}

void CLeg::SetInnerItem(entity::LegItem* pItem)
{
	m_pInnerItem = pItem;
}

void CPortfolio::SetItem( entity::PortfolioItem* pPortfItem )
{
	m_innerItem = PortfItemPtr(pPortfItem);
	
	// backup current legs given by initialization
	google::protobuf::RepeatedPtrField< ::entity::LegItem > legs = pPortfItem->legs();
	
	// clear generated legs and set my own legs that works normally
	pPortfItem->clear_legs();

	for(google::protobuf::RepeatedPtrField< ::entity::LegItem >::iterator iter = legs.begin();
		iter != legs.end(); ++iter)
	{
		entity::LegItem* nl = pPortfItem->add_legs();
		nl->set_symbol(iter->symbol());
		nl->set_ratio(iter->ratio());
		nl->set_side(iter->side());
		nl->set_last(iter->last());
		nl->set_ask(iter->ask());
		nl->set_bid(iter->bid());
		nl->set_ispreferred(iter->ispreferred());
		nl->set_status(iter->status());

		AddLeg(nl);
	}

	boost::shared_ptr<CDiffStrategy> strategy(CreateStrategy(pPortfItem->strategyname(), pPortfItem->strategydata()));
	m_strategy = strategy;
}

void CPortfolio::OnQuoteRecevied( boost::shared_ptr<entity::Quote>& pQuote )
{
	// update last
	for(vector<LegPtr>::iterator iter = m_vecLegs.begin(); iter != m_vecLegs.end(); ++iter)
	{
		if((*iter)->Symbol() == pQuote->symbol())
		{
			(*iter)->UpdateLast(pQuote->last());
			(*iter)->UpdateAsk(pQuote->ask());
			(*iter)->UpdateBid(pQuote->bid());
			break;
		}
	}

	// calculate the diff
	double diff = 0;
	for(vector<LegPtr>::iterator iter = m_vecLegs.begin(); iter != m_vecLegs.end(); ++iter)
	{
		double legPrice = (*iter)->Last();
		if(legPrice > 0)
		{
			if((*iter)->Side() == entity::LONG)
			{
				diff +=	legPrice;
			}
			else
				diff -= legPrice;
		}
		else	// if one of legs has no price, set diff 0 anyway
			diff = 0;
	}

	m_innerItem->set_diff(diff);

	if(m_strategyEnabled)
	{
		POSI_OPER poOp = m_strategy->Test(diff);
		if(poOp == OPEN_POSI && m_isAutoOpen)
		{
			m_porfMgr->NotifyOpenPosition(this, Quantity());
		}
		else if(poOp == CLOSE_POSI)
		{
			if(m_isAutoStopGain)
			{

			}
			else if(m_isAutoStopLoss)
			{

			}
		}
	}
	PushUpdate();
}

void CPortfolio::SetManager( CPortfolioManager* parentMgr )
{
	m_porfMgr = parentMgr;

	vector<string> listenSymbols;
	BOOST_FOREACH(const LegPtr& leg, m_vecLegs)
	{
		listenSymbols.push_back(leg->Symbol());
	}

	SetSymbols(listenSymbols);
	m_porfMgr->QuoteAggregator()->SubscribeQuotes(this);
}

void CPortfolio::Cleanup()
{
	m_porfMgr->QuoteAggregator()->UnsubscribeQuotes(this);
}

void CPortfolio::PushUpdate()
{
	m_porfMgr->PublishPortfolioUpdate(m_innerItem.get());
}

int CPortfolio::NewOrderId(string& newId)
{
	boost::mutex::scoped_lock lock(m_mut);
	newId = boost::str(boost::format("%s-%d") % ID().c_str() % ++m_openedOrderCount);
	return m_openedOrderCount;
}

void CPortfolio::ApplyStrategySetting( const string& name, const string& data )
{
	m_strategy->ApplySettings(data);
}

