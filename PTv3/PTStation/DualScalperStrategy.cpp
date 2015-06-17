#include "StdAfx.h"
#include "DualScalperStrategy.h"
#include "ManualOrderPlacer.h"
#include "globalmembers.h"
#include "Portfolio.h"
#include "DoubleCompare.h"
#include "OrderProcessor.h"

// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
// for func_state and func_state_machine
#include <boost/msm/front/euml/state_grammar.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

namespace DualScapler// Concrete FSM implementation
{
	struct evtLongOpened {};
	struct evtShortOpened {};
	struct evtLongClosed {};
	struct evtShortClosed {};
	struct evtErrorFound {};

	struct DualScaplerFront_ : public msm::front::state_machine_def < DualScaplerFront_ >
	{
		CDualScalperStrategy* Strategy;

		// constructor with arguments
		DualScaplerFront_(CDualScalperStrategy* pParent) : Strategy(pParent)
		{}

		// state machines also have entry/exit actions 
		template <class Event, class Fsm>
		void on_entry(Event const&, Fsm& fsm)
		{
		}

		struct BothEmpty : public msm::front::state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, "DualScalper entering: BothEmpty");
#endif
				fsm.Strategy->SetState(DUAL_SCALPER_BOTH_EMPTY);
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM& fsm)
			{
				LOG_DEBUG(logger, "DualScalper leaving: BothEmpty");
			}
#endif
		};

		struct HoldLongPosition : public msm::front::state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, "DualScalper entering: HoldLongPosition");
#endif
				fsm.Strategy->SetState(DUAL_SCALPER_HOLD_LONG);
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM& fsm) { LOG_DEBUG(logger, "DualScalper leaving: HoldLongPosition"); }
#endif
		};

		struct HoldShortPosition : public msm::front::state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, "DualScalper entering: HoldShortPosition");
#endif
				fsm.Strategy->SetState(DUAL_SCALPER_HOLD_SHORT);
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM& fsm) { LOG_DEBUG(logger, "DualScalper leaving: HoldShortPosition"); }
#endif
		};

		struct BothHeld : public msm::front::state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, "DualScalper entering: BothHeld");
#endif
				fsm.Strategy->SetState(DUAL_SCALPER_BOTH_HELD);
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM& fsm)
			{
				LOG_DEBUG(logger, "DualScalper leaving: BothHeld");
			}
#endif
		};

		struct AllOk : public msm::front::state < > {};

		struct Error : public msm::front::terminate_state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
				fsm.Strategy->SetState(DUAL_SCALPER_ERROR);
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM&) {}
#endif
		};

		typedef mpl::vector<BothEmpty, AllOk> initial_state;

		// action
		template <class Event>
		void on_open(Event const&)
		{
			//m_pPlacer->Send();
		}
		/*
		void on_cancel_success(evtCancelSuccess const& evt)
		{
		m_pPlacer->OnOrderCanceled(evt.m_pOrd);
		}

		// guards
		bool if_portfolio_canceled(evtCancelSuccess const&)
		{
		return m_pPlacer->IfPortfolioCanceled();
		}
		bool if_leg_canceled(evtCancelSuccess const&)
		{
		return !(m_pPlacer->IfPortfolioCanceled());
		}
		bool if_cancel_failed(evtCancelFailure const& evt)
		{
		// If Cancel failed due to order finished, DON'T go to Error
		return evt.m_ErrorId != 26;
		}
		*/
		typedef DualScaplerFront_ p;

		// Transition table for OrderPlacer
		struct transition_table : mpl::vector <
			//    Start					Event					Next			Action                     Guard
			//  +-------------------+-------------------+-------------------+---------------------------+--------------------------+
			_row < BothEmpty		, evtLongOpened		, HoldLongPosition		>,
			_row < BothEmpty		, evtShortOpened	, HoldShortPosition		>,
			_row < HoldLongPosition	, evtShortOpened	, BothHeld		>,
			_row < HoldLongPosition	, evtLongClosed		, BothEmpty		>,
			_row < HoldShortPosition, evtLongOpened		, BothHeld		>,
			_row < HoldShortPosition, evtShortClosed	, BothEmpty		>,
			_row < BothHeld			, evtLongClosed		, HoldShortPosition		>,
			_row < BothHeld			, evtShortClosed	, HoldLongPosition		>,
			_row < AllOk			, evtErrorFound		, Error			>
		> {};

		// Replaces the default no-transition response.
		template <class FSM, class Event>
		void no_transition(Event const& e, FSM& fsm, int state)
		{
			string unExpectedEvtName = typeid(e).name();
			LOG_DEBUG(logger, boost::str(boost::format(
				"no transition from state %d on event %s")
				% state % unExpectedEvtName));
			//fsm.process_event(evtErrorFound("遇到无法处理的事件"));
			logger.Error(boost::str(boost::format("DualScaplerFSM encounter unexpected event %s") % unExpectedEvtName));
		}
	};
	// Pick a back-end
	typedef msm::back::state_machine<DualScaplerFront_> DualScaplerFsm;
}


CDualScalperStrategy::CDualScalperStrategy()
	: m_diffThreshold(0)
	, m_priceTick(0.2)
	, m_openOffset(0.6)
	, m_closeOffset(0.4)
	, m_oppositeCloseThreshold(1)
	, m_longOrderPlacer(NULL)
	, m_shortOrderPlacer(NULL)
{
	m_fsm = boost::shared_ptr<void>(new DualScapler::DualScaplerFsm(this));
}


CDualScalperStrategy::~CDualScalperStrategy()
{
}

void CDualScalperStrategy::Apply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers)
{
	if (withTriggers)
	{
		logger.Debug("Applying Dual ScalperStrategy...");
	}

	CStrategy::Apply(strategyItem, withTriggers);

	m_priceTick = strategyItem.ds_pricetick();
	m_diffThreshold = strategyItem.ds_diffthreshold();
	m_openOffset = 0.2;
	m_closeOffset = 0.2;
	m_oppositeCloseThreshold = 0.4;
	m_longSideUserId = strategyItem.ds_longsideuserid();
	AddUserId(m_longSideUserId);
	m_shortSideUserId = strategyItem.ds_shortsideuserid();
	AddUserId(m_shortSideUserId);
}

void CDualScalperStrategy::Apply(const entity::StrategyItem& strategyItem, bool withTriggers)
{
	Apply(strategyItem, false);
}

void CDualScalperStrategy::Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	m_diff = pQuote->ask() - pQuote->bid();
	m_ask = pQuote->ask();
	m_askSize = 1; // pQuote->ask_size();
	m_bid = pQuote->bid();
	m_bidSize = 1; // pQuote->bid_size();

	if (m_askSize > 0 && m_bidSize > 0)
	{
		if (IsRunning() && !IsSuspending())
		{
			DualScalperState state = State();

			// if price diff meets condition
			if (DoubleGreaterEqual(m_diff, m_diffThreshold))
			{
				if (state == DUAL_SCALPER_BOTH_EMPTY)
				{
					// 1. Try to open
					OpenPosition(pQuote, timestamp);
				}
				else if (state == DUAL_SCALPER_BOTH_HELD)
				{
					// 2. Try to close
					ClosePosition(pQuote, timestamp);
				}
			}

			if (state == DUAL_SCALPER_HOLD_LONG)
			{
				if (m_longOrderPlacer->IsOnPending())
					m_longOrderPlacer->OnQuoteReceived(timestamp, pQuote);
				else if (m_longOrderPlacer->IsOpened())
					LongStopLoss(pQuote, timestamp);
			}
			else if (state == DUAL_SCALPER_HOLD_SHORT)
			{
				if (m_shortOrderPlacer->IsOnPending())
					m_shortOrderPlacer->OnQuoteReceived(timestamp, pQuote);
				else if (m_shortOrderPlacer->IsOpened())
					ShortStopLoss(pQuote, timestamp);
			}
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

void CDualScalperStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem)
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);
	pPortfUpdateItem->set_sc_diff(m_diff);
}

int CDualScalperStrategy::OnPortfolioAddPosition(CPortfolio* pPortfolio, const trade::MultiLegOrder& openOrder, int actualTradedVol)
{
	int qty = openOrder.quantity();

	double ord_profit = CStrategy::CalcOrderProfit(openOrder);
	AddProfit(pPortfolio, ord_profit);
	int totalOpenTimes = IncrementOpenTimes(pPortfolio, qty);
	IncrementCloseTimes(pPortfolio, qty);

	return totalOpenTimes;
}

CPortfolioOrderPlacer* CDualScalperStrategy::CreateOrderPlacer()
{
	return new CDualScalperOrderPlacer;
}

void CDualScalperStrategy::OnBindedRoutes()
{
	m_longOrderPlacer = dynamic_cast<CDualScalperOrderPlacer*>(GetRoute(m_longSideUserId));
	assert(m_longOrderPlacer != NULL);
	m_longOrderPlacer->SetUserId(m_longSideUserId);
	m_shortOrderPlacer = dynamic_cast<CDualScalperOrderPlacer*>(GetRoute(m_shortSideUserId));
	assert(m_shortOrderPlacer != NULL);
	m_shortOrderPlacer->SetUserId(m_shortSideUserId);
}

void CDualScalperStrategy::OpenPosition(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	double buyPx = pQuote->bid() + m_openOffset;
	double sellPx = pQuote->ask() - m_openOffset;

	double longLmtPrice[2] = { buyPx, 0.0 };
	double shortLmtPrice[2] = { sellPx, 0.0 };

	if (pQuote->bid_size() <= pQuote->ask_size())
	{
		m_longOrderPlacer->Run(entity::LONG, longLmtPrice, 2, timestamp);
		m_shortOrderPlacer->Run(entity::SHORT, shortLmtPrice, 2, timestamp);
	}
	else
	{
		m_shortOrderPlacer->Run(entity::SHORT, shortLmtPrice, 2, timestamp);
		m_longOrderPlacer->Run(entity::LONG, longLmtPrice, 2, timestamp);
	}

	LOG_DEBUG(logger, boost::str(boost::format("DualScapler - Open position @ B:%.2f - S:%.2f (%s)")
		% buyPx % sellPx % pQuote->update_time()));
}

void CDualScalperStrategy::ClosePosition(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	double buyPx = pQuote->bid() + m_openOffset;
	double sellPx = pQuote->ask() - m_openOffset;

	if (pQuote->bid_size() >= pQuote->ask_size())
	{
		m_longOrderPlacer->CloseOrder(sellPx, true);
		m_shortOrderPlacer->CloseOrder(buyPx, true);
	}
	else
	{
		m_shortOrderPlacer->CloseOrder(buyPx, true);
		m_longOrderPlacer->CloseOrder(sellPx, true);
	}

	LOG_DEBUG(logger, boost::str(boost::format("DualScapler - Close position @ B:%.2f - S:%.2f (%s)")
		% buyPx % sellPx % pQuote->update_time()));
}

void CDualScalperStrategy::LongStopLoss(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	double stopLossPx = pQuote->ask() - m_closeOffset;  // Attempt use as big as possible value 
	// if it's still possible to get profit
	double cost = m_longOrderPlacer->CostPrice();
	if (DoubleGreaterEqual(pQuote->ask() - m_closeOffset, cost))
	{
		// do nothing here, value already gets correctly set
	}
	else if (DoubleGreaterEqual(pQuote->ask() - m_priceTick, cost))
	{
		stopLossPx = pQuote->ask() - m_priceTick;
	}
	// no possiblity of profit
	else if (DoubleLessEqual(m_diff, m_oppositeCloseThreshold)) // if price diff is small
	{
		stopLossPx = pQuote->bid(); // set opposite price
	}
	
	m_longOrderPlacer->CloseOrder(stopLossPx, false);
	LOG_DEBUG(logger, boost::str(boost::format("DualScapler - Long stop loss @ %.2f (%s)") 
		% stopLossPx % pQuote->update_time()));
}

void CDualScalperStrategy::ShortStopLoss(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	double stopLossPx = pQuote->bid() + m_closeOffset;  // Attempt use as small as possible value 
	// if it's still possible to get profit
	double cost = m_shortOrderPlacer->CostPrice();
	if (DoubleLessEqual(pQuote->bid() + m_closeOffset, cost))
	{
		// do nothing here, value already gets correctly set
	}
	else if (DoubleLessEqual(pQuote->bid() + m_priceTick, cost))
	{
		stopLossPx = pQuote->bid() + m_priceTick;
	}
	else if (DoubleLessEqual(m_diff, m_oppositeCloseThreshold)) // if price diff is small
	{
		stopLossPx = pQuote->ask(); // set opposite price
	}

	m_shortOrderPlacer->CloseOrder(stopLossPx, false);
	LOG_DEBUG(logger, boost::str(boost::format("DualScapler - Long stop loss @ %.2f (%s)")
		% stopLossPx % pQuote->update_time()));
}

void CDualScalperStrategy::OnLegFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume)
{
	boost::lock_guard<boost::mutex> l(m_mutFsm);

	if (offset == trade::OF_OPEN && direction == trade::BUY && sendingIdx == 0)
	{
		boost::static_pointer_cast<DualScapler::DualScaplerFsm>(m_fsm)->process_event(DualScapler::evtLongOpened());
	}
	else if ((offset == trade::OF_CLOSE || offset == trade::OF_CLOSE_TODAY) && direction == trade::SELL && sendingIdx == 1)
	{
		boost::static_pointer_cast<DualScapler::DualScaplerFsm>(m_fsm)->process_event(DualScapler::evtLongClosed());
	}
	else if (offset == trade::OF_OPEN && direction == trade::SELL && sendingIdx == 0)
	{
		boost::static_pointer_cast<DualScapler::DualScaplerFsm>(m_fsm)->process_event(DualScapler::evtShortOpened());
	}
	else if ((offset == trade::OF_CLOSE || offset == trade::OF_CLOSE_TODAY) && direction == trade::BUY && sendingIdx == 1)
	{
		boost::static_pointer_cast<DualScapler::DualScaplerFsm>(m_fsm)->process_event(DualScapler::evtShortClosed());
	}
}

bool CDualScalperStrategy::OnStart()
{
	boost::static_pointer_cast<DualScapler::DualScaplerFsm>(m_fsm)->start();
	return true;
}

void CDualScalperStrategy::OnStop()
{
	boost::static_pointer_cast<DualScapler::DualScaplerFsm>(m_fsm)->stop();
}

void CMultiRouteStrategy::BindRoutes(CPortfolio* pPortfolio, OnBindingRouteHandler onBindingRouteHandler)
{
	for (vector<string>::iterator iter = m_userIds.begin(); iter != m_userIds.end(); ++iter)
	{
		const string& userId = (*iter);
		CPortfolioOrderPlacer* orderPlacer = CreateOrderPlacer();
		m_orderPlacerMap.insert(std::make_pair(userId, OrderPlacerPtr(orderPlacer)));
		COrderProcessor* pOrderProcessor = NULL;
		if (!onBindingRouteHandler.empty())
			onBindingRouteHandler(userId, &pOrderProcessor);
		orderPlacer->Initialize(pPortfolio, pOrderProcessor);
	
		
		if (pOrderProcessor == NULL)	// Must something wrong
		{
			logger.Error(boost::str(boost::format("Portfolio(%s) CANNOT bind OrderProcessor for userid(%s)") 
				% pPortfolio->ID() % userId ));
		}
		else
		{
			const string& ordProcUserId = pOrderProcessor->UserId();
			LOG_DEBUG(logger, boost::str(boost::format("Portfolio(%s) initialize an OrderPlacer with OrderProcessor(%s)") 
				% pPortfolio->ID() % ordProcUserId));
		}
	}

	OnBindedRoutes();
}

CPortfolioOrderPlacer* CMultiRouteStrategy::GetRoute(const string& userId)
{
	boost::unordered_map<string, OrderPlacerPtr>::iterator iter = m_orderPlacerMap.find(userId);
	if (iter != m_orderPlacerMap.end())
	{
		return (iter->second).get();
	}

	return NULL;
}
