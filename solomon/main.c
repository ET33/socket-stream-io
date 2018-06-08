#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <server.h>
#include <dataStructDefault.h>

#define BUFFER_SIZE 1024
#define SERVER_TYPE AF_INET
#define NUM_CONNECTIONS 1
#define PROTOCOL 0

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char host[NI_MAXHOST];

void getIP() {
    int cont = 0;
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                                           host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf(ANSI_COLOR_RED "getnameinfo() failed: %s\n" ANSI_COLOR_RESET, gai_strerror(s));
                exit(EXIT_FAILURE);
            }            
        }
        cont++;
    }        
}

// '0' stands for IP
// Check out /etc/protocol

int main(int argc, char * const argv[]) {
    if(argv[1] == NULL) {
        printf(ANSI_COLOR_YELLOW "Usage: %s PORT\n" ANSI_COLOR_RESET, argv[0]);
        exit(EXIT_FAILURE);
    }

    getIP();
    unsigned short int PORT = atoi(argv[1]);    

    // Initialize server structure
    server * s = create_server(BUFFER_SIZE, PORT, SERVER_TYPE, PROTOCOL, host);

    // Bind server to the given PORT
    attach_server(s, PORT);

    printf(ANSI_COLOR_GREEN "Server listening" ANSI_COLOR_YELLOW " %s" ANSI_COLOR_GREEN " on port" ANSI_COLOR_YELLOW " %d"ANSI_COLOR_GREEN"...\n" ANSI_COLOR_RESET, host, PORT);

    // Listen
    int ret = listen(s->fd, NUM_CONNECTIONS);
    if (ret)
        ERROR_EXIT(ANSI_COLOR_RED "Failed to listen for connections" ANSI_COLOR_RESET);

    // Create client socket
    int new_socket = accept(s->fd, NULL, NULL);
    if (new_socket == -1) {
        ERROR_EXIT(ANSI_COLOR_RED "Failed to accept client" ANSI_COLOR_RESET);
    } else {
        printf(ANSI_COLOR_GREEN "Connection accepted.\n" ANSI_COLOR_RESET);
    }

    // Application goes here
    // ......	
    data_unit msg;
    do {
        printf(ANSI_COLOR_RED "Server response: " ANSI_COLOR_RESET);
        scanf("%[^\n]%*c", msg.description);	
        msg.id = MESSAGE;
        
        /* Enviando a msg para o cliente. */
        send(new_socket, &msg, sizeof(msg), 0);

        /* Recebendo a msg do cliente. */
        if (recv(new_socket, &msg, sizeof(msg), 0) == -1){
            printf(ANSI_COLOR_RED "Error on receiving data from client\n" ANSI_COLOR_RESET);
            exit(EXIT_FAILURE);
        } else {
            printf(ANSI_COLOR_CYAN "Client response: " ANSI_COLOR_RESET "%s \n", msg.description);
        }
    } while(msg.id != EXIT);
        
    // Destroy server structure
    destroy_server(s);

    return 0;
}