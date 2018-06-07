#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <server.h>
#include <sys/socket.h>
#include <dataStructDefault.h>

server * create_server(unsigned int buffer_size, unsigned short int port,
                       int server_type, int protocol) {

    server * s = malloc(sizeof(server));
    if (s == NULL)
        ERROR_EXIT("Failed to init server structure");

    s->buffer = malloc(sizeof(char) * buffer_size);
    if (s->buffer == NULL)
        ERROR_EXIT("Failed to init buffer");
    
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
        ERROR_EXIT("Failed at server configuration");

    // Define the server address
    s->address.sin_family = server_type;
    s->address.sin_addr.s_addr = INADDR_ANY;
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
        ERROR_EXIT("Failed to bind server to specified PORT.");

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
