#include "X10.hpp"

void X10_Controller::transmit_code(X10_Code* code) {
  assert(this->X10_state == IDLE);
  this->set_state(SENDING);

  bool continous_flag = false;
  while(!code->packet.empty()) {
    if(!continous_flag && code->packet.size() == 3) {
      // A code packet should be followed by 3 power line cycles between each group.
      // If it isn't a BRIGHT or DIM command.
      for(int i = 0; i < 6; ++i) {
	this->add_encoded_bit_to_packet(0);
      }
    }
    
    // @Incomplete
    // For each binary code in our data packet: We want to shift off each bit,
    // and send out the corresponding manchester encoded bit.. What this means is that
    // for each sine wave, we want to send the bit and its complement on the zero crossing point.
    // I.E: 0110 -> 0&1 then 1&0 1&0 0&1, for a total of 8 bits. -bjarke, 29th January 2019.

    unsigned char current_bit;
    unsigned char current_bit_complement;
    for(int i = 0; i <= amount_of_bits(code->packet.front()); ++i) { 
      if(code->packet.front() == BRIGHT || code->packet.front() == DIM) { continous_flag = true; }
      current_bit = (code->packet.front() & (1 << amount_of_bits(code->packet.front())));
      current_bit_complement = ~current_bit;

      // @Cleanup
      this->add_encoded_bit_to_packet(current_bit);
      this->add_encoded_bit_to_packet(current_bit_complement);
    }

    code->packet.pop_front(); // Move onto the next instruction
  }

  // Start the external interrupt now that we have parsed the bits.

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

std::deque<char unsigned> X10_Controller::get_encoded_packet() {
  return this->encoded_packet;
}

// Used for debugging...
int main(int argc, char* argv[]) {
  
  X10_Code* code1 = new X10_Code(HOUSE_A, BRIGHT);
  X10_Controller* controller = new X10_Controller();

  std::cout << "Encoding our X10_code to: " << std::endl;
  controller->transmit_code(code1);

  return 0;
}

// @TODO
// When we cross 0 on the AC power line, we do an external interrupt. 
// Then we send a bit for 1ms, at 120kHz. -bjarke, 1st Febuary 2019.
ISR(INT0_vect) {
  auto packet = get_encoded_packet();
  if(!packet.empty()) {
    current_bit = packet.front();

    // Transmit the bit for 1 ms.
    
    // On the next interrupt, transmit the next bit, by removing this one.
    packet.pop_front();
  }
}

