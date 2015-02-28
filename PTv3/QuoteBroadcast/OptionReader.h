#pragma once
class COptionReader
{
public:
	COptionReader();
	~COptionReader();

	bool Load(int argc, char* argv[]);

	const string& getDate(){ return m_date; }
	const string& getSymbols(){ return m_symbols; }
	const vector<string>& getSymbolList() { return m_symbolList; }
	int getPort();
	int getRate();
	const string& getFromTime(){ return m_fromTime; }
	bool AfterFromTime(const string& timestamp);

private:
	string m_date;
	string m_symbols;
	string m_port;
	string m_rate;
	string m_fromTime;
	vector<string> m_symbolList;
};

