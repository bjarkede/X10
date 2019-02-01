#include "X10.hpp"

std::deque<char unsigned> encoded_packet;

void X10_Controller::transmit_code(X10_Code* code) {
  assert(this->X10_state == IDLE);
  this->set_state(SENDING);

  bool continous_flag = false;

  while(!code->packet.empty()) {
    if(!continous_flag && code->packet.size() == 3) {
      // A code packet should be followed by 3 power line cycles between each group.
      // If it isn't a BRIGHT or DIM command.
      for(int i = 0; i < 6; ++i) {
	encoded_packet.push_back(0x0);
      }
    }

    for(int i = 0; i < amount_of_bits(code->packet.size()); ++i) { 
      if(code->packet.front() == BRIGHT || code->packet.front() == DIM)	{ continous_flag = true; }
      
      // Send the front bit and its complement to the external interrupt routine.
      encoded_packet.push_back((code->packet.front() & (1 << i)) != 0);
      encoded_packet.push_back(current_bit^1);
    }

    code->packet.pop_front(); // Move onto the next instruction
  }

  // @Incomplete:
  // Start the external interrupt/TIMER0 now that we have parsed the bits.
  
  // Then wait until the global packet is empty...
  while(!encoded_packet.empty()) {
    // Do nothing.
  }

  // We finished transmitting
  this->set_state(IDLE);
  return;
}

X10_Code* X10_Controller::receive_code() {
  assert(this->X10_state == IDLE);
  this->set_state(RECEIVING);
  // @Incomplete:
  // We want to listen to the X10 network, and wait for a start-code
  // while we are idle.. if our house_code and number_code is sent,
  // we act on the information sent. -bjarke, 30th January 2019.
  
  // Now we wan't to parse the manchester encoded bits into our X10_Code scheme, and act on it.

  // Temp
  X10_Code* result = new X10_Code(HOUSE_A, OFF);
  
  this->set_state(IDLE);
  return result;
}

// Used for debugging...
int main(int argc, char* argv[]) {
  
  X10_Code* code = new X10_Code(HOUSE_A, ON);
  X10_Controller* controller = new X10_Controller();

  std::cout << "Encoding our X10_code to: " << std::endl;
  controller->transmit_code(code);

  return 0;
}

// @TODO
// When we cross 0 on the AC power line, we do an external interrupt. 
// Then we send a bit for 1ms, at 120kHz. -bjarke, 1st Febuary 2019.
ISR(INT0_vect) {
  if(!encoded_packet.empty()) {
    current_bit = encoded_packet.front();

    // @Incomplete:
    // Transmit the bit for 1 ms.
    START_TIMER0;
    
    // On the next interrupt, transmit the next bit, by removing this one.
    encoded_packet.pop_front();
  }
}

ISR(TIMER0_COMPA_vect) {
  // @Incomplete:
  // We need to send out HIGH on some port, then disable the timer and send low untill
  // the next zero crossing point. -bjarke, 1st Febuary 2019.
}


