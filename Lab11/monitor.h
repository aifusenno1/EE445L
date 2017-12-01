// Monitor.h

/*
	This one is likely going to be removed.
	If we successfully figure out the camera using TM4C, then will directly control
	the video signal with the camera module (in MotionDetect)
	If not, we will use Arduino, then we use one pin from the Arduino to control the monitor.
	In both case, this module is not required.
*/

//lets the feed from the camera into the video cable
void monitorOn(void);

//cuts the feed from the display
void monitorOff(void);

void monitorInit(void);

