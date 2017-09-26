

typedef struct stage_t {
	char *options[3];
	int time[3];
	int8_t  highlight;    // index (out of total)l; -1 for none
	int color[5];     
	uint8_t selected;      // 0 for not selected. 1 for selected
} stage;

