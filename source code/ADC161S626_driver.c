/****************************************************************************
 *
 * File Name            : ADC161S626_driver.c
 * Title                : driver file for ADC161S626
 * Date                 : 03/24/10
 * Version              : 1.0
 * Target MCU           : ATmega128 16MHz
 * Target Hardware      ; 
 * Author               : zhaoqi li, ilia vardishvili
 * DESCRIPTION
 * This file contains a driver for the ADC161S626. Once called, the function 
 * will initialize the appropriate port for data transmission. Since this driver
 * will be using a software approach, it will disable the SPI function of the MCU
 * at the beginning. At the beginning of the transmission, the program will write
 * 2 dummy clock to allow the ADC to prepare the data. Then the program will write
 * 8 clocks to the device and read the MISO port to determin the data bit by bit.
 * At the end of the program, the MCU will deselect the device and return the result.
 *
 * inputs: MISO
 * outputs: SCK, SS_bar (all inputs and outputs are defiend through macros)
 *
 * Warnings             : none
 * Restrictions         : none
 * Algorithms           : none
 * References           : none
 *
 * Revision History     : Initial version 
 *
 * lab section 01, bench 04
 * Module 3: Single Channel Temperature Measurement Using NTC Sensor
 *
 **************************************************************************
 */

#include <iom128.h>
#include <avr_macros.h>

#define SS_bar PORTA_Bit0
#define SCLK PORTA_Bit1
#define MISO PINA_Bit2
#define SETUP DDRA 
#define CONF  0x03


unsigned int ADC161_conv(){
  signed int result = 0;
  
  //SETUP = CONF;          //Configure PORT for data transmission
  
  
  SS_bar = 1;           // deselect device;
  SPCR = 0x00;          //disable SPI;
  SCLK = 1;             // clock polarity = 1, clock phase = 1
  
  SS_bar = 0;           //select device
  
  SCLK = 0;
  SCLK = 1;
  SCLK = 0;
  SCLK = 1;             // 2 dummy clocks to prepare data transmission
  
  for(char i=0; i < 16; i++){
    result = result << 1;
    SCLK = 0;           // load the next bit
    SCLK = 1;           // read the data at the leading edge
    
    if(MISO == 1)
      result |= 0x01;
  }
  
  SS_bar = 1;           //deselect device
  
  return result;
}
  