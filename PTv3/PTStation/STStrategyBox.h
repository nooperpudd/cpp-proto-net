#pragma once

#include "STPriceBarDataSource.h"
#include "entity/message.pb.h"
#include <boost/unordered_map.hpp>

class CSTStrategyBox
{
public:
	CSTStrategyBox();
	~CSTStrategyBox();

	void Apply(const entity::StrategyItem& strategyItem);

private:

	boost::unordered_map<string, PriceBarDataSourcePtr> m_priceBarDataSources;
};

