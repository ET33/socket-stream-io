#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

typedef struct node {
	struct node *next_n;
	int key;
	void *item;
} node;

struct queue {
	node *first_n;
	node *last_n;
	unsigned int size;
};

queue *q_init() {
	queue *q = malloc(sizeof(queue));
	if (q) {
		q->first_n = NULL;
		q->last_n = NULL;
		q->size = 0;
	}
	return q;
}

void q_destroy(queue *q) {
	if (q) {
		node *aux, *rem = q->last_n;
		while (rem != NULL) {
			aux = rem;
			rem = rem->next_n;
			free(aux);
		}
		free(q);
	}
}

void q_insert(queue *q, int key, void *item) {
	if (q) {
		node *new_n = malloc(sizeof(node));

		if (new_n == NULL)
			return;

		new_n->item = item;
		new_n->key = key;

		new_n->next_n = q->last_n;
		q->last_n = new_n;

		if (q->first_n == NULL)
			q->first_n = new_n;

		++q->size;
	}
}

void q_sort_insert(queue *q, int key, void *item) {
	if (q) {
		node *new_n = malloc(sizeof(node));

		if (new_n == NULL)
			return;

		new_n->item = item;
		new_n->key = key;
		new_n->next_n = NULL;

		node *cur = q->last_n, *prev = NULL;
		while (cur != NULL && cur->key > key) {
			prev = cur;
			cur = cur->next_n;
		}
		// cur is NULL or has a smaller or equal key
		// prev is NULL or has a greater key
		// Insert like: prev -> new_n -> cur
		if (prev != NULL) {
			new_n->next_n = prev->next_n;
			prev->next_n = new_n;
		} else {
			new_n->next_n = q->last_n;
			q->last_n = new_n;
		}

		if (cur == NULL) {
			if (q->first_n) {
				q->first_n->next_n = new_n;
			}
			q->first_n = new_n;
		}
		++q->size;
	}
}

void *q_pop(queue *q) {
	void *item = NULL;
	if (q && q->size > 0) {
		node *to_pop = q->last_n, *prev=NULL;

		while (to_pop != q->first_n) {
			prev = to_pop;
			to_pop = to_pop->next_n;
		}

		if (to_pop != NULL) {
			item = to_pop->item;
			q->first_n = prev;

			if (q->first_n == NULL)
				q->last_n = NULL;

			free(to_pop);

			--q->size;
		}
	}
	return item;
}

int q_size(queue *q) {
	if (q) {
		return q->size;
	}
	return 0;
}

int q_key_first(queue *q) {
	if (q && q->first_n) {
		return q->first_n->key;
	}
	return 0;
}

int q_key_last(queue *q) {
	if (q && q->last_n) {
		return q->last_n->key;
	}
	return 0;
}

// Queue debug

/*
#include <time.h>
int main(int argc, char * argv[]) {
	printf("Initing test...\n");
	queue *q=q_init();	

	srand(time(NULL));
	int *item, key;
	printf("Inserting test...\n");
	for (int i=0; i < 10; i++) {
		item = malloc(sizeof(int));
		*item = i;

		key = rand() % 100;

		printf("key: %d\tValue: %d\n", key, *item);
		q_sort_insert(q, key, item);
	}

	printf("Popping test...\n");
	while (q->size) {
		item = q_pop(q);
		printf("%d ", *item);
		free(item);
	}
	printf("\n");

	printf("Destroing test...\n");
	q_destroy(q);
	return 0;
}
*/
