

#include "print.h"
#include "uart.h"
#include <stdlib.h>

#define LINEEND "\r\n"

SerialPrint::SerialPrint()
{
	blockOn = false;
}

void SerialPrint::block()
{
	blockOn = true;
}

void SerialPrint::print(const char* string)
{
	if(blockOn)return;
	uart_puts(string);
}

void SerialPrint::print(int number)
{
	if(blockOn)return;
	char buffer[10];
	itoa(number, buffer, 10);
	uart_puts(buffer);
}

void SerialPrint::print(float floatnumber)
{
	if(blockOn)return;
	char buffer[10];
	dtostrf(floatnumber, 7, 3, buffer);
	uart_puts(buffer);
}


void SerialPrint::print(unsigned int number)
{
	if(blockOn)return;
	char buffer[10];
	itoa(number, buffer, 10);
	uart_puts(buffer);	
}

void SerialPrint::println(unsigned int number)
{
	if(blockOn)return;
	print(number);
	uart_puts(LINEEND);	
}

void SerialPrint::println(const char* string)
{
	if(blockOn)return;
	print(string);
	uart_puts(LINEEND);
}

void SerialPrint::println(int number)
{
	if(blockOn)return;
	print(number);
	uart_puts(LINEEND);
}

void SerialPrint::println(float floatnumber)
{
	if(blockOn)return;
	print(floatnumber);
	uart_puts(LINEEND);
}

SerialPrint serial;
