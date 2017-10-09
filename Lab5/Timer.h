// Timer0A.h
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#ifndef __TIMER0AINTS_H__ // do not include more than once
#define __TIMER0AINTS_H__

// ***************** Timer0A_Init ****************
// Activate Timer0A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer0A_Init(void(*task)(void), uint32_t period);

#endif // __TIMER0AINTS_H__

void Timer2A_Init(void(*task)(void), uint32_t period);
void Timer3A_Init(void(*task)(void), uint32_t period);

/******************* Timer0A Methods ****************************/

/** Timer0A_Start() **
 * Restart the Clock (TIMER 0A)
 */
void Timer0A_Start(void);

/** Timer0A_Stop() **
 * Stop the Clock (TIMER 0A)
 */
void Timer0A_Stop(void);

/** Timer0A_Arm() **
 * Enable interrupts from Timer0A.
 */
void Timer0A_Arm(void);

/** Timer0A_Disarm() **
 * Disable interrupts from Timer0A.
 */
void Timer0A_Disarm(void);

/** Timer0A_Period() **
 * Acknowledge a Timer0A interrupt
 */
void Timer0A_Acknowledge(void);

/** Timer0A_Period() **
 * Reset the period on Timer0A
 */
void Timer0A_Period(uint32_t period);


/******************* Timer2A Methods ****************************/

/** Timer2A_Start() **
 * Restart the Clock (TIMER 2A)
 */
void Timer2A_Start(void);

/** Timer2A_Stop() **
 * Stop the Clock (TIMER 2A)
 */
void Timer2A_Stop(void);

/** Timer2A_Arm() **
 * Enable interrupts from Timer2A.
 */
void Timer2A_Arm(void);

/** Timer2A_Disarm() **
 * Disable interrupts from Timer2A.
 */
void Timer2A_Disarm(void);

/** Timer2A_Period() **
 * Acknowledge a Timer2A interrupt
 */
void Timer2A_Acknowledge(void);


/** Timer2A_Period() **
 * Reset the period on Timer2A
 */
void Timer2A_Period(uint32_t period);

/******************* Timer3A Methods ****************************/

/** Timer3A_Start() **
 * Restart the Clock (TIMER 3A)
 */
void Timer3A_Start(void);

/** Timer3A_Stop() **
 * Stop the Clock (TIMER 3A)
 */
void Timer3A_Stop(void);

/** Timer3A_Arm() **
 * Enable interrupts from Timer3A.
 */
void Timer3A_Arm(void);

/** Timer3A_Disarm() **
 * Disable interrupts from Timer3A.
 */
void Timer3A_Disarm(void);

/** Timer3A_Period() **
 * Acknowledge a Timer3A interrupt
 */
void Timer3A_Acknowledge(void);


/** Timer3A_Period() **
 * Reset the period on Timer3A
 */
void Timer3A_Period(uint32_t period);