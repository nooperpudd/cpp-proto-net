#pragma once

#include "shm_remover.h"

#include <string>
#include <vector>

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

using namespace std;
using namespace boost::interprocess;

#define SHM_SUBSCRIBE_NAME "SubscribeQuote_1-"

struct SubscribeQuoteObj  
{  
	enum { MaxCount = 10, SymbolSize = 10 };  

	SubscribeQuoteObj()  
		: subscribe(true)
		, running(false)
		, ready(false)
		, count(0)
	{
		clear();
	}

	void clear()
	{
		memset(items, 0, sizeof(items));
	}

	//Mutex to protect access to the object  
	boost::interprocess::interprocess_mutex      mutex;  

	//Condition to wait when the object is ready for subscribe or unsubscribe  
	boost::interprocess::interprocess_condition  cond_ready;  

	//Condition to wait when the object is to be submit  
	boost::interprocess::interprocess_condition  cond_submit;  

	bool	ready;

	bool	subscribe;
	bool	running;

	int		count;
	//Items to fill  
	char	items[SymbolSize][MaxCount];
};  

typedef boost::function<void(bool)> OnSubscriberReadyFunc;

class CShmQuoteSubscribeProducer
{
public:
	CShmQuoteSubscribeProducer(const string& shmName)
		: m_shmName(shmName), m_remover(shmName), m_pData(NULL){}
	~CShmQuoteSubscribeProducer()
	{
		m_thWaitingForReady.join();
	}

	bool Init();
	void Put(vector<string>& symbols, bool subscribe);
	void NotifyTerminate();
	
	bool GetReady(int timeout);
	void SetOnReadyHandler(OnSubscriberReadyFunc onReady)
	{
		m_funcOnReady = onReady;
	}

private:
	void CheckReadyProc();

	string m_shmName;
	shm_remove m_remover;
	shared_memory_object m_shm;
	mapped_region m_region;

	SubscribeQuoteObj* m_pData;

	boost::thread m_thWaitingForReady;
	OnSubscriberReadyFunc m_funcOnReady;
};

typedef boost::function<void(char**,int)> SubscribeFunc;
typedef boost::function<void()> TerminateFunc;

class CShmQuoteSubscribeConsumer
{
public:
	CShmQuoteSubscribeConsumer(const string& shmName, SubscribeFunc subFunc, SubscribeFunc unsubFunc,
								TerminateFunc onTerminate)
		: m_shmName(shmName), m_pData(NULL)
		, m_subscribeSymbolFunc(subFunc), m_unsubSymbolFunc(unsubFunc)
		, m_onTerminateFunc(onTerminate){}
	~CShmQuoteSubscribeConsumer()
	{
		if(m_pData != NULL)
		{
			scoped_lock<interprocess_mutex> lock(m_pData->mutex);
			m_pData->running = false;
			m_pData->cond_submit.notify_one();
		}
		m_thread.join();
		m_pData = NULL;
	}

	bool Init();
	void Start();


private:

	void GetProc();

	string m_shmName;
	shared_memory_object m_shm;
	mapped_region m_region;

	SubscribeQuoteObj* m_pData;

	SubscribeFunc m_subscribeSymbolFunc;
	SubscribeFunc m_unsubSymbolFunc;
	TerminateFunc m_onTerminateFunc;

	boost::thread m_thread;
};

