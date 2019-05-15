#ifndef _X10_H
#define _X10_H

#include "X10const.hpp"

//#include <iostream>
//#include <deque> // @Incomplete: We dont want to use this anymore, we use bdeque.
//#include <tuple>
//#include <cassert>

#include "bdeque.hpp"

//#include <avr/io.h>
//#include <avr/interrupt.h>

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
enum state { IDLE = 0, SENDING = 1, RECEIVING = 2, ERROR = 3 };

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

struct X10_Controller {
private:
  state X10_state; 
protected:
public:
  X10_Controller() {
    DDRB = 0B00000001;
    X10_state = IDLE;
  }
  
  void transmit_code(X10_Code* code);
  //std::tuple<std::deque<char unsigned>, std::deque<char unsigned>> receive_code();
  bool idle();
 
  // Garbage
  state get_state(X10_Controller* controller) const {
    return this->X10_state;
  }
  void set_state(state new_state) {
    this->X10_state = new_state;
  }

};

bdeque_type* decode_manchester_deque(bdeque_type *d);
bdeque_type * convert_to_binary_string(bdeque_type *d);
bool compare_to_stop_code(bdeque_type *d);
bool split_and_compare_bits(bdeque_type *d);

void TIMER0_init() {
  SET_TIMER0_WAVEFORM;
  SET_TIMER0_MASK;
  OCR0A = 65; // See documentation for this value...
}

// We use this timer to create a delay of 1 ms
void TIMER1_init() {
  TCCR1B |= (1 << WGM12);  // Set CTC Mode
  TIMSK1 |= (1 << OCIE1A); // Interrupt enable
  ORC1A   = 15999;         // It takes roughly 1 ms to reach this
}

// We use TIMER2 to transmit at 300 khz.
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
  
  if(n == 6 || n == 3) {
    result = 3;
  } else {
    result = 4;
  }

  return result;
}

/*bool compare_to_stop_code(std::deque<char unsigned>  &d1, std::deque<char unsigned> &d2) {
  for(int i = d2.size(); i >= 1; i--) {
    if (d1[d1.size()-i] != d2[d2.size()-i]) { return false; }
  }

return true;
}

bool split_and_compare_bits(std::deque<char unsigned> d1) {
  std::deque<char unsigned> d2(
		    std::make_move_iterator(d1.begin() + d1.size()/2),
		    std::make_move_iterator(d1.end()));
  d1.erase(d1.begin() + d1.size()/2, d1.end());
  
  for(int i = 0; i <= d1.size(); i++){
    if(d1[i] != d2[i]) { return false; }
  }

  return true;
  }*/

#endif
