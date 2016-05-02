// HistoryDataServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HistConfiguration.h"
#include "LogFactory.h"
#include "Scheduler.h"

#define LOG_CONFIG "./hist-log4cpp.property"

log4cpp::Category* pLogger = NULL;

int main(int argc, char* argv[])
{
	if (!CLogFactory::GetInstance().Initialize())
		return -1;

	pLogger = &(CLogFactory::GetInstance().GetLogger("main"));

	CHistConfiguration* pConfig = CHistConfiguration::GetInstance();
	if(!pConfig->Load())
	{
		pLogger->errorStream() << "Failed to load configurations" << log4cpp::eol;
		return -1;
	}

	CScheduler scheduler;
	scheduler.Run(pConfig->GetStartTimepoints(), pConfig->GetEndTimepoints());
	
    return 0;
}

