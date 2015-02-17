#include "stdafx.h"
#include "TickDataReader.h"
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

READ_TICK_STATUS CTickDataReader::Read(boost::chrono::steady_clock::time_point expected, CThostFtdcDepthMarketDataField** pOutMarketData)
{
	if (m_tickDataFile.good())
	{
		if (m_tpTimestamp == expected)
		{
			*pOutMarketData = &mktDataField;
			return DATA_READY;
		}
		else if (m_tpTimestamp > expected)
			return DATA_NOT_AVAIL_FOR_TICK;

		string timestamp;
		int millisec;
		double last, ask, bid;
		bool parseSucc = false;
		while (!m_tickDataFile.eof() &&
			(timestamp.empty() ||
				(m_tpTimestamp < expected)))
		{
			m_tickDataFile.getline(m_buf, HIST_LINE_MAX_LENGTH);
			
			parseSucc = ParseLine(m_buf, &timestamp, &millisec, &last, &ask, &bid);
			if (!parseSucc)
			{
				std::cout << "Failed to parse line: " << m_buf << std::endl;
				return END_OF_FILE;
			}
			m_tpTimestamp = GetTimepointFromString(timestamp, millisec);
		}

		if (parseSucc)
		{
			strcpy_s(mktDataField.UpdateTime, timestamp.c_str());
			mktDataField.UpdateMillisec = millisec;

			mktDataField.LastPrice = last;
			mktDataField.AskPrice1 = ask;
			mktDataField.BidPrice1 = bid;

			if (m_tpTimestamp == expected)
			{
				*pOutMarketData = &mktDataField;
				return DATA_READY;
			}
			else
				return DATA_NOT_AVAIL_FOR_TICK;
		}

	}

	return END_OF_FILE;
}

bool CTickDataReader::ParseLine(char* pBuf, string* timestamp, int* millisec, double* last, double* ask, double* bid)
{
	int got = sscanf(pBuf, "%s %d:%d:%d.%d,%lf,%lf,%lf,%lf,%d,%d",
		date, &h, &m, &s, &ms, last, ask, bid, &close, &vol, &position);
	if (got == 11)
	{
		*timestamp = boost::str(boost::format("%02d:%02d:%02d") % h % m % s);
		if (ms >= 0 && ms < 400)
		{
			*millisec = 0;
		}
		else if (ms <= 800)
		{
			*millisec = 500;
		}
		else // ms > 800
		{
			*millisec = 0;
			s += 1;
			if (s == 60)
			{
				s = 0;
				m += 1;
				if (m == 60)
				{
					m = 0;
					h += 1;
				}
			}
		}
		return true;
	}
	
	return false;
}
