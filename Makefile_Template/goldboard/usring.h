
#ifndef _USRING_h
#define _USRING_h

#include "global.h"
#include "i2c.h"

#define USRING_I2C_ADDR   0x69  /*!< I2C-Address of the module */
#define USRING_REG			0
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
	void setThresholdValue(uint8_t id,uint8_t th);
	void setThresholdValueGolbal(uint8_t th);
private:

	bool _initialized;
};

#endif

