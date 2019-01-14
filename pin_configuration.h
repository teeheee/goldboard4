/*!
@file


@brief      Contains the PIN-Configuration of the Goldboard4

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de

@editor  2018 Alexander Ulbrich
     alexander.ulbrich@uni-ulm.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version
2.0  30.01.2018 reworked

@end
@ingroup hal
@addtogroup hal
*/


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


/**MOTOR**/

#define MOTORCOUNT 4

#define MOTOR_PWM_PORT PORTB //Pin NUmmer 0, 1, 2, 3
#define MOTOR_PWM_DDR  DDRB

#define MOTOR_PIN0 PB0
#define MOTOR_PIN1 PB1
#define MOTOR_PIN2 PB2
#define MOTOR_PIN3 PB3

#define POWERCOUNT 2

#define POWER_PWM_PORT PORTD //Pin Nummer 1, 2
#define POWER_PWM_DDR  DDRD

#define POWER_PIN0 PD2
#define POWER_PIN1 PD3


/** PWM PULSE IPUT***/

#define PULSECOUNT 8

#define PULSE1_PORT PORTD
#define PULSE1_PIN  PIND
#define PULSE1_DDR  DDRD

#define PULSE1_PIN0 PD4
#define PULSE1_PIN1 PD5
#define PULSE1_PIN2 PD6
#define PULSE1_PIN3 PD7

#define PULSE2_PORT PORTC
#define PULSE2_PIN  PINC
#define PULSE2_DDR  DDRC

#define PULSE2_PIN0 PC4
#define PULSE2_PIN1 PC5
#define PULSE2_PIN2 PC6
#define PULSE2_PIN3 PC7

/*** BTLEDCOUNT ***/

#define BTLEDCOUNT 2

#define BTLED_PORT PORTC
#define BTLED_PIN  PINC
#define BTLED_DDR  DDRC

#define BTLED_PIN0 PC2
#define BTLED_PIN1 PC3

/**** ADC ***/

#define ADCCOUNT 8

#define ADC_PORT   PORTA
#define ADC_PIN    PINA
#define ADC_DDR    DDRA

#define ADC_PIN0   PA0
#define ADC_PIN1   PA1
#define ADC_PIN2   PA2
#define ADC_PIN3   PA3
#define ADC_PIN4   PA4
#define ADC_PIN5   PA5
#define ADC_PIN6   PA6
#define ADC_PIN7   PA7

/**** Digital ****/

#define DIGITALCOUNT 4

#define DIGITAL_PORT PORTB
#define DIGITAL_PIN  PINB
#define DIGITAL_DDR  DDRB

#define DIGITAL_PIN0 PB4
#define DIGITAL_PIN1 PB5
#define DIGITAL_PIN2 PB6
#define DIGITAL_PIN3 PB7




#endif
