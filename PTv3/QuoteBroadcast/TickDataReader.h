#pragma once

#include <boost/filesystem.hpp>

#include "../PTStation/ThostTraderApi/ThostFtdcUserApiStruct.h"

#define HIST_LINE_MAX_LENGTH 256

using namespace boost::filesystem;

class COptionReader;

class CTickDataReader
{
public:
	CTickDataReader();
	~CTickDataReader();

	bool Open(const string& sybmol, const string& date);

	bool Read(CThostFtdcDepthMarketDataField** pOutMarketData, COptionReader* pOptions = NULL);

private:

	bool ParseLine(char* pBuf, string* timestamp, int* millisec, double* last, double* ask, double* bid);

	ifstream m_tickDataFile;
	char m_buf[HIST_LINE_MAX_LENGTH];

	CThostFtdcDepthMarketDataField mktDataField;

	// for ParseLine
	int h, m, s, ms;
	char date[12];
	int vol;
	int position;
	double close;
};

typedef boost::shared_ptr<CTickDataReader> TickDataReaderPtr;

