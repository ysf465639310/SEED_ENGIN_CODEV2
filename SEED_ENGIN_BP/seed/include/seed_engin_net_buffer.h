#ifndef SEED_ENGIN_NET_BUFFER
#define SEED_ENGIN_NET_BUFFER
#include <seed_engin_com_stdafx.h>
namespace SEED_BUFFER
{
	typedef std::size_t 	size_type;
	typedef boost::asio::streambuf streambuf_type;
	typedef streambuf_type::const_buffers_type const_buffers_type;
	typedef streambuf_type::mutable_buffers_type mutable_buffers_type;
	class DLL_API seed_tcp_buffer
	{
	private:
		BOOST_STATIC_CONSTANT(size_type,BUF_SIZE= 4096);
		streambuf_type m_buf;
	public:
		/****
		*@ prepare 申请内存，用于接收数据
		*@ para1 size_type  n 申请内存大小
		*@ return mutable_buffers_type 返回buffer
		***/
		mutable_buffers_type prepare(IN size_type n = BUF_SIZE);
		/****
		*@ prepare 申请内存，用于接收数据
		*@ para1 size_type  n 申请内存大小
		*@ return mutable_buffers_type 返回buffer
		***/
		void retrieve(IN size_type n);
		size_type size();
		/****
		*@ size 返回可读大小
		***/
		size_type size()const;
		/****
		*@ peek 查看可读的字节
		***/
		const CHAR* peek()const;
		/****
		*@ append 将数据放入内存
		*@ para1 const void *  data 申请内存大小
		*@ para1 size_type  	n 申请内存大小
		***/
		void append(IN const void *data,IN size_type len);
		/****
		*@ data 返回buffer 用于发送数据
		***/
		const_buffers_type data()const;
		/****
		*@ consume 指示缓存区消费 n 个字节
		***/
		void consume(IN size_type n);
	};
}
#endif