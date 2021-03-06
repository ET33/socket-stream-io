#include <pthread.h>
#include "sockets.h"
#include "queue.h"
#include "bytestream.h"

/*
    This module should take care of the bytestream
    exchange between "server" and "client". It should:
    -	Process data exchanged with the data_unit struct
    -	Organize the data accordingly to the id sequence
    -	Take care of the temporary audio microfiles
    - 	Play the sounds in the correct order


    In this module there is four functions, each one
    with a well-defined set of tasks. They work parallel
    from each other, and uses the same data structure to
    communicates in between.
    
    1. processSounds(...): 
    Creates this module structure, with a
    thread in detached (background) mode 
    for every other function below, and return 
    that structure in order to keep control of
    these threads.

    1.1. update_ready_queue(...):
    This function get all the data received via
    socket and translate it to this module
    data unit, storing two sorted queues:
    - ready_q: sorted and complete queue. Has
    the microaudio content in the correct order
    to be played.
    - aux_q: the purpose of this queue is just
    to support filling the ready_q correctly. 
    It is a sorted and incomplete queue of data 
    unit. If the first element of this queue is 
    the next to be placed in the ready_q, pop 
    element from aux_q and insert in ready_q.		

    1.2. process_ready_queue(..):
    This function has the task of collecting the
    nodes from que ready_q and creating a tempo-
    rary microaudio file for that piece of audio.

    1.3. play_microaudios(...):
    This function get all the microaudio files in
    the temporary microaudio subdirectory and uses
    a external program to play each one separatelly.

*/

/*
    How to integrate with the current Client-Server
    structure:

    Both client and server must call processSounds()
    just before the program main locking loop, giving 
    the correct parameters. This function should take 
    care of the rest without extern interference.
*/

/*
    Gets a file data line.
*/
char *readline(FILE *fp) {
    // 128 bytes initally for the file line is
    // a heuristic to speed up the process. Is is
    // application dependent.
    const unsigned int initial_size = 128;

    char *line = malloc(sizeof(char) * (1 + initial_size)),
         c = 0;
    unsigned long int counter = 0,
                      cur_size = initial_size;
    
    while (c != EOF && c != '\r' && c != '\n') {
        c = fgetc(fp);

        // Double the line size. Again, it is a heuristic to
        // speed up the process.
        if (counter == cur_size-1) {
            cur_size *= 2;
            line = realloc(line, sizeof(char) * (1 + cur_size));
        }
        
        line[counter] = c;
        counter++;
    }

    if (counter > 1) {
        line = realloc(line, sizeof(char) * (1 + counter));
        line[counter - 1] = '\0';
        return line;
    }

    free(line);
    return NULL;
}

/*
    Invokes "ls" command on the given directory
    and return a list with the filenames
*/
char **get_file_list(char *directory, unsigned long int *size) {
    char ls_command[] = "/bin/ls ";
    char *command = malloc(sizeof(char) * (1 + strlen(ls_command) + strlen(directory)));
    if (command == NULL) return NULL;

    // Concatenates the ls command with the given directory
    // filepath
    sprintf(command, "%s%s", ls_command, directory);

    char **microaudio_list = NULL,
         *aux = NULL;
    *size = 0;

    FILE *fp = popen(command, "r");
    if (fp != NULL) {
        while (!feof(fp)) {
            // Get a "ls" output line (file name)
            aux = readline(fp);
            if (aux) {
                microaudio_list = realloc(microaudio_list, sizeof(char *) * (1 + *size));
                microaudio_list[*size] = aux;
                (*size)++;
            }
        }

        pclose(fp);
    }

    free(command);
    return microaudio_list;
}

/*
    This function (invoked within a thread) has
    the task of processing the ready_q, creating
    temporary microaudio files.
*/
static void *process_ready_queue(void *vargs) {
	/*
		This function (openned within a thread) has the
		task of processing the ready_q, creating the
		temporary microaudio files.
	*/

	args_struct *args = (args_struct *) vargs;
	queue *ready_q = args->ready_q;
	int *process_end = args->process_end;
	char *temp_dir_path = args->temp_dir_path;
	
	char *microaudio = NULL, *microaudio_filepath = NULL;
	FILE *temp_f = NULL;

	const unsigned long int temp_dir_path_len = strlen(temp_dir_path);

	// This counter will keep track of the current
	// microaudio id, in order to create the correct
	// microaudio filenames. The counter overflow is 
	// not a worry.
	// Just for quick reference, the max value of this
	// thing is 18,446,744,073,709,551,615.
	unsigned long long int microaudio_counter = 0;

	// Repeat til program process runs off
	while (!(*process_end)) {
  
		// If there's something to create another 
		// temporary microaudio file...
		if (q_size(ready_q)) {
			microaudio = q_pop(ready_q);
			if (microaudio) {
				microaudio_filepath = malloc(sizeof(char) *
					(1 + temp_dir_path_len + MAX_COUNTER_LEN));

				// Concatenate temp_dir_path and microaudio_counter
				// on microaudio_filepath buffer
				sprintf(microaudio_filepath, "%s%s", 
					temp_dir_path, FINAL_FILE_NAME);

				// If there's a microaudio, create a temporary
				// microaudio file for it.
				temp_f = fopen(microaudio_filepath, "a");

				// Write microaudio content on the selected file
				fwrite(microaudio, sizeof(char), sizeof(data_unit), temp_f);

				// Free system used memory
				fclose(temp_f);
				free(microaudio_filepath);
				free(microaudio);
				microaudio_counter++;
			}
		}
	}

	return NULL;
}

/*
    This function (invoked within a thread) has the
    task to fill the ready_q with the correct data
    blocks.
*/
void *update_ready_queue(void *vargs) {

    args_struct *args = (args_struct *) vargs;
    queue *ready_q = args->ready_q;
    data_unit *cur_data_unit = args->cur_data_unit;
    //int *process_end = args->process_end;
    int key = 0;

    /*
        aux_q(ueue) -> A sorted and incomplete queue that
        appends the cur_data_unit and delivers
        the correct next data block to the ready_q.
    */
    queue *aux_q = args->aux_q;

    // Variable used to store the audio filepaths
    // given through the cur_data_unit inside the aux_q.
    char *bytestream_data = NULL;

    // Repeat til program process ends
    //while(!(*process_end)) {
        //if (cur_data_unit->control_id == MUSIC) {
            // It's necessary to transfer the content
            // of the cur_data_unit->description to
            // a dynamic memory region because the
            // cur_data_unit memory region will be
            // soon cleaned up at the end of this "if".
            bytestream_data = malloc(sizeof(char) * (1 + strlen(cur_data_unit->description)));

            strcpy(bytestream_data, cur_data_unit->description);
            
            q_sort_insert(aux_q, cur_data_unit->id, bytestream_data);
            
            // Expecting that "cur_data_unit"
            // isn't dinamically allocated.
            // cur_data_unit->control_id = INVALID;

            // Caution: >>DON'T FREE<< bytestream_data!
            // Its dynamic memory region is
            // stored in the aux_q!
            bytestream_data = NULL;
        //}
        

        // If there's something to process...
        if (q_size(aux_q)) {
            /*
                If the first key at the aux_q is the next data unit,
                append it to the ready_q.

                [...][n+k][n] --(?)--> 
                [n-1][n-2][...][1] --q_pop()--> 
                    PlaySound()
            */
            if (q_size(ready_q) == 0 || q_key_first(aux_q) == 1 + q_key_last(ready_q)) {
                bytestream_data = q_pop(aux_q);
                key = q_key_first(aux_q);
                q_insert(ready_q, key, bytestream_data);
            }
        }
    //}

    // q_destroy(aux_q);
    
    return NULL;
}


void destroy_sound_struct (sound_struct *ss) {
    if (ss) {

        if ((ss->thread_id)[PLAY_MICROAUDIOS])
            pthread_exit(ss->thread_id + PLAY_MICROAUDIOS);

        if ((ss->thread_id)[UPDATE_READY_QUEUE])
            pthread_exit(ss->thread_id + UPDATE_READY_QUEUE);

        if ((ss->thread_id)[PROCESS_READY_QUEUE])
            pthread_exit(ss->thread_id + PROCESS_READY_QUEUE);

        if (ss->args.ready_q)
            q_destroy(ss->args.ready_q);

        free(ss);
    }
}

/*
    This function creates the temporary subdirectory
    created to keep microaudios.
*/
void create_temp_microaudio_dir(char *path) {
    char *command = malloc(sizeof(char) * (2*strlen(path) + 17 + 1));
    sprintf(command, "rm -r %s; mkdir -p %s", path, path);
    system(command);
    free(command);
}

/*    
    This function removes the temporary subdirectory
    created to keep microaudios.
*/
void remove_temp_microaudio_dir(char *path) {
    char inst[] = "rm -rf ";
    char *command = malloc(sizeof(char) * (strlen(path) + strlen(inst) + 1));
    strcpy(command, inst);
    strcat(command, path);
    system(command);
    free(command);
}

/*
    ready_q(ueue) -> A sorted and complete queue that
    has the filepaths of the sounds to be played in order.
*/
sound_struct *processSounds(data_unit *cur_data_unit, int *process_end, char *temp_dir_path, int play_audio) {
    sound_struct *ss = malloc(sizeof(sound_struct));

    // This struct will be used to pass
    // arguments to threads.
    ss->args.ready_q = q_init();
    ss->args.aux_q = q_init();
    ss->args.cur_data_unit = cur_data_unit;
    ss->args.process_end = process_end;
    ss->args.temp_dir_path = temp_dir_path;

    // Create threads
    pthread_create(ss->thread_id + PROCESS_READY_QUEUE, NULL, process_ready_queue, (void *) &(ss->args));

    pthread_detach((ss->thread_id)[PROCESS_READY_QUEUE]);
    //pthread_detach((ss->thread_id)[UPDATE_READY_QUEUE]);

    return ss;
}

