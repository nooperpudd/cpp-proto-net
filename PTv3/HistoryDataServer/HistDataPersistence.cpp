#include "stdafx.h"
#include "HistDataPersistence.h"
#include "LogFactory.h"
#include "FileSystemUtil.h"

#define FIELD_WIDTH 12
#define HIST_DATA_FOLDER "HistData"

using namespace boost::filesystem;

log4cpp::Category& CHistDataPersistence::logger = CLogFactory::GetInstance().GetLogger("HistDataPersistence");
boost::unordered_set<string> CHistDataPersistence::m_stPersistences;
boost::mutex CHistDataPersistence::m_mutPersistences;

CHistDataPersistence::CHistDataPersistence()
	: m_precision(0)
{
}

CHistDataPersistence::~CHistDataPersistence()
{
	if (IsOpen())
		Close();
}

void CHistDataPersistence::Init(const string& symbol, int precision, const boost::gregorian::date& tradingDay)
{
	m_symbol = symbol;
	m_precision = precision;
	m_tradingDayString = boost::gregorian::to_iso_string(tradingDay);
	m_Name = boost::str(boost::format("%s-%u") % m_symbol % m_precision);
}

bool CHistDataPersistence::Open()
{
	if (!m_Name.empty())
	{
		if (!CreateFolderIfNotExists(HIST_DATA_FOLDER))
		{
			logger.errorStream() << "Cannot create folder " << HIST_DATA_FOLDER << log4cpp::eol;
			return false;
		}
		string filePath = boost::str(boost::format("%s/%s.txt")
			% HIST_DATA_FOLDER % m_Name);
		m_ofstream.open(filePath, std::ofstream::out | std::ofstream::app);
		return m_ofstream.is_open();
	}
	return false;
}

bool CHistDataPersistence::Open(const string& symbol, int precision, const boost::gregorian::date& tradingDay)
{
	Init(symbol, precision, tradingDay);
	if (!IfPersistenceExists(m_Name))
	{
		logger.warn("HistDataPersistence %s does already exit", m_Name.c_str());
		return false;
	}
	return Open();
}

void CHistDataPersistence::Close()
{
	m_ofstream.close();
}

void CHistDataPersistence::Write(const string& timestamp, double open, double high, double low, double close)
{
	if (IsOpen())
	{
		m_ofstream << m_tradingDayString << ", ";
		m_ofstream << timestamp << " , " << fixed << setprecision(1);
		m_ofstream << setw(FIELD_WIDTH) << open << ", ";
		m_ofstream << setw(FIELD_WIDTH) << high << ", ";
		m_ofstream << setw(FIELD_WIDTH) << low << ", ";
		m_ofstream << setw(FIELD_WIDTH) << close << endl;
	}
}

bool CHistDataPersistence::IfPersistenceExists(const string& name)
{
	boost::mutex::scoped_lock l(m_mutPersistences);
	boost::unordered_set<string>::iterator iterFound = m_stPersistences.find(name);
	if(iterFound != m_stPersistences.end())
	{
		return true;
	}
	m_stPersistences.insert(name);
	return false;
}
