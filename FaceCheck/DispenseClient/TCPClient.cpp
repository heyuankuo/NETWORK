#include "TCPClient.h"
#include <process.h>

CTCPClient::CTCPClient()
{
}

CTCPClient::~CTCPClient()
{
}

/**
* ������Ϣ
* �˺���ȷ��������Ϣ�������
*/
int CTCPClient::SafeSend(SOCKET sock, const char *buffer, unsigned len)
{
	unsigned tmplen = 0;
	while (tmplen < len)
	{
		int err = 0;
		err = send(sock, buffer + tmplen, len - tmplen, 0);
		if (SOCKET_ERROR == err)
		{
			if (10035 == err)
			{
				Sleep(10);
				continue;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			tmplen += err;
		}
	}

	return 0;
}

/**
* �ͻ��˷��ͳ���
*/
unsigned _stdcall CTCPClient::send_proc(void *accp)
{
	CAccepter *paccp = (CAccepter *)accp;
	Sleep(2000);
	while (1)
	{
		int nIndex = ::WSAWaitForMultipleEvents(1, paccp->m_pevent, TRUE, WSA_INFINITE, FALSE);
		if (nIndex == WSA_WAIT_FAILED)
		{
			// �쳣��Ϣ����
			trace("�ͻ������쳣");
		}
		// ��ȡ�¼�
		WSANETWORKEVENTS event = { 0 };
		::WSAEnumNetworkEvents(*(paccp->m_psock), *(paccp->m_pevent), &event);

		if (event.lNetworkEvents & FD_WRITE)		// �� �¼�
		{
			if (event.iErrorCode[FD_WRITE_BIT] == 0)
			{
				char *buffer = "hello world";
				// ���ͱ���
				int a = SafeSend(*(paccp->m_psock), buffer, strlen(buffer + 1));
				cout << "���Ϊ��" << a << endl;
			}
		}
	}

}

/***************************************�ⲿ���ýӿ�**************************************/

/**
* ����UDP����
*/
int CTCPClient::startudpserver()
{
	// �׽��ֳ�ʼ��
	WSADATA			wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		trace("WSAStartup failed!");
		return 1;
	}

	// �����׽���
	SOCKET sock_server = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sock_server)
	{
		trace("socket failed!");
		WSACleanup();//�ͷ��׽�����Դ;  
		return  -1;
	}

	// ָ��������ʽ
	WSAEVENT event_server = ::WSACreateEvent();
	int err = WSAEventSelect(sock_server, event_server, FD_READ | FD_WRITE);
	if (SOCKET_ERROR == err)
	{
		trace("�첽����ʧ��");
		closesocket(sock_server);
		WSACleanup();
		return -1;
	}

	// �������׽��ֵ�ַ����
	SOCKADDR_IN		addr_server;
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(6000);

	// ��������
	char* ip = NULL;
	hostent *phost = gethostbyname("heyuankuo.vicp.cc");
	in_addr addr;
	for (int i = 0;; i++)
	{
		char *p = phost->h_addr_list[i];
		if (p == NULL)
			break;
		memcpy(&addr.S_un.S_addr, p, phost->h_length);
		ip = inet_ntoa(addr);
	}
	addr_server.sin_addr.s_addr = inet_addr("60.205.215.87");

	CAccepter accp;
	accp.m_psock = &sock_server;
	accp.m_pevent = &event_server;

	err = connect(sock_server, (sockaddr *)&addr_server, sizeof addr_server);
	if (SOCKET_ERROR == err)
	{
		err = GetLastError();
		if (10035 == err)
		{
		}
		else
		{
			trace("����ʧ��");
		}
	}

	HANDLE h_sendThread = (HANDLE)_beginthreadex(NULL, NULL, send_proc, (void *)&accp, 0, NULL);

	::WaitForSingleObject(h_sendThread, INFINITE);
	CloseHandle(h_sendThread);
	return 0;
}
