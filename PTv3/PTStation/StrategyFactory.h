#pragma once

#include "entity/message.pb.h"

class CAvatarClient;
class CPortfolio;
class CStrategy;

class CStrategyFactory
{
public:
	CStrategyFactory(CPortfolio* pPortf, CAvatarClient* pAvatar);
	~CStrategyFactory();

	CStrategy* Create(const entity::StrategyItem& strategyItem);

private:

	void Initialize(CStrategy* pStrategy);

	CAvatarClient* m_avatar;
	CPortfolio * m_portfolio;
};

typedef boost::shared_ptr<CStrategyFactory> StrategyFactoryPtr;

