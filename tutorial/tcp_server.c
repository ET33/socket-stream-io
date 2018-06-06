#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>

/*	Server Socket Workflow
	1. Socket;
	2. Bind;
	3. Listen;
	4. Accept;
	3. Send/Receive Message;
	4. Close Connection.*/

int main(int argc, char const *argv[]) {
	char server_message[256] = "Welcome to Theodora!";

	/* Declarando o descritor de socket do servidor. */
	int server_socket;

	/* Criando o Server Socket (man socket). */
	server_socket = socket(DOMAIN, TYPE, PROTOCOL); // Testar se o socket foi criado com sucesso.

	/* Especificando o endereço do servidor (man 7 ip). */
	struct sockaddr_in server_adress;
	/* Preenchendo a estrutura do endereço do servidor. */
	server_adress.sin_family = ADRESS_FAMILY;
	server_adress.sin_port = PORT; // Usar htons(PORT).
	server_adress.sin_addr.s_addr = INTERNET_ADRESS;

	/* Vincula o nosso socket com o IP e a porta especificados (man bind). */
	bind(SOCKET, STRUCT STRUCT SOCKET_ADDR, SOCKET_ADDR_SIZE); // Testar se o bind foi feito com sucesso.

	/* Colocando o server socket na escuta por conexões (man listen). */
	listen(SOCKET, BACKLOG);

	/* Aceitando a conexão de um client socket (man accept). */
	int client_socket;
	client_socket = accept(SOCKET, STRUCT CLIENT_SOCKET_ADDR, CLIENT_SOCKET_ADDR_SIZE);
	
	/* Enviando mensagem para o cliente (man send). */
	send(CLIENT_SOCKET, SERVER_MESSAGE, SERVER_MESSAGE_SIZE, FLAG);

	/* Encerrando a conexão do Socket. */
	close(SOCKET);

	return 0;
}