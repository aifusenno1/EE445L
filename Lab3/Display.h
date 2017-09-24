//Paris Kaman
// Allen Pan
// Display.h
// draws the hands on the screen

#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "SysTick.h"
#include "ST7735.h"
#include <math.h>

#define HOURLENGTH 20
#define MINUTELENGTH 30
#define CENTERX 64
#define CENTERY 102
#define PI 3.14159

//************* ST7735_Line********************************************
//  Draws one line on the ST7735 color LCD
//  Inputs: (x1,y1) is the start point
//          (x2,y2) is the end point
// the relative positions of (x1,y1) and (x2,y2) need not to be concerned
// x1,x2 are horizontal positions, columns from the left edge
//               must be less than 128
//               0 is on the left, 127 is near the right
// y1,y2 are vertical positions, rows from the top edge
//               must be less than 160
//               159 is near the wires, 0 is the side opposite the wires
//        color 16-bit color, which can be produced by ST7735_Color565() 
// Output: none
void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

/* draws the 4 surrounding pixels of a given point, making the line thicker */
void ST7735_4Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

void eraseHands(int time);

void drawHands(int time);

