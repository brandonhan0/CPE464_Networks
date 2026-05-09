#include "application.h"

int createPDU(PDU_* pdu, uint32_t sequenceNum, uint8_t flag, uint8_t* payload, int payloadLen){

    /*
    
    lol i love structs live laugh love structs they make things so nice and readable
    
    typedef struct{
        uint32_t segmentNum; // network order
        uint8_t flag;
        int payloadLen;
        uint8_t payload[1400];
    }PDU_;

    */

    pdu->segmentNum = htonl(sequenceNum); // host to network order for 32 bits 
    pdu->flag = flag;
    pdu->payloadSize = payloadLen;
    memcpy(pdu->payload, payload, payloadLen); // should be pointer to the payload already
    return sizeof(PDU_);
}

void printPDU(PDU_* mypdu, int pduLength){
    if(in_cksum((unsigned short*)mypdu, pduLength) == 0){
        printf("Sequence Number: %d \n", mypdu->segmentNum);
        printf("\tFlag: %d \n", mypdu->flag);
        printf("\tPayload size: %d \n", mypdu->payloadSize);
        printf("\tPayload: %s \n", mypdu->payload);
    } else if(in_cksum((unsigned short*)mypdu, pduLength) != 0) printf("PDU(Seq num: %d): Corrupted\n", ntohl(mypdu->segmentNum)); // this is wrong do it better
}