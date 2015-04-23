#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>

template < typename T >
class CBufferRunner : public CThreadWorker
{
public:
	CBufferRunner(int threadNum = 1) : CThreadWorker(threadNum)
	{
	}

	CBufferRunner(boost::function<void(T&)> callback, int threadNum = 1)
		: CBufferRunner(threadNum)
	{
		Init(callback);
	}

	~CBufferRunner(void)
	{
	}

	void Init(boost::function<void(T&)> callback)
	{
		m_jobCallback = callback;
	}

	void Enqueue(T stuff)
	{
		QueueWorkItem(boost::bind(&CBufferRunner<T>::DoWork, this, stuff));
	}

private:
	void DoWork(T& stuff)
	{
		if (!m_jobCallback.empty())
			m_jobCallback(stuff);
	}

	boost::function<void(T&)> m_jobCallback;
};


class CThreadWorker
{
public:
	CThreadWorker(int threadNum = 1)
		: io_service(new boost::asio::io_service)
		, work(new boost::asio::io_service::work(*io_service))
	{
		for (int i = 0; i < threadNum; ++i)
			worker_threads.create_thread(boost::bind(&boost::asio::io_service::run, io_service));
		running.store(true);
	}

	~CThreadWorker()
	{
		work.reset();

		Stop();
	}

	void Stop()
	{
		running.store(false, boost::memory_order_release);
		io_service->stop();	// ensure io_service stopped
		worker_threads.join_all();
	}

	template <typename WorkHandler>
	void QueueWorkItem(WorkHandler handler)
	{
		if (running.load(boost::memory_order_acquire))
			io_service->post(handler);
	}

private:
	boost::shared_ptr< boost::asio::io_service > io_service;
	boost::shared_ptr< boost::asio::io_service::work > work;
	boost::thread_group worker_threads;
	boost::atomic<bool> running;
};

