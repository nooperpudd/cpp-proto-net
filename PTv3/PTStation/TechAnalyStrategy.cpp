#include "stdafx.h"
#include "TechAnalyStrategy.h"
#include "PriceBarDataSource.h"
#include "AvatarClient.h"
#include <assert.h>

const char* GetPosiDirectionText(entity::PosiDirectionType direction, bool chs)
{
	switch(direction)
	{
	case entity::LONG:
		return chs ? "��ͷ" : "LONG";
	case entity::SHORT:
		return chs ? "��ͷ" : "SHORT";
	default:
		return "";
	}
}

CTechAnalyStrategy::CTechAnalyStrategy(CAvatarClient* pAvatar)
	: CStrategy()
	, m_avatar(pAvatar)
{
}

CTechAnalyStrategy::~CTechAnalyStrategy(void)
{
}

void CTechAnalyStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	CStrategy::Test(pQuote, pPortfolio, timestamp);
	{
		boost::mutex::scoped_lock l(m_mutDataProxy);
		for (vector<CPriceBarDataProxy*>::iterator iter = m_pDataProxies.begin();
			iter != m_pDataProxies.end(); ++iter)
		{
			if((*iter)->Symbol() == pQuote->symbol())
				(*iter)->InQuote(pQuote, timestamp);
		}
	}
}

COHLCRecordSet* CTechAnalyStrategy::GetRecordSet(const string& symbol, int precision, boost::chrono::steady_clock::time_point& timestamp)
{
  for (vector<CPriceBarDataProxy*>::iterator iter = m_pDataProxies.begin();
		iter != m_pDataProxies.end(); ++iter)
	{
		if((*iter)->Symbol() == symbol &&
			(*iter)->Precision() == precision)
         return (*iter)->GetOHLCRecordSet (timestamp);
   }
   return NULL;
}

COHLCRecordSet* CTechAnalyStrategy::GetRecordSet( const string& symbol, int precision )
{
	for (vector<CPriceBarDataProxy*>::iterator iter = m_pDataProxies.begin();
		iter != m_pDataProxies.end(); ++iter)
	{
		if((*iter)->Symbol() == symbol &&
			(*iter)->Precision() == precision)
			return (*iter)->GetOHLCRecordSet();
	}
	return NULL;
}

void CTechAnalyStrategy::PrepareHistDataSrc(const entity::StrategyItem& strategyItem)
{
	for(int i = 0; i < strategyItem.histsources_size(); ++i)
	{
		const entity::HistSourceCfg& entityCfg = strategyItem.histsources(i);
		HistSrcCfgPtr histCfg(new CHistSourceCfg(entityCfg.symbol(), entityCfg.precision()));
		OnBeforeAddingHistSrcConfig(histCfg.get());
		m_histSrcConfigs.push_back(histCfg);
	}

	RegHistDataSrc();
}

void CTechAnalyStrategy::RegHistDataSrc()
{
	boost::mutex::scoped_lock l(m_mutDataProxy);

	for (vector<HistSrcCfgPtr>::iterator iter = m_histSrcConfigs.begin();
		iter != m_histSrcConfigs.end(); ++iter)
	{
		CPriceBarDataProxy* proxy = m_avatar->TechDataRepo().Register(
			(*iter)->Symbol, (*iter)->Precision, (*iter)->HistData, m_avatar->TradingDay());
		if(proxy != NULL)
			m_pDataProxies.push_back(proxy);
	}
}

void CTechAnalyStrategy::UnregHistDataSrc()
{
	boost::mutex::scoped_lock l(m_mutDataProxy);

	for (vector<CPriceBarDataProxy*>::iterator iter = m_pDataProxies.begin();
		iter != m_pDataProxies.end(); ++iter)
	{
		m_avatar->TechDataRepo().Unregister(*iter);
	}
	m_pDataProxies.clear();
}

void CTechAnalyStrategy::Cleanup()
{
	UnregHistDataSrc();
}
