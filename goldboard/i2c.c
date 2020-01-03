//------------------------------------------------------------------
// i2c.c
//
// This file contains the I2C driver without interrupt
//
// Copyright 2006 by RoCCI e.V.
// Author: Andreas Hausladen
// Adapted by: Roman Steiger
// Also adapted by: Christoph Rothermel
//------------------------------------------------------------------

#include "i2c.h"
//#include "avr_util.h"

// number of tries to connect to a slave
#define MAX_AQUIREBUS_TRIES 20

/* I2C master side status codes */
#define STATUS_START              0x08
#define STATUS_REPEAT_START       0x10
#define STATUS_ARBITRATION_LOST   0x38

#define STATUS_MT_SLA_ACK         0x18
#define STATUS_MT_SLA_NACK        0x20
#define STATUS_MT_DATA_ACK        0x28
#define STATUS_MT_DATA_NACK       0x30

#define STATUS_MR_SLA_ACK         0x40
#define STATUS_MR_SLA_NACK        0x48
#define STATUS_MR_DATA_ACK        0x50
#define STATUS_MR_DATA_NACK       0x58

/* I2C slave side status codes */
#define STATUS_TRANSMIT_REQ       0xA8
#define STATUS_ARB_LOST_TRANSMIT  0xB0
#define STATUS_RECEIVE_REQ        0x60
#define STATUS_ARB_LOST_RECEIVE   0x68
#define STATUS_BROADCAST_REQ      0x70
#define STATUS_ARB_LOST_BROADCAST 0x78

#define STATUS_ST_DATA_ACK        0xB8
#define STATUS_ST_DATA_NACK       0xC0
#define STATUS_ST_LASTDATA_ACK    0xC8

#define STATUS_SR_DATA_ACK        0x80
#define STATUS_SR_DATA_NACK       0x88
#define STATUS_SR_BDATA_ACK       0x90
#define STATUS_SR_BDATA_NACK      0x98
#define STATUS_SR_STOP            0xA0



typedef enum i2cBusMode_e
{
   I2C_TRANSMIT,
   I2C_RECEIVE
} i2cBusMode_t, *i2cBusMode_p;

uint16_t i2cBitrateKHz;


//----------------------------------------------------------
//52: 100 kHz bei 12MHz und I2C_PRESCALER == 0
//72: 100 kHz bei 16MHz und I2C_PRESCALER == 0
void i2cInit (uint16_t bitrateKHz)
{
   uint8_t bitrate_div;

   i2cBitrateKHz = bitrateKHz;
   
   // set i2c bitrate
   // SCL freq = F_CPU/(16+2*TWBR))
#ifdef TWPS0
      // for processors with additional bitrate division (mega128)
      // SCL freq = F_CPU/(16+2*TWBR*4^TWPS)
      // set TWPS to zero
      cbi(TWSR, TWPS0);
      cbi(TWSR, TWPS1);
#endif
   // calculate bitrate division 
   bitrate_div = ((F_CPU/1000l)/bitrateKHz);
   if (bitrate_div >= 16)
      bitrate_div = (bitrate_div - 16) / 2;

   TWBR = bitrate_div;
   TWSR = 0;                       // prescaler
   TWCR = _BV (TWINT) | _BV (TWEN) | _BV (TWEA);     // TWI enable acknowledge bit and TWI enable bit

   // activate SDA and SCL
#if defined (__AVR_ATmega128__)
   PORTD |= _BV (PB1) | _BV (PB0);
#elif defined (__AVR_ATmega32__)
   PORTC |= _BV (PB1) | _BV (PB0);
#else
#  error "Device type not defined"
#endif
}   // i2cInit
//----------------------------------------------------------



//----------------------------------------------------------
void i2cClose (void)
{
  TWCR = _BV (TWINT) | _BV (TWEN) | _BV (TWSTO);
}   // i2cClose
//----------------------------------------------------------



//----------------------------------------------------------
uint8_t i2cWaitForActionFinish (void)
{
   unsigned int counter = 0;
   while (!(TWCR & _BV (TWINT)) && (++counter < 0xFFFF)); // wait for valid status code

   if (counter == 0xFFFF)
   {
      I2C_ON_ERROR_FATAL;
   }
   
   return (TWSR & 0xF8);
}   // i2cWaitForActionFinish
//----------------------------------------------------------



//----------------------------------------------------------
/* i2cWaitForSignal waits until the TWI has terminated the given action
   and it returns the status of the TWI. */
uint8_t i2cWaitForSignal (uint8_t action)
{
  TWCR = _BV (TWINT) | _BV (TWEN) | action;
  return i2cWaitForActionFinish ();
}   // i2cWaitForSignal
//----------------------------------------------------------



//----------------------------------------------------------
/* Opens the I2C bus in one of the two master mode
   (I2C_TRANSMIT, I2C_RECEIVE). If the bus could not be aquired or the address
   was not accepted the bus is i2cClosed and the method returns false. If the
   arbitration was lost, the method retries to aquire the bus for some time. */
bool i2cOpen (uint8_t address, i2cBusMode_t mode)
{
  uint8_t ackStatus = 0;
  uint8_t nackStatus = 0;
  switch (mode)
  {
    case I2C_TRANSMIT:
      ackStatus = STATUS_MT_SLA_ACK;
      nackStatus = STATUS_MT_SLA_NACK;
      break;
    case I2C_RECEIVE:
      ackStatus = STATUS_MR_SLA_ACK;
      nackStatus = STATUS_MR_SLA_NACK;
      break;
  }

  uint8_t status;
  int tries = MAX_AQUIREBUS_TRIES;
  while (tries > 0)
  {
    // aquire bus
    status = i2cWaitForSignal ( _BV (TWSTA) );  // wait until the bus becomes free
    if ( (status != STATUS_START) && (status != STATUS_REPEAT_START))
    {
      TWCR = _BV (TWINT) | _BV (TWEN); // release bus
      return false;
    }

    // send STA+R/W
    if (mode == I2C_TRANSMIT)
      TWDR = address & 0xFE;  // master transmit mode => slave receive request
    else if (mode == I2C_RECEIVE)
      TWDR = address | 1;  // master receive mode => slave transmit request
    status = i2cWaitForSignal (0);  // transmit SLA+R/W

    if (status == ackStatus)
      return true;                // all OK
    if (status == nackStatus)     // retry for some time
    {
      --tries;
      i2cClose ();
      continue;
    }
    if (status == STATUS_ARBITRATION_LOST)
      continue;

    i2cClose ();                     // error
    return false;
  }
  return false;
}   // i2cOpen
//----------------------------------------------------------



//----------------------------------------------------------
bool i2cWriteData (uint8_t data)
{
  TWDR = data;
  uint8_t status = i2cWaitForSignal ( 0 );
  return (status == STATUS_MT_DATA_ACK) || (status == STATUS_MT_DATA_NACK);
}   // i2cWriteData
//----------------------------------------------------------



//----------------------------------------------------------
bool i2cReadData (uint8_t* pData, bool last)
{
  uint8_t status = i2cWaitForSignal ( last ? 0 : _BV (TWEA) );
  *pData = TWDR;
  return (status == STATUS_MR_DATA_ACK) || (status == STATUS_MR_DATA_NACK);
}   // i2cReadData
//----------------------------------------------------------



//----------------------------------------------------------
bool i2cWriteToSlave (uint8_t address, uint8_t buf [], int count)
{
  if (count > 0 && buf)
  {
    if (!i2cOpen (address, I2C_TRANSMIT))
      return false;
      
    for (int i = 0; i < count; ++i)
    {
      if (!i2cWriteData (buf [i]))
      {
        if ( (TWSR & 0xF8) != STATUS_ARBITRATION_LOST)
          i2cClose ();
        return false;
      }
    }
    i2cClose ();
  }
  return true;
}   // i2cWriteToSlave
//----------------------------------------------------------



//----------------------------------------------------------
bool i2cReadFromSlave (uint8_t address, uint8_t* pRx, uint8_t nRx)
{
  if (nRx > 0 && pRx)
  {
    if (!i2cOpen (address, I2C_RECEIVE))
      return false;
      
    for (int i = 0; i < nRx; ++i)
      if (!i2cReadData (&pRx  [i], (i == nRx - 1)))
      {
        if ( (TWSR & 0xF8) != STATUS_ARBITRATION_LOST)
          i2cClose ();
        return false;
      }
    i2cClose ();
  }
  return true;
}   // i2cReadFromSlave
//----------------------------------------------------------



//----------------------------------------------------------
bool i2cWriteRegister (uint8_t address, uint8_t reg, uint8_t data)
{
  uint8_t buf [2];
  buf [0] = reg;
  buf [1] = data;
  return i2cWriteToSlave (address, buf, 2);
}   // i2cWriteRegister
//----------------------------------------------------------



//----------------------------------------------------------
bool i2cReadRegister (uint8_t address, uint8_t reg, uint8_t* data)
{
  data = 0;
  if (!i2cOpen (address, I2C_TRANSMIT))
    return false;
    
  if (!i2cWriteData (reg))
    return false;

  if (!i2cOpen (address, I2C_RECEIVE))
    return false;
    
  if (!i2cReadData (data, true))
    return false;
    
  i2cClose ();
  return true;
}   // i2cReadRegister
//----------------------------------------------------------



//----------------------------------------------------------
bool i2cRead (uint8_t address, uint8_t reg, uint8_t* pRx, uint8_t nRx)
{
  if (!i2cOpen (address, I2C_TRANSMIT))
    return false;

  if (!i2cWriteData (reg))
    return false;

  return i2cReadFromSlave (address, pRx, nRx);
}   // i2cRead
//----------------------------------------------------------



//----------------------------------------------------------
uint8_t i2cWait (void)
{
   return TW_NO_INFO;
}

inline boolean i2cIsComplete ()
{
   return true;
}
 
inline uint8_t i2cGetLastError (void)
{
   return TW_NO_INFO;
}
//----------------------------------------------------------


