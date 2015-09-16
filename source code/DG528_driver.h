
/****************************************************************************
 *
 * File Name            : tbl_drvn_fsm_test.c
 * Title                : main file for ADC161S626
 * Date                 : 03/24/10
 * Version              : 1.0
 * Target MCU           : ATmega128 16MHz
 * Target Hardware      ; 
 * Author               : zhaoqi li, ilia vardishvili
 * DESCRIPTION: This header file includes a simple external function prototype
 * for the DG528 channel selection function. Include this file to use the 
 * DG528_driver function.
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
//Mux channel select function

extern void DG528_driver(unsigned char channel);