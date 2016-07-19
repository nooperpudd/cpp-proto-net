#include "stdafx.h"
#include "TickDataReader.h"
#include <boost/format.hpp>

#define TICK_DATA_DIRECTORY "TickData"

#ifndef WIN32
#define strcpy_s strcpy
#endif


void GetStringFromTimepoint(boost::chrono::steady_clock::time_point timePoint, string* outTimeString)
{
	boost::chrono::milliseconds ms = boost::chrono::duration_cast<boost::chrono::milliseconds>(timePoint.time_since_epoch());
	// separate seconds from centiseconds
	boost::chrono::seconds s = boost::chrono::duration_cast<boost::chrono::seconds>(ms);
	ms -= s;
	// separate minutes from seconds
	boost::chrono::minutes m = boost::chrono::duration_cast<boost::chrono::minutes>(s);
	s -= m;
	// separate hours from minutes
	boost::chrono::hours h = boost::chrono::duration_cast<boost::chrono::hours>(m);
	m -= h;

	stringstream ss;
	ss << h.count() << ":" << m.count() << ":" << s.count() << "." << ms.count();
	*outTimeString = ss.str();
}

int GetMilliSecPartOfTimepoint(boost::chrono::steady_clock::time_point timePoint)
{
	boost::chrono::milliseconds ms = boost::chrono::duration_cast<boost::chrono::milliseconds>(timePoint.time_since_epoch());
	// separate seconds from centiseconds
	boost::chrono::seconds s = boost::chrono::duration_cast<boost::chrono::seconds>(ms);
	ms -= s;
	return ms.count();
}

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

	m_tickDataFile.open(tickFilePath.c_str());

	strcpy_s(mktDataField.InstrumentID, sybmol.c_str());
	strcpy_s(mktDataField.TradingDay, date.c_str());

	return m_tickDataFile.good();
}

READ_TICK_STATUS CTickDataReader::Read(boost::chrono::steady_clock::time_point expected, CThostFtdcDepthMarketDataField** pOutMarketData, boost::chrono::steady_clock::time_point& actual)
{
	if (m_tickDataFile.good())
	{
		boost::chrono::milliseconds diff = boost::chrono::duration_cast < boost::chrono::milliseconds >
			(m_tpTimestamp - expected);
		int diff_ms = diff.count();
		if (abs(diff_ms) < 500)
		{
			mktDataField.UpdateMillisec = GetMilliSecPartOfTimepoint(expected);
			*pOutMarketData = &mktDataField;
			m_tpTimestamp = boost::chrono::steady_clock::time_point();	// reset
			return DATA_READY;
		}
		else if (diff_ms >= 500)
			return DATA_NOT_AVAIL_FOR_TICK;

		string timestamp;
		int millisec;
		double last, ask, bid;
		bool parseSucc = false;
		while (!m_tickDataFile.eof() &&
			(timestamp.empty() ||
				(diff_ms < -500)))
		{
			m_tickDataFile.getline(m_buf, HIST_LINE_MAX_LENGTH);
			
			parseSucc = ParseLine(m_buf, &timestamp, &millisec, &last, &ask, &bid);
			if (!parseSucc)
			{
				std::cout << "Failed to parse line: " << m_buf << std::endl;
				return END_OF_FILE;
			}
			m_tpTimestamp = GetTimepointFromString(timestamp, millisec);
			diff = boost::chrono::duration_cast < boost::chrono::milliseconds >
				(m_tpTimestamp - expected);
			if (diff > -boost::chrono::hours(23))
				diff_ms = diff.count();
			else
			{
				expected = m_tpTimestamp;
				diff_ms = 0;
			}
		}

		if (parseSucc)
		{
			strcpy_s(mktDataField.UpdateTime, timestamp.c_str());
			
			mktDataField.LastPrice = last;
			mktDataField.AskPrice1 = ask;
			mktDataField.BidPrice1 = bid;
			mktDataField.AskVolume1 = 1;
			mktDataField.BidVolume1 = 1;

			string act, exp;
			GetStringFromTimepoint(expected, &exp);
			GetStringFromTimepoint(m_tpTimestamp, &act);

			//cout << " Expect:" << exp << " Actual:" << act << endl;
			if (abs(diff_ms) < 500)
			{
				mktDataField.UpdateMillisec = GetMilliSecPartOfTimepoint(expected);
				*pOutMarketData = &mktDataField;
				actual = m_tpTimestamp;
				m_tpTimestamp = boost::chrono::steady_clock::time_point();	// reset
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
		*millisec = ms;
		return true;
	}
	
	return false;
}
