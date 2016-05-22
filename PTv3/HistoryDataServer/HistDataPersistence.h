#pragma once

#include <boost/unordered_set.hpp>
#include <boost/date_time.hpp>
#include <boost/filesystem/fstream.hpp>

class CHistDataPersistence
{
public:
	CHistDataPersistence();
	~CHistDataPersistence();
	
	void Init(const string& symbol, int precision, const boost::gregorian::date& tradingDay);
	bool Open();
	bool Open(const string& symbol, int precision, const boost::gregorian::date& tradingDay);
	bool IsOpen() const { return m_ofstream.is_open(); }
	void Close();
	void Write(const string& timestamp, double open, double high, double low, double close);
	const string& Name() const { return m_Name; }

private:
	static bool IfPersistenceExists(const string& name);

	static log4cpp::Category& logger;
	static boost::unordered_set<string> m_stPersistences;
	static boost::mutex m_mutPersistences;

	string m_symbol;
	string m_Name;
	unsigned int m_precision;
	string m_tradingDayString;
	boost::filesystem::ofstream m_ofstream;
};

