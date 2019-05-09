#pragma once

#include "X10const.hpp"

#include <iostream>
#include <deque>
#include <cassert>

#include <avr/io.h>
#include <avr/interrupt.h>

#define SET_TIMER0_WAVEFORM TCCR0A |= (1<<WGM01)
#define SET_TIMER0_MASK TIMSK0 |= (1<<OCIE0A)
#define START_TIMER0 TCCR0B |= (1<<CS00)
#define STOP_TIMER0 TCCR0B &= ~(1<<CS00)

#define START_TIMER1 TCCR1B |= (1<<CS10)
#define STOP_TIMER1 TCCR1B &= ~(1<<CS10)

#define SET_TIMER2_WAVEFORM TCCR2A |= (1<<WGM11)
#define SET_TIMER2_MASK TIMSK2 |= (1<<OCIE2A)
#define START_TIMER2 TCCR2B |= (1<<CS20)
#define STOP_TIMER2 TCCR1B &= ~(1<<CS20)

#define SET_INT0_SENSE_CONTROL EICRA |= (1<<ISC00)|(1<<ISC01) // Rising edge
#define START_INT0_INTERRUPT EIMSK |= (1<<INT0)
#define STOP_INT0_INTERRUPT EIMSK &= 0B11111110

// X10 States
enum state { IDLE = 0, SENDING = 1, RECEIVING = 2 };

// @Incomplete:
// We also want to send the function code, so we dont have to
// do two transmissions, one for house and key, and
// for house and function -bjarke, 8th May 2019.
struct X10_Code  {
  std::deque<char unsigned> packet;

  X10_Code(char unsigned hc, char unsigned nc, char unsigned fc)
  {
    // We push it two times, because we need to send it twice.
    for(int i = 0; i < 2; ++i) {
      packet.push_back(hc);
      packet.push_back(nc);
      packet.push_back(fc);
    }
  }
};

struct X10_Controller {
private:
  int receiving_pin;
  int interrupt_pin;
  
  state X10_state; 
protected:
public:
  X10_Controller() { X10_state = IDLE; }
  
  void transmit_code(X10_Code* code);
  X10_Code* receive_code();

  bool idle();
 
  // Garbage
  state get_state(X10_Controller* controller) const {
    return this->X10_state;
  }
  void set_state(state new_state) {
    this->X10_state = new_state;
  }

};

// This function takes a deque of manchester encoded bits and converts
// it to a deque of non-encoded bits.
std::deque<char unsigned> decode_manchester_deque(std::deque<char unsigned> &d1) {

  // First we we decode the manchester to just a regular queue of bits
  char unsigned current_bit;
  char unsigned next_bit;
  std::deque<char unsigned> result;

  while(!d1.empty()) {
    current_bit = d1.front();
    d1.pop_front();
    next_bit = d1.front();
    d1.pop_front();

    if(current_bit == 0x1) {
      if(next_bit == 0x0) {
	result.push_back(0x1);
      }
    } else if (current_bit == 0x0) {
      if(next_bit == 0x1) {
	result.push_back(0x0);
      }
    }
  }
  
  return result;
}

// This function takes a double ended queue full of bits, and converts
// it to a deque that has the house, key, and function code in it.
std::deque<char unsigned> convert_to_binary_string(std::deque<char unsigned> &d1) {
  assert(d1.size() == 14); // We should have a total of 14 bits in this
  
  char unsigned hc;
  char unsigned kc;
  char unsigned fc;

  int i = 0;
  int j = 0;
  int k = 0;
  
  for(std::deque<char unsigned>::reverse_iterator rit = d1.rbegin(); rit != d1.rend(); ++rit) {
    if(i < 6) {
      fc ^= (-*rit ^ fc) & (1 << i);
      i++;
    }
    if (i > 5 && j < 6) {
      kc ^= (-*rit ^ kc) & (1 << j);
      j++;
    }
    if(j > 5 && k < 6) {
      hc ^= (-*rit ^ hc) & (1 << k);
      k++;
    }
  }

  d1.clear();
  d1.push_back(hc);
  d1.push_back(kc);
  d1.push_back(fc);
  
  return d1;
}

// @Incomplete:
// We need to specify the output pin for the signal.
// We don't start the timer yet, because we want to do that exactly when we do
// the external interrupt on INT0. -bjarke, 2nd Febuary 2019.
void TIMER0_init() {
  SET_TIMER0_WAVEFORM;
  SET_TIMER0_MASK;
  OCR0A = 65; // See documentation for this value...
  
  return;
}

// We use this timer to create a delay of 1 ms
void TIMER1_init() {
  TCCR1B |= (1 << WGM12);  // Set CTC Mode
  TIMSK1 |= (1 << OCIE1A); // Interrupt enable
  ORC1A   = 15999;         // It takes roughly 1 ms to reach this
}

// @Incomplete:
// We use TIMER2 to transmit at 300 khz -bjarke, 7th May 2019.
void TIMER2_init() {
  SET_TIMER2_WAVEFORM;
  SET_TIMER2_MASK;
  ORC2A = 27; // 16 MHz / (2 * prescaler (1) * desired_frequency) = 26.67
}

void INT0_init() {
  SET_INT0_SENSE_CONTROL;
  START_INT0_INTERRUPT;
}

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
}

bool compare_to_stop_code(std::deque<char unsigned>  &d1, std::deque<char unsigned> &d2) {
  for(int i = d2.size(); i >= 1; i--) {
    if (d1[d1.size()-i] != d2[d2.size()-i]) { return false; }
  }

return true;
}

bool split_and_compare_bits(std::deque<char unsigned> &d1) {
  std::vector<char unsigned> d2(
		    std::make_move_iterator(d1.begin() + d1.size()/2),
		    std::make_move_iterator(d1.end()));
  d1.erase(d1.begin() + d1.size()/2, d1.end());
  
  for(int i = 0; i <= d1.size(); i++){
    if(d1[i] != d2[i]) { return false; }
  }

  return true;
}

