// Client side - UDP Code				    
// By Hugh Smith	4/1/2017		

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"

#include "application.h"
#include "libcpe464/networks/network-hooks.h" // this 

#define MAXBUF 2048

double errorRate = 0;


void talkToServer(int socketNum, struct sockaddr_in6 * server);
int readFromStdin(char * buffer);
int checkArgs(int argc, char * argv[]);

int main (int argc, char *argv[])
 {
	int socketNum = 0;				
	struct sockaddr_in6 server;		// Supports 4 and 6 but requires IPv6 struct
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);

	sendErr_init(errorRate, DROP_ON, FLIP_OFF, DEBUG_ON, RSEED_ON);

	socketNum = setupUdpClientToServer(&server, argv[2], portNumber);
	
	

	talkToServer(socketNum, &server);
	
	close(socketNum);

	return 0;
}

void talkToServer(int socketNum, struct sockaddr_in6 * server)
{
	int serverAddrLen = sizeof(struct sockaddr_in6);
	char * ipString = NULL;
	int dataLen = 0; 
	char buffer[MAXBUF];
	static uint8_t seq_num = 0;
	
	buffer[0] = '\0';
	while (buffer[0] != '.')
	{

		/*
		
		typedef struct{
			uint32_t segmentNum; // network order
			uint16_t checksum;
			uint8_t flag;
			int payloadSize;
			uint8_t payload[1400];
		}PDU_;

		*/

		PDU_ message = {};
		dataLen = readFromStdin(buffer);
		if(dataLen < 1400){
			seq_num+=1;
			int pduSize = createPDU(&message, seq_num, 1, buffer, dataLen);
			int sendYes = sendtoErr(socketNum, &message, pduSize, 0, (struct sockaddr *) server, serverAddrLen);	
			printPDU(&message, pduSize);
	
			int bytesRecv = recvfromErr(socketNum, buffer, MAXBUF, 0, (struct sockaddr *) server, &serverAddrLen);
	
			printf("Received:\n");
	
			PDU_* bleh = (PDU_*) buffer;
			(printf("\tMessage: %s\n", bleh->payload));

		} else{

			printf("Message too big, try again\n");

		}
	}
}

int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("Enter data: ");
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

int checkArgs(int argc, char * argv[])
{

    int portNumber = 0;
	
    /* check command line arguments  */
	
	/* rcopy .1 localhost 44444 */
	
	if (argc != 4)
	{
		printf("usage: %s error-rate host-name port-number \n", argv[0]);
		exit(1);
	}
	
	portNumber = atoi(argv[3]);
	errorRate = atof(argv[1]);


	return portNumber;
}





