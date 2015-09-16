/****************************************************************************
 *
 * File Name            : fsm.h
 * Title                : header file for the FSM
 * Date                 : 03/24/10
 * Version              : 1.0
 * Target MCU           : ATmega128 16MHz
 * Target Hardware      ; 
 * Author               : zhaoqi li, ilia vardishvili
 * DESCRIPTION: This program includes the definition of typedef state, key,
 * function pointer, and a struct that forms the table. 

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
// states in FSM, new states should be added after Man.
typedef enum {Auto, Man, Limit, Alert} state;

//All possible inputs to the FSM, new inputs may be added before the eol.
typedef enum {ocw, occw, icw, iccw, pbw, alarm, eol} key;

// pointers to function in a table
typedef void (* task_fn_ptr) ();

// self defined struct to construct a table
typedef struct{
  key keyval;
  state next_state;
  task_fn_ptr tf_ptr;
} transition;