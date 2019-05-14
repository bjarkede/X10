#include "bdeque.hpp"
#include "stdlib.h"

// A node in our datastructure is described like this
// It points to the next and previous elements and
// contains the value val.
struct node {
  struct node *next;
  struct node *prev;
  bdeque_val_typeann val;
};

// Our double ended queue has a pointer to the head and tail element.
struct bdeque {
  struct node *head;
  struct node *tail;
};

bdeque_typeann * bdeque_alloc() {
  bdeque_typeann *d = new bdeque_typeann();
  if(d != NULL) {
    d->head = d->tail = NULL; // Datastructure is empty
  }
  return d;
}

void bdeque_free(bdeque_typeann *d) {
  free(d);
}

bool bdeque_is_empty(bdeque_typeann *d) {
  return d->head == NULL;
}

void bdeque_push_front(bdeque_typeann *d, bdeque_val_typeann v) {
  struct node *n = new node();;
  n->val = v;
  n->next = d->head;
  n->prev = NULL;
  if(d->tail == NULL) {
    d->head = d->tail = n;
  } else {
    d->head->prev = n;
    d->head = n;
  }
}

void bdeque_push_back(bdeque_typeann *d, bdeque_val_typeann v) {
  struct node *n = new node();
  n->val = v;
  n->prev = d->tail;
  n->next = NULL;
  if(d->head == NULL) {
    d->head = d->tail = n;
  } else {
    d->tail->next = n;
    d->tail = n;
  }
}

bdeque_val_typeann bdeque_pop_front(bdeque_typeann *d) {
  bdeque_val_typeann v = d->head->val;
  struct node *n = d->head;
  if(d->head == d->tail) {
    d->head = d->tail = NULL;
  } else {
    d->head = n->next;
  }
  delete n;
  return v;
}

bdeque_val_typeann bdeque_pop_back(bdeque_typeann *d) {
  bdeque_val_typeann v = d->tail->val;
  struct node *n = d->tail;
  if(d->head == d->tail) {
    d->head = d->tail = NULL;
  } else {
    d->tail = n->prev;
  }
  delete n;
  return v;
}

bdeque_val_typeann bdeque_peek_front(bdeque_typeann *d) {
  return d->head ? d->head->val : NULL;
}

bdeque_val_typeann bdeque_peek_back(bdeque_typeann *d) {
  return d->tail ? d->tail->val : NULL;
}
