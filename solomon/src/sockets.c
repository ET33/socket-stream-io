#include "sockets.h"

socket_structure *create_socket(unsigned short int port, int server_type, int protocol, char *ip_address, int op) {
    socket_structure *socket_struct = malloc(sizeof(socket_structure));
    if (socket_struct == NULL)
        ERROR_EXIT(ANSI_COLOR_RED "Failed to init socket structure" ANSI_COLOR_RESET);

    /* Create socket file descriptor. */    
    if ((socket_struct->fd = socket(server_type, SOCK_STREAM, protocol)) == -1)
        ERROR_EXIT(ANSI_COLOR_RED "Failed to create socket" ANSI_COLOR_RESET);

    if (op == SERVER) {
	    /* Server configuration. */
	    int aux = 1;
	    int ret = setsockopt(socket_struct->fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &aux, sizeof(aux));
	    if (ret)
	        ERROR_EXIT(ANSI_COLOR_RED "Failed at server configuration" ANSI_COLOR_RESET);
	}

    /* Define the server address. */
    socket_struct->address.sin_family = server_type;
    socket_struct->address.sin_addr.s_addr = inet_addr(ip_address);
    socket_struct->address.sin_port = htons(port);

    return socket_struct;
}

int attach_server(socket_structure * socket_struct, unsigned short int port) {
    if (socket_struct == NULL)
        return 0;

    /* Bind server to the specified port. */
    int ret = bind(socket_struct->fd, (struct sockaddr *) &socket_struct->address, sizeof(struct sockaddr_in));

    /* From man: "On success, zero is returned.  
    On error, -1 is returned, and errno is set appropriately. "*/
    if (ret)
        ERROR_EXIT(ANSI_COLOR_RED "Failed to bind server to specified PORT." ANSI_COLOR_RESET);

    return 1;
}

void connect_server(socket_structure *client_socket, char *server_adress, char *port) {
	if (connect(client_socket->fd, (struct sockaddr *) &client_socket->address, sizeof(struct sockaddr_in)))
        ERROR_EXIT(ANSI_COLOR_RED "Failed to connect to server." ANSI_COLOR_RESET);
    else 
        printf(ANSI_COLOR_GREEN "Connection established with the server %s on port %s...\n" ANSI_COLOR_RESET, server_adress, port);
}

void destroy_socket(socket_structure *socket_struct) {
	/* Free memory allocated for all socket structure. */
	if (socket_struct == NULL) 
		return;

	close(socket_struct->fd);

	free(socket_struct);	
}

char *getIP() {	
	struct ifaddrs *ifaddr, *ifa;
	int family, socket_struct;
	char *host = malloc(sizeof(char) * (1 + NI_MAXHOST));

	if (getifaddrs(&ifaddr) == -1)
		ERROR_EXIT(ANSI_COLOR_RED "Failed at getting IP." ANSI_COLOR_RESET);	

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		family = ifa->ifa_addr->sa_family;

		if (family == AF_INET) {
			socket_struct = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (socket_struct != 0)
				ERROR2_EXIT(ANSI_COLOR_RED "getnameinfo() failed: %s\n" ANSI_COLOR_RESET, gai_strerror(socket_struct));											
		}
	}
	return host; 
}