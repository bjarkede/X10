#include "X10.hpp"

std::deque<char unsigned> encoded_packet; // This packet is empty unless we are sending an X10 Command.
std::deque<char unsigned> lpf_buffer;     // This buffer is loaded with bits received at 120khz.
std::deque<char unsigned> hpf_buffer;	  // This buffer is loaded with bits received at 300khz.

bool signal_state = true;                 // We use this to determine if to send HIGH or LOW in ISR.

state global_state;

void X10_Controller::transmit_code(X10_Code* code) {
  assert(this->X10_state == IDLE);
  this->set_state(SENDING);
  global_state = SENDING;

  bool continous_flag = false;
  char unsigned current_bit;

  // Push our start code to the encoded_packet.
  encoded_packet.push_back(0x0);
  encoded_packet.push_back(0x1);
  encoded_packet.push_back(0x1);
  encoded_packet.push_back(0x1);
 
  // Encode our X10_Scheme to manchester.
  while(!code->packet.empty()) {
    if(!continous_flag && code->packet.size() == 3) {
      for(int i = 0; i < 6; ++i) {
	encoded_packet.push_back(0x0);
      }
    }

    for(int i = 0; i < amount_of_bits(code->packet.size()); ++i) { 
      if(code->packet.front() == BRIGHT || code->packet.front() == DIM)	{ continous_flag = true; }

      current_bit = (code->packet.front() & (1 << i)) != 0;

      encoded_packet.push_back(current_bit);
      encoded_packet.push_back(current_bit^1);
    }

    code->packet.pop_front(); // Move onto the next instruction
  }

  // Add the STOP_CODE to our encoded_packet..
  encoded_packet.push_back(0x1);
  encoded_packet.push_back(0x1);
  encoded_packet.push_back(0x1);
  encoded_packet.push_back(0x0);
  
  // Start the external interrupt/TIMER0 now that we have parsed the bits.
  INT0_init();
  TIMER0_init();
  TIMER1_init(); // Used for 1ms delays
  sei();

  START_INT0_INTERRUPT;
  
  // Then wait until the global packet is empty...
  while(!encoded_packet.empty()) {
    // Do nothing.
  }

  // We finished transmitting so disable INT0 and TIMER0 
  STOP_INT0_INTERRUPT;
  STOP_TIMER0;
  cli();

  global_state = IDLE;
  this->set_state(IDLE);
  return;
}

std::tuple<std::deque<char unsigned>, std::deque<char unsigned>> X10_Controller::receive_code() {
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
  if(!encoded_packet.empty() && global_state == SENDING) {
    current_bit = encoded_packet.front();

    START_TIMER1; // This creates an interrupt after 1ms.

    while(((TCCR1B >> CS10) & 1) == 1) {
      if(current_bit == 0x1) {
	START_TIMER0;
      }
    }
    
    // On the next interrupt, transmit the next bit, by removing this one.
    encoded_packet.pop_front();
  }

  if(!encoded_pakcet.empty() && global_state == ERROR) {
    // @Incomplete:
    // If we get an error in transmission, and need to send something back to stop the transmission
    // We enter this statement, and transmit the messeage at 300 kHz. -bjarke, 9th May 2019.
    current_bit = encoded_packet.front();

    START_TIMER1; // Creates an intterupt after 1ms.

    while(((TCCR1B >> CS10) & 1) == 1) {
      if(current_bit = 0x1) {
	START_TIMER2;
      }
    }

    encoded_packet.pop_front();
  }

  if(global_state = RECEIVING) {

    START_TIMER1;

    int lpf_prev_size = lpf_buffer.size();
    int hpf_prev_size = hpf_buffer.size();
    
    while(((TCCR1B >> CS10) & 1) == 1) {
      // If some port goes HIGH, load either the lpf or hpf buffer.
      if(((PINB >> 1) & 1) == 1) { // If there is a 1 on this PIN, load 1 into LPF.
	lpf_buffer.push_back(0x1);
      }
      if(((PINB >> 2) & 1) == 1) { // If there is a 1 on this PIN, load 1 into HPF.
	hpf_buffer.push_back(0x1);
      }
    }

    // If the size of the buffer didn't, this means that we didn't receive a 1.
    // Therefore we load a 0 into the buffer. 
    if(lpf_prev_size == lpf_buffer.size()) {
      lpf_buffer.push_back(0x0);
    }
    if(hpf_prev_size == hpf_buffer.size()) {
      hpf_buffer.push_back(0x0);
    }
  }
} 

ISR(TIMER1_COMPA_vect) {
  STOP_TIMER1; // Stop TIMER1, since 1 ms has passed.
  STOP_TIMER0; // Likewise stop TIMER0, since we no longer want to transmit the bit.
  STOP_TIMER2; // Stop this as well if we are sending at 220 KHz.
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


