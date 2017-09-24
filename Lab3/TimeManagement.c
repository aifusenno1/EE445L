//Paris Kaman
// Allen Pan
// TimeManagement.c
// takes care of time stepping

#include "TimeManagement.h"

int handleTime(int flag, int time){
	int newTime = time;
	int secFlag = flag;
	
	if(secFlag){
		newTime++;
		secFlag = 0;
		
		if(newTime == 12 * 3600){
			newTime = 0;
		}
		int hour = (newTime / 3600) % 12;
		int minute = (newTime / 60) % 60;
		int second = (newTime % 60);
			
		ST7735_SetCursor(0,0);
		
		if(hour == 0){
			ST7735_OutUDec(12);
		}
		else if( hour < 10){
			ST7735_OutUDec(0);
		}
		  ST7735_OutUDec(hour);
		
		ST7735_OutChar(':');
		if(minute < 10){
			ST7735_OutUDec(0);
		}
		ST7735_OutUDec(minute);
		
		ST7735_OutChar(':');
		if(second < 10){
			ST7735_OutUDec(0);
		}
		ST7735_OutUDec(second);
}
	return newTime;
}

