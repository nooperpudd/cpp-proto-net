#pragma once

#include "MarketAgentCallback.h"
#include "BufferRunner.h"
#include "OperationRecordData.h"
#include "InputOrder.h"
#include "QuoteCache.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

class CReturnOrder;
class CTrade;

namespace OP{

	enum LONG_SHORT_FLAG
	{
		SHORT = 0,
		LONG = 1
	};

	enum OPEN_CLOSE_FLAG
	{
		OPEN = 0,
		CLOSE = 1
	};

};

class COrderProcessor : public CMarketAgentCallback
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	void Initialize();
	void SetSymbol(const std::string& symb);

	void OpenPosition(int quantity, int longshort);
	void ClosePosition();

	virtual void OnSubscribeCompleted();
	virtual void OnUnsubscribeCompleted();
	virtual void OnQuoteReceived(CQuote* pQuote);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///��¼������Ӧ
	void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID);

	///����¼��������Ӧ
	void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg);

	///��������������Ӧ
	void OnRspOrderAction(bool succ, const std::string& orderRef, const std::string& msg);

	///����֪ͨ
	void OnRtnOrder(CReturnOrder* order);

	///�ɽ�֪ͨ
	void OnRtnTrade(CTrade* pTrade);

	///Ͷ���߽�����ȷ����Ӧ
	void OnRspSettlementInfoConfirm(){}

	///�����ѯ��Լ��Ӧ
	void OnRspQryInstrument(){}

	///�����ѯ�ʽ��˻���Ӧ
	void OnRspQryTradingAccount(){}

	///�����ѯͶ���ֲ߳���Ӧ
	void OnRspQryInvestorPosition(){}

	//////////////////////////////////////////////////////////////////////////

private:
	const char* NextOrderRef();

	void ProcessQuote(boost::shared_ptr<CQuote>& pQuote);

	void ForwardQuote(boost::shared_ptr<CQuote>& pQuote);

	boost::shared_ptr<CInputOrder> CreateOrder(int quantity, 
		OP::OPEN_CLOSE_FLAG open_close, OP::LONG_SHORT_FLAG long_short, double limitPrice);
	
	std::vector<std::string> m_currentSymbols;
	CBufferRunner< boost::shared_ptr<CQuote> > m_bufferRunner;

	int m_orderRefID;
	boost::mutex m_mutOrderRef;
	char ORDER_REF_BUF[13];

	boost::shared_ptr<COperationRecordData> m_currentRecord;

	CQuoteCache m_latestQuote; 
};

