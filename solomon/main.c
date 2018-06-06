#include <server.h>

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
	int new_socket=accept(s->fd, NULL, NULL);
	if (new_socket==-1)
		ERROR_EXIT("Failed to accept client");

	// Application goes here
	// ......
	send(new_socket, "Hello Theodora", 15*sizeof(char), 0);
	
	// Destroy server structure
	destroy_server(s);

	return 0;
}
