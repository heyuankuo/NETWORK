#pragma once
#include <iostream>
#include "winsock2.h"

using std::cout;
using std::endl;

#define RECV_BUFFER_SIZE		1024 * 216		// ���ջ���������
#define SEND_BUFFER_SIZE		1024 * 216		// ���ͻ���������

#pragma comment(lib, "Ws2_32.lib")

struct CAccepter
{
	SOCKET		*m_psock;				// �ͻ��׽���
	WSAEVENT	*m_pevent;				// �׽����¼�
	SOCKADDR_IN m_addr;			//	�ͻ�����ַ
	HANDLE		m_hthread_recv;		// ���վ��
	HANDLE		m_hthread_send;		// ���;��
	char		*m_buffer_recv;		// ���ջ�����
	char		*m_buffer_send;		// ���ͻ�����
	void		*msgparse;		// ���Ľ�����
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
	 * ����׽���-�¼�
	 * @param		sock		[in]		�׽�������
	 * @param		event		[in]		�¼����
	 */
	int AddRecord( SOCKET sock, WSAEVENT event);

	/**
	 * ɾ���׽���-�¼�
	 * @param		index		[in]		Ҫɾ������Ŀ������
	 */
	int DeleteRecord( unsigned index);

public:

	SOCKET		m_sock_server;
	SOCKET      m_socks[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT    m_events[WSA_MAXIMUM_WAIT_EVENTS];
	
	ULONG		m_uindex; // ջ��
};

