

#include "pwm.h"

#define PWM_DATA_SIZE (PWM_PIN_COUNT+1)
#define ARRAY_DATA_SIZE (PWM_DATA_SIZE*2)

void copyData();
void sortData();
void calculateOCR();

uint8_t dutyCicles[PWM_PIN_COUNT] =
{ 0 };
uint8_t sortingPointer[PWM_PIN_COUNT] =
{ 0 };

uint8_t OCRArray[ARRAY_DATA_SIZE] =
{ 0 };
uint8_t maskArray[ARRAY_DATA_SIZE] =
{ 0 };

volatile uint8_t* workingOCR = &OCRArray[0];
uint8_t* loadingOCR = &OCRArray[PWM_DATA_SIZE];

volatile uint8_t* workingMask = &maskArray[0];
uint8_t* loadingMask = &maskArray[PWM_DATA_SIZE];

volatile uint8_t pwm_sync = 0;

uint8_t initialized = 0;

inline void doCopyData();

ISR(TIMER2_COMP_vect) // ca 50 Takte
{
	static uint8_t pwm_index = 0;

	if (pwm_index == 0) //anfang der PWM Sequenz
	{ //bits setzen
		if (pwm_sync) //new data -> reset everything
		{
			doCopyData();
			pwm_sync = 0;
			clearBits(0b111111); // irgendwie glitcht das noch ein bisschen :-(
		}
		uint8_t maske = workingMask[pwm_index];
		setBits(maske);
		OCR2 = workingOCR[pwm_index];
	}
	else
	{ //bits loeschen
		uint8_t maske = workingMask[pwm_index];
		clearBits(maske);
		OCR2 = workingOCR[pwm_index];
	}
	pwm_index++;
	if (workingOCR[pwm_index] == 0 || pwm_index == PWM_DATA_SIZE) //Ende erreicht
		pwm_index = 0;
}

void initPwm()
{
	if (initialized)
		return;
	cli();
	for (int i = 0; i < PWM_PIN_COUNT; i++)
	{
		sortingPointer[i] = i;
	}
	calculateOCR();
	doCopyData(); //copy without syncing

	MOTOR_PWM_DDR |= ALL_MOTOR_PINS;
	POWER_PWM_DDR |= ALL_POWER_PINS;

	TCCR2 |= (1 << CS21) | (1 << CS20) | (1 << WGM21);
	TIMSK |= (1 << OCIE2);
	OCR2 = 255;
	sei();
	initialized = 1;
}

void setPwm(uint8_t id, uint8_t duty)
{
	if (id < PWM_PIN_COUNT)
		dutyCicles[id] = duty;
}

void updatePwm() //1393 Takte
{
	while (pwm_sync)
		;
	sortData(); //637 Takte
	calculateOCR(); //527 Takte
	copyData(); // copy with syncing.
}

void sortData() //bubble sort (also try index sorting)
{
	for (uint8_t n = PWM_PIN_COUNT; n > 1; n--)
	{
		for (uint8_t i = 0; i < n - 1; i++)
		{
			if (dutyCicles[sortingPointer[i]]
					> dutyCicles[sortingPointer[i + 1]])
			{
				uint8_t pointer = sortingPointer[i];
				sortingPointer[i] = sortingPointer[i + 1];
				sortingPointer[i + 1] = pointer;
			}
		}
	}
}

inline void doCopyData()
{
	uint8_t* tmpOCR = (uint8_t*) workingOCR;
	uint8_t* tmpMask = (uint8_t*) workingMask;
	workingOCR = loadingOCR;
	workingMask = loadingMask;
	loadingOCR = tmpOCR;
	loadingMask = tmpMask;
}

void copyData()
{
	while (pwm_sync)
		;
	pwm_sync = 1;
}

void calculateOCR()
{
	loadingMask[0] = 0;
	uint8_t time = 0;
	uint8_t edge_index = 0;
	for (uint8_t pin_index = 0; pin_index < PWM_PIN_COUNT; pin_index++)
	{
		uint8_t pin = sortingPointer[pin_index];
		uint8_t dutyCicle = dutyCicles[pin];
		if (dutyCicle > MINIMAL_OCR_OFFSET) //because the interrupt is to slow there is only the option for zero offset or MINIMAL_OCR_OFFSET
		{
			loadingMask[0] |= (1 << pin); //rising edge aktivieren
			int nextOCR = dutyCicle - time;
			if (nextOCR < MINIMAL_OCR_OFFSET / 2) //because the interrupt is to slow there is only the option for zero offset or MINIMAL_OCR_OFFSET
			{
				loadingMask[edge_index] |= (1 << pin); //falling edge aktivieren
			}
			else
			{
				//because the interrupt is to slow there is only the option for zero offset or MINIMAL_OCR_OFFSET
				if (nextOCR < MINIMAL_OCR_OFFSET)
					nextOCR = MINIMAL_OCR_OFFSET;
				loadingMask[edge_index + 1] = (1 << pin); //falling edge aktivieren
				loadingOCR[edge_index] = nextOCR;
				time += nextOCR;
				edge_index++;
			}
		}
	}
	loadingOCR[edge_index] = 0xff - time; //fill the remaining time
	if (edge_index < PWM_DATA_SIZE - 1)
		loadingOCR[edge_index + 1] = 0;  // last ocr is 0
}

#ifdef TEST

#include <util/delay.h>

uint8_t Testcase[6][6] =
{
{ 0, 0, 0, 0, 0, 0 },
{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
{ 125, 125, 125, 125, 125, 125 },
{ 0, 125, 125, 0, 125, 0 },
{ 101, 102, 103, 104, 105, 106 },
{ 60, 20, 40, 30, 10, 50 } };

#define NUMBER_OF_TESTCASES 6

void testPwm()
{
	initPwm();
	cli();
	setBits(0xff);
	_delay_ms(10);
	clearBits(0xff);
	_delay_ms(10);
	setBits(0xff);
	_delay_ms(10);
	clearBits(0xff);
	_delay_ms(10);
	sei();
	for (int i = 0; i < NUMBER_OF_TESTCASES; i++)
	{
		for (int p = 0; p < PWM_PIN_COUNT; p++)
		{
			setPwm(p, Testcase[i][p]);
		}
		updatePwm();
		_delay_ms(10);
		for (int p = 0; p < PWM_PIN_COUNT; p++)
			setPwm(p, Testcase[0][p]);
		updatePwm();
		_delay_ms(5);
	}

	for (int i = 0; i < 255; i++)
	{
		setPwm(0, i);
		setPwm(1, (i+10)%255);
		setPwm(2, (i+20)%255);
		setPwm(3, (i+30)%255);
		setPwm(4, (i+40)%255);
		setPwm(5, (i+50)%255);
		updatePwm();
		_delay_ms(10);
	}
}
#endif
