#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
int sendPDU(int socketNumber, uint8_t* dataBuffer, int lengthOfData, int flag);
int recvPDU(int socketNumber, uint8_t* dataBuffer, int bufferSize, int flag);

typedef enum{
	C_S_INIT, /* = 1
    - client blocks until it receives flag 2 or 3 now and server needs to send that
    */
	S_C_GOOD_HANDLE,/* = 2 
    - this means good handle
    - response to a 1 flag from client
    */
    S_C_BAD_HANDLE,/* = 3
    - straight forward hopfully
    */
    S_C_BROADCAST,/* = 4
    - sent from server to all other clients to indicate broadcast packet
    */
    C_S_C_MESSAGE,/* = 5
    - this is a flag sent to a client from another client via the server
    */
    

   

	
}flags;

typedef enum{
	MESSAGE,
	MULTICAST,
	BROADCAST,
	HANDLELIST
}commands;

#pragma pack(push, 1)
typedef struct{ // i think this is right idk
    uint8_t flag; // flag first
    uint8_t srcHandleLen; // length
    uint8_t srcHandle[100]; // max 100 bytes
    uint8_t numDest; // this will be hardcoded to 1 for %m commands
    uint8_t dstHandleLen;
    uint8_t dstHandle[100];
    uint8_t message[200]; // message is 200 bytes max
} Mpacket;
#pragma pop(push)