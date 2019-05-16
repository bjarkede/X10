#pragma once

#include <stdlib.h>
//#include <cassert>

typedef struct bdeque bdeque_type;
typedef char unsigned bdeque_valtype;

// A node in our datastructure is described like this
// It points to the next and previous elements and
// contains the value val.
struct node {
	struct node *next;
	struct node *prev;
	bdeque_valtype val;
};

// Our double ended queue has a pointer to the head and tail element.
struct bdeque {
	struct node *head;
	struct node *tail;
};

bdeque_type * bdeque_alloc(); // Creates a new bdeque_type, returns pointer to memory
void bdeque_free(bdeque_type *d); // Free memory

// This functions checks whether the datastructure is empty or not

bool bdeque_is_empty(bdeque_type *d);
int bdeque_size(bdeque_type *d);
void bdeque_resize(bdeque_type *d, int v);

// These functions inserts data into our datastucture 

void bdeque_push_front(bdeque_type *d, bdeque_valtype v);
void bdeque_push_back(bdeque_type *d, bdeque_valtype v);

// All functions below this comment act upon our deque, and returns some value.

bdeque_valtype bdeque_pop_front(bdeque_type *d);
bdeque_valtype bdeque_pop_back(bdeque_type *d);
bdeque_valtype bdeque_peek_front(bdeque_type *d);
bdeque_valtype bdeque_peek_back(bdeque_type *d);

