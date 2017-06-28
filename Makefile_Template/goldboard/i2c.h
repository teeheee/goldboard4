/*
 * c't-Bot
 * 
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your
 * option) any later version. 
 * This program is distributed in the hope that it will be 
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public 
 * License along with this program; if not, write to the Free 
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 * 
 */

/*! 
 * @file    i2c.h
 * @brief   I2C-Treiber, derzeit nur Master, interruptbasiert
 * @author  Timo Sandmann (mail@timosandmann.de)
 * @date    05.09.2007
 *
 * Changes by Roman Steiger
 * @date    17.01.2008
 */


#ifndef __I2C_H__
#define __I2C_H__

#include "global.h"
#include <stdbool.h>
#include <stdlib.h>

#include <util/twi.h>

#define TW_TIMEOUT_ERROR   0xFF

#ifndef I2C_ON_ERROR_FATAL
#define I2C_ON_ERROR_FATAL
#endif

/*----------------------------------------------------------
 * Function prototypes
 */
#ifdef __cplusplus
extern "C" {
#endif

/** Transfer state.
 * true if transfer is finished; false if transfer is active
 */
boolean i2cIsComplete (void);
 
/** TW_NO_INFO if no error occured */
uint8_t i2cGetLastError (void);

/*!
 * Initialisiert das I2C-Modul
 * @param bitrate Init-Wert in kHz
 *
 * SCL frequency = CPU Clock frequency / 16 + 2(TWBR) * 4^TWPS
 *  ï TWBR = Value of the TWI Bit Rate Register
 *  ï TWPS = Value of the prescaler bits in the TWI Status Register
 */
void i2cInit (uint16_t bitrateKHz);

void i2cClose (void);

/*!
 * Sendet nTx Bytes an einen I2C-Slave und liest anschliessend nRx Bytes
 * @param address Slave-Adresse
 * @param *pTx    Zeiger auf Puffer fuer zu sendende Daten
 * @param nTx     Anzahl der zu sendenden Bytes 
 * @param *pRx    Zeiger auf Puffer fuer zu lesende Daten
 * @param nRx     Anzahl der zu lesenden Bytes
 */
bool i2cTransfer (uint8_t address, uint8_t * pTx, uint8_t nTx, uint8_t * pRx, uint8_t nRx);

/*!
 * Sendet ein Byte an einen I2C-Slave und liest anschliessend nRx Bytes
 * @param address Slave-Adresse
 * @param reg     Slave Register
 * @param *pRx    Zeiger auf Puffer fuer zu lesende Daten
 * @param nRx     Anzahl der zu lesenden Bytes
 */
bool i2cRead (uint8_t address, uint8_t reg, uint8_t * pRx, uint8_t nRx);

bool i2cReadRegister (uint8_t address, uint8_t reg, uint8_t* data);

/*!
 * Sendet ein Kommando und ein Byte an einen I2C-Slave
 * @param address Slave-Adresse
 * @param reg     Slave register, das zunaechst an den Slave gesendet wird
 * @param data  Byte, das anschlieﬂend an den Slave gesendet wird
 */
bool i2cWriteRegister (uint8_t address, uint8_t reg, uint8_t data);


bool i2cWriteToSlave (uint8_t address, uint8_t * buf, int count);

/*!
 * Sendet nTx Bytes an einen I2C-Slave
 * @param address Slave-Adresse
 * @param *pTx    Zeiger auf Puffer fuer zu sendende Daten
 * @param nTx     Anzahl der zu sendenden Bytes 
 */
static inline void i2cWrite(uint8_t address, uint8_t * pTx, uint8_t nTx)
{
	i2cWriteToSlave(address, pTx, nTx);
   //i2cTransfer (address, pTx, nTx, NULL, 0);
}


/*!
 * Wartet, bis der aktuelle I2C-Transfer beendet ist
 * @return TW_NO_INFO (0xf8) falls alles ok, sonst Fehlercode
 */
uint8_t i2cWait (void);

/** For internal use only! Don't use! */
void i2cTest (void);

#ifdef  __cplusplus
}
#endif   /* __cplusplus */
//----------------------------------------------------------


#endif   // __I2C_H__
