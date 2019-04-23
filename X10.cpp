#include "X10.hpp"

std::deque<char unsigned> encoded_packet; // This packet is empty unless we are sending an X10 Command.
std::deque<char unsigned> lpf_buffer;     // This buffer is loaded with bits received at 120hz.
std::deque<char unsigned> hpf_buffer;	  // This buffer is loaded with bits received at 220hz.

// @Incomplete:
// The X10 protocol is not yet finished, we need to specify which codes
// that trigger 220 kHz transmission, by default we transmit at 120 kHz. -bjarke, 22nd April 2019.
void X10_Controller::transmit_code(X10_Code* code) {
  assert(this->X10_state == IDLE);
  this->set_state(SENDING);

  bool continous_flag = false;
  char unsigned current_bit;
  
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

      // We don't encode start codes..
      if(code->packet.size() == 6 || code->packet.size() == 3) {
	encoded_packet.push_back(current_bit)
      } else {
	encoded_packet.push_back(current_bit);
	encoded_packet.push_back(current_bit^1);
      }
    }

    code->packet.pop_front(); // Move onto the next instruction
  }

  // Start the external interrupt/TIMER0 now that we have parsed the bits.
  INT0_init();
  TIMER0_init();
  TIMER1_init(); // Used for 1ms delays
  sei();
  
  // Then wait until the global packet is empty...
  while(!encoded_packet.empty()) {
    // Do nothing.
  }

  // We finished transmitting so disable INT0 and TIMER0 
  STOP_INT0_INTERRUPT;
  STOP_TIMER0;
  cli();
  
  this->set_state(IDLE);
  return;
}

// @Incomplete:
// We want to listen to the X10 network, and wait for a start-code
// while we are idle.. if our house_code and number_code is sent,
// we act on the information sent. -bjarke, 30th January 2019.
X10_Code* X10_Controller::receive_code() {
  assert(this->X10_state == IDLE);
  this->set_state(RECEIVING);

  // Start the external interrupt.
  INT0_init();
  TIMER1_init(); // We use this to time the length of the bursts.
  sei();
  
  // TODO:
  // We start receiving bytes and place them into a deque. We receive in two buffers.
  // If we are selected in one of the commands we act on it, if not we do nothing. -bjarke, 15th April 2019.

  while(1) {
    // @IDEA:
    // If we have a local buffer of the last 4-6 bytes received in either
    // buffer, and if one of those are equal to some stop-code (0000),
    // we can break from this and start decoding the deques. -bjarke, 23th April 2019.
  }

  // @Incomplete:
  // We need to implement a manchester to X10_Code decoder. -bjarke, 23th April 2019.

  X10_Code* result = new X10_Code(HOUSE_A, OFF);
  
  this->set_state(IDLE);
  return result;
}

bool X10_Controller::idle() {
  assert(this->X10_state == IDLE);
  
  // TODO:
  // We want to use a timer to keep track of some global buffer
  // of bytes. When this buffer equals our X10 start code, we
  // start receiving the bytes. -bjarke, 4th Febuary 2019.

  INT0_init();
  TIMER2_init(); // @Incomplete: Implement this to read data into global buffers.
  sei();

  while(1) {

    if(lpf_buffer.size() > 4 && hpf_buffer.size() > 4) {
      // Maintain 4 bits while idle untill the start_code is registered.
      lpf_buffer.pop_front();
      hpf_buffer.pop_front();
    }

  }

  return true;
}

ISR(INT0_vect) {
  if(!encoded_packet.empty()) {
    current_bit = encoded_packet.front();

    // @TODO:
    // Add functionality to choose between 120/220 KHz transmission
    // depending on some global state based on previous received X10 commands -bjarke, 22nd April 2019.
    START_TIMER0;
    
    START_TIMER1; // This creates an interrupt after 1ms.

    while(((TCCR1B >> CS10) & 1) == 1) {}
    
    // On the next interrupt, transmit the next bit, by removing this one.
    encoded_packet.pop_front();
  }

  // @Incomplete: We use this when we want to receive bits.
  if(encoded_packet.empty()) {}
  
}

ISR(TIMER1_COMPA_vect) {
  STOP_TIMER1; // Stop TIMER1, since 1 ms has passed.
  STOP_TIMER0; // Likewise stop TIMER0, since we no longer want to transmit the bit.
  STOP_TIMER2; // Stop this as well if we are sending at 220 KHz.
}

ISR(TIMER0_COMPA_vect) {
  // @Incomplete:
  // When we know which pin we need to send the burst on, we toggle the pin here.
  // -bjarke, 23th April 2019.
}

ISR(TIMER2_COMPA_vect) {
  // @Incomplete:
  // If we are sending at 220 KHz we toggle the pin here. -bjarke, 23th April 2019.
}


