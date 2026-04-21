#include "handletable.h"

tableItem *handleTable;

int initHandleTable(void){
    handleTable = malloc(curMaxTableSize * sizeof(tableItem)); // can handle 100 of these bad boys
    if (handleTable == NULL) return -1;
    return 0;
}

int addItem(int socketNum, uint8_t* handleName){
    if(handleTable == NULL) return -1;
    if(curTableSize >= curMaxTableSize) increaseTableSize();
    tableItem newHandle = {};
    newHandle.socketNum = socketNum;
    if(strlen(handleName) >= 99) return -1;
    strncpy((char *)newHandle.handleName, (char *)handleName, sizeof(newHandle.handleName) - 1);
    newHandle.handleName[sizeof(newHandle.handleName) - 1] = '\0';
    handleTable[curTableSize] = newHandle;
    curTableSize++;
    return 0;
}

int increaseTableSize(){
    curMaxTableSize += 100;
    tableItem *newTable = realloc(handleTable, curMaxTableSize * sizeof(tableItem));
    if(newTable == NULL) return -1;
    handleTable = newTable;
    return 0;
}

int getHandle(int socketNum, uint8_t* buffer){ // return size of handle of the socket number and fill the buffer
    for(int i = 0; i < curTableSize; i++){
        if(handleTable[i].socketNum == socketNum){
            int size = strlen((uint8_t*)handleTable[i].handleName)+1;
            memcpy(buffer, &handleTable[i].handleName, size); // fill buffer
            return size;
        }
    }
    return -1;
}

int getSocketNum(uint8_t* handle){// must be null terminated
    for(int i = 0; i < curTableSize; i++){
        if(strcmp(handle, handleTable[i].handleName) == 0){ // if its in here
            return handleTable[i].socketNum;
        }
    }
    return -1;
}

int giveHandleTable(){
    printf("Clients online:\n");
    for(int i = 0; i < curTableSize; i++){
        printf("\t%s\n", handleTable[i].handleName);
    }
} // todo

int getTableSize(void){
    return curTableSize;
}