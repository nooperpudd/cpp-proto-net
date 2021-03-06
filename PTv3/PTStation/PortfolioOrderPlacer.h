#pragma once

#include "LegOrderPlacer.h"
#include "BufferRunner.h"
#include "entity/trade.pb.h"
#include "entity/quote.pb.h"

#ifndef USE_FEMAS_API

#ifdef USE_ZEUSING_API
#include "RtnOrderWrapper_Zeus.h"
#else
#include "RtnOrderWrapper.h"
#endif

#else
#include "RtnOrderWrapper_FM.h"
#endif

#include <boost/atomic.hpp>

class CPortfolio;
class COrderProcessor;
class CInputOrder;

using namespace std;

enum PortfolioFinishState { PortfolioError, PortfolioCanceled, PortfolioFilled };

typedef boost::function<void(int, PortfolioFinishState, entity::PosiOffsetFlag, int)> PortfolioTradedEvent;

class CPortfolioOrderPlacer
{
public:
	CPortfolioOrderPlacer(void);
	virtual ~CPortfolioOrderPlacer(void);
	
	void Initialize(CPortfolio* pPortf, COrderProcessor* pOrderProc)
	{
		m_pPortf = pPortf;
		m_pOrderProcessor = pOrderProc;
	}

	// The first order processor given in Initialize is not correct for AvatarMultiClient
	// Thus this method gets chance to update correct one
	void UpdateOrderProcessor(COrderProcessor* pOrderProc)
	{
		m_pOrderProcessor = pOrderProc;
	}

	// Prepare multileg order template and input orders for sending
	virtual bool Prepare();

	virtual void Cleanup();

	// Truly submit order to trade agent
	void Run(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize);
	void Run(entity::PosiDirectionType posiDirection, double* pLmtPxArr, int iPxSize, 
		const boost::chrono::steady_clock::time_point& trigQuoteTimestamp);
	
	// Identification for order processor to lookup while order returned
	const string& Id() 
	{
		assert(m_activeOrdPlacer != NULL);
		return m_activeOrdPlacer->OrderRef(); 
	}

	boost::chrono::steady_clock::time_point& TrigTimestamp(){ return m_triggingTimestamp; }

	bool IsWorking(){ return m_isWorking.load(boost::memory_order_consume); }
	bool IsOnPending();

	// For Fsm to begin sending leg order
	void Send(const char* openOrderId = NULL);
	// On leg order submit done
	void OnAccept(const RtnOrderWrapperPtr& pRtnOrder);
	
	void OnCanceling();
	virtual void OnLegCanceled(const RtnOrderWrapperPtr& pRtnOrder);
	void OnLegRejected(const RtnOrderWrapperPtr& pRtnOrder);
	void OnPortfolioCanceled();

	void OnPending(const RtnOrderWrapperPtr& pRtnOrder);
	void OnPendingTimeUp();
	void OnQuoteReceived(boost::chrono::steady_clock::time_point& quoteTimestamp, entity::Quote* pQuote);

	void OnFilled(const RtnOrderWrapperPtr& pRtnOrder);
	void OnPartiallyFilled(const RtnOrderWrapperPtr& pRtnOrder);
	void OnOrderCanceled(const RtnOrderWrapperPtr& pRtnOrder);
	void OnCompleted();
	void OnError(const string& errMsg);
	void OnPartiallyFilledToCanceling(const RtnOrderWrapperPtr& pRtnOrder);

	// Belows is for OrderProcess to invoke
	void OnOrderReturned(RtnOrderWrapperPtr& rtnOrder);
	void OnOrderPlaceFailed(const string& errMsg);
	void OnOrderCancelFailed(int errorId, const string& errMsg);

	virtual bool IfPortfolioCanceled();

	entity::PosiDirectionType PosiDirection(){ return m_posiDirection; }
	void OutputStatus(const string& statusMsg);
	void SetMlOrderStatus(const string& statusMsg);

	int ExecId(){ return m_execId; }
	void SetExecId(int execId){ m_execId = execId; }
	CPortfolio* Portfolio(){ return m_pPortf; }
	void SetPortfolioTradedEventHandler(PortfolioTradedEvent portfTradedHandler)
	{
		m_portfTradedEventHandler = portfTradedHandler;
	}

	void GetStateName(string& stateName, int stateId);

	bool IsCloseYesterday(){ return m_closeYesterday; }
	void SetCloseYesterday(bool closeYesterday){ m_closeYesterday = closeYesterday; }

protected:

	void Initialize(const string& mlOrdId);

	virtual void BuildTemplateOrder(){}
	virtual void SetDirection(entity::PosiDirectionType posiDirection);
	virtual void SetLimitPrice(double* pLmtPxArr, int iPxSize);
	virtual void OnAddingLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer){}
	virtual void OnLegOrderSent(int orderPlacerIdx){}
	virtual void OnLegOrderFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume, int execId){}
	virtual void OnLegOrderCanceled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, int execId){}
	virtual void OnPortfolioDone(PortfolioFinishState portfState){}
	virtual CLegOrderPlacer* CreateLegOrderPlacer(int openTimeout, int maxRetryTimes);

	void SetNewOrderId(const string& mlOrdId, const char* openOrdId);
	void ResetTemplate();
	void FillSendingOrderNote();
	void ResetSendingOrderNote();
	void SetFirstLeg();
	void ResetOrderPlacerStatus();
	void GotoRetry(const RtnOrderWrapperPtr& pRtnOrder);
	void RaiseLegOrderFilledEvent(int sendingIdx, const RtnOrderWrapperPtr& pRtnOrder);
	void RaiseLegOrderCanceledEvent(int sendingIdx, const RtnOrderWrapperPtr& pRtnOrder);
	void GotoNext();
	void AfterLegDone();
	void AfterPortfolioDone(PortfolioFinishState portfState);
	void RaiseError(const string& errMsg){ RaiseError(errMsg.c_str()); }
	void RaiseError(const char* errMsg);

	void GenLegOrderPlacers();
	void UpdateLegOrder(const RtnOrderWrapperPtr& pRtnOrder);
	void UpdateLastDoneOrder();
	void UpdateMultiLegOrder(bool pushToClient = true);
	
	void PushWholeMultiLegOrder(trade::MultiLegOrder* pOrder);
	void PushIndividualLegOrder(const string& portfId, const string& mlOrderId, trade::Order* legOrd);

	void CleanupProc();
	void ForceCleanup();
	bool IsReadyForPrepare(){ return !m_isReady; }

	bool SendNextOnFilled(){ return m_sendNextOnFilled; }
	void SendNextOnFilled(bool val){ m_sendNextOnFilled = val; }
	void GoStart(const char* openOrderId); // Just start FSM for PortfolioArbitrageOrderPlacer
#ifdef USE_FEMAS_API
	void GotoSentState();
#endif 
	
	void RaisePortfolioDoneEvent(PortfolioFinishState doneState, int volumeTraded, entity::PosiOffsetFlag offsetFlag)
	{
		if (!m_portfTradedEventHandler.empty())
			m_portfTradedEventHandler(m_execId, doneState, offsetFlag, volumeTraded);
	}

	entity::PosiOffsetFlag GetPortfolioOffset();

	void HandleRtnOrder(RtnOrderWrapperPtr& rtnOrder);

	struct NextQuote
	{
		double last;
		double ask;
		double bid;
	};
	NextQuote m_nextQuote;

	CPortfolio* m_pPortf;
	COrderProcessor* m_pOrderProcessor;
	int m_execId;
	PortfolioTradedEvent m_portfTradedEventHandler;

	boost::shared_ptr<trade::MultiLegOrder> m_multiLegOrderTemplate;
	vector< LegOrderPlacerPtr > m_legPlacers;
	CLegOrderPlacer* m_activeOrdPlacer;
	CLegOrderPlacer* m_lastDoneOrdPlacer;
	bool m_isFirstLeg;
	boost::atomic<bool> m_isWorking;
	bool m_isReady;
	bool m_sendNextOnFilled;
	bool m_closeYesterday;
	string m_sendingOrderNote;
	entity::PosiDirectionType m_posiDirection;
	boost::chrono::steady_clock::time_point m_triggingTimestamp;

	boost::thread m_thCleanup;
	boost::condition_variable m_condCleanDone;
	boost::mutex m_mutCleaning;

	boost::shared_ptr<void> m_fsm;
	boost::timed_mutex m_mutOuterAccessFsm;

	string m_pushingMlOrdId;
	boost::mutex m_mutPushMlOrd;
	boost::condition_variable m_condMlOrdPushed;

	CBufferRunner<RtnOrderWrapperPtr> m_rtnOrderPump;
};

typedef boost::shared_ptr<CPortfolioOrderPlacer> OrderPlacerPtr;