//Motor.h
//Pan
//Kaman

#include <stdint.h>

void Motor_Init(void);

void PWM_Duty(uint16_t duty);

void Motor_Off(void);

extern int32_t desiredSpeed;	// in 0.1 rps
