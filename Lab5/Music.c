
#include <string.h>
#include <stdint.h>
#include "Timer.h"
#include "Music.h"
#include "DAC.h"
#include "ST7735.h"

/***** public variable *****/
int musicPlaying;

/** instruments **/
static const unsigned short Wave[32] = {  
  2048,2360,2660,2937,3179,3378,3526,3617,3648,3617,3526,
  3378,3179,2937,2660,2360,2048,1736,1436,1159,917,718,
  570,479,448,479,570,718,917,1159,1436,1736
};
static const unsigned short Flute[32] = {  
  1007,1252,1374,1548,1698,1797,1825,1797,1675,1562,1383,
  1219,1092,1007,913,890,833,847,810,777,744,674,
  598,551,509,476,495,533,589,659,758,876
};  
static const unsigned short Trumpet[32] = {  
  2013,2179,2318,2397,2135,1567,572,153,1567,3043,2973,
  2353,2135,2074,2170,2126,2022,2030,2091,2126,2022,2022,
  2118,2231,2170,2153,2161,2214,2179,2100,2030,2091
}; 

static const unsigned short Horn[32] = { 
	  1063,1082,1119,1275,1678,1748,1275,755,661,661,703,
	  731,769,845,1039,1134,1209,1332,1465,1545,1427,1588,
	  1370,1086,708,519,448,490,566,684,802,992
}; 

static uint16_t melody_out = 0;
static uint16_t harmony_out = 0;
static long melody_note_duration;     // made it global because of initial note
static long harmony_note_duration;

typedef struct {
	const unsigned short *melody;
	const unsigned short *harmony;
} instrument_comb;  // instrument combination

#define MELODY_NOTE_NUM 8
#define HARMONY_NOTE_NUM 8
struct Note song_melody[] = {
	{C4, quarter}, {D4, quarter}, {E4, quarter}, {F4, quarter},
	{G4, quarter}, {A4, quarter}, {B4, quarter}, {C5, quarter}
};

struct Note song_harmony[] = {
	{F4, quarter}, {G4, quarter}, {A4, quarter}, {Bb4, quarter},
	{C5, quarter}, {D5, quarter}, {E5, quarter}, {F5, quarter}
};

//struct Note song_harmony[] = {
//	{C4, quarter}, {D4, quarter}, {E4, quarter}, {F4, quarter},
//	{G4, quarter}, {A4, quarter}, {B4, quarter}, {C5, quarter}
//};


#define NUMBER_OF_INSTRUMENT_COMBS  4
static instrument_comb instruments[] = {{Wave, Wave}, {Wave, Flute}, {Wave, Trumpet}, {Wave, Horn}};
struct Song song = {60, song_melody, Horn, MELODY_NOTE_NUM, song_harmony, Horn, HARMONY_NOTE_NUM};


static unsigned cur_instrument_comb = 0;  // not init in Music_Init, so that when rewind the instrument does not get reset
static void melody_handler(void);
static void harmony_handler(void);	
static unsigned long getNoteDuration(struct Note note, uint16_t tempo);
static unsigned long getBeatDuration(uint16_t tempo);
	
void Music_Init(void) {
	melody_note_duration = getNoteDuration(*song.melody, song.tempo);
	harmony_note_duration = getNoteDuration(*song.harmony, song.tempo);
	
	musicPlaying = 1;
	Timer2A_Init(&melody_handler, (*song.melody).note);
	Timer3A_Init(&harmony_handler, (*song.harmony).note);
}

/* TO DO
	 figure out how to represent a rest
*/


static void melody_handler(void) {
  static uint16_t instrumentIndex = 0;
	static uint16_t noteIndex = 0;
	static long curDuration = 0;
	
	if (noteIndex >= song.melody_note_num) {  // done playing the song
		// for now, make the music stop when it plays through once
		// need to figure out how to cycle
			DAC_Out(harmony_out);
			return;
	}
			
	melody_out = song.melody_instrument[instrumentIndex];
	uint16_t output = (melody_out + harmony_out) / 2;

	DAC_Out(output);
	curDuration++;
	instrumentIndex = (instrumentIndex + 1) % TABLE_SIZE;
	
	if (curDuration >= melody_note_duration) {  // this note has been played long enough
		// move on to the next note
//		noteIndex = noteIndex + 1;
		noteIndex = (noteIndex + 1);
		instrumentIndex = 0;
		curDuration = 0;
		melody_note_duration = getNoteDuration(song.melody[noteIndex], song.tempo); 		// update the note duration
		Timer2A_Period(song.melody[noteIndex].note);  // change the int period to the new note
	}
	
}

static void harmony_handler(void) {
  static uint16_t instrumentIndex = 0;
	static uint16_t noteIndex = 0;
	static long curDuration = 0;
	
	if (noteIndex >= song.harmony_note_num) {  // done playing the song
		// for now, make the music stop when it plays through once
		// need to figure out how to cycle
			DAC_Out();
			return;
	}
			
	harmony_out = song.harmony_instrument[instrumentIndex];
	uint16_t output = (melody_out + harmony_out) / 2;
	DAC_Out(output);
	curDuration++;
	instrumentIndex = (instrumentIndex + 1) % TABLE_SIZE;
	
	if (curDuration >= harmony_note_duration) {  // this note has been played long enough
		// move on to the next note
		noteIndex = noteIndex + 1;
		instrumentIndex = 0;
		curDuration = 0;
		harmony_note_duration = getNoteDuration(song.harmony[noteIndex], song.tempo); 		// update the note duration
		Timer3A_Period(song.harmony[noteIndex].note);  // change the int period to the new note
	}
}

/*
  Start the music
*/
void Music_Play(void){
	Timer2A_Arm();
	Timer3A_Arm();
	musicPlaying = 1;
}

void Music_Pause(void){
	Timer2A_Disarm();
	Timer3A_Disarm();
	musicPlaying = 0;
}

void Music_Rewind(void) {
	Music_Pause();
	Music_Init();  // Assuming re-init timers will reset the static varaibles inside the handlers
}

void Music_ChangeIntrument(void) {
	cur_instrument_comb = (cur_instrument_comb + 1) % NUMBER_OF_INSTRUMENT_COMBS;
	song.melody_instrument = instruments[cur_instrument_comb].melody;
  song.harmony_instrument = instruments[cur_instrument_comb].harmony;
}

/* 
assume time signature n/4: a beat is quarter-note 
returns the number of periods for one beat given TEMPO of the song
*/
static unsigned long getBeatDuration(uint16_t tempo) {
	// 	TEMPO beats per (minute = 60 * CLOCK_RATE periods) 
	unsigned long periods_per_beat = CLOCK_RATE / tempo * 60;
	return periods_per_beat;
}

/*
	return number of interrupts for the given note length and note freq (period)
*/
static unsigned long getNoteDuration(struct Note note, uint16_t tempo) {
	unsigned long periods_per_beat = getBeatDuration(tempo);
	/*
	whole = 16 = 4 beats
	so LENGTH/4 = number of beats for the note
	so periods of the note = LENGTH/4 * periods_per_beat
	*/
	unsigned long periods_for_note = note.length * periods_per_beat /4;
	return periods_for_note / note.note;   // note.note is the period of interrupt (taken into consideration the TABLE_SIZE)
}


