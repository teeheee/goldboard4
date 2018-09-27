/*!
@file

 @brief      Driver for motors on Goldboard4

 -----------------------------------------------------------------------
 @author  2015 Christoph Rothermel
 Christoph.Rothermel@gmx.de

 @editor 2018 Alexander Ulbrich
 alexander.ulbrich@uni-ulm.de
 -----------------------------------------------------------------------
 @History

 1.00 28.01.2015 First working version
 2.0  30.01.2018 new back-end added

 @end
@ingroup gb4
@addtogroup gb4
@end
*/

#ifndef _MOTOR_h
#define _MOTOR_h

#include "PCF8574A.h"


#define MIN_SPEED 40
/*!
@class Motor
@brief Für diese Klasse muss kein Objekt erstellt werden. Es ist Teil der Goldboard4 Klasse und kann über das gb Objekt angesprochen werden.
		Typische Verwendung hat die Form gb.motors[2].rotate(20);
@ingroup zf
@addtogroup zf
*/
class Motor
{
	public:
		Motor();
		/*! Initialisiert den Motor. Diese Funktion wird von der Goldboard4 Klasse aufgerufen und sollte nicht verwendet werden.
		 */
		void init(uint8_t pwmPin, PCF8574A* directionPortexpander);

		/*! Dreht den Motor mit einer bestimmten Geschwindigkeit. Bei starken Änderungen wird die Beschleunigung gedrosselt um den Motor zu schonen.
			@param[in] speed setzt die Geschwindigkeit des Motors in Prozent [-100 bis 100]. Das
						Vorzeichen gibt die Drehrichtung an.
		 */
		void rotate(int16_t speed);

		/*! Verändert die maximale Beschleunigung aller Motoren.
			@param[in] percentPerHundretMs setzt die Beschleunigung mit der Rate Prozent pro hundert Millisekunden.
						Ein Wert von 50 würde die Geschwindigkeit um maximal 50% alle 100ms erhöhen.
		 */
		void setAcceleration(uint8_t percentPerHundretMs);
		

		void setMinSpeed(uint8_t minspeed);
	private:
		uint8_t _pwmPin;
		PCF8574A* _directionPortexpander;
		uint8_t _minspeed;
		
};

/*! Interrupt service Routine der Motor Klasse. Sie kümmert sich nur um das begrenzen 
der Beschleunigung und ändern der Richtung (I2C). Aufgrund der verwendung von I2C müssen Interrupts 
aktiv sein da sonst I2C nicht funktioniert. Um kritischen Wettlauf zu vermeiden darf die Funktion maximal 
einmal zur selben Zeit ausgeführt werden. Falls sie zu oft aufgerufen wird, kann es zu seltsamen Fehlern kommen.	 
*/
void motor_isr();

#endif

