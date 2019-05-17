#include "X10.hpp"


int main(int argc, char* argv[]) {

  // Init our X10 Controller for this device
  X10_Controller controller = X10_Controller();
  
  /* @Completed Implementation.
  X10_Code *code = (X10_Code *)malloc(sizeof(X10_Code));
  code->construct_packet(HOUSE_A, KEY_2, ON);
  
  controller.transmit_code(code);
  */
  

  while(1) {
    if(!controller.idle()) { // @TODO: Test idle().
		PORTB |= 1 << 1;
	  //bdeque_type * received_packet = controller.receive_code();
    }
  }

  return 0;
}
