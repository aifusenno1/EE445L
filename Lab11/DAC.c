//DAC.c
/*
SSI3Clk PD0 (1) I/O    SSI module 3 clock.
SSI3Fss PD1 (1) I/O    SSI module 3 frame signal.
SSI3Rx  PD2 (1) I      SSI module 3 receive.
SSI3Tx  PD3 (1) O      SSI module 3 transmit.

TLV5616 input impedance for Refin pin is 10M ohm
load current Il = 1.5v/10M = 0.15E-6 A
LM4041C: Iz = 80E-6 A Vref = 1.233 V
Vz = 1.5 v
Vz = Vref(1+R2/R1) = 1.5 V
R2 = 2200 R1 = 10000
Rs <= (3.3 - Vz)/(Il+Iz) = 22457
Rs = 22000

TPA731
Rf = 10k
Ri = 22k
Ci = 0.1 micro
Cs = 0.1 micro
Cb = 4.7 micro
*/

#include <stdint.h>
#include "DAC.h"
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"

// initialize port D for SSI3
void DAC_Init(void) {
	SYSCTL_RCGCSSI_R |= 0x08;  // SSI3
	SYSCTL_RCGCGPIO_R |= 0x08;  // port D
	while ((SYSCTL_PRGPIO_R&0x08) == 0) {};
	GPIO_PORTD_AMSEL_R &= ~0x0F; // 2) disable analog on PD0-3
	GPIO_PORTD_AFSEL_R |= 0x0F; // 3) Enable alternative functionality on PD0-3
	GPIO_PORTD_PCTL_R &= ~0x0000FFFF;
	GPIO_PORTD_PCTL_R |= 0x00001111;// 4) choose GPIO functionality
	GPIO_PORTD_DIR_R |= 0x0F;   // 5) set PB7-4 to be outputs
	GPIO_PORTD_DEN_R |= 0x0F;    // 6) outputs are digital
		
	SSI3_CR1_R = 0x00000000;  //2) Disable SSI, master mode
	SSI3_CPSR_R = 0x08;  //3) 10Mhz SSIClk Fssi = Fbus / (CPSDVSR * (1 + SCR))
	SSI3_CR0_R &= ~(0x0000FFF0);  //  Freescale SCR = 0, SPH = 0, SPO = 0
	SSI3_CR0_R |= 0x0F;              // DSS = 16-bit data
	SSI3_CR1_R |= SSI_CR1_SSE;  //4) enable SSI / set the SSE
}

void DAC_Out(uint16_t data) {
		data &= 0x0FFF;
		//ST7735_OutUDec(data);
	 while((SSI3_SR_R & SSI_SR_TNF) == 0) {}; // wait till Transmit FIFO not full
		SSI3_DR_R = data;
}






