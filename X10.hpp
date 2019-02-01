#pragma once

#include "X10const.hpp"

#include <iostream>
#include <deque>
#include <cassert>

#include <avr/interrupt.h>

// X10 States
enum state { IDLE = 0, SENDING = 1, RECEIVING = 2 };

struct X10_Code  {
  std::deque<char unsigned> packet;
  X10_Code(char unsigned house_code, char unsigned number_code)
  {
    // We push it two times, because we need to send it twice.
    for(int i = 0; i < 2; ++i) {
      packet.push_back(START_CODE);
      packet.push_back(house_code);
      packet.push_back(number_code);
    }
  }
};

struct X10_Controller {
private:
  int transmission_pin;
  int receiving_pin;
  int interrupt_pin;
  
  state X10_state; 
protected:
public:
  X10_Controller() { X10_state = IDLE;}
  
  void transmit_code(X10_Code* code);
  X10_Code* receive_code();
 
  // Garbage
  state get_state(X10_Controller* controller) {
    return this->X10_state;
  }
  void set_state(state new_state) {
    this->X10_state = new_state;
  }

};

// @Hack:
// We know the order in which our START/HOUSE 4 long, and NUMBER/FUNCTION 5 long
// Enter our transmission, so we hardcode the length.. -bjarke, 1st Febuary 2019.
int amount_of_bits(int n) {
  int result = 0;
  
  if(n == 6 || n == 5 || n == 3 || n == 2) {
    result = 4;
  } else {
    result = 5;
  }

  return result;
};
