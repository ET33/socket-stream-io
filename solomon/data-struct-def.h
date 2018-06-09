#ifndef __DATA_STRUCT_DEF_H_
#define __DATA_STRUCT_DEF_H_

#include "server.h"

enum{
    MESSAGE,
    AUDIO,
    VIDEO,
    IMAGE,
    ERROR,
    PLAY,
    STOP,
    COMMAND,
    START,
    EXIT
};

// Basic data transfer between client and server
typedef struct {
    int id;
    char description[BUFFER_SIZE];
} data_unit;

#endif
