#include "sensors.h"




/* Macht das Sensor-/Pinmapping*/
void updateSensorValue() {
#define US_DELAY 65
#define KOMPASS_DELAY 20
#define PIXY_DELAY 20
#define BODEN_DELAY 2
#define IRWert_DELAY 2
	static unsigned long US_time = 0;
	static int counter = 0;

	if (millis() - US_time > US_DELAY && US_ENABLE) {
		/*switch(counter)
		{
		case 0:
			UltraschallWert[0] = US[US_RECHTSVORNE].readRange();
			break;
		case 1:
			UltraschallWert[1] = US[US_RECHTSHINTEN].readRange();
			break;
		case 2:
			UltraschallWert[2] = US[US_LINKSHINTEN].readRange();
			break;
		case 3:
			UltraschallWert[3] = US[US_LINKSVORNE].readRange();
			break;
		}*/
		switch(counter)
		{
			case 0:
				UltraschallWert[0] = US[US_RECHTSVORNE].getValueCMBlocking();
				break;
			case 1:
				UltraschallWert[1] = US[US_RECHTSHINTEN].getValueCMBlocking();
				break;
			case 2:
				UltraschallWert[2] = US[US_LINKSHINTEN].getValueCMBlocking();
				break;
			case 3:
				UltraschallWert[3] = US[US_LINKSVORNE].getValueCMBlocking();
				break;
		}
		counter = (counter + 1) % 4;
		US_time = millis();
	}

	static unsigned long kompass_time = 0;
	if (millis() - kompass_time > KOMPASS_DELAY && KOMPASS_ENABLE) {
		KompassWert = kompass.getValue();
		kompass_time = millis();
	}

	static unsigned long pixy_time = 0;
	if (millis() - pixy_time > PIXY_DELAY && PIXY_ENABLE) {
		Pixy_BlockCount = pixy.getBlocks();
		if(Pixy_BlockCount > 0)
		{
			for(int i = 0; i < Pixy_BlockCount; i++)
			{
				if(pixy.blocks[i].signature == PIXY_BALL_SIGNATUR)
				{
					Pixy_BallWert[0] = pixy.blocks[i].x;
					Pixy_BallWert[1] = pixy.blocks[i].y;
					break;
				}
			}
		}
		pixy_time = millis();
	}

	static unsigned long boden_time = 0;
	if (millis() - boden_time > BODEN_DELAY && BODEN_ENABLE) {
		BodenWert = ring.getValue();
		boden_time = millis();
	}

	static unsigned long IRWert_time = 0;
	if (millis() - IRWert_time > IRWert_DELAY  && IR_ENABLE) {
		IRWert[0] = gb.getPWMPulsedLight(IRWert_VORNE);
		IRWert[1] = gb.getPWMPulsedLight(IRWert_RECHTS);
		IRWert[2] = gb.getPWMPulsedLight(IRWert_HINTEN);
		IRWert[3] = gb.getPWMPulsedLight(IRWert_LINKS);
		IRWert_time = millis();
	}
}
