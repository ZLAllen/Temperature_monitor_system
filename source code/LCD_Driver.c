/****************************************************************************
 *
 * File Name            : lcd_dog_driver.h
 * Title                : Header file for LCD module
 * Date                 : 02/07/10
 * Version              : 1.0
 * Target MCU           : ATmega128 16MHz
 * Target Hardware      ; 
 * Author               : zhaoqi li, ilia vardishvili
 * DESCRIPTION
 * This file includes all the driver functions needed by the main function
 * to output messages to the lcd screen. The main functions will also need to
 * include <intrinsic.h> <avr_macro.h> in order to use all the functions in this 
 * header. A multi-module version of this header function will be used next
 * time.
 *
 * public function: init_lcd_dog, update_lcd_dog, putchar, clear_dsp
 * local function: lcd_spi_transmit_CMD, lcd_spi_tranmit_DATA
 *
 * Warnings             : none
 * Restrictions         : none
 * Algorithms           : none
 * References           : none
 *
 * Revision History     : Initial version 
 *
 * lab section 01, bench 04
 * Module 2: user interface
 *
 **************************************************************************
 */
#include <iom128.h>
#include <intrinsics.h>     //Intrinsic functions.
#include <avr_macros.h>     //Useful macros.

/*
;
;*********************************************************************
;
;   *** Port B Interface Definitions:
;
;
;  Port B alt names   SCK   MISO  MOSI  /SS    RS    -     -     -
;  LCD Mod Signal     D6     -     D7   /CSB   -     -     -     -
;  LCD Mod Pin #      29     -     28    38    -     -     -     -
;
;      Notes:  RS ==>  0 = command regs, 1 = data regs
;             /SS = active low SPI select signal
;
;******************
*/
#define	SCK     1
#define	MISO    3
#define	MOSI    2
#define	SS_bar  0
#define	RS      4
#define	BLC     5	

//Current Mode : 2 lines
   #define Addr_2 0xC0     //0x90 for 3 line display, 0xC0 for 2 lines
   #define Fun_set 0x39    // 0x39 for 2/3 lines, 0x35 for 1 line
   #define Bias_set 0x1C       //0x1C for 1/2 line, 0x1D for 3 lines

#define MAX_CHAR 16    // maximum character in a line

/*
 * Puts display buffers in near initialize to zero segment.
 * See chapter on segments in IAR reference guide.
 */
char dsp_buff_1[16];
char dsp_buff_2[16];
char dsp_buff_3[16];

//******* SPI transmission subroutines*********************************
/*
 ********************************
 NAME:       lcd_spi_transmit_CMD
 ASSUMES:    temp = byte for LCD.
             SPI port is configured.
 RETURNS:    nothing
 MODIFIES:   temp, PortB, SPCR
 CALLED BY:  init_dsp, update
 DESCRITION: outputs a byte passed in r16 via SPI port. Waits for data
             to be written by spi port before continuing.
 *********************************************************************
*/

void lcd_spi_transmit_CMD(unsigned char temp){
   CLEARBIT(PORTB,RS);
   CLEARBIT(PORTB,SS_bar);
   SPDR = temp;
   while(!TESTBIT(SPSR,SPIF));
   SPSR &= 0x7F;
   SETBIT(PORTB, SS_bar);
 }

/*
;*********************************
 NAME:       lcd_spi_transmit_DATA
 ASSUMES:    temp = byte to transmit to LCD.
             SPI port is configured.
 RETURNS:    nothing
 MODIFIES:   temp, SPCR
 CALLED BY:  init_dsp, update
 DESCRITION: outputs a byte passed in r16 via SPI port. Waits for
             data to be written by spi port before continuing.
 *****************************************************************
 */
  void lcd_spi_transmit_DATA(unsigned char data){
    PORTB = (1 << RS);
    CLEARBIT(PORTB, SS_bar);
    CLEARBIT(SPSR, SPIF);
    SPDR = data;
    while(!TESTBIT(SPSR,SPIF));
    SPSR &= 0x7F;
    SETBIT(PORTB, SS_bar);
  }

/*
 ************************
 NAME:       init_lcd_dog
 ASSUMES:    nothing
 RETURNS:    nothing
 MODIFIES:   temp, SPSR, SPCR
 CALLED BY:  main application
 DESCRITION: inits DOG module LCD display for SPI (serial) operation.
 NOTE:  Can be used as is with MCU clock speeds of 4MHz or less.
 ********************************************************************
  public __version_1 void init_dsp(void)
*/


void init_lcd_dog(void){
    unsigned char temp;
  
    SPCR = 0x5F;                            // init SPI port for DOG LCD.
    SPSR &= ~(1 << SPIF);
  
    __delay_cycles(640000);                 //startup delay.

//func_set1  
    temp = Fun_set;                           //send fuction set #1
    lcd_spi_transmit_CMD(temp);
    __delay_cycles(480);                   //delay for command to be processed
 
//func_set2
   temp = Fun_set;                         //send fuction set #2
  lcd_spi_transmit_CMD(temp);
  __delay_cycles(480);                  //delay for command to be processed

//bias_set  
   temp = Bias_set;                         //set bias value
  lcd_spi_transmit_CMD(temp);
  __delay_cycles(480);         

//power_ctrl  
   temp = 0x50;                         //0x50 nominal (delicate adjustment)
  lcd_spi_transmit_CMD(temp);
  __delay_cycles(480);         

//follower_ctrl 
   temp = 0x6C;                         //follower mode on... 
  lcd_spi_transmit_CMD(temp);
  __delay_cycles(640000);  
  
//contrast_set  
   temp = 0x74;                         //7C was too bright 
  lcd_spi_transmit_CMD(temp);
  __delay_cycles(480); 
 
//display_on 
   temp = 0x0C;                         //display on, cursor off, blink off 
  lcd_spi_transmit_CMD(temp);
  __delay_cycles(480);  

//clr_display  
   temp = 0x01;                         //clear display, cursor home
  lcd_spi_transmit_CMD(temp);           
  __delay_cycles(480);  

//entry_mode  
   temp = 0x06;                         //clear display, cursor home
  lcd_spi_transmit_CMD(temp);           
  __delay_cycles(480);  
 
}

/*
 **************************
 NAME:       update_lcd_dog
 ASSUMES:    display buffers loaded with display data
 RETURNS:    nothing
 MODIFIES:   data,SREG, SPSR, SPCR
 
 DESCRITION: Updates the LCD display lines 1, 2, and 3, using the
   contents of dsp_buff_1, dsp_buff_2, and dsp_buff_3, respectively.
 *******************************************************************
 * public __version_1 void update_dsp_dog (void)
 */

void update_lcd_dog(void){
  unsigned char temp, i, data;
  
  SPCR = 0x5F;                            // init SPI port for DOG LCD.
  SPSR &= ~(1 << SPIF);

// snd_ddram_addr_1  
  temp = 0x80;                           //init DDRAM addr-ctr 
  lcd_spi_transmit_CMD(temp);
  __delay_cycles(480); 
  
// snd snd_buff_1 
  for(i = 0; i < MAX_CHAR; i++){
    data = dsp_buff_1[i];
    lcd_spi_transmit_DATA(data);
    __delay_cycles(480); 
  }

// snd_ddram_addr_2  
  temp = Addr_2;                           //init DDRAM addr-ctr 
  lcd_spi_transmit_CMD(temp);
  __delay_cycles(480); 
  
// snd snd_buff_2 
  for(i = 0; i < MAX_CHAR; i++){
    data = dsp_buff_2[i];
    lcd_spi_transmit_DATA(data);
    __delay_cycles(480); 
  }
  
// snd_ddram_addr_3  
  temp = 0xD0;                           //init DDRAM addr-ctr 
  lcd_spi_transmit_CMD(temp);
  __delay_cycles(480); 
  
// snd snd_buff_3 
  for(i = 0; i < MAX_CHAR; i++){
    data = dsp_buff_3[i];
    lcd_spi_transmit_DATA(data);
    __delay_cycles(480); 
  }  
}
//******* End Of SPI transmission *********************************//