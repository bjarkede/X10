#include "X10.hpp"

using namespace std;

int main(int argc, char* argv[]) {

  // Init our X10 Controller for this device
  X10_Controller controller = X10_Controller();
  
  /* @Completed Implementation.
  X10_Code *code = (X10_Code *)malloc(sizeof(X10_Code));
  code->construct_packet(HOUSE_A, KEY_2, ON);
  
  controller.transmit_code(code);
  */
  /*Custom_deque d = Custom_deque(16);

  d.push_back(32);
  d.push_back(16);
  d.push_back(64);

  Custom_deque b = Custom_deque(16);

  b.push_back(32);
  b.push_back(16);
  b.push_back(64);

  for(int i = 0; i < 20; i++) {
    b.push_back(i);
    d.push_back(i);
  }

  b.push_back(13);

  cout << d.size() << endl;
  cout << b.size() << endl;
  cout << d.equals(b) << endl;*/

  while(1) {
    if(controller.idle()) { // @TODO: Test idle().
		controller.receive_code();
	}
  }

  return 0;
}
