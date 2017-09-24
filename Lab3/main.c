#include <stdint.h>
#include "tm4c123gh6pm.h"
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

typedef struct stage_t {
	uint8_t stageNum;
	char *options[3];
	uint8_t optionsLen;
	int time[3];
	uint8_t timeLen;
	uint8_t totalLen;     // concat options with time
	uint8_t highlighted;  // -1 for none highlighted, otherwise index (out of totalLen)
} stage;

static int count = 0;
static int secFlag = 0;
stage stage1 = {1, {}, 0, {}, 0, 0, -1};  // stage 1: clock display
stage stage2 = {2, {"Set Clock", "Set Alarm", "Exit"}, 3, {}, 0, 2, 0};				 // stage 2: select menu
stage stage3 = {3, {"Save", "Exit"}, 2, {0, 0, 0}, 3, 5, 2};									// stage 3: set time
stage stage4 = {3, {"Save", "Exit"}, 2, {0, 0, 0}, 3, 5, 2};									// stage 4: set alarm
uint8_t curState = 1;

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
	
//  ST7735_DrawBitmap(4, 159, cal, 120, 160);
	while(1){
		
		int tempTime = time;
    GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x04; // toggle PF2
		WaitForInterrupt();
		time = handleTime(secFlag, time);
		if(time != tempTime){ // if time changed, reset flag, check alarm
			secFlag = 0;
			alarm = checkForAlarm(time);
		}
		switch (stage) {
			case 1:
				
			case 2:
				
			case 3:
				
			case 4:
			
		}
		if((time % 60) == 0){ // every minute, erase hand and draw again
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
