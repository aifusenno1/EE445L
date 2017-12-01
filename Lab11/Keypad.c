// Keypad.c

#include "Keypad.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h" 

#define DATA_WIDTH 16

int lockCode;

void ShortWait(uint32_t n){uint32_t volatile time;
  while(n){
    time = 7272*2/91;  // 11msec
    while(time){
	  	time--;
    }
    n--;
  }
}

// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}

// PE0 is input
// Make PE1, PE2, PE3 output, enable digital I/O, ensure alt. functions off
void PortE_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x10;        // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTE_PCTL_R &= ~0x0000FFFF; // 3) regular GPIO
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // 4) disable analog function on PF2, PF4
  GPIO_PORTE_PUR_R |= 0x00;         // 5) pullup for PE0
  GPIO_PORTE_DIR_R |= 0x0E;         // 5) set direction to output for PE1, PE2, PE3
  GPIO_PORTE_AFSEL_R &= ~0x0F;      // 6) regular port function
  GPIO_PORTE_DEN_R |= 0x0F;         // 7) enable digital port
}


void setPassCode(int code){
	lockCode = code;
}

//INPUT: int attempt that was entered from keypad
//RETURNS: 1 if attempt is the right code ---- 0 if the attempt is incorrect
int isPassCode(int attempt){
	if(attempt == lockCode){
		return 1;
	}
	else{
		return 0;
	}
}

int whichPin(int* pins){
	if(pins[0] == 1) return 1;
	if(pins[1] == 1) return 2;
	if(pins[2] == 1) return 3;
	if(pins[3] == 1) return 4;
	if(pins[4] == 1) return 5;
	if(pins[5] == 1) return 6;
	if(pins[6] == 1) return 7;
	if(pins[7] == 1) return 8;
	if(pins[8] == 1) return 9;
	if(pins[9] == 1) return 10;
	if(pins[10] == 1) return 0;
	if(pins[11] == 1) return 11;
	return -1;
}

void getPins(int* pins){	
	GPIO_PORTE_DATA_R |= 0x0A;									//make the CLKINH pin (PE3) and the SH/LD pin (PE1) high 
	int bitval = 0;
	int byteval = 0;
	int pinValues = 0;
	DelayWait10ms(10);
	bitval = 0;
	byteval = 0;
	GPIO_PORTE_DATA_R ^= 0x02;								//toggle the SH/LD pin to be low
	DelayWait10ms(1);
	GPIO_PORTE_DATA_R ^= 0x02;								//toggle the SH/LD pin to be high
	GPIO_PORTE_DATA_R ^= 0x08;								//toggle CLKINH to be low -- starts the shifting
	for(int i = 0; i < DATA_WIDTH; i++){
		bitval = GPIO_PORTE_DATA_R & 0x01;			//extract PE0
		byteval |= (bitval << ((DATA_WIDTH-1) - i));
		GPIO_PORTE_DATA_R ^= 0x04;							//CLK UP
		ShortWait(100);
		GPIO_PORTE_DATA_R ^= 0x04;							//CLK DOWN
		ShortWait(100);		
	}
	pinValues = byteval ^ 0x0FFFF;
	
	pins[0] = ((pinValues & 0x0008) > 0);		//Pin 1 
	pins[1] = ((pinValues & 0x0080) > 0);		//Pin 2
	pins[2] = ((pinValues & 0x0800) > 0);		//Pin 3
	pins[3] = ((pinValues & 0x0004) > 0);		//Pin 4
	pins[4] = ((pinValues & 0x0040) > 0);		//Pin 5
	pins[5] = ((pinValues & 0x0400) > 0);		//Pin 6
	pins[6] = ((pinValues & 0x0002) > 0);		//Pin 7
	pins[7] = ((pinValues & 0x0020) > 0);		//Pin 8
	pins[8] = ((pinValues & 0x0200) > 0);		//Pin 9
	pins[9] = ((pinValues%2) > 0);					//Pin *
	pins[10] = ((pinValues & 0x0010) > 0);	//Pin 0
	pins[11] = ((pinValues & 0x0100) > 0);	//Pin #
	
	GPIO_PORTE_DATA_R ^= 0x08;								//toggle CLKINH
	
	return;
}


//initializes the ports on the TM4C123 that take input from the keypad
// also should do EdgeInterrupt_Init maybe
void Keypad_Init(void){
		PortE_Init();
}



