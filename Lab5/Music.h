

//BASE FREQUENCY
#define B	 	494
#define Bb	466
#define A		440
#define Ab	415
#define G		392
#define Gb	370
#define F		349
#define E		330
#define Eb	311
#define D		294
#define Db	277
#define C		262

#define MAX_SONG_NOTES 1000
#define TABLE_SIZE 32

struct note{
	int tempo;								//given in beats per minute to have a starting tempo for the song -- may get changed by buttons or something?
	//int numNotes;
	double noteLength;	//double so it can be 1 for full note or .5 for half note etc
	char notePitch[3];	//2 chars in each note for things like C 3 or Ab5 etc.
};

int getFrequency(char note[3]);