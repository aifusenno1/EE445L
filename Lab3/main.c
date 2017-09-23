#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "SysTick.h"
#include "ST7735.h"
#include "TimeManagement.h"
#include "Display.h"
#include "Alarm.h"
#include "Button.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

static int count = 0;
static int secFlag = 0;



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
  
	ST7735_InitR(INITR_REDTAB);
  GPIO_PORTF_DIR_R |= 0x04;   // make PF2 out (built-in blue LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;// disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;   // enable digital I/O on PF2
                              // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;     // disable analog functionality on PF  
	
	long sr;
  sr = StartCritical();
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 7999;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
                              // enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x07;
  EndCritical(sr);
	EdgeCounter_Init();
	
  EnableInterrupts();
	
	int time = 3 * 3600 + (30 * 60);
	int alarm = 0;
	
	ST7735_FillScreen(ST7735_BLACK);
	drawHands(time);
	
	
	//temp clock numbers
	ST7735_SetCursor(10,6);
	ST7735_OutUDec(12);
	ST7735_SetCursor(14,7);
	ST7735_OutUDec(1);
	ST7735_SetCursor(16,8);
	ST7735_OutUDec(2);
	ST7735_SetCursor(17,10);
	ST7735_OutUDec(3);
	ST7735_SetCursor(16,12);
	ST7735_OutUDec(4);
	ST7735_SetCursor(14,13);
	ST7735_OutUDec(5);
	ST7735_SetCursor(11,14);
	ST7735_OutUDec(6);
	ST7735_SetCursor(7,13);
	ST7735_OutUDec(7);
	ST7735_SetCursor(5,12);
	ST7735_OutUDec(8);
	ST7735_SetCursor(4,10);
	ST7735_OutUDec(9);
	ST7735_SetCursor(4,8);
	ST7735_OutUDec(10);	
	ST7735_SetCursor(6,7);
	ST7735_OutUDec(11);
	
	//ST7735_DrawBitmap(4, 159, clock1, 120, 120);
	while(1){
		
		
		
		int tempTime = time;
    GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x04; // toggle PF2
		WaitForInterrupt();
		time = handleTime(secFlag, time);
		if(time != tempTime){
			secFlag = 0;
			alarm = checkForAlarm(time);
		}
		if((time % 60) == 0){
			eraseHands(time-60);
			drawHands(time);
		}
		if(alarm){
			playAlarm();
		}
		
		
		
		
		
//    SysTick_Wait(1);        // approximately 720 ns
//    SysTick_Wait(2);        // approximately 720 ns
//    SysTick_Wait(10000);    // approximately 0.2 ms
    //SysTick_Wait10ms(1);      // approximately 10 ms
		
  }
}
