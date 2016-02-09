#include "StdAfx.h"
#include "AvatarClient.h"
#include "StrategyFactory.h"
#include "ArbitrageMultiStrategy.h"
#include "ScalperStrategy.h"
#include "ManualStrategy.h"
#include "ChangePositionStrategy.h"
#include "HistSlopeStrategy.h"
#include "WMATrendStrategy.h"
#include "LinerRegressionStrategy.h"
#include "ASCTrendStrategy.h"
#include "RangeTrendStrategy.h"
#include "ArbitrageManualMultiStrategy.h"
#include "DualScalperStrategy.h"
#include "IcebergStrategy.h"
#include "DualQueueStrategy.h"


CStrategyFactory::CStrategyFactory(CPortfolio* pPortf, CAvatarClient* pAvatar)
	: m_portfolio(pPortf)
	, m_avatar(pAvatar)
{
}


CStrategyFactory::~CStrategyFactory()
{
}

CStrategy* CStrategyFactory::Create(const entity::StrategyItem& strategyItem)
{
	entity::StrategyType strategyType = strategyItem.type();
	CStrategy* created = NULL;
	switch (strategyType)
	{
	case entity::ARBITRAGE:
		created = new CArbitrageMultiStrategy(m_avatar, m_portfolio);
		break;
	case entity::SCALPER:
		created = new CScalperStrategy();
		break;
	case entity::MANUAL:
		created = new CManualStrategy();
		break;
	case entity::CHANGE_POSITION:
		created = new CChangePositionStrategy(m_avatar, m_portfolio);
		break;
	case entity::HIST_SLOPE:
		created = new CHistSlopeStrategy(m_avatar);
		break;
	case entity::WMA_TREND:
		created = new CWMATrendStrategy(m_avatar);
		break;
	case entity::LINER_REGRESSION:
		created = new CLinerRegressionStrategy(m_avatar);
		break;
	case entity::ASC_TREND:
		created = new CASCTrendStrategy(m_avatar);
		break;
	case entity::RANGE_TREND:
		created = new CRangeTrendStrategy(m_avatar);
		break;
	case entity::ARBITRAGE_MANUAL:
		created = new CArbitrageManualMultiStrategy(m_avatar, m_portfolio);
		break;
	case entity::DUAL_SCALPER:
		created = new CDualScalperStrategy();
		break;
	case entity::ICEBERG:
		created = new CIcebergStrategy();
		break;
	case entity::DUAL_QUEUE:
		created = new CDualQueueStrategy();
	}

	created->Apply(strategyItem, m_portfolio, false);

	Initialize(created);

	return created;
}

void CStrategyFactory::Initialize(CStrategy* pStrategy)
{
	if (pStrategy != NULL)
	{
		pStrategy->InitOrderPlacer(m_portfolio, &(m_avatar->OrderProcessor()));
	}
}
