// Timer.h
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

void Timer1_Init(void(*task)(void), uint32_t period);
void Timer2_Init(void(*task)(void), uint32_t period);
void Timer3_Init(void(*task)(void), uint32_t period);


void inline Timer1_Start(void);
void inline Timer1_Stop(void);
void inline Timer1_Arm(void);
void inline Timer1_Disarm(void);
void inline Timer1_Acknowledge(void);
void inline Timer1_Period(uint32_t period);
/******************* Timer2 Methods ****************************/
void inline Timer2_Start(void);
void inline Timer2_Stop(void);
void inline Timer2_Arm(void);
void inline Timer2_Disarm(void);
void inline Timer2_Acknowledge(void);
void inline Timer2_Period(uint32_t period);

/******************* Timer3 Methods ****************************/
void inline Timer3_Start(void);
void inline Timer3_Stop(void);
void inline Timer3_Arm(void);
void inline Timer3_Disarm(void);
void inline Timer3_Acknowledge(void);
void inline Timer3_Period(uint32_t period);
