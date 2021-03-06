/****************************************************************************
 *
 * File Name            : fsm_function.c
 * Title                : Header file for LCD module
 * Date                 : 02/07/10
 * Version              : 1.0
 * Target MCU           : ATmega128 16MHz
 * Target Hardware      ; 
 * Author               : zhaoqi li, ilia vardishvili
 * DESCRIPTION
 * This file includes 3 fsm functions: incr_channel, decr_channel, null_fn
 * All functions take no parameter and return nothing. incr_channel will increment
 * the global valuable channel_display by 1 and roll over at 8. decr_channel will
 * decrement the channel_display by 1 and reset to 7 after 0. null_fn will simply
 * return and do nothing. incr_limit_10 and incr_limit_1 increases the limit, 
 * decr_limit_10 and decr_limit_1 decreases the limit.
 *
 * Warnings             : none
 * Restrictions         : none
 * Algorithms           : none
 * References           : none
 *
 * Revision History     : Initial version 
 *
 * lab section 01, bench 04
 * Module 4: table_driven fsm
 *
 **************************************************************************
*/
#include <iom128.h>
#include <avr_macros.h> //useful macros
#include <intrinsics.h> //useful intrinsic functions

extern unsigned char counter;
extern char flag;
extern unsigned char channel_display;
extern __eeprom signed char limit[8];
signed char temp;

void incr_channel()
{
  channel_display = (channel_display + 1)%8;
}

void decr_channel()
{
  channel_display <= 0? channel_display = 7: --channel_display;
}

void null_fn()
{
  return;
}

void incr_limit_10(){
  __EEGET(temp, channel_display);
  if(temp + 10 > 94)
   __EEPUT(channel_display, 0);
  else
    __EEPUT(channel_display, temp+10);
}

void decr_limit_10(){
  __EEGET(temp, channel_display);
  if(temp < 10)
    __EEPUT(channel_display, 94);
  else
   __EEPUT(channel_display, temp-10);
}

void incr_limit_1(){
  __EEGET(temp, channel_display);
  if(temp == 94)
    __EEPUT(channel_display, 0);
  else
    __EEPUT(channel_display, temp+1);
}

void decr_limit_1(){
   __EEGET(temp, channel_display);
  if(temp == 0)
   __EEPUT(channel_display, 94);
  else
    __EEPUT(channel_display, temp-1); ;
}

void start_counter(){
  counter = 10;
  return;
}

void dsp_f(){
  flag ^= 1 << 0;
  return;
}