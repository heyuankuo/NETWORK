#pragma once
#include <iostream>
#include "winsock2.h"

using std::cout;
using std::endl;

#define RECV_BUFFER_SIZE		1024 * 216		// 接收缓冲区长度
#define SEND_BUFFER_SIZE		1024 * 216		// 发送缓冲区长度

#pragma comment(lib, "Ws2_32.lib")

struct CAccepter
{
	SOCKET		*m_psock;				// 客户套接字
	WSAEVENT	*m_pevent;				// 套接字事件
	SOCKADDR_IN m_addr;			//	客户机地址
	HANDLE		m_hthread_recv;		// 接收句柄
	HANDLE		m_hthread_send;		// 发送句柄
	char		*m_buffer_recv;		// 接收缓冲区
	char		*m_buffer_send;		// 发送缓冲区
	void		*msgparse;		// 报文解析器
};

void trace(char *s);

class CSockStack
{
public:
	CSockStack() :m_uindex(0)
	{
		memset(&m_events, 0, sizeof m_events);
		memset(&m_socks, 0, sizeof m_socks);
	}
	~CSockStack();

public:
	/**
	 * 添加套接字-事件
	 * @param		sock		[in]		套接字描述
	 * @param		event		[in]		事件句柄
	 */
	int AddRecord( SOCKET sock, WSAEVENT event);

	/**
	 * 删除套接字-事件
	 * @param		index		[in]		要删除的项目的索引
	 */
	int DeleteRecord( unsigned index);

public:

	SOCKET		m_sock_server;
	SOCKET      m_socks[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT    m_events[WSA_MAXIMUM_WAIT_EVENTS];
	
	ULONG		m_uindex; // 栈顶
};

