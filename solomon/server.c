#include "sockets.h"

/* '0' stands for IP:
Check out /etc/protocol with cat command. */

typedef struct {
    int *socket_list;
    int number_of_sockets;
} socket_connections;

typedef struct {
    socket_connections **rooms;
    int number_of_rooms;
} socket_rooms;

socket_connections *init_socket_connections() {
    socket_connections *sc = (socket_connections *) malloc(sizeof(socket_connections));
    sc->socket_list = NULL;
    sc->number_of_sockets = 0;
    return sc;
}

socket_rooms *init_socket_rooms() {
    socket_rooms *sr = (socket_rooms *) malloc(sizeof(socket_rooms));
    sr->rooms = NULL;
    sr->number_of_rooms = 0;
    return sr;
}

void create_room(socket_rooms *all_rooms) {
    all_rooms->rooms = (socket_connections **) realloc(all_rooms->rooms, sizeof(socket_connections *) * (all_rooms->number_of_rooms+1));
    all_rooms->rooms[all_rooms->number_of_rooms++] = init_socket_connections();

}

void add_new_socket(socket_connections *sc, int new_socket) {
    sc->socket_list = (int *) realloc(sc->socket_list, sizeof(int) * (sc->number_of_sockets+1));
    sc->socket_list[sc->number_of_sockets++] = new_socket;
}

void *listen_socket(void *arguments) {
    int client_socket = *((int *) arguments);
    data_unit msg;

    do {
        if (recv(client_socket, &msg, sizeof(msg), 0) == -1) {
            printf(ANSI_COLOR_RED "Error on receiving data from client\n" ANSI_COLOR_RESET);
            exit(EXIT_FAILURE);
        } else {
            // redirect the msg to destination client
            send(msg.destination, &msg, sizeof(msg), 0);
        }
    } while (1);

    return NULL;
}

int main(int argc, char * const argv[]) {
    if (argc != 2)
        ERROR2_EXIT(ANSI_COLOR_YELLOW "Usage: %s port\n" ANSI_COLOR_RESET, argv[0]);

    char *host = getIP();
    unsigned short int port = atoi(argv[1]);

    /* Initialize server structure. */
    socket_structure *server_socket = create_socket(BUFFER_SIZE, port, SERVER_TYPE, PROTOCOL, host, SERVER);

    /* Bind server to the given port. */
    attach_server(server_socket, port);

    printf(ANSI_COLOR_GREEN "Server listening" ANSI_COLOR_YELLOW " %s" ANSI_COLOR_GREEN " on port" ANSI_COLOR_YELLOW " %d" ANSI_COLOR_GREEN"...\n" ANSI_COLOR_RESET, host, port);

    socket_connections *all_sockets = init_socket_connections();
    socket_rooms *all_rooms = init_socket_rooms();

    pthread_t *t_ids = NULL;
    int number_of_threads = 0;

    /* Application section. */
    do {
        /* Listen. */
        if (listen(server_socket->fd, NUM_CONNECTIONS))
            ERROR_EXIT(ANSI_COLOR_RED "Failed to listen for connections" ANSI_COLOR_RESET);

        /* Accept Client */
        int new_socket = accept(server_socket->fd, NULL, NULL);
        if (new_socket == -1)
            ERROR_EXIT(ANSI_COLOR_RED "Failed to accept client" ANSI_COLOR_RESET);
        else
            printf(ANSI_COLOR_GREEN "Connection accepted.\n" ANSI_COLOR_RESET);

        t_ids = (pthread_t *) realloc(t_ids, sizeof(pthread_t) * (number_of_threads + 1));
        pthread_create(&t_ids[number_of_threads++], NULL, listen_socket, &new_socket);

        add_new_socket(all_sockets, new_socket);

        if (all_sockets->number_of_sockets % 2 == 0) {
            create_room(all_rooms);

            int socket1 = all_sockets->socket_list[all_sockets->number_of_sockets-2],
                socket2 = all_sockets->socket_list[all_sockets->number_of_sockets-1];

            add_new_socket(all_rooms->rooms[all_rooms->number_of_rooms-1], socket1);
            add_new_socket(all_rooms->rooms[all_rooms->number_of_rooms-1], socket2);
            send(socket1, &socket2, sizeof(int), 0);
            send(socket2, &socket1, sizeof(int), 0);
        }
    } while (1);

    free(all_sockets->socket_list);
    free(t_ids);
    int i;
    for (i = 0; i < all_rooms->number_of_rooms; i++) {
        free(all_rooms->rooms[i]);
    }
    /* Free host IP memory. */
    if (host)
        free(host);

    /* Destroy server structure. */
    destroy_socket(server_socket);

    return 0;
}
