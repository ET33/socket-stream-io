#ifndef __CLIENT_INTERFACE_H_
#define __CLIENT_INTERFACE_H_

// Define and typedefs section
typedef struct {
	socket_structure *client_socket;
	sound_struct *ss;
	int process_end;
	pthread_t recv_thread, send_thread;
	data_unit msg_recv, msg_send;
} client_args_struct;

// Function declaration
void *send_data(void *vargs);
void *recv_data(void *vargs);
data_unit process_commands(data_unit msg);
void process_data(data_unit data, sound_struct *ss);

#endif
