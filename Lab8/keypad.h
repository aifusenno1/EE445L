// Keypad.h

//INPUT: int attempt that was entered from keypad
//RETURNS: boolean stating if the attempted passcode equals the saved code
int isPassCode(int attempt);


//initializes the ports on the TM4C123 that take input from the keypad
// also should do EdgeInterrupt_Init maybe
void keypadInit(void);
