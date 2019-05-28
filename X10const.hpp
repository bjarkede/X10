#pragma once

// Figure 5. House Code and Key Code Tables
// http://jvde.us/info/x10_protocol.pdf

#define START_CODE 0B0111
#define STOP_CODE  0B1110

// X10 House Codes

#define HOUSE_A 0B0110
#define HOUSE_B 0B0111
#define HOUSE_C 0B0100
#define HOUSE_D 0B1000
#define HOUSE_E 0B0101
#define HOUSE_F 0B1001
#define HOUSE_G 0B1010
#define HOUSE_H 0B1011
#define HOUSE_I 0B1110
#define HOUSE_J 0B1111
#define HOUSE_K 0B1100
#define HOUSE_L 0B1101
#define HOUSE_M 0B0000
#define HOUSE_N 0B0001
#define HOUSE_O 0B0010
#define HOUSE_P 0B0011

// X10 Key Codes

#define KEY_1  0B00110
#define KEY_2  0B00111
#define KEY_3  0B00100
#define KEY_4  0B00101
#define KEY_5  0B01000
#define KEY_6  0B01001
#define KEY_7  0B01010
#define KEY_8  0B01011
#define KEY_9  0B01110
#define KEY_10 0B01111
#define KEY_11 0B01100
#define KEY_12 0B01101
#define KEY_13 0B00000
#define KEY_14 0B00001
#define KEY_15 0B00010
#define KEY_16 0B00011

// X10 Commands / Function codes

#define ALL_UNITS_OFF	 0B10000
#define ALL_LIGHTS_ON	 0B11000
#define ON	       	 0B10100
#define OFF	         0B11100
#define DIM		 0B10010
#define BRIGHT		 0B11010
#define ALL_LIGHTS_OFF	 0B10110
#define EXTENDED_CODE	 0B11110
#define BREW_COFFEE	 0B10001
#define COFFEE_DONE  0B11001
#define PRE_SET_DIM	 0B10101
#define TIME_OFF	 0B10011
#define TIME_1	 0B11011
#define TIME_2	 0B10111
#define TIME_3	 0B11111
