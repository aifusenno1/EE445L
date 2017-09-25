//Paris Kaman
// Allen Pan
// TimeManagement.c
// takes care of time stepping

#include "TimeManagement.h"

int updateTime(int flag, int time) {
   if (flag) return ((time+1)%(3600*12));
   else return time;
}

void outputTime(int time){
   
   int hour = (time / 3600) % 12;
   int minute = (time / 60) % 60;
   int second = (time % 60);
   
   ST7735_SetCursor(0,0);
   
	 if(hour == 0) {
		  ST7735_OutUDec(12);
	 }
   else if( hour < 10){
      ST7735_OutUDec(0);
   } else {
		    ST7735_OutUDec(hour);
	 }
   
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

/* return a string of two digits */
char *getSeconds(int time) {
   int sec = time%60;
   static char secStr[2] = "";
   if (sec < 10) {
      secStr[0] = '0';
      secStr[1] = 48 + sec;
   } else {
      secStr[0] = 48 + sec/10;
      secStr[1] = 48 + sec%10;
   }
   return secStr;
}
char *getMinutes(int time) {
   int min = time/60%60;
   static char minStr[2] = "";
   if (min < 10) {
      minStr[0] = '0';
      minStr[1] = 48 + min;
   } else {
      minStr[0] = 48 + min/10;
      minStr[1] = 48 + min%10;
   }
   return minStr;
}
char *getHours(int time) {
   int hour = time/3600%12;
   static char hourStr[2] = "";
   if (hour < 10) {
      hourStr[0] = '0';
      hourStr[1] = 48 + hour;
   } else {
      hourStr[0] = 48 + hour/10;
      hourStr[1] = 48 + hour%10;
   }
   return hourStr;
   
}
