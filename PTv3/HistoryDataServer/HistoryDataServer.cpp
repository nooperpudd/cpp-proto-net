// HistoryDataServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HistConfiguration.h"



int main(int argc, char* argv[])
{
	CHistConfiguration* config = CHistConfiguration::GetInstance();
	bool succ = config->Load();
	
    return 0;
}

