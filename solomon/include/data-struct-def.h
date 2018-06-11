#ifndef __DATA_STRUCT_DEF_H_
#define __DATA_STRUCT_DEF_H_

#include "sockets.h"

enum{
	INVALID,
	MESSAGE,
	MESSAGE_NOANS,
	STOP,
	MUSIC,
	ERROR,
	LIST,
	PLAY,
	INFO,
	NEXT,
	PREVIOUS,
	SHUFFLE,
	COMMAND,
	START,
	EXIT,
	SERVER,
	CLIENT,
	NUSERS,
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
