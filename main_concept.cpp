#include "X10.hpp"

// Sample code of how our X10_Devices should be structered to function.
// Description:
//
// Our device runs our idle() function as often as it can. If our idle returns
// false we run receive_code() that changes our device state to receiving. 
// We allocate the encoded packet that we parsed in receive to our X10_Code scheme.
// Then we act on it and change our X10_Controller state to IDLE/SENDING, depending
// on the instructions we received from the X10 network. -bjarke, 3rd Febuary 2019.

int main(int argc, char* argv[]) {

  // Init our X10 Controller for this device
  X10_Controller* controller = new X10_Controller();

  for(;;) {
    if(controller->X10_state != IDLE) {
      X10_Code code = controller->receive_code();

      // Act on the received X10_Code...      
    }
  }

  return 0;
}
