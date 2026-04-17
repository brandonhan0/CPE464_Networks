#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
int sendPDU(int socketNumber, uint8_t* dataBuffer, int lengthOfData, int flag);
int recvPDU(int socketNumber, uint8_t* dataBuffer, int bufferSize, int flag);
    