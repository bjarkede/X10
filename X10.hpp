#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>

#include "X10const.hpp"
#include "bdeque.hpp"

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
  Custom_deque packet;  
  void construct_packet(char unsigned hc, char unsigned nc, char unsigned fc);
};

class X10_Controller {
private:
  state X10_state;
  
  char unsigned _house_code;
  char unsigned _number_code;
  char unsigned _function_code;
protected:
public:
  X10_Controller();
  
  void transmit_code(X10_Code* code);
  void receive_code();
  bool idle();
 
  // Garbage
  state get_state(X10_Controller* controller) const;
  void set_state(state new_state);
  void decode_manchester_deque(Custom_deque &d);
};

void TIMER0_init();
void TIMER1_init();
void TIMER2_init();
void INT0_init();

int amount_of_bits(int n);