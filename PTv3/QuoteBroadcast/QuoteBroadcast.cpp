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


#include "../PTStation/ThostTraderApi/ThostFtdcUserApiStruct.h"

using boost::asio::ip::udp;

bool ParseLine(char* pBuf, string* timestamp, int* millisec, double* last, double* ask, double* bid);

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

		bool broadcasting = true;
		while (broadcasting)
		{
			for (vector<TickDataReaderPtr>::iterator iter = tickDataReaders.begin();
				iter != tickDataReaders.end(); ++iter)
			{
				CThostFtdcDepthMarketDataField* pMktDataField = NULL;
				broadcasting = (*iter)->Read(&pMktDataField, &options);
				if (broadcasting)
				{
					send_buf.fill('\0');
					memcpy(send_buf.c_array(), pMktDataField, sizeof(CThostFtdcDepthMarketDataField));

					socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);
					std::cout << pMktDataField->InstrumentID << "\t " << pMktDataField->LastPrice << "\t " << pMktDataField->UpdateTime << " " << pMktDataField->UpdateMillisec << std::endl;
				}
			}

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

