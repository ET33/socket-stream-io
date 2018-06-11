#include "sockets.h"
#include "bytestream.h"
#include "client_interface.h"

int main(int argc, char * const argv[]){
	if (argc != 3)
		ERROR2_EXIT(ANSI_COLOR_YELLOW "Usage: %s IP_ADDRESS PORT\n" ANSI_COLOR_RESET, argv[0]);
	
	client_args_struct args = {0};

	/* Criando o socket client e especificando o endereço do servidor. */
	args.client_socket = create_socket(atoi(argv[2]), SERVER_TYPE, PROTOCOL, argv[1], CLIENT);

	/* Estabelece conexão com o socket server. */
	connect_server(args.client_socket, argv[1], argv[2]);

	// To do: concatenate this default path to a UNIQUE ID
	// for each client
	create_temp_microaudio_dir(TEMP_CLIENT_DIR);

	/* Application section. */
	args.msg_recv.control_id = MESSAGE;
	args.msg_recv.id = INVALID;
	args.msg_send.control_id = MESSAGE;
	args.msg_send.id = INVALID;    

	// /* Calling the audio processing function */
	args.ss = processSounds(
		&args.msg_recv, 
		&args.process_end, 
		TEMP_CLIENT_DIR, 
		1);

	/* Making asynchronous communication. */    
	pthread_create(&args.recv_thread, NULL, recv_data, (void *) &args);
	pthread_create(&args.send_thread, NULL, send_data, (void *) &args);

	/* Join threads */
	pthread_join(args.recv_thread, NULL);    
	pthread_join(args.send_thread, NULL);    

	/* Encerra a conexão do socket. */
	destroy_socket(args.client_socket);    
	destroy_sound_struct(args.ss);

	return 0;
}
