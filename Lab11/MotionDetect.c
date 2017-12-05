/* MotionDetect.c

Specific API see VC0706 Datasheet
NOTE
timeout = 200 for ReadResponse is strictly required, observed from testing
*/

#include <stdint.h>
#include "MotionDetect.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Serial.h"
#include "Timer.h"


#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable

#define CAMERA_BUFFER_SIZE				  100
#define VC0706_RESET  							0x26
#define VC0706_COMM_MOTION_CTRL 		0x37
#define VC0706_COMM_MOTION_STATUS 	0x38
#define VC0706_COMM_MOTION_DETECTED 0x39
#define VC0706_MOTION_CTRL					0x42
#define VC0706_MOTION_STATUS				0x43
#define VC0706_TVOUT_CTRL 					0x44
#define VC0706_MOTIONCONTROL 				0x00
#define VC0706_UARTMOTION 					0x01
#define VC0706_ACTIVATEMOTION 			0x01
#define VC0706_REC_PRO_SIGN					0x56	// Receive
#define VC0706_RET_PRO_SIGN					0x76	// Return

uint8_t cameraBuff[CAMERA_BUFFER_SIZE+1];
static uint8_t bufferLen = 0;
static uint8_t serialNum = 0;

static void flush(void);
static void DelayWait1ms(uint32_t n);
static void UART_Init(void);
static void UART_OutChar(uint8_t data);
static int verifyResponse(uint8_t cmd);
static void sendCommand(uint8_t cmd, uint8_t args[], uint8_t argn);
static uint8_t readResponse(uint8_t numbytes, uint8_t timeout);
static void printBuff(void);

/*
Initialize the Camera
*/
int MotionDetect_Init(void) {
	UART_Init();
	//Serial_OutString("VC0706 Camera Initialization\n\r");
//	if (reset()) {
//		 //ST7735_OutString("Camera found\n\r");
//	} else {
//		// ST7735_OutString("Camera not found?\n\r");
//		return 0;
//	}
	return 1;
}

/*
resets the camera
*/
int reset(void) {
	flush();
	
	uint8_t args[] = {0x00};
	uint8_t argn = 1;
	uint8_t respLen = 5; 
	sendCommand(VC0706_RESET, args, argn);
	// get reply
	if (readResponse(respLen, 200)!=respLen) 
		return 0;
	if (!verifyResponse(VC0706_RESET))
		return 0;
	return 1;
}

/*
enable/disable motion control
mainly for internal usage
*/
int setMotionStatus(uint8_t x, uint8_t d1, uint8_t d2) {

	flush();  // flush the FIFO; cameraBuff will contain useless data
	//ST7735_OutString("After Flush\n");
	uint8_t args[] = {0x03, x,d1,d2};
	uint8_t respLen = 5; 
	
	sendCommand(VC0706_MOTION_CTRL, args, sizeof(args));
	//ST7735_OutString("sent\n");

	// get reply
	int reply = readResponse(respLen, 200);
	//ST7735_OutString("got response\n");

	if (reply!=respLen)
		return 0;
	if (!verifyResponse(VC0706_MOTION_CTRL)) {
		ST7735_OutString(" fail ");
		ST7735_OutUDec(cameraBuff[0]); 
				return 0;
	}

	return 1;
}

/*
enable/disable motion detection according to the flag
enable: 1
disable: 0
*/
int setMotionDetect(uint8_t flag) {
	if (! setMotionStatus(VC0706_MOTIONCONTROL, VC0706_UARTMOTION, VC0706_ACTIVATEMOTION))
		return 0;  
	//ST7735_OutString("After\n");
	readResponse(100,50);  // flush the FIFO; cameraBuff will contain useless data
	uint8_t args[] = {0x01,flag};
	uint8_t respLen = 5; 
	sendCommand(VC0706_COMM_MOTION_CTRL, args, sizeof(args));
	// get reply
	int reply = readResponse(respLen, 200);
	if (reply !=respLen)
		return 0;
	if (!verifyResponse(VC0706_COMM_MOTION_CTRL)) {
		ST7735_OutString(" fail2 ");
		ST7735_OutUDec(cameraBuff[0]); 
			return 0;
	}
	return 1;
}

/*
Check if motion detection is on or off
returns the status 1 for On and 0 for OFF
*/
int getMotionDetect(void) {
	//ST7735_OutString("before flush\n");
	readResponse(100,50);  // flush the FIFO; cameraBuff will contain useless data
	uint8_t args[] = {0x0};
	uint8_t respLen = 6; 

	sendCommand(VC0706_COMM_MOTION_STATUS, args, sizeof(args));
	//ST7735_OutString("sent\n");

	// get reply
	if (readResponse(respLen, 200)!=respLen) 
		return 0;
	if (!verifyResponse(VC0706_COMM_MOTION_STATUS))
		return 0;
	return cameraBuff[5];
}

/*
return whether a motion is detected or not
no need for sending command, camera sends message whenever it detects motion (another reason to flush FIFO)
*/
int motionDetected(void) {
	uint8_t respLen = 4;
	if (readResponse(respLen, 100) != respLen) {
		return 0;
	}
	if (! verifyResponse(VC0706_COMM_MOTION_DETECTED)) {
		ST7735_OutUDec(cameraBuff[0]); 
		return 0;
	}
	return 1;
}

int counter = 0;
void handler() {
	counter++;
	if (counter == 100){
		TIMER0_CTL_R &= ~TIMER_CTL_TBEN;    // 1) disable TIMER3A during setup
		TVoff();
		counter = 0;
	}
}

int TVon() {
	readResponse(100,50);  // flush the FIFO; cameraBuff will contain useless data
	uint8_t args[] = {0x1, 0x1};
	uint8_t respLen = 5;
	sendCommand(VC0706_TVOUT_CTRL, args, sizeof(args));
	// get reply
	if (readResponse(respLen, 200)!=respLen) 
	return 0;
	if (!verifyResponse(VC0706_TVOUT_CTRL))
	return 0;
	
	//Timer0B_Init( handler,800000);
		Timer2A_Init(&handler, 8000000);

	return 1;
}


int TVoff() {
	readResponse(100,50);  // flush the FIFO; cameraBuff will contain useless data
	uint8_t args[] = {0x1, 0x0};
	uint8_t respLen = 5;
	
	sendCommand(VC0706_TVOUT_CTRL, args, sizeof(args));
	
	// get reply
	if (readResponse(respLen, 200)!=respLen) 
		return 0;
	if (!verifyResponse(VC0706_TVOUT_CTRL))
		return 0;
	return 1;
}


/*
Send a command to the camera
first element in ARGS should be the number of arguments
and ARGN should be number of arguments + 1
*/
static void sendCommand(uint8_t cmd, uint8_t args[], uint8_t argn) {	
	UART_OutChar(VC0706_REC_PRO_SIGN);
	UART_OutChar(serialNum);
	UART_OutChar(cmd);
	for (uint8_t i=0; i<argn; i++) {
		UART_OutChar(args[i]);
	}
}


static uint8_t readResponse(uint8_t numbytes, uint8_t timeout) {
	uint8_t counter = 0;
	bufferLen = 0;
	
	while ((counter != timeout)&&(bufferLen != numbytes)) {
		// if not receiving data within the time limit, return 
		if ((UART6_FR_R&UART_FR_RXFE) != 0) { // receive buffer empty
			counter++;
			DelayWait1ms(3);  // delay
			continue;
		}
		
		cameraBuff[bufferLen++] =	UART6_DR_R&0xFF;
		counter = 0;
	}
	//ST7735_OutUDec(counter);
		//ST7735_OutString(" ");

	//ST7735_OutUDec(bufferLen);

	//ST7735_OutString("\n");
	return bufferLen;
}

static int verifyResponse(uint8_t cmd) {
	if (cameraBuff[0] != VC0706_RET_PRO_SIGN ||
		cameraBuff[1] != serialNum ||
		cameraBuff[2] != cmd ||
		cameraBuff[3] != 0x0
	)  // status bit == 0 is right, others are wrong
		return 0;
	return 1;
}

/*
prints cameraBuff to serial port
*/
static void printBuff() {
//	for (uint8_t i=0; i<bufferLen; i++) {
//		Serial_OutUHex(cameraBuff[i]);
//		Serial_OutString(" ");
//	}
//	Serial_OutString("\n\r");
}

/*
flush the receive FIFO
*/
static void flush(void) {
	readResponse(100,150);
}

//------------UART_Init------------
// Initialize the UART for 38,400 baud rate (assuming 50 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
// NOTE: currently using PD6 and PD7 (UART2) will change to PD4 and PD5 (UART6)
static void UART_Init(void){
  SYSCTL_RCGCUART_R |= 0x40;            // activate UART6
  SYSCTL_RCGCGPIO_R |= 0x08;            // activate port D
  while((SYSCTL_PRGPIO_R&0x08) == 0){};
  UART6_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
  UART6_IBRD_R = 130;                    // IBRD = int(80,000,000 / (16 * 38,400)) = int(130.2083)
  UART6_FBRD_R = 13;                     // FBRD = int(0.2083 * 64 + 0.5) = 13
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART6_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART6_CTL_R |= UART_CTL_UARTEN;                 // enable UART
  GPIO_PORTD_AFSEL_R |= 0x30;           // enable alt funct on PD5-4
  GPIO_PORTD_DEN_R |= 0x30;             // enable digital I/O on PD7-6
                                        // configure PD7-6 as UART
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFF00FFFF)+0x00110000;
  GPIO_PORTD_AMSEL_R &= ~0x30;          // disable analog functionality on PD
}



//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(uint8_t data){
	while((UART6_FR_R&UART_FR_TXFF) != 0);
	UART6_DR_R = data;
	//Serial_OutUHex(data);
	//Serial_OutString(" ");
}

// delay for 1 ms
// modified the code from valvano ware, not sure if it's actually 1 ms
void DelayWait1ms(uint32_t n){
	uint32_t volatile time;
	while(n){
		time = 72724*2/91;  // 1msec
		while(time){
			time--;
		}
		n--;
	}
}


//int main(){
//	PLL_Init(Bus80MHz);       // 80  MHz
//	ST7735_InitR(INITR_REDTAB);
//	Serial_Init();
//	MotionDetect_Init();
//	int reply;
//	setMotionDetect(1);
//	//	Serial_OutUDec(reply);
//	//	Serial_OutString("\n\r");
//	Serial_OutString("Motion detection is ");
//	getMotionDetect()? Serial_OutString("ON\n\r") : Serial_OutString("OFF\n\r");
//	
//	for (;;) {
//		// periodically poll motionDetected
//		// will implement this with a timer interrupt
//		if (motionDetected()){
//			Serial_OutString("Motion!");
//			//TVon()
//			// turn TV on and have a timer count down for 30 seconds
//		}
//		DelayWait1ms(500);
//	}
//	
//}

