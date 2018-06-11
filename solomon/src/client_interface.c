#include "sockets.h"
#include "bytestream.h"
#include "client_interface.h"

/* Process data send by the server. */
void process_data(data_unit data) {
	/* Execute the required operation. */
	switch(data.control_id) {		
		case GREETINGS:
			printf(
				"Welcome to" 
				ANSI_COLOR_RED " Solomon" ANSI_COLOR_RESET 
				", a streaming socket audio player.\n");        
			printf(
				"Type " 
				ANSI_COLOR_YELLOW "HELP" ANSI_COLOR_RESET 
				" to see the command list.\n");
			break;

		case LIST:
			printf("%s\n", data.description);
			break;

		case MUSIC:
			/* Play the track. */			
			break;

		case MESSAGE_NOANS: /* <- that's designedly, don't change unless you know what you're doing. */
		case MESSAGE:
			printf("\n");
			printf(
				ANSI_COLOR_BLUE 
				"Server response:" 
				ANSI_COLOR_RESET " %s\n", 
				data.description);
			break;

		case EXIT: /* The server is shutting down. */
			printf("Disconnecting...\n");				    					
			break;

		default:
			if (strlen(data.description) > 1)
				printf("Invalid command.\n");
			break;
	}

}

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
	else if (strcasecmp(str[0], "PLAY") == 0 && i <= 1)
		msg.control_id = PLAY;
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
			printf("HELP - List all the commands supported by this program.\n");
			printf("LIST - List all the music tracks avaiable on the server. \n");
			printf("PLAY - Play the first music track available on the server. \n");
			printf("EXIT - Disconnect from the server and exit the program. \n");			
			break;

		case LIST:
			printf("Listing musics from the server... \n");
			break;

		case PLAY:			
		 	if (i == 2) {
		 		strcpy(msg.description, str[1]);			
		 		printf("Play Track #%s...\n", msg.description);
		 	} else {
		 		printf("Play Track first server music...\n");
		 	}
		 	break;

		case INVALID:
			if (strlen(str[0]) > 1) {
				printf("Invalid command.\n");
			}
			break;

		case EXIT:							
			printf("Disconnecting...\n");			
			break;

		default:
			printf("Invalid command.\n");
			break;
	}

	/* Temp memory free. */
	free(str);

	return msg;
}

void *recv_data(void *vargs) {
	client_args_struct *args = (client_args_struct *) vargs;

	do {    	
		/* Receiving data from the server. */
		if (recv(args->client_socket->fd, &args->msg, sizeof(args->msg), 0) == -1) {
		    ERROR_EXIT(
			ANSI_COLOR_RED 
			"Error on receiving data from server\n" 
			ANSI_COLOR_RESET);
		} else {
		    process_data(args->msg);
		}                       
		
		if(args->msg.control_id == EXIT) {
			args->process_end = 1;	
			pthread_exit(NULL);
		}
	} while (!args->process_end);

	return NULL;
}

void *send_data(void *vargs) {
	client_args_struct *args = (client_args_struct *) vargs;
	char *command = NULL;

	do {   	           
		if (args->msg.control_id != EXIT && 
			args->msg.control_id != MESSAGE_NOANS) {
			printf(
				ANSI_COLOR_MAGENTA 
				"Client response: " 
				ANSI_COLOR_RESET);
		}

		// Avoids buffer overflow
		command = readline(stdin);	
		if (command) {
			strncpy(
				args->msg.description, 
				command, 
				MIN(BUFFER_SIZE-1, strlen(command)));
			args->msg.description[BUFFER_SIZE-1] = '\0';
			free(command);
		}

		if (args->msg.description != NULL && 
			strlen(args->msg.description) > 1) {
			printf("antes\n");
			args->msg = process_commands(args->msg);
			printf("depois\n");
		} else {
			args->msg.control_id = INVALID;
		}
		
		/* Sending data to the server. */
		if (args->msg.control_id != INVALID && 
			args->msg.control_id != HELP) {
			send(
				args->client_socket->fd, 
				&args->msg, 
				sizeof(args->msg), 
				0);
		}

		if (args->msg.control_id == EXIT) {
			args->process_end = 1;
			pthread_exit(NULL);
		}

	} while (!args->process_end);

	return NULL;
}
