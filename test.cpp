#include <iostream>
#include "bdeque.cpp"

using namespace std;

int amount_of_bits(int n) {
	int result = 0;
	
	if(n == 6 || n == 3) {
		result = 4;
	} else {
		result = 5;
	}

	return result;
}


int main () {

  Custom_deque lol = Custom_deque(60);

  Custom_deque packet = Custom_deque(12);
  for(int i = 0; i < 2; i++) {
    packet.push_back(0B0110);
    packet.push_back(0B00111);
    packet.push_back(0B10100);
  }

  unsigned char current_bit;

  while(!packet.is_empty()) {
    for(int i = 0; i < amount_of_bits(packet.size()); i++) {
      current_bit = (packet.peek_front() >> i) & 1;
    
      lol.push_back(current_bit);
      lol.push_back(current_bit^1);
      }
    
    packet.pop_front(); // Move onto next instruction
  }

  for(int i = 0; i < lol.size(); i++) {
    cout << static_cast<int>(lol.get_element_at(i)) << endl;
  }
  
  cout << "Size of encoded_packet is: " << lol.size() << endl;
  
  return 1;
}
