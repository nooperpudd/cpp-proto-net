#pragma once

#ifndef USE_FEMAS_API

#ifdef USE_ZEUSING_API
#define APP_VERSION "4.5.0 for Zeusing"
#else
#define APP_VERSION "4.5.0"
#endif

#else
#define APP_VERSION "4.4.3 for Femas"
#endif

class CConfiguration
{
public:
	CConfiguration(void);
	~CConfiguration(void);

	bool Load(int argc, char* argv[]);

	bool IsLogEnabled();
	const string& LogConfigPath();

	const string& GetPort();
	int GetPortNum();
	const string& GetAddr();

private:
	// Logging
	bool m_enableLogging;
	std::string m_logConfigFilePath;

	// Connection
	std::string m_addr;
	std::string m_port;

};

