#pragma once

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>

using boost::asio::ip::udp;

typedef boost::function<void(char*, std::size_t)> OnDataReceivedFunc;

class CUdpListener : private boost::noncopyable
{
public:
	explicit CUdpListener(unsigned int port, OnDataReceivedFunc on_data_recv) 
		: io_service_()
		, socket_(io_service_)
		, on_data_recv_(on_data_recv)
	{
		socket_.open(udp::v4());
		socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
		socket_.bind(udp::endpoint(boost::asio::ip::address_v4::any(), port));

		start_receive();
		running_thread_ = boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_));
	}
	~CUdpListener()
	{
		socket_.close();
		io_service_.stop();
		if (boost::this_thread::get_id() != running_thread_.get_id())
			running_thread_.join();
		std::cout << "CUdpListener destructor done." << std::endl;
	}
private:
	void start_receive()
	{
		recv_buffer_.fill('\0');

		socket_.async_receive_from(
			boost::asio::buffer(recv_buffer_), remote_endpoint_,
			boost::bind(&CUdpListener::handle_receive, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive(const boost::system::error_code& error,
		std::size_t bytes_transferred)
	{
		if (!error || error == boost::asio::error::message_size)
		{
			// handle received quote
			on_data_recv_(recv_buffer_.c_array(), bytes_transferred);

			start_receive();
		}
		else if (error == boost::asio::error::operation_aborted)
		{
			std::cout << "socket is closing" << std::endl;
		}
		else
		{
			std::cout << "error happens " << error << std::endl;
		}
	}

	OnDataReceivedFunc on_data_recv_;

	/// The io_service used to perform asynchronous operations.
	boost::asio::io_service io_service_;
	boost::thread running_thread_;

	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	boost::array<char, 4096> recv_buffer_;
};

typedef boost::shared_ptr<CUdpListener> UdpListenerPtr;