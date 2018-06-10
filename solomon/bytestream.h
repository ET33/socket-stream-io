#ifndef __BYTESTREAM_H_
#define __BYTESTREAM_H_

// Include section
#include <pthread.h>
#include "queue.h"

// Typedef and define section

typedef struct {
	queue *ready_q;
	data_unit *cur_data_unit;
	int *process_end;
} args_struct;

typedef struct {
	// Thread vector
	pthread_t thread_id[2];
	args_struct args;
} sound_struct;

// Function declaration
sound_struct *processSounds(data_unit *cur_data_unit, int *process_end);
void destroy_sound_struct (sound_struct *ss);

#endif
