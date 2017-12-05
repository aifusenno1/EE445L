//Paris Kaman
// Allen Pan
// Alarm.c
// determines whether the alarm needs to go off or not

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Alarm.h"
#include "ST7735.h"
#include "PLL.h"
#define PF2                     (*((volatile uint32_t *)0x40025010))

int alarmTime = (6 * 3600) + 10;
int alarm = 0;
int inAlarm = 0;

void PortD_Init(void){
   SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
   while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
   // 2) no need to unlock PD3-0
   GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PD3-0
   GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO
   GPIO_PORTD_DIR_R |= 0x0F;         // 5) make PD0 out
   GPIO_PORTD_AFSEL_R &= ~0x0F;      // 6) regular port function
   GPIO_PORTD_DEN_R |= 0x0F;         // 7) enable digital I/O on PD0
}

int checkForAlarm(int time){
	if(time == alarmTime){
		return 1;
	}
	return 0;
}



