#include "LogManager.h"
#include "Configuration.h"

#include <iostream>

#include <log4cpp/Portability.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

using namespace std;

extern CConfiguration config;

log4cpp::Category& GetRootLogger()
{
	return log4cpp::Category::getRoot();
}

std::string loggerName("logger");

log4cpp::Category& GetLogger()
{
	return log4cpp::Category::getInstance(loggerName);
}

CLogManager::CLogManager(void):
	m_isEnabled(false)
{
}

CLogManager::~CLogManager(void)
{
}

void CLogManager::Init()
{
	bool enabled = config.IsLogEnabled();
	if(!enabled){
		cout << "Log is DISABLED" << endl;
		return;
	}

	try
	{
		std::string initFileName = "./log4cpp.property";
		log4cpp::PropertyConfigurator::configure(initFileName);

		m_isEnabled = true;
		cout << "Log initialized successfully" << endl;
	}
	catch(exception& e)
	{
		cerr << "Failed to initialize log due to " << e.what() << endl;
	}
}

void CLogManager::Trace( const char* text )
{
	if(m_isEnabled)
	{
		Debug(text);
	}
}

void CLogManager::Trace( const std::string& text)
{
	Debug(text);
}

void CLogManager::Debug( const char* text )
{
	if(m_isEnabled)
	{
		GetRootLogger().debug(text);
	}
}

void CLogManager::Debug( const std::string& text)
{
	if(m_isEnabled)
	{
		GetRootLogger().debug(text);
	}
}

void CLogManager::Info( const char* text )
{
	if(m_isEnabled)
	{
		GetLogger().info(text);
	}
}

void CLogManager::Info( const std::string& text)
{
	if(m_isEnabled)
	{
		GetLogger().info(text);
	}
}

void CLogManager::Warning( const char* text )
{
	if(m_isEnabled)
	{
		GetLogger().warn(text);
	}
}

void CLogManager::Warning( const std::string& text)
{
	if(m_isEnabled)
	{
		GetLogger().warn(text);
	}
}

void CLogManager::Error( const char* text )
{
	if(m_isEnabled)
	{
		GetLogger().error(text);
	}
}

void CLogManager::Error( const std::string& text)
{
	if(m_isEnabled)
	{
		GetLogger().error(text);
	}
}

void CLogManager::Fatal( const char* text )
{
	if(m_isEnabled)
	{
		GetLogger().fatal(text);
	}
}

void CLogManager::Fatal( const std::string& text)
{
	if(m_isEnabled)
	{
		GetLogger().fatal(text);
	}
}