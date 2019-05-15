#include "bdeque.hpp"
#include "stdlib.h"
#include "X10const.hpp"
#include <iostream>

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

struct X10_Code {
  bdeque_type *packet;
  
  X10_Code(char unsigned hc, char unsigned nc, char unsigned fc)
  {
    packet = bdeque_alloc();
    for(int i = 0; i < 2; ++i) {
      bdeque_push_back(packet, hc);
      bdeque_push_back(packet, nc);
      bdeque_push_back(packet, fc);
    }
  }

  ~X10_Code() { bdeque_free(packet); }
  
};

int amount_of_bits(int n) {
  int result = 0;
  
  if(n == 6 || n == 3) {
    result = 3;
  } else {
    result = 4;
  }

  return result;
}

bdeque_type *encoded_packet = bdeque_alloc();

int main() {

  X10_Code* test = new X10_Code(HOUSE_A, KEY_1, ON);

  if(bdeque_is_empty(test->packet)) {
    std::cout << "We are empty!" << std::endl;
  } else {
    std::cout << "We are not empty!" << std::endl;
    std::cout << "Our size is: " << bdeque_size(test->packet) << std::endl;
  }

  // Test the transmit_code on our new datastructure
  bool continous_flag = false;
  char unsigned current_bit;

  bdeque_push_back(encoded_packet, 0x0);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);

  while(!bdeque_is_empty(test->packet)) {
    if(!continous_flag && bdeque_size(test->packet) == 3) {
      for(int i = 0; i < 6; ++i) {
	bdeque_push_back(encoded_packet, 0x0);
      }
    }

    for(int i = amount_of_bits(bdeque_size(test->packet)); i >= 0 ; --i) {
      if(bdeque_peek_front(test->packet) == BRIGHT || bdeque_peek_front(test->packet) == DIM) { continous_flag = true; }

      current_bit = (bdeque_peek_front(test->packet) >> i) & 0x1;
      bdeque_push_back(encoded_packet, current_bit);
      bdeque_push_back(encoded_packet, current_bit^1);
    }

    bdeque_pop_front(test->packet);
  }

  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x0);

  
  return 1;
}
