#include "UDPClient.h"
#include "TCPClient.h"

int main(int argc, char *argv[])
{
	CUDPClient udpclient;
	udpclient.startudpserver();

	/*CTCPClient tcpclient;
	tcpclient.starttcpserver();*/
	return 0;
}