#pragma once

class CLogFactory
{
	CLogFactory();
	static boost::shared_ptr<CLogFactory> m_instance;
	static boost::mutex m_mutex;

	static void Destory()
	{
		m_instance.reset();
	}

public:
	
	~CLogFactory();

	static CLogFactory& GetInstance()
	{
		boost::mutex::scoped_lock l(m_mutex);

		if (NULL == m_instance.get())
		{
			m_instance.reset(new CLogFactory);
		}

		return *(m_instance.get());
	}

	bool Initialize(const char* logConfigPath = NULL, const char* logDestDir = NULL) const;

	log4cpp::Category& GetLogger() const;

	log4cpp::Category& GetLogger(const string& loggerName) const;

	friend class CLoggerGuard;
};

class CLoggerGuard
{
public: 
	~CLoggerGuard()
	{
		CLogFactory::Destory();
	}
};
