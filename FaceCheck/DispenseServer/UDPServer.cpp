#include "UDPServer.h"
#include <process.h>

CUDPServer::CUDPServer()
{
}


CUDPServer::~CUDPServer()
{
}

/**
* 接收控制
* 定长接收，避免沾包
* @param			sock			[in]		套接字描述符
* @param			buffer			[in]		接收缓冲区
* @param			sum				[in]		每次接收的长度
*/
int CUDPServer::SafeRecv(SOCKET sock, char *buffer, unsigned sum)
{
	int recvsum = 0;
	while (1)
	{
		int err = recv(sock, buffer + recvsum, sum, 0);
		if (SOCKET_ERROR == err)
		{
			err = GetLastError();
			if (10035 == err)
			{
				return 0;
			}
			else
			{
				return -1;
			}
		}
		else if (err > 0)
		{
			// trace(buffer + recvsum);
			recvsum += err;
			continue;
		}
	}
}

/**
*	客户连接数据收发
*/
unsigned  __stdcall CUDPServer::recv_client(void *accp)
{
	CAccepter *paccp = (CAccepter *)accp;

	Sleep(2000);
	while (1)
	{
		// 等待客户数据
		int nIndex = ::WSAWaitForMultipleEvents(1, paccp->m_pevent, TRUE, WSA_INFINITE, FALSE);
		if (nIndex == WSA_WAIT_FAILED)
		{
			// 异常消息处理
			trace("客户连接异常");
		}

		// 提取事件
		WSANETWORKEVENTS event = { 0 };
		::WSAEnumNetworkEvents(*(paccp->m_psock), *(paccp->m_pevent), &event);

		if (event.lNetworkEvents & FD_READ)		// 读 事件
		{
			if (event.iErrorCode[FD_READ_BIT] == 0)
			{
				memset(paccp->m_buffer_recv, 0, RECV_BUFFER_SIZE);
				SafeRecv(*(paccp->m_psock), paccp->m_buffer_recv, 1024);

				// 解析
				trace(paccp->m_buffer_recv);
			}
			else
			{
				trace("接受异常");
			}
		}
		else if (event.lNetworkEvents & FD_CLOSE)		// 关闭 事件
		{
			if (0 == event.iErrorCode[FD_CLOSE_BIT] ||
				10053 == event.iErrorCode[FD_CLOSE_BIT])
			{
				trace("客户端断开连接");
				CoUninitialize();
				return 0;
			}
		}
		continue;
	}
	return 0;
}

/**
* 客户连接配置管理
*/
unsigned _stdcall CUDPServer::MangerClient(void *pclient)
{
	CSockStack *client = (CSockStack *)pclient;

	// 配置工作方式
	WSAEVENT event_server = ::WSACreateEvent();
	int err = WSAEventSelect(client->m_sock_server, event_server, FD_READ | FD_CLOSE);
	if (SOCKET_ERROR == err)
	{
		err = ::WSAGetLastError();
		trace("异步配置失败");
		closesocket(client->m_sock_server);
		WSACleanup();
		return -1;
	}

	// 连接对象，用于将连接的各种参数传递到读写实现过程
	CAccepter accp;
	accp.m_psock = &client->m_sock_server;
	accp.m_pevent = &event_server;
	accp.m_buffer_recv = new char[RECV_BUFFER_SIZE]();
	accp.m_buffer_send = new char[SEND_BUFFER_SIZE]();

	// 启动读写线程
	HANDLE h_recv = (HANDLE)_beginthreadex(NULL, NULL, recv_client, (void *)&accp, NULL, NULL);

	::WaitForMultipleObjects(1, &h_recv, TRUE, INFINITE);
	CloseHandle(h_recv);

	// 连接中断，回收资源
	closesocket(*(client->m_socks));

	if (accp.m_buffer_recv)
	{
		delete[] accp.m_buffer_recv;
		accp.m_buffer_recv = NULL;
	}

	if (accp.m_buffer_send)
	{
		delete[] accp.m_buffer_send;
		accp.m_buffer_send = NULL;
	}

	return 0;
}

/***************************************外部调用接口**************************************/

/**
 * 启动UDP服务
 */
int CUDPServer::startudpserver()
{
	// 套接字初始化
	WSADATA			wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		trace("WSAStartup failed!");
		return 1;
	}

	// 创建套接字
	SOCKET sock_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sock_server)
	{
		trace("socket failed!");
		WSACleanup();//释放套接字资源;  
		return  -1;
	}

	// 服务器套接字地址配置
	SOCKADDR_IN		addr_server;
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(8080);
	addr_server.sin_addr.s_addr = INADDR_ANY;

	// 绑定套接字
	int err = bind(sock_server, (LPSOCKADDR)&addr_server, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == err)
	{
		trace("bind failed!");
		closesocket(sock_server);
		WSACleanup();
		return -1;
	}

	CSockStack clients; // 创建套接字管理栈
	clients.m_sock_server = sock_server;

	HANDLE h_recv = (HANDLE)_beginthreadex(NULL, NULL, MangerClient, (void *)&clients, NULL, NULL);

	::WaitForMultipleObjects(1, &h_recv, TRUE, INFINITE);
	CloseHandle(h_recv);

	return 0;
}
