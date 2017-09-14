/** Project Name: goldboard4_V1_1 
**/

#include "Goldboard4.h"
#include "lcd.h"

Goldboard4 gb;

int main(void)
{	
	lcd_init(LCD_DISP_ON_CURSOR,&gb.digital);
	lcd_puts("hello world");
	while(1)
	{
		delay(100);
	}
}

