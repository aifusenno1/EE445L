//Paris Kaman
// Allen Pan
// Alarm.c
// determines whether the alarm needs to go off or not

static int alarmTime = (13 * 3600);

int setAlarm(int time){
	alarmTime = time;
	return 1;
}

int checkForAlarm(int time){
	if(time == alarmTime){
		return 1;
	}
	return 0;
}

void playAlarm(void){
	return; 
}



