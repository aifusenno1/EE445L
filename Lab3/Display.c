//Paris Kaman
// Allen Pan
// Display.c
// draws the hands on the screen

#include <stdint.h>
#include <stdlib.h>
#include "../inc/tm4c123gh6pm.h"
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
// x1,x2 are horizontal positions, columns from the left edge
//               must be less than 128
//               0 is on the left, 127 is near the right
// y1,y2 are vertical positions, rows from the top edge
//               must be less than 160
//               159 is near the wires, 0 is the side opposite the wires
//        color 16-bit color, which can be produced by ST7735_Color565() 
// Output: none
void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	int deltaX = abs(x2-x1);
	int deltaY = abs(y2-y1);
	int X1 = x1;
	int X2 = x2;
	int Y1 = y1;
	int Y2 = y2;
	// vertical line
	if (x1 == x2) {
		if (y2 > y1)
			ST7735_DrawFastVLine(x1,y1,y2-y1,color);
		else
			ST7735_DrawFastVLine(x1,y2,y1-y2,color);

	} else if (deltaX >= deltaY){
	// arbitrary line
		for (int x=x1; x<=x2; x++) {
			ST7735_DrawPixel(x, y1+ (y2-y1)*(x-x1)/(x2-x1), color);
		}
	}
	else{
		if(y2 < y1){
			int tempX = x1;
			int tempY = y1;
			X1 = X2;
			Y1 = Y2;
			X2 = tempX;
			Y2 = tempY;
		}
		for (int y = Y1; y <= Y2; y++) {
			ST7735_DrawPixel(X1+ (X2-X1)*(y-Y1)/(Y2-Y1), y, color);
		}
	}
}

void ST7735_4Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	ST7735_Line(x1, y1, x2, y2, color);
	ST7735_Line(x1, y1+1, x2, y2+1, color);
	ST7735_Line(x1+1, y1, x2+1, y2, color);
	ST7735_Line(x1+1, y1+1, x2+1, y2+1, color);
}

void eraseHands(int time){
  double hourAngle = 0;
	double minuteAngle = 0;
	
	hourAngle = -(((time / 60) / 2)-90);  // get minutes and then divide by 2 for degrees:: 720 minutes in 12 Hours / 360 degrees = 2 minutes/degree
	minuteAngle = -((((time / 60)%60) * 6)-90);
	
	double hourY = HOURLENGTH * sin(PI * hourAngle / 180);
	double hourX = HOURLENGTH * cos(PI * hourAngle / 180);
	double minuteY = MINUTELENGTH * sin(PI * minuteAngle / 180);
	double minuteX = MINUTELENGTH * cos(PI * minuteAngle / 180);
	
	if(hourX < 0){
		ST7735_4Line(CENTERX+(int)hourX, CENTERY - (int)hourY, CENTERX, CENTERY, ST7735_BLACK);
	}else{
		ST7735_4Line(CENTERX, CENTERY, CENTERX+(int)hourX, CENTERY - (int)hourY, ST7735_BLACK);
	}
	
	if(minuteX < 0){
		ST7735_4Line(CENTERX+(int)minuteX, CENTERY - (int)minuteY, CENTERX, CENTERY, ST7735_BLACK);
	}else{
		ST7735_4Line(CENTERX, CENTERY, CENTERX+(int)minuteX, CENTERY - (int)minuteY, ST7735_BLACK);
	}
	
}

void drawHands(int time){
  double hourAngle = 0;
	double minuteAngle = 0;
	
	hourAngle = -(((time / 60) / 2)-90);  // get minutes and then divide by 2 for degrees:: 720 minutes in 12 Hours / 360 degrees = 2 minutes/degree
	minuteAngle = -((((time / 60)%60) * 6)-90);
	
	double hourY = HOURLENGTH * sin(PI * hourAngle / 180);
	double hourX = HOURLENGTH * cos(PI * hourAngle / 180);
	double minuteY = MINUTELENGTH * sin(PI * minuteAngle / 180);
	double minuteX = MINUTELENGTH * cos(PI * minuteAngle / 180);
	
	if(hourX < 0){
		ST7735_4Line(CENTERX+(int)hourX, CENTERY - (int)hourY, CENTERX, CENTERY, ST7735_BLUE);
	}else{
		ST7735_4Line(CENTERX, CENTERY, CENTERX+(int)hourX, CENTERY - (int)hourY, ST7735_BLUE);
	}
	
	if(minuteX < 0){
		ST7735_4Line(CENTERX+(int)minuteX, CENTERY - (int)minuteY, CENTERX, CENTERY, ST7735_WHITE);
	}else{
		ST7735_4Line(CENTERX, CENTERY, CENTERX+(int)minuteX, CENTERY - (int)minuteY, ST7735_WHITE);
	}
	
}

