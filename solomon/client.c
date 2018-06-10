#include "sockets.h"
#include "bytestream.h"

/* Process the command inserted by the client. */
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
	if (strcasecmp(str[0], "HELP") == 0 && i == 1) 
		msg.control_id = HELP;		
	else if (strcasecmp(str[0], "LIST") == 0 && i == 1)
		msg.control_id = LIST;
	else if (strcasecmp(str[0], "PLAY") == 0 && i <= 2)
		msg.control_id = PLAY;
	else if (strcasecmp(str[0], "STOP") == 0 && i == 1)
		msg.control_id = STOP;
	else if (strcasecmp(str[0], "EXIT") == 0 && i == 1)
		msg.control_id = EXIT;
	else 
		msg.control_id = INVALID;

	/* Cleaning the buffer. */
	msg.description[0] = '\0';
	
	/* Execute the required operation. */
	switch(msg.control_id) {
		case HELP:
			printf("List of controls. \n");
			printf("HELP - \n");
			printf("LIST - \n");
			printf("PLAY - \n");
			printf("STOP - \n");
			printf("EXIT - \n");			
			break;

		case LIST:
			printf("Listing musics from the server... \n");
			break;

		case PLAY:			
			if (i == 2) {
				strcpy(msg.description, str[1]);			
				printf("Play Track %s\n", msg.description);
			} else {
				printf("Play Track 0\n");
			}
			break;

		case STOP:
			/* Parar o player. */
			printf("Player Stopped. \n");
			break;

		case INVALID:
			printf("Invalid command.\n");
			break;

		case EXIT:							
			printf("Disconnecting...\n");	
			break;

		default:
			printf("Invalid command.\n");
			break;
	}

	/* Temp memory free. */
	// if(str != NULL) {
	// 	int j;
	// 	for(j = 0; j < i; j++) 
	// 		if(str[j] != NULL)
	// 			free(str[j]);
	// 	free(str);
	// }
	
	return msg;
}

int main(int argc, char * const argv[]){
    if (argc != 3) 
		ERROR2_EXIT(ANSI_COLOR_YELLOW "Usage: %s IP_ADDRESS PORT\n" ANSI_COLOR_RESET, argv[0]);
    
    /* Criando o socket client e especificando o endereço do servidor. */
    socket_structure *client_socket = create_socket(BUFFER_SIZE, atoi(argv[2]), SERVER_TYPE, PROTOCOL, argv[1], CLIENT);

    /* Estabelece conexão com o socket server. */
    connect_server(client_socket, argv[1], argv[2]);
    
    /* Application section. */
    data_unit msg = {0};
    msg.control_id = MESSAGE;
    msg.id = INVALID;
    int process_end = 0;

    /* Calling the audio processing function */
    sound_struct *ss = processSounds(&msg, &process_end);

    printf("Welcome to" ANSI_COLOR_RED " Solomon" ANSI_COLOR_RESET ", a streaming socket audio player.\n");        
    printf("Type " ANSI_COLOR_YELLOW "HELP" ANSI_COLOR_RESET " to see the list of commands.\n");

    do {
        if (recv(client_socket->fd, &msg, sizeof(msg), 0) == -1) 
            ERROR_EXIT(ANSI_COLOR_RED "Error on receiving data from server\n" ANSI_COLOR_RESET);
        else 
            printf(ANSI_COLOR_BLUE "Server response:" ANSI_COLOR_RESET " %s\n", msg.description);
               
        printf(ANSI_COLOR_MAGENTA "Client response: " ANSI_COLOR_RESET);
        scanf("%[^\n]%*c", msg.description);
        msg = process_commands(msg);
        
        /* Enviando msg para o servidor. */
        if(msg.control_id != INVALID && msg.control_id != HELP)
        	send(client_socket->fd, &msg, sizeof(msg), 0);
    } while (!process_end && msg.control_id != EXIT);

    /* Encerra a conexão do socket. */
    destroy_socket(client_socket);    
    destroy_sound_struct(ss);

    return 0;
}