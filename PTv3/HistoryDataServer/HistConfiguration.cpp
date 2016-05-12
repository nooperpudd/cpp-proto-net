#include "stdafx.h"
#include "HistConfiguration.h"
#include "LogFactory.h"

#include <boost/program_options.hpp>
#include <fstream>

#define CONFIG_FILE "config.ini"

namespace po = boost::program_options;

boost::shared_ptr<CHistConfiguration> CHistConfiguration::m_instance;
boost::mutex CHistConfiguration::m_mutex;

log4cpp::Category& CHistConfiguration::logger = CLogFactory::GetInstance().GetLogger("Configuration");

enum { MAX_SUB_SYMBOL_COUNT = 10 };

CHistConfiguration::CHistConfiguration()
{
	m_symbols.resize(MAX_SUB_SYMBOL_COUNT);
	m_timeFrames.resize(MAX_SUB_SYMBOL_COUNT);
}


CHistConfiguration::~CHistConfiguration()
{
}

bool CHistConfiguration::Load()
{
	try {
		// Declare a group of options that will be 
		// allowed only on command line
		po::options_description configDesc("CommonConfig");
		configDesc.add_options()
			("Server.trading", po::value<string>(&m_tradingAddress))
			("Server.market_data", po::value<string>(&m_marketDataAddress))
			("Login.broker", po::value<string>(&m_broker))
			("Login.acct", po::value<string>(&m_acct))
			("Login.user_id", po::value<string>(&m_userId))
			("Login.passwd", po::value<string>(&m_passwd))
			("Schedule.start", po::value<string>(&m_startTimepoints))
			("Schedule.end", po::value<string>(&m_endTimepoints))
			;
			
		for (int i = 0; i < MAX_SUB_SYMBOL_COUNT; ++i)
		{
			string symbConfigName = boost::str(boost::format("Symbol%d.symbol") % (i + 1));
			string timeFrameConfigName = boost::str(boost::format("Symbol%d.time_frames") % (i + 1));
			po::options_description symbDesc("Symbol1");
			symbDesc.add_options()
				(symbConfigName.c_str(), po::value<string>(&m_symbols[i]))
				(timeFrameConfigName.c_str(), po::value<string>(&m_timeFrames[i]))
				;
			configDesc.add(symbDesc);
		}
		
		po::variables_map vm;
		std::ifstream file;
		file.open(CONFIG_FILE);
		logger.debug("Parsing %s for configurations", CONFIG_FILE);
		po::store(po::parse_config_file(file, configDesc), vm);
		po::notify(vm);
		
		Print();

		return true;
	}
	catch (exception& e) {
		logger.errorStream() << "error: " << e.what() << log4cpp::eol;
	}
	catch (...) {
		logger.errorStream() << "Exception of unknown type!" << log4cpp::eol;
	}
	// must be something wrong, thus get here
	return false;
}

int CHistConfiguration::GetSymbolInfos(vector<boost::tuple<string, string>>& symbolTuples)
{
	int count = m_symbols.size() <= m_timeFrames.size() ? m_symbols.size() : m_timeFrames.size();
	for (int i = 0; i < count; ++i)
	{
		symbolTuples.push_back(boost::make_tuple(m_symbols[i], m_timeFrames[i]));
	}

	return symbolTuples.size();
}

void CHistConfiguration::Print()
{
	logger.infoStream() << "Server.trading: " << m_tradingAddress << log4cpp::eol;
	logger.infoStream() << "Server.market_data: " << m_marketDataAddress << log4cpp::eol;

}
