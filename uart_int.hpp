/****************************************
* "uart_int.h":                         *
* Header file for Mega2560 UART driver. *
* Using UART 0.                         *
* If parameter Rx_Int <> 0 :            *
* Receiver interrupt will be enabled    *
*                                       *
* Henning Hargaard, 11/11 2015          *
*****************************************/ 
void InitUART(unsigned long BaudRate, unsigned char DataBit, char Parity, unsigned char Rx_Int);
unsigned char CharReady();
char ReadChar();
void SendChar(char Tegn);
void SendString(char* Streng);
void SendInteger(int Tal);
/****************************************/