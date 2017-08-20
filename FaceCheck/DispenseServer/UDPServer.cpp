#include "UDPServer.h"
#include <process.h>

CUDPServer::CUDPServer()
{
}


CUDPServer::~CUDPServer()
{
}

/**
* ���տ���
* �������գ�����մ��
* @param			sock			[in]		�׽���������
* @param			buffer			[in]		���ջ�����
* @param			sum				[in]		ÿ�ν��յĳ���
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
*	�ͻ����������շ�
*/
unsigned  __stdcall CUDPServer::recv_client(void *accp)
{
	CAccepter *paccp = (CAccepter *)accp;

	Sleep(2000);
	while (1)
	{
		// �ȴ��ͻ�����
		int nIndex = ::WSAWaitForMultipleEvents(1, paccp->m_pevent, TRUE, WSA_INFINITE, FALSE);
		if (nIndex == WSA_WAIT_FAILED)
		{
			// �쳣��Ϣ����
			trace("�ͻ������쳣");
		}

		// ��ȡ�¼�
		WSANETWORKEVENTS event = { 0 };
		::WSAEnumNetworkEvents(*(paccp->m_psock), *(paccp->m_pevent), &event);

		if (event.lNetworkEvents & FD_READ)		// �� �¼�
		{
			if (event.iErrorCode[FD_READ_BIT] == 0)
			{
				memset(paccp->m_buffer_recv, 0, RECV_BUFFER_SIZE);
				SafeRecv(*(paccp->m_psock), paccp->m_buffer_recv, 1024);

				// ����
				trace(paccp->m_buffer_recv);
			}
			else
			{
				trace("�����쳣");
			}
		}
		else if (event.lNetworkEvents & FD_CLOSE)		// �ر� �¼�
		{
			if (0 == event.iErrorCode[FD_CLOSE_BIT] ||
				10053 == event.iErrorCode[FD_CLOSE_BIT])
			{
				trace("�ͻ��˶Ͽ�����");
				CoUninitialize();
				return 0;
			}
		}
		continue;
	}
	return 0;
}

/**
* �ͻ��������ù���
*/
unsigned _stdcall CUDPServer::MangerClient(void *pclient)
{
	CSockStack *client = (CSockStack *)pclient;

	// ���ù�����ʽ
	WSAEVENT event_server = ::WSACreateEvent();
	int err = WSAEventSelect(client->m_sock_server, event_server, FD_READ | FD_CLOSE);
	if (SOCKET_ERROR == err)
	{
		err = ::WSAGetLastError();
		trace("�첽����ʧ��");
		closesocket(client->m_sock_server);
		WSACleanup();
		return -1;
	}

	// ���Ӷ������ڽ����ӵĸ��ֲ������ݵ���дʵ�ֹ���
	CAccepter accp;
	accp.m_psock = &client->m_sock_server;
	accp.m_pevent = &event_server;
	accp.m_buffer_recv = new char[RECV_BUFFER_SIZE]();
	accp.m_buffer_send = new char[SEND_BUFFER_SIZE]();

	// ������д�߳�
	HANDLE h_recv = (HANDLE)_beginthreadex(NULL, NULL, recv_client, (void *)&accp, NULL, NULL);

	::WaitForMultipleObjects(1, &h_recv, TRUE, INFINITE);
	CloseHandle(h_recv);

	// �����жϣ�������Դ
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

/***************************************�ⲿ���ýӿ�**************************************/

/**
 * ����UDP����
 */
int CUDPServer::startudpserver()
{
	// �׽��ֳ�ʼ��
	WSADATA			wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		trace("WSAStartup failed!");
		return 1;
	}

	// �����׽���
	SOCKET sock_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sock_server)
	{
		trace("socket failed!");
		WSACleanup();//�ͷ��׽�����Դ;  
		return  -1;
	}

	// �������׽��ֵ�ַ����
	SOCKADDR_IN		addr_server;
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(8080);
	addr_server.sin_addr.s_addr = INADDR_ANY;

	// ���׽���
	int err = bind(sock_server, (LPSOCKADDR)&addr_server, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == err)
	{
		trace("bind failed!");
		closesocket(sock_server);
		WSACleanup();
		return -1;
	}

	CSockStack clients; // �����׽��ֹ���ջ
	clients.m_sock_server = sock_server;

	HANDLE h_recv = (HANDLE)_beginthreadex(NULL, NULL, MangerClient, (void *)&clients, NULL, NULL);

	::WaitForMultipleObjects(1, &h_recv, TRUE, INFINITE);
	CloseHandle(h_recv);

	return 0;
}
