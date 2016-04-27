#pragma once
class CLogFactory
{
	CLogFactory();
	static boost::shared_ptr<CLogFactory> m_instance;
	static boost::mutex m_mutex;

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

	bool Initialize();

	log4cpp::Category& GetLogger();

	log4cpp::Category& GetLogger(const string& loggerName);
};

