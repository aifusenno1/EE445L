//Paris Kaman
// Allen Pan
// Alarm.c
// determines whether the alarm needs to go off or not

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Alarm.h"
#include "ST7735.h"
#define PF2                     (*((volatile uint32_t *)0x40025010))

int alarmTime = (6 * 3600) + 10;
int alarm = 0;
int inAlarm = 0;

int checkForAlarm(int time){
	if(time == alarmTime){
		return 1;
	}
	return 0;
}



