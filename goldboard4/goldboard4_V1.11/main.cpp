/** Project Name: goldboard4_V1_1 
**/

#include "Goldboard4.h"
#include "twi.h"

Goldboard4 gb;

int main(void)
{	
	DDRC |= (1<<PC7);
	DDRC |= (1<<PC6);
	PORTC &= ~(1<<PC7);
	PORTC &= ~(1<<PC6);
	while(1)
	{
		gb.motor[1].rotate(100);
		gb.motor[0].rotate(-50);
		gb.motor[3].rotate(-30);
		gb.motor[2].rotate(40);
		delay(500);
		gb.motor[0].rotate(0);
		gb.motor[1].rotate(0);
		gb.motor[2].rotate(0);
		gb.motor[3].rotate(0);
		delay(500);
	}
}

