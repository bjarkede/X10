#pragma once

#include "X10const.hpp"

#include <iostream>
#include <deque> 

// X10 States
enum state { IDLE = 0, SENDING = 1, RECEIVING = 2 };

struct X10_Code  {
  std::deque<char unsigned> packet;
  X10_Code(char unsigned house_code, char unsigned number_code)
  {
    packet.push_back(START_CODE);
    packet.push_back(house_code);
    packet.push_back(number_code);
  }
};

struct X10_Interface {
private:
  int transmission_pin;
  int receiving_pin;

  state X10_state; 
protected:
public:
  X10_Interface() { X10_state = IDLE;}
  
  void transmit_code(X10_Code* code);
  X10_Code receive_code();
  
  // @Incomplete
  // Since X10 transmissions are synchronized to the zero crossing point of the AC power line
  // we want to be able to see if we hit it, so we can transmit. -bjarke, 29th January 2019
  bool wait_for_zero_crossing_point();

  // Garbage
  state get_state(X10_Interface* interface) {
    return this->X10_state;
  }
  void set_state(state new_state) {
    this->X10_state = new_state;
  }
};

int amount_of_bits(unsigned char n) {
  int count = 0;
  while(n) {
    count += n % 2;
    n >>= 1;
  }
  return count;
};
