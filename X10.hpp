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
// We use TIMER2 to transmit at 220 khz -bjarke, 22nd April 2019.
void TIMER2_init() {
  SET_TIMER2_WAVEFORM;
  SET_TIMER2_MASK;
  ORC2A = 35; // See documentation for this value...
}

void INT0_init() {
  SET_INT0_SENSE_CONTROL;
  START_INT0_INTERRUPT;
  
  return;
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
