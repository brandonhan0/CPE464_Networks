// createPDU
// printPDU

#include <stdint.h>
#include <arpa/inet.h>
#include "checksum.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#pragma pack(push,1)
typedef struct{
    uint32_t segmentNum; // network order
    uint16_t checksum;
    uint8_t flag;
    uint16_t payloadSize;
    uint8_t payload[1400];
}PDU_;
#pragma pack(pop)

int createPDU(PDU_* pdu, uint32_t sequenceNum, uint8_t flag, uint8_t* payload, uint16_t payloadLen);
void printPDU(uint8_t* buffer, int pduLength);
