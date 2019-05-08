#include <iostream>
#include <deque>
#include <bitset>

int main() {

  std::deque<char unsigned> test;

  // House Code Test
  test.push_back(0x0);
  test.push_back(0x1);
  test.push_back(0x1);
  test.push_back(0x0);
  test.push_back(0x1);
  test.push_back(0x0);
  test.push_back(0x0);
  test.push_back(0x1);

  // Key Code
  test.push_back(0x0);
  test.push_back(0x1);
  test.push_back(0x1);
  test.push_back(0x0);
  test.push_back(0x0);
  test.push_back(0x1);
  test.push_back(0x1);
  test.push_back(0x0);
  test.push_back(0x0);
  test.push_back(0x1);

  // Function code
  test.push_back(0x0);
  test.push_back(0x1);
  test.push_back(0x1);
  test.push_back(0x0);
  test.push_back(0x0);
  test.push_back(0x1);
  test.push_back(0x1);
  test.push_back(0x0);
  test.push_back(0x0);
  test.push_back(0x1);

  char unsigned current_bit;
  char unsigned next_bit;
  std::deque<char unsigned> result;

  while(!test.empty()) {
    current_bit = test.front();
    test.pop_front();
    next_bit = test.front();
    test.pop_front();
    if(current_bit == 0x1) {
      if(next_bit == 0x0) {
	result.push_back(current_bit);
      }
    } else if (current_bit == 0x0) {
      if(next_bit == 0x1) {
	result.push_back(current_bit);
      }
    }
  }

  // @TODO:
  // Decoding bits work, now we want to make bit-strings
  // so we can compare to our X10 constants. -bjarke, 7th May 2019.
  /*char unsigned front;

  while(!result.empty()) {
    front = result.front();
    result.pop_front();
    std::cout << static_cast<unsigned>(front) << std::endl;
    }*/

  char unsigned hc;
  char unsigned kc;
  char unsigned fc;

  int i = 0;
  int j = 0;
  int k = 0;
  
  for(std::deque<char unsigned>::reverse_iterator rit = result.rbegin(); rit != result.rend(); ++rit) {
    if(i < 6) {
      fc ^= (-*rit ^ fc) & (1 << i);
      i++;
    }
    if (i > 5 && j < 6) {
      kc ^= (-*rit ^ kc) & (1 << j);
      j++;
    }
    if(j > 5 && k < 6) {
      hc ^= (-*rit ^ hc) & (1 << k);
      k++;
    }
  }

  std::cout << "HOUSE_CODE is: " << static_cast<unsigned>(hc) << std::endl;
  std::cout << "NUMBER CODE is: " << static_cast<unsigned>(kc) << std::endl;
  std::cout << "FUNCTION_CODE is: " << static_cast<unsigned>(fc) << std::endl;
  
  return 1;
}
