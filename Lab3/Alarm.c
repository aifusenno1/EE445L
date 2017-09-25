//Paris Kaman
// Allen Pan
// Alarm.c
// determines whether the alarm needs to go off or not

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Alarm.h"
#include "ST7735.h"
#define PF2                     (*((volatile uint32_t *)0x40025010))

int alarmTime = (6 * 3600) + 10;
int numToggles = 0;

// ***************** Timer2_Init ****************
// Activate Timer2 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer2_Init(void){
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = 10000;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER2A timeout
	ST7735_SetCursor(0,0);
	ST7735_OutUDec(10);
	
  if(numToggles == 1000){
		numToggles = 0;
	}
	if(numToggles < 300){
		PF2 ^= 0x04;
	}
	numToggles++;
}


int setAlarm(int time){
	alarmTime = time;
	return 1;
}

void alarmOn(void){
	//GPIO_PORTF_IM_R |= 0x04;
	ST7735_SetCursor(0,2);
	ST7735_OutUDec(99);
	TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

void alarmOff(void){
	//GPIO_PORTF_IM_R &= ~0x04;;   // shut off interrupts
	ST7735_SetCursor(0,3);
	ST7735_OutUDec(55);
	TIMER2_CTL_R = 0x00000000;    // 10) disable timer2A
	numToggles = 0;
}

int checkForAlarm(int time){
	if(time == alarmTime){
		alarmOn();
		return 1;
	}
	return 0;
}



