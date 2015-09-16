/****************************************************************************
 *
 * File Name            : main.c
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
 * the way to display the voltage result. In the auto mode, the temperature for each
 * channel will be refresh each second, but the channel value for reamin for 3 
 * second and then display the temperature of the next channel for another 3 second.
 * In the man mode, the temperature will stay at 1 channel until a input cause the 
 * program to display the readings from a different channel. Pressing the pushbutton
 * in the Man state will cause the system to enter the Limit state, in which the 
 * user will be able to access and change the temperature limit for the current 
 * channel. The temperature limit is to be stored in an array of character in the 
 * eeprom. In the regulate Auot or Man state, if the measured temperature goes above
 * the limit, the system will generate a software interrupt in order to enter the 
 * Alert state, in which the system will display an alert message and keep beeping the
 * buzzer. User can slient the buzzer for 8 second if the press the pushbutton to 
 * escape into the Man state. If the temperature still exceed the limit, the system will
 * re-enter the Alert state and display the warning message. However, the buzzer will
 * not resume until the slience counter finishs counting. 



 * This program will be using a table_driven fsm and interrupt inputs to regulates
 * all state changes.

 * Warnings             : none
 * Restrictions         : none
 * Algorithms           : none
 * References           : none
 *
 * Revision History     : Initial version 
 *
 * lab section 01, bench 04
 * Module 05: Table Driven Finite State Machine (FSM) Control of an Electric
 * Car Temperature Monitoring System (ECTMS) II
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
#include "fsm.h"                //FSM
 
//function template
void limit_init();
extern signed int tb_lookup (unsigned int adc_data);
extern int putchar(int c);

//present state
extern state present_state;
extern char index;
// conversion result array
unsigned int channel_voltage[8][8];
//limit array 
__eeprom signed char limit[8]; 
//temporary ADC and temperature storage
signed char temp_limit;
signed int temp_int;
static unsigned long int adc_sum;
//channel to conv and channel to display
unsigned char channel_conv = 0;
unsigned char channel_display = 0;      //default channel 0;
//temporary varaibles
static unsigned char i = 0, current_time = 0;
// variables to hold the value of voltage
signed int temperature = 0;
unsigned char temp_tenth = 0;
char unit_temp = 'C';
char flag = 0;
//loop counter to monitor clock
unsigned char counter;
//arrays to store strings
const char *a[] = {"Ambit.O", "Motor  ", "H.Contr", "D.Convt", "B.Box#1", "B.Box#2", 
            "Ambit.I", "Chnl #7"};


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
  
  SETBIT(DDRD, 7);    //enable buzzer output
  SETBIT(PORTD, 7);   //disable buzzer
  
  EIMSK = 0x0F;                 // enable individual interrupt
  EICRA = 0x53;
  
  __enable_interrupt();
  
  //infinite loop -- approximately 1 second for each loop
  while(1){
    if(present_state == Limit){
      __EEGET(temp_limit,channel_display);
      if(flag & (1 << 0)){
        temp_int = ((signed int)(temp_limit)*9)/5 + 32;
        unit_temp = 'F';
      }else{
        temp_int = (signed int)temp_limit;
        unit_temp = 'C';
      }
      
      clear_dsp();
      printf("Limit: %d ",temp_int);
      putchar(unit_temp);
      update_lcd_dog();
      
    }else{
      for(channel_conv = 0; channel_conv < 8; channel_conv++){
        DG528_driver(channel_conv);       //select channel
        for(i = 0; i < 10; i++){  //conv for each channel 10 times
          channel_voltage[channel_conv][i%8] = ADC161_conv();
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
      
      if(present_state == Alert && counter == 0)
        CLEARBIT(PORTD, 7);
      else{
        SETBIT(PORTD, 7);
        if(counter > 0)
          counter --;
      }
      //summing ADC reading for a particular channel
      adc_sum = 0;
      for(i = 0; i<8; i++)
        adc_sum += (unsigned long int)channel_voltage[channel_display][i];
      
      //average the sum and look up the temperature in the table  
      temperature = tb_lookup(((unsigned int)(adc_sum >> 3)));
      
      if(flag & (1 << 0)){
        temperature = (temperature* 9)/5 + 320;
        unit_temp = 'F';
      }else
        unit_temp = 'C';
      if(temperature < 0)
         temp_tenth = (0 - temperature)%10;
      else
         temp_tenth = temperature%10;
      temperature = temperature/10;
      clear_dsp();
      printf("%s: %d.%d ", a[channel_display], temperature, temp_tenth);
      putchar(unit_temp);
      update_lcd_dog();
      __EEGET(temp_limit, channel_display);
       if(flag & (1 << 0))
          temp_int = ((signed int)(temp_limit)*9)/5 + 32;
       else
         temp_int = (signed int)temp_limit;
       
       if(((temp_int != 0) && 
          (temp_int <= temperature)) || present_state == Alert)
      {
        index = 16;
        printf("LIMIT EXCEEDED!!");
        update_lcd_dog();
        SETBIT(PORTD, 0);
        CLEARBIT(PORTD, 0);
      }
      __delay_cycles(8000000);    //delay_for_1 second
    }
  }
}
              