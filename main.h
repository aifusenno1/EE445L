

typedef struct stage_t {
	uint8_t stageNum;
	char *options[3];
	uint8_t optionsLen;
	int time[3];
	uint8_t timeLen;
	uint8_t totalLen;     // concat options with time
	int8_t  highlight;    // index (out of total)l; -1 for none
	int color[5];     
	uint8_t selected;      // 0 for not selected. 1 for selected
} stage;

