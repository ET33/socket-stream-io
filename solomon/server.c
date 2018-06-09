#include "server.h"

server *create_server(
	unsigned int buffer_size, 
	unsigned short int port, 
	int server_type, 
	int protocol, 
	char *ip_address) {

    server *s = malloc(sizeof(server));
    if (s == NULL)
        ERROR_EXIT(
		ANSI_COLOR_RED 
		"Failed to init server structure" 
		ANSI_COLOR_RESET);

    s->buffer = malloc(sizeof(char) * buffer_size);
    if (s->buffer == NULL)
        ERROR_EXIT(
		ANSI_COLOR_RED 
		"Failed to init buffer" 
		ANSI_COLOR_RESET);
    
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
        ERROR_EXIT(
		ANSI_COLOR_RED 
		"Failed at server configuration" 
		ANSI_COLOR_RESET);

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
        ERROR_EXIT(
		ANSI_COLOR_RED 
		"Failed to bind server to specified PORT." 
		ANSI_COLOR_RESET);

    return 1;
}

void destroy_server(server *s) {
	// Free memory allocated for all server structure
	if (s) {
		close(s->fd);

		if (s->buffer)
			free(s->buffer);
		    
		free(s);
	}
}

char *getIP() {
	int cont = 0;
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char *host = malloc(sizeof(char) * (1 + NI_MAXHOST));

	if (getifaddrs(&ifaddr) == -1) {
		ERROR_EXIT("Failed at getting IP.");
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		family = ifa->ifa_addr->sa_family;

		if (family == AF_INET) {
			s = getnameinfo(
				ifa->ifa_addr, 
				sizeof(struct sockaddr_in),
				host, 
				NI_MAXHOST, 
				NULL, 
				0, 
				NI_NUMERICHOST);

			if (s != 0) {
			printf(
				ANSI_COLOR_RED 
				"getnameinfo() failed: %s\n" 
				ANSI_COLOR_RESET, 
				gai_strerror(s));
			exit(EXIT_FAILURE);
			}			

		}

		cont++;
	}

	return host; 
}

