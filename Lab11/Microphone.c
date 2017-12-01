#include "Microphone.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Serial.h"
#include "Timer.h"


static void ADC0_InitSWTriggerSeq3_Ch11(void);
static uint32_t ADC0_InSeq3(void);
static void DelayWait1ms(uint32_t n);
static void Microphone_Sample(void);

uint32_t voiceVal;

//int main(){
//	PLL_Init(Bus80MHz);       // 80  MHz
//	ST7735_InitR(INITR_REDTAB);
//	Serial_Init();
//	Microphone_Init();
//	for(;;) {
//		int adc = ADC0_InSeq3();
//		Serial_OutUDec(adc);
//		Serial_OutString("\n\r");
//		DelayWait1ms(500);
//	}
//}


void Microphone_Init() {
	ADC0_InitSWTriggerSeq3_Ch11();
	Timer1A_Init(Microphone_Sample, 1813); // 80,000,000/44,100 = 1814 clock cycles / sample
}

static void Microphone_Sample() {
	voiceVal = ADC0_InSeq3();
}



// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain11 (PB5)
// SS3 interrupts: enabled but not promoted to controller
static void ADC0_InitSWTriggerSeq3_Ch11(void){ 
  SYSCTL_RCGCADC_R |= 0x0001;   // 7) activate ADC0 
                                  // 1) activate clock for Port B
  SYSCTL_RCGCGPIO_R |= 0x02;       
  while((SYSCTL_PRGPIO_R&0x02)==0){}; // allow time for clock to start
	GPIO_PORTB_PCTL_R &= ~0x00F00000; // 3) regular GPIO
  GPIO_PORTB_DIR_R &= ~0x20;      // 2) make PB5 input
  GPIO_PORTB_AFSEL_R |= 0x20;     // 3) enable alternate function on PB5
  GPIO_PORTB_DEN_R &= ~0x20;      // 4) disable digital I/O on PB5
  GPIO_PORTB_AMSEL_R |= 0x20;    

//  while((SYSCTL_PRADC_R&0x0001) != 0x0001){};    // good code, but not yet implemented in simulator

  ADC0_PC_R &= ~0xF;              // 7) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field
  ADC0_SSMUX3_R += 11;             //    set channel
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;           // 13) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;         // 14) enable sample sequencer 3
}

//------------ADC0_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
static uint32_t ADC0_InSeq3(void){
	uint32_t result;
  ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
    // if you have an A0-A3 revision number, you need to add an 8 usec wait here
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
  return result;
}


static void DelayWait1ms(uint32_t n){
	uint32_t volatile time;
  while(n){
    time = 72724*2/91;  // 10msec
    while(time){
      time--;
    }
    n--;
  }
}

