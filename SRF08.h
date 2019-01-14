/**********************************************************************
@file       SRF08.h

@brief      Driver for the SRF08 sonar sensor

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#ifndef _SRF08_h
#define _SRF08_h

#include "Arduino.h"
#include "Wire.h"
#include "time.h"

class SRF08
{
public:
	SRF08();
	void	init(uint8_t address);
	bool 	measurementStart();
	bool 	measurementComplete();
	int 	measurementGet();
    void	changeAddress(uint8_t newAddress);
	int		getValueCM();
	int 	getValueCMBlocking();
	bool	isInitialized();
	bool 	checkACK();
	void 	setRange (uint16_t millimeters);
	void 	setGain (uint8_t gain);
private:
	bool 			_initialized;
    int				_address;
	unsigned int	_lastValue;

	uint16_t 		_maxValue;

	typedef enum sonicstate_s {
		STATE_START,
		STATE_CM_SENT,
		STATE_RESULT_REQUEST_SENT,
		STATE_RESULT_RECEIVED
	} sonicstate_t;

	sonicstate_t 	_state;

	void 	_i2cWriteRegister(uint8_t reg,uint8_t data);

};


#endif
