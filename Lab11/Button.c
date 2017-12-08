/* Button.c */
#include "Button.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "MotionDetect.h"
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

volatile static unsigned long last4, last0;      // previous
extern uint8_t curStage;
int timeron = 0;
static void PortF_Init(void);
static void EdgeInterrupt_Init(void);
extern int alarm, doorBell;


void Button_Init(void) {
	PortF_Init();
	EdgeInterrupt_Init();
	
}




static void Timer0Arm(void){
   TIMER0_CTL_R &= ~0x00000001;    // 1) disable TIMER0A during setup
   TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
   TIMER0_TAMR_R = 0x0000001;    // 3) 1-SHOT mode
   TIMER0_TAILR_R = 800000;      // 4) 10ms reload value
   TIMER0_TAPR_R = 0;            // 5) bus clock resolution
   TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
   TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
   NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
   // interrupts enabled in the main program after all devices initialized
   // vector number 35, interrupt number 19
   NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
   TIMER0_CTL_R |= 0x00000001;    // 10) enable TIMER0A
}
// Interrupt 10 ms after rising edge of PF4
void Timer0A_Handler(void){
   TIMER0_IMR_R = 0x00000000;    // disarm timeout interrupt
   last4 = PF4 & 0x10;  // switch state
   last0 = PF0 & 0x01;
   GPIO_PORTF_ICR_R = 0x11; // (e) acknowledge interrupt
   GPIO_PORTF_IM_R |= 0x11; // (f) re-arm interrupt on PF4 & PF0
   NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // (g) priority 5
   NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
   
}

void GPIOPortF_Handler(void){

   if (GPIO_PORTF_RIS_R&0x10) { // PF4 is pressed
      GPIO_PORTF_IM_R &= ~0x10;     // disarm interrupt on PF4
      if(last4){    // 0x10 means it was previously released; negative logic

      }

   }
   
   if (GPIO_PORTF_RIS_R&0x01) { // PF0 is pressed
      GPIO_PORTF_IM_R &= ~0x01;     // disarm interrupt on PF0
      long sr;
      if(last0){    // 0x01 means it was previously released; negative logic
				TVon();
				alarm = 1;
				doorBell = 1;
      }

   }
   Timer0Arm(); // start one shot
}

/* initialize PF4 for interrupt */
static void EdgeInterrupt_Init(void) {
   GPIO_PORTF_IS_R &= ~0x11; // (d) PF4 & PF0 are edge-sensitive
   GPIO_PORTF_IBE_R |= 0x11; //     is both edges
   GPIO_PORTF_ICR_R = 0x11; // (e) clear flag4 flag0
   GPIO_PORTF_IM_R |= 0x11; // (f) arm interrupt on PF4 and pf0
   NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
   NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
   
   SYSCTL_RCGCTIMER_R |= 0x01;      // 0) activate TIMER0
   last4 = PF4 & 0x10;                 // initial switch state
   last0 = PF0 & 0x01;
}



static void PortF_Init(void){ volatile uint32_t delay;
   SYSCTL_RCGCGPIO_R |= 0x00000020;  // 1) activate clock for Port F
   delay = SYSCTL_RCGCGPIO_R;        // allow time for clock to start
   GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
   GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
   // only PF0 needs to be unlocked, other bits can't be locked
   GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
   GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
   GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
   GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
   GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
   GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}

