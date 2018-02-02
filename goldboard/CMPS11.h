/*!
@file

@brief      Driver for the CMPS11 compass module

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
@ingroup sensors
@addtogroup sensors
*/

#ifndef _CMPS11_h
#define _CMPS11_h

#include "Wire.h"
#include "Arduino.h"

#define CMPS11_I2C_ADDR			0x60  /*!< I2C-Address of the module */
#define CMPS11_ANGLE_8_REG		1     /* Register for 8 bit measurement (0-255) */
#define CMPS11_HIRES_REG		2     /* Register for 16 bit measurement (0-3599 (0-359,9�)) */

#define CMPS11_ERROR_TIMEOUT	100	  /* After this time, there MUST be a value */


class CMPS11
{
	public:
		CMPS11();
		void init();
		bool isInitialized();
		uint8_t getValue();
		void setAs128Degree();

	private:
		
		uint8_t _value;
		int16_t _128DegreeValue;
		bool _128DegreeEnabled;
		bool _initialized;
};

#endif

