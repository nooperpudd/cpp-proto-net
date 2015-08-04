#pragma once

#include <boost/atomic.hpp>
#include <boost/asio/steady_timer.hpp>

class CPortfolioOrderPlacer;
class CLegOrderPlacer;

class CAsyncOrderPendingTimer
{
public:
	CAsyncOrderPendingTimer(CPortfolioOrderPlacer* pOrdPlacer)
		: m_pOrdPlacer(pOrdPlacer)
		, m_legPlacer(NULL)
		, io_service(new boost::asio::io_service)
		, work(new boost::asio::io_service::work(*io_service))
		, m_timer(*io_service)
		, m_isStop(true)
	{
		m_thWaiting = boost::thread(boost::bind(&boost::asio::io_service::run, io_service));
	}

	~CAsyncOrderPendingTimer()
	{
		Cancel(); // in case still running
		m_thWaiting.join();
	}

	void SetLegOrderPlacer(CLegOrderPlacer* pLegOrderPlacer) { m_legPlacer = pLegOrderPlacer; }
	
	void Run(boost::chrono::steady_clock::time_point expireTimePoint);

	void Cancel() 
	{ 
		if(m_isStop.exchange(true, boost::memory_order_acquire) == false)
		{
			m_timer.cancel();
		}
		
	}
	bool IsStop() { return m_isStop.load(boost::memory_order_consume); }

private:
	void FireEvent(const boost::system::error_code& e);

	boost::shared_ptr< boost::asio::io_service > io_service;
	boost::shared_ptr< boost::asio::io_service::work > work;
	boost::asio::steady_timer m_timer;
	boost::atomic<bool> m_isStop;
	boost::thread m_thWaiting;

	CPortfolioOrderPlacer* m_pOrdPlacer;
	CLegOrderPlacer* m_legPlacer;
};

