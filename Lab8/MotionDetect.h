/*
	Motion Detection controls Monitor
	Here are the user functions
*/


int MotionDetect_Init(void);
int getMotionDetect(void);
int motionDetected(void);
int setMotionDetect(uint8_t flag);
int setMotionStatus(uint8_t x, uint8_t d1, uint8_t d2);
int reset(void);
int TVon(void);
int TVoff(void);

