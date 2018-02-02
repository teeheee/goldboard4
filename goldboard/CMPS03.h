/*!
@file

@brief      Driver for the CMPS03 compass module

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

#ifndef _CMPS03_h
#define _CMPS03_h

#include <avr/io.h>
#include "time.h"
#include "Wire.h"

#define CMPS03_I2C_ADDR   		0xC0  /*!< I2C-Address of the module */
#define CMPS03_LORES_REG		1     /* Register for 8 bit measurement (0-255) */
#define CMPS03_HIRES_REG		2     /* Register for 16 bit measurement (0-3599 (0-359,9�)) */

#define CMPS03_ERROR_TIMEOUT	10	  /* After this time, there MUST be a value */

class CMPS03
{
	public:
		CMPS03();
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

