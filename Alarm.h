//Paris Kaman
// Allen Pan
// Alarm.c
// determines whether the alarm needs to go off or not

// ***************** Timer2_Init ****************
// Activate Timer2 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer2_Init(void);

void Timer2A_Handler(void);

int setAlarm(int time);

void alarmOn(void);

void alarmOff(void);

int checkForAlarm(int time);

