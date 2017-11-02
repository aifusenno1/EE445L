// Timer.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 9.8

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2, Program 7.5, example 7.6

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
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

static void (*Task2)(void);   // user function
static void (*Task3)(void);   // user function


/** Timer2A_Init() **
 * Activate TIMER2A to countdown for period seconds
 * Initializes Timer2A for period interrupts
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
 */
void Timer2A_Init(void(*task)(void), uint32_t period){
	long sr;
  sr = StartCritical(); 
	volatile uint32_t delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate TIMER2
	delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
	Task2 = task;          // user function
	TIMER2_CTL_R = ~TIMER_CTL_TAEN;    // 1) disable TIMER2A during setup
	TIMER2_CFG_R = 0;    // 2) configure for 32-bit mode
	TIMER2_TAPR_R = 0;            // 5) bus clock resolution
	TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD;   // 3) configure for periodic mode, default down-count settings
	TIMER2_TAILR_R = period-1;    	// 4) reload value
	TIMER2_IMR_R = TIMER_IMR_TATOIM;// arm timeout interrupt
	TIMER2_ICR_R = TIMER_ICR_TATOCINT;    // 6) clear TIMER2A timeout flag

	// Timer 2 interupts
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF); // clear priority
	NVIC_PRI5_R = (NVIC_PRI5_R | 0x80000000); // priority 4
  NVIC_EN0_R = 1 << 23;              // enable interrupt 23 in NVIC
	TIMER2_CTL_R = 0x00000001;    // 10) enable
  EndCritical(sr);
}

/** Timer3A_Init() **
 * Activate TIMER3A to countdown for period seconds
 * Initializes Timer2A for period interrupts
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
 */
void Timer3A_Init(void(*task)(void), uint32_t period){
	long sr;
  sr = StartCritical(); 
	volatile uint32_t delay;
SYSCTL_RCGCTIMER_R |= 0x08;   // 0) activate Timer3
	delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
	Task3 = task;          // user function
TIMER3_CTL_R = ~TIMER_CTL_TAEN;    // 1) disable TIMER3A during setup
	TIMER3_CFG_R = 0;    // 2) configure for 32-bit mode
	TIMER3_TAPR_R = 0;            // 5) bus clock resolution
TIMER3_TAMR_R = TIMER_TAMR_TAMR_PERIOD;   // 3) configure for periodic mode, default down-count settings
	TIMER3_TAILR_R = period - 1;    	// 4) reload value
	TIMER3_IMR_R = TIMER_IMR_TATOIM;// arm timeout interrupt
	TIMER3_ICR_R = TIMER_ICR_TATOCINT;    // 6) clear TIMER3A timeout flag
	// Timer 3 interupts
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF); // clear priority
NVIC_PRI8_R = (NVIC_PRI8_R | 0x80000000); // priority 4
  NVIC_EN1_R = 1 << 3;              // enable interrupt 35 in NVIC
	TIMER3_CTL_R = 0x00000001;    // 10) enable
  EndCritical(sr);
}


void Timer2A_Handler(void){
  Timer2A_Acknowledge();
  (*Task2)();                // execute user task
}

void Timer3A_Handler(void){
   Timer3A_Acknowledge();
  (*Task3)();                // execute user task
}


/******************* Timer2A Methods ****************************/

/** Timer2A_Start() **
 * Restart the Clock (TIMER 2A)
 */
void Timer2A_Start(){
	TIMER2_CTL_R |= TIMER_CTL_TAEN;
}

/** Timer2A_Stop() **
 * Stop the Clock (TIMER 2A)
 */
void Timer2A_Stop(){
	TIMER2_CTL_R &= ~TIMER_CTL_TAEN;
}

/** Timer2A_Arm() **
 * Enable interrupts from Timer2A.
 */
void Timer2A_Arm(){
	NVIC_EN0_R = 1 << 23;
}

/** Timer2A_Disarm() **
 * Disable interrupts from Timer2A.
 */
void Timer2A_Disarm(){
	NVIC_DIS0_R = 1 << 23;
}

/** Timer2A_Acknowledge() **
 * Acknowledge a Timer2A interrupt
 */
void Timer2A_Acknowledge(){
	TIMER2_ICR_R = TIMER_ICR_TATOCINT; 
}

/** Timer2A_Period() **
 * Reset the period on Timer2A
 */
void Timer2A_Period(uint32_t period){
	TIMER2_TAILR_R = period - 1; 
}

/******************* Timer3A Methods ****************************/

/** Timer3A_Start() **
 * Restart the Clock (TIMER 2A)
 */
void Timer3A_Start(){
	TIMER3_CTL_R |= TIMER_CTL_TAEN;
}

/** Timer3A_Stop() **
 * Stop the Clock (TIMER 2A)
 */
void Timer3A_Stop(){
	TIMER3_CTL_R &= ~TIMER_CTL_TAEN;
}

/** Timer3A_Arm() **
 * Enable interrupts from Timer3A.
 */
void Timer3A_Arm(){
	NVIC_EN1_R = 1 << 3;
}

/** Timer3A_Disarm() **
 * Disable interrupts from Timer3A.
 */
void Timer3A_Disarm(){
	NVIC_DIS1_R = 1 << 3;
}

/** Timer3A_Acknowledge() **
 * Acknowledge a Timer3A interrupt
 */
void Timer3A_Acknowledge(){
	TIMER3_ICR_R = TIMER_ICR_TATOCINT; 
}

/** Timer3A_Period() **
 * Reset the period on Timer3A
 */
void Timer3A_Period(uint32_t period){
	TIMER3_TAILR_R = period - 1; 
}
