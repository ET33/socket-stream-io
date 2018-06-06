#ifndef dataStructDefault
#define dataStructDefault

enum{
	MESSAGE,
	AUDIO,
	VIDEO,
	IMAGE,
	ERROR,
	PLAY,
	STOP,
	COMMAND,
	START,
	EXIT
};

typedef struct form{
	int id;
	char description[1024];
}data_unit;

#endif