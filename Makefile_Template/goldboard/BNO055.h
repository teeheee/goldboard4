/**********************************************************************
@file       BNO055.h

@brief      Driver the BNO055 compass module

-----------------------------------------------------------------------
@author  2017 Alexander Ulbrich
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#ifndef _BNO055_h
#define _BNO055_h

#include "global.h"

#include "i2c.h"

      /* Page id register definition */
#define 	BNO055_PAGE_ID_ADDR  		       0X07

      /* PAGE0 REGISTER DEFINITION START*/
#define      BNO055_CHIP_ID_ADDR 			   0x00
#define      BNO055_ACCEL_REV_ID_ADDR   	   0x01
#define      BNO055_MAG_REV_ID_ADDR     	   0x02
#define      BNO055_GYRO_REV_ID_ADDR    	   0x03
#define      BNO055_SW_REV_ID_LSB_ADDR  	   0x04
#define      BNO055_SW_REV_ID_MSB_ADDR         0x05
#define      BNO055_BL_REV_ID_ADDR             0X06

      /* Accel data register */
#define      BNO055_ACCEL_DATA_X_LSB_ADDR      0X08
#define      BNO055_ACCEL_DATA_X_MSB_ADDR      0X09
#define      BNO055_ACCEL_DATA_Y_LSB_ADDR      0X0A
#define      BNO055_ACCEL_DATA_Y_MSB_ADDR      0X0B
#define      BNO055_ACCEL_DATA_Z_LSB_ADDR      0X0C
#define      BNO055_ACCEL_DATA_Z_MSB_ADDR      0X0D

      /* Mag data register */
#define      BNO055_MAG_DATA_X_LSB_ADDR        0X0E
#define      BNO055_MAG_DATA_X_MSB_ADDR        0X0F
#define      BNO055_MAG_DATA_Y_LSB_ADDR        0X10
#define      BNO055_MAG_DATA_Y_MSB_ADDR        0X11
#define      BNO055_MAG_DATA_Z_LSB_ADDR        0X12
#define      BNO055_MAG_DATA_Z_MSB_ADDR        0X13

      /* Gyro data registers */
#define      BNO055_GYRO_DATA_X_LSB_ADDR       0X14
#define      BNO055_GYRO_DATA_X_MSB_ADDR       0X15
#define      BNO055_GYRO_DATA_Y_LSB_ADDR       0X16
#define      BNO055_GYRO_DATA_Y_MSB_ADDR       0X17
#define      BNO055_GYRO_DATA_Z_LSB_ADDR       0X18
#define      BNO055_GYRO_DATA_Z_MSB_ADDR       0X19

      /* Euler data registers */
#define      BNO055_EULER_H_LSB_ADDR            0X1A
#define      BNO055_EULER_H_MSB_ADDR            0X1B
#define      BNO055_EULER_R_LSB_ADDR            0X1C
#define      BNO055_EULER_R_MSB_ADDR            0X1D
#define      BNO055_EULER_P_LSB_ADDR            0X1E
#define      BNO055_EULER_P_MSB_ADDR            0X1F


      /* Linear acceleration data registers */
#define      BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR     0X28
#define      BNO055_LINEAR_ACCEL_DATA_X_MSB_ADDR     0X29
#define      BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR     0X2A
#define      BNO055_LINEAR_ACCEL_DATA_Y_MSB_ADDR     0X2B
#define      BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR     0X2C
#define      BNO055_LINEAR_ACCEL_DATA_Z_MSB_ADDR     0X2D

      /* Gravity data registers */
#define      BNO055_GRAVITY_DATA_X_LSB_ADDR   0X2E
#define      BNO055_GRAVITY_DATA_X_MSB_ADDR   0X2F
#define      BNO055_GRAVITY_DATA_Y_LSB_ADDR   0X30
#define      BNO055_GRAVITY_DATA_Y_MSB_ADDR   0X31
#define      BNO055_GRAVITY_DATA_Z_LSB_ADDR   0X32
#define      BNO055_GRAVITY_DATA_Z_MSB_ADDR   0X33

      /* Temperature data register */
#define      BNO055_TEMP_ADDR                 0X34

      /* Status registers */
#define      BNO055_CALIB_STAT_ADDR           0X35
#define      BNO055_SELFTEST_RESULT_ADDR      0X36
#define      BNO055_INTR_STAT_ADDR            0X37

#define      BNO055_SYS_CLK_STAT_ADDR         0X38
#define      BNO055_SYS_STAT_ADDR             0X39
#define      BNO055_SYS_ERR_ADDR              0X3A

      /* Unit selection register */
#define      BNO055_UNIT_SEL_ADDR             0X3B
#define      BNO055_DATA_SELECT_ADDR          0X3C

      /* Mode registers */
#define      BNO055_OPR_MODE_ADDR             0X3D
#define      BNO055_PWR_MODE_ADDR             0X3E

#define      BNO055_SYS_TRIGGER_ADDR          0X3F
#define      BNO055_TEMP_SOURCE_ADDR          0X40

      /* Operation mode settings*/
#define      OPERATION_MODE_CONFIG            0X00
#define      OPERATION_MODE_ACCONLY           0X01
#define      OPERATION_MODE_MAGONLY           0X02
#define      OPERATION_MODE_GYRONLY           0X03
#define      OPERATION_MODE_ACCMAG            0X04
#define      OPERATION_MODE_ACCGYRO           0X05
#define      OPERATION_MODE_MAGGYRO           0X06
#define      OPERATION_MODE_AMG               0X07
#define      OPERATION_MODE_IMUPLUS           0X08
#define      OPERATION_MODE_COMPASS           0X09
#define      OPERATION_MODE_M4G               0X0A
#define      OPERATION_MODE_NDOF_FMC_OFF      0X0B
#define      OPERATION_MODE_NDOF              0X0C

#define BNO055_I2C_ADDR   		0xC0  /*!< I2C-Address of the module */
#define BNO055_LORES_REG		1     /* Register for 8 bit measurement (0-255) */
#define BNO055_HIRES_REG		2     /* Register for 16 bit measurement (0-3599 (0-359,9°)) */

#define BNO055_ERROR_TIMEOUT	100	  /* After this time there MUST be a value */

class BNO055
{
	public:
		BNO055();
		void init();
		bool isInitialized();
		uint8_t getValue();
		void setAs128Degree();
		void calibrate(uint8_t type);
		bool isCalibrating();
		void setmode(uint8_t mode);

	private:
		void startMeasurement();
		bool getMeasurementValue(int16_t* val);
		
		uint8_t _value;
		uint8_t _lastValue;
		int16_t _128DegreeValue;
		bool _128DegreeEnabled;
		bool _measurementStarted;
		bool _initialized;
};

#endif

