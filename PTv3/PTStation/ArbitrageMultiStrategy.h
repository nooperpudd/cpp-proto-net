#pragma once

#include "MultiOpenStrategy.h"

class CArbitrageMultiStrategy : public CMultiOpenStrategy
{
public:
	CArbitrageMultiStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio);
	~CArbitrageMultiStrategy();
};

