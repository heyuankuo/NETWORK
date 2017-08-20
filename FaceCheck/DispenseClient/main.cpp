#include "UDPClient.h"
#include "TCPClient.h"

int main(int argc, char *argv[])
{
	CUDPClient udpclient;
	udpclient.startudpserver();

	/*CTCPClient tcpclient;
	tcpclient.startudpserver();*/
	return 0;
}