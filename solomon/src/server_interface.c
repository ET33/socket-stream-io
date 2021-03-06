#include "sockets.h"
#include "bytestream.h"
#include "server_interface.h"

void break_file(void *vargs, char *filepath) {
	server_args_struct *args = (server_args_struct *) vargs;
	FILE *audio_file;
	unsigned long long int fsize;

	printf("Trying to open: %s\n", filepath);
	audio_file = fopen(filepath, "rb");

	if (!audio_file) {
		printf("Could not open file\n");
		return;
	}

	fseek(audio_file, 0, SEEK_END);
	fsize = ftell(audio_file);
	rewind(audio_file);

	unsigned int n_of_data_units = fsize / BUFFER_SIZE;

	unsigned int data_unit_counter;
	data_unit **msgs = malloc(sizeof(data_unit *) * (n_of_data_units + 1));
	printf(ANSI_COLOR_BLUE "Total of %u packages to be send to client...\n" ANSI_COLOR_RESET, n_of_data_units);

	int bytes_read;
	
	for (data_unit_counter = 0; data_unit_counter <= n_of_data_units; data_unit_counter++) {
		msgs[data_unit_counter] = malloc(sizeof(data_unit));

		if(data_unit_counter == 0) {
			msgs[0]->control_id = START;
			strcpy(msgs[0]->description, filepath);
			send(args->client_socket, msgs[0], sizeof(data_unit), 0);
		}

		// 1 means 'one byte'
		bytes_read = fread(msgs[data_unit_counter]->description, 1, BUFFER_SIZE, audio_file);
		// send(args->client_socket, msgs[data_unit_counter], sizeof(data_unit), 0);

		if (bytes_read < BUFFER_SIZE)
			msgs[data_unit_counter]->description[bytes_read] = '\0';		
		msgs[data_unit_counter]->control_id = MUSIC;
		msgs[data_unit_counter]->id = data_unit_counter;

		sleep(1);
		send(args->client_socket, msgs[data_unit_counter], sizeof(data_unit), 0);	
		printf(ANSI_COLOR_GREEN "Package send - id %d\n" ANSI_COLOR_RESET, msgs[data_unit_counter]->id);	
	}


	fclose(audio_file);

	// msgs[0]->control_id = PLAY;
	// send(args->client_socket, msgs[0], sizeof(data_unit), 0);
}

void *server_send_data_units(void *vargs) {
	server_args_struct *args = (server_args_struct *) vargs;

	char *music = (char *) malloc(100);
	sprintf(music, "%s/%s", args->music_dir, "Spektrem - Shine [NCS Release].mp3");
	//sprintf(music, "%s/%s", args->music_dir, "Cartoon - On & On (feat. Daniel Levi)  [NCS Release].mp3");
	printf("%s\n", music);

	break_file(args, music);

	//for (register unsigned int i = 0; i < messages->number_of_data_units; i++)
	//	send(args->client_socket, messages->msgs[i], sizeof(data_unit), 0);						

	return NULL;
}

data_unit process_commands(data_unit msg, char *music_path) {
	unsigned long int number_of_files;	
	char **list_of_files = get_file_list(music_path, &number_of_files);

	char **str = malloc(sizeof(char **));
	char *token = NULL;
	char delim[] = " ,;\t";

	/* Get the first token. */
	token = strtok(msg.description, delim);

	/* Walk through other tokens. */
	register unsigned int i;
	for (i = 0; token != NULL; i++) {
		str = realloc(str, sizeof(char **) * (i + 1));
		str[i] =  token;
		token = strtok(NULL, delim);
	}

	/* Case insensitive compare. */
	if (strcasecmp(str[0], "HELP") == 0 && i == 1)
		msg.control_id = HELP;
	else if (strcasecmp(str[0], "EXIT") == 0 && i == 1)
		msg.control_id = EXIT;
	else if (strcasecmp(str[0], "PATH") == 0 && i == 1)
		msg.control_id = PATH;
	else if (strcasecmp(str[0], "LIST") == 0 && i == 1)
		msg.control_id = LIST;
	else
		msg.control_id = INVALID;
	
	switch (msg.control_id) {
		case HELP:
			printf("List of commands.\n");
			printf("PATH - Print the path of musics folder. \n");
			printf("LIST - List all the music tracks avaiable on the folder. \n");
			printf("EXIT - Disconnect the server and exit the program. \n");
			break;

		case PATH:
			printf("Musics folder path: %s\n", music_path);
			msg.control_id = INVALID;
			break;

		case LIST:
			printf("Music List:\n");
			for (register unsigned int i = 0; i < number_of_files; i++)
				printf("[%d] %s\n", i, list_of_files[i]);		
			msg.control_id = INVALID;
			break;	

		case EXIT:
			printf("Disconnecting...\n");
			break;

		default:
			printf("Invalid command, Type 'help' for command list. \n");
			break;
	}

	/* Temp memory free. */
	free(str);

	return msg;
}

void process_data(server_args_struct *args){
	/* Execute the required operation. */
	unsigned long int number_of_files;
	char **list_of_files;
	//pthread_t send_data_thread;
	char aux[10];

	switch(args->msg_recv.control_id) {
		case PLAY:
			sleep(1);
			strcpy(args->msg_send.description, "Playing...");
			args->msg_send.control_id = MESSAGE;
			send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);	
			server_send_data_units((void *) args);
			//pthread_create(&send_data_thread, NULL, server_send_data_units, (void *) args);
			sleep(1);
			args->msg_send.control_id = PLAY;
			send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);	
			break;

		case LIST:
			list_of_files = get_file_list(args->music_dir, &number_of_files);
			strcpy(args->msg_send.description, "Music List:");
			for (register unsigned int i = 0; i < number_of_files; i++) {
				strcat(args->msg_send.description, "\n");
				sprintf(aux, "[%d] ", i);
				strcat(args->msg_send.description, aux);
				strcat(args->msg_send.description, list_of_files[i]);
			}
			args->msg_send.control_id = MESSAGE;
			send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);
			break;

		case STOP:
			/* Para de enviar a música para o cliente. */
			strcpy(args->msg_send.description, "Server stopped sending music.");
			args->msg_send.control_id = MESSAGE;
			send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);
			break;

		case EXIT:
			strcpy(args->msg_send.description, "Thanks for using Theodora Music Stream!\n");
			args->msg_send.control_id = MESSAGE_NOANS;
			send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);
			args->msg_send.control_id = EXIT;
			send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);
			break;

		default:
			if (strlen(args->msg_recv.description) > 0) {
				strcpy(args->msg_send.description, "Invalid operation!");
				args->msg_send.control_id = MESSAGE;
			} else {
				args->msg_send.control_id = INVALID;
			}
			send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);
			break;
	}

}

void *server_recv_data(void *vargs) {
	server_args_struct *args = (server_args_struct *) vargs;

	do {
		/* Recebendo a msg do cliente. */
		if (recv(args->client_socket, &args->msg_recv, sizeof(data_unit), 0) == -1)
			ERROR_EXIT(ANSI_COLOR_RED "Error on receiving data from client" ANSI_COLOR_RESET);
		else {
			process_data(args);
		}

		if(args->msg_send.control_id == EXIT) {
			args->process_end = 1;
			pthread_exit(NULL);
		}

	} while (!args->process_end);

	return NULL;
}

void *server_send_data(void *vargs) {
	server_args_struct *args = (server_args_struct *) vargs;
	char *command = NULL;

	do {
		if (args->msg_send.control_id != EXIT && args->msg_send.control_id != MESSAGE_NOANS) {
			printf(ANSI_COLOR_RED "Server response: " ANSI_COLOR_RESET);
		}

		// Avoids buffer overflow
		command = readline(stdin);
		*args->msg_send.description = '\0';
		if (command) {
			unsigned long int command_size = MIN(BUFFER_SIZE-1, strlen(command));
			strncpy(args->msg_send.description, command, command_size);
			args->msg_send.description[command_size] = '\0';
			free(command);
		}

		if (*args->msg_send.description != '\0' && strlen(args->msg_send.description) > 0) {
				args->msg_send = process_commands(args->msg_send, args->music_dir);
		} else {
				args->msg_send.control_id = INVALID;
		}

		/* Enviando a msg_send para o cliente. */
		if (args->msg_send.control_id != INVALID && args->msg_send.control_id != HELP)
			send(args->client_socket, &args->msg_send, sizeof(args->msg_send), 0);

		if (args->msg_send.control_id == EXIT) {
			args->process_end = 1;
			pthread_exit(NULL);
		}

	} while (!args->process_end);

	return NULL;
}
