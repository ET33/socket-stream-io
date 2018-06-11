#include "sockets.h"
#include "bytestream.h"

typedef struct {
	socket_structure *client_socket;
	sound_struct *ss;
	int process_end;
	pthread_t recv_thread, send_thread;
	data_unit msg;
} client_args_struct;

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

		case MESSAGE:
			printf("\n");
			printf(ANSI_COLOR_BLUE "Server response:" ANSI_COLOR_RESET " %s\n", data.description);
			printf(ANSI_COLOR_MAGENTA "Client response: " ANSI_COLOR_RESET);			
			fflush(stdout);
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
	// else if (strcasecmp(str[0], "STOP") == 0 && i == 1)
		// msg.control_id = STOP;
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
			//printf("PlAY NR_TRACK - Play a specific music by a valid track number. \n");
			// printf("STOP - Stop the player from playing the music and the server from sending it.\n");
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
		if (args->msg.control_id != EXIT) {
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
			args->msg = process_commands(args->msg);
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

int main(int argc, char * const argv[]){
	if (argc != 3) {
		ERROR2_EXIT(
			ANSI_COLOR_YELLOW 
			"Usage: %s IP_ADDRESS PORT\n" 
			ANSI_COLOR_RESET, 
			argv[0]);
	}

	client_args_struct args = {0};

	/* Criando o socket client e especificando o endereço do servidor. */
	args.client_socket = create_socket(
		BUFFER_SIZE, 
		atoi(argv[2]), 
		SERVER_TYPE, 
		PROTOCOL, 
		argv[1], 
		CLIENT);

	/* Estabelece conexão com o socket server. */
	connect_server(args.client_socket, argv[1], argv[2]);

	// To do: concatenate this default path to a UNIQUE ID
	// for each client
	create_temp_microaudio_dir(TEMP_CLIENT_DIR);

	/* Application section. */
	args.msg.control_id = MESSAGE;
	args.msg.id = INVALID;    

	/* Calling the audio processing function */
	args.ss = processSounds(
		&args.msg, 
		&args.process_end, 
		TEMP_CLIENT_DIR, 
		1);

	/* Making asynchronous communication. */    
	pthread_create(
		&args.recv_thread, 
		NULL, 
		recv_data, 
		(void *) &args);

	pthread_create(
		&args.send_thread, 
		NULL, 
		send_data, 
		(void *) &args);

	/* Join threads */
	pthread_join(args.recv_thread, NULL);    
	pthread_join(args.send_thread, NULL);    

	/* Encerra a conexão do socket. */
	destroy_socket(args.client_socket);    
	destroy_sound_struct(args.ss);

	return 0;
}
