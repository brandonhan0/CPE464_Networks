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

#include "networks.h"
#include "safeUtil.h"
#include "new.h"
#include "pollLib.h"



#define MAXBUF 1024
#define DEBUG_FLAG 1



void recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int mainServerSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
	int cont_flag = 0;
	portNumber = checkArgs(argc, argv);

	setupPollSet(); // initializes poll

	//create the server socket
	mainServerSocket = tcpServerSetup(portNumber);
	addToPollSet(mainServerSocket); // adds mainServerSocket to poll set

	while(cont_flag == 0){
		serverControl(mainServerSocket);
	}


	/* close the sockets */
	//close(clientSocket);
	close(mainServerSocket);

	
	return 0;
}

void recvFromClient(int clientSocket)
{
	uint8_t dataBuffer[MAXBUF];
	int messageLen = 0;
	
	//now get the data from the client_socket

	if ((messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF, 0)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	if (messageLen > 0)
	{
		printf("Socket %d: Message received, length: %d Data: %s\n", clientSocket, messageLen, dataBuffer);
		
		// send it back to client (just to test sending is working... e.g. debugging)
		messageLen = sendPDU(clientSocket, dataBuffer, messageLen, 0);
		printf("Socket %d: msg sent: %d bytes, text: %s\n", clientSocket, messageLen, dataBuffer);
	}
	else // if recv returns 0 than it means client has closed
	{
		close(clientSocket);
		removeFromPollSet(clientSocket); // removes from poll set
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
	int socket = pollCall(0);
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