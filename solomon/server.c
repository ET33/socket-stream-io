#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <server.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dataStructDefault.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

server *create_server(unsigned int buffer_size, unsigned short int port, int server_type, int protocol, char *ip_address) {
    server * s = malloc(sizeof(server));
    if (s == NULL)
        ERROR_EXIT(ANSI_COLOR_RED "Failed to init server structure" ANSI_COLOR_RESET);

    s->buffer = malloc(sizeof(char) * buffer_size);
    if (s->buffer == NULL)
        ERROR_EXIT(ANSI_COLOR_RED "Failed to init buffer" ANSI_COLOR_RESET);
    
    // Create server file descriptor
    s->fd = socket(server_type, SOCK_STREAM, protocol);

    // Server configuration
    int aux = 1;
    int ret = setsockopt(s->fd,
                         SOL_SOCKET,
                         SO_REUSEADDR | SO_REUSEPORT,
                         &aux,
                         sizeof(aux));
    if (ret)
        ERROR_EXIT(ANSI_COLOR_RED "Failed at server configuration" ANSI_COLOR_RESET);

    // Define the server address
    s->address.sin_family = server_type;
    s->address.sin_addr.s_addr = inet_addr(ip_address);
    s->address.sin_port = htons(port);

    return s;
}

int attach_server(server * s, unsigned short int port) {
    if (s == NULL)
        return 0;

    // Bind server to the specified port
    int ret = bind(s->fd,
                   (struct sockaddr *) &s->address,
                   sizeof(struct sockaddr_in));

    // From man: "On success, zero is returned.  
    // On error, -1 is returned, and errno is set appropriately. "
    if (ret)
        ERROR_EXIT(ANSI_COLOR_RED "Failed to bind server to specified PORT." ANSI_COLOR_RESET);

    return 1;
}

void destroy_server(server * s) {
    if (s) {
        close(s->fd);

        if (s->buffer)
            free(s->buffer);
            
        free(s);
    }
}
