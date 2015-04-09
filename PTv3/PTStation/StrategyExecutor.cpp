#include "StdAfx.h"
#include "StrategyExecutor.h"
#include "globalmembers.h"

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

namespace strategy// Concrete FSM implementation
{
	struct evtOpening {};
	struct evtClosing {};
	struct evtOrderFilled {};
	struct evtErrorFound {};


	struct ExecutorFront_ : public msm::front::state_machine_def < ExecutorFront_ >
	{
		CStrategyExecutor* Executor;

		// constructor with arguments
		ExecutorFront_(CStrategyExecutor* pExecutor) :Executor(pExecutor)
		{}

		// state machines also have entry/exit actions 
		template <class Event, class Fsm>
		void on_entry(Event const&, Fsm& fsm)
		{
		}

		struct EmptyPosition : public msm::front::state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, boost::str(boost::format("executor(%d) entering: EmptyPosition") % fsm.Executor->ExecId()));
#endif
				fsm.Executor->SetState(EMPTY_POSITION);
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM& fsm) 
			{ 
				LOG_DEBUG(logger, boost::str(boost::format("executor(%d) leaving: EmptyPosition") % fsm.Executor->ExecId()));
			}
#endif
		};

		struct HoldPosition : public msm::front::state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, boost::str(boost::format("executor(%d) entering: HoldPosition") % fsm.Executor->ExecId()));
#endif
				fsm.Executor->SetState(HOLD_POSITION);
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM& fsm) { LOG_DEBUG(logger, boost::str(boost::format("executor(%d) leaving: HoldPosition") % fsm.Executor->ExecId())); }
#endif
		};

		struct PendingOpen : public msm::front::state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, boost::str(boost::format("executor(%d) entering: PendingOpen") % fsm.Executor->ExecId()));
#endif
				fsm.Executor->SetState(PENDING_OPEN);
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM& fsm) { LOG_DEBUG(logger, boost::str(boost::format("executor(%d) leaving: PendingOpen") % fsm.Executor->ExecId())); }
#endif
		};

		struct PendingClose : public msm::front::state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, boost::str(boost::format("executor(%d) entering: PendingClose") % fsm.Executor->ExecId()));
#endif
				fsm.Executor->SetState(PENDING_CLOSE);
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM& fsm) { LOG_DEBUG(logger, boost::str(boost::format("executor(%d) leaving: PendingClose") % fsm.Executor->ExecId())); }
#endif
		};

		struct Idle : public msm::front::terminate_state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
#ifdef LOG_FOR_TRADE
				LOG_DEBUG(logger, boost::str(boost::format("executor(%d) entering: Idle") % fsm.Executor->ExecId()));
#endif
				fsm.Executor->SetState(EXECUTOR_IDLE);
				fsm.Executor->OnFinished();
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM& fsm) 
			{
				LOG_DEBUG(logger, boost::str(boost::format("executor(%d) leaving: Idle") % fsm.Executor->ExecId()));
			}
#endif
		};

		struct AllOk : public msm::front::state < > {};

		struct Error : public msm::front::terminate_state < >
		{
			template <class Event, class FSM>
			void on_entry(Event const& evt, FSM& fsm)
			{
				fsm.Executor->SetState(EXECUTOR_IDLE);
			}
#ifdef LOG_FOR_TRADE
			template <class Event, class FSM>
			void on_exit(Event const&, FSM&) {}
#endif
		};

		typedef mpl::vector<EmptyPosition, AllOk> initial_state;

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
		typedef ExecutorFront_ p;

		// Transition table for OrderPlacer
		struct transition_table : mpl::vector <
			//    Start					Event					Next			Action                     Guard
			//  +-------------------+-------------------+-------------------+---------------------------+--------------------------+
			_row < EmptyPosition	, evtOpening		, PendingOpen		>,
			_row < PendingOpen		, evtOrderFilled	, HoldPosition		>,
			_row < HoldPosition		, evtClosing		, PendingClose		>,
			_row < PendingClose		, evtOrderFilled	, Idle			    >,
			_row < AllOk			, evtErrorFound		, Error				>
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
			logger.Error(boost::str(boost::format("Encounter unexpected event %s") % unExpectedEvtName));
		}
	};
	// Pick a back-end
	typedef msm::back::state_machine<ExecutorFront_> ExecutorFsm;
}

CStrategyExecutor::CStrategyExecutor(int execId, int quantity)
	: m_currentState(EXECUTOR_IDLE)
	, m_quantity(quantity)
	, m_execId(execId)
	, m_deferringCleanup(false)
{
	m_fsm = boost::shared_ptr<void>(new strategy::ExecutorFsm(this));

}


CStrategyExecutor::~CStrategyExecutor()
{
}

void CStrategyExecutor::Start()
{
	boost::static_pointer_cast<strategy::ExecutorFsm>(m_fsm)->start();
}

void CStrategyExecutor::FireEvent(ExecutorEvent execEvent)
{
	assert(m_fsm.get() != NULL);
	switch (execEvent)
	{
	case EXEC_OPEN:
		boost::static_pointer_cast<strategy::ExecutorFsm>(m_fsm)->process_event(strategy::evtOpening());
		break;
	case EXEC_CLOSE:
		boost::static_pointer_cast<strategy::ExecutorFsm>(m_fsm)->process_event(strategy::evtClosing());
		break;
	case EXEC_FILLED:
		boost::static_pointer_cast<strategy::ExecutorFsm>(m_fsm)->process_event(strategy::evtOrderFilled());
		break;
	default:
		break;
	}
}

entity::PosiDirectionType CStrategyExecutor::PosiDirection()
{
	if (m_orderPlacer.get() != NULL)
		return m_orderPlacer->PosiDirection();

	return entity::NET;
}

void CStrategyExecutor::OnFilled(int volumeTraded)
{
	FireEvent(EXEC_FILLED);
}

bool CStrategyExecutor::Prepare()
{
	bool succ = false;
	if (m_orderPlacer.get() != NULL)
	{
		succ = m_orderPlacer->Prepare();
		if (!succ)
			logger.Warning(boost::str(boost::format("Executor(%d) did NOT get prepared successfully") % m_execId));
	}
	return succ;
}

void CStrategyExecutor::Cleanup()
{
	if (State() <= EMPTY_POSITION)
	{
		LOG_DEBUG(logger, boost::str(boost::format("Executor(%d) Did 'Actual' Cleanup") % m_execId));
		if (m_orderPlacer.get() != NULL)
			m_orderPlacer->Cleanup();

		m_deferringCleanup = false;
	}
	else
	{
		LOG_DEBUG(logger, boost::str(boost::format("Executor(%d) ''Deferred'' Cleanup") % m_execId));
		m_deferringCleanup = true;
	}
}

void CStrategyExecutor::OnFinished()
{
	if (m_deferringCleanup)
		Cleanup();
}
