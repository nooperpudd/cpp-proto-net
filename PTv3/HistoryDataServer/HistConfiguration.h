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

	const string& GetBroker() const
	{
		return m_broker;
	}
	const string& GetAcct() const
	{ return m_acct; }
	const string& GetUserId() const
	{ return m_userId; }
	const string& GetPasswd() const
	{ return m_passwd; }

	const string& GetTradingServerAddr() const { return m_tradingAddress; }
	const string& GetMarketDataSvrAddr() const { return m_marketDataAddress; }

	const string& GetStartTimepoints() const { return m_startTimepoints; }
	const string& GetEndTimepoints() const { return m_endTimepoints; }


private:
	static boost::shared_ptr<CHistConfiguration> m_instance;
	static boost::mutex m_mutex;

	static log4cpp::Category& logger;

	void Print();

	string m_tradingAddress;
	string m_marketDataAddress;

	string m_broker;
	string m_acct;
	string m_userId;
	string m_passwd;

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

