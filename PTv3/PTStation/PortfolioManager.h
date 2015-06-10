#pragma once
#include "Portfolio.h"
#include "entity/message.pb.h"

class CQuoteRepositry;
class CAvatarClient;

typedef boost::function<void(CPortfolio*)> OnAddPortfolioHandler;

class CPortfolioManager
{
public:
	CPortfolioManager(void);
	virtual ~CPortfolioManager(void);

	void SetOnAddPortfolioHandler(OnAddPortfolioHandler handler){ m_onAddPortfolioHandler = handler; }
	void AddPortfolio(CAvatarClient* client, const entity::PortfolioItem& portfItem);
	CPortfolio* Get(const string& portfolioId);
	void Clear();
protected:
	std::vector<PortfolioPtr> m_portfolios;
	OnAddPortfolioHandler m_onAddPortfolioHandler;
};


