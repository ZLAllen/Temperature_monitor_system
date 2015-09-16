/****************************************************************************
 *
 * File Name            : tbl_drvn_fsm_test.c
 * Title                : main file for ADC161S626
 * Date                 : 03/24/10
 * Version              : 1.0
 * Target MCU           : ATmega128 16MHz
 * Target Hardware      ; 
 * Author               : zhaoqi li, ilia vardishvili
 * DESCRIPTION: This program includes a main function which is continously 
 * performing background tasking. In the infinite function loop, in each second,
 * the program will take voltage readings from each channels 10 times and store
 * the volue into an array. Then the program will determine the state and select
 * the way to display the voltage result. In the auto mode, the voltage for each
 * channel will be refresh each second, but the channel value for reamin for 3 
 * second and then display the voltage of the next channel for another 3 second.
 * In the man mode, the voltage will stay at 1 channel until a input cause the 
 * program to display the readings from a different channel. This program will
 * be using a table_driven fsm and interrupt inputs to regulates all state changes.

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
#include <avr_macros.h> //useful macros
#include <intrinsics.h> //useful intrinsic functions
#include <stdio.h>

#include "LCD_Driver.h"         //LCD
#include "ADC161S626_driver.h"  //ADC
#include "DG528_driver.h"       //MUX
#include "alert_fsm.h"            //FSM
  
void get_volt();

//present state
extern state present_state;

// conversion result array
unsigned int channel_voltage[8];
//limit array
unsigned int voltage_limit[8];

//channel to conv and channel to display
unsigned char channel_conv = 0;
unsigned char channel_display = 0;      //default channel 0;
//temp varaibles
char times, current_time = 0;
unsigned int voltage, limit;
// variables to hold the value of voltage
unsigned char unit = 0, tenths = 0, hundredths = 0, thousandths = 0;


void main(void){
  __disable_interrupt();
  //Anolog mux channel select output
  DDRA = 0x3B;
  
  DDRB = 0xFF;          //initialize LCD
  SETBIT(PORTB, 0);     //deselect slave
  init_lcd_dog();
  clear_dsp();
  
  DDRD = 0x01;                  //  initialize PortD for interrupt
  PORTD = 0x02;
  
  EIMSK = 0x0F;                 // enable individual interrupt
  EICRA = 0x53;
  
  __enable_interrupt();
  
  //infinite loop -- approximately 1 second for each loop
  while(1){
    if(present_state == Limit){
      get_volt();
      clear_dsp();
      printf("limit: %d.", unit);
      printf("%d%d%d V", tenths, hundredths, thousandths); 
      update_lcd_dog();
    }else{
      for(channel_conv = 0; channel_conv < 8; channel_conv++){
        DG528_driver(channel_conv);       //select channel
        for(times = 0; times < 10; times++){  //conv for each channel 10 times
          channel_voltage[channel_conv] = ADC161_conv();
        }
      }
      
      if(present_state == Auto){
        if(current_time == 3){
          current_time = 0;     //reset timer;
          channel_display = (channel_display + 1)%8;
        }else
          current_time ++ ;
      }else
        current_time = 0;      //always reset timer on man state
       
      get_volt();
      clear_dsp();
      printf("chnl %d: %d", channel_display, unit); 
      printf(".%d%d%d V ", tenths, hundredths, thousandths);
      update_lcd_dog();
      if(((voltage_limit[channel_display] != 0) && 
          (voltage_limit[channel_display]< limit)) || present_state == Alert)
      {
            printf("LIMIT EXCEEDED!!");
            update_lcd_dog();
            SETBIT(PORTD, 0);
            CLEARBIT(PORTD, 0);
      }
      __delay_cycles(7500000);    //delay_for_1 second
    }
  }
}

void get_volt(){
    if(present_state == Limit)
      voltage = voltage_limit[channel_display];
    else
      limit = voltage = (unsigned int)((unsigned long int)(channel_voltage[channel_display])*4096/32768);
       unit = (int)(voltage/1000);
       voltage -= unit * 1000;
  
       tenths = (int)(voltage/100);
       voltage -= tenths * 100;
  
       hundredths = (int)(voltage/10);
       voltage -= hundredths * 10;
  
       thousandths = voltage;
}