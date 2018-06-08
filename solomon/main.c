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
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
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
        printf("Usage: %s PORT\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    getIP();
    unsigned short int PORT = atoi(argv[1]);    

    // Initialize server structure
    server * s = create_server(BUFFER_SIZE, PORT, SERVER_TYPE, PROTOCOL, host);

    // Bind server to the given PORT
    attach_server(s, PORT);

    printf("Server listening %s on port %d...\n", host, PORT);

    // Listen
    int ret = listen(s->fd, NUM_CONNECTIONS);
    if (ret)
        ERROR_EXIT("Failed to listen for connections");

    // Create client socket
    int new_socket = accept(s->fd, NULL, NULL);
    if (new_socket == -1) {
        ERROR_EXIT("Failed to accept client");
    } else {
        printf("Connection accepted.\n");
    }

    // Application goes here
    // ......	
    data_unit msg;
    do {
        printf("Digite a mensagem a ser enviada para o cliente\n");
        scanf("%[^\n]%*c", msg.description);	
        msg.id = MESSAGE;

        /* Enviando a msg para o cliente. */
        send(new_socket, &msg, sizeof(msg), 0);

        /* Recebendo a msg do cliente. */
        if (recv(new_socket, &msg, sizeof(msg), 0) == -1){
            printf("Error on receiving data from client\n");
            exit(EXIT_FAILURE);
        } else {
            printf("Client response: %s \n", msg.description);
        }
    } while(msg.id != EXIT);
        
    // Destroy server structure
    destroy_server(s);

    return 0;
}