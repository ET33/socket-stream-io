#include <server.h>
#include <dataStructDefault.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_TYPE AF_INET

#define NUM_CONNECTIONS 1

// '0' stands for IP
// Check out /etc/protocol
#define PROTOCOL 0


int main(int argc, char * const argv[]) {

	// Initialize server structure
	server * s=create_server(BUFFER_SIZE, PORT, SERVER_TYPE, PROTOCOL);	

	// Bind server to the given PORT
	attach_server(s, PORT);

	// Listen
	int ret=listen(s->fd, NUM_CONNECTIONS);
	if (ret)
		ERROR_EXIT("Failed to listen for connections");

	// Create client socket
	int new_socket = accept(s->fd, NULL, NULL);
	if (new_socket == -1) {
		ERROR_EXIT("Failed to accept client");
	} else {
		printf("Connection accepted.\n");
	}

	// Application goes here
	// ......	
	data_unit msg;
	do {
		printf("Digite a mensagem a ser enviada para o cliente\n");
		scanf("%s%*c", msg.description);	
		msg.id = MESSAGE;

		/* Enviando a msg para o cliente. */
		send(new_socket, &msg, sizeof(msg), 0);

		/* Recebendo a msg do cliente. */
		if(recv(new_socket, &msg, sizeof(msg), 0) == -1){
			printf("Error on receiving data from client\n");
			exit(EXIT_FAILURE);
		} else {
			printf("Client response: %s \n", msg.description);
		}
	} while(msg.id != EXIT);
		
	// Destroy server structure
	destroy_server(s);

	return 0;
}
