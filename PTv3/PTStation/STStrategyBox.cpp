#include "stdafx.h"
#include "STStrategyBox.h"


CSTStrategyBox::CSTStrategyBox()
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
