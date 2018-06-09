#include "sockets.h"

void *hear(void *arguments) {
    int socket_server = *((int *)arguments);
    data_unit msg;

    do {
        if (recv(socket_server, &msg, sizeof(msg), 0) == -1) {
            printf(ANSI_COLOR_RED "Error on receiving data from client\n" ANSI_COLOR_RESET);
            exit(EXIT_FAILURE);
        } else {
            printf(ANSI_COLOR_CYAN "Client response: " ANSI_COLOR_RESET "%s \n", msg.description);
            *msg.description = '\0';
        }
    } while (1);

    return NULL;
}

void *talk(void *arguments) {
    socket_structure *client_socket = (socket_structure *) arguments;
    data_unit msg;
    msg.id = MESSAGE;
    msg.destination = client_socket->target_fd;

    do {
        printf(ANSI_COLOR_RED "Your message: " ANSI_COLOR_RESET);
        scanf("%[^\n]%*c", msg.description);	
        send(client_socket->fd, &msg, sizeof(msg), 0);
        *msg.description = '\0';
    } while (1);

    return NULL;
}

int main(int argc, char * const argv[]){
    if (argc != 3) 
        ERROR2_EXIT(ANSI_COLOR_YELLOW "Usage: %s IP_ADDRESS PORT\n" ANSI_COLOR_RESET, argv[0]);
    
    /* Criando o socket client e especificando o endereço do servidor. */
    socket_structure *client_socket = create_socket(BUFFER_SIZE, atoi(argv[2]), SERVER_TYPE, PROTOCOL, argv[1], CLIENT);

    /* Estabelece conexão com o socket server. */
    connect_server(client_socket, argv[1], argv[2]);
    
    if (recv(client_socket->fd, &client_socket->target_fd, sizeof(int), 0) == -1) 
        ERROR_EXIT(ANSI_COLOR_RED "Could not retrieve target socket.\n" ANSI_COLOR_RESET);

    printf("server socket: %d\n target_socket: %d\n", client_socket->fd, client_socket->target_fd);

    /* Application section. */
    pthread_t talk_thread, hear_thread;

    pthread_create(&talk_thread, NULL, talk, &client_socket);
    pthread_create(&hear_thread, NULL, hear, &client_socket->fd);

    pthread_join(talk_thread, NULL);

    /* Encerra a conexão do socket. */
    destroy_socket(client_socket);    

    return 0;
}