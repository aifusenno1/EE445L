// Microphone.h
// Paris Kaman
// Allen Pan
/*
	Microphone uses ADC to sample the input
	ADC sampling rate is 44100kHz
*/
#include <stdint.h>

extern uint32_t voiceVal; // sampled by ADC
void Microphone_Init(void);

