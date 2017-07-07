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



/***Construktor****/

HC05::HC05() 
{
	lib_state = AT_STATE_UNINITALIZED;
	uart_init(UART_BAUD_SELECT(38400UL,F_CPU));	
}



/** configuration***/

uint8_t HC05::setPairingPassword(const char* Password) //TODO: check if changes were made
{
	sendSerial(AT_PASSWORD);
	sendSerial("=");
	sendSerial(Password);
	sendSerial("\r\n");
	return getResponse();
}


uint8_t HC05::setDeviceName(const char* deviceName) //TODO: check if changes were made
{
	sendSerial(AT_NAME);
	sendSerial("=");
	sendSerial(deviceName);
	sendSerial("\r\n");
	return getResponse();
}


/*** establish connection***/


uint8_t HC05::inquierPairing()
{
	int ret = setMode(SLAVE_ROLE);
	if(ret)
		return ret;
	sendSerial(ATINQUIER);
	sendSerial("\r\n");
	ret = getResponse();
	if(ret)
		return ret;
	lib_state = HC05_STATE_CONNECTING;
	return 0;
}



uint8_t HC05::pairWith(const char* deviceName,const char* Password)
{

	//TODO
	return 0;
}

bool HC05::isConnected()
{
	if(lib_state==HC05_STATE_CONNECTED)
		return true;
	if(lib_state==HC05_STATE_CONNECTING)
		if(getResponse()==HC05_OK)
		{	
			lib_state = HC05_STATE_CONNECTED;
			return true;
		}
	if(lib_state==HC05_STATE_DISCONNECTED)
	{
		if(getResponse()==HC05_OK)
		{	
			lib_state = HC05_STATE_CONNECTING;
			return false;
		}	
	}
	return false;
}

/*** communicate***/

bool HC05::sendByte(uint8_t byte)
{
	if(isConnected())
	{
		char buffer[2] = {byte, 0};
		sendSerial(buffer);
		return true;
	}
	else
		return false;
}

bool HC05::sendString(const char* data)
{
	if(isConnected())
	{
		sendSerial(data);
		return true;
	}
	else
		return false;
}


int HC05::getLine(char* buffer, int buffersize) //TODO: needs to catch everything AT related stuff
{
	int length = getSerial(buffer,buffersize);



	if(length > 1)
	{
		if(buffer[0]=='+')
		{
			if(strstr(buffer,"+DISC"))
			{
				lib_state = HC05_STATE_DISCONNECTED;
			}
			return 0;
		}
	}

	return length;
}

/*** maintaining ****/


uint8_t HC05::resetDevice() //TODO: not realy good...
{
	sendSerial(AT_RESET);
	sendSerial("\r\n");
	delay(800); // give the module some time... 
	return getResponse();
}


uint8_t HC05::factoryResetDevice() //TODO: not realy good...
{
	sendSerial(AT_DEFAULT);
	sendSerial("\r\n");
	delay(1000); // give the module some time... 
	return getResponse();
}


uint8_t HC05::getlastError()
{
	uint8_t tmp = last_lib_error;
	last_lib_error = 0;
	return tmp;
}

uint8_t HC05::getStatus()
{
	return lib_state;
}

uint8_t HC05::getATStatus() //TODO: is it realy needed? lot of memory...
{
	char buffer[100];

	sendSerial(AT_STATE);
	sendSerial("?");
	sendSerial("\r\n");

	int ret = getResponse(buffer,100);

	if(ret==HC05_OK)
	{
		if(strstr(buffer,"INITIALIZED"))
			at_state = AT_STATE_INITIALIZED;
		if(strstr(buffer,"UNINITIALIZED"))
			at_state = AT_STATE_UNINITALIZED;
		if(strstr(buffer,"READY"))
			at_state = AT_STATE_READY;
		if(strstr(buffer,"PAIRABLE"))
			at_state = AT_STATE_PAIRABLE;
		if(strstr(buffer,"PAIRED"))
			at_state = AT_STATE_PAIRED;
		if(strstr(buffer,"INQUIRING"))
			at_state = AT_STATE_INQUIRING;
		if(strstr(buffer,"CONNECTING"))
			at_state = AT_STATE_CONNECTING;
		if(strstr(buffer,"CONNECTED"))
			at_state = AT_STATE_CONNECTED;
		if(strstr(buffer,"DISCONNECTED"))
			at_state = AT_STATE_DISCONNECTED;
	}
	else
		at_state = AT_STATE_ERROR;
	return at_state;
}


bool HC05::init() //TODO:  autobaud
{
	sendSerial(AT_INIT);
	sendSerial("\r\n");

	int ret = getResponse();

	if(ret==HC05_OK || ret==HC05_ERROR_RESP_CODE(17))
	{
			lib_state = HC05_STATE_INIT;
			last_lib_error = 0;
			return true;
	}
	else
	{
			lib_state = HC05_STATE_ERROR;
			last_lib_error = ret;
			return false;		
	}
}





uint8_t HC05::setMode(const char* mode) //TODO: check if mode is correct
{

	sendSerial(AT_ROLE);
	sendSerial("=");
	sendSerial(mode);
	sendSerial("\r\n");
	return getResponse();
}









/**** PRIVATE FUNKTIONS*****/

void HC05::sendSerial(const char*  data)
{
	uart_puts(data);
}

uint8_t HC05::getSerialByte()
{

		int x = uart_getc();
		uint8_t c = x&0xff;
		uint8_t error = x>>8;
		if(error!=(UART_NO_DATA>>8) && error) //serialbus error
		{
			last_lib_error = HC05_ERROR_SERIAL;
			return 0;
		}
		if(error!=(UART_NO_DATA>>8)) // only when data available
			return c;
		else
			return 0; //no data available
}


int HC05::getSerial(char* buffer, int buffersize)
{
	uint32_t currentTime = millis();
	int index = 0;

	while(1)
	{
		uint8_t c = getSerialByte();
		if(millis()-currentTime>500)	//timeout 500ms
		{
			last_lib_error = HC05_ERROR_TIMEOUT;
			return -1;
		}
		if(index == buffersize-1)		//bufferoverflow protection (there needs to be room for zero byte)
		{
			last_lib_error = HC05_ERROR_LIB;
			return -1;
		}
		if(c) //poll only when data available
		{
			buffer[index] = (char)c;
			index++;
			if(c == '\n')
			{
				buffer[index]=0;
				return index;
			}
		}
	}
}





uint8_t HC05::getResponse(char* responseBuffer, int bufferSize)
{
	char buffer[AT_BUFFERSIZE];
	

	while(1)
	{
		int length = getSerial(buffer,AT_BUFFERSIZE);
		if(length < 0) //check for serial errors
			return last_lib_error;

		//check for OK
		if(strstr(buffer,AT_OK))
			return HC05_OK;

		//check for ERROR
		char* pos = strstr(buffer,AT_ERROR);
		if(pos)
		{
			//return error code in format HC05_ERROR_RESP_CODE
			char* errorcode = pos+7; 
			return HC05_ERROR_RESP_CODE(atoi(errorcode));
		}

		//check for FAIL
		if(strstr(buffer,AT_FAIL))
		{
			return HC05_ERROR_LIB;
		}

		//check if response Buffer is big enough
		if(bufferSize > length)
			return HC05_ERROR_LIB;

		strncpy(responseBuffer,buffer,length);
		responseBuffer+=length;
		
	}
}





uint8_t HC05::getResponse()
{
	return getResponse(0,0);
}

