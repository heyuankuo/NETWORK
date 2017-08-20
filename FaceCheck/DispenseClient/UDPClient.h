#pragma once
#include "SockStack.h"

class CUDPClient
{
public:
	CUDPClient();
	~CUDPClient();

	int startudpserver();

public:
	static int SafeSend(SOCKET sock, const char *buffer, unsigned len);

	static unsigned  __stdcall send_proc(void *accp);

protected:

};

