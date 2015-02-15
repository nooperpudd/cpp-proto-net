#include "stdafx.h"
#include "TickDataReader.h"
#include "OptionReader.h"
#include <boost/format.hpp>

#define TICK_DATA_DIRECTORY "TickData"

CTickDataReader::CTickDataReader()
{
	memset(&mktDataField, 0, sizeof(mktDataField));
}


CTickDataReader::~CTickDataReader()
{
}

bool CTickDataReader::Open(const string& sybmol, const string& date)
{
	string tickFilePath = boost::str(boost::format("%s/%s_%s.csv")
		% TICK_DATA_DIRECTORY % sybmol % date);

	cout << "Checking tick historical data file " << tickFilePath << " ...";
	path today(tickFilePath);
	if (!exists(today))
	{
		cout << "   Not found!" << endl;
		return false;
	}
	cout << "   OK" << endl;

	m_tickDataFile.open(tickFilePath);

	strcpy_s(mktDataField.InstrumentID, sybmol.c_str());
	strcpy_s(mktDataField.TradingDay, date.c_str());

	return m_tickDataFile.good();
}

bool CTickDataReader::Read(CThostFtdcDepthMarketDataField** pOutMarketData, COptionReader* pOptions /*= NULL*/)
{
	if (m_tickDataFile.good())
	{
		string timestamp;
		int millisec;
		double last, ask, bid;
		bool parseSucc = false;
		while (!m_tickDataFile.eof() &&
			(timestamp.empty() ||
			(pOptions != NULL && !pOptions->AfterFromTime(timestamp))))
		{
			m_tickDataFile.getline(m_buf, HIST_LINE_MAX_LENGTH);
			
			parseSucc = ParseLine(m_buf, &timestamp, &millisec, &last, &ask, &bid);
			if (!parseSucc)
			{
				std::cout << "Failed to parse line: " << m_buf << std::endl;
				return false;
			}
		}

		if (parseSucc)
		{
			strcpy_s(mktDataField.UpdateTime, timestamp.c_str());
			mktDataField.UpdateMillisec = millisec;

			mktDataField.LastPrice = last;
			mktDataField.AskPrice1 = ask;
			mktDataField.BidPrice1 = bid;
			*pOutMarketData = &mktDataField;
			return true;
		}
	}

	return false;
}

bool CTickDataReader::ParseLine(char* pBuf, string* timestamp, int* millisec, double* last, double* ask, double* bid)
{
	int got = sscanf(pBuf, "%s %d:%d:%d.%d,%lf,%lf,%lf,%lf,%d,%d",
		date, &h, &m, &s, &ms, last, ask, bid, &close, &vol, &position);
	*timestamp = boost::str(boost::format("%02d:%02d:%02d") % h % m % s);
	*millisec = ms < 400 ? 0 : 500;
	return got == 11;
}
