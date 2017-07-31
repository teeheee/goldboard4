/**********************************************************************
@file       HC05.h

@brief      Contains the functions for the Goldboard4 bluetooth module extension

-----------------------------------------------------------------------
@author  2017 Alexander Ulbrich
         alexander.ulbrich@uni-ulm.de
-----------------------------------------------------------------------

@end
***********************************************************************/

#ifndef __HC05_H__
#define __HC05_H__



#include "Print.h"
#include "uart.h"
#include "config.h"


#define HC05_BUFFERSIZE		100
#define HC05_TIMEOUT		5000UL


#define HC05_ERROR_NO_ERROR 			0
#define HC05_ERROR_RESPONSE_TIMEOUT		1
#define HC05_ERROR_NO_MATCH				2
#define HC05_ERROR_UART_ERROR		    3
#define HC05_ERROR_WRONG_RESPONSE		4
#define HC05_ERROR_REALY_STRANGE_ERROR	0xff



class HC05
{
//variables
public:

private:
	uint8_t buffer[HC05_BUFFERSIZE];
	uint8_t isCommandMode;

//functions
public:
	HC05();

	uint8_t init();

	uint8_t* getDeviceMac();
	uint8_t isActive(uint8_t* device);
	uint8_t isConnected();

	uint8_t connectTo(uint8_t* device);
	uint8_t waitforConnection();
	uint8_t disconnect();

	uint8_t* getLine();
	uint8_t getChar();
	int getInt();
	uint8_t getData(uint8_t* data,int datalength);

	uint8_t sendLine(const char* data);
	uint8_t sendChar(uint8_t data);
	uint8_t sendInt(int data);
	uint8_t sendData(uint8_t* data,int datalength);

	uint8_t getLastError();

private:
	uint8_t error_code;

	void flushInput();
	uint8_t cmdMode(uint8_t onoff);
	uint8_t reboot();
	uint8_t set(uint8_t reg, const char* value);
	uint8_t getResponse(); //checks for resp string

}; //HC05

#endif //__HC05_H__
