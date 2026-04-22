/******************************************************************************
* myServer.c
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
#include <stdbool.h>

#include "networks.h"
#include "safeUtil.h"
#include "new.h"
#include "pollLib.h"
#include "handletable.h"



#define MAXBUF 1400
#define DEBUG_FLAG 1




void recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int mainServerSocket = 0; 
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);
	initHandleTable();
	setupPollSet();

	mainServerSocket = tcpServerSetup(portNumber);
	addToPollSet(mainServerSocket);

	while(true){
		serverControl(mainServerSocket);
	}
	close(mainServerSocket);	
	return 0;
}

void recvFromClient(int clientSocket)
{
	uint8_t dataBuffer[MAXBUF];
	int messageLen = 0;
	uint8_t flag;
	
	//now get the data from the client_socket

	if ((messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF, 0)) < 0) // data buffer will now have whatever pdu it is and now we gotta figure it out using the flag
	{
		printf("hi1\n");
		perror("recv call");
		exit(-1);
	}

	if (messageLen > 0)
	{
		// process flag first
		flag = dataBuffer[0];		
		printf("flag: %d\n", flag);

		switch(flag){
			case C_S_INIT:
			{
			 	InitPacket* data = (InitPacket*)dataBuffer;
				ServerPacket message = {};
				printf("%s has joined\n", data->srcHandle);
				if(doesHandleExist(data->srcHandle, data->srcHandleLen) == 0){ // 0 indicates this does not exist and were good
					message.flag = S_C_GOOD_HANDLE;
				} else message.flag = S_C_BAD_HANDLE;
				strcpy(&message.message, data->srcHandle);
				int sendSize = sendPDU(clientSocket, &message, sizeof(ServerPacket), 0);
				addItem(clientSocket, data->srcHandle); // adds to handle table
				break;
			}
			case C_S_C_MESSAGE: // %m
			{
				Mpacket* data = (Mpacket*)dataBuffer;
				printf("receiving data from:%s\n", data->srcHandle);
				printf("Dest Len: %d, Destination handle: %s\n", data->dests[0].handleLen, data->dests[0].handle);
				if(doesHandleExist(data->dests[0].handle, data->dests[0].handleLen) == 0){
					printf("This handle does not exist you frickin chud\n");
					clientDoesNotExistError(clientSocket ,data->dests[0].handle);
					return -1;
				}
				printf("This is the dest handle: %s", &data->dests[0].handle);
				int receiverSocket = getSocketNum(&data->dests[0].handle);
				int sentBytes = sendPDU(receiverSocket, data, messageLen, 0); // sends to the client
				break;
			}
			case C_S_C_BROADCAST:
				break;
			case C_S_C_MULTICAST: // this will send the message to everyone except the ppl that dont exist
			{
				Mpacket* data = (Mpacket*)dataBuffer;
				printf("receiving data from:%s\n", data->srcHandle);
				for(int i = 0; i < data->numDest; i++){ // check if they even exist first
					if(doesHandleExist(data->dests[i].handle, data->dests[i].handleLen) == 0){
						printf("This handle(%s) does not exist you frickin chud\n", data->dests[i].handle);
						clientDoesNotExistError(clientSocket ,data->dests[i].handle);
						continue;
					}
					printf("This is the dest handle: %s", &data->dests[i].handle);
					int receiverSocket = getSocketNum(&data->dests[i].handle);
					int sentBytes = sendPDU(receiverSocket, data, messageLen, 0); // sends to the client
				}
				break;
			}
			case C_S_HANDLE_REQ:
				break;
		}
	}
	else // if recv returns 0 than it means client has closed
	{
		close(clientSocket);
		removeFromPollSet(clientSocket); 
		printf("Socket %d: Connection closed by other side\n", clientSocket);
	}

}



int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	
	return portNumber;
}

void serverControl(int mainServerSocket){
	int socket = pollCall(-1);
	if(socket > 0){ // if theres a socket
		if(socket == mainServerSocket){ // if this is the acceptor socket
			addNewSocket(mainServerSocket); // adds new socket to poll
		}
		else{ // if its not a new socket its one of the clients already connected
			processClient(socket);
		}
	}
}

void processClient(int clientSocket){
	recvFromClient(clientSocket);
}

void addNewSocket(int mainServerSocket){
	int clientSocket = tcpAccept(mainServerSocket, DEBUG_FLAG);
	addToPollSet(clientSocket); // add client to poll set
}

int clientDoesNotExistError(int socketDest, uint8_t* badHandle){
	ServerPacket message = {};
	message.flag = 7;
	strcpy(&message.message, badHandle);
	int sendSize = sendPDU(socketDest, &message, sizeof(ServerPacket), 0);
	return 0;
}