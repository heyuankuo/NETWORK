#pragma once
#include "SockStack.h"
class CTCPClient
{
public:
	CTCPClient();
	~CTCPClient();

	int starttcpserver();

public:
	static int SafeSend(SOCKET sock, const char *buffer, unsigned len);

	static unsigned _stdcall send_proc(void *accp);
};

