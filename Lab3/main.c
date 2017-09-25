/*
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

#define PF3                     (*((volatile uint32_t *)0x40025020))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF1                     (*((volatile uint32_t *)0x40025008))

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
int time = 6*3600;
int alarm = 0;

stage stages[4] = {
   {0, {"\n"}, 0, {'\n'}, 0, 0, -1, {'\n'}, 0},  													// stage 0: clock display
   {1, {"Set Clock", "Set Alarm", "Exit"}, 3, {'\n'}, 0, 2, 0, {ST7735_YELLOW,ST7735_WHITE,ST7735_WHITE}, 0},   // stage 1: select menu
   {2, {"Save", "Exit"}, 2, {0, 0, 0}, 3, 5, 2, {ST7735_WHITE,ST7735_WHITE,ST7735_YELLOW,ST7735_WHITE,ST7735_WHITE}, 0},               // stage 2: set time
   {3, {"Save", "Exit"}, 2, {0, 0, 0}, 3, 5, 2, {ST7735_WHITE,ST7735_WHITE,ST7735_YELLOW,ST7735_WHITE,ST7735_WHITE}, 0}		              // stage 3: set alarm
};
uint8_t curStage = 0;

// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
   count++;
   if(count == 100){
      count = 0;
      secFlag = 1;
   }
}

int main(){
   
   SYSCTL_RCGCGPIO_R |= 0x20;  // activate port F
   PLL_Init(Bus80MHz);                   // 80 MHz
   PortF_Init();
   ST7735_InitR(INITR_REDTAB);
   
   long sr;
   sr = StartCritical();
   NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
   NVIC_ST_RELOAD_R = 7999;// reload value
   NVIC_ST_CURRENT_R = 0;      // any write to current clears it
   NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
   // enable SysTick with core clock and interrupts
   NVIC_ST_CTRL_R = 0x07;
   EndCritical(sr);
   EdgeInterrupt_Init();
   
   EnableInterrupts();
   
   ST7735_FillScreen(ST7735_BLACK);
   drawHands(time);
   
   //  ST7735_DrawBitmap(4, 159, cal, 120, 160);
   while(1){
      
      int tempTime = time;
      //GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x04; // toggle PF2
      WaitForInterrupt();
      
      time = updateTime(secFlag, time);
      sr = StartCritical();
      switch (curStage) {
         case 0:
         if(time != tempTime){ // if time changed, redraw, reset flag, check alarm
            outputTime(time);	
            secFlag = 0;
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
      
      if(alarm){
         playAlarm();
      }
      
      
      
      
      
      //    SysTick_Wait(1);        // approximately 720 ns
      //    SysTick_Wait(2);        // approximately 720 ns
      //    SysTick_Wait(10000);    // approximately 0.2 ms
      //SysTick_Wait10ms(1);      // approximately 10 ms
      
   }
}
