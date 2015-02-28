// QuoteBroadcast.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "OptionReader.h"
#include "TickDataReader.h"

#include <sstream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

using boost::asio::ip::udp;

boost::chrono::steady_clock::time_point GetTimepointFromString(const string& timeString, int millisec)
{
	int hh, mm, ss;
	int got = sscanf(timeString.c_str(), "%d:%d:%d", &hh, &mm, &ss);
	if (got == 3)
	{
		boost::chrono::steady_clock::duration d =
			boost::chrono::hours(hh)
			+ boost::chrono::minutes(mm)
			+ boost::chrono::seconds(ss)
			+ boost::chrono::milliseconds(millisec);
		return boost::chrono::steady_clock::time_point(d);
	}

	return boost::chrono::steady_clock::time_point();
}

int main(int argc, char* argv[])
{
	try
	{
		COptionReader options;
		bool succ = options.Load(argc, argv);
		if (!succ)
			return -1;

		vector<TickDataReaderPtr> tickDataReaders;
		const vector<string>& symbolList = options.getSymbolList();
		for (vector<string>::const_iterator iter = symbolList.begin(); iter != symbolList.end(); ++iter)
		{
			TickDataReaderPtr reader(new CTickDataReader);
			reader->Open(*iter, options.getDate());
			tickDataReaders.push_back(reader);
		}

		
		int port = options.getPort();
		cout << "Begin broadcasting quote to " << port << endl;

		boost::asio::io_service io_service;

		udp::socket socket(io_service, udp::v4());
		socket.set_option(udp::socket::broadcast(true));

		udp::endpoint receiver_endpoint(boost::asio::ip::address_v4::broadcast(), port);

		boost::array<char, 1024> send_buf;
		int millisec = 500 / options.getRate();
		boost::posix_time::millisec interval(millisec);

		boost::chrono::steady_clock::time_point expected = 
			options.getFromTime().empty() ?
				GetTimepointFromString("9:15:00") : 
				GetTimepointFromString(options.getFromTime());

		bool broadcasting = true;
		while (broadcasting)
		{
			int eof = 0;
			for (vector<TickDataReaderPtr>::iterator iter = tickDataReaders.begin();
				iter != tickDataReaders.end(); ++iter)
			{
				CThostFtdcDepthMarketDataField* pMktDataField = NULL;
				READ_TICK_STATUS status = (*iter)->Read(expected, &pMktDataField);
				if (status == DATA_READY)
				{
					send_buf.fill('\0');
					memcpy(send_buf.c_array(), pMktDataField, sizeof(CThostFtdcDepthMarketDataField));

					socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);
					std::cout << pMktDataField->InstrumentID << "\t " << pMktDataField->LastPrice << "\t " << pMktDataField->UpdateTime << " " << pMktDataField->UpdateMillisec << std::endl;
				}
				else if (status == DATA_NOT_AVAIL_FOR_TICK)
				{
					std::cout << "No tick data for " << (*iter)->Symbol() << std::endl;
				}
				else // EOF
				{
					eof += 1;
				}
			}
			
			if (eof == tickDataReaders.size())
				break;

			expected += boost::chrono::milliseconds(500);
			boost::this_thread::sleep(interval);
		}

		std::cout << "Done broadcasting quote for " << options.getSymbols() << " " << options.getDate() << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

