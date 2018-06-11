#ifndef __SERVER_INTERFACE_H_
#define __SERVER_INTERFACE_H_

// Define and typedefs section
typedef struct {
	socket_structure *server_socket;
	int new_socket;
	sound_struct *ss;
	int process_end;
	pthread_t recv_thread, send_thread;
	char *filepath;
	char *music_dir;
	data_unit msg;
} server_args_struct;

typedef struct {
	data_unit **msgs;
	int number_of_data_units;
	int socket;
} file_units_struct;

// Function declaration
void *server_send_data(void *vargs);
void *server_recv_data(void *vargs);
void process_data(server_args_struct *args);
data_unit process_commands(data_unit msg, char *music_path);
void *send_data_units(void *vargs);
file_units_struct *break_file(char *filepath);

#endif
