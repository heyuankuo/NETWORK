#include "UDPServer.h"
#include "TCPServer.h"

int main(int argc, char *argv[])
{
	/*CUDPServer udpserver;
	udpserver.startudpserver();*/

	CTCPServer tcpserver;
	tcpserver.startudpserver();
	return 0;
}