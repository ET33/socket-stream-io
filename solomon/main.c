#include "server.h"
#include "queue.h"

// '0' stands for IP:
// Check out /etc/protocol with cat command

int main(int argc, char * const argv[]) {
	if(argv[1] == NULL) {
		printf(
			ANSI_COLOR_YELLOW 
			"Usage: %s port\n" 
			ANSI_COLOR_RESET, 
			argv[0]);
		exit(EXIT_FAILURE);
	}

	char *host = getIP();
	unsigned short int port = atoi(argv[1]);	

	// Initialize server structure
	server *s = create_server(
		BUFFER_SIZE, 
		port, 
		SERVER_TYPE, 
		PROTOCOL, 
		host);

	// Bind server to the given port
	attach_server(s, port);

	printf(
		ANSI_COLOR_GREEN "Server listening" 
		ANSI_COLOR_YELLOW " %s" 
		ANSI_COLOR_GREEN " on port" 
		ANSI_COLOR_YELLOW " %d" 
		ANSI_COLOR_GREEN"...\n" 
		ANSI_COLOR_RESET, 
		host, port);

	// Listen
	int ret = listen(s->fd, NUM_CONNECTIONS);
	if (ret) {
		ERROR_EXIT(
			ANSI_COLOR_RED 
			"Failed to listen for connections" 
			ANSI_COLOR_RESET);
	}

	// Create client socket
	int new_socket;
	if ((new_socket = accept(s->fd, NULL, NULL)) == -1) {
		ERROR_EXIT(
			ANSI_COLOR_RED 
			"Failed to accept client" 
			ANSI_COLOR_RESET);
	}

	printf(
		ANSI_COLOR_GREEN 
		"Connection accepted.\n" 
		ANSI_COLOR_RESET);

	// Application goes here
	data_unit msg;
	do {
		printf(
			ANSI_COLOR_RED 
			"Server response: " 
			ANSI_COLOR_RESET);

		scanf("%[^\n]%*c", msg.description);	
		msg.id = MESSAGE;

		/* Enviando a msg para o cliente. */
		send(new_socket, &msg, sizeof(msg), 0);

		/* Recebendo a msg do cliente. */
		if (recv(new_socket, &msg, sizeof(msg), 0) == -1){
			ERROR_EXIT(
			ANSI_COLOR_RED 
			"Error on receiving data from client" 
			ANSI_COLOR_RESET);

		} else {

			printf(
			ANSI_COLOR_CYAN 
			"Client response: " 
			ANSI_COLOR_RESET "%s \n", 
			msg.description);

		}
	} while(msg.id != EXIT);

	// Free host IP memory
	if (host)
		free(host);
	// Destroy server structure
	destroy_server(s);

	return 0;
}
