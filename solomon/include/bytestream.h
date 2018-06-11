#ifndef __BYTESTREAM_H_
#define __BYTESTREAM_H_

// Include section
#include <pthread.h>
#include "queue.h"

// Typedef and define section
#define TEMP_SERVER_DIR "./.SERVER_DIR/"

// Note that just this path isn't suffice for
// the client, as it's possible to exists n clients
// for a single server.
#define TEMP_CLIENT_DIR "./.CLIENT_DIR/"

// Max digits of a UNSIGNED LONG LONG INT
// Calculated as 
//	  ceil(log10(1 + MAX_UNSIGNED_LONG_LONG_INT))
// 	= ceil(log10(1 + 18.446.744.073.709.551.615))
//	= 20
#define MAX_COUNTER_LEN 20

// The full command is 
// "ffplay -loglevel panic -nodisp <temp_dir_path_len><temp_dir_ls[i]> && rm <temp_dir_path_len><temp_dir_ls[i]>"
// This define counts "ffplay -loglevel panic -nodisp "(31) + " && rm "(7) = 38
#define PLAY_MICROAUDIO_CMD_SIZE 38

enum {
    PROCESS_READY_QUEUE,
    UPDATE_READY_QUEUE,
    PLAY_MICROAUDIOS,
    THREAD_NUM
};

typedef struct {
    queue *ready_q, *aux_q;
    data_unit *cur_data_unit;
    int *process_end;
    char *temp_dir_path;
} args_struct;

typedef struct {
    // Thread vector
    pthread_t thread_id[THREAD_NUM];
    args_struct args;
} sound_struct;

// Function declaration
char **get_file_list(char *directory, unsigned long int *size);
sound_struct *processSounds(data_unit *cur_data_unit, int *process_end, char *temp_dir_path, int play_audio);
void destroy_sound_struct (sound_struct *ss);
void create_temp_microaudio_dir(char *path);
void remove_temp_microaudio_dir(char *path);
char *readline(FILE *fp);
void *update_ready_queue(void *vargs);

#endif
