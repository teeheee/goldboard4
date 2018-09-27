/*!
@file

 @brief      Bluetooth library for the RN42 firmware flashed on an HC05 module

 -----------------------------------------------------------------------
 @author  2018 Alexander Ulbrich
 alexander.ulbrich@uni-ulm.de
 -----------------------------------------------------------------------
 @History

 0.1  30.01.2018 not Tested

@end
@ingroup external
@addtogroup external
@end
*/

#ifndef GOLDBOARD4_HC05_H_
#define GOLDBOARD4_HC05_H_

#include "Arduino.h"

#define HC05_MASTER 1
#define HC05_SLAVE  2

#define HC05_TIMEOUT 10 //ms

/*!
@class HC05
@brief Bluetooth libs
@ingroup zf
@addtogroup zf
*/
class HC05{
private:
    uint8_t _masterslave;
public:
	HC05();
	void init(uint8_t MasterSlave, const char* remoteMAC);
	void write(uint8_t* data, unsigned int length);
	unsigned int read(uint8_t* data, unsigned int length);
};


#endif /* GOLDBOARD4_HC05_H_ */
