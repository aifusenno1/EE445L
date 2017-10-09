
// note periods = 80000000 / (freq * TABLE_SIZE)
// TABLE_SIZE = 32
#define C2 38223   // 65.406 Hz
#define Db2 36077   // 69.296 Hz
#define D2 34052   // 73.416 Hz
#define Eb2 32141   // 77.782 Hz
#define E2 30337   // 82.407 Hz
#define F2 28635   // 87.307 Hz
#define Gb2 27027   // 92.499 Hz
#define G2 25511   // 97.999 Hz
#define Ab2 24079   // 103.826 Hz
#define A2 22727   // 110.000 Hz
#define Bb2 21452   // 116.541 Hz
#define B2 20248   // 123.471 Hz
#define C3 19111   // 130.813 Hz
#define Db3 18039   // 138.591 Hz
#define D3 17026   // 146.832 Hz
#define Eb3 16071   // 155.563 Hz
#define E3 15169   // 164.814 Hz
#define F3 14317   // 174.614 Hz
#define Gb3 13514   // 184.997 Hz
#define G3 12755   // 195.998 Hz
#define Ab3 12039   // 207.652 Hz
#define A3 11364   // 220.000 Hz
#define Bb3 10726   // 233.082 Hz
#define B3 10124   // 246.942 Hz
#define C4 9556   // 261.626 Hz
#define Db4 9019   // 277.183 Hz
#define D4 8513   // 293.665 Hz
#define Eb4 8035   // 311.127 Hz
#define E4 7584   // 329.628 Hz
#define F4 7159   // 349.228 Hz
#define Gb4 6757   // 369.994 Hz
#define G4 6378   // 391.995 Hz
#define Ab4 6020   // 415.305 Hz
#define A4 5682   // 440.000 Hz
#define Bb4 5363   // 466.164 Hz
#define B4 5062   // 493.883 Hz
#define C5 4778   // 523.251 Hz
#define Db5 4510   // 554.365 Hz
#define D5 4257   // 587.330 Hz
#define Eb5 4018   // 622.254 Hz
#define E5 3792   // 659.255 Hz
#define F5 3579   // 698.456 Hz
#define Gb5 3378   // 739.989 Hz
#define G5 3189   // 783.991 Hz
#define Ab5 3010   // 830.609 Hz
#define A5 2841   // 880.000 Hz
#define Bb5 2681   // 932.328 Hz
#define B5 2531   // 987.767 Hz
#define C6 2389   // 1046.502 Hz
#define Db6 2255   // 1108.731 Hz
#define D6 2128   // 1174.659 Hz
#define Eb6 2009   // 1244.508 Hz
#define E6 1896   // 1318.510 Hz
#define F6 1790   // 1396.913 Hz
#define Gb6 1689   // 1479.978 Hz
#define G6 1594   // 1567.982 Hz
#define Ab6 1505   // 1661.219 Hz
#define A6 1420   // 1760.000 Hz
#define Bb6 1341   // 1864.655 Hz
#define B6 1265   // 1975.533 Hz
#define C7 1194   // 2093.005 Hz

// note length
#define whole 16
#define half 8
#define quarter 4
#define eighth 2
#define sixteenth 1


#define MAX_SONG_NOTES 128
#define TABLE_SIZE 32
#define CLOCK_RATE 80000000

struct Note {
	unsigned short note;		// the note period value  defined
	unsigned short length;	// number of beats for the note
};

struct Song {
	unsigned short tempo;		//beats per minute
	
	struct Note *melody;
	const unsigned short *melody_instrument;
	unsigned short melody_note_num;
	
	struct Note *harmony;
	const unsigned short *harmony_instrument;
	unsigned short harmony_note_num;

};

extern int musicPlaying;

void Music_Init(void);
void Music_Play(void);
void Music_Pause(void);
void Music_Rewind(void);
void Music_ChangeIntrument(void);
