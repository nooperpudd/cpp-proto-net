#pragma once

#include "STPriceBarDataSource.h"
#include "entity/message.pb.h"
#include "entity/quote.pb.h"
#include <boost/unordered_map.hpp>

class CStrategyOperation;

class CSTStrategyBox
{
public:
	CSTStrategyBox(CStrategyOperation* pStrategyOperation);
	virtual ~CSTStrategyBox();

	void Apply(const entity::StrategyItem& strategyItem);
	void Test(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);

protected:

	virtual void OnTest(entity::Quote* pQuote) = 0;

	void OpenPostion(const string& symbol, double price, int vol, const string& timestamp) const;
	void ClosePostion(const string& symbol, double price, int vol, const string& timestamp) const;

	CStrategyOperation* m_pStrategyOperation;
	boost::unordered_map<string, PriceBarDataSourcePtr> m_priceBarDataSources;
};

