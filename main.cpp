#include "Goldboard4.h"
#include "LSM303.h"

Goldboard4 gb;

int main(){
	LSM303 lsm;

 	lsm.init();
  	lsm.enableDefault();

  	lsm.m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
	lsm.m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};
	while(1)
	{
		lsm.read();
		SERIAL_PRINTLN(lsm.heading());
		delay(500);
	}
	return 0;
}
