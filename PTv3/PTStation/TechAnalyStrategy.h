#pragma once

#include "Strategy.h"

enum CLOSE_POSITION_PURPOSE
{
	CLOSE_POSITION_UNKNOWN,
	CLOSE_POSITION_FORCE,
	CLOSE_POSITION_STOP_GAIN,
	CLOSE_POSITION_STOP_LOSS
};

class CPriceBarDataProxy;
class CAvatarClient;
class COHLCRecordSet;

class CHistSourceCfg
{
public:
	CHistSourceCfg(const string& symb, int precision)
		: Symbol(symb), Precision(precision)
		, HistData(false)
		, WeightAvg(false)
	{}

	string Symbol;
	int Precision;
	bool HistData;
	bool WeightAvg;
};

typedef boost::shared_ptr<CHistSourceCfg> HistSrcCfgPtr;

class CTechAnalyStrategy : public CStrategy
{
public:
	CTechAnalyStrategy(CAvatarClient* pAvatar);
	virtual ~CTechAnalyStrategy(void);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	virtual void Cleanup();
   
	COHLCRecordSet* GetRecordSet(const string& symbol, int precision, boost::chrono::steady_clock::time_point& timestamp);  
	COHLCRecordSet* GetRecordSet(const string& symbol, int precision);
protected:
	const vector<HistSrcCfgPtr>& HistSrcConfigs(){ return m_histSrcConfigs; }
	const vector<CPriceBarDataProxy*>& DataProxies() { return m_pDataProxies; }

	virtual void OnBeforeAddingHistSrcConfig(CHistSourceCfg* pHistSrcCfg){}

	void PrepareHistDataSrc(const entity::StrategyItem& strategyItem);
	void RegHistDataSrc();
	void UnregHistDataSrc();

private:
	vector<HistSrcCfgPtr> m_histSrcConfigs;
	vector<CPriceBarDataProxy*> m_pDataProxies;
	boost::mutex m_mutDataProxy;

	CAvatarClient* m_avatar;
};

const char* GetPosiDirectionText(entity::PosiDirectionType direction, bool chs = false);

