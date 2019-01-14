/*!
@file

 @brief      Driver for the usring Sensor module

 -----------------------------------------------------------------------
 @author  2017 Alexander Ulbrich
 alexander.ulbrich@uni-ulm.de
 -----------------------------------------------------------------------
 @History

 1.00 29.6.2017 First working version
 2.0  30.01.2018 new back-end added

@end
@ingroup sensors
@addtogroup sensors
*/


#ifndef _USRING_h
#define _USRING_h

#include <avr/io.h>

#define USRING_I2C_ADDR   52  /*!< I2C-Address of the module */
#define USRING_REG		0
#define USRING_AN_REG		2
#define USRING_TH_REG		18
#define USRING_ERROR_TIMEOUT	100	  /* After this time, there MUST be a value */


class usring
{
public:
	usring();
	void init();
	bool isInitialized();
	uint16_t getValue();
	uint8_t getanalogValue(uint8_t id);
	uint8_t getThresholdValue(uint8_t id);
	void setThresholdValue(uint8_t id, uint8_t th);
	void setThresholdValueGolbal(uint8_t th);
	void setFilterValue(uint8_t value);
	bool checkAck();
private:
	bool _initialized;
};

#endif

