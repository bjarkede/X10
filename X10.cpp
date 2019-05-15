#include "X10.hpp"

bdeque_type *encoded_packet = bdeque_alloc(); // This packet is empty unless we are sending an X10 Command.
bdeque_type *lpf_buffer     = bdeque_alloc();     // This buffer is loaded with bits received at 120khz.
bdeque_type *hpf_buffer     = bdeque_alloc();	  // This buffer is loaded with bits received at 300khz.

bool signal_state = true;                 // We use this to determine if to send HIGH or LOW in ISR.

state global_state;

void X10_Controller::transmit_code(X10_Code* code) {
  assert(this->X10_state == IDLE);
  this->set_state(SENDING);
  global_state = SENDING;

  bool continous_flag = false;
  char unsigned current_bit;

  // Push our start code to the encoded_packet.
  bdeque_push_back(encoded_packet, 0x0);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
 
  // Encode our X10_Scheme to manchester.
  while(!bdeque_is_empty(code->packet)) {
    if(!continous_flag && bdeque_size(code->packet) == 3) {
      for(int i = 0; i < 6; ++i) {
	bdeque_push_back(encoded_packet, 0x0);
      }
    }
    
    for(int i = amount_of_bits(bdeque_size(code->packet)); i >= 0 ; --i) {
      if(bdeque_peek_front(code->packet) == BRIGHT || bdeque_peek_front(code->packet) == DIM) { continous_flag = true; }
      
      current_bit = (bdeque_peek_front(code->packet) >> i) & 0x1;
      bdeque_push_back(encoded_packet, current_bit);
      bdeque_push_back(encoded_packet, current_bit^1);
    }
    
    bdeque_pop_front(code->packet); // Move onto next instruction
  }

  
  // Add the STOP_CODE to our encoded_packet..
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x0);
  
  // Start the external interrupt/TIMER0 now that we have parsed the bits.
  INT0_init();
  TIMER0_init();
  TIMER2_init();
  TIMER1_init(); // Used for 1ms delays
  sei();

  START_INT0_INTERRUPT;
  
  // Then wait until the global packet is empty...
  while(!bdeque_is_empty(encoded_packet)) {
    // Do nothing.
  }

  // We finished transmitting so disable INT0 and TIMER0 
  STOP_INT0_INTERRUPT;
  STOP_TIMER0;
  cli();
  delete(code);

  global_state = IDLE;
  this->set_state(IDLE);
  return;
}

std::tuple<std::deque<char unsigned>, std::deque<char unsigned> > X10_Controller::receive_code() {
  assert(this->X10_state == IDLE);
  this->set_state(RECEIVING);
  global_state = RECEIVING;

  // Start the external interrupt.
  INT0_init();
  TIMER1_init(); // We use this to time the length of the bursts.
  sei();

  START_INT0_INTERRUPT;

  // We need to check if the last four bits of one of the buffers
  // is equal to the stop_code.
  std::deque<char unsigned> compare_deque;
  compare_deque.push_back(0x1);
  compare_deque.push_back(0x1);
  compare_deque.push_back(0x1);
  compare_deque.push_back(0x0);
  
  bool is_equal_stop = false;
  
  while(!is_equal_stop) {
    is_equal_stop = compare_to_stop_code(lpf_buffer, compare_deque);
  }

  // Stop when we have seen the stop code
  STOP_INT0_INTERRUPT;
  STOP_TIMER1;
  cli();
  
  // Resize the vectors, to remove the stop-code.
  lpf_buffer.resize(lpf_buffer.size() - 4);
  hpf_buffer.resize(hpf_buffer.size() - 4);

  if(!split_and_compare_bits(decode_manchester_deque(lpf_buffer))) {
    // @Incomplete:
    // The two messages are not equal to eachother, we
    // need to handle this somehow, and send a messeage back that we got an error. -bjarke 9th May 2019.
  }
  
  if(!split_and_compare_bits(decode_manchester_deque(hpf_buffer))) {

  }
  
  global_state = IDLE;
  this->set_state(IDLE);
  return std::make_tuple(convert_to_binary_string(lpf_buffer), convert_to_binary_string(hpf_buffer));
}

bool X10_Controller::idle() {
  assert(this->X10_state == IDLE);
  assert(lpf_buffer.empty());
  assert(hpf_buffer.empty());
  this->set_state(IDLE);
  global_state = IDLE;

  INT0_init();
  sei();

  START_INT0_INTERRUPT;

  // We compare the lpf/hpf buffers 4 bits to the start code.
  std::deque<char unsigned> compare_deque;
  compare_deque.push_back(0x0);
  compare_deque.push_back(0x1);
  compare_deque.push_back(0x1);
  compare_deque.push_back(0x1);
  
  bool is_equal_lpf = false;
  bool is_equal_hpf = false;

  while(!is_equal_lpf || !is_equal_hpf) {
    if(lpf_buffer.size() > 4 && hpf_buffer.size() > 4) {
      // Maintain 4 bits while idle untill the start_code is registered.
      lpf_buffer.pop_front();
      hpf_buffer.pop_front();
    }
	  
    if(compare_deque.size() == lpf_buffer.size() || compare_deque.size() == hpf_buffer.size()) {
      is_equal_lpf = std::equal(compare_deque.begin(), compare_deque.end(), lpf_buffer.begin());
      is_equal_hpf = std::equal(compare_deque.begin(), compare_deque.end(), hpf_buffer.begin());
    }
  }

  if(is_equal_lpf || is_equal_hpf) {
    STOP_INT0_INTERRUPT;
    cli();
    
    lpf_buffer.clear();
    hpf_buffer.clear();
    
    return false; // We received the start-code and need to receive.
  } else { return true; }
}

ISR(INT0_vect) {
  if(!bdeque_is_empty(encoded_packet) && global_state == SENDING) {
    current_bit = bdeque_peek_front(encoded_packet);

    START_TIMER1; // This creates an interrupt after 1ms.

    while(((TCCR1B >> CS10) & 1) == 1) {
      if(current_bit == 0x1) {
	START_TIMER0;
      }
    }
    
    // On the next interrupt, transmit the next bit, by removing this one.
    bdeque_pop_front(encoded_packet);
  }

  if(!bdeque_is_empty(encoded_packet) && global_state == ERROR) {
    // @Incomplete:
    // If we get an error in transmission, and need to send something back to stop the transmission
    // We enter this statement, and transmit the messeage at 300 kHz. -bjarke, 9th May 2019.
    current_bit = bdeque_peek_front(encoded_packet);

    START_TIMER1; // Creates an intterupt after 1ms.

    while(((TCCR1B >> CS10) & 1) == 1) {
      if(current_bit = 0x1) {
	START_TIMER2;
      }
    }

    bdeque_pop_front(encoded_packet);
  }

  if(global_state = RECEIVING) {

    START_TIMER1;

    int lpf_prev_size = bdeque_size(lpf_buffer);
    int hpf_prev_size = bdeque_size(hpf_buffer);
    
    while(((TCCR1B >> CS10) & 1) == 1) {
      // If some port goes HIGH, load either the lpf or hpf buffer.
      if(((PINB >> 1) & 1) == 1) { // If there is a 1 on this PIN, load 1 into LPF.
	bdeque_push_back(lpf_buffer, 0x1);
      }
      if(((PINB >> 2) & 1) == 1) { // If there is a 1 on this PIN, load 1 into HPF.
        bdeque_push_back(hpf_buffer, 0x1);
      }
    }

    // If the size of the buffer didn't, this means that we didn't receive a 1.
    // Therefore we load a 0 into the buffer. 
    if(lpf_prev_size == bdeque_size(lpf_buffer)) {
      bdeque_push_back(lpf_buffer, 0x0);
    }
    if(hpf_prev_size == bdeque_size(hpf_buffer)) {
      bdeque_push_back(hpf_buffer, 0x0);
    }
  }
} 

ISR(TIMER1_COMPA_vect) {
  STOP_TIMER1; // Stop TIMER1, since 1 ms has passed.
  STOP_TIMER0; // Likewise stop TIMER0, since we no longer want to transmit the bit.
  STOP_TIMER2; // Stop this as well if we are sending at 220 KHz.

  // There are some things we want to make sure are stopped here..
  PORTB |= 0 << 0;
}

ISR(TIMER0_COMPA_vect) {
  if(signal_state) {
    signal_state = false; 
    PORTB |= 1 << 0;
  } else {
    signal_state = true;
    PORTB |= 0 << 0;
  }
}

ISR(TIMER2_COMPA_vect) {
   if(signal_state) {
    signal_state = false; 
    PORTB |= 1 << 0;
  } else {
    signal_state = true;
    PORTB |= 0 << 0;
  }
}


