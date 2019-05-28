#include "X10.hpp"
#include "uart_int.hpp"

#define SET_INT1_SENSE_CONTROL EICRA |= (1<<ISC11)|(1<<ISC10) // Rising edge
#define START_INT1_INTERRUPT EIMSK   |= (1<<INT1)
#define STOP_INT1_INTERRUPT EIMSK    &= ~(1<<INT1)

void INT1_init() {
	SET_INT1_SENSE_CONTROL;
	START_INT1_INTERRUPT;
}

Custom_deque UART_buffer = Custom_deque(14);
X10_Code* INT1_code = (X10_Code *)malloc(sizeof(X10_Code));

// Init our X10 Controller for this device
X10_Controller controller = X10_Controller();

int main(int argc, char* argv[]) {
	
	InitUART(9600, 8, 'N', 1);
	
	// We specify what we are:
	char unsigned HOUSE = HOUSE_A;
	char unsigned UNIT  = KEY_1;
	
	X10_Code * code = (X10_Code *)malloc(sizeof(X10_Code)); // We re-use this memory when we send.
	
	INT1_init();
	
	while(1) {
		// Enter this statement, if we have received X10_Code from our interface.
		if(UART_buffer.size() == 3) {
			code->construct_packet(UART_buffer.get_element_at(0), UART_buffer.get_element_at(1), UART_buffer.get_element_at(2));
			controller.transmit_code(code);
			
			code->packet.clear(); // Clear this so we can construct the next code.
			UART_buffer.clear();  // Clear this so we can load new X10_Code from interface.
			
		}
		
		// Check if our controller have received the start_code, if it has receive_code().
		if(controller.idle()) {
			controller.receive_code();
			if(controller.get_house_code() == HOUSE && controller.get_number_code() == UNIT) {
				if(controller.get_function_code() == COFFEE_DONE)
				{
				}
			}
		}
	}
	
	return 0;
}

ISR(INT1_vect)
{
	INT1_code->construct_packet(HOUSE_A, KEY_2, BREW_COFFEE);
	controller.transmit_code(INT1_code);
	INT1_code->packet.clear(); // Clear this so we can built a new packet.
	
	INT1_code->construct_packet(HOUSE_A, KEY_2, ALL_LIGHTS_ON);
	controller.transmit_code(INT1_code);
	INT1_code->packet.clear(); // Clear for next interrupt.
}

ISR(USART0_RX_vect) {
	unsigned char rx_value = UDR0;
	UART_buffer.push_back(rx_value);
}