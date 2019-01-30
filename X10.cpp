#include "X10.hpp"

void X10_Interface::transmit_code(X10_Code* code) {
  if(this->get_state(this) == RECEIVING) {
    std::cout << "Tried sending code when receiving.." << std::endl;
    return;
  }
  
  this->set_state(SENDING);
  while(!code->packet.empty()) {
    // @Incomplete
    // For each binary code in our data packet: We want to shift off each bit,
    // and send out the corresponding manchester encoded bit.. What this means is that
    // for each sine wave, we want to send the bit and its complement on the zero crossing point.
    // I.E: 0110 -> 0&1 then 1&0 1&0 0&1, for a total of 8 bits. -bjarke, 29th January 2019.

    // @Speed:
    // We could make the code into an array of characters and iterate.
    // -bjarke, 30th January 2019.
    unsigned char bit;
    unsigned char bit_complement;
    for(int i = 0; i <= amount_of_bits(code->packet.front()); ++i) {
      bit = (code->packet.front() & (1 << amount_of_bits(code->packet.front())));
      bit_complement = ~bit;
    }

    code->packet.pop_front(); // Move onto the next instruction
  }

  // We finished transmitting
  this->set_state(IDLE);
  return;
}

X10_Code X10_Interface::receive_code() {
  // @Incomplete:
  // We want to listen to the X10 network, and wait for a start-code
  // while we are idle.. if our house_code and number_code is sent,
  // we act on the information sent. -bjarke, 30th January 2019.
  if(this->X10_state == SENDING) {
    // Dont't do anything if we're sending data!
  }
  // Now we wan't to parse the manchester encoded bits into our X10_Code scheme, and act on it.
  
}

bool X10_Interface::wait_for_zero_crossing_point() {
  // @Incomplete:
  // We wan't to trigger on a external interrupt, that gets send from the hardware when we meet a crossing point
  // Then we can use the ISR to send out a 120khz burst for a certain amount of time... -bjarke, 30th January 2019. 

}

// Used for debugging...
int main(int argc, char* argv[]) {
  
  X10_Code* code1 = new X10_Code(HOUSE_A, ON);
  X10_Interface* interface = new X10_Interface();

  while(!code1->packet.empty()) {
    std::cout << (int)code1->packet.front() << std::endl;
    code1->packet.pop_front();
  }

  std::cout << "Our current state is: " << interface->get_state(interface) << std::endl; 

  return 0;
}

