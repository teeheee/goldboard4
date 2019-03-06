

#include "interrupt.h"
#include "config.h"
#include <avr/interrupt.h>

volatile uint8_t interrupt_mode = 0;
volatile uint8_t interrupt_mask = 0;
volatile uint8_t interrupt_running = 0;
void (*interrupt_callback_array[8])();

void interrupt_isr()
{
  if(interrupt_running)
    return;
  interrupt_running = 1;
  sei();
  static uint8_t last_state = 0;
  uint8_t state = 0;
  state |= (PULSE1_PIN & ALL_PULSE1_PINS); //PD7 ... PD4
  state |= (PULSE2_PIN & ALL_PULSE2_PINS) >> 4; //PC7 ... PC4
  state = __builtin_avr_insert_bits(0x76540123,state,0);

  uint8_t is_active = (state ^ last_state) & interrupt_mask;
  last_state = state;
  if( is_active ) {
    uint8_t run = is_active & (interrupt_mode ^ state);
    for(uint8_t index = 0; index < 8; index++){
      if( run & (1 << index) )
        (*interrupt_callback_array[index])();
    }
  }

  interrupt_running = 0;
}

void dummy_function(){

}

void interrupt_init()
{
	PULSE1_DDR &= ~(ALL_PULSE1_PINS); //as input
	PULSE2_DDR &= ~(ALL_PULSE2_PINS); //as input
	PULSE1_PORT |= (ALL_PULSE1_PINS); //pull up on
	PULSE2_PORT |= (ALL_PULSE2_PINS); //pull up on
  for(uint8_t index = 0; index < 8; index++){
      interrupt_callback_array[index] = &dummy_function;
  }
}


void interrupt_attach(uint8_t pin, void (*callback)(), uint8_t mode)
{
  interrupt_callback_array[pin] = callback;
  interrupt_mask |= (1<<pin);
  if(mode)
    interrupt_mode |= (1<<pin);
  else
    interrupt_mode &= ~(1<<pin);
}

void interrupt_detach(uint8_t pin)
{
  interrupt_mask &= ~(1<<pin);
  interrupt_mode &= ~(1<<pin);
  interrupt_callback_array[pin] = &dummy_function;
}
