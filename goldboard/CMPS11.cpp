#include "CMPS11.h"
#include "Goldboard4.h"
#include "Wire.h"
#include "time.h"
#include "error.h"

const char error_prefix[] PROGMEM = "CMPS11: ";

CMPS11::CMPS11() {
	init();
}

void CMPS11::init() {
	_128DegreeValue = 0;
	_128DegreeEnabled = false;
}

uint8_t CMPS11::getValue() {
	int16_t val = 0;

	Wire.beginTransmission(CMPS11_I2C_ADDR);  //starts communication with CMPS11
	Wire.write(CMPS11_ANGLE_8_REG); //Sends the register we wish to start reading from
	if (Wire.endTransmission() != 0) {
		ERROR_MESSAGE_PREFIX(error_prefix, progmem_no_ack);
		return 0;
	}

	Wire.requestFrom(CMPS11_I2C_ADDR, 1);
	unsigned long time = millis();
	while (Wire.available() < 1)
		if (millis() - time > 1000) {
			ERROR_MESSAGE_PREFIX(error_prefix, progmem_timeout);
			return false;
		}
	val = Wire.read();

	if (_128DegreeEnabled) {
		val += _128DegreeValue;
		if (val >= 255)
			val = 0 + (val - 255);
		else if (val < 0)
			val = 255 + val;
	}

	return (uint8_t) val;
}

int CMPS11::getAccelerometerX() {
	int val = 0;
	Wire.beginTransmission(CMPS11_I2C_ADDR);  //starts communication with CMPS11
	Wire.write(CMPS11_ACCELERATION_X_REG); //Sends the register we wish to start reading from
	if (Wire.endTransmission() != 0) {
		ERROR_MESSAGE_PREFIX(error_prefix, progmem_no_ack);
		return 0;
	}

	Wire.requestFrom(CMPS11_I2C_ADDR, 2);
	unsigned long time = millis();
	while (Wire.available() < 2)
		if (millis() - time > CMPS11_ERROR_TIMEOUT) {
			ERROR_MESSAGE_PREFIX(error_prefix, progmem_timeout);
			return 0;
		}       // Wait for all bytes to come back
	val = Wire.read();               // Read back the 5 bytes
	val = Wire.read() + (val << 8);
	return val;
}

int CMPS11::getAccelerometerY() {
	int val = 0;
	Wire.beginTransmission(CMPS11_I2C_ADDR);  //starts communication with CMPS11
	Wire.write(CMPS11_ACCELERATION_Y_REG); //Sends the register we wish to start reading from
	if (Wire.endTransmission() != 0) {
		ERROR_MESSAGE_PREFIX(error_prefix, progmem_no_ack);
		return 0;
	}

	Wire.requestFrom(CMPS11_I2C_ADDR, 2);
	unsigned long time = millis();
	while (Wire.available() < 2)
		if (millis() - time > CMPS11_ERROR_TIMEOUT) {
			ERROR_MESSAGE_PREFIX(error_prefix, progmem_timeout);
			return false;
		}         // Wait for all bytes to come back
	val = Wire.read();               // Read back the 5 bytes
	val = Wire.read() + (val << 8);
	return val;
}

void CMPS11::setAs128Degree() {
	uint8_t errorCounter = 0;

	_128DegreeValue = 0;
	_128DegreeEnabled = false;
	while (_128DegreeValue == 0) {
		_128DegreeValue = getValue();
		delay(10);

		errorCounter++;

		if (errorCounter > 10) {
			ERROR_MESSAGE_PREFIX(error_prefix, "set 128 error");
			return;
		}
	}

	_128DegreeValue = 128 - _128DegreeValue;
	_128DegreeEnabled = true;
}


void CMPS11::cmd(uint8_t byte) {
	Wire.beginTransmission(CMPS11_I2C_ADDR);
	Wire.write(0);
	Wire.write(byte);
	if (Wire.endTransmission() != 0) {
		ERROR_MESSAGE_PREFIX(error_prefix, progmem_no_ack);
	}
	delay(50);
}

void CMPS11::startCalibration() {
	uint8_t buffer[3] = CMPS11_CALIBRATION_SEQUENCE;
	cmd(buffer[0]);
	cmd(buffer[1]);
	cmd(buffer[2]);
}

void CMPS11::exitCalibration() {
	cmd(0xF8);
	delay(50);
}
