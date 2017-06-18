#ifndef SEED_ENGIN_NET_STRUCT_H
#define SEED_ENGIN_NET_STRUCT_H
#include<seed_engin_com_stdafx.h>
namespace SEED_ASIO
{
	enum SOCKET_STATUS
	{
		RUN,
		STOP,
		INIT,
		INVALID
	};
	class io_service_pool: boost::noncopyable
	{
	public:
		typedef boost::asio::io_service 		ios_type;
		typedef boost::asio::io_service::work   work_type;
		typedef boost::ptr_vector<ios_type>     io_services_type;
		typedef boost::ptr_vector<work_type>    works_type;
	private:
		io_services_type 		m_io_services;
		works_type		 		m_works;
		boost::thread_group		m_threads;
		std::size_t 			m_next_io_service;
	public:
		explicit io_service_pool(int n =4):m_next_io_service(0)
		{
			BOOST_ASSERT(n>0);
			init(n);
		}
		ios_type& get()
		{
			if(m_next_io_service >= m_io_services.size())
			{
				m_next_io_service=0;
			}
			return m_io_services[m_next_io_service++];
		}
		ios_type& get(ULONG_32 & GruopID)
		{
			if (m_next_io_service >= m_io_services.size())
			{
				m_next_io_service = 0;
			}
			GruopID = m_next_io_service++;
			return m_io_services[GruopID];
		}
		void start()
		{
			if(m_threads.size() > 0)
			{return;}
			for(ULONG_32 i=0;i <m_io_services.size();i++)
			{
				ios_type & ios=m_io_services[i];
				m_threads.create_thread(boost::bind(&ios_type::run,boost::ref(ios)));
			}
		}
		void run()
		{
			start();
			m_threads.join_all();
		}
		void stop()
		{
			m_works.clear();
			std::for_each(m_io_services.begin(),m_io_services.end(),
				boost::bind(&ios_type::stop,_1));
		}
		void join()
		{
			m_threads.join_all();
		}
		std::size_t size() const
		{
			return m_io_services.size();
		}
	private:
		void init(int n)
		{
			for(int i =0; i < n; i++)
			{
				m_io_services.push_back(boost::factory<ios_type*>()());
				m_works.push_back(
					boost::factory<work_type*>()
					(m_io_services.back()));
			}
		}
	};
}
#endif