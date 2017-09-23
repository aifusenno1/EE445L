//Paris Kaman
// Allen Pan
// TimeManagement.c
// takes care of time stepping

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "SysTick.h"
#include "ST7735.h"

void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

void eraseHands(int time);

void drawHands(int time);

