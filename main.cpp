
#include "pin_configuration.h"

#include "Goldboard4.h"
#include "SonarSRF08.h"
#include "CMPS11.h"
#include "CMPS03.h"
#include "VL53L0X.h"
#include "usring.h"
#include "HC05.h"
#include "lcd.h"
#include "PixyI2C.h"

Goldboard4 gb;

//#define MINIMAL

#ifndef MINIMAL
//#define TEST_GB
//#define TEST_SRF08
//#define TEST_CMPS11
//#define TEST_CMPS03
//#define TEST_usring
//#define TEST_VL53L0X
//#define TEST_HC05Master
#define TEST_HC05SLAVE
//#define TEST_PIXY
//#define TEST_LCD
#endif

int main(void)
{
	SERIAL_PRINTLN("Buttontest");
	gb.waitForButton(0);
	SERIAL_PRINTLN("Button0");
	delay(500);
	gb.setLed(1,true);
	delay(200);
	gb.setLed(1,false);
	delay(200);
	gb.setLed(0,true);
	delay(200);
	gb.setLed(0,false);
	gb.waitForButton(1);
	SERIAL_PRINTLN("Button1");
	gb.scanI2C();
	while(1)
	{
	#ifdef TEST_GB
SERIAL_PRINTLN("-------------internal TEST--------------");
		gb.speedTest();
		gb.testTime();
SERIAL_PRINTLN("-------------Aktoren TEST--------------");
		gb.testPWM();
		gb.testPowerpins();
		gb.testMotors();
		gb.testServos();
SERIAL_PRINTLN("-------------INPUT TEST--------------");
		gb.testPulse();
		gb.testDigital();
		gb.testAnalog();
SERIAL_PRINTLN("-------------I2C TEST--------------");

	#endif
	#ifdef TEST_SRF08
		SERIAL_PRINTLN("start srf08 test");
		SonarSRF08 sonar(113);
		SERIAL_PRINTLN("set address to 112");
		//sonar.writeAddress(112);
		SERIAL_PRINTLN("measure test");
	    for(int x = 0; x < 10; x++)
		{
			SERIAL_PRINTLN(sonar.readRange());
			delay(500);
		}
		SERIAL_PRINTLN("end srf08 test");
	#endif
	#ifdef TEST_CMPS11
		SERIAL_PRINTLN("start cmps11 test");
		static uint8_t calibrated = 0;
		CMPS11 compass;
		if(calibrated == 0)
		{
			SERIAL_PRINTLN("start cmps11 calibration");
			compass.init();
			compass.startCalibration();
			gb.waitForButton(0);
			compass.exitCalibration();
			delay(30);
			SERIAL_PRINTLN("end cmps11 calibration");
			calibrated = 1;
		}
		compass.setAs128Degree();
		SERIAL_PRINTLN("measure test");
		for(int x = 0; x < 20; x++)
		{
			SERIAL_PRINT("compass=");
			SERIAL_PRINT(compass.getValue());
			SERIAL_PRINT(", acc_y=");
			SERIAL_PRINT(compass.getAccelerometerX());
			SERIAL_PRINT(", acc_x=");
			SERIAL_PRINTLN(compass.getAccelerometerY());
			delay(500);
		}
		SERIAL_PRINTLN("end cmps11 test");	
	#endif
	#ifdef TEST_CMPS03
		SERIAL_PRINTLN("start cmps03 test");
		CMPS03 compass2;
		compass2.init();
		SERIAL_PRINTLN("measure test");
		for(int x = 0; x < 10; x++)
		{
			SERIAL_PRINTLN(compass2.getValue());
			delay(500);
		}
		SERIAL_PRINTLN("end cmps03 test");	
	#endif
	#ifdef TEST_usring
		SERIAL_PRINTLN("start usring test");
		usring ring;
		ring.init();
		SERIAL_PRINTLN("measure test");
		for(int x = 0; x < 10; x++)
		{
			for(int i = 0; i < 16; i++)
			{
				SERIAL_PRINT(ring.getanalogValue(i));
				SERIAL_PRINT("|");
			}
			SERIAL_PRINTLN("");
			delay(500);
		}
		SERIAL_PRINTLN("end usring test");
	#endif
	#ifdef TEST_VL53L0X
		SERIAL_PRINTLN("start vl53l0x test");
		VL53L0X laser;
		laser.init();
		laser.setTimeout(500);
		laser.startContinuous();
		SERIAL_PRINTLN("measure test");
		for(int x = 0; x < 1000; x++)
		{
			SERIAL_PRINTLN(laser.readRangeContinuousMillimeters());	
			if (laser.timeoutOccurred()) { SERIAL_PRINTLN(" TIMEOUT"); }
			delay(10);
		}
		SERIAL_PRINTLN("end vl53l0x test");
	#endif
	#if defined(TEST_HC05Master)
		SERIAL_PRINTLN("start HC05 Master test");
		HC05 bluetooth;
		bluetooth.init(HC05_MASTER,"macadresse");
		int x = 4242;
		for(int i = 0; i < 10; i++)
		{
			SERIAL_PRINTLN("sending");
			bluetooth.write((uint8_t*)&x,2);
			SERIAL_PRINTLN("waiting for data...");
			while(bluetooth.read((uint8_t*)&x,2)==2);
			SERIAL_PRINTLN(x);
			delay(1000);	
		}
		SERIAL_PRINTLN("end HC05 Master test");
	#elif defined(TEST_HC05SLAVE)
		SERIAL_PRINTLN("start HC05 Slave test");
		HC05 bluetooth;
		bluetooth.init(HC05_SLAVE,"none");
		int x;
		for(int i = 0; i < 10; i++)
		{
			SERIAL_PRINTLN("waiting for data...");
			while(bluetooth.read((uint8_t*)&x,2)==2);
			SERIAL_PRINTLN(x);
			SERIAL_PRINTLN("sending");
			bluetooth.write((uint8_t*)&x,2);
			delay(1000);
		}
		SERIAL_PRINTLN("end HC05 Slave test");
	#endif
	#ifdef TEST_PIXY
		SERIAL_PRINTLN("start pixy test");
		PixyI2C pixy;
		for(int i = 0; i < 10; i++)
		{
			int blockcount = pixy.getBlocks();
			for(int b = 0; b < blockcount; b++)
			{
				SERIAL_PRINT("signature=");
				SERIAL_PRINT(pixy.blocks[b].signature);
				SERIAL_PRINT(", x=");
				SERIAL_PRINT(pixy.blocks[b].x);
				SERIAL_PRINT(", y=");
				SERIAL_PRINT(pixy.blocks[b].y);
				SERIAL_PRINTLN("");
			}
		}
		SERIAL_PRINTLN("end pixy test");	
	#endif
	#ifdef TEST_LCD
		lcd_init(LCD_DISP_ON_CURSOR_BLINK,&gb.digital);
		lcd_puts("hallo Welt");
	#endif
	}
}

