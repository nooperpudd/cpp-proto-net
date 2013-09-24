#pragma once

#include "Strategy.h"

class CPriceBarDataProxy;
class CAvatarClient;
class COHLCRecordSet;

class CHistSourceCfg
{
public:
	CHistSourceCfg(const string& symb, int precision)
		: Symbol(symb), Precision(precision)
	{}

	string Symbol;
	int Precision;
};

typedef boost::shared_ptr<CHistSourceCfg> HistSrcCfgPtr;

class CTechAnalyStrategy : public CStrategy
{
public:
	CTechAnalyStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar);
	virtual ~CTechAnalyStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void Cleanup();
   
	COHLCRecordSet* GetRecordSet(const string& symbol, int precision, boost::chrono::steady_clock::time_point& timestamp);  

protected:
	const vector<HistSrcCfgPtr>& HistSrcConfigs(){ return m_histSrcConfigs; }
	const vector<CPriceBarDataProxy*>& DataProxies() { return m_pDataProxies; }

	void RegHistDataSrc();
	void UnregHistDataSrc();

private:
	vector<HistSrcCfgPtr> m_histSrcConfigs;
	vector<CPriceBarDataProxy*> m_pDataProxies;
	boost::mutex m_mutDataProxy;

	CAvatarClient* m_avatar;
};

