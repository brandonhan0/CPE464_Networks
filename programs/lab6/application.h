// createPDU
// printPDU

#include <stdint.h>
#include <arpa/inet.h>
#include "checksum.h"

#pragma pack(push,1)
typedef struct{
    uint32_t segmentNum; // network order
    uint8_t flag;
    int payloadSize;
    uint8_t payload[1400];
}PDU_;
#pragma pack(pop)

int createPDU(PDU_* pdu, uint32_t sequenceNum, uint8_t flag, uint8_t* payload, int payloadLen);
void printPDU(PDU_* mypdu, int pduLength);
