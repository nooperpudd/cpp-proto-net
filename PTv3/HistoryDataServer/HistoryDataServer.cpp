// HistoryDataServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HistConfiguration.h"
#include "LogFactory.h"
#include "Scheduler.h"
#include "HistAvatar.h"

#define LOG_CONFIG "./hist-log4cpp.property"

log4cpp::Category* pLogger = NULL;
boost::shared_ptr<CHistAvatar> g_AvatarInst;

void OnStartTimeOutHandler(int startTimePointIdx)
{
	g_AvatarInst.reset(new CHistAvatar);
	g_AvatarInst->Start();
	pLogger->info("Done %d start time point.", startTimePointIdx + 1);
}

void OnEndTimeOutHandler(int endTimePointIdx)
{
	if(g_AvatarInst.get() != NULL)
	{
		g_AvatarInst->Stop();
		g_AvatarInst.reset();
	}
	pLogger->info("Done %d end time point.", endTimePointIdx + 1);
}

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
	TimeOutHandlerFunc funcOnStart(&OnStartTimeOutHandler);
	scheduler.SetStartTimeOutHandler(funcOnStart);
	TimeOutHandlerFunc funcOnEnd(&OnEndTimeOutHandler);
	scheduler.SetEndTimeoutHandler(funcOnEnd);

	scheduler.Run(pConfig->GetStartTimepoints(), pConfig->GetEndTimepoints());
	
	pLogger->infoStream() << "History Data Server exit..." << log4cpp::eol;
    return 0;
}

