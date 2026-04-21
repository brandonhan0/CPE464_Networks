#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
int sendPDU(int socketNumber, uint8_t* dataBuffer, int lengthOfData, int flag);
int recvPDU(int socketNumber, uint8_t* dataBuffer, int bufferSize, int flag);


#pragma pack(push, 1)
typedef struct{ // i think this is right idk
    uint8_t srcHandleLen;
    uint8_t srcHandle[srcHandleLen];
    uint8_t numDest; // this will be hardcoded to 1 for %m commands
    uint8_t dstHandleLen;
    uint8_t dstHandle[dstHandleLen];
    uint8_t message[200]; // message is 200 bytes
} Mpacket;
#pragma pop(push)