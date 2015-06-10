#include "StdAfx.h"
#include "PortfolioManager.h"

CPortfolioManager::CPortfolioManager(void)
{
}


CPortfolioManager::~CPortfolioManager(void)
{
}

void CPortfolioManager::AddPortfolio( CAvatarClient* client, const entity::PortfolioItem& portfItem )
{
	PortfolioPtr portf(new CPortfolio(client, portfItem));
	m_portfolios.push_back(portf);
	if (!m_onAddPortfolioHandler.empty())
		m_onAddPortfolioHandler(portf.get());
}

CPortfolio* CPortfolioManager::Get( const string& portfolioId )
{
	for(std::vector<PortfolioPtr>::iterator iter = m_portfolios.begin();
		iter != m_portfolios.end(); ++iter)
	{
		if((*iter)->ID() == portfolioId)
		{
			return iter->get();
		}
	}

	return NULL;
}

void CPortfolioManager::Clear()
{
	for (std::vector<PortfolioPtr>::iterator iter = m_portfolios.begin();
		iter != m_portfolios.end(); ++iter)
	{
		(*iter)->ReleaseAvatar();
	}

	m_portfolios.clear();
}
