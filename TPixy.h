//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//
// This file is for defining the Block struct and the Pixy template class.
// (TPixy).  TPixy takes a communication link as a template parameter so that 
// all communication modes (SPI, I2C and UART) can share the same code.  
//

#ifndef _TPIXY_H
#define _TPIXY_H

#include "Arduino.h"
#include "error.h"

// Communication/misc parameters
#define PIXY_INITIAL_ARRAYSIZE      30
#define PIXY_MAXIMUM_ARRAYSIZE      130
#define PIXY_START_WORD             0xaa55
#define PIXY_START_WORD_CC          0xaa56
#define PIXY_START_WORDX            0x55aa
#define PIXY_MAX_SIGNATURE          7
#define PIXY_DEFAULT_ARGVAL         0xffff

// Pixy x-y position values
#define PIXY_MIN_X                  0L
#define PIXY_MAX_X                  319L
#define PIXY_MIN_Y                  0L
#define PIXY_MAX_Y                  199L

// RC-servo values
#define PIXY_RCS_MIN_POS            0L
#define PIXY_RCS_MAX_POS            1000L
#define PIXY_RCS_CENTER_POS         ((PIXY_RCS_MAX_POS-PIXY_RCS_MIN_POS)/2)

enum BlockType
{
	NORMAL_BLOCK, CC_BLOCK
};

/*!
@stuct Block
@ingroup PixyI2C
@addtogroup PixyI2C
*/
struct Block
{
	uint16_t signature;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint16_t angle;
};

/*!
@class TPixy
@ingroup PixyI2C
@addtogroup PixyI2C
*/
template<class LinkType> class TPixy
{
public:
	TPixy(uint16_t arg = PIXY_DEFAULT_ARGVAL);
	~TPixy();

	/** gibt die Anzahl der empfangen Blöcke zurück. 
	*	Wenn die Funkion öfter als alle 20 ms abgefragt wird gibt sie 0 zurück.
	*/
	uint16_t getBlocks(uint16_t maxBlocks = 1000);

	/** steuert den Servo Ausgang der Pixy.
	*/
	int8_t setServos(uint16_t s0, uint16_t s1);

	/** Ändert die Helligkeit der Kamera
	*/
	int8_t setBrightness(uint8_t brightness);

	/** schaltet die RGB Led der Pixy auf den r,g,b Wert.
	*/
	int8_t setLED(uint8_t r, uint8_t g, uint8_t b);
	void init();

	/** in diesem Array steht die Information über die erkannten Objekte drin
	*/
	Block *blocks;

private:
	boolean getStart();
	void resize();

	LinkType link;
	boolean skipStart;
	BlockType blockType;
	uint16_t blockCount;
	uint16_t blockArraySize;
};


template<class LinkType> TPixy<LinkType>::TPixy(uint16_t arg)
{
	skipStart = false;
	blockCount = 0;
	blockArraySize = PIXY_INITIAL_ARRAYSIZE;
	blocks = (Block *) malloc(sizeof(Block) * blockArraySize);
	link.setArg(arg);
}

template<class LinkType> void TPixy<LinkType>::init()
{
	link.init();
}

template<class LinkType> TPixy<LinkType>::~TPixy()
{
	free(blocks);
}

template<class LinkType> boolean TPixy<LinkType>::getStart()
{
	uint16_t w, lastw;

	lastw = 0xffff;
	int timeout = 0;
	while (true)
	{
		w = link.getWord();
		if (w == 0 && lastw == 0)
		{
			delayMicroseconds(10);
			//SERIAL_PRINTLN("nothing to be read");
			return false;
		}
		else if (w == PIXY_START_WORD && lastw == PIXY_START_WORD)
		{
			blockType = NORMAL_BLOCK;
			return true;
		}
		else if (w == PIXY_START_WORD_CC && lastw == PIXY_START_WORD)
		{
			blockType = CC_BLOCK;
			return true;
		}
		else if (w == PIXY_START_WORDX)
		{
			//SERIAL_PRINT("reorder");
			link.getByte(); // resync
		}
		lastw = w;
		timeout++;
		if(timeout>1000)
		{
			ERROR_MESSAGE("Pixy: i2c timeout");
			return false;
		}
	}
}

template<class LinkType> void TPixy<LinkType>::resize()
{
	blockArraySize += PIXY_INITIAL_ARRAYSIZE;
	blocks = (Block *) realloc(blocks, sizeof(Block) * blockArraySize);
}

template<class LinkType> uint16_t TPixy<LinkType>::getBlocks(uint16_t maxBlocks)
{
	uint8_t i;
	uint16_t w, checksum, sum;
	Block *block;

	if (!skipStart)
	{
		if (getStart() == false)
			return 0;
	}
	else
		skipStart = false;

	for (blockCount = 0;
			blockCount < maxBlocks && blockCount < PIXY_MAXIMUM_ARRAYSIZE;)
	{
		checksum = link.getWord();
		if (checksum == PIXY_START_WORD) // we've reached the beginning of the next frame
		{
			skipStart = true;
			blockType = NORMAL_BLOCK;
			//SERIAL_PRINTLN("skip");
			return blockCount;
		}
		else if (checksum == PIXY_START_WORD_CC)
		{
			skipStart = true;
			blockType = CC_BLOCK;
			return blockCount;
		}
		else if (checksum == 0)
			return blockCount;

		if (blockCount > blockArraySize)
			resize();

		block = blocks + blockCount;


		for (i = 0, sum = 0; i < sizeof(Block) / sizeof(uint16_t); i++)
		{
			if (blockType == NORMAL_BLOCK && i >= 5) // skip
			{
				block->angle = 0;
				break;
			}
			w = link.getWord();
			sum += w;
			*((uint16_t *) block + i) = w;
		}

		if (checksum == sum)
			blockCount++;
		else{
		  //SERIAL_PRINT("cs error ");
		}
		w = link.getWord();
		if (w == PIXY_START_WORD)
			blockType = NORMAL_BLOCK;
		else if (w == PIXY_START_WORD_CC)
			blockType = CC_BLOCK;
		else
			return blockCount;
	}
	return 0;
}

template<class LinkType> int8_t TPixy<LinkType>::setServos(uint16_t s0,
		uint16_t s1)
{
	uint8_t outBuf[6];

	outBuf[0] = 0x00;
	outBuf[1] = 0xff;
	*(uint16_t *) (outBuf + 2) = s0;
	*(uint16_t *) (outBuf + 4) = s1;

	return link.send(outBuf, 6);
}

template<class LinkType> int8_t TPixy<LinkType>::setBrightness(
		uint8_t brightness)
{
	uint8_t outBuf[3];

	outBuf[0] = 0x00;
	outBuf[1] = 0xfe;
	outBuf[2] = brightness;

	return link.send(outBuf, 3);
}

template<class LinkType> int8_t TPixy<LinkType>::setLED(uint8_t r, uint8_t g,
		uint8_t b)
{
	uint8_t outBuf[5];

	outBuf[0] = 0x00;
	outBuf[1] = 0xfd;
	outBuf[2] = r;
	outBuf[3] = g;
	outBuf[4] = b;

	return link.send(outBuf, 5);
}

#endif
