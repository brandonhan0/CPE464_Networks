#include <stdlib.h>
#include <stdint.h>
#include "string.h"

typedef struct{
    uint8_t handleName[100];
    int socketNum;
} tableItem;

static int curTableSize = 0;
static int curMaxTableSize = 100;

int makeTable(void);
int addItem(int socketNum, uint8_t* handleName);
int increaseTableSize();
int getHandle(int socketNum, uint8_t* buffer);
int getSocketNum(uint8_t* handle);
int getTableSize(void);