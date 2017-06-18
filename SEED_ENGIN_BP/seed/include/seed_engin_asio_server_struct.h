#ifndef _SEED_ENGIN_ASIO_CLIENT_S_H_
#define _SEED_ENGIN_ASIO_CLIENT_S_H_
#include<seed_engin_com_def.h>
typedef struct DLL_API session_config
{
	std::string  m_clientip;
	ULONG_32	 m_ulport;
	ULONG_32	 m_ulClientID;
	ULONG_32     m_ulGroupID;
}SESSION_CONFIG;


typedef struct DLL_API tcp_server_config
{}TCP_SERVER_CONFIG;
#endif