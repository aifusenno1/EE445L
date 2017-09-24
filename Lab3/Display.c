//Paris Kaman
// Allen Pan
// Display.c
// draws the hands on the screen

#include "Display.h"


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
void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	int deltaX = abs(x2-x1);
	int deltaY = abs(y2-y1);

	// vertical line
	if (x1 == x2) {
		if (y2 > y1)
			ST7735_DrawFastVLine(x1,y1,y2-y1,color);
		else
			ST7735_DrawFastVLine(x1,y2,y1-y2,color);
	
	}
		// arbitrary line
	else if (deltaX >= deltaY){  // x changes faster than y, so each x will be different, but some y's may be the same
		if (x1 <= x2)
			for (int x=x1; x<=x2; x++) {
				ST7735_DrawPixel(x, y1+ (y2-y1)*(x-x1)/(x2-x1), color);
			}
		else
			for (int x=x2; x<=x1; x++) {
				ST7735_DrawPixel(x, y1+ (y2-y1)*(x-x1)/(x2-x1), color);
		}
	}
	else{  // deltaY > deltaX   y changes faster than x, so we want to draw each value of y instead of x.
		if(y1 <= y2)
			for (int y = y1; y <= y2; y++) {
				ST7735_DrawPixel(x1+ (x2-x1)*(y-y1)/(y2-y1), y, color);
			}
		else
			for (int y = y2; y <= y1; y++) {
				ST7735_DrawPixel(x1+ (x2-x1)*(y-y1)/(y2-y1), y, color);
			}
		}
}

/* draws the 4 surrounding pixels of a given point, making the line thicker */
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
	
		ST7735_4Line(CENTERX, CENTERY, CENTERX+(int)hourX, CENTERY - (int)hourY, ST7735_BLACK);
		ST7735_4Line(CENTERX, CENTERY, CENTERX+(int)minuteX, CENTERY - (int)minuteY, ST7735_BLACK);
	
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

	ST7735_4Line(CENTERX, CENTERY, CENTERX+(int)hourX, CENTERY - (int)hourY, ST7735_BLUE);

	ST7735_4Line(CENTERX, CENTERY, CENTERX+(int)minuteX, CENTERY - (int)minuteY, ST7735_WHITE);

	
}

