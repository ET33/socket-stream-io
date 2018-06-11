#include "sockets.h"
#include "bytestream.h"
#include "server_interface.h"

int main(int argc, char * const argv[]) {
	if (argc != 3) {
		ERROR2_EXIT(
			ANSI_COLOR_YELLOW 
			"Usage: %s port music_dir\n" 
			ANSI_COLOR_RESET, 
			argv[0]);
	}

	char *host = getIP();
	unsigned short int port = atoi(argv[1]);	

	server_args_struct args = {0};
	args.music_dir = argv[2];

	/* Initialize server structure. */
	args.server_socket = create_socket(BUFFER_SIZE, port, SERVER_TYPE, PROTOCOL, host, SERVER);

	/* Bind server to the given port. */
	attach_server(args.server_socket, port);

	/* Listen. */	
	printf(
		ANSI_COLOR_GREEN "Server listening" ANSI_COLOR_YELLOW 
		" %s" ANSI_COLOR_GREEN " on port" ANSI_COLOR_YELLOW 
		" %d" ANSI_COLOR_GREEN"...\n" ANSI_COLOR_RESET, 
		host, port);

	if (listen(args.server_socket->fd, NUM_CONNECTIONS)) {
		ERROR_EXIT(
			ANSI_COLOR_RED 
			"Failed to listen for connections" 
			ANSI_COLOR_RESET);
	}
	
	/* Create client socket. */	
	if ((args.client_socket = accept(args.server_socket->fd, NULL, NULL)) == -1) {
		ERROR_EXIT(
			ANSI_COLOR_RED 
			"Failed to accept client" 
			ANSI_COLOR_RESET);
	} else {
		printf(
			ANSI_COLOR_GREEN 
			"Connection accepted.\n" 
			ANSI_COLOR_RESET);
	}

	/* Application section. */
	args.msg_recv.control_id = MESSAGE;
	args.msg_recv.id = INVALID;	
	args.msg_send.control_id = MESSAGE;
	args.msg_send.id = INVALID;	
	
	/* Calling the audio processing function */
	// args.ss = processSounds(
	// 	&args.msg_recv, 
	// 	&args.process_end, 
	// 	args.music_dir, 
	// 	0);

	printf("Welcome to " 
		ANSI_COLOR_CYAN "Theodora" ANSI_COLOR_RESET 
		" music server socket stream!\nType" 
		ANSI_COLOR_YELLOW " HELP " ANSI_COLOR_RESET 
		"for command list.\n");

	args.msg_send.control_id = GREETINGS;
	send(args.client_socket, &args.msg_send, sizeof(data_unit), 0);

	/* Free host IP memory. */
	if (host)
		free(host);

	/* Making asynchronous communication. */    
	pthread_create(
		&args.recv_thread, 
		NULL, 
		server_recv_data, 
		(void *) &args);

	pthread_create(
		&args.send_thread, 
		NULL, 
		server_send_data, 
		(void *) &args);

	pthread_join(args.recv_thread, NULL);    

	/* Destroy server structure. */
	destroy_socket(args.server_socket);
	destroy_sound_struct(args.ss);

	return 0;
}
