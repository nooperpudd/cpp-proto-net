#pragma once

#include "TradeAgentCallback.h"
#include "PortfolioOrderHelper.h"
#include "entity/quote.pb.h"

#include <string>

#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/unordered_map.hpp>

class CPortfolio;
class CTradeAgent;
class CClientAgent;
class CInputOrder;
class CPortfolioOrderPlacer;

using namespace std;

class COrderProcessor2 : public CTradeAgentCallback
{
public:
	COrderProcessor2(void);
	~COrderProcessor2(void);

	void Initialize(CClientAgent* pClientAgent, CTradeAgent* pTradeAgent);

	void AddOrderPlacer(CPortfolioOrderPlacer* pOrdPlacer);
	void RemoveOrderPlacer(const string& placerId /* order ref*/);
	
	int LockForSubmit(string& outOrdRef);
	bool SubmitAndUnlock(CInputOrder* pInputOrder);
	int GenerateOrderRef(string& outOrdRef);

	void CancelOrder(const string& ordRef, 
					 const string& exchId, 
					 const string& ordSysId, 
					 const string& userId,
					 const string& symbol);

	boost::tuple<bool, string> PlaceOrder( const string& symbol, 
										   trade::TradeDirectionType direction, 
										   const string& openDate, 
										   PlaceOrderContext* placeOrderCtx);

	bool QuerySymbol( const std::string& symbol, entity::Quote** ppQuote );

	bool QueryAccountInfo(string* outSerializedAcctInfo);
	void QueryPositionDetails(const string& symbol);

	void PublishMultiLegOrderUpdate(trade::MultiLegOrder* pOrder);
	void PublishOrderUpdate(const string& portfId, const string& mlOrderId, trade::Order* legOrd);
	void PublishTradeUpdate(trade::Trade* pTrade);
	void PublishPositionDetail(trade::PositionDetailInfo* pPosiDetailInfo);
	
	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///��¼������Ӧ
	virtual void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID);

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg);

	///��������������Ӧ
	virtual void OnRspOrderAction(bool succ, const std::string& orderRef, int errorId, const std::string& msg);

	///����֪ͨ
	virtual void OnRtnOrder(trade::Order* order);

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(trade::Trade* pTrade);

	///�����ѯͶ���ֲ߳���ϸ��Ӧ
	virtual void OnRspQryInvestorPositionDetail(trade::PositionDetailInfo* pPositionDetail);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(){}

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(){}

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(const trade::AccountInfo& accountInfo);

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(){}

	//////////////////////////////////////////////////////////////////////////

	bool IsReadyForSubmit(){ return !m_bIsSubmitting; }
	int TotalOpenTimes(){ return m_totalOpenTimes; }
	int TotalCancelTimes(){ return m_totalCancelTimes; }

	bool ReachOpenTimesLimit(){ return m_totalOpenTimes >= m_maxTotalOpenTimes; }
	bool ReachCancelTimesLimit() { return m_totalCancelTimes >= m_maxTotalCancelTimes; }

	void SetMaxOpenTimes(int maxOpenTimes){ m_maxTotalOpenTimes = maxOpenTimes; }
	void SetMaxCancelTimes(int maxCancelTimes){ m_maxTotalCancelTimes = maxCancelTimes; }

	const string& BrokerId();
	const string& InvestorId();

private:

	static void PrintOrderStatus(trade::Order* order);
	
	void DispatchRtnOrder(boost::shared_ptr<trade::Order>& rtnOrder);

	trade::InputOrder* BuildCloseOrder(const string& symbol, trade::TradeDirectionType direction, const string& openDate, PlaceOrderContext* placeOrderCtx);
	void AddOpenTimes() { ++m_totalOpenTimes; }
	void AddCancelTimes() { ++m_totalCancelTimes; }

	boost::unordered_map<string, CPortfolioOrderPlacer*> m_workingOrderPlacers;
	boost::recursive_mutex m_ordPlacersMapMutex;

	int m_maxOrderRef;
	boost::mutex m_mutOrdRefIncr;
	boost::condition_variable m_condSubmit;
	bool m_bIsSubmitting;

	CTradeAgent* m_pTradeAgent;
	CClientAgent* m_pClientAgent;

	boost::condition_variable m_condQryAcct;
	boost::mutex m_mutQryAcct;
	string m_serializedQryAcctInfo;

	int m_totalOpenTimes;
	int m_totalCancelTimes;
	int m_maxTotalOpenTimes;
	int m_maxTotalCancelTimes;
};

