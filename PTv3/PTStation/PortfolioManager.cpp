#include "StdAfx.h"
#include "PortfolioManager.h"
#include "globalmembers.h"

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
	EnsureAllStop(true);

	for (std::vector<PortfolioPtr>::iterator iter = m_portfolios.begin();
		iter != m_portfolios.end(); ++iter)
	{
		(*iter)->ReleaseAvatar();
	}

	m_portfolios.clear();
}

void CPortfolioManager::EnsureAllStop(bool wait)
{
	bool stillHasRunning = false;
	for (std::vector<PortfolioPtr>::iterator iter = m_portfolios.begin();
		iter != m_portfolios.end(); ++iter)
	{
		const PortfolioPtr& portf = (*iter);
		if (portf->Strategy()->IsRunning())
		{
			portf->StopStrategy();
			stillHasRunning = true;
		}
	}

	if (stillHasRunning && wait)
	{
		LOG_DEBUG(logger, boost::str(boost::format("There are still strategies RUNNING, wait %d seconds for stop")
			% 20));
		boost::this_thread::sleep_for(boost::chrono::seconds(20));
	}
}
