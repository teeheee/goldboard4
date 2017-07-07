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

/** AT COMMANDS*****/

#define AT_RESET 	"AT+RESET"
#define AT_DEFAULT 	"AT+ORGL"
#define AT_NAME		"AT+NAME"
#define AT_PASSWORD	"AT+PSWD"
#define AT_ROLE		"AT+ROLE"
	#define SLAVE_ROLE 	"0"
	#define MASTER_ROLE "1"
#define ATINQUIER 	"AT+INQ"
#define AT_INIT 	"AT+INIT"
#define AT_STATE 	"AT+STATE"


/*** AT RESPONSE*****/

#define AT_OK    "OK\r\n"
#define AT_ERROR "ERROR" //\r\n fehlt hier da ERROR das format
#define AT_FAIL  "FAIL\r\n"

/*** AT STATES*****/

#define AT_STATE_UNINITALIZED		0
#define AT_STATE_INITIALIZED		1
#define AT_STATE_READY				2
#define AT_STATE_PAIRABLE			3
#define AT_STATE_PAIRED				4
#define AT_STATE_INQUIRING			5
#define AT_STATE_CONNECTING			6
#define AT_STATE_CONNECTED 			7
#define AT_STATE_DISCONNECTED 		8
#define AT_STATE_ERROR	 			9


/*** LIB STATES ****/

#define HC05_STATE_PREINIT    1
#define HC05_STATE_INIT       2
#define HC05_STATE_CONNECTING 3
#define HC05_STATE_CONNECTED	4
#define HC05_STATE_DISCONNECTED	4
#define HC05_STATE_ERROR	 5


/*** LIB ERRORS ****/

#define HC05_NO_ERROR		0
#define HC05_OK				0
#define HC05_ERROR_SERIAL	1
#define HC05_ERROR_DEVICE	2
#define HC05_ERROR_LIB		3
#define HC05_ERROR_TIMEOUT	4
#define HC05_ERROR_RESP		5
#define HC05_ERROR_RESP_CODE(x) (x+(1<<7))


#define AT_BUFFERSIZE		200


class HC05 
{
//variables
public:

private:
	const char* deviceName;
	const char* pairingPassword;

	uint8_t lib_state;
	uint8_t last_lib_error;

	uint8_t at_state;
	uint8_t last_at_error;

//functions
public:
	HC05();
	HC05(const char* deviceName,const char* Password);

	/** configuration***/

	uint8_t setPairingPassword(const char* Password);
	uint8_t setDeviceName(const char* deviceName);
	
	/*** establish connection***/

	uint8_t pairWith(const char* deviceName,const char* Password);
	uint8_t inquierPairing();
	uint8_t inquierPairing(int timeout);

	bool isConnected();

	/*** communicate***/ //ADD a better communication protokoll



	bool sendByte(uint8_t byte);
	bool sendString(const char* data);

	int 	getLine(char* buffer, int buffersize);


	/*** maintaining ****/

	uint8_t factoryResetDevice();
	uint8_t resetDevice();
	uint8_t getStatus();
	bool init();
	uint8_t getlastError();
	uint8_t setMode(const char* mode);


private:
	uint8_t getATStatus();
	int 	getSerial(char* buffer,int buffersize);
	uint8_t getSerialByte();
	void 	sendSerial(const char*  data);
	uint8_t getResponse();
	uint8_t getResponse(char* responseBuffer,int size);
}; //HC05

#endif //__HC05_H__
