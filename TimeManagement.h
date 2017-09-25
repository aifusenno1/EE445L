//Paris Kaman
// Allen Pan
// TimeManagement.c
// takes care of time stepping

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "SysTick.h"
#include "ST7735.h"

int updateTime(int flag, int time);

void outputTime(int time);

void getSeconds(int time, char secStr[2]);
void getMinutes(int time, char minStr[2]);
void getHours(int time, char hourStr[2]);
