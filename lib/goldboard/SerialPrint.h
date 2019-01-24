/*!
@file

 @brief      handels the SERIAL_PRINT parameters

 -----------------------------------------------------------------------
 @author  2018 Alexander Ulbrich
 alexander.ulbrich@uni-ulm.de
 -----------------------------------------------------------------------
 @History

 1.0  30.01.2018 First working version

@end
@ingroup gb4
@addtogroup gb4
*/
#ifndef GOLDBOARD4_PRINT_H_
#define GOLDBOARD4_PRINT_H_


#include "Print.h"

class SerialPrint : public Print
{
private:
	bool blockOn;
public:
	SerialPrint();
	void block(); //if this function is called all serial data over print is blocked
	virtual size_t write(uint8_t);
};

extern SerialPrint serial;

#endif /* GOLDBOARD4_PRINT_H_ */
