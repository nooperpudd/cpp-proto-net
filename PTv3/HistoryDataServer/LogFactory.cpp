#include "stdafx.h"
#include "LogFactory.h"

#include <boost/filesystem.hpp>

#include <log4cpp/PropertyConfigurator.hh>

#define LOG_CONFIG "./hist-log4cpp.property"
#define LOG_DIRECTORY "./log"

boost::shared_ptr<CLogFactory> CLogFactory::m_instance;
boost::mutex CLogFactory::m_mutex;

CLogFactory::CLogFactory()
{
}

CLogFactory::~CLogFactory()
{
	log4cpp::Category::shutdown();
}

bool CLogFactory::Initialize()
{
	log4cpp::PropertyConfigurator::configure(LOG_CONFIG);
	cout << "Log initialized successfully" << endl;

	boost::filesystem::path logDirPath(LOG_DIRECTORY);
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

log4cpp::Category& CLogFactory::GetLogger()
{
	return log4cpp::Category::getRoot();
}

log4cpp::Category& CLogFactory::GetLogger(const string& loggerName)
{
	return log4cpp::Category::getInstance(loggerName);
}
