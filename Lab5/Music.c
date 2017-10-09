
#include "Music.h"
#include <string.h>



int getFrequency(char note[3]){
	double base = 0;
	char name[2] = {"  "};
	name[0] = note[0];
	name[1] = note[1];
	int octave = note[2] - '0';
	
	if(strcmp(name, "B ") == 0){
		base = B;
	}
	else if(strcmp(name, "Bb") == 0){
		base = Bb;
	}
	else if(strcmp(name, "A ") == 0){
		base = A;
	}
	else if(strcmp(name, "Ab") == 0){
		base = Ab;
	}
	else if(strcmp(name, "G ") == 0){
		base = G;
	}
	else if(strcmp(name, "Gb") == 0){
		base = Gb;
	}
	else if(strcmp(name, "F ") == 0){
		base = F;
	}
	else if(strcmp(name, "E ") == 0){
		base = E;
	}
	else if(strcmp(name, "Eb") == 0){
		base = Eb;
	}
	else if(strcmp(name, "D ") == 0){
		base = D;
	}
	else if(strcmp(name, "Db") == 0){
		base = Db;
	}
	else if(strcmp(name, "C ") == 0){
		base = C;
	}
	
	int shifts = octave - 4;		// 4 is the base octave all the frequencies are listed as
	if(shifts == 0){
		return base;
	}
	else if(shifts > 0){
		return ((int)base << shifts);
	}
	else{
		return ((int)base >> (-shifts));
	}
}
