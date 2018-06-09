#include "sockets.h"
#include "bytestream.h"

int main(int argc, char * const argv[]){
    if (argc != 3) 
        ERROR2_EXIT(ANSI_COLOR_YELLOW "Usage: %s IP_ADDRESS PORT\n" ANSI_COLOR_RESET, argv[0]);
    
    /* Criando o socket client e especificando o endereço do servidor. */
    socket_structure *client_socket = create_socket(BUFFER_SIZE, atoi(argv[2]), SERVER_TYPE, PROTOCOL, argv[1], CLIENT);

    /* Estabelece conexão com o socket server. */
    connect_server(client_socket, argv[1], argv[2]);
    
    /* Application section. */
    data_unit msg;
    do {
        if (recv(client_socket->fd, &msg, sizeof(msg), 0) == -1) 
            ERROR_EXIT(ANSI_COLOR_RED "Error on receiving data from server\n" ANSI_COLOR_RESET);
        else 
            printf(ANSI_COLOR_BLUE "Server response:" ANSI_COLOR_RESET " %s\n", msg.description);
                
        printf(ANSI_COLOR_MAGENTA "Client response: " ANSI_COLOR_RESET);

        scanf("%[^\n]%*c", msg.description);
        msg.id = MESSAGE;

        /* Enviando msg para o servidor. */
        send(client_socket->fd, &msg, sizeof(msg), 0);
    } while (msg.id != EXIT);

    /* Encerra a conexão do socket. */
    destroy_socket(client_socket);    

    return 0;
}
