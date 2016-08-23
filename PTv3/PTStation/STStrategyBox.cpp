#include "stdafx.h"
#include "STStrategyBox.h"
#include "StrategyOperation.h"


CSTStrategyBox::CSTStrategyBox(CStrategyOperation* pStrategyOperation)
	: m_pStrategyOperation(pStrategyOperation)
{
}


CSTStrategyBox::~CSTStrategyBox()
{
}

void CSTStrategyBox::Apply(const entity::StrategyItem& strategyItem)
{
	for (int i = 0; i < strategyItem.histsources_size(); ++i)
	{
		const entity::HistSourceCfg& entityCfg = strategyItem.histsources(i);
		const string& symb = entityCfg.symbol();
		int precision = entityCfg.precision();

		string key;
		if(CSTPriceBarDataSource::BuildKey(symb, precision, &key))
		{
			m_priceBarDataSources.insert(
				make_pair(key, boost::make_shared<CSTPriceBarDataSource>(symb, precision)));
		}

	}
}

void CSTStrategyBox::Test(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	OnTest(pQuote);
}

void CSTStrategyBox::OpenPostion(const string& symbol, double price, int vol, const string& timestamp) const
{
	if (m_pStrategyOperation != NULL)
		m_pStrategyOperation->OpenPosition(symbol, price, vol, timestamp);
}

void CSTStrategyBox::ClosePostion(const string& symbol, double price, int vol, const string& timestamp) const
{
	if (m_pStrategyOperation != NULL)
		m_pStrategyOperation->ClosePosition(symbol, price, vol, timestamp);
}
