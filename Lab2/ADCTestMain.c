 // ADCTestMain.c
// Xinyuan (Allen) Pan && Paris Kaman
// Lab 2
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// September 20, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

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

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include <limits.h>
#include "ADCSWTrigger.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"

#define PF4  	 				 (*((volatile uint32_t *)0x40025040))
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
int calcJitter(void);					// calculates the jitter
void calcPMF(uint32_t pmf[128]); // calculates and fills the pmf array
void Timer0A_Init100HzInt(void);
void Timer0A_Handler(void);
void Timer2A_Init(void);
void Timer1_Init(void);
void Pause(void);
void PortF_Init(void);

static volatile uint32_t ADCvalue;
static int timeStamp[1000];
static volatile uint32_t data[1000];
static volatile uint32_t entireDistribution[5000];

int i = 0; // counter, up to 1000

int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
	Timer1_Init();
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
	Timer2A_Init();
	ST7735_InitR(INITR_REDTAB);
	PortF_Init();
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      // turn off LED
  EnableInterrupts();
	
//	for(int k = 0; k < 4; k++){
//	i = 0;
	ADC0_SAC_R = 0;	
  while(1){
    PF1 ^= 0x02;  // toggles when running in main
	//	GPIO_PORTF_DATA_R ^= 0x02; // toggles when running in main
		PF1 = (PF1*12345678)/1234567+0x02; // this line causes jitter
		
	if (i == 1000) {
			// after filling the arrays, find the jitter
			int jitter = calcJitter();
			// find the minimum and maximum data, then find the range
			ST7735_FillScreen(ST7735_BLACK);
			uint32_t pmf[128];
			calcPMF(pmf);
		
			// draw pmf
			
			ST7735_DrawFastHLine(0, 159, 127, ST7735_WHITE);
			for (int j=0;j<=127;j++)
				ST7735_DrawFastVLine(j,32+(127-pmf[j]),pmf[j],ST7735_WHITE);
				ST7735_SetCursor(0,3);
			i = 1001;   // only allow this block run one time
			ST7735_SetCursor(0,0);
			ST7735_OutUDec(ADC0_SAC_R);
			
			Pause(); // push button to continue
			break;
		}
  }
//}
}

// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 79999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}

// This function initializes Timer2A to request interrupts
// used for creating jitter
void Timer2A_Init(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x04;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER2_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER2_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER2_TAILR_R = 7989;         // start value for close to 10 kHz interrupts
  TIMER2_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER2_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x20000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}

// ***************** TIMER1_Init ****************
// Activate TIMER1 for countdown
// unit: 12.5 ns
void Timer1_Init(){
	DisableInterrupts();
	volatile uint32_t delay;
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
	delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = 0xFFFFFFFF;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
	TIMER1_ICR_R = 0x00000001; 	
	NVIC_EN0_R = 1<<21;   
  TIMER1_CTL_R |= TIMER_CTL_TAEN;    // 10) enable TIMER1A

}


// the interrupt service routine
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
  PF2 ^= 0x04;                   // profile
	if (i < 1000) {
		timeStamp[i] = TIMER1_TAR_R;
	  data[i] = ADCvalue;		
		i++;
	}
//	else {
//		TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
//		DisableInterrupts();
//	}

}


void Timer2A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
  PF2 ^= 0x04;                   // profile
	if (i < 1000) {
		timeStamp[i] = TIMER1_TAR_R;
	  data[i] = ADCvalue;		
		i++;
	}
//	else {
//		TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
//		DisableInterrupts();
//	}

}


// PF4 is input (the pushbutton)
// Make PF2 an output, enable digital I/O, ensure alt. functions off
void PortF_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x14;      // 4) disable analog function on PF2, PF4
  GPIO_PORTF_PUR_R |= 0x10;         // 5) pullup for PF4
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x14;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x14;         // 7) enable digital port
}

// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}

// click the on port pushbutton to advance
void Pause(void){
  while(PF4==0x00){ 
    DelayWait10ms(10);
  }
  while(PF4==0x10){
    DelayWait10ms(10);
  }
}


//************* calcJitter********************************************
// calculate the time jitter and return it
int calcJitter(void) {
		int timeDiff[999];  // omit the first time difference
		int maxDiff = 0;
		int minDiff = INT_MAX;
		for (int j=1;j<=999;j++) {
				timeDiff[j] = timeStamp[j] - timeStamp[j+1];
				if (timeDiff[j] > maxDiff) maxDiff = timeDiff[j];
				if (timeDiff[j] < minDiff) minDiff = timeDiff[j];
			}
		return maxDiff - minDiff;
}

//************* calcPMF********************************************
// calculates the PMF and fills out the array
// Input: pmf is the unsigned integer array of size 128
// Output: none
void calcPMF(uint32_t pmf[128]) {
		for (int j=0;j<=999;j++)
			entireDistribution[data[j]]++;
		uint32_t peak = 0;
		uint32_t peakIndex = 0;
		for (int j=0;j<=4095;j++)
			if (entireDistribution[j] > peak) {
				peak = entireDistribution[j];
				peakIndex = j;
			 }
			ST7735_SetCursor(0,1);
			ST7735_OutUDec(peakIndex);
		int xMiddle = peakIndex;
		int xStart = xMiddle - 64; // only extract the middle 128 data values 
	  if(xStart < 0){
			xStart = 0;
		} else if ( (xStart + 127) > 4095) {
			xStart = 3968;
		}
		// fill out the pmf & scale down to 0-127
		for (int j=0;j<=127;j++)
			pmf[j] = entireDistribution[xStart + j] * 128/1000;
		
}
