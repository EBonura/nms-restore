typedef struct
{
	short numsprites;
}tSaveHeader;


typedef struct
{
	char name[9];	// NULL terminated filename NO extension
	short x,y;			// position in map
	short startframe;	// initial frame to be displayed
	short playtype;	// 0=LOOP 1=PING-PONG
}tSaveInfo;

