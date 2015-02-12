#include "stdafx.h"
#include "OptionReader.h"

#include <boost/date_time.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

namespace po = boost::program_options;

COptionReader::COptionReader()
{
}


COptionReader::~COptionReader()
{
}

bool COptionReader::Load(int argc, char* argv[])
{
	try {
		// Declare a group of options that will be 
		// allowed only on command line
		po::options_description generic("Startup options");
		generic.add_options()
			("date,d", po::value<string>(&m_date),
			"Specify trading date")
			("symbol,s", po::value<string>(&m_symbol),
			"Specify symbol which quotes will be broadcasted")
			("port,p", po::value<string>(&m_port),
			"Specify port to broadcast udp packets");

		po::options_description cmdline_options;
		cmdline_options.add(generic);

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
		po::notify(vm);

		if (m_date.length() > 0
			&& m_symbol.length() > 0
			&& m_port.length() > 0)
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

int COptionReader::getPort()
{
	return boost::lexical_cast<int>(m_port.c_str());
}
