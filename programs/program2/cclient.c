/******************************************************************************
* myClient.c
*
* Writen by Prof. Smith, updated Jan 2023
* Use at your own risk.  
*
*****************************************************************************/

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
#include <stdint.h>

#include "networks.h"
#include "safeUtil.h"
#include "new.h"
#include "pollLib.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

typedef enum{
	MESSAGE,
	MULTICAST,
	BROADCAST,
	HANDLELIST
}commands;

void sendToServer(int socketNum);
int readFromStdin(uint8_t * buffer);
void checkArgs(int argc, char * argv[]);

int main(int argc, char * argv[]) // ./cclient handle server-name server-port i think server name would be like unix1-5 but we can ask like during class tmr
{
	int socketNum = 0;         //socket descriptor
	int cont_flag = 0;
	checkArgs(argc, argv);
	setupPollSet();
	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[1], argv[2], DEBUG_FLAG); // now its cclient handle server-name server-port, [1], [2] was local host, server port number

	addToPollSet(socketNum);
	addToPollSet(STDIN_FILENO);

	while(cont_flag == 0){
		clientControl(socketNum);
	}
	
	close(socketNum);
	
	return 0;
}

void sendToServer(int socketNum)
{
	uint8_t buffer[MAXBUF];   //data buffer
	int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */
	int recvBytes = 0;
	uint8_t flag = 0;
	
	sendLen = readFromStdin(buffer);
	printf("read: %s string len: %d (including null)\n", buffer, sendLen);
	
	sent =  sendPDU(socketNum, buffer, sendLen, 0);

	if (sent < 0)
	{

		perror("send err");
		exit(-1);
	}

	printf("Socket:%d: Sent, Length: %d msg: %s\n", socketNum, sent, buffer);
	
	// just for debugging, recv a message from the server to prove it works.


	//recvBytes = safeRecv(socketNum, buffer, MAXBUF, 0);
	recvBytes = recvPDU(socketNum, buffer, MAXBUF, 0);
	if(recvBytes == 0) {perror("Server has terminated"); close(socketNum); exit(-1);}
	printf("Socket %d: Byte recv: %d message: %s\n", socketNum, recvBytes, buffer);
	
}

int readFromStdin(uint8_t * textbuffer)
{
	char aChar = 0;
	int inputLen = 0;        
	char prevChar = 0;
	commands command;

	/*
	
	4 commands:

	%m or %M: %M handle1 Hello how are you (sends message to a handle) split it up like have a handle buffer <100 bytes

	%c or %C: %C num-handles destination-handle destination-handle [destination-handle] [text]

			- num-handles must be between 2 and 9

	%l or %L: 

	%b or %B: %B This is a broadcast message (this is probsbly the easiest one we can start with this tbh)
	
	*/


	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("$: ");
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			if(prevChar == '%'){ // tells us the command we wanna go with
				switch(aChar){
					case 'M': // literally here just to catch upper case im not doin a bunch of if statements brah
					case 'm':
						command = MESSAGE;
						break;
					case 'C':
					case 'c':
						command = MULTICAST;
						break;
					case 'b':
					case 'B':
						command = BROADCAST;
						break;
					case 'l':
					case 'L':
						command = HANDLELIST;
						break;
					default:
						command = 0;
				}	
			}

			if(command != 0){
				switch(command){
					case MESSAGE:
						Mpacket* packetOut = {};
						scanf("%s")
						break;
					case MULTICAST:
						break;
					case BROADCAST:
						break;
					case HANDLELIST:
						break;
				}
			}


			buffer[inputLen] = aChar;
			prevChar = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 3)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}

void clientControl(int socketNum){ // still need to do stuff with flag idk
	uint8_t buffer[MAXBUF];   //data buffer
	uint8_t flag;

	int fd = pollCall(0);
	if(fd == socketNum){
		// do socket
		processMsgFromServer(socketNum, buffer);
	} else{
		// stdin
		processStdin(socketNum, buffer, &flag);
	}
}

void processStdin(int socketNum, uint8_t* buffer, uint8_t* flag){
	int sendLen = 0;        //amount of data to send
	int sent = 0;
	sendLen = readFromStdin(buffer);

	printf("read: %s string len: %d (including null)\n", buffer, sendLen);
	sent =  sendPDU(socketNum, buffer, sendLen, &flag);

	if (sent < 0){perror("send err"); exit(-1);}

	printf("Socket:%d: Sent, Length: %d msg: %s\n", socketNum, sent, buffer);
}

void processMsgFromServer(int socketNum, uint8_t* buffer){
	int recvBytes = 0;
	recvBytes = recvPDU(socketNum, buffer, MAXBUF, &flag);
	if(recvBytes == 0){perror("Server terminated"); exit(-1);}
	printf("Socket %d: Byte recv: %d message: %s\n", socketNum, recvBytes, buffer);
}