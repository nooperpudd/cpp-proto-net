#pragma once

#include <boost/filesystem.hpp>

#include "ThostFtdcUserApiStruct.h"

#define HIST_LINE_MAX_LENGTH 256

using namespace boost::filesystem;

enum READ_TICK_STATUS
{
	DATA_READY = 0,
	DATA_NOT_AVAIL_FOR_TICK = -1,
	END_OF_FILE = -2
};

class CTickDataReader
{
public:
	CTickDataReader();
	~CTickDataReader();

	bool Open(const string& sybmol, const string& date);
	const char* Symbol(){ return mktDataField.InstrumentID; }
	READ_TICK_STATUS Read(boost::chrono::steady_clock::time_point expected, CThostFtdcDepthMarketDataField** pOutMarketData, boost::chrono::steady_clock::time_point& actual);

private:

	bool ParseLine(char* pBuf, string* timestamp, int* millisec, double* last, double* ask, double* bid);

	std::ifstream m_tickDataFile;
	char m_buf[HIST_LINE_MAX_LENGTH];

	CThostFtdcDepthMarketDataField mktDataField;
	boost::chrono::steady_clock::time_point m_tpTimestamp;

	// for ParseLine
	int h, m, s, ms;
	char date[12];
	int vol;
	int position;
	double close;
};

typedef boost::shared_ptr<CTickDataReader> TickDataReaderPtr;

