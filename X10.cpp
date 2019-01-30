#include "X10.hpp"

void X10_Interface::transmit_code(X10_Code* code) {
  while(!code->packet.empty()) {
    // @Incomplete
    // For each binary code in our data packet: We want to shift off each bit,
    // and send out the corresponding manchester encoded bit.. What this means is that
    // for each sine wave, we want to send the bit and its complement on the zero crossing point.
    // I.E: 0110 -> 0&1 then 1&0 1&0 0&1, for a total of 8 bits. -bjarke, 29th January 2019.

    // @Speed:
    // We could make the code into an array of characters and iterate.
    // -bjarke, 30th January 2019.
    for(int i = 0; i <= amount_of_bits(code->packet.front()); ++i) {
      // 1. Step: Wait for crossing.
      // 2. Step: Get the bit, and send the first manchester encoded version.
      // 3. Step: Wait for crossing.
      // 4. Step: Send the second part of the manchester encoded version.
    }

    code->packet.pop_front(); // Move onto the next instruction
  }
}

bool X10_Interface::wait_for_zero_crossing_point() {

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

