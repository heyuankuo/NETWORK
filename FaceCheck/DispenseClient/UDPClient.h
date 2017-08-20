#pragma once
#include "SockStack.h"

class CUDPClient
{
public:
	CUDPClient();
	~CUDPClient();

	int startudpserver();

public:
	static int SafeRecv(SOCKET sock, char *buffer, unsigned sum);

	static unsigned  __stdcall recv_client(void *accp);

	static unsigned __stdcall MangerClient(void *pclient);

protected:

};

