#ifndef __QUEUE_H_
#define __QUEUE_H_

// Typedef section
typedef struct queue queue;

// Function declaration
queue *q_init();
void q_destroy(queue *q);
void q_insert(queue *q, int key, void *item);
void q_sort_insert(queue *q, int key, void *item);
void *q_pop(queue *q);

#endif
