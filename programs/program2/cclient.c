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
#include <stdbool.h>

#include "networks.h"
#include "safeUtil.h"
#include "new.h"
#include "pollLib.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

uint8_t srcHandler[100];

void sendToServer(int socketNum);
int readFromStdin(uint8_t * buffer);
void checkArgs(int argc, char * argv[]);

int main(int argc, char * argv[]) // ./cclient handle server-name server-port i think server name would be like unix1-5 but we can ask like during class tmr
{
	int socketNum = 0;         //socket descriptor
	checkArgs(argc, argv);
	setupPollSet();

	socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG); // now its cclient handle server-name server-port, [1], [2] was local host, server port number
    if(sizeof(srcHandler) <= strlen(argv[1])) return -1; 
	strcpy(srcHandler, argv[1]); // should work and add the null ternimator	
	addToPollSet(socketNum);
	addToPollSet(STDIN_FILENO);

	if(CheckHandle(socketNum) != 0){return -1;} // will close connection if bad
	while(true){
		clientControl(socketNum);
	}
	close(socketNum);
	
	return 0;
}

void sendToServer(int socketNum){
	uint8_t buffer[MAXBUF];   //data buffer
	int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent
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
	recvBytes = recvPDU(socketNum, buffer, MAXBUF, 0);
	if(recvBytes == 0) {perror("Server has terminated"); close(socketNum); exit(-1);}
	printf("Socket %d: Byte recv: %d message: %s\n", socketNum, recvBytes, buffer);
	
}

int readFromStdin(uint8_t * buffer){
// THIS HAS TO PUT MESSAGE IN BUFFER AND RETURN SIZE OF BUFFER
	char aChar = 0;
	int inputLen = 0;        
	char prevChar = 0;
	commands command;

	buffer[0] = '\0'; // this is just like if we enter nothing

	char stdinBuffer[1024];
	if (fgets(stdinBuffer, sizeof(stdinBuffer), stdin)) { // should take whole stdin and put in string so no blocking w muiltiple scanfs 
		//tokenizing looked at string example code off google bc how else r u supposed to know how these functions work dawg
		char *token = strtok(stdinBuffer, " "); // gets first token this should be command
		int counter = 0;
		if(token != NULL) { // gets command
			printf("%s\n", token);
			if(token[0] == '%'){
				switch(token[1]){
					case 'M':
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
						printf("Invalid command\n");
						return -1;
				}
			}
		}
		
		token = strtok(NULL, " ");
		int arg_counter = 0;
		switch(command){
			case MESSAGE: // for message we want to know the dest-handle-name first, than the rest is the tx-message
			{
				Mpacket packetOut = {};
				packetOut.flag = 5;
				packetOut.srcHandleLen = strlen(srcHandler)+1;
				if(packetOut.srcHandleLen > 99) {printf("Invalid src handle, handle longer than 100 characters\n"); return -1;}
				strcpy(packetOut.srcHandle, &srcHandler); 
				packetOut.numDest = 1;
				token = strtok(NULL, " "); // this should get desthandle
				packetOut.dests[0].handleLen = strlen(token)+1;
				if(packetOut.dests[0].handleLen > 99) {printf("Invalid dst handle, handle longer than 100 characters\n"); return -1;}
				strcpy(packetOut.dests[0].handle, token); 
				uint8_t* theMessage = strtok(NULL, ""); // i love this function
				if(strlen(theMessage) >= 199){printf("This message is to big\n"); return -1;}
			    strcpy(packetOut.message, theMessage);
				memcpy(buffer, &packetOut, sizeof(Mpacket));
				return sizeof(Mpacket);
				break; // it never gets here but whatever
			}
			case MULTICAST:
			{
				while(token != NULL){
					token = strtok(NULL, " ");
				}
				break;
			}
			case BROADCAST:
			{
				while(token != NULL){
					token = strtok(NULL, " ");
				}
				break;
			}
			case HANDLELIST:
			{
				break;
			}
		}	
	}
}










	// char cmd[3];

	// // blocking calls for commands
	// scanf("%2s", cmd); // get cmd
	// printf("%s\n", cmd);
	// if(cmd[0] == '%'){
	// 	switch(cmd[1]){
	// 		case 'M':
	// 		case 'm':
	// 			command = MESSAGE;
	// 			break;
	// 		case 'C':
	// 		case 'c':
	// 			command = MULTICAST;
	// 			break;
	// 		case 'b':
	// 		case 'B':
	// 			command = BROADCAST;
	// 			break;
	// 		case 'l':
	// 		case 'L':
	// 			command = HANDLELIST;
	// 			break;
	// 		default:
	// 			printf("Invalid command\n");
	// 			command = -1;
	// 	}	
	// } else {
	// 	printf("Invalid command\n");
	// 	command = -1;
	// }
	// printf("command: %d", command);
	// switch(command){

	// 	case MESSAGE: // for message we want to know the dest-handle-name first, than the rest is the tx-message

	// 		Mpacket packetOut = {};

	// 		packetOut.flag = 5;
	// 		packetOut.srcHandleLen = strlen(srcHandler)+1;
	// 		if(packetOut.srcHandleLen > 100) printf("Invalid src handle, handle longer than 100 characters\n");
	// 		memcpy(&packetOut.srcHandle, &srcHandler, packetOut.srcHandleLen); // should work and add the null ternimator
	// 		packetOut.numDest = 1;
	// 		scanf("%s", &packetOut.dests[0].handle); // this comes after the command so booya
	// 		packetOut.dests[0].handleLen = strlen(packetOut.dests[0].handle)+1;
	// 		if(packetOut.dests[0].handleLen > 100)	 printf("Invalid dst handle, handle longer than 100 characters\n");
	// 		scanf("%s", &packetOut.message); // this should have a null ternimator
	// 		memcpy(buffer, &packetOut, sizeof(Mpacket)); // put message struct in buffer

	// 		return sizeof(Mpacket);
	// 		break; // it never gets here but whatever
	// 	case MULTICAST:
	// 		break;
	// 	case BROADCAST:
	// 		break;
	// 	case HANDLELIST:
	// 		break;
	// }	
	// return -1;


void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s handle server-name server-port \n", argv[0]);
		exit(1);
	}
}

void clientControl(int socketNum){ // still need to do stuff with flag idk
	uint8_t buffer[MAXBUF];   //data buffer
	uint8_t flag;
	printf("$: ");
	fflush(stdout); // pollCall was blocking my stdin print bro wtf
	int fd = pollCall(-1);
	if(fd == socketNum){
		// do socket
		processMsgFromServer(socketNum, buffer);
	} else{
		// stdin
		processStdin(socketNum, buffer);
	}
}

void processStdin(int socketNum, uint8_t* buffer){
	int sendLen = 0;        //amount of data to send
	int sent = 0;
	sendLen = readFromStdin(buffer);
	if(sendLen >= 0){
		printf("read: %s string len: %d (including null)\n", buffer, sendLen);
		sent =  sendPDU(socketNum, buffer, sendLen, 0);
	
		if (sent < 0){perror("send err"); exit(-1);}
	
		printf("Socket:%d: Sent, Length: %d msg: %s\n", socketNum, sent, buffer);
	}
}

void processMsgFromServer(int socketNum, uint8_t* buffer){
	int recvBytes = 0;
	flags flag;
	recvBytes = recvPDU(socketNum, buffer, MAXBUF, 0);
	if(recvBytes == 0){perror("Server terminated\n"); exit(-1);}

	// process flag first
	flag = buffer[0];
	
	switch(flag){
		case S_C_GOOD_HANDLE:
			printf("Server says handle good\n");
			break;
		case S_C_BAD_HANDLE:

			printf("Server says <%s> does not exist\n");
			break;
		case C_S_C_BROADCAST:
			break;
		case C_S_C_MESSAGE: // %m command from other clients(c->s->c)
			Mpacket* data = (Mpacket*) buffer;
			printf("%s: %s\n", data->srcHandle, data->message); // should literally just spit out the message
			break;
		case C_S_C_MULTICAST:
			break;
		case S_C_MULTICAST_ERROR:
			break;
		case S_C_HANDLE_RESP_1:
			break;
		case S_C_HANDLE_RESP_2:
			break;
	    case S_C_L_DONE:
			break;
	}
}

int CheckHandle(int socketNum){
	InitPacket message = {};
	uint8_t buffer[MAXBUF];

	message.flag = 1;
	message.srcHandleLen = strlen(srcHandler)+1;
	if(message.srcHandleLen > 100) printf("Invalid src handle, handle longer than 100 characters\n");
	memcpy(&message.srcHandle, &srcHandler, strlen(srcHandler)+1); // should work and add the null ternimator
	int sent =  sendPDU(socketNum, &message, sizeof(InitPacket), 0);
	if (sent < 0){perror("send err"); exit(-1);}
	int feedback = recvPDU(socketNum, buffer, MAXBUF, 0);
	ServerPacket* data = (ServerPacket*) buffer;
	if(data->flag == S_C_BAD_HANDLE){
		printf("Handle already has: %s\n", srcHandler);
		close(socketNum);
		return -1;
	}
	printf("Joined: %s\n", srcHandler);

	return 0;
}