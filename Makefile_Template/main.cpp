/** Project Name: goldboard4_V1_1 
**/

#include "Goldboard4.h"
#include "HC05.h"
#include "uart.h"

Goldboard4 gb;
HC05 bt;


int main(void)
{	
	gb.initLED(1);
	gb.setLed(1,true);
	while(1)
	{
		uart_puts("asasdfasdf");
		delay(100);
	}
	gb.initLED(1);
	gb.setLed(1,true);
	gb.initLED(0);
	gb.setLed(0,false);
	bt.waitforConnection();
	gb.setLed(0,true);
	while(1)
	{
		if(bt.isConnected())
		{
			delay(100);
			bt.sendLine("hello world\r\n");
			gb.setLed(0,true);
		}
		else
		{
			gb.setLed(0,false);
			delay(1000);
			gb.setLed(0,true);
			delay(100);
		}
		delay(1000);
	}
}

