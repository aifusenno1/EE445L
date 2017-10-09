//DAC.c
/*
SSI2Clk PB4 (2) I/O    SSI module 2 clock.
SSI2Fss PB5 (2) I/O    SSI module 2 frame signal.
SSI2Rx  PB6 (2) I      SSI module 2 receive.
SSI2Tx  PB7 (2) O      SSI module 2 transmit.
*/

#include "DAC.h"
#include <stdint.h>
#include "..//inc//tm4c123gh6pm.h"

void DAC_Init(uint16_t data) {
	SYSCTL_RCGCSSI_R |= 0x04;  // SSI2
	SYSCTL_RCGCGPIO_R |= 0x02;  // port B
	while ((SYSCTL_PRGPIO_R&0x02) == 0) {};
	GPIO_PORTB_AMSEL_R &= ~0xF0; // 2) disable analog on PB4-7
	GPIO_PORTB_AFSEL_R |= 0xF0; // 3) Enable alternative functionality on PB4-7
	GPIO_PORTB_PCTL_R &= ~0xFFFF0000;
	GPIO_PORTB_PCTL_R |= 0x22220000;// 4) choose GPIO functionality
	GPIO_PORTB_DIR_R |= 0xF0;   // 5) set PB7-4 to be outputs
	GPIO_PORTB_DEN_R |= 0xF0;    // 6) outputs are digital
		
	SSI2_CR1_R = 0x00000000;  //2) Disable SSI, master mode
	SSI2_CPSR_R = 0x08;  //3) 10Mhz SSIClk Fssi = Fbus / (CPSDVSR * (1 + SCR))
	SSI2_CR0_R &= ~(0x0000FFFF);  //3) SCR = 0, Freescale frame format.
	SSI2_CR0_R |= 0x4F;  //5)) DSS = 16 bit data,  SPH = 0 and SP0 = 1 ( use 16 bit data??)
	SSI2_CR1_R |= SSI_CR1_SSE;  //4) enable SSI / set the SSE
}
	