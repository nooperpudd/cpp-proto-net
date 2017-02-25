#pragma once

#include "PortfolioOrderPlacer.h"

class CPortfolioTrendOrderPlacer : public CPortfolioOrderPlacer
{
public:
	CPortfolioTrendOrderPlacer(int execId);
	CPortfolioTrendOrderPlacer(void);
	~CPortfolioTrendOrderPlacer(void);

	void CloseOrder(double limitprice);

	bool IsOpened();
	bool IsClosing();

	double CostPrice();
	double EntryPrice() const { return m_entryPrice; }
	void SetEntryPrice(double entryPx) { m_entryPrice = entryPx; }

protected:
	virtual void BuildTemplateOrder();
	virtual void OnAddingLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer);
	virtual void OnLegOrderSent(int orderPlacerIdx);
	virtual void OnLegOrderFilled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, double price, int volume, int execId);
	virtual void OnLegOrderCanceled(int sendingIdx, const string& symbol, trade::OffsetFlagType offset, trade::TradeDirectionType direction, int execId);
	virtual void OnPortfolioDone();
	virtual int GetQuantity();

	virtual trade::SubmitReason SubmitReason(){ return trade::SR_Trend; }

	int m_sentOrderIdx;

	double m_entryPrice;
};

