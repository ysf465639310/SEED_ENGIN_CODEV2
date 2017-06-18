#include<seed_engin_asio_server.h>
#include<seed_engin_com_stdafx.h>
#include<seed_engin_net_struct.h>
using namespace SEED_SERVER_ASIO;
using namespace SEED_ASIO;


struct DLL_API tcp_group
{
	std::map<ULONG_32, boost::shared_ptr<tcp_session> > m_clients;
};

class tcp_session::Impl :boost::noncopyable,
	public boost::enable_shared_from_this<tcp_session::Impl>
{
public:
	typedef seed_tcp_buffer               buffer_type;
	typedef boost::asio::io_service       ios_type;
	typedef boost::asio::ip::tcp::socket  socket_type;
	typedef ios_type::strand		   strand_type;
	typedef boost::shared_ptr<tcp_server_handler> handle_type;
	Impl(ios_type& ios, SESSION_CONFIG config, handle_type& handle);
	virtual ~Impl();
	void do_close();
	void to_close();
	void start();
	void write(const void * data, std::size_t len);
	void set_session(boost::shared_ptr<tcp_session> session);
	ios_type&    io_service();
	socket_type& socket();
	buffer_type& read_buf();
	buffer_type& send_buf();
	SESSION_CONFIG get_config();
	handle_type& get_handle();
	ULONG_32 get_stauts();
	void set_status(ULONG_32 type);
	void write();
private:
	void close();
	void read();
	void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error, std::size_t bytes_transferred);
	void set_ip_and_port(std::string ip, ULONG_32 ulport);

private:
	ios_type&    m_io;
	handle_type  m_handle;
	socket_type  m_socket;
	buffer_type  m_read_buf;
	buffer_type  m_write_buf;
	strand_type  m_strand;
	SESSION_CONFIG  m_config;
	boost::shared_ptr<tcp_session> m_session;
	boost::shared_ptr<tcp_group>   m_group;
	boost::atomic_int32_t m_status;
};
tcp_session::Impl::Impl(ios_type & ios, SESSION_CONFIG config, 
	handle_type & handle):m_socket(ios),
	m_config(config),m_handle(handle), m_io(ios), m_strand(ios)
{

}
tcp_session::Impl::~Impl()
{

}
void tcp_session::Impl::set_session(boost::shared_ptr<tcp_session> session)
{
	m_session = session;
}

SESSION_CONFIG tcp_session::Impl::get_config()
{
	return m_config;
}
tcp_session::Impl::handle_type & tcp_session::Impl::get_handle()
{
	return m_handle;
}
ULONG_32 tcp_session::Impl::get_stauts()
{
	return m_status;
}
void tcp_session::Impl::set_status(ULONG_32 type)
{
	m_status = type;
}
void tcp_session::Impl::set_ip_and_port(std::string ip, ULONG_32 ulport)
{
	m_config.m_clientip = ip;
	m_config.m_ulport = ulport;
}
void tcp_session::Impl::do_close()
{
	this->close();
}
void tcp_session::Impl::to_close()
{
	m_strand.post(boost::bind(&Impl::do_close, shared_from_this()));
}
void tcp_session::Impl::start()
{
	read();
}
tcp_session::Impl::ios_type & tcp_session::Impl::io_service()
{
	return  m_io;
}

tcp_session::Impl::socket_type& tcp_session::Impl::socket()
{
	return m_socket;
}
tcp_session::Impl::buffer_type& tcp_session::Impl::read_buf()
{
	return m_read_buf;
}
tcp_session::Impl::buffer_type& tcp_session::Impl::send_buf()
{
	return m_write_buf;
}
void tcp_session::Impl::close()
{
	boost::system::error_code ignored_ec;
	m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	m_socket.close(ignored_ec);
	if (m_handle && m_handle->handle_close)
	{
		m_handle->handle_close(m_session);
	}
	if (m_group)
	{
		std::map<ULONG_32, boost::shared_ptr<tcp_session> >::iterator it = m_group->m_clients.find(m_config.m_ulClientID);
		if (it != m_group->m_clients.end())
		{
			m_group->m_clients.erase(m_config.m_ulClientID);
		}
	}
}
void tcp_session::Impl::read()
{
	m_socket.async_read_some(m_read_buf.prepare(),boost::bind(&tcp_session::Impl::handle_read,shared_from_this(),
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}
void tcp_session::Impl::handle_read(const boost::system::error_code & error, std::size_t bytes_transferred)
{
	if (error)
	{
		this->to_close();
		return;
	}
	if (m_handle && m_handle->handle_read)
	{
		m_handle->handle_read(m_session, bytes_transferred);
	}
	m_read_buf.retrieve(bytes_transferred);
	m_read_buf.consume(bytes_transferred);
	read();
}
void  tcp_session::Impl::write()
{
	m_socket.async_read_some(m_read_buf.prepare(), boost::bind(&tcp_session::Impl::handle_write, shared_from_this(),
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}
void tcp_session::Impl::handle_write(const boost::system::error_code & error, std::size_t bytes_transferred)
{
	if (error)
	{
		this->to_close();
		return;
	}
	if (m_handle && m_handle->handle_write)
	{
		m_handle->handle_write(m_session, bytes_transferred);
	}
	m_write_buf.consume(bytes_transferred);
}
void tcp_session::Impl::write(const void * data, std::size_t len)
{
	m_write_buf.append(data, len);
	this->write();
}
/******************************华丽的分割线************************************/
tcp_session::tcp_session(ios_type & ios, SESSION_CONFIG config, handle_type & handle)
{
	m_impl = boost::make_shared<tcp_session::Impl>(ios, config, handle);
}
boost::shared_ptr<tcp_session> SEED_SERVER_ASIO::tcp_session::get_session_ptr()
{
	return shared_from_this();
}
void tcp_session::close()
{
	if (m_impl)
	{
		m_impl->do_close();
	}
}
void tcp_session::start()
{
	if (m_impl)
	{
		m_impl->set_session(shared_from_this());
		m_impl->start();
	}
}
void tcp_session::write(const void * data, std::size_t len)
{
	if (m_impl)
	{
		m_impl->write(data,len);
	}
}
tcp_session::ios_type & tcp_session::io_service()
{
	return m_impl->io_service();
}
tcp_session::socket_type & tcp_session::socket()
{
	return m_impl->socket();
}
tcp_session::buffer_type & tcp_session::read_buf()
{
	return m_impl->read_buf();
}
tcp_session::buffer_type & tcp_session::send_buf()
{
	return m_impl->send_buf();
}
tcp_session::handle_type & tcp_session::get_handle()
{
	return m_impl->get_handle();
}
ULONG_32 SEED_SERVER_ASIO::tcp_session::get_stauts()
{
	return ULONG_32();
}
/******************************华丽的分割线************************************/
class tcp_server::Impl:boost::noncopyable,
	public boost::enable_shared_from_this<tcp_server::Impl>
{
public:
	typedef io_service_pool::ios_type  ios_type;
	typedef ios_type::strand		   strand_type;
	typedef boost::asio::ip::tcp::acceptor acceptor_type;
	typedef boost::shared_ptr<tcp_server_handler> handle_type;
	typedef boost::shared_ptr<tcp_session> tcp_session_ptr;
	Impl(ULONG_32 port, io_service_pool & ios_pool, TCP_SERVER_CONFIG config, handle_type & handle);
	virtual~Impl();
	void start();
	void to_accept();
	void do_acccept();
	ULONG_32 get_socekt_status();
private:
	void start_accept();
	void handle_accept(const boost::system::error_code & error, tcp_session_type session, ULONG_32 GroupID);
	void set_socket_opt();
private:
	ios_type&        m_io;
	io_service_pool& m_ios_pool;
	acceptor_type   m_acceptor;
	ULONG_32 m_ulport;
	ULONG_32 num;
	boost::atomic_uint32_t  m_status;
	boost::shared_ptr<tcp_server_handler> m_handle;
	strand_type m_strand;
	TCP_SERVER_CONFIG m_config;
	std::map<ULONG_32, boost::shared_ptr<tcp_group> > m_groups;
	boost::atomic_uint32_t  m_clientid_adapters;
};

tcp_server::Impl::Impl(ULONG_32 port, io_service_pool& ios_pool, TCP_SERVER_CONFIG config, handle_type& handle):m_ios_pool(ios_pool), m_io(m_ios_pool.get()),
		m_acceptor(m_io,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port)),m_handle(handle), m_strand(m_io), m_config(config)
{
	m_ulport = port;
	num = num;
	m_clientid_adapters = 1;
	for (size_t i = 1; i <= ios_pool.size(); i++)
	{
		m_groups[i] = boost::make_shared<tcp_group>();
	}
}
tcp_server::Impl::~Impl()
{

}
void tcp_server::Impl::start()
{
	this->set_socket_opt();
	this->to_accept();
}

void tcp_server::Impl::to_accept()
{
	m_strand.post(boost::bind(&Impl::do_acccept, shared_from_this()));
}

void tcp_server::Impl::do_acccept()
{
	this->start_accept();
}

ULONG_32 tcp_server::Impl::get_socekt_status()
{
	return m_status;
}

void tcp_server::Impl::start_accept()
{

	SESSION_CONFIG config;
	config.m_ulClientID = m_clientid_adapters++;
	tcp_session_ptr session = boost::factory<tcp_session_ptr>()(m_ios_pool.get(config.m_ulGroupID), config, m_handle);
	std::map<ULONG_32, boost::shared_ptr<tcp_group> >::iterator it = m_groups.find(config.m_ulGroupID);
	if (it != m_groups.end())
	{
		boost::shared_ptr<tcp_group> group = it->second;
		if (!group)
		{
			group= boost::make_shared<tcp_group>();
			it->second = group;
		}
		group->m_clients[config.m_ulClientID] = session;
	}
	m_acceptor.async_accept(session->socket(),
		boost::bind(&tcp_server::Impl::handle_accept, shared_from_this(), boost::asio::placeholders::error, session, config.m_ulGroupID));
}

void tcp_server::Impl::handle_accept(const boost::system::error_code & error, tcp_session_type session,ULONG_32 GroupID)
{
	start_accept();
	if (error)
	{
		session->close();
	}
	if (m_handle && m_handle->handle_accept)
	{
		m_handle->handle_accept(session);
	}
	session->start();
}

void tcp_server::Impl::set_socket_opt()
{

}

tcp_server::tcp_server(ULONG_32 port, io_service_pool & m_ios_pool, TCP_SERVER_CONFIG config, handle_type & handle)
{
	m_impl = boost::make_shared<tcp_server::Impl>(port, m_ios_pool, config, handle);
}

void tcp_server::start()
{
	if (m_impl)
	{
		m_impl->start();
	}
}

