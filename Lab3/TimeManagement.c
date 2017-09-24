//Paris Kaman
// Allen Pan
// TimeManagement.c
// takes care of time stepping

#include "TimeManagement.h"

int updateTime(int flag, int time) {
	if (flag) return ++time;
	else return time;
}

void outputTime(int time){
	
		if(time == 12 * 3600){
			time = 0;
		}
		int hour = (time / 3600) % 12;
		int minute = (time / 60) % 60;
		int second = (time % 60);
			
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

