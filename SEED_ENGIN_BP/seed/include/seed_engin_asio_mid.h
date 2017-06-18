#ifndef SEED_ENGIN_MID_H_
#define SEED_ENGIN_MID_H_
#include <seed_engin_com_stdafx.h>
#include <seed_engin_asio_server.h>
#include <seed_engin_asio_client.h>
using namespace  SEED_SERVER_ASIO;
using namespace  SEED_ASIO_CLIENT;
namespace SEED_NET_MID
{
	class DLL_API tcp_mid_server:public tcp_server_handler,
		public boost::enable_shared_from_this<tcp_mid_server>,
		boost::noncopyable
	{
	public:
		enum TYPE
		{
			MY_TYPE=0,
			OTHER_TYPE,
			TEST_TYPE,
			INVALID
		};
		tcp_mid_server();
		virtual ~tcp_mid_server();
		static void tcp_handle_accept(tcp_session_type session);
		static void tcp_handle_close(tcp_session_type session);
		static void tcp_handle_read(tcp_session_type session, std::size_t n);
		static void tcp_handle_write(tcp_session_type session, std::size_t n);
		/*MY_TYPE or OTHER_TYPE*/
		void start(ULONG_32 ulport, io_service_pool & m_ios_pool, TCP_SERVER_CONFIG config, ULONG_32 BufferType);
		void sendMessageToClient(const CHAR* data, ULONG_32 ulLength, tcp_session_type session);
		/*会话建立*/
		virtual ULONG_32 SessionOpen(tcp_session_type session);
		/*会话关闭*/
		virtual ULONG_32 SessionClose(tcp_session_type session);
		/*处理完整的协议*/
		virtual ULONG_32 ProtocolMsgProc(const CHAR* data, ULONG_32 ulLength, tcp_session_type session) = 0;
		/*从底层获取数据，并且拼装成一个完整的协议*/
		virtual ULONG_32 BufferProcMyType(const CHAR* data, ULONG_32 ulLength, tcp_session_type session) = 0;
		/*从底层获取数据，并且拼装成一个完整的协议*/
		virtual ULONG_32 BufferProcOtherType(const CHAR* data, ULONG_32 ulLength, tcp_session_type session) = 0;
		/*pingpong 测试*/
		virtual ULONG_32 BufferProcTestType(const CHAR* data, ULONG_32 ulLength, tcp_session_type session);
		/*pingpong 测试*/
		virtual ULONG_32 ProtocolMsgTestProc(const CHAR* data, ULONG_32 ulLength, tcp_session_type session);

	private:
		boost::shared_ptr<tcp_server> m_server;
		boost::atomic_uint32_t  m_type;
	};
	class DLL_API tcp_mid_client :public tcp_client_handler,
		public boost::enable_shared_from_this<tcp_mid_client>,
		boost::noncopyable
	{
	public:
		enum TYPE
		{
			MY_TYPE = 0,
			OTHER_TYPE,
			TEST_TYPE,
			INVALID
		};
		typedef boost::asio::io_service 	 ios_type;
		tcp_mid_client();
		virtual ~tcp_mid_client();
		void SendDataToServer(IN const CHAR* data, IN size_t length);
		void Connect(std::string ip,ULONG_32 ulPort, ios_type& ios, ULONG_32 type);
		void Reconnect();
		void Close();
		ULONG_32 GetStatus();
		static void tcp_handle_open(tcp_client_type client) ;
		static void tcp_handle_close(tcp_client_type client);
		static void tcp_handle_read(tcp_client_type client,std::size_t n);
		static void tcp_handle_write(tcp_client_type client,std::size_t n);
		/*会话建立*/
		virtual ULONG_32 LinkOpen(tcp_client_type client)=0;
		/*会话关闭*/
		virtual ULONG_32 LinkClose(tcp_client_type client)=0;
		/*处理完整的协议*/
		virtual ULONG_32 ProtocolMsgProc(const CHAR* data, ULONG_32 ulLength, tcp_client_type client) = 0;
		/*从底层获取数据，并且拼装成一个完整的协议*/
		virtual ULONG_32 BufferProcMyType(const CHAR* data, ULONG_32 ulLength, tcp_client_type client) = 0;
		/*从底层获取数据，并且拼装成一个完整的协议*/
		virtual ULONG_32 BufferProcOtherType(const CHAR* data, ULONG_32 ulLength, tcp_client_type client) = 0;

		virtual ULONG_32 BufferProcTestType(const CHAR* data, ULONG_32 ulLength, tcp_client_type client);
		/*pingpong 测试*/
		virtual ULONG_32 ProtocolMsgTestProc(const CHAR* data, ULONG_32 ulLength, tcp_client_type client);
	private:
		boost::shared_ptr<tcp_client> m_client;
		boost::atomic_uint32_t  m_type;
	};
}
#endif // !SEED_ENGIN_MID
