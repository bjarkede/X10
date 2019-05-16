#include "X10.hpp"

bdeque_type *encoded_packet = bdeque_alloc(); // This packet is empty unless we are sending an X10 Command.
bdeque_type *lpf_buffer     = bdeque_alloc();     // This buffer is loaded with bits received at 120khz.
bdeque_type *hpf_buffer     = bdeque_alloc();	  // This buffer is loaded with bits received at 300khz.

bool signal_state = true;                 // We use this to determine if to send HIGH or LOW in ISR.
int lpf_prev_size = 0;
int hpf_prev_size = 0;

state global_state;

X10_Controller::X10_Controller() {
	DDRB = 0B11111110;
	DDRD = 0B11111111;
	X10_state = IDLE;
}

state X10_Controller::get_state(X10_Controller* controller) const {
	return this->X10_state;
}

void X10_Controller::set_state(state new_state) {
	this->X10_state = new_state;
}

void X10_Code::construct_packet(char unsigned hc, char unsigned nc, char unsigned fc) {
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
  
  PORTB |= 1 << 5;

  global_state = IDLE;
  this->set_state(IDLE);
  return;
}

bdeque_type* X10_Controller::receive_code() {
  this->set_state(RECEIVING);
  global_state = RECEIVING;

  // Start the external interrupt.
  INT0_init();
  TIMER1_init(); // We use this to time the length of the bursts.
  sei();

  START_INT0_INTERRUPT;

  // We need to check if the last four bits of one of the buffers
  // is equal to the stop_code.
  bdeque_type *compare_deque = bdeque_alloc();
  bdeque_push_back(compare_deque, 0x1);
  bdeque_push_back(compare_deque, 0x1);
  bdeque_push_back(compare_deque, 0x1);
  bdeque_push_back(compare_deque, 0x0);
  
  bool is_equal_stop = false;
  
  while(!is_equal_stop) {
    is_equal_stop = compare_to_stop_code(lpf_buffer, compare_deque);
  }
  
  free(compare_deque); // Free this memory

  // Stop when we have seen the stop code
  STOP_INT0_INTERRUPT;
  STOP_TIMER1;
  cli();
  
  // Resize the vectors, to remove the stop-code.
  bdeque_resize(lpf_buffer, bdeque_size(lpf_buffer) - 4);
  bdeque_resize(hpf_buffer, bdeque_size(hpf_buffer) - 4);

  // @TODO:
  // We need to implement the functions used below to our new datastructure -bjarke, 16th May 2019.
  if(!split_and_compare_bits(decode_manchester_deque(lpf_buffer))) {
    // @Incomplete:
    // The two messages are not equal to eachother, we
    // need to handle this somehow, and send a messeage back that we got an error. -bjarke, 9th May 2019.
  }
  
  if(!split_and_compare_bits(decode_manchester_deque(hpf_buffer))) {

  }
  
  global_state = IDLE;
  this->set_state(IDLE);
  return hpf_buffer; // @TODO return the right thing
  }

/*bool X10_Controller::idle() {
  this->set_state(IDLE);
  global_state = IDLE;

  // @TODO:
  // We might want to make sure that the buffers are empty when we enter idle. -bjarke, 15th May 2019.

  INT0_init();
  sei();

  START_INT0_INTERRUPT;

  // We compare the lpf/hpf buffers 4 bits to the start code.
  std::deque<char unsigned> compare_deque;
  compare_deque.push_back(0x0);
  compare_deque.push_back(0x1);
  compare_deque.push_back(0x1);
  compare_deque.push_back(0x1);
  
  bool is_equal_lpf = false;
  bool is_equal_hpf = false;

  while(!is_equal_lpf || !is_equal_hpf) {
    if(lpf_buffer.size() > 4 && hpf_buffer.size() > 4) {
      // Maintain 4 bits while idle untill the start_code is registered.
      lpf_buffer.pop_front();
      hpf_buffer.pop_front();
    }
	  
    if(compare_deque.size() == lpf_buffer.size() || compare_deque.size() == hpf_buffer.size()) {
      is_equal_lpf = std::equal(compare_deque.begin(), compare_deque.end(), lpf_buffer.begin());
      is_equal_hpf = std::equal(compare_deque.begin(), compare_deque.end(), hpf_buffer.begin());
    }
  }

  if(is_equal_lpf || is_equal_hpf) {
    STOP_INT0_INTERRUPT;
    cli();
    
    lpf_buffer.clear();
    hpf_buffer.clear();
    
    return false; // We received the start-code and need to receive.
  } else { return true; }
  }*/

/*bdeque_type * decode_manchester_deque(bdeque_type *d) {
  char unsigned current_bit;
  char unsigned next_bit;
  bdeque_type *result = bdeque_alloc();

  while(!bdeque_is_empty(d1)) {
    current_bit = bdeque_pop_front(d1);
    next_bit = bdeque_pop_front(d1);

    if(current_bit == 0x1) {
      if(next_bit == 0x0) {
	bdeque_push_back(result, 0x1);
      }
    } else if (current_bit == 0x0) {
      if(next_bit == 0x1) {
	bdeque_push_back(result, 0x0);
      }
    }
  }

  bdeque_free(d1);
  return result;
  }*/

/*bdeque_type * convert_to_binary_string(bdeque_type *d) {

  char unsigned hc;
  char unsigned kc;
  char unsigned fc;

  bdeque_type *result = bdeque_alloc();
  
  int i = 0;
  int j = 0;
  int k = 0;
  
  struct node *n = result->head;
  
  //for(std::deque<char unsigned>::reverse_iterator rit = d1.rbegin(); rit != d1.rend(); ++rit) {
  while(n != d1->tail) {  
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
  
  bdeque_free(d1);
  bdeque_push_back(result, hc);
  bdeque_push_back(result, kc);
  bdeque_push_back(result, fc);
  
  return result;
  }*/

bool compare_to_stop_code(bdeque_type *d1, bdeque_type *d2) {
	struct node *n1 = d1->tail;
	struct node *n2 = d2->tail;
	for(int i = 0; i < 4; i++) {
		if(n1->val != n2->val) {
			free(n1);
			free(n2);
			return false;
		}
		n1 = n1->prev;
		n2 = n2->prev;
	}
	free(n1);
	free(n2);
	return true;
}

ISR(INT0_vect) {
  char unsigned current_bit;		
		
  //PORTB |= 1 << 1;		
		
  if(!bdeque_is_empty(encoded_packet) && global_state == SENDING) {
    current_bit = bdeque_peek_front(encoded_packet);

    START_TIMER1; // This creates an interrupt after 1ms.
   
    if(current_bit == 0x1) {
		PORTB |= 1 << 2;
		START_TIMER0;
    }
	
	if(current_bit == 0x0) {
		PORTB |= 1 << 3;	
	}
   
    // On the next interrupt, transmit the next bit, by removing this one.
  }

  if(!bdeque_is_empty(encoded_packet) && global_state == ERROR) {
    // @Incomplete:
    // If we get an error in transmission, and need to send something back to stop the transmission
    // We enter this statement, and transmit the message at 300 kHz. -bjarke, 9th May 2019.
    current_bit = bdeque_peek_front(encoded_packet);

    START_TIMER1; // Creates an interrupt after 1ms.

    while(((TCCR1B >> CS10) & 1) == 1) {
      if(current_bit == 0x1) {
	START_TIMER2;
      }
    }

    bdeque_pop_front(encoded_packet);
  }

  if(global_state == RECEIVING) {

    START_TIMER1;

    lpf_prev_size = bdeque_size(lpf_buffer);
    hpf_prev_size = bdeque_size(hpf_buffer);

	if(((PINB >> 1) & 1) == 1) { // If there is a 1 on this PIN, load 1 into LPF.
		bdeque_push_back(lpf_buffer, 0x1);
    }
    if(((PINB >> 2) & 1) == 1) { // If there is a 1 on this PIN, load 1 into HPF.
        bdeque_push_back(hpf_buffer, 0x1);
    }

  }
} 

ISR(TIMER1_COMPA_vect) {
  STOP_TIMER1; // Stop TIMER1, since 1 ms has passed.
  STOP_TIMER0; // Likewise stop TIMER0, since we no longer want to transmit the bit.
  STOP_TIMER2; // Stop this as well if we are sending at 220 KHz.
  
  switch(global_state) {
	case SENDING:
		bdeque_pop_front(encoded_packet);
	case RECEIVING:
		// If the size of the buffer didn't, this means that we didn't receive a 1.
		// Therefore we load a 0 into the buffer.
		if(lpf_prev_size == bdeque_size(lpf_buffer)) {
			bdeque_push_back(lpf_buffer, 0x0);
		}
		if(hpf_prev_size == bdeque_size(hpf_buffer)) {
			bdeque_push_back(hpf_buffer, 0x0);
		}
	case ERROR:
	default:
		break;
		// Do nothing
  }
	 
  // There are some things we want to make sure are stopped here..
  PORTB &= ~1 << 0; // Sending HIGH on output
  
  // These are for debugging...
  PORTB &= ~1 << 4;
  PORTB &= ~1 << 2;
  PORTB &= ~1 << 3;
}

ISR(TIMER0_COMPA_vect) {
  PORTB |= 1 << 4;
  if(signal_state) {
    signal_state = false; 
    PORTB |= 1 << 0;
  } else {
    signal_state = true;
    PORTB &= ~1 << 0;
  }
}

ISR(TIMER2_COMPA_vect) {
   if(signal_state) {
    signal_state = false; 
    PORTB |= 1 << 0;
  } else {
    signal_state = true;
    PORTB &= ~1 << 0;
  }
}

void TIMER0_init() {
	  SET_TIMER0_WAVEFORM;
	  SET_TIMER0_MASK;
	  OCR0A = 65; // See documentation for this value...
}

void TIMER1_init() {
	  TCCR1B |= (1 << WGM12);  // Set CTC Mode
	  TIMSK1 |= (1 << OCIE1A); // Interrupt enable
	  OCR1A   = 15999;         // It takes roughly 1 ms to reach this
}

void TIMER2_init() {
	SET_TIMER2_WAVEFORM;
	SET_TIMER2_MASK;
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