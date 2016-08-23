// StrategyTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LogFactory.h"
#include "MATrendStrategyBox.h"
#include "StrategyOperation.h"


log4cpp::Category* pLogger = NULL;

class MockStrategy : public CStrategyOperation
{
public:
	void OpenPosition(const string& symbol, double price, int vol, const string& timestamp)
	{
		pLogger->info("%s, %s, OPEN, %.2f, %d", timestamp.c_str(), symbol.c_str(), price, vol);
	}
	void ClosePosition(const string& symbol, double price, int vol, const string& timestamp)
	{
		pLogger->info("%s, %s, CLOSE, %.2f, %d", timestamp.c_str(), symbol.c_str(), price, vol);
	}
};

int main()
{
	if (!CLogFactory::GetInstance().Initialize("strategy-log4cpp.property"))
		return -1;
	CLoggerGuard loggerGuard;

	pLogger = &(CLogFactory::GetInstance().GetLogger("main"));
	pLogger->info("Start testing strategy ...");

	MockStrategy mockStrategy;
	CMATrendStrategyBox strategyBox(&mockStrategy);

	strategyBox.Apply(entity::StrategyItem());

    return 0;
}

