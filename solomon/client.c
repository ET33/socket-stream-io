#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dataStructDefault.h>

#define BUFFER_SIZE 1024
#define PORT 8080

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*	Client Socket Workflow
    1. Socket;
    2. Connect;
    3. Send/Receive Message;
    4. Close Connection.
*/

int main(int argc, char * const argv[]){
    if(argc != 3) {
        printf(ANSI_COLOR_YELLOW "Usage: %s IP_ADDRESS PORT\n" ANSI_COLOR_RESET, argv[0]);
        exit(EXIT_FAILURE);
    }

    // Criando socket
    int socket_client = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_client == -1) {
        printf(ANSI_COLOR_RED "Failed to create Socket\n" ANSI_COLOR_RESET);
        exit(EXIT_FAILURE);
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
        printf(ANSI_COLOR_RED "Failed to connect to server." ANSI_COLOR_RESET);
        exit(EXIT_FAILURE);
    } else {
        printf(ANSI_COLOR_GREEN "Conexão estabelecida com o servidor.\n" ANSI_COLOR_RESET);
    }

    // Application goes here
    // ......	
    data_unit msg;
    do {
        if (recv(socket_client, &msg, sizeof(msg), 0) == -1) {
            printf(ANSI_COLOR_RED "Error on receiving data from server\n" ANSI_COLOR_RESET);
            exit(EXIT_FAILURE);
        } else {
            printf(ANSI_COLOR_BLUE "Server response:" ANSI_COLOR_RESET " %s \n", msg.description);
        }
        
        printf(ANSI_COLOR_MAGENTA "Client response: " ANSI_COLOR_RESET);
        scanf("%[^\n]%*c", msg.description);
        msg.id = MESSAGE;

        /* Enviando msg para o servidor. */
        send(socket_client, &msg, sizeof(msg), 0);
    } while (msg.id != EXIT);

    // Fecha o socket
    close(socket_client);

    return 0;
} 