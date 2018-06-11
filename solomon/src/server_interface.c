#include "sockets.h"
#include "bytestream.h"
#include "server_interface.h"

file_units_struct *break_file(char *filepath) {
	file_units_struct *messages = malloc(sizeof(file_units_struct));
	FILE *audio_file;
	unsigned long long int fsize;

	audio_file = fopen(filepath, "rb");

	if (!audio_file) {
		printf("Could not open file\n");
		return NULL;
	}

	fseek(audio_file, 0, SEEK_END);
	fsize = ftell(audio_file);
	rewind(audio_file);

	unsigned int n_of_data_units = fsize / BUFFER_SIZE;
	unsigned int data_unit_counter;
	data_unit **msgs = malloc(sizeof(data_unit *) * 
		(n_of_data_units + 1));

	for (data_unit_counter = 0; 
		data_unit_counter <= n_of_data_units; data_unit_counter++) {
		static int bytes_read;

		msgs[data_unit_counter] = malloc(sizeof(data_unit));
		msgs[data_unit_counter]->control_id = MUSIC;
		msgs[data_unit_counter]->id = data_unit_counter;
		// 1 means 'one byte'
		bytes_read = fread(msgs[data_unit_counter]->description, 
			1, BUFFER_SIZE, audio_file);

		if (bytes_read < BUFFER_SIZE) {
		    msgs[data_unit_counter]->description[bytes_read] = '\0';
		}
	}

	fclose(audio_file);

	messages->msgs = msgs;
	messages->number_of_data_units = data_unit_counter;

	return messages;
}

void *server_send_data_units(void *vargs) {
	server_args_struct *args = (server_args_struct *) vargs;

	file_units_struct *messages = break_file(args->filepath);
	for (register unsigned int i = 0; 
		i < messages->number_of_data_units; i++) {
		send(
			args->server_socket->fd, 
			messages->msgs[i], 
			sizeof(data_unit), 
			0);
	}

	return NULL;
}

data_unit process_commands(data_unit msg) {
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
	if (strcasecmp(str[0], "HELP") == 0 && i == 1) {
		msg.control_id = HELP;
	} else if (strcasecmp(str[0], "EXIT") == 0 && i == 1) {
		msg.control_id = EXIT;
	} else {
		msg.control_id = INVALID;
	}

	switch (msg.control_id) {
		case HELP:
			printf("List of commands.\n");
			printf("EXIT - Disconnect the server and exit the program. \n");
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

void process_data(server_args_struct *args){
	/* Execute the required operation. */
	unsigned long int number_of_files;
	data_unit data = args->msg;

	char **list_of_files = get_file_list(
		args->music_dir, 
		&number_of_files);

	pthread_t thread_ids[number_of_files];

	switch(data.control_id) {
		case PLAY:
			for (register unsigned int i = 0; i < number_of_files; i++) {
				pthread_create(thread_ids + i, NULL, server_send_data_units, list_of_files[i]);				
			}
			strcpy(data.description, "Playing...");
			data.control_id = MESSAGE;
			send(args->new_socket, &data, sizeof(data), 0);
			data.control_id = MUSIC;
			break;

		case LIST:
			strcpy(data.description, "Music List:");
			for (register unsigned int i = 0; i < number_of_files; i++) {
				strcat(data.description, "\n");
				strcat(data.description, list_of_files[i]);
			}
			data.control_id = MESSAGE;
			send(args->new_socket, &data, sizeof(data), 0);			
			break;

		case STOP:
			/* Para de enviar a música para o cliente. */	
			strcpy(data.description, "Server stopped sending music.");
			data.control_id = MESSAGE;
			send(args->new_socket, &data, sizeof(data), 0);			
			break;		

		case EXIT:
			strcpy(data.description, "Thanks for using Theodora Music Stream!\n");
			data.control_id = MESSAGE_NOANS;
			send(args->new_socket, &data, sizeof(data), 0);
			data.control_id = EXIT;
			send(args->new_socket, &data, sizeof(data), 0);
			break;

		default:
			if (strlen(data.description) > 1) {
				strcpy(data.description, "Invalid operation!");
				data.control_id = MESSAGE;
			} else {
				data.control_id = INVALID;
			}
			send(args->new_socket, &data, sizeof(data), 0);			
			break;
	}

	for (register unsigned int i = 0; i < number_of_files; i++)
		free(list_of_files[i]);
	free(list_of_files);
}

void *server_recv_data(void *vargs) {
	server_args_struct *args = (server_args_struct *) vargs;

	do {    	
		/* Recebendo a msg do cliente. */
		if (recv(args->new_socket, &args->msg, sizeof(args->msg), 0) == -1)
			ERROR_EXIT(
				ANSI_COLOR_RED 
				"Error on receiving data from client" 
				ANSI_COLOR_RESET);
		else {
			process_data(args);
		}
        
		if(args->msg.control_id == EXIT) {
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
                if (args->msg.control_id != EXIT &&
                        args->msg.control_id != MESSAGE_NOANS) {
			printf(
				ANSI_COLOR_RED 
				"Server response: " 
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

		/* Enviando a msg para o cliente. */
		if (args->msg.control_id != INVALID && 
			args->msg.control_id != HELP)
			send(args->new_socket, &args->msg, sizeof(args->msg), 0);            			
		
		if (args->msg.control_id == EXIT) {
			args->process_end = 1;	
			pthread_exit(NULL);
		}

	} while (!args->process_end);

	return NULL;
}

