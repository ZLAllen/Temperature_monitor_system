/****************************************************************************
 *
 * File Name            : tbl_drvn_fsm_test.c
 * Title                : main file for ADC161S626
 * Date                 : 03/24/10
 * Version              : 1.0
 * Target MCU           : ATmega128 16MHz
 * Target Hardware      ; 
 * Author               : zhaoqi li, ilia vardishvili
 * DESCRIPTION: This program defines a driver function to select the input channel
 * for the DG528 mux. The function is passed the valuable indicating the channel
 * selection. The function will then shift the value to the correct position in 
 * order to allow the MCU to output a valid selection to the mux. Finally the 
 * function will delay a period of time to allow the new voltage to settle down.
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

void DG528_driver(unsigned char channel){
  channel = channel << 3;       //shift the value to the approriate position
  channel &= 0x38;             // mask unused bits
   
  PORTA = channel;             //output selection to mux;
  __delay_cycles(1000);         //delay for voltage settle downo
  
  return;
}