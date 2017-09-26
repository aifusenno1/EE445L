//Paris Kaman
// Allen Pan
// TimeManagement.c
// takes care of time stepping

#include "TimeManagement.h"
#include "ST7735.h"

int time = 6*3600;
char sec[]={0,0,'\0'};
char min[]={0,0,'\0'};
char hour[]={0,0,'\0'};
uint32_t h,m,s; // time as numbers

int updateTime(int flag, int time) {
   if (flag) return ((time+1)%(3600*12));
   else return time;
}

void outputTime(int time){
   
   int hour = (time / 3600) % 12;
   int minute = (time / 60) % 60;
   int second = (time % 60);
   
   ST7735_SetCursor(6,2);
   
	 if(hour == 0) {
		  ST7735_OutUDec(12);
	 }
   else if( hour < 10){
      ST7735_OutUDec(0);
		  ST7735_OutUDec(hour);

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
void getSeconds(int time, char secStr[2]) {
   int sec = time%60;
   if (sec < 10) {
      secStr[0] = '0';
      secStr[1] = 48 + sec;
   } else {
      secStr[0] = 48 + sec/10;
      secStr[1] = 48 + sec%10;
   }

}

void getMinutes(int time, char minStr[2]) {
   int min = (time/60)%60;
   if (min < 10) {
      minStr[0] = '0';
      minStr[1] = 48 + min;
   } else {
      minStr[0] = 48 + min/10;
      minStr[1] = 48 + min%10;
   }
}
void getHours(int time, char hourStr[2]) {
   int hour = (time/3600)%12;
	 if (hour == 0) {
		 hourStr = "12";
	 }
   else if (hour < 10) {
      hourStr[0] = '0';
      hourStr[1] = 48 + hour;
   } else {
      hourStr[0] = 48 + hour/10;
      hourStr[1] = 48 + hour%10;
   }   
}
