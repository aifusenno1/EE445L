/* Button. h*/

#define PF4                     (*((volatile uint32_t *)0x40025040))
#define PF3                     (*((volatile uint32_t *)0x40025020))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF0                     (*((volatile uint32_t *)0x40025004))
	

void Button_Init(void); 

extern int doorBell;