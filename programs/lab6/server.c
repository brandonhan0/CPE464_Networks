/* Server side - UDP Code				    */
/* By Hugh Smith	4/1/2017	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"

#include "application.h"

#define MAXBUF 2048

void processClient(int socketNum);
int checkArgs(int argc, char *argv[]);

double errorRate = 0;

int main ( int argc, char *argv[]  )
{ 
	int socketNum = 0;				
	int portNumber = 0;

	portNumber = checkArgs(argc, argv);
		
	socketNum = udpServerSetup(portNumber);

	processClient(socketNum);

	close(socketNum);
	
	return 0;
}

void processClient(int socketNum)
{
	int dataLen = 0; 
	uint8_t buffer[MAXBUF];	  
	struct sockaddr_in6 client;		
	int clientAddrLen = sizeof(client);	
	
	buffer[0] = '\0';
	while (buffer[0] != '.')
	{
		dataLen = safeRecvfrom(socketNum, buffer, MAXBUF, 0, (struct sockaddr *) &client, &clientAddrLen);
		printPDU(buffer, dataLen);

		PDU_* bleh = (PDU_*) buffer;

		safeSendto(socketNum, buffer, MAXBUF, 0, (struct sockaddr *) & client, clientAddrLen);
	}
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number

	/* server .05 44444 */

	int portNumber = 0;
	
	if (argc > 3)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}

	if (argc >= 2) // 1 will be the error rate
	{
		errorRate = atof(argv[1]);
	}

	if (argc >= 3) // 2 will be the port number
	{
		portNumber = atoi(argv[2]);
	}
	
	return portNumber;
}


