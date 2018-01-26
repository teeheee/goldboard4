/**********************************************************************
@file       CMPS03.h

@brief      Driver the CMPS03 compass module

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
@editor  2017 Alexander Ulbrich

-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version
1.01 3.07.2017	Added checkAck

@end
***********************************************************************/

#ifndef _CMPS03_h
#define _CMPS03_h

#include "global.h"

#include "i2c.h"

#define CMPS03_I2C_ADDR   0xC0  /*!< I2C-Address of the module */
#define CMPS03_LORES_REG		1     /* Register for 8 bit measurement (0-255) */
#define CMPS03_HIRES_REG		2     /* Register for 16 bit measurement (0-3599 (0-359,9°)) */

#define CMPS03_ERROR_TIMEOUT	100	  /* After this time, there MUST be a value */

class CMPS03
{
	public:
		CMPS03();
		void init();
		bool isInitialized();
		uint8_t getValue();
		void setAs128Degree();
		bool checkAck();

	private:
		void startMeasurement();
		bool getMeasurementValue(int16_t* val);
		
		uint8_t _value;
		uint8_t _lastValue;
		int16_t _128DegreeValue;
		bool _128DegreeEnabled;
		bool _measurementStarted;
		bool _initialized;
};

#endif

