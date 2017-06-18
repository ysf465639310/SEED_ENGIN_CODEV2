#ifndef SEED_ENGIN_ASIO_SERVER_H
#define SEED_ENGIN_ASIO_SERVER_H
#include<seed_engin_com_stdafx.h>
#include<seed_engin_net_buffer.h>
using namespace SEED_BUFFER;
using namespace SEED_ASIO;
namespace SEED_SERVER_ASIO
{
	class tcp_session;
	typedef const boost::shared_ptr<tcp_session>& tcp_session_type;
	struct DLL_API tcp_server_handler
	{
		boost::function<void(tcp_session_type)> handle_accept;
		boost::function<void(tcp_session_type)> handle_close;
		boost::function<void(tcp_session_type, std::size_t n)> handle_read;
		boost::function<void(tcp_session_type, std::size_t n)> handle_write;
	};
	class DLL_API tcp_session:boost::noncopyable,
		public boost::enable_shared_from_this<tcp_session>
	{
	public:
		typedef seed_tcp_buffer               buffer_type;
		typedef boost::asio::io_service       ios_type;
		typedef boost::asio::ip::tcp::socket  socket_type;
		typedef boost::shared_ptr<tcp_server_handler> handle_type;
		tcp_session(ios_type& ios, SESSION_CONFIG config, handle_type& handle);
		boost::shared_ptr<tcp_session> get_session_ptr();
		void close();
		void start();
		void write(const void * data, std::size_t len);
		ios_type&    io_service();
		socket_type& socket();
		buffer_type& read_buf();
		buffer_type& send_buf();
		handle_type& get_handle();
		ULONG_32 get_stauts();
		class Impl;
	private:
		boost::shared_ptr<Impl> m_impl;
	};
	class DLL_API tcp_server :boost::noncopyable,
		public boost::enable_shared_from_this<tcp_server>
	{
	public:
		typedef seed_tcp_buffer               buffer_type;
		typedef boost::asio::io_service       ios_type;
		typedef boost::asio::ip::tcp::socket  socket_type;
		typedef boost::shared_ptr<tcp_server_handler> handle_type;
		tcp_server(ULONG_32 port, io_service_pool& m_ios_pool, TCP_SERVER_CONFIG config, handle_type& handle);
		void start();
		class Impl;
	private:
		boost::shared_ptr<Impl> m_impl;
	};

}
#endif