

#include "uart.h"
#include <stdlib.h>
#include "SerialPrint.h"

#define LINEEND "\r\n"

SerialPrint::SerialPrint()
{
	blockOn = false;
}

void SerialPrint::block()
{
	blockOn = true;
}

size_t SerialPrint::write(uint8_t buffer)
{
	uart_putc((unsigned char) buffer);
	return sizeof(buffer);
}

SerialPrint serial;
