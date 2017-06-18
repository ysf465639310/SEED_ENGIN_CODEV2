#include<seed_engin_asio_mid.h>
SEED_NET_MID::tcp_mid_server::tcp_mid_server()
{
	handle_accept = tcp_handle_accept;
	handle_close = tcp_handle_close;
	handle_read = tcp_handle_read;
	handle_write = tcp_handle_write;
	m_type = TEST_TYPE;
}

SEED_NET_MID::tcp_mid_server::~tcp_mid_server()
{
}

void SEED_NET_MID::tcp_mid_server::tcp_handle_accept(tcp_session_type session)
{
	boost::shared_ptr < tcp_server_handler > handle = session->get_handle();
	boost::shared_ptr <tcp_mid_server> server = boost::static_pointer_cast<tcp_mid_server>(handle);
	if (server)
	{
		server->SessionOpen(session);
	}
}

void SEED_NET_MID::tcp_mid_server::tcp_handle_close(tcp_session_type session)
{
	boost::shared_ptr < tcp_server_handler > handle = session->get_handle();
	boost::shared_ptr <tcp_mid_server> server = boost::static_pointer_cast<tcp_mid_server>(handle);
	if (server)
	{
		server->SessionClose(session);
	}
}

void SEED_NET_MID::tcp_mid_server::tcp_handle_read(tcp_session_type session, std::size_t n)
{
	const CHAR* data = session->read_buf().peek();
	boost::shared_ptr < tcp_server_handler > handle = session->get_handle();
	boost::shared_ptr <tcp_mid_server> server = boost::static_pointer_cast<tcp_mid_server>(handle);
	if (server && MY_TYPE == server->m_type)
	{
		server->BufferProcMyType(data,n,session);
	}
	else if (server && OTHER_TYPE == server->m_type)
	{
		server->BufferProcOtherType(data, n, session);
	}
	else if (server && TEST_TYPE == server->m_type)
	{
		server->BufferProcTestType(data, n, session);
	}
	return;
}
void SEED_NET_MID::tcp_mid_server::tcp_handle_write(tcp_session_type session, std::size_t n)
{
	//log
}

void SEED_NET_MID::tcp_mid_server::start(ULONG_32 ulport, io_service_pool & m_ios_pool, TCP_SERVER_CONFIG config,ULONG_32 BufferType)
{
	m_type = BufferType;
	boost::shared_ptr < tcp_server_handler > handle = boost::static_pointer_cast<tcp_mid_server>(shared_from_this());
	m_server = boost::make_shared<tcp_server>(ulport, m_ios_pool, config, handle);
	if (m_server)
	{
		m_server->start();
	}
}

void SEED_NET_MID::tcp_mid_server::sendMessageToClient(const CHAR * data, ULONG_32 ulLength, tcp_session_type session)
{
	session->write(data, ulLength);
}

ULONG_32 SEED_NET_MID::tcp_mid_server::SessionOpen(tcp_session_type session)
{
	return 0;
}

ULONG_32 SEED_NET_MID::tcp_mid_server::SessionClose(tcp_session_type session)
{
	return 0;
}

ULONG_32 SEED_NET_MID::tcp_mid_server::BufferProcTestType(const CHAR * data, ULONG_32 ulLength, tcp_session_type session)
{
	this->ProtocolMsgTestProc(data, ulLength,session);
	return 0;
}

ULONG_32 SEED_NET_MID::tcp_mid_server::ProtocolMsgTestProc(const CHAR * data, ULONG_32 ulLength, tcp_session_type session)
{
	std::cout << std::string(data, ulLength) << std::endl;
	session->write(data, ulLength);
	return 0;
}
/******************************华丽的分割线************************************/
SEED_NET_MID::tcp_mid_client::tcp_mid_client()
{
	handle_open  = tcp_handle_open;
	handle_close = tcp_handle_close;
	handle_read  = tcp_handle_read;
	handle_write = tcp_handle_write;
}

SEED_NET_MID::tcp_mid_client::~tcp_mid_client()
{
}

void SEED_NET_MID::tcp_mid_client::SendDataToServer(IN const CHAR * data, IN size_t length)
{
	if (m_client)
	{
		m_client->send(data,length);
	}
}

void SEED_NET_MID::tcp_mid_client::Connect(std::string ip, ULONG_32 ulPort, ios_type & ios,ULONG_32 type)
{
	m_type = type;
	boost::shared_ptr < tcp_client_handler > handle = boost::static_pointer_cast<tcp_mid_client>(shared_from_this());
	m_client = boost::make_shared<tcp_client>(ios, handle);
	if (m_client)
	{
		boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), ulPort);
		m_client->connect(ep);
	}
}

void SEED_NET_MID::tcp_mid_client::Reconnect()
{
	if (m_client)
	{
		m_client->reconnect();
	}
}

void SEED_NET_MID::tcp_mid_client::Close()
{
	if (m_client)
	{
		m_client->close();
	}
}

ULONG_32 SEED_NET_MID::tcp_mid_client::GetStatus()
{
	if (m_client)
	{
		m_client->get_status();
	}
	return STOP;
}

void SEED_NET_MID::tcp_mid_client::tcp_handle_open(tcp_client_type client)
{
	boost::shared_ptr < tcp_client_handler > handle = client->get_handle();
	boost::shared_ptr < tcp_mid_client > sptr_client = boost::static_pointer_cast<tcp_mid_client>(handle);
	if (sptr_client)
	{
		sptr_client->LinkOpen(client);
	}
}

void SEED_NET_MID::tcp_mid_client::tcp_handle_close(tcp_client_type client)
{
	boost::shared_ptr < tcp_client_handler > handle = client->get_handle();
	boost::shared_ptr < tcp_mid_client > sptr_client = boost::static_pointer_cast<tcp_mid_client>(handle);
	if (sptr_client)
	{
		sptr_client->LinkClose(client);
	}
}

void SEED_NET_MID::tcp_mid_client::tcp_handle_read(tcp_client_type client, std::size_t n)
{
	boost::shared_ptr < tcp_client_handler > handle = client->get_handle();
	boost::shared_ptr < tcp_mid_client > sptr_client = boost::static_pointer_cast<tcp_mid_client>(handle);
	const CHAR* data = client->read_buf().peek();
	if (sptr_client && MY_TYPE == sptr_client->m_type)
	{
		sptr_client->BufferProcMyType(data, n, client);
	}
	else if (sptr_client && OTHER_TYPE == sptr_client->m_type)
	{
		sptr_client->BufferProcOtherType(data, n, client);
	}
	else if (sptr_client && TEST_TYPE == sptr_client->m_type)
	{
		sptr_client->BufferProcTestType(data, n, client);
	}
}

void SEED_NET_MID::tcp_mid_client::tcp_handle_write(tcp_client_type client, std::size_t n)
{
	boost::shared_ptr < tcp_client_handler > handle = client->get_handle();
	boost::shared_ptr < tcp_mid_client > sptr_client = boost::static_pointer_cast<tcp_mid_client>(handle);
}
ULONG_32 SEED_NET_MID::tcp_mid_client::BufferProcTestType(const CHAR * data, ULONG_32 ulLength, tcp_client_type client)
{
	this->ProtocolMsgTestProc(data, ulLength, client);
	return 0;
}

ULONG_32 SEED_NET_MID::tcp_mid_client::ProtocolMsgTestProc(const CHAR * data, ULONG_32 ulLength, tcp_client_type client)
{
	std::cout << std::string(data, ulLength) << std::endl;
	client->send(data, ulLength);
	return 0;
}
