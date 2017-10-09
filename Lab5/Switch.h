


	

static void Timer1Arm(void);
// Interrupt 10 ms after rising edge of PF4
void Timer1A_Handler(void);

void GPIOPortF_Handler(void);

/* initialize PF4 for interrupt */
void EdgeInterrupt_Init(void);



void PortF_Init(void);

