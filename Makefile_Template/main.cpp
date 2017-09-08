/** Project Name: goldboard4_V1_1 
**/

#include "Goldboard4.h"
#include "lcd.h"

Goldboard4 gb;

int main(void)
{	
	gb.initLED(1);
	gb.setLed(1,0);
	//lcd_init(LCD_DISP_ON_CURSOR_BLINK,&gb.digital);
	//lcd_puts("hello world");
	while(1)
	{
		gb.scanI2C();
		gb.motor[1].rotate(100);
		delay(100);
		gb.setLed(1,0);
		gb.motor[1].rotate(-100);
		delay(100);
		gb.setLed(1,1);
	}
}

