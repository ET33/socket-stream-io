#include "sockets.h"
#include "bytestream.h"

/* '0' stands for IP:
Check out /etc/protocol with cat command. */

socket_structure *server_socket;
int new_socket;
sound_struct *ss;
int process_end = 0;
pthread_t recv_thread, send_thread;

data_unit process_commands(data_unit msg) {
	char **str = (char **) malloc(sizeof(char **));
    const char delim[2] = " ";
    char *token = NULL;

    /* Get the first token. */
    token = strtok(msg.description, delim);

    /* Walk through other tokens. */
    int i = 0;
    for(i = 0; token != NULL; i++) {
        str = (char **) realloc(str, sizeof(char **) * (i + 1));
        str[i] =  token;        
        token = strtok(NULL, delim);        
    }

    /* Case insensitive compare. */
    if (strcasecmp(str[0], "HELP") == 0 && i == 1) {
    	msg.control_id = HELP;
    } else if (strcasecmp(str[0], "PATH") == 0 && i == 2) {
    	msg.control_id = PATH;
    } else if (strcasecmp(str[0], "NUSERS") == 0 && i == 1) {
    	msg.control_id = NUSERS;
    } else if (strcasecmp(str[0], "EXIT") == 0 && i == 1) {
    	msg.control_id = EXIT;
    } else {
    	msg.control_id = INVALID;
    }

    switch (msg.control_id) {
    	case HELP:
    		printf("List of commands.\n");
    		printf("PATH PATH_MUSICS - Define the path of musics folder. \n");
    		printf("NUSER - Number of clients connected. \n");
    		printf("EXIT - Disconnect the server and exit the program. \n");
    		break;

    	case PATH:
    		/* Get the path from the user. */
    		printf("Current path of musics: /\n");    		
    		break;

    	case NUSERS:
    		printf("2 Clients Connected on the server\n");
    		break;

    	case EXIT:
    		printf("Disconnecting...\n");			
			break;

    	default:
    		printf("Invalid command, Type 'help' for command list. \n");
    		break;
    }

    return msg;
}

void process_data(data_unit data){
	/* Execute the required operation. */
	switch(data.control_id) {
		case PLAY:
			/* Pegar o nome da música que está tocando e enviar para o cliente. */
			strcpy(data.description, "Playing...");
			data.control_id = MESSAGE;
			send(new_socket, &data, sizeof(data), 0);
			data.control_id = MUSIC;
			break;

		case LIST:
			/* Dar um ls e enviar para o client a lista de musicas, exemplo
				[0] Author1 - Musica1
				[1] Author2 - Musica2 */
			/* Coloca o resultado em description e enviar de volta para o client. */
			//strcpy(data.description, "LISTING MUSICS\n");
			//send(new_socket, &data, sizeof(data), 0);			
			strcpy(data.description, "Music List");
			data.control_id = MESSAGE;
			send(new_socket, &data, sizeof(data), 0);			
			break;

		case STOP:
			/* Para de enviar a música para o cliente. */	
			strcpy(data.description, "Server stopped sending music.");
			data.control_id = MESSAGE;
			send(new_socket, &data, sizeof(data), 0);			
			break;		

		case EXIT:
			strcpy(data.description, "Thanks for using Theodora Music Stream!");
			data.control_id = MESSAGE;
			send(new_socket, &data, sizeof(data), 0);
			data.control_id = EXIT;
			send(new_socket, &data, sizeof(data), 0);
			break;

		default:
			strcpy(data.description, "Invalid operation!");
			data.control_id = MESSAGE;
			send(new_socket, &data, sizeof(data), 0);			
			break;
	}
}

void *recv_data(void *args) {
	data_unit msg = *((data_unit *) args);

	do {    	
		/* Recebendo a msg do cliente. */
		if (recv(new_socket, &msg, sizeof(msg), 0) == -1)
			ERROR_EXIT(ANSI_COLOR_RED "Error on receiving data from client" ANSI_COLOR_RESET);
		else {
			//printf(ANSI_COLOR_CYAN "Client response: " ANSI_COLOR_RESET "%s \n", msg.description);
			process_data(msg);
		}
        
		if(msg.control_id == EXIT) {
			process_end = 1;	
			pthread_exit(&recv_thread);
		}
    } while (!process_end);

    return NULL;
}

void *send_data(void *args) {
	data_unit msg = *((data_unit *) args);

	do {   	           
		printf(ANSI_COLOR_RED "Server response: " ANSI_COLOR_RESET);
		msg.control_id = MESSAGE;
		scanf("%[^\n]%*c", msg.description);	
		if (msg.description[0] == '\0')
			getchar();
		else 
			msg = process_commands(msg);

		/* Enviando a msg para o cliente. */
		if (msg.control_id != INVALID && msg.control_id != HELP)
			send(new_socket, &msg, sizeof(msg), 0);            			
		
		if(msg.control_id == EXIT) {
			process_end = 1;	
			pthread_exit(&send_thread);
		}
    } while (!process_end);

    return NULL;
}

int main(int argc, char * const argv[]) {
	if (argc != 2)
		ERROR2_EXIT(ANSI_COLOR_YELLOW "Usage: %s port\n" ANSI_COLOR_RESET, argv[0]);

	char *host = getIP();
	unsigned short int port = atoi(argv[1]);	

	/* Initialize server structure. */
	server_socket = create_socket(BUFFER_SIZE, port, SERVER_TYPE, PROTOCOL, host, SERVER);

	/* Bind server to the given port. */
	attach_server(server_socket, port);

	/* Listen. */	
	printf(ANSI_COLOR_GREEN "Server listening" ANSI_COLOR_YELLOW " %s" ANSI_COLOR_GREEN " on port" ANSI_COLOR_YELLOW " %d" ANSI_COLOR_GREEN"...\n" ANSI_COLOR_RESET, host, port);
	if (listen(server_socket->fd, NUM_CONNECTIONS))
		ERROR_EXIT(ANSI_COLOR_RED "Failed to listen for connections" ANSI_COLOR_RESET);
	
	/* Create client socket. */	
	if ((new_socket = accept(server_socket->fd, NULL, NULL)) == -1)
		ERROR_EXIT(ANSI_COLOR_RED "Failed to accept client" ANSI_COLOR_RESET);
	else
		printf(ANSI_COLOR_GREEN "Connection accepted.\n" ANSI_COLOR_RESET);

	/* Application section. */
	data_unit msg = {0};
	msg.control_id = MESSAGE;
	msg.id = INVALID;	
	
	/* Calling the audio processing function */
	ss = processSounds(&msg, &process_end);

	printf("Welcome to " ANSI_COLOR_CYAN "Theodora" ANSI_COLOR_RESET " music server socket stream!\nType" ANSI_COLOR_YELLOW " HELP " ANSI_COLOR_RESET "for command list.\n");

	msg.control_id = GREETINGS;
	send(new_socket, &msg, sizeof(msg), 0);

	/* Free host IP memory. */
	if (host)
		free(host);

	/* Making asynchronous communication. */    
    pthread_create(&recv_thread, NULL, recv_data, (void *) &msg);
    pthread_create(&send_thread, NULL, send_data, (void *) &msg);
	pthread_join(recv_thread, NULL);    

	/* Destroy server structure. */
	destroy_socket(server_socket);
	destroy_sound_struct(ss);

	return 0;
}
