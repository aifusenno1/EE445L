// PeriodicTimer0AInts.c
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

// oscilloscope or LED connected to PF3-1 for period measurement
// When using the color wheel, the blue LED on PF2 is on for four
// consecutive interrupts then off for four consecutive interrupts.
// Blue is off for: dark, red, yellow, green
// Blue is on for: light blue, blue, purple, white
// Therefore, the frequency of the pulse measured on PF2 is 1/8 of
// the frequency of the Timer0A interrupts.

#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include "PLL.h"
#include "Timer0A.h"
#include "MAX5353.h"
#include "Switch.h"
#include "DAC.h"
#include <string.h>

#define MAX_SONG_NOTES 1000
#define TABLE_SIZE 32

struct note{
	int tempo;								//given in beats per minute to have a starting tempo for the song -- may get changed by buttons or something?
	//int numNotes;
	double noteLength;	//double so it can be 1 for full note or .5 for half note etc
	char notePitch[3];	//2 chars in each note for things like C 3 or Ab5 etc.
};

static const unsigned short Horn[TABLE_SIZE] = { 
	  1063,1082,1119,1275,1678,1748,1275,755,661,661,703,
	  731,769,845,1039,1134,1209,1332,1465,1545,1427,1588,
	  1370,1086,708,519,448,490,566,684,802,992
	}; 

static struct note song[MAX_SONG_NOTES];
	
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))


//BASE FREQUENCY
#define B	 	494
#define Bb	466
#define A		440
#define Ab	415
#define G		392
#define Gb	370
#define F		349
#define E		330
#define Eb	311
#define D		294
#define Db	277
#define C		262


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

int dacIndex = 0;

void UserTask(void){
  DAC_Out(Horn[dacIndex]);
	dacIndex = (dacIndex + 1) % TABLE_SIZE;
}

int getIntValue(int freq){
	return 80000000 / (freq * TABLE_SIZE);		//number of clocks in a second/frequency to get number of waves in a second and then divided by 32 because there are 32 points in the wave.
}

int getFrequency(char note[3]){
	double base = 0;
	char name[2] = {"  "};
	name[0] = note[0];
	name[1] = note[1];
	int octave = note[2] - '0';
	
	if(strcmp(name, "B ") == 0){
		base = B;
	}
	else if(strcmp(name, "Bb") == 0){
		base = Bb;
	}
	else if(strcmp(name, "A ") == 0){
		base = A;
	}
	else if(strcmp(name, "Ab") == 0){
		base = Ab;
	}
	else if(strcmp(name, "G ") == 0){
		base = G;
	}
	else if(strcmp(name, "Gb") == 0){
		base = Gb;
	}
	else if(strcmp(name, "F ") == 0){
		base = F;
	}
	else if(strcmp(name, "E ") == 0){
		base = E;
	}
	else if(strcmp(name, "Eb") == 0){
		base = Eb;
	}
	else if(strcmp(name, "D ") == 0){
		base = D;
	}
	else if(strcmp(name, "Db") == 0){
		base = Db;
	}
	else if(strcmp(name, "C ") == 0){
		base = C;
	}
	
	int shifts = octave - 4;		// 4 is the base octave all the frequencies are listed as
	if(shifts == 0){
		return base;
	}
	else if(shifts > 0){
		return ((int)base << shifts);
	}
	else{
		return ((int)base >> (-shifts));
	}
}


// if desired interrupt frequency is f, Timer0A_Init parameter is busfrequency/f

//debug code
int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 50 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
  while((SYSCTL_PRGPIO_R&0x0020) == 0){};// ready?
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x0E;     // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x0E;        // enable digital I/O on PF3-1
                                   // configure PF3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
  LEDS = 0;                        // turn all LEDs off
	char test[3] = {"A 2"};
	int x = getFrequency(test);
	int y = getIntValue(x);
	DAC_Init(1063);
	EdgeInterrupt_Init();
	PortF_Init();
	 
		//TODO: FIX THIS STUFF TO LAB 5
//  Timer0A_Init(&UserTask, F20KHZ);     // initialize timer0A (20,000 Hz)
  Timer0A_Init(&UserTask, y);  // initialize timer0A (16 Hz)
  EnableInterrupts();

  while(1){
    WaitForInterrupt();
		
  }
}
