#include "bdeque.hpp"
#include "X10const.hpp"

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

bdeque_type * bdeque_alloc() {
  bdeque_type *d = (bdeque_type *)malloc(sizeof(bdeque_type));
  if(d != NULL) {
    d->head = d->tail = NULL; // Datastructure is empty
  }
  return d;
}

void bdeque_free(bdeque_type *d) {
  free(d);
}

bool bdeque_is_empty(bdeque_type *d) {
  return d->head == NULL;
}

void bdeque_push_front(bdeque_type *d, bdeque_valtype v) {
  struct node *n = (node *)malloc(sizeof(struct node));
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

void bdeque_push_back(bdeque_type *d, bdeque_valtype v) {
  struct node *n = (node *)malloc(sizeof(struct node));
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

bdeque_valtype bdeque_pop_front(bdeque_type *d) {
  bdeque_valtype v = d->head->val;
  struct node *n = d->head;
  if(d->head == d->tail) {
    d->head = d->tail = NULL;
  } else {
    d->head = n->next;
  }
  free(n);
  return v;
}

bdeque_valtype bdeque_pop_back(bdeque_type *d) {
  bdeque_valtype v = d->tail->val;
  struct node *n = d->tail;
  if(d->head == d->tail) {
    d->head = d->tail = NULL;
  } else {
    d->tail = n->prev;
  }
  free(n);
  return v;
}

bdeque_valtype bdeque_peek_front(bdeque_type *d) {
  return d->head ? d->head->val : (int)NULL;
}

bdeque_valtype bdeque_peek_back(bdeque_type *d) {
  return d->tail ? d->tail->val : (int)NULL;
}

int bdeque_size(bdeque_type *d) {
  struct node *n = d->head;
  int i = 1; // We init this as one, because we dont count the the tail when we increment.

  while(n != d->tail) {
    ++i;
    n = n->next;
  }

  return i;
}