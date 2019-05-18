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
  bdeque_type *packet;  
  void construct_packet(char unsigned hc, char unsigned nc, char unsigned fc);
};

struct X10_Controller {
private:
  state X10_state; 
protected:
public:
  X10_Controller();
  
  void transmit_code(X10_Code* code);
  bdeque_type* receive_code();
  bool idle();
 
  // Garbage
  state get_state(X10_Controller* controller) const;
  void set_state(state new_state);
};

bdeque_type* decode_manchester_deque(bdeque_type *d);
bdeque_type* convert_to_binary_string(bdeque_type *d);
bool compare_to_stop_code(bdeque_type *d1, bdeque_type *d2);
bool split_and_compare_bits(bdeque_type *d);

void TIMER0_init();
void TIMER1_init();
void TIMER2_init();
void INT0_init();

int amount_of_bits(int n);

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
  
  bdeque_type* d2 = bdeque_alloc();
  struct node* n1  = d1->head;
  struct node* n1  = d1->head;
  
  for(int i = 0; i != bdeque_size(d1)/2; i ++) {
	  n = n->next;
  }
  
  d2->head = n;
  d2->tail = d1->tail;
  
  while(n1->next != NULL) {
	if(n1->val != n2->val) {
		return false;	
	}
  }

  return true;
  }*/
