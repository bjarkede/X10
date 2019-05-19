#include "X10.hpp"

//static bdeque_type *encoded_packet;    // This packet is empty unless we are sending an X10 Command.
//static bdeque_type *lpf_buffer;       // This buffer is loaded with bits received at 120khz.
//static bdeque_type *hpf_buffer;     	  // This buffer is loaded with bits received at 300khz.
//static bdeque_type *compare_deque;  

bool signal_state = true;                 // We use this to determine if to send HIGH or LOW in ISR.
bool is_equal_lpf = false;
bool is_equal_hpf = false;

state global_state;

Custom_deque lpf_buffer;
Custom_deque lpf_receive_buffer;
Custom_deque hpf_buffer;
Custom_deque encoded_packet;
Custom_deque compare_deque_unique_code;
Custom_deque error_buffer;

X10_Controller::X10_Controller() {
	DDRB = 0B11111100;
	DDRD = 0B11111000;
	DDRA = 0B00000000;
	X10_state = IDLE;
	
	_house_code    = 0x0;
	_number_code   = 0x0;
	_function_code = 0x0;
	
	//encoded_packet = bdeque_alloc();
	//lpf_buffer = bdeque_alloc(); 
	//hpf_buffer = bdeque_alloc(); 
	//compare_deque = bdeque_alloc(); 
	
	lpf_buffer          = Custom_deque(4);
	lpf_receive_buffer  = Custom_deque(64); // 56 bits for code, 4 bits stop code
	compare_deque_unique_code = Custom_deque(4);
	error_buffer = Custom_deque(4);

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

void X10_Controller::set_state(state new_state) {
	this->X10_state = new_state;
}

/*void X10_Code::construct_packet(char unsigned hc, char unsigned nc, char unsigned fc) {
	packet = bdeque_alloc();
	for(int i = 0; i < 2; ++i) {
		bdeque_push_back(packet, hc);
		bdeque_push_back(packet, nc);
		bdeque_push_back(packet, fc);
	}
}

void X10_Controller::transmit_code(X10_Code* code) {
  this->set_state(SENDING);
  global_state = SENDING;

  bool continous_flag = false;
  char unsigned current_bit;

  // Push our start code to the encoded_packet.
  bdeque_push_back(encoded_packet, 0x0);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
 
  // Encode our X10_Scheme to manchester.
  while(!bdeque_is_empty(code->packet)) {
    if(!continous_flag && bdeque_size(code->packet) == 3) {
      for(int i = 0; i < 6; ++i) {
	bdeque_push_back(encoded_packet, 0x0);
      }
    }
    
    for(int i = amount_of_bits(bdeque_size(code->packet)); i >= 0 ; --i) {
      if(bdeque_peek_front(code->packet) == BRIGHT || bdeque_peek_front(code->packet) == DIM) { continous_flag = true; }
      
      current_bit = (bdeque_peek_front(code->packet) >> i) & 0x1;
      bdeque_push_back(encoded_packet, current_bit);
      bdeque_push_back(encoded_packet, current_bit^1);
    }
    
    bdeque_pop_front(code->packet); // Move onto next instruction
  }

  
  // Add the STOP_CODE to our encoded_packet..
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x1);
  bdeque_push_back(encoded_packet, 0x0);
  
  // Start the external interrupt/TIMER0 now that we have parsed the bits.
  INT0_init();
  TIMER0_init();
  TIMER2_init();
  TIMER1_init(); // Used for 1ms delays
  sei();

  START_INT0_INTERRUPT;
  
  // Then wait until the global packet is empty...
  while(!bdeque_is_empty(encoded_packet)) {
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
*/

void X10_Controller::receive_code() {
  this->set_state(RECEIVING);
  global_state = RECEIVING;
  
  bool is_equal_stop = false;
  
  while(!is_equal_stop) {
    if (lpf_receive_buffer.size() > 50) {
		if(lpf_receive_buffer.compare_last_four(compare_deque_unique_code)) {
			is_equal_stop = true;
		}
		if(lpf_receive_buffer.compare_last_four(error_buffer)) {
			this->set_state(ERROR);
			return;
		}
	}
  }

  STOP_INT0_INTERRUPT;
  STOP_TIMER1;
  cli();
  
  for(int i = 0; i < 4; i++) {
	  lpf_receive_buffer.pop_back();
  }

  decode_manchester_deque(lpf_receive_buffer);
  
  global_state = IDLE;
  this->set_state(IDLE);
  return;
}

bool X10_Controller::idle() {
  this->set_state(IDLE);
  global_state = IDLE;

  // Make sure that the buffers are empty.
  lpf_buffer.clear();

  INT0_init();
  TIMER1_init();
  sei();

  START_INT0_INTERRUPT;

  while(!is_equal_lpf) {
	// Do nothing.
  }
  
  lpf_buffer.clear();
    
  return true;
}

void X10_Controller::decode_manchester_deque(Custom_deque &d) {
  if(!d.is_symmetrical()) {
	this->set_state(ERROR);
	return;	  
  }
  
  _house_code    = 0x0;
  _number_code   = 0x0;
  _function_code = 0x0;
 
  char unsigned current_bit;
  char unsigned next_bit;
  
  const int house_code_offset = 3;
  const int number_code_offset = 7;
  const int function_code_offset = 11;
  
  PORTB |= 1 << 3;

  for (int i = 0; i < 14; i++) {
  current_bit = d.get_element_at(i*2);
  next_bit    = d.get_element_at((i*2)+1);
	  
	  if (current_bit == 0x1) 
	  {
		if (next_bit == 0x0) {
			if (i <= house_code_offset)	
				_house_code |= (1 << (house_code_offset-i));
				
			if (i > house_code_offset && i < house_code_offset + number_code_offset)
				_number_code |= (1 << (number_code_offset-(i-house_code_offset)));
				
			if (i > house_code_offset && i < function_code_offset)
				_function_code |= (1 << (house_code_offset-(i- function_code_offset)));
		}
	  }
	  if (current_bit == 0x0)
	  {
		  if (next_bit == 0x1)
		  {
			  if (i <= house_code_offset)
			  _house_code |= (0 << (house_code_offset-i));
			  
			  if (i > house_code_offset && i < house_code_offset + number_code_offset)
			  _number_code |= (0 << (number_code_offset-(i-house_code_offset)));
			  
			  if (i > house_code_offset && i < function_code_offset)
			  _function_code |= (0 << (house_code_offset-(i- function_code_offset)));
		  }
	  }
  }
  
  if (_house_code == HOUSE_A)
	PORTB |= 1 << 2;
  
 return;
}

/*bdeque_type * convert_to_binary_string(bdeque_type *d) {

  char unsigned hc;
  char unsigned kc;
  char unsigned fc;

  bdeque_type *result = bdeque_alloc();
  
  int i = 0;
  int j = 0;
  int k = 0;
  
  struct node *n = d->tail;
  
  //for(std::deque<char unsigned>::reverse_iterator rit = d1.rbegin(); rit != d1.rend(); ++rit) {
  while(n->prev != NULL) {  
    if(i < 6) {
      fc ^= (-n->val ^ fc) & (1 << i);
      i++;
    }
    if (i > 5 && j < 6) {
      kc ^= (-n->val ^ kc) & (1 << j);
      j++;
    }
    if(j > 5 && k < 6) {
      hc ^= (-n->val ^ hc) & (1 << k);
      k++;
    }
    
    n = n->next;
  }
  
  bdeque_free(d);
  
  bdeque_push_back(result, hc);
  bdeque_push_back(result, kc);
  bdeque_push_back(result, fc);
  
  return result;
  }*/

ISR(INT0_vect) {
  char unsigned current_bit;	
		
  /*if(!bdeque_is_empty(encoded_packet) && global_state == SENDING) {
    current_bit = bdeque_peek_front(encoded_packet);

    START_TIMER1; // This creates an interrupt after 1ms.
   
    if(current_bit == 0x1) {
		TIMSK0 |= 1<<OCIE0A;
		START_TIMER0;
    }
	
	if(current_bit == 0x0) {
	}
   
    // On the next interrupt, transmit the next bit, by removing this one.
  }*/

	/*
  if(!bdeque_is_empty(encoded_packet) && global_state == ERROR) {
    current_bit = bdeque_peek_front(encoded_packet);

    START_TIMER1; // Creates an interrupt after 1ms.

    if(current_bit == 0x1) {
		TIMSK2 |= 1<<OCIE2A;
		START_TIMER2;
    }
  }*/
  // TODO:
  // Implement logic in this state, for the hpf_buffer. -bjarke, 18th May 2019.
  if(global_state == IDLE) {
	if(PINA & (1 << 0)) { // If there is a 1 on this PIN, load 1 into LPF.
		lpf_buffer.push_back(0x1);
    } else {
		lpf_buffer.push_back(0x0);
	}
	
	if(lpf_buffer.size() == compare_deque_unique_code.size()) {
		if(lpf_buffer.equals(compare_deque_unique_code)) {
			is_equal_lpf = true;
			global_state = RECEIVING;
		}
	}
  }
  else if(global_state == RECEIVING) {
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
		//bdeque_pop_front(encoded_packet);
	case IDLE:
		// Do nothing.
	case ERROR:
	default:
		break;
		// Do nothing.
  }
}

ISR(TIMER0_COMPA_vect) {
  //PORTB ^= 1 << 1;
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
	OCR2A = 27; // 16 MHz / (2 * prescaler (1) * desired_frequency) = 26.67
}

void INT0_init() {
	SET_INT0_SENSE_CONTROL;
	START_INT0_INTERRUPT;
}

int amount_of_bits(int n) {
	int result = 0;
	
	if(n == 6 || n == 3) {
		result = 3;
		} else {
		result = 4;
	}

	return result;
}
