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

/***** SPECIAL RESPONSES DATA*****/

uint8_t recvHook(uint8_t data);

#define STATUS_MESSAGES_LENGTH 2

uint8_t escape_sequence_status = 0;
const char* status_messages[STATUS_MESSAGES_LENGTH] = {
	"###CONNECT\0", //1
	"###DISCONNECT\0", //2
};

#define ESCAPE_STATUS_CONNECT 1
#define ESCAPE_STATUS_DISCONNECT 2


/*******NORMAL RESPONSES DATA*********/

#define RESPONSES_LENGTH 12 //darf nicht mit gleichem Buchstaben anfangen!!!!

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
	"TRYING\r\0",  //10
	"CONNECT failed\r\0",  //11
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
#define HC05_TRYING 10
#define HC05_CONNECT_FAILED 11

 

//private functions

// onoff=1 means command mode, onoff=0 means data mode
// return 0: 	when everything is fine
//		  0xff: when there was an Error
uint8_t HC05::cmdMode(uint8_t onoff)
{
	delay(10); //this is necessary for the device to detect commands
	flushInput();
	if(onoff  && isCommandMode!=1) //check if already in command mode
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
	uart_puts("R,1\r");
	isCommandMode = 0;
	delay(2000); //give it some time to reboot (normaly 500ms)
	if(getResponse()==HC05_REBOOT)
	{
		if(cmdMode(1))
			return 0xff;
		return 0;
	}
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
	flushInput();
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

uint8_t recvHook(uint8_t data)
{
	static uint8_t match_counters[STATUS_MESSAGES_LENGTH] = {0,0};
	for(uint8_t i = 0; i < STATUS_MESSAGES_LENGTH; i++)
	{
		if(status_messages[i][match_counters[i]] == data)
		{
			match_counters[i]++;
			if(status_messages[i][match_counters[i]]==0)
				escape_sequence_status=i+1;
		}
		else
			match_counters[i]=0;
	}
	return 0;
}



//public functions

HC05::HC05()
{

}

uint8_t HC05::factoryReset()
{
	if(cmdMode(1))
		return 0xff;
	if(set('F',"1")) //factory reset
		return 0xff-1;
	if(reboot())
		return 0xff-2;
	return 0;
}

uint8_t HC05::init(const char* _name, const char* _pin, uint8_t _mode)
{
	isCommandMode=-1;
	connectionState=-1;
	name = _name;
	pin = _pin;
	mode = _mode;
	uart_init(UART_BAUD_SELECT(115200,F_CPU));
	addRecvHandler(recvHook);
	cmdMode(0);
	if(cmdMode(1))
		return 0xff;
	//factoryReset();
	if(set('~',"3")) //serial port to PC
		return 0xff-3;
	if(set('T',"253")) //continous configuration enabled
		return 0xff-4;
	if(set('N',name)) //name
		return 0xff-5;
	if(set('A',"0")) // authentification mode
		return 0xff-6;
	if(set('P',pin)) //authentification pin
		return 0xff-7;
	if(mode == HC05_MODE_MASTER)
		if(set('M',"1")) // master mode
			return 0xff-8;
	if(mode == HC05_MODE_SLAVE)
		if(set('M',"0")) // slave mode
			return 0xff-8;
	if(set('O',"###")) // escape sequence
		return 0xff-8;
	if(reboot())
		return 0xff-9;
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
	if(connectionState==-1) //unknown status
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
			connectionState = 1;
			return 1;
		}
		else if(resp == HC05_000) //not connected
		{	
			if(cmdMode(0))
				return 0xff-1;
			connectionState = 0;
			return 0;
		}
		else // error handling
		{
			if(error_code==0)
				error_code = HC05_ERROR_WRONG_RESPONSE;
			return 0xff;
		}
	}
	else if(escape_sequence_status != 0) //status changed
	{
		if(escape_sequence_status == ESCAPE_STATUS_CONNECT)
			connectionState = 1;
		else
			connectionState = 0;
		escape_sequence_status = 0;
		flushInput();
	}
	return connectionState;
}


uint8_t HC05::connectTo(const char* device) //TODO connectTo(uint8_t* device)
{
	if(cmdMode(1))
		return 0xff-1;
	flushInput();
	uart_puts("C,");
	uart_puts(device);
	uart_putc('\r');
	uint8_t resp = getResponse();
	if(resp == HC05_TRYING)
	{
		uint8_t resp = getResponse();
		if(resp == HC05_CONNECT_FAILED)
		{
			flushInput();
			return 0;
		}
		if(escape_sequence_status == ESCAPE_STATUS_CONNECT)
		{
			flushInput();
			return 1;
		}
	}
	return 0xff;
}

uint8_t HC05::waitforConnection()
{
	while(1)
	{
			uint8_t resp = isConnected();
			if(resp == 1)
				return 0;
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
	if(getResponse()==HC05_KILL || escape_sequence_status==ESCAPE_STATUS_DISCONNECT)
	{
		escape_sequence_status=0;
		if(cmdMode(0))
			return 0xff-2;
		return 0;
	}
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
