// QuoteBroadcast.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "OptionReader.h"

#include <sstream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "../PTStation/ThostTraderApi/ThostFtdcUserApiStruct.h"

using boost::asio::ip::udp;
using namespace std;
using namespace boost::filesystem;

#define HIST_LINE_MAX_LENGTH 256

bool ParseLine(char* pBuf, string* timestamp, int* millisec, double* last, double* ask, double* bid);

int main(int argc, char* argv[])
{
	try
	{
		COptionReader options;
		bool succ = options.Load(argc, argv);
		if (!succ)
			return -1;

		string tickFilePath = boost::str(boost::format("TickData/%s_%s.csv") 
			% options.getSymbol() % options.getDate());

		cout << "Checking tick historical data file ...";
		path today(tickFilePath);
		if (!exists(today))
		{
			cout << "   Not found!" << endl;
			return -2;
		}
		cout << "   OK" << endl;
		int port = options.getPort();
		cout << "Begin broadcasting quote to " << port << endl;

		boost::asio::io_service io_service;

		udp::socket socket(io_service, udp::v4());
		socket.set_option(udp::socket::broadcast(true));

		udp::endpoint receiver_endpoint(boost::asio::ip::address_v4::broadcast(), port);

		char buf[HIST_LINE_MAX_LENGTH];
		ifstream f(tickFilePath.c_str());
		if (f.good())
		{
			while (!f.eof())
			{
				f.getline(buf, HIST_LINE_MAX_LENGTH);

				string timestamp;
				int millisec;
				double last, ask, bid;
				bool succ = ParseLine(buf, &timestamp, &millisec, &last, &ask, &bid);
				if (succ)
				{
					
				}
			}
		}
		ostringstream timeStream;
		boost::posix_time::time_facet* const timeFormat = new boost::posix_time::time_facet("%H:%M:%S");
		timeStream.imbue(std::locale(timeStream.getloc(), timeFormat));

		boost::array<char, 1024> send_buf;
		//strcpy_s(send_buf.c_array(), 20, "test string to send");



		CThostFtdcDepthMarketDataField mktDataField;
		memset(&mktDataField, 0, sizeof(mktDataField));

		strcpy_s(mktDataField.InstrumentID, "IF1502");

		for (int i = 0; i < 6000; ++i)
		{
			mktDataField.LastPrice = 3200;
			mktDataField.AskPrice1 = 3201;
			mktDataField.BidPrice1 = 3200;

			const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
			timeStream << now;
			string timeText = timeStream.str();
			strcpy_s(mktDataField.UpdateTime, timeText.c_str());
			timeStream.str("");
			timeStream.clear();

			send_buf.fill('\0');
			memcpy(send_buf.c_array(), &mktDataField, sizeof(mktDataField));

			socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);
			std::cout << "Sent " << i << ": " << timeText << std::endl;
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}

		/*
		boost::array<char, 128> recv_buf;
		udp::endpoint sender_endpoint;
		size_t len = socket.receive_from(
		boost::asio::buffer(recv_buf), sender_endpoint);

		std::cout.write(recv_buf.data(), len);
		*/

		std::cout << "Send All Done" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

bool ParseLine(char* pBuf, string* timestamp, int* millisec, double* last, double* ask, double* bid)
{
	static int h, m, s, ms;
	static char date[12];
	static int vol;
	static int position;
	static double close;

	int got = sscanf(pBuf, "%s %d:%d:%d.%d,%lf,%lf,%lf,%lf,%d,%d", 
		date, &h, &m, &s, &ms, last, ask, bid, &close, &vol, &position);
	*timestamp = boost::str(boost::format("%02d:%02d:%02d") % h % m % s);
	*millisec = ms < 500 ? 0 : 500;
	return got == 11;
}