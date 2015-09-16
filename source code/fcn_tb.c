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
const unsigned long int table[] =
{
176974, 166356, 156441, 147177, 138518, 130421, 122847, 115759, 
109122, 102906, 97081, 91621, 86501, 81698, 77190, 72957, 68982,
 65246, 61736, 58434, 55329, 52407, 49656, 47066, 44626, 42327, 
40159, 38115, 36187, 34368, 32650, 31029, 29498, 28052, 26685, 
25392, 24170, 23013, 21918, 20882, 19901, 18971, 18090, 17255, 
16463, 15712, 14999, 14323, 13681, 13072, 12493, 11942, 11419, 
10922, 10450, 10000, 9572, 9165, 8777, 8408, 8057, 7722, 7402, 
7098, 6808, 6531, 6267, 6015, 5775, 5545, 5326, 5117, 4917, 4725,
 4543, 4368, 4201, 4041, 3888, 3742, 3602, 3468, 3340, 3217, 3099, 
2986, 2878, 2774, 2675, 2579, 2488, 2400, 2316, 2235, 2157, 2083, 
2011, 1942, 1876, 1813, 1752, 1693, 1637, 1582, 1530, 1480, 1432, 
1385, 1340, 1297, 1255, 1215, 1177, 1140, 1104, 1070, 1037, 1005, 
973, 944, 915, 887, 861, 835, 810
};

signed char tb_lookup (unsigned long int adc_data);

void main()
{
  signed char data = tb_lookup(0);
  printf("%d", data);
}
signed char tb_lookup (unsigned long int adc_data)
{
  char i, index = 0;
  unsigned long int result;
  result = (unsigned long int)(adc_data)* 10000/(FULL_SCALE - adc_data);
  for(i = 0; i < 125; i++)
  {
    if(result >= table[i])
    {
      index = i;
      break;
    }
  }
  if(index == 0)
    return 94;
  else
    return index - 30;
}
    