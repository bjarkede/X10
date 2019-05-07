#include <iostream>
#include <deque>

int main() {

  std::deque<char unsigned> test;
  test.push_back(0x1);
  test.push_back(0x0);
  test.push_back(0x0);
  test.push_back(0x1);
  test.push_back(0x1);
  test.push_back(0x0);
  test.push_back(0x1);
  test.push_back(0x0);

  char unsigned current_bit;
  char unsigned next_bit;

  while(!test.empty()) {
    current_bit = test.front();
    test.pop_front();
    next_bit = test.front();
    test.pop_front();
    std::cout << static_cast<unsigned>(current_bit) << " : " << static_cast<unsigned>(next_bit) << std::endl;
    if(current_bit == 0x1) {
      if(next_bit == 0x0) {
	std::cout << "HIGH-to-LOW transition is a 1." << std::endl;
      }
    } else if (current_bit == 0x0) {
      if(next_bit == 0x1) {
	std::cout << "LOW-to-HIGH transition is a 0." << std::endl;
      }
    }
  }
  
  return 1;
}
