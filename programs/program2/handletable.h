#include <stdlib.h>
#include <stdint.h>
#include "string.h"

typedef struct{
    uint8_t handleName[100];
    int socketNum;
    int removed;
} tableItem;

static uint32_t curTableSize = 0;
static int curMaxTableSize = 100;

int initHandleTable(void);
int addItem(int socketNum, uint8_t* handleName);
int increaseTableSize();
int getHandle(int socketNum, uint8_t* buffer);
int getSocketNum(uint8_t* handle);
uint32_t getTableSize(void);
void removeHandle(int socketNum);
int giveHandleTableItem(uint8_t* buffer, int itemNum);
int doesHandleExist(uint8_t* buffer, int bufferSize);
int giveHandleTableSocketNum(int itemNum);