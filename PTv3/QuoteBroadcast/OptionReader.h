#pragma once
class COptionReader
{
public:
	COptionReader();
	~COptionReader();

	bool Load(int argc, char* argv[]);

	const string& getDate(){ return m_date; }
	const string& getSymbol(){ return m_symbol; }
	int getPort();
	int getRate();
	bool AfterFromTime(const string& timestamp);

private:
	string m_date;
	string m_symbol;
	string m_port;
	string m_rate;
	string m_fromTime;
};

