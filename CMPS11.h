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

#define CMPS11_I2C_ADDR			96  /*!< I2C-Address of the module */
#define CMPS11_ANGLE_8_REG		1     /* Register for 8 bit measurement (0-255) */
#define CMPS11_ACCELERATION_X_REG		12
#define CMPS11_ACCELERATION_Y_REG		14



#define CMPS11_ERROR_TIMEOUT	100	  /* After this time, there MUST be a value */

#define CMPS11_CALIBRATION_SEQUENCE {0xF0,0xF5,0xF7}

/*!
@class CMPS11
@brief CMPS11 sensor Klasse. Es kann nur ein Sensor (ein Objekt) pro Roboter verwendet werden.
@ingroup zf
@addtogroup zf
*/
class CMPS11
{
	public:
		CMPS11();
		void init();
		bool isInitialized();
		uint8_t getValue();
		int getAccelerometerX();
		int getAccelerometerY();
		void setAs128Degree();
		void startCalibration();
		void exitCalibration();
	private:
		void cmd(uint8_t byte);
		int16_t _128DegreeValue;
		bool _128DegreeEnabled;
};

#endif

