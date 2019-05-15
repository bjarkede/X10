#pragma once

#include <stdlib.h>
//#include <cassert>

typedef struct bdeque bdeque_type;
typedef char unsigned bdeque_valtype;

bdeque_type * bdeque_alloc(); // Creates a new bdeque_type, returns pointer to memory
void bdeque_free(bdeque_type *d); // Free memory

// This functions checks whether the datastructure is empty or not

bool bdeque_is_empty(bdeque_type *d);
int bdeque_size(bdeque_type *d);

// These functions inserts data into our datastucture 

void bdeque_push_front(bdeque_type *d, bdeque_valtype v);
void bdeque_push_back(bdeque_type *d, bdeque_valtype v);

// All functions below this comment act upon our deque, and returns some value.

bdeque_valtype bdeque_pop_front(bdeque_type *d);
bdeque_valtype bdeque_pop_back(bdeque_type *d);
bdeque_valtype bdeque_peek_front(bdeque_type *d);
bdeque_valtype bdeque_peek_back(bdeque_type *d);

