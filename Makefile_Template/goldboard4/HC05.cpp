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



#define RESPONSES_LENGTH 10 //darf nicht mit gleichem Buchstaben anfangen!!!!

const char* responses[RESPONSES_LENGTH] = {
	"?\r\0", //0
	"AOK\r\0", //1
	"ERR\r\0", //2
	"Reboot!\r\0", //3
	"1,0,0\r\0", //4
	"0,0,0\r\0", //5
	"END\r\0", //6
	"KILL\r\0", //7
	"CMD\r\0",   //8
	"4,0,0\r\0",   //9
	}; 

#define HC05_QM 0
#define HC05_AOK 1
#define HC05_ERR 2
#define HC05_REBOOT 3
#define HC05_100 4
#define HC05_000 5
#define HC05_END 6
#define HC05_KILL 7
#define HC05_CMD 8
#define HC05_400 9

//private functions

// onoff=1 means command mode, onoff=0 means data mode
// return 0: 	when everything is fine
//		  0xff: when there was an Error
uint8_t HC05::cmdMode(uint8_t onoff)
{
	delay(10); //this is necessary for the device to detect commands
	flushInput();
	if(onoff && isCommandMode!=1) //check if already in command mode
	{
		uart_puts("$$$");
		isCommandMode = 1;
	}
	else if(!onoff && isCommandMode!=0) //check if already in data mode
	{
		uart_puts("---\r");
		isCommandMode = 0;
	}
	else //do nothing when called multiple times
		return 0;
	uint8_t resp = getResponse();
	if(resp==HC05_CMD || resp==HC05_END) //everything is fine
	{
		return 0;
	}
	else //error handling
	{
		isCommandMode = -1;
		if(error_code==0)
			error_code = HC05_ERROR_WRONG_RESPONSE;
		return 0xff;
	}
}

uint8_t HC05::reboot()
{
	if(cmdMode(1))
		return 0xff;

	delay(100);
	uart_puts("R,1\r");
	isCommandMode = 0;
	delay(2000);
	if(getResponse()==HC05_REBOOT)
		return 0;
	else //error handling
	{
		if(error_code==0)
			error_code = HC05_ERROR_WRONG_RESPONSE;
		return 0xff;
	}
}

uint8_t HC05::set(uint8_t reg, const char* value) //S,?\r -> AOK or ERR
{
	if(cmdMode(1))
		return 0xff;

	uart_putc('S');
	uart_putc(reg);
	uart_putc(',');
	uart_puts(value);
	uart_puts("\r");

	if(getResponse()==HC05_AOK)
		return 0;
	else //error handling
	{
		if(error_code==0)
			error_code = HC05_ERROR_WRONG_RESPONSE;
		return 0xff;
	}
}

void HC05::flushInput()
{
	while(1)
	{
		delay(1);
		int data = uart_getc();
		uint8_t error = data>>8;
		if(error != 0) //no character recived
			return;
	}
}


// this is the brain of this Interface, Try not to mess with it!!!
uint8_t HC05::getResponse()
{
	int index = 0;
	int respnummer = -1;
	long cur_time = millis();
	uint8_t matchlist[RESPONSES_LENGTH];
	for(int i = 0; i < RESPONSES_LENGTH; i ++)
		matchlist[i] = 1;
	while(1)
	{
		uint16_t data = uart_getc();
		uint16_t error = data & 0xff00;
		uint8_t character = (uint8_t)data;

		if(error == 0 && character > 0) //character recived
		{
			if(character=='\n') //String match
			{
				flushInput();
				return respnummer;
			}

			respnummer=-1;
			for(int i = 0; i < RESPONSES_LENGTH; i++)
			{
				if(character == responses[i][index] && matchlist[i]==1)
				{
					cur_time = millis();
					respnummer = i;
				}
				else
					matchlist[i]=0;
			}

			if(respnummer > -1) //Character match
				index++;
			else if(character != 0 && respnummer==-1)//no Character match = no String match and ignore 0
			{

				flushInput();
				error_code = HC05_ERROR_NO_MATCH;
				return 0xff-3;
			}
		}
		else if(error != UART_NO_DATA)//uart error
		{
			error_code = HC05_ERROR_UART_ERROR;
			flushInput();
			return 0xff-2;
		}
		if(millis()-cur_time > HC05_TIMEOUT)//timeout
		{
			error_code = HC05_ERROR_RESPONSE_TIMEOUT;
			flushInput();
			return 0xff-1;
		}
	}
	error_code = HC05_ERROR_REALY_STRANGE_ERROR;
	return 0xff;
}

//public functions

HC05::HC05()
{

}


uint8_t HC05::init()
{
	isCommandMode=-1;
	uart_init(UART_BAUD_SELECT(115200,F_CPU));
	delay(100);
	cmdMode(0);
	delay(100);
	if(cmdMode(1))
		return 0xff;
	delay(100);
	if(set('F',"1")) //factory reset
		return 0xff-1;
	if(reboot())
		return 0xff-2;
	if(set('~',"3")) //Serial Port to PC
		return 0xff-3;
	if(set('T',"255")) //Continous configuration enabled
		return 0xff-4;
	if(set('N',"Robot")) //Serial Port to PC
		return 0xff-3;
	if(reboot())
		return 0xff-5;
	return 0;
}

uint8_t* HC05::getDeviceMac() //TODO getDeviceMac() 
{
	//uart_puts("GB\r");
	return 0;
}

uint8_t HC05::isActive(uint8_t* device) //TODO isActive(uint8_t* device) 
{
	return 0;
}

uint8_t HC05::isConnected()
{
	if(cmdMode(1))
		return 0xff-1;
	flushInput();
	uart_puts("GK\r");
	uint8_t resp = getResponse();
	if(resp == HC05_100 || resp == HC05_400) // connected
	{
		//isCommandMode=-1; // not sure if there is a state change???
		if(cmdMode(0))
			return 0xff-1;
		return 1;
	}
	else if(resp == HC05_000) //not connected
	{	
		if(cmdMode(0))
			return 0xff-1;
		return 0;
	}
	else // error handling
	{
		if(error_code==0)
			error_code = HC05_ERROR_WRONG_RESPONSE;
		return 0xff;
	}
}


uint8_t HC05::connectTo(uint8_t* device) //TODO connectTo(uint8_t* device)
{
	return 0;
}

uint8_t HC05::waitforConnection()
{
	while(1)
	{
			uint8_t resp = isConnected();
			if(resp == 1)
				return 1;
			else if(resp != 0)
				return resp;
			delay(500);
	}
}

uint8_t HC05::disconnect()
{
	if(cmdMode(1))
		return 0xff-1; //error
	flushInput();
	uart_puts("K,\r");
	if(getResponse()==HC05_KILL)
		return 0;
	else // error handling
	{
		if(error_code==0)
			error_code = HC05_ERROR_WRONG_RESPONSE;
		return 0xff;
	}
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


uint8_t HC05::getLastError()
{
	int tmp = error_code;
	error_code = 0;
	return tmp;
}
