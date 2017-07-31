/**********************************************************************
@file       HC05.cpp

@brief      Contains the functions for the Goldboard4 bluetooth module extension

-----------------------------------------------------------------------
@author  2017 Alexander Ulbrich
         alexander.ulbrich@uni-ulm.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#include "HC05.h"
#include "uart.h"
#include "config.h"
#include "wiring_private.h"
#include "Serial.h"



#define RESPONSES_LENGTH 9 //darf nicht mit gleichem Buchstaben anfangen!!!!

const char* responses[RESPONSES_LENGTH] = {
	"?\r\n", //0
	"AOK\r\n", //1
	"ERR\r\n", //2
	"Reboot!\r\n", //3
	"1,0,0\r\n", //4
	"0,0,0\r\n", //5
	"END\r\n", //6
	"KILL\r\n", //7
	"CMD\r\n"}; //8

#define HC05_QM 0
#define HC05_AOK 1
#define HC05_ERR 2
#define HC05_REBOOT 3
#define HC05_100 4
#define HC05_000 5
#define HC05_END 6
#define HC05_KILL 7
#define HC05_CMD 8

//private functions

uint8_t HC05::cmdMode(uint8_t onoff)
{
	flushInput();
	if(onoff && isCommandMode==0)
	{
		uart_puts("$$$");
		isCommandMode = 1;
	}
	else if(!onoff && isCommandMode==1)
	{
		uart_puts("---\r\n");
		isCommandMode = 0;
	}
	else
		return 0;
	uint8_t resp = getResponse();
	if(resp==HC05_CMD || resp==HC05_END)
	{
		return 0;
	}
	//uart_putc(resp);
	return 1;
}

uint8_t HC05::reboot()
{
	if(cmdMode(1))
		return 1;

	uart_puts("R,1\r\n");
	isCommandMode = 0;
	delay(1000);
	if(getResponse()==HC05_REBOOT)
		return 0;
	return 1;
}

uint8_t HC05::set(uint8_t reg, const char* value) //S,?\r\n -> AOK or ERR
{
	if(cmdMode(1))
		return 1;

	uart_putc('S');
	uart_putc(reg);
	uart_putc(',');
	uart_puts(value);
	uart_puts("\r\n");

	if(getResponse()==HC05_AOK)
		return 0;
	return 1;
}

void HC05::flushInput()
{
	while(1)
	{
		int data = uart_getc();
		uint8_t error = data>>8;
		if(error != 0) //no character recived
			return;
	}
}

uint8_t HC05::getResponse()
{
	int index = 0;
	int respnummer = 0;
	long cur_time = millis();
	while(1)
	{
		int data = uart_getc();
		uint8_t error = data & 0xff00;
		uint8_t character = (uint8_t)data;
		if(error == 0) //character recived
		{
			if(character=='\n')
				return respnummer;//match
			respnummer=-1;
			for(int i = 0; i < RESPONSES_LENGTH; i++)
			{
				if(character == responses[i][index])
				{
					index++;
					cur_time = millis();
					respnummer = i;		
				}
			}
			if(character != 0 && respnummer==-1)//no match
			{	
				uart_putc('|');
				uart_putc(character);
				uart_putc('|');
				for(int i = 0; i < RESPONSES_LENGTH; i++)
					uart_putc(responses[i][index]);
				uart_putc('|');
				flushInput();
				return 0xff-3;
			}
		}
		else if(error != UART_NO_DATA)//uart error
		{
			return 0xff-2; 
		}
		if(millis()-cur_time > HC05_TIMEOUT)//timeout
		{
			return 0xff-1; 
		}
	}
	return 0xff;
}/*
uint8_t HC05::getResponse()
{
	int index = 0;
	volatile uint32_t timeout = 0;
	while(1)
	{
		int data = uart_getc();
		uint8_t error = data & 0xff00;
		uint8_t character = (uint8_t)data;
		if(error == 0) //character recived
		{
			buffer[index]=character;
			index++;
			if(character=='\n')
			{
				buffer[index]=0;
				break;
			}
		}
		else if(error != UART_NO_DATA)//uart error
		{
			uart_puts("uart error\r\n");
			return 0xff-2; 
		}
		if(timeout > HC05_TIMEOUT)//timeout
		{
			uart_puts("timeout\r\n");
			return 0xff-1; 
		}	
		timeout++;
	}

	uart_puts("HEUREKA\r\n");

	int respons;
	for(int c = 0; c < index; c++)
	{
		respons = -1;
		for(int i = 0; i < RESPONSES_LENGTH; i++)
		{
			if(buffer[c] == responses[i][c])
			{
				respons = i;
				break;
			}
		}
		if(respons == -1)//no match
			return 0xff;
	}
	return respons;
}
*/
//public functions

HC05::HC05()
{

}


uint8_t HC05::init()
{
	isCommandMode=1;
	uart_init(UART_BAUD_SELECT(115200,F_CPU));
	cmdMode(0);
	delay(100);
	if(cmdMode(1))
		;//return 1;
	delay(100);
	if(set('F',"1")) //factory reset
		;//return 1;
	if(reboot())
		;//return 1;
	if(set('~',"3")) //Serial Port to PC
		;//return 1;
	if(set('T',"253")) //Continous configuration enabled
		;//return 1;
	if(reboot())
		;//return 1;
	return 0;
}

uint8_t* HC05::getDeviceMac() //TODO getDeviceMac() 
{
	//uart_puts("GB\r\n");
	return 0;
}

uint8_t HC05::isActive(uint8_t* device) //TODO isActive(uint8_t* device) 
{
	return 0;
}

uint8_t HC05::isConnected()
{
	if(cmdMode(1))
		return 1;
	uart_puts("GK\r\n");
	if(getResponse()==HC05_100)
		return 1;
	return 0;
}


uint8_t HC05::connectTo(uint8_t* device) //TODO connectTo(uint8_t* device)
{
	return 0;
}

uint8_t HC05::waitforConnection()
{
	while(!isConnected())
			delay(100);
	return 1;
}

uint8_t HC05::disconnect()
{
	if(cmdMode(1))
		return 1;
	uart_puts("K,\r\n");
	if(getResponse()==HC05_KILL)
		return 0;
	return 1;
}


uint8_t* HC05::getLine()//TODO  getLine()
{
	cmdMode(0);
	return 0;
}

uint8_t HC05::getChar()//TODO  getChar()
{
	cmdMode(0);
	return 0;
}

int HC05::getInt()//TODO  getInt()
{
	cmdMode(0);
	return 0;
}

uint8_t HC05::getData(uint8_t* data, int datalength)//TODO getData(uint8_t* data, int datalength)
{
	cmdMode(0);
	return 0;
}


uint8_t HC05::sendLine(const char* data)
{
	cmdMode(0);
	uart_puts((const char*)data);
	return 1;
}

uint8_t HC05::sendChar(uint8_t data)
{
	cmdMode(0);
	uart_putc(data);
	return 1;
}

uint8_t HC05::sendInt(int data) //TODO sendInt(int data)
{
	cmdMode(0);
	return 0;
}

uint8_t HC05::sendData(uint8_t* data, int datalength) //TODO sendData(uint8_t* data, int datalength)
{
	cmdMode(0);
	return 1;
}


