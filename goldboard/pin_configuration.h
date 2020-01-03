/**********************************************************************
@file       pin_configuration.cpp

@brief      Contains the PIN-Configuration of the Goldboard4

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/


/*
                        ATMEL ATmega32
					   
                          +---\\---+
 	   (XCK/T0) D0 PB0	01|        |40  PA0 AI7 D31 (ADC0)
 	       (T1) D1 PB1	02|        |39  PA1 AI6 D30 (ADC1)
 	(INT2/AIN0) D2 PB2	03|        |38  PA2 AI5 D29 (ADC2)
 	 (OC0/AIN1) D3 PB3	04|        |37  PA3 AI4 D28 (ADC3)
	       (SS) D4 PB4	05|        |36  PA4 AI3 D27 (ADC4)
	     (MOSI) D5 PB5	06|        |35  PA5 AI2 D26 (ADC5)
	     (MISO) D6 PB6	07|        |34  PA6 AI1 D25 (ADC6)
	      (SCK) D7 PB7	08|        |33  PA7 AI0 D24 (ADC7)
                 RESET	09|        |32  AREF
                   VCC	10|        |31  GND
                   GND	11|        |30  AVCC
                 XTAL2	12|        |29  PC7 D23 (TOSC2)
                 XTAL1	13|        |28  PC6 D22 (TOSC1)
          (RXD) D8 PD0	14|        |27  PC5 D21 (TDI)
          (TXD) D9 PD1	15|        |26  PC4 D20 (TDO)
        (INT0) D10 PD2	16|        |25  PC3 D19 (TMS)
        (INT1) D11 PD3	17|        |24  PC2 D18 (TCK)
        (OC1B) D12 PD4	18|        |23  PC1 D17 (SDA)
        (OC1A) D13 PD5	19|        |22  PC0 D16 (SCL)
        (ICP1) D14 PD6	20|        |21  PD7 D15 (OC2)
                          +--------+
*/

#ifndef Pins_Config_h
#define Pins_Config_h

#include <avr/pgmspace.h>

#define NOT_ON_TIMER 0
#define TIMER0A 1
#define TIMER0B 2
#define TIMER1A 3
#define TIMER1B 4
#define TIMER2  5

// analog sensors
#define ACOUNT 8
#define A0 39 
#define A1 38
#define A2 37
#define A3 36
#define A4 35
#define A5 34
#define A6 33
#define A7 32
static const uint8_t AARRAY[] = {A0, A1, A2, A3, A4, A5, A6, A7};

// digital sensors
#define DCOUNT 4
#define D0 4
#define D1 5
#define D2 6
#define D3 7
static const uint8_t DARRAY[] = {D0, D1, D2, D3};

// pwm sensors
#define PWMCOUNT 8
#define PWM0 28
#define PWM1 27
#define PWM2 26
#define PWM3 25
#define PWM4 17
#define PWM5 18
#define PWM6 19
#define PWM7 20
static const uint8_t PWMARRAY[] = {PWM0, PWM1, PWM2, PWM3, PWM4, PWM5, PWM6, PWM7};

// motor enable pins
#define MCOUNT 4
#define ME0 0
#define ME1 1
#define ME2 2
#define ME3 3
static const uint8_t MEARRAY[] = {ME0, ME1, ME2, ME3};
	
// Motor direction pins NOTE_ These are on PCF8574AT
#define MD0_0 0
#define MD0_1 1
#define MD1_0 2
#define MD1_1 3
#define MD2_0 4
#define MD2_1 5
#define MD3_0 6
#define MD3_1 7
static const uint8_t MDARRAY[4][2] = {{MD0_0, MD0_1}, {MD1_0, MD1_1}, {MD2_0, MD2_1}, {MD3_0, MD3_1}};

// buttons
#define BTLEDCOUNT 2
#define BTLED0 23
#define BTLED1 24
static const uint8_t BTLEDARRAY[] = {BTLED0, BTLED1};
	
// power
#define PCOUNT 2
#define P0 15
#define P1 16
static const uint8_t PARRAY[] = {P0, P1};

// I2C
/*
Defined in i2c
#define SDA_PORT PORTC
#define SDA_PIN 1
#define SCL_PORT PORTC
#define SCL_PIN 0*/

// UART
#define RXD 13
#define TXD 14


#undef TCCR2A

#define NOT_A_PORT 0
#define NOT_ON_TIMER 0

#define NUM_DIGITAL_PINS            32
#define NUM_ANALOG_INPUTS           8
#define analogInputToDigitalPin(p)  ((p < 8) ? (p) + 24 : -1)
#define digitalPinHasPWM(p)         ((p) == 4 || (p) == 5 || (p) == 7)

#ifdef ARDUINO_MAIN
const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &DDRA,
	(uint16_t) &DDRB,
	(uint16_t) &DDRC,
	(uint16_t) &DDRD,
	NOT_A_PORT,
	NOT_A_PORT,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PORTA,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
	NOT_A_PORT,
	NOT_A_PORT,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PINA,
	(uint16_t) &PINB,
	(uint16_t) &PINC,
	(uint16_t) &PIND,
	NOT_A_PORT,
	NOT_A_PORT,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[40] = {
	POB,  // PB0 ** D0
	POB,  // PB1 ** D1
	POB,  // PB2 ** D2
	POB,  // PB3 ** D3
	POB,  // PB4 ** D4
	POB,  // PB5 ** D5
	POB,  // PB6 ** D6
	POB,  // PB7 ** D7
	NOT_A_PIN, // RESET ** D8
	NOT_A_PIN, // VCC ** D9
	NOT_A_PIN, // GND ** D10
	NOT_A_PIN, // XTAL2 ** D11
	NOT_A_PIN, // XTAL1 ** D12
	POD,  // PD0 ** D13
	POD,  // PD1 ** D14
	POD,  // PD2 ** D15
	POD,  // PD3 ** D16
	POD,  // PD4 ** D17
	POD,  // PD5 ** D18
	POD,  // PD6 ** D19
	POD,  // PD7 ** D20
	POC,  // PC0 ** D21
	POC,  // PC1 ** D22
	POC,  // PC2 ** D23
	POC,  // PC3 ** D24
	POC,  // PC4 ** D25
	POC,  // PC5 ** D26
	POC,  // PC6 ** D27
	POC,  // PC7 ** D28
	NOT_A_PIN, // AVCC ** D29
	NOT_A_PIN, // GND ** D30
	NOT_A_PIN, // AREF ** D31
	POA,  // PA7 ** A0 D32
	POA,  // PA6 ** A1 D33
	POA,  // PA5 ** A2 D34
	POA,  // PA4 ** A3 D35
	POA,  // PA3 ** A4 D36
	POA,  // PA2 ** A5 D37
	POA,  // PA1 ** A6 D38
	POA,  // PA0 ** A7 D39
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[40] = {	
	_BV(0),  // PB0 ** D0
	_BV(1),  // PB1 ** D1
	_BV(2),  // PB2 ** D2
	_BV(3),  // PB3 ** D3
	_BV(4),  // PB4 ** D4
	_BV(5),  // PB5 ** D5
	_BV(6),  // PB6 ** D6
	_BV(7),  // PB7 ** D7
	NOT_A_PIN, // RESET ** D8
	NOT_A_PIN, // VCC ** D9
	NOT_A_PIN, // GND ** D10
	NOT_A_PIN, // XTAL2 ** D11
	NOT_A_PIN, // XTAL1 ** D12
	_BV(0),  // PD0 ** D8
	_BV(1),  // PD1 ** D9
	_BV(2),  // PD2 ** D10
	_BV(3),  // PD3 ** D11
	_BV(4),  // PD4 ** D12
	_BV(5),  // PD5 ** D13
	_BV(6),  // PD6 ** D14
	_BV(7),  // PD7 ** D15
	_BV(0),  // PC0 ** D16
	_BV(1),  // PC1 ** D17
	_BV(2),  // PC2 ** D18
	_BV(3),  // PC3 ** D19
	_BV(4),  // PC4 ** D20
	_BV(5),  // PC5 ** D21
	_BV(6),  // PC6 ** D22
	_BV(7),  // PC7 ** D23
	NOT_A_PIN, // AVCC ** D29
	NOT_A_PIN, // GND ** D30
	NOT_A_PIN, // AREF ** D31
	_BV(7),  // PA7 ** A0 D32
	_BV(6),  // PA6 ** A1 D33
	_BV(5),  // PA5 ** A2 D34
	_BV(4),  // PA4 ** A3 D35
	_BV(3),  // PA3 ** A4 D36
	_BV(2),  // PA2 ** A5 D37
	_BV(1),  // PA1 ** A6 D38
	_BV(0),  // PA0 ** A7 D39
};

#endif
#endif