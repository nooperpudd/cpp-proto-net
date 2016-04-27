// HistoryDataServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HistConfiguration.h"
#include "LogFactory.h"

#define LOG_CONFIG "./hist-log4cpp.property"

int main(int argc, char* argv[])
{
	if (!CLogFactory::GetInstance().Initialize())
		return -1;

	CHistConfiguration* config = CHistConfiguration::GetInstance();
	bool succ = config->Load();
	
    return 0;
}

