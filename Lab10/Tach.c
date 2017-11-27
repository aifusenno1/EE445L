
#include <stdint.h>
#include "Tach.h"
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"

#define PF2                     (*((volatile uint32_t *)0x40025010))
#define TIMER_TAMR_TACMR        0x00000004  // GPTM TimerA Capture Mode
#define TIMER_TAMR_TAMR_CAP     0x00000003  // Capture mode
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_CTL_TAEVENT_POS   0x00000000  // Positive edge
#define TIMER_CTL_TAEVENT_NEG   0x00000004  // Negative edge
#define TIMER_CTL_TAEVENT_BOTH  0x0000000C  // Both edges
#define TIMER_IMR_CAEIM         0x00000004  // GPTM CaptureA Event Interrupt
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load
#define NVIC_EN0_INT21          0x00200000  // Interrupt 19 enable

uint32_t Period;              // (1/clock) units
uint32_t First;               // Timer0A first edge
int32_t Done;                 // set each rising 

void Tach_Init(void) {
	SYSCTL_RCGCTIMER_R |= 0x02;// activate timer1 
  SYSCTL_RCGCGPIO_R |= 0x22;       // activate port B and port F
                                   // allow time to finish activating
  First = 0;                       // first will be wrong
  Done = 0;                        // set on subsequent
  GPIO_PORTB_DIR_R &= ~0x20;       // make PB5 in
  GPIO_PORTB_AFSEL_R |= 0x20;      // enable alt funct on PB5/T1CCP1
  GPIO_PORTB_DEN_R |= 0x20;        // enable digital I/O on PB5
                                   // configure PB6 as T1CCP1
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFF0FFFFF)+0x00700000;
  GPIO_PORTB_AMSEL_R &= ~0x20;     // disable analog functionality on PB5
	
  GPIO_PORTF_DIR_R |= 0x04;        // make PF2 out (PF2 built-in blue LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;     // disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;        // enable digital I/O on PF2
                                   // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
	
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN; // disable timer1A during setup
  TIMER1_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
                                   // configure for 24-bit capture mode
  TIMER1_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);
                                   // configure for rising edge event
  TIMER1_CTL_R &= ~(TIMER_CTL_TAEVENT_POS|0xC);
  TIMER1_TAILR_R = TIMER_TAILR_TAILRL_M;// start value
  TIMER1_TAPR_R = 0xFF;            // activate prescale, creating 24-bit
  TIMER1_IMR_R |= TIMER_IMR_CAEIM; // enable capture match interrupt
  TIMER1_ICR_R = TIMER_ICR_CAECINT;// clear timer1A capture match flag
  TIMER1_CTL_R |= TIMER_CTL_TAEN;  // enable timer1A 16-b, +edge timing, interrupts
                                   // Timer0A=priority 2
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00004000; // top 3 bits
  NVIC_EN0_R = NVIC_EN0_INT21;     // enable interrupt 19 in NVIC

}

void Timer1A_Handler(void){
  PF2 = PF2^0x04;  // toggle PF2
  PF2 = PF2^0x04;  // toggle PF2
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// acknowledge timer0A capture match
  Period = (First - TIMER0_TAR_R)&0xFFFFFF;// 24 bits, 12.5ns resolution
  First = TIMER0_TAR_R;            // setup for next
  Done = 1;
  PF2 = PF2^0x04;  // toggle PF2
}

uint32_t getMeasure(void) {
	ST7735_SetCursor(0,0);
	ST7735_OutString("RPS = ");
	ST7735_OutUDec(80000000/Period);
	return 1;
}

