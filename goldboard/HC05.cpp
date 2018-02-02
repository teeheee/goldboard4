

#include "HC05.h"
#include "uart.h"
#include "time.h"

HC05::HC05()
{
	_masterslave = HC05_SLAVE;
}

void HC05::init(uint8_t MasterSlave, const char* remoteMAC)
{
	_masterslave = MasterSlave;
	if(_masterslave == HC05_MASTER)
	{
		uart_puts("$$$");//Internen Bereich betreten
		delay(50);
		uart_puts("GB\r\n");//Adresse ausgeben
		delay(50);
		uart_puts("SM,1\r\n");//Mastermode einschalten
		delay(50);
		uart_puts("C,");//connect to Slave
		uart_puts(remoteMAC);
		uart_puts("\r\n");
		delay(50);
		uart_puts("---\r\n");//Internen Bereich verlassen
	}
	else
	{
		uart_puts("$$$");//Internen Bereich betreten
		delay(50);
		uart_puts("GB\r\n");//Adresse ausgeben
		delay(50);
		uart_puts("SM,0\r\n");//Slavemode einschalten
		delay(50);
		uart_puts("---\r\n");//Internen Bereich verlassen
	}
	while((uart_getc()>>8)!=UART_NO_DATA);
}

void HC05::write(uint8_t* data, unsigned int length)
{
	for(unsigned int i = 0; i < length; i++)
		uart_putc(data[i]);
}

unsigned int HC05::read(uint8_t* data,unsigned int length)
{
	for(unsigned int i = 0; i < length; i++)
	{
		uint32_t current_time = millis();
		uint16_t recv = uart_getc();
		while(recv>>8 == UART_NO_DATA &&   millis()-current_time > HC05_TIMEOUT)
			recv = uart_getc();
		if(recv>>8 > 0) //error
			return i;
		data[i] = recv & 0xff;
	}
	return length;
}
