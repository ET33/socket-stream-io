#include "sockets.h"
#include "bytestream.h"
#include "server_interface.h"

file_units_struct *break_file(char *filepath) {
	file_units_struct *messages = malloc(sizeof(file_units_struct));
	FILE *audio_file;
	unsigned long long int fsize;

	printf("Trying to open: %s\n", filepath);
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

	int bytes_read;

	for (data_unit_counter = 0;
		data_unit_counter <= n_of_data_units;
		data_unit_counter++) {

		msgs[data_unit_counter] = malloc(sizeof(data_unit));
		msgs[data_unit_counter]->control_id = MUSIC;
		msgs[data_unit_counter]->id = data_unit_counter;
		// 1 means 'one byte'
		bytes_read = fread(msgs[data_unit_counter]->description,
			1, BUFFER_SIZE, audio_file);

		printf("bytes read: %u\n", bytes_read);

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

	char *teste = (char *) malloc(100);
	sprintf(teste, "%s/%s", args->music_dir, "Spektrem - Shine [NCS Release].mp3");

	file_units_struct *messages = break_file(teste);

	send(
		args->client_socket,
		messages->msgs[0],
		sizeof(data_unit),
		0);

	// for (register unsigned int i = 0; i < messages->number_of_data_units; i++) {
	// 	args->msg_send = *messages->msgs[i];
	// 	printf("\nMessage id: %d\nControl id: %d\n",
	// 		args->msg_send.id,
	// 		args->msg_send.control_id);
	// 		//, args->msg_send.description);
	// 	send(
	// 		args->client_socket,
	// 		&args->msg_send,
	// 		sizeof(data_unit),
	// 		0);
	// 	sleep(1);
	// }

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
	char **list_of_files;
	pthread_t send_data_thread;

	switch(args->msg_recv.control_id) {
		case PLAY:
			strcpy(args->msg_send.description, "Playing...");
			args->msg_send.control_id = MESSAGE;
			// send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);

			pthread_create(
				&send_data_thread,
				NULL,
				server_send_data_units,
				(void *) args);

			break;

		case LIST:
			list_of_files = get_file_list(
				args->music_dir,
				&number_of_files);
			strcpy(args->msg_send.description, "Music List:");
			for (register unsigned int i = 0; i < number_of_files; i++) {
				strcat(args->msg_send.description, "\n");
				strcat(args->msg_send.description, list_of_files[i]);
			}
			args->msg_send.control_id = MESSAGE;
			// send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);
			break;

		case STOP:
			/* Para de enviar a música para o cliente. */
			strcpy(args->msg_send.description, "Server stopped sending music.");
			args->msg_send.control_id = MESSAGE;
			// send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);
			break;

		case EXIT:
			strcpy(args->msg_send.description, "Thanks for using Theodora Music Stream!\n");
			args->msg_send.control_id = MESSAGE_NOANS;
			// send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);
			args->msg_send.control_id = EXIT;
			// send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);
			break;

		default:
			if (strlen(args->msg_recv.description) > 1) {
				strcpy(args->msg_send.description, "Invalid operation!");
				args->msg_send.control_id = MESSAGE;
			} else {
				args->msg_send.control_id = INVALID;
			}
			// send(args->client_socket, &args->msg_send, sizeof(data_unit), 0);
			break;
	}

}

void *server_recv_data(void *vargs) {
	server_args_struct *args = (server_args_struct *) vargs;

	do {
		/* Recebendo a msg do cliente. */
		if (recv(args->client_socket, &args->msg_recv, sizeof(data_unit), 0) == -1)
			ERROR_EXIT(
				ANSI_COLOR_RED
				"Error on receiving data from client"
				ANSI_COLOR_RESET);
		else {
			if (args->msg_recv.control_id != INVALID) {
				printf("id %d\ncontrol_id: %d\ndescription: %s\n", args->msg_recv.id, args->msg_recv.control_id, args->msg_recv.description);
				process_data(args);
			}
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
		if (args->msg_send.control_id != EXIT &&
			args->msg_send.control_id != MESSAGE_NOANS) {
			printf(
				ANSI_COLOR_RED
				"Server response: "
				ANSI_COLOR_RESET);
		}

		// Avoids buffer overflow
		command = readline(stdin);
		if (command) {
				strncpy(
						args->msg_send.description,
						command,
						MIN(BUFFER_SIZE-1, strlen(command)));
				args->msg_send.description[BUFFER_SIZE-1] = '\0';
				free(command);
		}

		if (args->msg_send.description != NULL &&
				strlen(args->msg_send.description) > 1) {
				args->msg_send = process_commands(args->msg_send);
		} else {
				args->msg_send.control_id = INVALID;
		}

		/* Enviando a msg_send para o cliente. */
		if (args->msg_send.control_id != INVALID &&
			args->msg_send.control_id != HELP)
			send(args->client_socket, &args->msg_send, sizeof(args->msg_send), 0);

		if (args->msg_send.control_id == EXIT) {
			args->process_end = 1;
			pthread_exit(NULL);
		}

	} while (!args->process_end);

	return NULL;
}

