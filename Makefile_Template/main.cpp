/** Project Name: goldboard4_V1_1 
**/

#include "Goldboard4.h"
#include "HC05.h"
#include "uart.h"

Goldboard4 gb;
HC05 bt;

#define STATE_CONNECTED 1
#define STATE_SENDING	  2
#define STATE_POWERON	  3
#define STATE_IDLE	  4
#define STATE_ERROR	  5
#define STATE_FINISHED 6

int error = 0;

void setState(int state)
{
	switch(state)
	{
		case STATE_IDLE:
			gb.setLed(0,true);
			gb.setLed(1,false);
			break;

		case STATE_SENDING:
			gb.setLed(0,false);
			gb.setLed(1,false);
			break;

		case STATE_POWERON:
			gb.setLed(0,false);
			gb.setLed(1,true);
			break;

		case STATE_CONNECTED:
			gb.setLed(0,true);
			gb.setLed(1,true);
			break;

		case STATE_ERROR:
			while(1)
			{
				gb.setLed(0,true);
				gb.setLed(1,true);
				delay(1000);
				gb.setLed(0,false);
				gb.setLed(1,false);
				delay(1000);
				for(int i = 0; i < error; i++)
				{
					gb.setLed(0,true);
					gb.setLed(1,true);
					delay(200);
					gb.setLed(0,false);
					gb.setLed(1,false);
					delay(200);
				}
				delay(1000);
			}
			break;

		case STATE_FINISHED:
			for(int i = 0; i < 20; i++)
			{
				gb.setLed(0,true);
				gb.setLed(1,false);
				delay(100);
				gb.setLed(0,false);
				gb.setLed(1,true);
				delay(100);
			}
			break;
	}
}

#define MASTER
//#define SLAVE

int main(void)
{
	int disconnect_counter = 0;
	int resp;
	gb.initLED(0);
	gb.initLED(1);
	setState(STATE_POWERON);

	#ifdef MASTER
		resp = bt.init("Robot2","1111",HC05_MODE_MASTER);
	#endif

	#ifdef SLAVE
		resp = bt.init("Robot1","1111",HC05_MODE_MASTER);
	#endif

	if(resp)
	{
		error = bt.getLastError();
		setState(STATE_ERROR);
	}

	#ifdef SLAVE
		resp = bt.waitforConnection();
	#endif
	#ifdef MASTER
		do
		{
			resp = bt.connectTo("00066672703E");
		}while(resp == 0);
	#endif

	if(resp>1)
	{
		error = bt.getLastError();
		setState(STATE_ERROR);
	}
	setState(STATE_CONNECTED);
	while(1)
	{
		delay(50);
		resp = bt.isConnected();
		if(resp == 1)
		{
			setState(STATE_SENDING);
			#ifdef SLAVE
				bt.sendLine("Slave: hello world\r\n");
			#endif
			#ifdef MASTER
				bt.sendLine("Master: hello world\r\n");
			#endif

			setState(STATE_CONNECTED);
			disconnect_counter++;
		}
		else if(resp == 0)
		{
			setState(STATE_IDLE);
		}
		else
		{
			error = bt.getLastError();
			setState(STATE_ERROR); 
		}


		#ifdef SLAVE
			if(disconnect_counter==20) //disconnect after 20 send
			{
				if(bt.disconnect()==0)
				{
					setState(STATE_FINISHED);
					disconnect_counter=0;
				}
				else
				{
					error = bt.getLastError();
					setState(STATE_ERROR);
				}
			}
		#endif
	}
}

