// Monitor.h

/*
	This one is likely to be removed.
	If we successfully figure out the camera using TM4C, then will directly control
	the video signal with the camera module (in MotionDetect)
	If not, we will use Arduino, then we will see if it is easy to control the monitor.
	If not, we will just leave the monitor on all the time.
*/

//lets the feed from the camera into the video cable
void monitorOn(void);

//cuts the feed from the display
void monitorOff(void);

void monitorInit(void);