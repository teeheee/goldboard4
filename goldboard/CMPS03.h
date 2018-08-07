/*!
@file

@brief      Treiber für das CMPS03 Kompass Modul

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

#include "Arduino.h"
#include "Wire.h"


#define CMPS03_I2C_ADDR   		96  /* I2C-Address of the module */
#define CMPS03_LORES_REG		1     /* Register for 8 bit measurement (0-255) */
#define CMPS03_HIRES_REG		2     /* Register for 16 bit measurement (0-3599 (0-359,9�)) */

#define CMPS03_ERROR_TIMEOUT	100	  /* After this time, there MUST be a value */

/*!
@class CMPS03
@brief CMPS03 sensor Klasse. Es kann nur ein Sensor (ein Objekt) pro Roboter verwendet werden.
@ingroup zf
@addtogroup zf
*/
class CMPS03
{
	public:
		CMPS03();
		/*! Initialisiert die Sensorklasse. Muss einmal am Anfang aufgerufen werden.
		*/
		void init();
		/*! Gibt den Kompasswert zurück. Es macht keinen Sinn die Methode öfter als alle 33ms abzufragen,
			da der Wert sich nicht öfter ändert. Falls der Übergang von 255 -> 0 nicht an einem Punkt ist muss der Sensor neu kalibriert werden.
			@return 0-255 für die Richtung des Kompasssensors.
		*/		
		uint8_t getValue();
		/*! Verschiebt den aktuellen Wert von getValue() auf 128.
		*/ 
		void setAs128Degree();

		/*! gibt true zurueck wenn der Sensor angeschlossen ist und funktioniert
		 */
		bool checkACK();

	private:
		
		int16_t _128DegreeValue;
		bool _128DegreeEnabled;
};

#endif

