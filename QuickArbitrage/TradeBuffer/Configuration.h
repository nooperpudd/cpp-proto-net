#pragma once

#include <string>

#include "LogSeverityLevel.h"

#define CONSOLE "console"
#define REMOTE "remote"

class CConfiguration
{
public:
	CConfiguration(void);
	~CConfiguration(void);

	bool Load(int argc, _TCHAR* argv[]);

	bool IsLogEnabled();
	const char* GetLogFilePath();
	severity_level GetLogLevel();

	const char* GetMarketDataAddr();
	const char* GetBrokerID();
	const char* GetInvestorID();
	const char* GetPassword();
	const char* GetControlType();

private:
	// Logging
	bool m_enableLogging;
	std::string m_logFilePath;
	severity_level m_logLevel;

	// Quote
	std::string m_marketDataAddress;
	std::string m_brokerID;
	std::string m_investorID;
	std::string m_password;

	std::string m_controlType;
};

