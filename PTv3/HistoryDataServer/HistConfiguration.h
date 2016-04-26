#pragma once
class CHistConfiguration
{
private:
	CHistConfiguration();

public:
	~CHistConfiguration();

	static CHistConfiguration* GetInstance()
	{
		boost::mutex::scoped_lock l(m_mutex);

		if(NULL == m_instance.get())
		{
			m_instance.reset(new CHistConfiguration);
		}

		return m_instance.get();
	}

	bool Load();

	const string& GetAcct() const
	{ return m_acct; }
	const string& GetUserId() const
	{ return m_userId; }
	const string& GetPasswd() const
	{ return m_passwd; }

private:
	static boost::shared_ptr<CHistConfiguration> m_instance;
	static boost::mutex m_mutex;


	string m_tradingAddress;
	string m_marketDataAddress;

	string m_acct;
	string m_userId;
	string m_passwd;

	int m_symbolCount;

	string m_startTimepoints;
	string m_endTimepoints;

	class SubSymbolItem
	{
	public:
		string Sybmol;
		string TimeFrame;
	};

	vector<string> m_symbols;
	vector<string> m_timeFrames;
};

