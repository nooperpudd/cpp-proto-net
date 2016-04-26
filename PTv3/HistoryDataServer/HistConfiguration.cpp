#include "stdafx.h"
#include "HistConfiguration.h"

#include <boost/program_options.hpp>
#include <fstream>

namespace po = boost::program_options;

boost::shared_ptr<CHistConfiguration> CHistConfiguration::m_instance;
boost::mutex CHistConfiguration::m_mutex;

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
		file.open("config.ini");
		po::store(po::parse_config_file(file, configDesc), vm);
		po::notify(vm);
		

		return true;
	}
	catch (exception& e) {
		cerr << "error: " << e.what() << endl;
	}
	catch (...) {
		cerr << "Exception of unknown type!" << endl;
	}
	// must be something wrong, thus get here
	return false;
}
