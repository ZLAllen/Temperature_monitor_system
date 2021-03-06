/****************************************************************************
 *
 * File Name            : FSM_ext.c
 * Title                : main file for ADC161S626
 * Date                 : 03/24/10
 * Version              : 1.0
 * Target MCU           : ATmega128 16MHz
 * Target Hardware      ; 
 * Author               : zhaoqi li, ilia vardishvili
 * DESCRIPTION: This program includes a fsm definition and a simple main function
 * which contains a sequence of input to change the behavior of the fsm. The fsm
 * has two major states(Auto and Man). Inputs of ocw and occw will cause the state
 * to toggle. This program will be using a table driven fsm approach.
 * Auto: occw and ocw will change the state to Man, other inputs do nothing. 
 * All input will cause the fsm to execute the function null_fn() which simply 
 * returns to the fsm.
 *
 * Man: occw and ocw will change the state to Auto, icw will trigger the function
 * incr_channel which increment the global varable channel. The value of channel
 * will roll over at 8. iccw will trigger the function decr_channel which
 * decrements the variable. The value will be reset to 7 at 0 decrement.
 *
 * Limit: pbw to Man state will cause the system to enter the limit state, in which
 * user can adjust or read the temeprature limit for the current channel. The temperature
 * limit will be saved into the eeprom. press the pushbutton again to exit.
 * 
 * Alert: In the regulate Auot or Man state, if the measured temperature goes above
 * the limit, the system will generate a software interrupt in order to enter the 
 * Alert state, in which the system will display an alert message and keep beeping the
 * buzzer. User can slient the buzzer for 8 second if the press the pushbutton to 
 * escape into the Man state. If the temperature still exceed the limit, the system will
 * re-enter the Alert state and display the warning message. However, the buzzer will
 * not resume until the slience counter finishs counting.

 * call: incr_channel(), decr_channel(), null_fn(), start_counter, incr_limit_10,
 * incr_limit_1, decr_limit_10, decr_limit_1.
 *
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
#include "fsm.h"

//prototype of all FSM functions.
extern void incr_channel();
extern void decr_channel();
extern void incr_limit_10();
extern void decr_limit_10();
extern void incr_limit_1();
extern void decr_limit_1();
extern void start_counter();
extern void dsp_f();
extern void null_fn();

// Initial state after the reset
state present_state = Auto;




// The state transition of array consists of an array of arrays of structures.
// Each array of structure conrresponds to a particular present state value.
const transition auto_transitions[] = 
{
// INPUT        NEXT_STATE       TASK
  {occw,        Man,             null_fn},
  {ocw,         Man,             null_fn},
  {alarm,       Alert,           null_fn},
  {pbw,         Auto,            dsp_f  },
  {eol,         Auto,            null_fn}
};

const transition man_transitions[] = 
{
// INPUT        NEXT_STATe      TASK
  {occw,        Auto,           null_fn},
  {ocw,         Auto,           null_fn},
  {icw,         Man,            incr_channel},
  {iccw,        Man,            decr_channel},
  {pbw,         Limit,          null_fn},
  {alarm,       Alert,          null_fn},
  {eol,         Auto,           null_fn}
};

const transition limit_transitions[] = 
{
// INPUT        NEXT_STATe      TASK
  {occw,        Limit,          decr_limit_10},
  {ocw,         Limit,          incr_limit_10},
  {icw,         Limit,          incr_limit_1},
  {iccw,        Limit,          decr_limit_1},
  {pbw,         Man,            null_fn},
  {eol,         Limit,          null_fn}
};

const transition alert_transitions[] = 
{
// INPUT        NEXT_STATe      TASK
  {pbw,         Man,            start_counter},
  {eol,         Alert,          null_fn}
};

//outer array includes the pointer to the arrays of state transitions.
const transition *ps_transitions_ptr[4] =
{
  auto_transitions,
  man_transitions,
  limit_transitions,
  alert_transitions
};

// fsm function
void fsm(state ps, key keyval)
{
  
  int i;
  for(i=0; (ps_transitions_ptr[ps][i].keyval != keyval) 
    && (ps_transitions_ptr[ps][i].keyval != eol); i++);
  
  ps_transitions_ptr[ps][i].tf_ptr();
  
  present_state = ps_transitions_ptr[ps][i].next_state;
  
}