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

private:
	string m_date;
	string m_symbol;
	string m_port;
};

