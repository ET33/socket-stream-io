#ifndef __SERVER_RD_H_
#define __SERVER_RD_H_

// Include section
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
#include "data-struct-def.h"



// Define and typedef section
#define ERROR_EXIT(STR) ({perror(STR); exit(EXIT_FAILURE);})
#define BUFFER_SIZE 		1024
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

typedef struct {
    int fd;
    struct sockaddr_in address;
    char *buffer;
} server;




// Function declaration section
server *create_server(
	unsigned int buffer_size, 
	unsigned short int port, 
	int server_type, 
	int protocol, 
	char *ip_address);

int attach_server(
	server * s, 
	unsigned short int port);

void destroy_server(server * s);

char *getIP();

#endif
