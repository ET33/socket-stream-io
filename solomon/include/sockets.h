#ifndef __SOCKETS_H_
#define __SOCKETS_H_

/* Include section. */
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
/* There's also "data-struct-def.h" included
below because it depends on the BUFFER_SIZE
define constant. */

/* Define and typedef section. */
#define ERROR_EXIT(STR) ({perror(STR); exit(EXIT_FAILURE);})
#define ERROR2_EXIT(STR, ARGS) ({fprintf(stderr, STR, ARGS); exit(EXIT_FAILURE);})
#define BUFFER_SIZE 		 (1024*1024)
#define SERVER_TYPE 		AF_INET
#define NUM_CONNECTIONS		1
#define PROTOCOL 		0
#define ANSI_COLOR_RED		"\x1b[31m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_YELLOW	"\x1b[33m"
#define ANSI_COLOR_BLUE		"\x1b[34m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_CYAN		"\x1b[36m"
#define ANSI_COLOR_RESET	"\x1b[0m"
#define MIN(A,B) ((A) < (B) ? (A) : (B))

typedef struct {
    int fd;
    struct sockaddr_in address;
    char *buffer;
} socket_structure;

/* Included this lib here because it needs
to known the BUFFER_SIZE in order to work
correctly. */
#include "data-struct-def.h"

/* Function declaration section */
socket_structure *create_socket(unsigned short int port, int server_type, int protocol, char *ip_address, int op);
int attach_server(socket_structure *s, unsigned short int port);
void connect_server(socket_structure * socket_struct, char *server_adress, char *port);
void destroy_socket(socket_structure *s);
char *getIP();

#endif
