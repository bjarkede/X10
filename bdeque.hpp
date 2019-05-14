#ifndef _BDEQUE_H
#define _BDEQUE_H

typedef struct bdeque bdeque_typeann;
typedef int bdeque_val_typeann;

bdeque_typeann * bdeque_alloc(); // Creates a new bdeque_type, returns pointer to memory
void bdeque_free(bdeque_typeann *d); // Free memory

// This functions checks whether the datastructure is empty or not

bool bdeque_is_empty(bdeque_typeann *d);

// These functions inserts data into our datastucture 

void bdeque_push_front(bdeque_typeann *d, bdeque_val_typeann v);
void bdeque_push_back(bdeque_typeann *d, bdeque_val_typeann v);

// All functions below this comment act upon our deque, and returns some value.

bdeque_val_typeann bdeque_pop_front(bdeque_typeann *d);
bdeque_val_typeann bdeque_pop_back(bdeque_typeann *d);
bdeque_val_typeann bdeque_peek_front(bdeque_typeann *d);
bdeque_val_typeann bdeque_peek_back(bdeque_typeann *d);

#endif
