#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
int sendPDU(int socketNumber, uint8_t* dataBuffer, int lengthOfData, int flag);
int recvPDU(int socketNumber, uint8_t* dataBuffer, int bufferSize, int flag);

typedef enum{
    ZERO,
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
    C_S_C_BROADCAST,/* = 4
    - sent from server to all other clients to indicate broadcast packet
    */
    C_S_C_MESSAGE,/* = 5
    - this is a flag sent to a client from another client via the server
    */
    C_S_C_MULTICAST, /* = 6
    - indicates was a multicast packet
    */
    S_C_MULTICAST_ERROR, /* = 7
    - indicates if handle in multicast does not exist
    */
    NOTHING8,
    NOTHING9,
    C_S_HANDLE_REQ,/* = 10
    - client is requesting handle list
    */
    S_C_HANDLE_RESP_1,/* = 11
    - server responding to client handle list request 
    - this message gives the number of handles in the handle table
    */
    S_C_HANDLE_RESP_2, /* = 12
    - this flag comes right after 11 and will basically send this flag with every handle send and the client has to receive it x many times
    */
   S_C_L_DONE /* = 13
   - tells the client that handle list is done
   */
	
}flags;

typedef enum{
	MESSAGE,
	MULTICAST,
	BROADCAST,
	HANDLELIST
}commands;

#pragma pack(push, 1)
typedef struct{
     uint8_t handleLen;
     uint8_t handle[100];
}destHandle;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct{ // i think this is right idk
    uint8_t flag; // flag = 1 normally
    uint8_t srcHandleLen; // length
    uint8_t srcHandle[100]; // max 100 bytes
    uint8_t numDest; // this will be hardcoded to 1 for %m commands
    destHandle dests[10]; // allows for muiltiple handles
    uint8_t message[200]; // message is 200 bytes max
} Mpacket;
#pragma pop(push)

#pragma pack(push, 1)
typedef struct{ // i think this is right idk
    uint8_t flag; // flag = 4 normally
    uint8_t srcHandleLen; // length
    uint8_t srcHandle[100]; // max 100 bytes
    uint8_t message[200]; // message is 200 bytes max
} Bpacket;
#pragma pop(push)

#pragma pack(push, 1)
typedef struct{ // i think this is right idk
    uint8_t flag; // flag = 4 normally
    uint8_t srcHandleLen; // length
    uint8_t srcHandle[100]; // max 100 bytes
} Initpacket;
#pragma pop(push)

#pragma pack(push, 1)
typedef struct{ // i think this is right idk
    uint8_t flag; // flag = 4 normally
    uint8_t message[200]; // length
} ServerPacket;
#pragma pop(push)

