#ifndef __DATA_STRUCT_DEF_H_
#define __DATA_STRUCT_DEF_H_

#include "sockets.h"

enum{
    INVALID,
    MESSAGE,
    MUSIC,
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
    NUSERS,
    GREETINGS,
    PATH,
    HELP
};

/* Basic data transfer between client and server. */
typedef struct {
	int control_id; // Interface Commands
    int id; // Package Order Number
    char description[BUFFER_SIZE]; // Data
} data_unit;

#endif
