#include "SockStack.h"

void trace(char *s)
{
	cout << s << endl;
}

CSockStack::~CSockStack()
{
}

/**
* 添加套接字-事件
* @param		sock		[in]		套接字描述
* @param		event		[in]		事件句柄
* @return		1		表示项目已达上限
* @return		0		表示操作成功
*/
int CSockStack::AddRecord(SOCKET sock, WSAEVENT event)
{
	if (WSA_MAXIMUM_WAIT_EVENTS == m_uindex)
	{
		// 栈已装满
		return 1;
	}

	m_socks[m_uindex] = sock;
	m_events[m_uindex] = event;
	InterlockedIncrement(&m_uindex);
	return 0;
}

/**
* 删除套接字-事件
* @param		index		[in]		要删除的项目的索引
*/
int CSockStack::DeleteRecord(unsigned index)
{
	if( index <0 || index > m_uindex - 1)
	{ 
		// 输入参数无效
		return 1;
	}

	for (unsigned i = index; i < m_uindex; ++i)
	{
		m_socks[i] = m_socks[i + 1];
		m_events[i] = m_events[i + 1];
	}

	m_socks[m_uindex] = 0;
	m_events[m_uindex] = 0;
	InterlockedDecrement(&m_uindex);

	return 0;
}
