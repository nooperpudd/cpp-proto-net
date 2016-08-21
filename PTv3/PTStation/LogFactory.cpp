#include "stdafx.h"
#include "LogFactory.h"

#include <boost/filesystem.hpp>

#include <log4cpp/PropertyConfigurator.hh>

#define LOG_CONFIG "./log4cpp.property"
#define LOG_DIRECTORY "./log"

boost::shared_ptr<CLogFactory> CLogFactory::m_instance;
boost::mutex CLogFactory::m_mutex;

CLogFactory::CLogFactory()
{
}

CLogFactory::~CLogFactory()
{
	cout << "Log is shutting down" << endl;
	log4cpp::Category::shutdown();
}

bool CLogFactory::Initialize(const char* logConfigPath, const char* logDestDir) const
{
	log4cpp::PropertyConfigurator::configure(logConfigPath == NULL ? LOG_CONFIG : logConfigPath);
	cout << "Log initialized successfully" << endl;

	boost::filesystem::path logDirPath(logDestDir == NULL ? LOG_DIRECTORY : logDestDir);
	if (boost::filesystem::exists(logDirPath))
	{
		if (boost::filesystem::is_directory(logDirPath))
		{
			cout << "Log directory exists." << endl;
		}
		else
		{
			cout << "There is a duplicate file named 'log' in working directory. Please remove it." << endl;
			return false;
		}

	}
	else
	{
		if (boost::filesystem::create_directory(logDirPath))
		{
			cout << "Log directory is created." << endl;
		}
		else
		{
			cout << "Cannot create log directory." << endl;
			return false;
		}
	}

	return true;
}

log4cpp::Category& CLogFactory::GetLogger() const
{
	return log4cpp::Category::getRoot();
}

log4cpp::Category& CLogFactory::GetLogger(const string& loggerName) const
{
	return log4cpp::Category::getInstance(loggerName);
}
