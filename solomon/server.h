#ifndef __SERVER_H_
#define __SERVER_H_

// Include section
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

// Define and typedef section
#define ERROR_EXIT(STR) {perror(STR); exit(EXIT_FAILURE);}

typedef struct {
    int fd;
    struct sockaddr_in address;
    char *buffer;
} server;

server *create_server(unsigned int buffer_size, unsigned short int port, int server_type, int protocol, char *ip_address);
int attach_server(server * s, unsigned short int port);
void destroy_server(server * s);

#endif