#include <seed_engin_net_buffer.h>
using namespace SEED_BUFFER;
/****
*@ prepare 申请内存，用于接收数据
*@ para1 size_type  n 申请内存大小
*@ return mutable_buffers_type 返回buffer
***/
mutable_buffers_type seed_tcp_buffer::prepare(IN size_type n /*= BUF_SIZE*/)
{return m_buf.prepare(n);}
/****
*@ prepare 申请内存，用于接收数据
*@ para1 size_type  n 申请内存大小
*@ return mutable_buffers_type 返回buffer
***/
void seed_tcp_buffer::retrieve(IN size_type n)
{m_buf.commit(n);}
/****
*@ size 返回可读大小
***/
size_type seed_tcp_buffer::size()
{return m_buf.size();}
/****
*@ peek 查看可读的字节
***/
const CHAR* seed_tcp_buffer::peek() const
{return boost::asio::buffer_cast<const char*>(m_buf.data());}
/****
*@ append 将数据放入内存
*@ para1 const void *  data 申请内存大小
*@ para1 size_type  	n 申请内存大小
***/
void seed_tcp_buffer::append(IN const void *data, IN size_type len)
{
	m_buf.sputn(static_cast<const CHAR*>(data),
		boost::numeric_cast<std::streamsize>(len));
}
/****
*@ data 返回buffer 用于发送数据
***/
const_buffers_type seed_tcp_buffer::data()const 
{return m_buf.data();}
/****
*@ consume 指示缓存区消费 n 个字节
***/
void seed_tcp_buffer::consume(IN size_type n)
{m_buf.consume(n);}