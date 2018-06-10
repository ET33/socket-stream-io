#include <pthread.h>
#include "sockets.h"
#include "queue.h"

/*
 * This modulus should take care of the bytestream
 * exchange between "server" and "client". It should:
 * -	Process data exchanged with the data_unit struct
 * -	Organize the data accordingly to the id sequence
 * -	Take care of the temporary audio microfiles (to do)
 * - 	Play the sounds in the correct order (to do)
 */

/*
	How to integrate with the current Client-Server
	structure:

	Both client and server must call processSounds()
	just before the program main locking loop, giving 
	the correct parameters. This function should take 
	care of the rest without extern interference.
*/

typedef struct {
	queue *ready_q;
	data_unit *cur_data_unit;
	int *process_end;
} args_struct;

static void *process_ready_queue(void *vargs) {
	/*
		This function (openned within a thread) has the
		task of processing the ready_q.
	*/

	args_struct *args = (args_struct *) vargs;
	queue *ready_q = args->ready_q;
	int *process_end = args->process_end;
	
	char *command = NULL, *filepath;
	char aplayCommand[] = "aplay ";
	const unsigned long int aplaySize = strlen(aplayCommand);

	// Repeat til program process runs off
	while (!(*process_end)) {
		
		// If there's something to play...
		if (q_size(ready_q)) {
			filepath = q_pop(ready_q);
			if (filepath) {
				// "aplay " size 
				// + filepath size 
				// + null terminator character
				command = malloc(sizeof(char) * (aplaySize + strlen(filepath) + 1));


				// Concatenate "aplay " with given filepath
				strcpy(command, aplayCommand);
				strcat(command + aplaySize, filepath); 

				// Play sound on the given filepath
				// Check if command correspondent audio
				// microfile exists and play it right here
				// (to do!)
				//syscall(command);

				printf("(consider playing audio now) test: %s\n", command);

				// Free system used memory
				free(filepath);
				free(command);
			}
		}
	}

	return NULL;
}

static void *update_ready_queue(void *vargs) {
	/*
		This function (openned within a thread) has the
		task to fill the ready_q with the correct data
		blocks.
	*/

	args_struct *args = (args_struct *) vargs;
	queue *ready_q = args->ready_q;
	data_unit *cur_data_unit = args->cur_data_unit;
	int *process_end = args->process_end;
	int key = 0;

	/*
		aux_q(ueue) -> A sorted and incomplete queue that
			appends the cur_data_unit and delivers
			the correct next data block to the ready_q.
	*/
	queue *aux_q = q_init();

	// Variable used to store the audio filepaths
	// given through the cur_data_unit inside the aux_q.
	char *filepath = NULL;

	// Repeat til program process ends
	while(!(*process_end)) {
		if (cur_data_unit) {
			// It's necessary to transfer the content
			// of the cur_data_unit->description to
			// a dynamic memory region because the
			// cur_data_unit memory region will be
			// soon cleaned up at the end of this "if".
			filepath = malloc(sizeof(char) * (1 + strlen(cur_data_unit->description)));

			strcpy(filepath, cur_data_unit->description);
			
			q_sort_insert(aux_q, cur_data_unit->id, filepath);
			
			// Expecting that "cur_data_unit"
			// isn't dinamically allocated.
			cur_data_unit = NULL;
			filepath = NULL;
		}
		
		/*
		If the first key at the aux_q is the next data unit,
		append it to the ready_q.

		[...][n+k][n] --(?)--> 
			[n-1][n-2][...][1] --q_pop()--> 
				PlaySound()
		*/
		if (q_size(ready_q) == 0 || q_key_first(aux_q) == 1 + q_key_last(ready_q)) {
			filepath = q_pop(aux_q);
			key = q_key_first(aux_q);
			q_insert(ready_q, key, filepath);
		}
	}

	q_destroy(aux_q);
	
	return NULL;
}

void processSounds(data_unit *cur_data_unit, int *process_end) {
	// Thread vector
	pthread_t thread_id[2];

	/*
		ready_q(ueue) -> A sorted and complete queue that
			has the filepaths of the sounds to be
	 		played in order.
	*/
	queue *ready_q = q_init();

	// This struct will be used to pass
	// arguments to threads.
	args_struct args = {
		.ready_q = ready_q,
		.cur_data_unit = cur_data_unit,
		.process_end = process_end
	};

	// Create threads
	pthread_create(thread_id + 0, NULL, process_ready_queue, (void *) &args);
	pthread_create(thread_id + 1, NULL, update_ready_queue, (void *) &args);

	// Join (wait) threads terminate
	pthread_join(thread_id[0], NULL);
	pthread_join(thread_id[1], NULL);
	
	q_destroy(ready_q);
}

