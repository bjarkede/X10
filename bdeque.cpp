#include "bdeque.hpp"
#include "X10const.hpp"


Custom_deque::Custom_deque(int size)
{
  data = (char unsigned *)malloc(sizeof(unsigned char) * size);
  _capacity = size;
  clear();
}

bool Custom_deque::is_empty()
{
  return _size == 0;
}

int Custom_deque::size()
{
  return _size;
}

bool Custom_deque::equals(Custom_deque& other)
{
  if(other.size() != _size)
    return false;

  for(int i = 0; i < _size; i++)
    if (other.get_element_at(i) != get_element_at(i))
      return false;

  return true;
}

unsigned char Custom_deque::get_element_at(int index)
{
  int newIndex = _front + index;

  if (newIndex > _size)
    return -1;

  else if (newIndex > _capacity)
    newIndex = newIndex - _capacity;

  return data[newIndex];
}


void Custom_deque::clear()
{
  for(int i = 0; i < _capacity; i++)
    data[i] = 0;
}

void Custom_deque::push_back(unsigned char item)
{
  data[_index] = item;
  incrementIndex();
  _size++;
  if (_size > _capacity)
    _size = _capacity;
}

unsigned char Custom_deque::pop_front()
{
  unsigned char value = data[_front];
  incrementIndex();
  _size--;
  return value;
}

unsigned char Custom_deque::peek_front()
{
  return data[_front];
}

void Custom_deque::incrementIndex()
{
  _index++;
  if (_index > _capacity)
    _index = 0;
}







bdeque_type * bdeque_alloc() {
  bdeque_type *d = (bdeque_type *)malloc(sizeof(bdeque_type));
  if(d != nullptr) {
    d->head = d->tail = nullptr; // Datastructure is empty
  }
  return d;
}

void bdeque_free(bdeque_type *d) {
  free(d);
}

bool bdeque_is_empty(bdeque_type *d) {
  return d->head == nullptr;
}

bool bdeque_equal(bdeque_type *d1, bdeque_type *d2) {
  if(bdeque_size(d1) != bdeque_size(d2)) {
    return false;
  }
  
  struct node *n1 = d1->head;
  struct node *n2 = d2->head;
  
  for(int i = 0; i <= bdeque_size(d1); ++i) {
    if(n1->val != n2->val) { return false; }
    n1 = n1->next;
    n2 = n2->next;
  }
  return true;
}

// The integer value, v, decides how many times we pop the last element in our linked list.
void bdeque_resize(bdeque_type *d, int v) {
	for(int i = 0; i < v; i++) {
		bdeque_pop_back(d);
	}
}

void bdeque_clear(bdeque_type *d) {
  while(d->head != nullptr) {
    bdeque_pop_back(d);
  }
}

void bdeque_push_front(bdeque_type *d, bdeque_valtype v) {
  struct node *n = (node *)malloc(sizeof(struct node));
  n->val = v;
  n->next = d->head;
  n->prev = nullptr;
  if(d->tail == nullptr) {
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
  n->next = nullptr;
  if(d->head == nullptr) {
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
    d->head = d->tail = nullptr;
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
    d->head = d->tail = nullptr;
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
  if(d->head == nullptr) {
	  return 0;
  }
  
  struct node *n = d->head;
  int i = 1;

  while(n->next != nullptr) {
    ++i;
    n = n->next;
  }

  return i;
}
