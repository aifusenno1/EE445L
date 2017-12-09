// main.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Allen Pan
// Paris Kaman
// November 20, 2017


#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include <string.h>
#include "PLL.h"
#include "ST7735.h"
#include "Button.h"
#include "Timer.h"
#include "MotionDetect.h"
#include "Serial.h"
#include "Microphone.h"
#include "DAC.h"
#include "Keypad.h"
#include "Servo.h"
#include "main.h"
#include "Alarm.h"
#include "SysTick.h"



#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

stage stages[5] = {
   {{"\n"}, {'\n'}, -1, {'\n'}, 0},  																																												// stage 0: set password
   {{"Set Clock", "Set Alarm", "Exit"}, {'\n'}, 0, {ST7735_YELLOW,ST7735_WHITE,ST7735_WHITE}, 0},   												// stage 1: idle/motion detecting
   {{"Save", "Exit"}, {0, 0, 0}, 2, {ST7735_WHITE,ST7735_WHITE,ST7735_YELLOW,ST7735_WHITE,ST7735_WHITE}, 0},               	// stage 2: enter password
   {{"Save", "Exit"}, {0, 0, 0}, 2, {ST7735_WHITE,ST7735_WHITE,ST7735_YELLOW,ST7735_WHITE,ST7735_WHITE}, 0},	              // stage 3: entry granted
	 {{"Save", "Exit"}, {0, 0, 0}, 2, {ST7735_WHITE,ST7735_WHITE,ST7735_YELLOW,ST7735_WHITE,ST7735_WHITE}, 0}		              // stage 3: alarm tripped 
};


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
static void keypadDelay(void);

	
static uint8_t curStage = 0;
static int count = 0;
static int guessCount = 0;
static int set[4] = {0,0,0,0};
static char guess[4] = {'_', '_', '_', '_'};
static int pins[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
static int detecting = 0;
static int inThree = 0;
static int inFour = 0;
static int attempts = 0;


void State_Handler(void){
	//long sr = StartCritical();
//      int tempTime = time;
//      GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x04; // toggle PF2
//      
//      time = updateTime(secFlag, time);
//      alarm = checkForAlarm(time);
//      if(time != tempTime){ // if time changed, redraw, reset flag, check alarm
//         secFlag = 0;
//      }
//			timeInactive = time - lastTimePressed;
      switch (curStage) {
				
				/*SCENE 0
					Landing screen which prompts the user to enter a 4 digit passcode
					*/
				case 0:
						 // Initial Display
					ST7735_FillScreen(ST7735_BLACK);
					ST7735_SetCursor(0,3);
					ST7735_OutString("  Set your password:\n\n\n\n");
					ST7735_OutString("       * * * *");
						while(count < 4){
							getPins(pins);
							if(pins[0] || pins[1] || pins[2] || pins[3] || pins[4] || pins[5] ||
								pins[6] || pins[7] || pins[8] || pins[9] || pins[10] || pins[11]){
									int num = whichPin(pins);			//determine which number is pressed
									if(num < 10 && num >= 0){
										//if(validPins(num)){
											set[count] = num;
											ST7735_SetCursor(7 + 2 * count, 7);
											ST7735_OutUDec(num);
											count++;
										//}
										keypadDelay();
									}
								}
						}
						setPassCode(set[0] * 1000 + set[1] * 100 + set[2] * 10 + set[3]);
						curStage = 1;
						detecting = 0;
         break;
						
						
         case 1:
					 curStage = 2;
					if(detecting == 0){
						ST7735_FillScreen(ST7735_BLACK);
						ST7735_SetCursor(0,3);
						ST7735_OutString("     Password set\n\n\n\n");
						ST7735_OutString("   Motion Detecting");
						detecting = 1;
						//doorLock();
						attempts = 0;
					}
         break;
				 
				 
         case 2:
					 doorLock();
					ST7735_FillScreen(ST7735_BLACK);
					ST7735_SetCursor(0,3);
					ST7735_OutString("   Enter Password:\n\n\n\n");
					ST7735_OutString("     _ _ _ _");
				 inThree = 0;
				 inFour = 0;
				 
					while(guessCount < 4){
							getPins(pins);
							if(pins[0] || pins[1] || pins[2] || pins[3] || pins[4] || pins[5] ||
								pins[6] || pins[7] || pins[8] || pins[9] || pins[10] || pins[11]){
									int num = whichPin(pins);			//determine which number is pressed
									TVon();
									if(num < 10 && num >= 0){
										//if(validPins(num)){
											guess[guessCount] = num + 48;
											guessCount++;
											ST7735_SetCursor(0,7);
											ST7735_OutString("     ");
											ST7735_OutChar(guess[0]);
											ST7735_OutString(" ");
											ST7735_OutChar(guess[1]);
											ST7735_OutString(" ");
											ST7735_OutChar(guess[2]);
											ST7735_OutString(" ");
											ST7735_OutChar(guess[3]);
										//}
											keypadDelay();

									}
								}
						}
//							ST7735_SetCursor(0,10);
//							ST7735_OutString("Attempts Left: ");
//							ST7735_OutUDec(3 - attempts);
					if(isPassCode((guess[0]-48) * 1000 + (guess[1]-48) * 100 + (guess[2]-48) * 10 + (guess[3]-48))){
							curStage = 3;
							guess[0] = '_';
							guess[1] = '_';
							guess[2] = '_';
							guess[3] = '_';		
						guessCount = 0;
					}
					else{
						attempts++;
						if(attempts < 3){
							guess[0] = '_';
							guess[1] = '_';
							guess[2] = '_';
							guess[3] = '_';		
							guessCount = 0;
						}
						else{
							curStage = 4;
						}
					}
         break;
				 
				 
         case 3:
					if(inThree){
						 break;
					 }
					ST7735_FillScreen(ST7735_BLACK);
					ST7735_SetCursor(0,5);
					ST7735_OutString("   Welcome Home\n\n\n\n");
					inThree = 1;
					 doorUnlock();
					 for(int j = 0; j < 100; j++){
							keypadDelay();
					 }
					 curStage = 2;
         break;
				 
				 
				 case 4:
					 if(inFour){
						 break;
					 }
					 ST7735_FillScreen(ST7735_BLACK);
					ST7735_SetCursor(0,3);
					ST7735_OutString("   INTRUDER ALERT\n\n\n\n");
					ST7735_OutString("    WEAPONS ONLINE");
				 //Music_Play();	//alarm
					 alarm = 1;			//alarm
					 inFour = 1;
					 for(int j = 0; j < 100; j++){
					 		keypadDelay();
									}
					 curStage = 2;
							guess[0] = '_';
							guess[1] = '_';
							guess[2] = '_';
							guess[3] = '_';		
									guessCount = 0;
									
         break;
      }		
      //EndCritical(sr); 
}

static void keypadDelay(){
		for(int i = 0; i < 100000; i++){}
}


int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 80 MHz
  ST7735_InitR(INITR_REDTAB);
	Alarm_Init();
	SysTick_Init();
	Microphone_Init();
	Keypad_Init();
	Servo_Init();
	Serial_Init();
	Button_Init();
	MotionDetect_Init();
	EnableInterrupts();

	setMotionDetect(1);
	getMotionDetect()? ST7735_OutString("ON\n") : ST7735_OutString("OFF\n");
	TVoff();
   while(1){
	    State_Handler(); 
   }
}




