/*!
@file

@brief      Driver for the PCF8574A Remote 8-Bit I/O expander

 -----------------------------------------------------------------------
 @author  2015 Christoph Rothermel
 Christoph.Rothermel@gmx.de

 @editor 2018 Alexander Ulbrich
 alexander.ulbrich@uni-ulm.de
 ----------------------------------------------------------------------
 @History

 1.00 28.01.2015 First working version
 2.0  30.01.2018 new back-end added

@end
@ingroup external gb4
@addtogroup external gb4
@end
*/

#ifndef PCF8574A_H_
#define PCF8574A_H_

#include "Wire.h"
/*!
@class PCF8574A
@brief Für diese Klasse muss kein Objekt erstellt werden. Es ist Teil der Goldboard4 Klasse und kann über das gb Objekt angesprochen werden.
		Typische Verwendung hat die Form gb.motors[2].rotate(20);
@ingroup zf
@addtogroup zf
*/
class PCF8574A
{
	//variables
public:
protected:
private:
	uint8_t _pcfdata;
	uint8_t _writeNeeded;
	uint8_t _address;

	//functions
public:
	PCF8574A();

	void init(uint8_t address);

	void setPin(uint8_t, bool);

	uint8_t read();
	void write();

protected:
private:

};
//PCF8574Pin

#endif /* PCF8574A_H_ */
