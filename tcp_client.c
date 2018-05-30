#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>

/*	Client Socket Workflow
	1. Socket;
	2. Connect;
	3. Send/Receive Message;
	4. Close Connection.*/

int main(int argc, char const *argv[]) {
	/* Declarando o descritor de socket do cliente. */	
	int client_socket;
	
	/* Criando o Client Socket (man socket). */
	client_socket = socket(DOMAIN, TYPE, PROTOCOL); // Testar se o socket foi criado com sucesso.

	/* Especificando o endereço do servidor (man 7 ip). */
	struct sockaddr_in server_adress;
	/* Preenchendo a estrutura do endereço do servidor. */
	server_adress.sin_family = ADRESS_FAMILY;
	server_adress.sin_port = PORT; // Usar htons(PORT).
	server_adress.sin_addr.s_addr = INTERNET_ADRESS;
	
	/* Estabelecendo uma conexão com o Socket Server (man connect). */
	int connection_status = connect(SOCKET, STRUCT SOCKET_ADDR, SOCKET_ADDR_SIZE); // Testar se a conexão foi estabelecida com sucesso.

	/* Recebendo dados do servidor (man recv). */
	char server_response[256];
	recv(SOCKET, BUFFER, BUFFER_SIZE, FLAG); // 

	/* Imprimindo na tela a resposta do servidor. */
	printf("Server response: %s\n", server_response);

	/* Encerrando a conexão do Socket. */
	close(SOCKET);

	return 0;
}