#ifndef __DATA_STRUCT_DEF_H_
#define __DATA_STRUCT_DEF_H_

#include "sockets.h"

enum{
    INVALID,
    MESSAGE,
    AUDIO,
    VIDEO,
    IMAGE,
    ERROR,
    LIST,
    PLAY,
    PAUSE,
    REPEAT,
    INFO,
    REPEAT_ONCE,
    NEXT,
    PREVIOUS,
    SHUFFLE,
    REPLAY,
    STOP,
    COMMAND,
    START,
    EXIT,
    SERVER,
    CLIENT,
    RECONNECT,
    PATH
};

/* Basic data transfer between client and server. */
typedef struct {
    int id;
    char description[BUFFER_SIZE];
} data_unit;

#endif
