/****************************************************************************
 Title:     HD44780U LCD library
 Author:    Peter Fleury <pfleury@gmx.ch>  http://tinyurl.com/peterfleury
 File:	    $Id: lcd.c,v 1.15.2.2 2015/01/17 12:16:05 peter Exp $
 Software:  AVR-GCC 3.3 
 Target:    any AVR device, memory mapped mode only for AT90S4414/8515/Mega

 DESCRIPTION
       Basic routines for interfacing a HD44780U-based text lcd display

       Originally based on Volker Oth's lcd library,
       changed lcd_init(), added additional constants for lcd_command(),
       added 4-bit I/O mode, improved and optimized code.

       Library can be operated in memory mapped mode (LCD_IO_MODE=0) or in 
       4-bit IO port mode (LCD_IO_MODE=1). 8-bit IO port mode not supported.
       
       Memory mapped mode compatible with Kanda STK200, but supports also
       generation of R/W signal through A8 address line.

 USAGE
       See the C include lcd.h file for a description of each function
       
*****************************************************************************/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd.h"




PCF8574A* lcd_pcf;


#if LCD_IO_MODE
#define lcd_e_delay()   _delay_us(LCD_DELAY_ENABLE_PULSE)
#define lcd_e_high()    lcd_pcf->setPin(LCD_E_PIN,1) // LCD_E_PORT  |=  _BV(LCD_E_PIN);
#define lcd_e_low()     lcd_pcf->setPin(LCD_E_PIN,0) // LCD_E_PORT  &= ~_BV(LCD_E_PIN);
#define lcd_e_toggle()  toggle_e()
#define lcd_rw_high()   lcd_pcf->setPin(LCD_RW_PIN,1) // LCD_RW_PORT |=  _BV(LCD_RW_PIN)
#define lcd_rw_low()    lcd_pcf->setPin(LCD_RW_PIN,0) // LCD_RW_PORT &= ~_BV(LCD_RW_PIN)
#define lcd_rs_high()   lcd_pcf->setPin(LCD_RS_PIN,1) // LCD_RS_PORT |=  _BV(LCD_RS_PIN)
#define lcd_rs_low()    lcd_pcf->setPin(LCD_RS_PIN,0) // LCD_RS_PORT &= ~_BV(LCD_RS_PIN)

#define lcd_d0_high()   lcd_pcf->setPin(LCD_DATA0_PIN,1) 
#define lcd_d0_low()    lcd_pcf->setPin(LCD_DATA0_PIN,0) 
#define lcd_d1_high()   lcd_pcf->setPin(LCD_DATA1_PIN,1) 
#define lcd_d1_low()    lcd_pcf->setPin(LCD_DATA1_PIN,0) 
#define lcd_d2_high()   lcd_pcf->setPin(LCD_DATA2_PIN,1) 
#define lcd_d2_low()    lcd_pcf->setPin(LCD_DATA2_PIN,0) 
#define lcd_d3_high()   lcd_pcf->setPin(LCD_DATA3_PIN,1) 
#define lcd_d3_low()    lcd_pcf->setPin(LCD_DATA3_PIN,0) 

#define lcd_data_update() lcd_pcf->write()

#endif

#if LCD_IO_MODE
#if LCD_LINES==1
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_1LINE 
#else
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_2LINES 
#endif
#else
#if LCD_LINES==1
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_8BIT_1LINE
#else
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_8BIT_2LINES
#endif
#endif

#if LCD_CONTROLLER_KS0073
#if LCD_LINES==4

#define KS0073_EXTENDED_FUNCTION_REGISTER_ON  0x2C   /* |0|010|1100 4-bit mode, extension-bit RE = 1 */
#define KS0073_EXTENDED_FUNCTION_REGISTER_OFF 0x28   /* |0|010|1000 4-bit mode, extension-bit RE = 0 */
#define KS0073_4LINES_MODE                    0x09   /* |0|000|1001 4 lines mode */

#endif
#endif

/* 
** function prototypes 
*/
#if LCD_IO_MODE
static void toggle_e(void);
#endif

/*
** local functions
*/


/************************************************************************* 
delay for a minimum of <us> microseconds
the number of loops is calculated at compile-time from MCU clock frequency
*************************************************************************/
#define delay(us)  _delay_us(us) 


#if LCD_IO_MODE
/* toggle Enable Pin to initiate write */
static void toggle_e(void)
{
    lcd_e_high();
    lcd_data_update();
    lcd_e_delay();
    lcd_e_low();
    lcd_data_update();
}
#endif


/*************************************************************************
Low-level function to write byte to LCD controller
Input:    data   byte to write to LCD
          rs     1: write data    
                 0: write instruction
Returns:  none
*************************************************************************/
#if LCD_IO_MODE
static void lcd_write(uint8_t data,uint8_t rs) 
{

    if (rs) {        /* write data        (RS=1, RW=0) */
       lcd_rs_high();
    } else {         /* write instruction (RS=0, RW=0) */
       lcd_rs_low();
    }
    lcd_data_update();
    lcd_rw_low();    /* RW=0  write mode      */
    lcd_data_update();

    
        
        /* output high nibble first */
        lcd_d3_low();
        lcd_d2_low();
        lcd_d1_low();
        lcd_d0_low();
    	if(data & 0x80) lcd_d3_high();
    	if(data & 0x40) lcd_d2_high();
    	if(data & 0x20) lcd_d1_high();
    	if(data & 0x10) lcd_d0_high();   
        lcd_data_update();
        lcd_e_toggle();
        
        /* output low nibble */
        lcd_d3_low();
        lcd_d2_low();
        lcd_d1_low();
        lcd_d0_low();
    	if(data & 0x08) lcd_d3_high();
    	if(data & 0x04) lcd_d2_high();
    	if(data & 0x02) lcd_d1_high();
    	if(data & 0x01) lcd_d0_high();
        lcd_data_update();

        lcd_e_toggle();        
        
        /* all data pins high (inactive) */
        lcd_d0_high();
        lcd_d1_high();
        lcd_d2_high();
        lcd_d3_high();
        lcd_data_update();
}
#else
#define lcd_write(d,rs) if (rs) *(volatile uint8_t*)(LCD_IO_DATA) = d; else *(volatile uint8_t*)(LCD_IO_FUNCTION) = d;
/* rs==0 -> write instruction to LCD_IO_FUNCTION */
/* rs==1 -> write data to LCD_IO_DATA */
#endif



/*************************************************************************
loops while lcd is busy, returns address counter
*************************************************************************/
static uint8_t lcd_waitbusy(void)

{
    register uint8_t c;
    
    /* wait until busy flag is cleared */
  //LCD_DATA0_PORT  while ( (c=lcd_read(0)) & (1<<LCD_BUSY)) {}
    
    /* the address counter is updated 4us after the busy flag is cleared */
    delay(LCD_DELAY_BUSY_FLAG);

    /* now read the address counter */
    return 0;//(lcd_read(0));  // return address counter
    
}/* lcd_waitbusy */


/*************************************************************************
Move cursor to the start of next line or to the first line if the cursor 
is already on the last line.
*************************************************************************/
static inline void lcd_newline(uint8_t pos)
{
    register uint8_t addressCounter;


#if LCD_LINES==1
    addressCounter = 0;
#endif
#if LCD_LINES==2
    if ( pos < (LCD_START_LINE2) )
        addressCounter = LCD_START_LINE2;
    else
        addressCounter = LCD_START_LINE1;
#endif
#if LCD_LINES==4
#if KS0073_4LINES_MODE
    if ( pos < LCD_START_LINE2 )
        addressCounter = LCD_START_LINE2;
    else if ( (pos >= LCD_START_LINE2) && (pos < LCD_START_LINE3) )
        addressCounter = LCD_START_LINE3;
    else if ( (pos >= LCD_START_LINE3) && (pos < LCD_START_LINE4) )
        addressCounter = LCD_START_LINE4;
    else 
        addressCounter = LCD_START_LINE1;
#else
    if ( pos < LCD_START_LINE3 )
        addressCounter = LCD_START_LINE2;
    else if ( (pos >= LCD_START_LINE2) && (pos < LCD_START_LINE4) )
        addressCounter = LCD_START_LINE3;
    else if ( (pos >= LCD_START_LINE3) && (pos < LCD_START_LINE2) )
        addressCounter = LCD_START_LINE4;
    else 
        addressCounter = LCD_START_LINE1;
#endif
#endif
    lcd_command((1<<LCD_DDRAM)+addressCounter);

}/* lcd_newline */


/*
** PUBLIC FUNCTIONS 
*/

/*************************************************************************
Send LCD controller instruction command
Input:   instruction to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_command(uint8_t cmd)
{
    lcd_waitbusy();
    lcd_write(cmd,0);
}


/*************************************************************************
Send data byte to LCD controller 
Input:   data to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_data(uint8_t data)
{
    lcd_waitbusy();
    lcd_write(data,1);
}



/*************************************************************************
Set cursor to specified position
Input:    x  horizontal position  (0: left most position)
          y  vertical position    (0: first line)
Returns:  none
*************************************************************************/
void lcd_gotoxy(uint8_t x, uint8_t y)
{
#if LCD_LINES==1
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
#endif
#if LCD_LINES==2
    if ( y==0 ) 
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
#endif
#if LCD_LINES==4
    if ( y==0 )
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else if ( y==1)
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
    else if ( y==2)
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE3+x);
    else /* y==3 */
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE4+x);
#endif

}/* lcd_gotoxy */


/*************************************************************************
*************************************************************************/
int lcd_getxy(void)
{
    return lcd_waitbusy();
}


/*************************************************************************
Clear display and set cursor to home position
*************************************************************************/
void lcd_clrscr(void)
{
    lcd_command(1<<LCD_CLR);
}


/*************************************************************************
Set cursor to home position
*************************************************************************/
void lcd_home(void)
{
    lcd_command(1<<LCD_HOME);
}


/*************************************************************************
Display character at current cursor position 
Input:    character to be displayed                                       
Returns:  none
*************************************************************************/
void lcd_putc(char c)
{
    uint8_t pos;


    pos = lcd_waitbusy();   // read busy-flag and address counter
    if (c=='\n')
    {
        lcd_newline(pos);
    }
    else
    {
#if LCD_WRAP_LINES==1
#if LCD_LINES==1
        if ( pos == LCD_START_LINE1+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE1,0);
        }
#elif LCD_LINES==2
        if ( pos == LCD_START_LINE1+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE2,0);    
        }else if ( pos == LCD_START_LINE2+LCD_DISP_LENGTH ){
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE1,0);
        }
#elif LCD_LINES==4
        if ( pos == LCD_START_LINE1+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE2,0);    
        }else if ( pos == LCD_START_LINE2+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE3,0);
        }else if ( pos == LCD_START_LINE3+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE4,0);
        }else if ( pos == LCD_START_LINE4+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE1,0);
        }
#endif
        lcd_waitbusy();
#endif
        lcd_write(c, 1);
    }

}/* lcd_putc */


/*************************************************************************
Display string without auto linefeed 
Input:    string to be displayed
Returns:  none
*************************************************************************/
void lcd_puts(const char *s)
/* print string on lcd (no auto linefeed) */
{
    register char c;

    while ( (c = *s++) ) {
        lcd_putc(c);
    }

}/* lcd_puts */


/*************************************************************************
Display string from program memory without auto linefeed 
Input:     string from program memory be be displayed                                        
Returns:   none
*************************************************************************/
void lcd_puts_p(const char *progmem_s)
/* print string from program memory on lcd (no auto linefeed) */
{
    register char c;

    while ( (c = pgm_read_byte(progmem_s++)) ) {
        lcd_putc(c);
    }

}/* lcd_puts_p */


/*************************************************************************
Initialize display and select type of cursor 
Input:    dispAttr LCD_DISP_OFF            display off
                   LCD_DISP_ON             display on, cursor off
                   LCD_DISP_ON_CURSOR      display on, cursor on
                   LCD_DISP_CURSOR_BLINK   display on, cursor on flashing
Returns:  none
*************************************************************************/
void lcd_init(uint8_t dispAttr, PCF8574A* _pcf)
{
    lcd_pcf = _pcf;
    /*
     *  Initialize LCD to 4 bit I/O mode
     */
    delay(LCD_DELAY_BOOTUP);             /* wait 16ms or more after power-on       */
    
    /* initial write to lcd is 8bit */
    lcd_d1_high();    // LCD_FUNCTION>>4;
    lcd_d0_high();    // LCD_FUNCTION_8BIT>>4;
    lcd_data_update();
    lcd_e_toggle();
    delay(LCD_DELAY_INIT);               /* delay, busy flag can't be checked here */
   
    /* repeat last command */ 
    lcd_e_toggle();      
    delay(LCD_DELAY_INIT_REP);           /* delay, busy flag can't be checked here */
    
    /* repeat last command a third time */
    lcd_e_toggle();      
    delay(LCD_DELAY_INIT_REP);           /* delay, busy flag can't be checked here */

    /* now configure for 4bit mode */
    lcd_d0_low();   // LCD_FUNCTION_4BIT_1LINE>>4
    lcd_data_update();
    lcd_e_toggle();
    delay(LCD_DELAY_INIT_4BIT);          /* some displays need this additional delay */
    
    /* from now the LCD only accepts 4 bit I/O, we can use lcd_command() */    


    lcd_command(LCD_FUNCTION_DEFAULT);      /* function set: display lines  */

    lcd_command(LCD_DISP_OFF);              /* display off                  */
    lcd_clrscr();                           /* display clear                */ 
    lcd_command(LCD_MODE_DEFAULT);          /* set entry mode               */
    lcd_command(dispAttr);                  /* display/cursor control       */

}/* lcd_init */
