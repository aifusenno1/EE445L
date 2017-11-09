#include "MotionDetect.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"

#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable

#define CAMERA_BUFFER_SIZE		  100
#define VC0706_RESET  0x26
#define VC0706_GEN_VERSION 0x11
#define VC0706_COMM_MOTION_CTRL 0x37
#define VC0706_COMM_MOTION_STATUS 0x38
#define VC0706_COMM_MOTION_DETECTED 0x39
#define VC0706_MOTION_CTRL 0x42
#define VC0706_MOTION_STATUS 0x43
#define VC0706_REC_PRO_SIGN		0x56	// Receive
#define VC0706_RET_PRO_SIGN		0x76	// Return

uint8_t cameraBuff[CAMERA_BUFFER_SIZE+1];
static uint8_t bufferLen = 0;
static uint8_t serialNum = 0;


static void UART_Init(void);
char UART_InChar(void);
void UART_OutChar(char data);
int verifyResponse(uint8_t cmd);
void sendCommand(uint8_t cmd, uint8_t args[], uint8_t argn);
uint8_t readResponse(uint8_t numbytes, uint8_t timeout);
void printBuff(void);
int reset(void);

int motionDetectInit(void) {
	UART_Init();
	return reset();
}

static void flush(void) {
	readResponse(100,10);
}

 int reset(void) {
	flush();  // flush the response
	uint8_t args[] = {0x00};
	uint8_t argn = 1;
   uint8_t respLen = 5; 
	
   sendCommand(VC0706_RESET, args, argn);
   // get reply
   if (readResponse(respLen, 200)!=respLen) 
     return 0;
 		printBuff();

   if (!verifyResponse(VC0706_RESET))
 		return 0;
   return 1;
 }

char * getVersion(void) {
  uint8_t args[] = {0x00};
  uint8_t argn = 1;
  uint8_t respLen = 16;
  sendCommand(VC0706_GEN_VERSION, args, argn);
  // get reply
  if (readResponse(respLen, 200)!=respLen) 
    return 0;
		printBuff();

  if (!verifyResponse(VC0706_GEN_VERSION))
		return 0;
  cameraBuff[bufferLen] = 0;  // end it!
  return (char *) cameraBuff;  // return it!
}


/*
	Send a command to the camera
	first element in ARGS should be the number of arguments
	and ARGN should be number of arguments + 1
*/
void sendCommand(uint8_t cmd, uint8_t args[], uint8_t argn) {	
	UART_OutChar(VC0706_REC_PRO_SIGN);
	UART_OutChar(serialNum);
	UART_OutChar(cmd);

	for (uint8_t i=0; i<argn; i++) {
	  UART_OutChar(args[i]);
	}
}

uint8_t readResponse(uint8_t numbytes, uint8_t timeout) {
//	uint8_t counter = 0;
	bufferLen = 0;

//	while ((timeout != counter) && (bufferLen != numbytes)) {

//		// if not receiving data within the time limit, return 
//		if ((UART2_FR_R&UART_FR_RXFE) == 0) { // receive buffer empty
//				counter++;
//				// delay
//				continue;
//		}

	while (bufferLen != numbytes) {
		cameraBuff[bufferLen++] =	UART_InChar();
		//ST7735_OutChar(cameraBuff[bufferLen-1]);
		ST7735_OutChar('1');
	}

	return bufferLen;
}

int verifyResponse(uint8_t cmd) {
	if (cameraBuff[0] != VC0706_RET_PRO_SIGN ||
		cameraBuff[1] != serialNum ||
		cameraBuff[2] != cmd ||
		cameraBuff[3] != 0x0)  // status bit == 0 is right, others are wrong
		return 0;
	return 1;
}



void printBuff() {
	for (uint8_t i=0; i<bufferLen; i++) {
		ST7735_OutChar(cameraBuff[i]);
	}
}



//------------UART_Init------------
// Initialize the UART for 38,400 baud rate (assuming 50 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
// NOTE: currently using PD6 and PD7 (UART2) will change to PD4 and PD5 (UART6)
static void UART_Init(void){
  SYSCTL_RCGCUART_R |= 0x04;            // activate UART2
  SYSCTL_RCGCGPIO_R |= 0x08;            // activate port D
  while((SYSCTL_PRGPIO_R&0x08) == 0){};
  UART2_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
  UART2_IBRD_R = 32;                    // IBRD = int(80,000,000 / (16 * 38,400)) = int(130.2083)
  UART2_FBRD_R = 35;                     // FBRD = int(0.2083 * 64 + 0.5) = 13
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART2_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART2_CTL_R |= 0x301;                 // enable UART
  GPIO_PORTD_AFSEL_R |= 0xC0;           // enable alt funct on PD7-6
  GPIO_PORTD_DEN_R |= 0xC0;             // enable digital I/O on PD7-6
                                        // configure PD7-6 as UART
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0x00FFFFFF)+0x11000000;
  GPIO_PORTD_AMSEL_R &= ~0xC0;          // disable analog functionality on PD
}



//void UART_Init(void){ // should be called only once
//	 SYSCTL_RCGCUART_R |= 0x02;            // activate UART2
//  SYSCTL_RCGCGPIO_R |= 0x04;            // activate port D
//  while((SYSCTL_PRGPIO_R&0x04) == 0){};
//  UART1_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
//  UART1_IBRD_R = 32;                    // IBRD = int(80,000,000 / (16 * 38,400)) = int(130.2083)
//  UART1_FBRD_R = 35;                     // FBRD = int(0.2083 * 64 + 0.5) = 13
//                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
//  UART1_LCRH_R = 0x00000070;
//  UART1_CTL_R |= 0x01;                 // enable UART
//  GPIO_PORTC_AFSEL_R |= 0x30;           // enable alt funct on PD7-6
//  GPIO_PORTC_DEN_R |= 0x30;             // enable digital I/O on PD7-6
//                                        // configure PD7-6 as UART
//  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;
//  GPIO_PORTC_AMSEL_R &= ~0x30;          // disable analog functionality on PD
//}


//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
char UART_InChar(void){
  while((UART2_FR_R&UART_FR_RXFE) != 0);

  return((char)(UART2_DR_R&0xFF));
}
//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(char data){
  while((UART2_FR_R&UART_FR_TXFF) != 0);
  UART2_DR_R = data;
}

int main(){
	PLL_Init(Bus20MHz);       // 80  MHz
	ST7735_InitR(INITR_REDTAB);
	int a = motionDetectInit();
	
	//ST7735_OutUDec(a);
	getVersion();
	//printBuff();
	
	for (;;) {
//			char in = UART_InChar();
//		ST7735_OutChar(in);

	}
	
}

