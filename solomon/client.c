#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

/*	Client Socket Workflow
	1. Socket;
	2. Connect;
	3. Send/Receive Message;
	4. Close Connection.*/

int main(int argc, char * const argv[]){
	// Criando socket
	int socket_client = socket(AF_INET, SOCK_STREAM, 0);

	//especificando endereco do servidor

	struct sockaddr_in server_adress;
	server_adress.sin_family = AF_INET;
	server_adress.sin_port = htons(PORT);
	server_adress.sin_addr.s_addr = INADDR_ANY;

	// estabelece conexao com o socket server
	int connection_status = connect(socket_client, (const struct sockaddr*) &server_adress, sizeof(server_adress));
	if (connection_status) {
		printf("Failed to connect to server.");
		exit(EXIT_FAILURE);
	}
		

	// recebendo dados do servidor
	char server_response[1024];
	recv(socket_client, server_response, sizeof(server_response), 0);

	// Imprimindo na tela a resposta do servidor
	printf("Server response, %s \n", server_response);

	close(socket_client);
	

	return 0;
} 
