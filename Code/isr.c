/****************************************************************************
 *
 * File Name            : isr.c
 * Title                : main file for ADC161S626
 * Date                 : 03/24/10
 * Version              : 1.0
 * Target MCU           : ATmega128 16MHz
 * Target Hardware      ; 
 * Author               : zhaoqi li, ilia vardishvili
 * DESCRIPTION: This file includes 2 ISR to receive inputs from the inner and 
 * outer knob of the rotary encoder. INT2 will be responsible for the inner knob
 * and INT3 will be responsible for the outer knob. Clockwise turn will provide
 * cw input for the particular knob, ccw for counter clockwise turn. pbw for 
 * pushbutton press. INIT0 is a software-generated interrupt which is triggered
 * by a positive pulse generated by PORTD 0
 * After reading the input, the ISR will call the fsm to determne the next state. 
 * Warnings             : none
 * Restrictions         : none
 * Algorithms           : none
 * References           : none
 *
 * Revision History     : Initial version 
 *
 * lab section 01, bench 04
 * Module 04: Table Driven Finite State Machine (FSM) Control of an Electric
 * Car Temperature Monitoring System (ECTMS) I
 *
 **************************************************************************
 */
#include <iom128.h>
#include <avr_macros.h>
#include <intrinsics.h>
#include "fsm.h"

void check_release();
extern state present_state;
extern void fsm(state present_state, key key_input);
key key_input;
  
union{
  unsigned char byteImage;
  struct bitImage{
    unsigned char imbit0 : 1,
      imbit1 : 1,
      imbit2 : 1,
      imbit3 : 1,
      imbit4 : 1,
      imbit5 : 1,
      imbit6 : 1,
      imbit7 : 1;
  }temp;
};


/*
 * Interrupt serivce routine
 */
#pragma vector = INT0_vect
__interrupt void ISR_INT0(void)
{
  key_input = alarm;
  fsm(present_state, alarm);
}

#pragma vector = INT1_vect
__interrupt void ISR_INT1(void)
{
  key_input = pbw;
  fsm(present_state, key_input);
  check_release();
}

#pragma vector = INT2_vect
__interrupt void ISR_INT2(void)
{
  byteImage = PIND;
  if(temp.imbit4^temp.imbit2)
    key_input = icw;
  else
    key_input = iccw;
  fsm(present_state, key_input);
}

#pragma vector = INT3_vect
__interrupt void ISR_INT3(void)
{
  byteImage = PIND;
  if(temp.imbit5^temp.imbit3)
    key_input = ocw;
  else
    key_input = occw;
  fsm(present_state, key_input);
}

/*
*  Check keypad is released and not bouncing.
*/
void check_release(void)
{
  while(!TESTBIT(PIND,INT1));     //Check that keypad key is released.
  
  __delay_cycles(50000);          //Delay (.05secs) / (1 / 1MHz) cycles.
  
  while(!TESTBIT(PIND,INT1));     //Check that key has stopped bouncing. 
  return;
}