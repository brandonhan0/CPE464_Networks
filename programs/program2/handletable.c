#include "handletable.h"

tableItem *handleTable;

// need to implement removing an item, implement this by just getting handle item and changing name to  "" TT

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
    if(strlen(handleName) >= sizeof(newHandle.handleName)) return -1;
    strcpy(newHandle.handleName, handleName); // should work and add the null ternimator
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

int giveHandleTableItem(uint8_t* buffer, int itemNum){
    memcpy(buffer, handleTable[itemNum].handleName, strlen(handleTable[itemNum].handleName)+1); // puts handle name in buffer
    return 0;
}

uint32_t getTableSize(void){
    return curTableSize;
}

int doesHandleExist(uint8_t* buffer, int bufferSize){
    for(int i = 0; i < curTableSize; i++){
        if(0 == strncmp(buffer, handleTable[i].handleName, bufferSize)){ // basically if it exists than return 1
            return 1; 
        }
    }
    return 0; // otherwise return 0
}