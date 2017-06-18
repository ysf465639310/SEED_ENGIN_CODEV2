#include <seed_engin_com_stdafx.h>
#include<seed_engin_asio_client.h>
#include<seed_engin_asio_client_struct.h>
using namespace SEED_ASIO_CLIENT;
using namespace SEED_BUFFER;
using namespace SEED_ASIO;
class tcp_client::impl:boost::noncopyable,
	public boost::enable_shared_from_this<tcp_client::impl>

{
public:
		typedef seed_tcp_buffer              buffer_type;
		typedef tcp_client::socket_type      socket_type;
		typedef tcp_client::ios_type         ios_type;
		typedef tcp_client::tcp_type         tcp_type;
		typedef ios_type::strand			 strand_type;
		typedef boost::shared_ptr<tcp_client_handler> handle_type;
		impl(ios_type& ios,handle_type& handle);
		virtual ~impl();
		void write(const CHAR* data,ULONG_32 ullength);
		ULONG_32 get_status();
		void to_close();
		void do_close();
		void to_connect(tcp_type::endpoint ep);
		void do_connect(tcp_type::endpoint ep);
		void re_connect();
		handle_type& get_handle();
		buffer_type& read_buf();
		buffer_type& send_buf();
private:
		void start_connect(tcp_type::endpoint ep);
		void handle_connect(const boost::system::error_code& error);
		void read();
		void close();
		void handle_read(const boost::system::error_code& error,size_t bytes_transferred);
		void handle_write(const boost::system::error_code& error,size_t bytes_transferred);
		void write();
		void setsocketopt();
private:
	ios_type&  	m_ios;
	socket_type m_socket;
	strand_type m_strand;
	boost::shared_ptr<tcp_client_handler> m_handle;
	boost::shared_ptr<tcp_client>   	  m_client;
	seed_tcp_buffer			m_recv_buffer;
	seed_tcp_buffer			m_send_buffer;
	std::string m_ip;
	ULONG_32 m_port;
	boost::atomic_uint32_t  m_status;
};
tcp_client::impl::impl(ios_type& ios,handle_type& handle):m_ios(ios),m_socket(ios),m_strand(ios), m_handle(handle)
{
	m_send_buffer.prepare();
	m_recv_buffer.prepare();
	m_status = INVALID;
}
tcp_client::impl::~impl()
{
	
}
ULONG_32 tcp_client::impl::get_status()
{
	return m_status;
}
void  tcp_client::impl::write(const CHAR* data, ULONG_32 ullength)
{
	m_send_buffer.append(data,ullength);
	write();
}
void tcp_client::impl::to_connect(tcp_type::endpoint ep)
{
	m_strand.post(boost::bind(&impl::do_connect, shared_from_this(),ep));  
}
void tcp_client::impl::do_connect(tcp_type::endpoint ep)
{
	this->setsocketopt();
	start_connect(ep);
}
void tcp_client::impl::re_connect()
{
	if (m_status == RUN)
	{
		this->to_close();
	}
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(m_ip),m_port);
	this->do_connect(ep);
}
tcp_client::impl::handle_type & tcp_client::impl::get_handle()
{
	return m_handle;
}
void tcp_client::impl::to_close()
{
	m_strand.post(boost::bind(&impl::do_close, shared_from_this()));  
}
void tcp_client::impl::do_close()
{
	this->close();
}
void tcp_client::impl::close()
{
	try
	{
		m_status = STOP;
		boost::system::error_code ignored_ec;
		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both,ignored_ec);
		m_socket.close(ignored_ec);
		if(m_handle && m_handle->handle_close)
		{
			m_handle->handle_close(m_client);
		}	
	}
	catch(std::exception& e)
	{
		
	}
}
void tcp_client::impl::start_connect(tcp_type::endpoint ep)
{
	m_socket.async_connect(ep,
		boost::bind(&impl::handle_connect,
		shared_from_this(),
		boost::asio::placeholders::error));
}
void tcp_client::impl::handle_connect(const boost::system::error_code& error)
{
	if(error)
	{
		this->to_close();
		return;
	}
	m_status = RUN;
	if(m_handle && m_handle->handle_open)
	{
		m_handle->handle_open(m_client);
	}
	read();
}
void tcp_client::impl::read()
{
	m_socket.async_read_some(m_recv_buffer.prepare(),
		boost::bind(&impl::handle_read,shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
}
void tcp_client::impl::handle_read(const boost::system::error_code& error,size_t bytes_transferred)
{
	if(error)
	{
		this->do_close();
		return;
	}
	if(m_handle && m_handle->handle_read)
	{
		m_handle->handle_read(m_client,bytes_transferred);
	}
	m_recv_buffer.consume(bytes_transferred);
	read();
}
void tcp_client::impl::handle_write(const boost::system::error_code& error,size_t bytes_transferred)
{
	if(error)
	{
		this->to_close();
		return;
	}
	if(m_handle && m_handle->handle_write)
	{
		m_handle->handle_write(m_client,bytes_transferred);
	}
	m_send_buffer.consume(bytes_transferred);
}
void tcp_client::impl::write()
{
	m_socket.async_write_some(m_send_buffer.data(),
		boost::bind(&impl::handle_read,shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}
void tcp_client::impl::setsocketopt()
{
	
}

tcp_client::impl::buffer_type & tcp_client::impl::read_buf()
{
	return m_recv_buffer;
}

tcp_client::impl::buffer_type & tcp_client::impl::send_buf()
{
	return m_send_buffer;
}

tcp_client::tcp_client(ios_type& ios,handle_type& handle)
{
	try
	{
		m_sptr = boost::make_shared<impl>(ios,handle);	
	}
	catch(std::exception& e)
	{
		std::cout << "create impl error ..." << std::endl;
	}
}
tcp_client::~tcp_client()
{
	
}
void tcp_client::send(IN const CHAR* data, IN size_t length)
{
	if(m_sptr)
	{
		m_sptr->write(data, length);
	}
}
void tcp_client::connect(tcp_type::endpoint ep)
{
	if(m_sptr)
	{
		m_sptr->to_connect(ep);	
	}
}
void tcp_client::reconnect()
{
	if(m_sptr)
	{
		m_sptr->re_connect();
	}
}
void tcp_client::close()
{
	if(m_sptr)
	{
		m_sptr->to_close();	
	}
}
ULONG_32 tcp_client::get_status()
{
	if(m_sptr)
	{
		return m_sptr->get_status();	
	}
	return INVALID;
}

SEED_ASIO_CLIENT::tcp_client::handle_type & SEED_ASIO_CLIENT::tcp_client::get_handle()
{
	return m_sptr->get_handle();
}

SEED_ASIO_CLIENT::tcp_client::buffer_type & SEED_ASIO_CLIENT::tcp_client::read_buf()
{
	return m_sptr->read_buf();
}

SEED_ASIO_CLIENT::tcp_client::buffer_type & SEED_ASIO_CLIENT::tcp_client::send_buf()
{
	return m_sptr->send_buf();
}
