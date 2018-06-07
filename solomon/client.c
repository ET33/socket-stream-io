#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dataStructDefault.h>

#define BUFFER_SIZE 1024
#define PORT 8080

/*	Client Socket Workflow
    1. Socket;
    2. Connect;
    3. Send/Receive Message;
    4. Close Connection.
*/

int main(int argc, char * const argv[]){

    // Criando socket
    int socket_client = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_client == -1) {
        printf("Failed to create Socket\n");
        exit(EXIT_FAILURE);
    }

    //especificando endereco do servidor
    struct sockaddr_in server_adress;
    server_adress.sin_family = AF_INET;
    server_adress.sin_port = htons(PORT);
    server_adress.sin_addr.s_addr = INADDR_ANY;

    // estabelece conexao com o socket server
    int connection_status = connect(socket_client,
                                    (struct sockaddr *) &server_adress,
                                    sizeof(server_adress));

    if (connection_status) {
        printf("Failed to connect to server.");
        exit(EXIT_FAILURE);
    } else {
        printf("Conexão estabelecida com o servidor.\n");
    }

    // Application goes here
    // ......	
    data_unit msg;
    do {
        if (recv(socket_client, &msg, sizeof(msg), 0) == -1) {
            printf("Error on receiving data from server\n");
            exit(EXIT_FAILURE);
        } else {
            printf("Server response: %s \n", msg.description);
        }
        
        printf("Digite a mensagem a ser enviada para o servidor\n");
        scanf("%s%*c", msg.description);
        msg.id = MESSAGE;

        /* Enviando msg para o servidor. */
        send(socket_client, &msg, sizeof(msg), 0);
    } while (msg.id != EXIT);

    // Fecha o socket
    close(socket_client);

    return 0;
} 