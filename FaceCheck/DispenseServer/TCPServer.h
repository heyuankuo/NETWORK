#pragma once
#include "SockStack.h"
class CTCPServer
{
public:
	CTCPServer();
	~CTCPServer();

	int startudpserver();

public:
	static int SafeRecv(SOCKET sock, char *buffer, unsigned sum);

	static unsigned  __stdcall recv_client(void *accp);

	static void MangerClient(void *pclient);

};

