/* main.c
Xinyuan (Allen) Pan & Paris Kaman

PD0: Speaker
PE4: Potentiometer
PF4: button
PF1: button
*/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "SysTick.h"
#include "ST7735.h"
#include "TimeManagement.h"
#include "Display.h"
#include "Alarm.h"
#include "Button.h"
#include "main.h"
#include "ADCSWTrigger.h"

#define PF3                     (*((volatile uint32_t *)0x40025020))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF1                     (*((volatile uint32_t *)0x40025008))
//#define PD0                     (*((volatile uint32_t *)0x40007

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


static int count = 0;
static int secFlag = 0;
char sec[]={0,0,'\0'};
char min[]={0,0,'\0'};
char hour[]={0,0,'\0'};
uint32_t h,m,s; // time as numbers
int time = 6*3600;
int alarm = 0;
static int numToggle = 0;
int inAlarm = 0;
uint32_t ADCvalue;

stage stages[4] = {
   {{"\n"}, {'\n'}, -1, {'\n'}, 0},  													// stage 0: clock display
   {{"Set Clock", "Set Alarm", "Exit"}, {'\n'}, 0, {ST7735_YELLOW,ST7735_WHITE,ST7735_WHITE}, 0},   // stage 1: select menu
   {{"Save", "Exit"}, {0, 0, 0}, 2, {ST7735_WHITE,ST7735_WHITE,ST7735_YELLOW,ST7735_WHITE,ST7735_WHITE}, 0},               // stage 2: set time
   {{"Save", "Exit"}, {0, 0, 0}, 2, {ST7735_WHITE,ST7735_WHITE,ST7735_YELLOW,ST7735_WHITE,ST7735_WHITE}, 0}		              // stage 3: set alarm
};
uint8_t curStage = 0;

void PortD_Init(void){
   SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
   while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
   // 2) no need to unlock PD3-0
   GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PD3-0
   GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO
   GPIO_PORTD_DIR_R |= 0x0F;         // 5) make PD0 out
   GPIO_PORTD_AFSEL_R &= ~0x0F;      // 6) regular port function
   GPIO_PORTD_DEN_R |= 0x0F;         // 7) enable digital I/O on PD0
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
   count++;
   if(count == 100){
      count = 0;
      secFlag = 1;
   }
   if(alarm | inAlarm){
      if(numToggle < 50){
         GPIO_PORTD_DATA_R ^= 0x01;
      }
      if(numToggle == 50){
         GPIO_PORTD_DATA_R &= ~0x01;
      }
      if(numToggle == 100){
         numToggle = 0;
      }
      inAlarm = 1;
      numToggle++;
   }
   
}

int main(){
   
   SYSCTL_RCGCGPIO_R |= 0x20;  // activate port F
   PLL_Init(Bus80MHz);                   // 80 MHz
   PortF_Init();
   PortD_Init();
   ADC0_InitSWTriggerSeq3_Ch9();
   ST7735_InitR(INITR_REDTAB);
   
   long sr;
   sr = StartCritical();
   NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
   NVIC_ST_RELOAD_R = 799999;// reload value
   NVIC_ST_CURRENT_R = 0;      // any write to current clears it
   NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
   // enable SysTick with core clock and interrupts
   NVIC_ST_CTRL_R = 0x07;
   EndCritical(sr);
   EdgeInterrupt_Init();
   
   EnableInterrupts();
   
	 // Initial Display
   ST7735_FillScreen(ST7735_BLACK);
	 outputTime(time);	
   drawFace();
   drawHands(time);
   
   while(1){
      
      int tempTime = time;
      //GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x04; // toggle PF2
      WaitForInterrupt();
      
      time = updateTime(secFlag, time);
      alarm = checkForAlarm(time);
      if(time != tempTime){ // if time changed, redraw, reset flag, check alarm
         secFlag = 0;
      }
      sr = StartCritical();
      switch (curStage) {
         case 0:
         if(time != tempTime){ // if time changed, redraw, reset flag, check alarm
            outputTime(time);	
         }
         if((time % 60) == 0){ // every minute, erase hand and draw again
            eraseHands(time-60);
            drawHands(time);
         }
         break;
         case 1:
         ST7735_DrawString(6,6,stages[1].options[0], stages[1].color[0]);
         ST7735_DrawString(6,8,stages[1].options[1], stages[1].color[1]);
         ST7735_DrawString(6,10,stages[1].options[2], stages[1].color[2]);
         
         break;
         case 2:
         ADCvalue = ADC0_InSeq3();
         if (stages[2].selected) {
            if (stages[2].highlight == 2) { // hour: 1-12
               h = ADCvalue*12/4096 + 1; // use 4096 here to avoid 13
               if (h < 10) {
                  hour[0] = '0';
                  hour[1] = 48 + h;
               } else {
                  hour[0] = 48 + h/10;
                  hour[1] = 48 + h%10;
               }   
            }
            else if (stages[2].highlight == 3) { // min: 0-59
               m = ADCvalue*60/4096; // use 4096 to avoid 60
               if (m < 10) {
                  min[0] = '0';
                  min[1] = 48 + m;
               } else {
                  min[0] = 48 + m/10;
                  min[1] = 48 + m%10;
               }   
            }
            else if (stages[2].highlight == 4) { // sec: 0-59
               s = ADCvalue*60/4096;
               
               if (s < 10) {
                  sec[0] = '0';
                  sec[1] = 48 + s;
               } else {
                  sec[0] = 48 + s/10;
                  sec[1] = 48 + s%10;
               }   
            }
         }
         ST7735_DrawString(8,8,stages[2].options[0], stages[2].color[0]);
         ST7735_DrawString(8,10,stages[2].options[1], stages[2].color[1]);
         ST7735_SetCursor(6, 6);
         
         ST7735_DrawString(6,6,hour,stages[2].color[2]);
         ST7735_DrawString(8,6,":",ST7735_WHITE);
         ST7735_DrawString(9,6,min,stages[2].color[3]);
         ST7735_DrawString(11,6,":",ST7735_WHITE);
         ST7735_DrawString(12,6,sec,stages[2].color[4]);
         break;
				 
         case 3:
					 ADCvalue = ADC0_InSeq3();
					 if (stages[3].selected) {
            if (stages[3].highlight == 2) { // hour: 1-12
               h = ADCvalue*12/4096 + 1; // use 4096 here to avoid 13
               if (h < 10) {
                  hour[0] = '0';
                  hour[1] = 48 + h;
               } else {
                  hour[0] = 48 + h/10;
                  hour[1] = 48 + h%10;
               }   
            }
            else if (stages[3].highlight == 3) { // min: 0-59
               m = ADCvalue*60/4096; // use 4096 to avoid 60
               if (m < 10) {
                  min[0] = '0';
                  min[1] = 48 + m;
               } else {
                  min[0] = 48 + m/10;
                  min[1] = 48 + m%10;
               }   
            }
            else if (stages[3].highlight == 4) { // sec: 0-59
               s = ADCvalue*60/4096;
               
               if (s < 10) {
                  sec[0] = '0';
                  sec[1] = 48 + s;
               } else {
                  sec[0] = 48 + s/10;
                  sec[1] = 48 + s%10;
               }   
            }
         }
         ST7735_DrawString(8,8,stages[3].options[0], stages[3].color[0]);
         ST7735_DrawString(8,10,stages[3].options[1], stages[3].color[1]);
         ST7735_SetCursor(6, 6);
         
         ST7735_DrawString(6,6,hour,stages[3].color[2]);
         ST7735_DrawString(8,6,":",ST7735_WHITE);
         ST7735_DrawString(9,6,min,stages[3].color[3]);
         ST7735_DrawString(11,6,":",ST7735_WHITE);
         ST7735_DrawString(12,6,sec,stages[3].color[4]);
         break;
      }		
      EndCritical(sr);  
      
   }
}
