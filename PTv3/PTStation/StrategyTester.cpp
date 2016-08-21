// StrategyTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LogFactory.h"
#include "STStrategyBox.h"

log4cpp::Category* pLogger = NULL;

int main()
{
	if (!CLogFactory::GetInstance().Initialize("strategy-log4cpp.property"))
		return -1;
	CLoggerGuard loggerGuard;

	pLogger = &(CLogFactory::GetInstance().GetLogger("main"));
	pLogger->info("Start testing strategy ...");

	CSTStrategyBox strategyBox;

    return 0;
}

