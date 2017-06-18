#ifndef _SEED_ENGIN_ASIO_CLIENT_H_
#define _SEED_ENGIN_ASIO_CLIENT_H_
#include<seed_engin_com_stdafx.h>
using namespace SEED_BUFFER;
namespace SEED_ASIO_CLIENT
{
	class tcp_client;
	typedef const boost::shared_ptr<tcp_client>& tcp_client_type;
	struct DLL_API tcp_client_handler
	{
		boost::function<void(tcp_client_type)> handle_open;
		boost::function<void(tcp_client_type)> handle_close;

		boost::function<void(tcp_client_type, std::size_t n)> handle_read;
		boost::function<void(tcp_client_type, std::size_t n)> handle_write;
	};

	class DLL_API tcp_client: boost::noncopyable,
			public boost::enable_shared_from_this<tcp_client>
	{
	public: 
		typedef seed_tcp_buffer              buffer_type;
		typedef boost::asio::ip::tcp::socket socket_type;
		typedef boost::asio::io_service 	 ios_type;
		typedef boost::asio::ip::tcp  		 tcp_type;
		typedef boost::shared_ptr<tcp_client_handler> handle_type;
		tcp_client(ios_type& ios,handle_type& handle);
		~tcp_client();
		void send(IN const CHAR* data, IN size_t length);
		void connect(tcp_type::endpoint ep);
		void reconnect();
		void close();
		ULONG_32 get_status();
		handle_type& get_handle();
		buffer_type& read_buf();
		buffer_type& send_buf();
		class impl;
	private:	
		boost::shared_ptr<impl> m_sptr;
	};
}
#endif