/****************************************************************************
 *
 * File Name            : fcn_tb.c
 * Title                : main file for ADC161S626
 * Date                 : 03/24/10
 * Version              : 1.0
 * Target MCU           : ATmega128 16MHz
 * Target Hardware      ; 
 * Author               : zhaoqi li, ilia vardishvili
 * DESCRIPTION: a function that provides a table look for the input ADC reading
 * then return the corresponding temperature to the caller.
 * DG528_driver function.
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
#include <stdio.h>
#define FULL_SCALE 32768
const unsigned long int table_1[] =
{
176974, 166356, 156441, 147177, 138518, 130421, 122847, 115759, 
109122, 102906, 97081, 91621, 86501, 81698, 77190, 72957, 68982,
65246, 61736, 58434, 55329, 52407, 49656, 47066, 44626, 42327,
40159, 38115, 36187, 34368, 32650
};
const unsigned int table_2[] = 
{ 
31029, 29498, 28052, 26685,
25392, 24170, 23013, 21918,
20882, 19901, 18971, 18090,
17255, 16463, 15712, 14999, 
14323, 13681, 13072, 12493, 
11942, 11419, 10922, 10450,//54
10000, 9572,  9165,  8777, 
8408,  8057,  7722,
};
const unsigned int table_3[] = 
{
7402, 7098, 6808, 6531, 6267, 6015, 5775, 5545,
5326, 5117, 4917, 4725,4543, 4368, 4201, 4041, 
3888, 3742, 3602, 3468, 3340, 3217, 3099, 2986, 
2878, 2774, 2675, 2579, 2488, 2400, 2316 
};
const unsigned int table_4[] = 
{
2235, 2157, 2083, 2011, 1942, 1876, 1813, 1752, 
1693, 1637, 1582, 1530, 1480, 1432, 1385, 1340, 
1297,1255, 1215, 1177, 1140, 1104, 1070, 1037,
1005, 973,  944,  915,  887,  861,  835,  810 
};

signed int tb_lookup (unsigned int adc_data)
{
  char i;
  signed int index = 125;
  unsigned long int var_a, var_b;
  unsigned long int result;
  result = (unsigned long int)(adc_data)* 10000/(FULL_SCALE - adc_data);
  //check first array
  if(table_1[30] < result){
    for(i = 0; i < 31; i++){
      if(result > table_1[i]){
        index = i;
        if(i != 0){
          var_a = table_1[i-1];
          var_b = table_1[i];
        }
        break;
      }
    }
    }
  else if(table_2[30] < result){
     for(i = 0; i < 31; i++){
      if(result > table_2[i]){
        index = 31 + i;
        if(i == 0){
          var_a = table_1[30];
          var_b = table_2[0];
        }else{
          var_a = table_2[i-1];
          var_b = table_2[i];
        }     
        break;
      }
     }
      }
  else if(table_3[30] < result){
     for(i = 0; i < 31; i++){
      if(result > table_3[i]){
        index = 62 + i;
        if(i == 0){
          var_a = table_2[30];
          var_b = table_3[0];
        }else{
          var_a = table_3[i-1];
          var_b = table_3[i];
        }
        break;
      }
     }
      }
  else{
     for(i = 0; i < 30; i++){
      if(result > table_4[i]){
        index = 93 + i;
         if(i == 0){
          var_a = table_3[30];
          var_b = table_4[0];
        }else{
          var_a = table_4[i-1];
          var_b = table_4[i];
        }
        break;
      }
     }
  }
       
        
 if(index == 125)
   return 940;
  else if (index == 0)
    return -300;
  else{
    index = (index - 30)*10;
    index -= ((result - var_b)*10)/(var_a - var_b);
    return index;
  }
    
}
    