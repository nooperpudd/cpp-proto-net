#include "StdAfx.h"
#include "ArbitrageMultiStrategy.h"


CArbitrageMultiStrategy::CArbitrageMultiStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar, CPortfolio* pPortfolio)
	: CMultiOpenStrategy(strategyItem, pAvatar, pPortfolio)
{
}


CArbitrageMultiStrategy::~CArbitrageMultiStrategy()
{
}
