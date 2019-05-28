#include "X10.hpp"

volatile bool is_equal_lpf  = false;
volatile bool is_equal_hpf  = false;
volatile bool is_equal_stop = false;
volatile bool cleared_receive    = false;
volatile bool cleared_idle       = false;

volatile state global_state;

Custom_deque lpf_buffer;
Custom_deque lpf_receive_buffer;
Custom_deque hpf_buffer;
Custom_deque encoded_packet;
Custom_deque compare_deque_unique_code;
Custom_deque error_buffer;

X10_Controller::X10_Controller() {
	DDRB                      = 0B11111100;
	DDRD                      = 0B11111000;
	DDRA                      = 0B00000000;
	X10_state                 = IDLE;
	
	_house_code               = 0x0;
	_number_code              = 0x0;
	_function_code            = 0x0;

	lpf_buffer                = Custom_deque(4);
	lpf_receive_buffer        = Custom_deque(64); // 56 bits for code, 4 bits stop code
	compare_deque_unique_code = Custom_deque(4);
	error_buffer              = Custom_deque(4);
	encoded_packet            = Custom_deque(64);

	// We need this to compare
	compare_deque_unique_code.push_back(0x0);
	compare_deque_unique_code.push_back(0x1);
	compare_deque_unique_code.push_back(0x1);
	compare_deque_unique_code.push_back(0x1);
	
	error_buffer.push_back(0x0);
	error_buffer.push_back(0x0);
	error_buffer.push_back(0x0);
	error_buffer.push_back(0x0);
}

state X10_Controller::get_state(X10_Controller* controller) const {
	return this->X10_state;
}

char unsigned X10_Controller::get_house_code() const {
	return this->_house_code;
}

char unsigned X10_Controller::get_number_code() const {
	return this->_number_code;
}

char unsigned X10_Controller::get_function_code() const {
	return this->_function_code;
}

void X10_Controller::set_state(state new_state) {
	this->X10_state = new_state;
}

void X10_Code::construct_packet(char unsigned hc, char unsigned nc, char unsigned fc) {
	packet = Custom_deque(6);
	for(int i = 0; i < 2; i++) {
		packet.push_back(hc);
		packet.push_back(nc);
		packet.push_back(fc);
	}
}

void X10_Controller::transmit_code(X10_Code* code) {
  this->set_state(SENDING);
  global_state = SENDING;

  char unsigned current_bit;

  // Push our unique_code to the encoded_packet.
  encoded_packet.push_back(0x0);
  encoded_packet.push_back(0x1);
  encoded_packet.push_back(0x1);
  encoded_packet.push_back(0x1);
 
  // Encode our X10_Scheme to manchester.
  while(!code->packet.is_empty()) {
    for(int i = 0; i < amount_of_bits(code->packet.size()); i++) {
      current_bit = (code->packet.peek_front() >> ((amount_of_bits(code->packet.size())-1)-i)) & 1;
      encoded_packet.push_back(current_bit);
      encoded_packet.push_back(current_bit^1);
    }
    
    code->packet.pop_front(); // Move onto next instruction
  }

  // Add the unique_code to our encoded_packet..
  encoded_packet.push_back(0x0);
  encoded_packet.push_back(0x1);
  encoded_packet.push_back(0x1);
  encoded_packet.push_back(0x1);
  
  // Start the external interrupt/TIMER0 now that we have parsed the bits.
  INT0_init();
  TIMER0_init();
  TIMER2_init();
  TIMER1_init(); // Used for 1ms delays
  sei();

  START_INT0_INTERRUPT;
  
  // Then wait until the global packet is empty...
  while(!encoded_packet.is_empty()) {
    // Do nothing.
  }

  // We finished transmitting so disable INT0 and TIMER0 
  STOP_INT0_INTERRUPT;
  STOP_TIMER0;
  cli();

  global_state = IDLE;
  this->set_state(IDLE);
  return;
}

bool X10_Controller::receive_code() {
  if(!cleared_receive) {
	this->set_state(RECEIVING);
	global_state = RECEIVING;
  
	lpf_receive_buffer.clear();
	
	cleared_receive = true;
  }
  
if (lpf_receive_buffer.size() > 56) {
	if(lpf_receive_buffer.compare_last_four(compare_deque_unique_code)) {
		is_equal_stop = true;
	}
	if(lpf_receive_buffer.compare_last_four(error_buffer)) {
		this->set_state(ERROR);
	}
} 
  if (!is_equal_stop) return false; 

  for(int i = 0; i < 4; i++) {
	  lpf_receive_buffer.pop_back();
  }
  
  decode_manchester_deque(lpf_receive_buffer);
  
  lpf_receive_buffer.clear();
  
  cleared_receive = false;
  is_equal_lpf = false;
  
  global_state = IDLE;
  this->set_state(IDLE);
  return true;
}

bool X10_Controller::idle() {
  // Make sure that the buffers are empty.
  if(!cleared_idle && !is_equal_lpf) {
	this->set_state(IDLE);
	global_state = IDLE;

	lpf_buffer.clear();

	INT0_init();
	TIMER1_init();
	sei();

	START_INT0_INTERRUPT;

	cleared_idle = true;
  }

  if (!is_equal_lpf) {
	return false;
  }
  
  lpf_buffer.clear();
  
  cleared_idle = false;
    
  return true;
}

void X10_Controller::decode_manchester_deque(Custom_deque &d) {
  /*for(int i = 0; i < d.size(); i++) {
	  SendChar(d.get_element_at(i));
  }	*/
  //SendInteger(d.size());
  if(!d.is_symmetrical()) {
	this->set_state(ERROR);
	return;	  
  }
  //SendInteger(2);
  
  _house_code    = 0x0;
  _number_code   = 0x0;
  _function_code = 0x0;
 
  char unsigned current_bit;
  char unsigned next_bit;
  
  const int house_code_offset = 3;
  
  int k = 4;
  int j = 4;

  for (int i = 0; i < 14; i++) {
  current_bit = d.get_element_at(i*2);
  next_bit    = d.get_element_at((i*2)+1);
	  
	  if (current_bit == 0x1) 
	  {
		if (next_bit == 0x0) {
			if (i <= house_code_offset)	
				_house_code |= (1 << (house_code_offset-i));
				
			if (i > house_code_offset && i < 9) {
				_number_code |= (1 << k);
				k--;
			}
			if (i > 8 && i < 14) {
				_function_code |= (1 << j);
				j--;
			}
		}
	  }
	  if (current_bit == 0x0)
	  {
		  if (next_bit == 0x1)
		  {
			  if (i <= house_code_offset)
			  _house_code |= (0 << (house_code_offset-i));
			  
			  if (i > house_code_offset && i < 9) {
				_number_code |= (0 << k);
				k--;
			  }
			  
			  if (i > 8 && i < 14) {
			  _function_code |= (0 << j);
				j--;
			  }
		  }
	  }
  }
  
  SendChar(_house_code);
  SendChar(_number_code);
  SendChar(_function_code);
  
 return;
}

ISR(INT0_vect) {
  volatile char unsigned current_bit;	

  PORTB |= 0 << 1;
		
  if(!encoded_packet.is_empty() && global_state == SENDING) {
    current_bit = encoded_packet.peek_front();

    START_TIMER1; // This creates an interrupt after 1ms.
   
    if(current_bit == 0x1) {
		TIMSK0 |= 1<<OCIE0A;
		START_TIMER0;
    }
	
	if(current_bit == 0x0) { 
		// Do nothing. 
	}
		
  }

	/*
  if(!bdeque_is_empty(encoded_packet) && global_state == ERROR) {
    current_bit = bdeque_peek_front(encoded_packet);

    START_TIMER1; // Creates an interrupt after 1ms.

    if(current_bit == 0x1) {
		TIMSK2 |= 1<<OCIE2A;
		START_TIMER2;
    }
  }*/
 
  if(global_state == IDLE) {
	if(PINA & (1 << 0)) { // If there is a 1 on this PIN, load 1 into LPF.
		lpf_buffer.push_back(0x1);
    } else {
		lpf_buffer.push_back(0x0);
	}
	
	if(lpf_buffer.size() == compare_deque_unique_code.size()) {
		if(lpf_buffer.equals(compare_deque_unique_code)) {
			is_equal_lpf = true;
		}
	}
  } else if(global_state == RECEIVING) {
		if(PINA & (1 << 0)) { // If there is a 1 on this PIN, load 1 into LPF.
			lpf_receive_buffer.push_back(0x1);
		} else {
			lpf_receive_buffer.push_back(0x0);
	    }
  }  
} 

ISR(TIMER1_COMPA_vect) {
  STOP_TIMER1; // Stop TIMER1, since 1 ms has passed.
  STOP_TIMER0; // Likewise stop TIMER0, since we no longer want to transmit the bit.
  STOP_TIMER2; // Stop this as well if we are sending at 300 KHz.
  
  TIMSK0 &= ~(1<<OCIE0A);
  TIMSK2 &= ~(1<<OCIE2A);
  
  switch(global_state) {
	case SENDING:
		encoded_packet.pop_front();
		PORTB &= ~(1 << 1);
	case IDLE:
		// Do nothing.
	case ERROR:
	default:
		break;
		// Do nothing.
  }
}

ISR(TIMER0_COMPA_vect) {
  PORTB ^= 1 << 1;
}

ISR(TIMER2_COMPA_vect) {
  //PORTB ^= 1 << 1;
}

void TIMER0_init() {
	  SET_TIMER0_WAVEFORM;
	  SET_TIMER0_MASK;
	  TCNT0 = 0;
	  OCR0A = 65; // See documentation for this value...
}

void TIMER1_init() {
	  TCCR1B |= (1 << WGM12);  // Set CTC Mode
	  TCNT1   = 0;
	  TIMSK1 |= (1 << OCIE1A); // Interrupt enable
	  OCR1A   = 15999;         // It takes roughly 1 ms to reach this
}

void TIMER2_init() {
	SET_TIMER2_WAVEFORM;
	SET_TIMER2_MASK;
	TCNT2 = 0;
	OCR2A = 7; // 16 MHz / (2 * prescaler (1) * desired_frequency) = 6.66667
}

void INT0_init() {
	SET_INT0_SENSE_CONTROL;
	START_INT0_INTERRUPT;
}

int amount_of_bits(int n) {
	int result = 0;
	
	if(n == 6 || n == 3) {
		result = 4;
		} else {
		result = 5;
	}

	return result;
}
