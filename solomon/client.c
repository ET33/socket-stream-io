#include "server.h"
#include "queue.h"

int main(int argc, char * const argv[]){
    if(argc != 3) {
        printf(
		ANSI_COLOR_YELLOW 
		"Usage: %s IP_ADDRESS PORT\n" 
		ANSI_COLOR_RESET, 
		argv[0]);
        exit(EXIT_FAILURE);
    }

    // Criando socket
    int socket_client;
    if ((socket_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        ERROR_EXIT(
		ANSI_COLOR_RED 
		"Failed to create Socket" 
		ANSI_COLOR_RESET);
    }

    //especificando endereco do servidor
    struct sockaddr_in server_adress;
    server_adress.sin_family = AF_INET;
    server_adress.sin_port = htons(atoi(argv[2]));
    server_adress.sin_addr.s_addr = inet_addr(argv[1]);

    // estabelece conexao com o socket server
    int connection_status = connect(socket_client,
                                    (struct sockaddr *) &server_adress,
                                    sizeof(server_adress));

    if (connection_status) {
        ERROR_EXIT (ANSI_COLOR_RED 
		"Failed to connect to server."
		ANSI_COLOR_RESET);
    } else {
        printf(	ANSI_COLOR_GREEN 
		"Connection established with the "
		"server %s on %s...\n" 
		ANSI_COLOR_RESET, 
		argv[1], argv[2]);
    }

    // Application goes here
    data_unit msg;
    do {
        if (recv(socket_client, &msg, sizeof(msg), 0) == -1) {
            ERROR_EXIT(
		ANSI_COLOR_RED 
		"Error on receiving data from server\n" 
		ANSI_COLOR_RESET);
        } else {
            printf(
		ANSI_COLOR_BLUE 
		"Server response:" 
		ANSI_COLOR_RESET " %s\n", 
		msg.description);
        }
        
        printf(
		ANSI_COLOR_MAGENTA 
		"Client response: " 
		ANSI_COLOR_RESET);

        scanf("%[^\n]%*c", msg.description);
        msg.id = MESSAGE;

        /* Enviando msg para o servidor. */
        send(socket_client, &msg, sizeof(msg), 0);
    } while (msg.id != EXIT);

    // Fecha o socket
    close(socket_client);

    return 0;
} 
