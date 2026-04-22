#include "new.h"

int sendPDU(int socketNumber, uint8_t* dataBuffer, int lengthOfData, int flag){ // sending pdus

    /*
    
    
    I did this following your video so like if it doesnt work its kind of your fault not mine
    
    
    1 send 2 receives???   

    recv 1 is length of pdu

    recv 2 is the rest of pdu


    this returns data bytes sent

    */
    int numBytes = 0;
    uint16_t new_size_host = lengthOfData + 2; // for pdu size stuff
    uint16_t new_size_network = htons(new_size_host); // put in network order

    uint8_t out[new_size_host]; // byte array new buffer

    uint8_t* ptr = out; // point to that shi

    memcpy(ptr, &new_size_network, 2); // should show make first 2 bytes the size
    memcpy(ptr+2, dataBuffer, lengthOfData); // copy data into pdu data or whatever
    numBytes = send(socketNumber, ptr, new_size_host, 0);
    if(numBytes < 0) return -1;// pdu length in host order duh we are the host

    return numBytes; // return the data bytes we sent 

}

int recvPDU(int socketNumber, uint8_t* dataBuffer, int bufferSize, int flag){ // receiving pdus

    /*
    
    use wait all to receive data in buffer

    first function call to this will be bufferSize 2 for the pdu length,
    second will be the data

    maybe taks MSG_WAITALL flag as parameter who knows we shall see

    lwk i think i need to output to this data buffer wait
    
    */
    int numBytes = 1;
    uint16_t size;
    numBytes = recv(socketNumber, &size, 2, MSG_WAITALL);
    if(numBytes < 0){printf("error on first recv\n"); return -1;} // this should return size of the data buffer booya

    if(numBytes == 0) {printf("kill\n");return 0;} // client closed

    int size_host = ntohs(size); // how big it is in host order


    if(bufferSize < size_host){printf("buffer too small\n"); return -1;}
    numBytes = recv(socketNumber, dataBuffer, size_host-2, MSG_WAITALL);
    if(numBytes < 0){printf("error in second recv\n"); return -1;} // this should return data

    if(numBytes == 0) return 0; // client closed

    return numBytes; // do not add 2 to this
}