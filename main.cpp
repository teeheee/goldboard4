#include "Goldboard4.h"

Goldboard4 gb;

int main(){
	int ledstate = 0;	
	gb.scanI2C();
	gb.testDigital();
	delay(1000);
	while(1)
	{
				if(!gb.getButton(0) && !gb.getButton(1)){
	 				ledstate ^= 1;
					if(ledstate){
						gb.setLed(1,true);
						gb.setLed(0,false);
					} else {
						gb.setLed(1,false);
						gb.setLed(0,true);
					}
					SERIAL_PRINT("\033c");
					SERIAL_PRINT("Analog: ");
					for(int i = 0; i < 8; i++){
						SERIAL_PRINT("\t");
						serial.print(gb.getAnalog(i),DEC);
						SERIAL_PRINT("  ");
					}
					SERIAL_PRINTLN("");
					SERIAL_PRINT("                                                                    \r");
					SERIAL_PRINT("Digital LOW: ");
					for(int i = 0; i < 28; i++){
						if(!gb.getDigital(i))
							SERIAL_PRINT(i);
							SERIAL_PRINT(",");
					}
					SERIAL_PRINTLN("");
					SERIAL_PRINT("                                                                    \r");
					SERIAL_PRINT("Digital HIGH: ");
					for(int i = 0; i < 28; i++){
						if(gb.getDigital(i))
							SERIAL_PRINT(i);
							SERIAL_PRINT(",");
					}
					SERIAL_PRINTLN("");
					delay(500);
				}
				else if(gb.getButton(0) && !gb.getButton(1)){
					SERIAL_PRINT("\033c");
					SERIAL_PRINT("Digital output");
					for(int i = 0; i < 28; i++){
						gb.setDigital(i,0);
					}
					for(int i = 27; i >= 0; i--){
						gb.setDigital(i,1);
						delay(1);
					}
					delay(1);
				}
				else if(gb.getButton(1) && !gb.getButton(0)){
					SERIAL_PRINT("\033c");
					SERIAL_PRINT("Motor 0");
					for(int i = -300; i < 300; i++){
						gb.motor[0].rotate(i);
						delay(10);
					}
					gb.setMotorsOff();
					SERIAL_PRINT("\033c");
					SERIAL_PRINT("Motor 1");
					for(int i = -300; i < 300; i++){
						gb.motor[1].rotate(i);
						delay(10);
					}
					gb.setMotorsOff();
					SERIAL_PRINT("\033c");
					SERIAL_PRINT("Motor 2");
					for(int i = -300; i < 300; i++){
						gb.motor[2].rotate(i);
						delay(10);
					}
					gb.setMotorsOff();
					SERIAL_PRINT("\033c");
					SERIAL_PRINT("Motor 3");
					for(int i = -300; i < 300; i++){
						gb.motor[3].rotate(i);
						delay(10);
					}
					gb.setMotorsOff();
					SERIAL_PRINT("\033c");
					SERIAL_PRINT("POWERPWM 0");
					for(int i = 0; i < 300; i++){
						gb.setPowerPWM(0,i);
						delay(10);
					}					
					gb.setPowerPWM(0,0);
					SERIAL_PRINT("\033c");
					SERIAL_PRINT("POWERPWM 1");
					for(int i = 0; i < 300; i++){
						gb.setPowerPWM(1,i);
						delay(10);
					}
					gb.setPowerPWM(1,0);
					SERIAL_PRINT("\033c");
					SERIAL_PRINT("SET POWER 0");
					gb.setPower(0,true);
					delay(1000);
					gb.setPower(0,false);
					SERIAL_PRINT("\033c");
					SERIAL_PRINT("SET POWER 0");
					gb.setPower(1,true);
					delay(1000);
					gb.setPower(1,false);

				}
	}
	return 0;
}
