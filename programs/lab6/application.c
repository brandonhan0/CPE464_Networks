#include "application.h"

int createPDU(PDU_* pdu, uint32_t sequenceNum, uint8_t flag, uint8_t* payload, uint16_t payloadLen){

    /*
    
    lol i love structs live laugh love structs they make things so nice and readable
    
    typedef struct{
        uint32_t segmentNum; // network order
        uint16_t checksum;
        uint8_t flag;
        int payloadLen;
        uint8_t payload[1400];
    }PDU_;

    */

    pdu->segmentNum = htonl(sequenceNum); // host to network order for 32 bits 
    pdu->flag = flag;
    pdu->payloadSize = payloadLen;
    memcpy(pdu->payload, payload, payloadLen); // should be pointer to the payload already
    pdu->checksum = 0;
    int total_length = pdu->payloadSize + 7;
    unsigned short crc = in_cksum((unsigned short*)pdu, total_length); // fake error
    memcpy(&pdu->checksum, &crc, 2);
    crc = in_cksum((unsigned short*)pdu, total_length); // fake error
    if(crc == 0) printf("yay checksum is 0\n"); else printf("boo checksum bad\n");
    return sizeof(PDU_);
}

void printPDU(uint8_t* buffer, int pduLength){
    PDU_* mypdu = (PDU_*) buffer;

    int total_length = mypdu->payloadSize + 7;
    int crc = in_cksum((unsigned short*)buffer, total_length); // fake error
    if(crc == 0){
        printf("Sequence Number: %d \n", ntohl(mypdu->segmentNum));
        printf("\tFlag: %d \n", mypdu->flag);
        printf("\tPayload size: %d \n", mypdu->payloadSize);
        printf("\tPayload: %s \n", mypdu->payload);
    }else printf("boo checksum bad\n");
}