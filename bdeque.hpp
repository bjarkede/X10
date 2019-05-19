#pragma once

#include <stdlib.h>
//#include <cassert>


class Custom_deque
{
public:
  Custom_deque();	
  Custom_deque(int size);
  bool is_empty();
  bool equals(Custom_deque& other);
  int size();
  void clear();
  
  unsigned char get_element_at(int index);
  void push_back(unsigned char item);
  unsigned char pop_front();
  unsigned char pop_back();
  unsigned char peek_front();
  
  bool compare_last_four(Custom_deque& other);
  bool is_symmetrical();

private:
  unsigned char* data;
  int _index = 0;
  int _size = 0;
  int _capacity = 0;
  int _front = 0;
  void incrementIndex();
  void decrementIndex();
};