
#ifndef	__H_GENERAL
#define	__H_GENERAL

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Initialise_Malloc();		/* INITIALISE MALLOC AT START OF GAME */
void	Rts(void);				/* VOID ROUTINE FOR QUITTING */
void	Flicker_Line();			/* FLICKER RASTER FOR FRAME TIME INDICATION */
void	Solid_Line();				/* SOLID RASTER FOR FRAME TIME INDICATION */
void	Read_Pads(void);			/* READ AND DEBOUNCE THE PADS */
void	Draw_Line(USHORT x,USHORT y,USHORT x1,USHORT y1,BYTE red,BYTE green,BYTE blue,BYTE zed);
void	Clear_Glow_Lines(void);		/* RESTORE: THE LASER'S BEAM (GENERAL.C) */
void	Add_Glow_Line(LONG x,LONG y,LONG width);
void	Add_Ring(LONG radius,LONG start_x,LONG start_y);	/* RESTORE: THE RING WEAPON'S DISC (GENERAL.C) */
void	Draw_Glow_Lines(void);
void	Draw_Pod_Rope(void);		/* RESTORE: THE POD'S ROPE (GENERAL.C) */
void	Draw_GLine(USHORT x,USHORT y,USHORT x1,USHORT y1,
					BYTE red0,BYTE green0,BYTE blue0,
					BYTE red1,BYTE green1,BYTE blue1,BYTE zed);

void	Show_VRAM(void);			/* SHOW STATUS OF THE VIDRAM */
void	Print_Hex(ULONG);			/* PRINT VARIABLE IN HEX */
void	Print_Decs(LONG);			/* PRINT VARIABLE IN DECIMAL */
void	Print_Decm(LONG);			/* PRINT VARIABLE IN DECIMAL */
void	Print_Decl(LONG);			/* PRINT VARIABLE IN DECIMAL */
void	Resident_Tims(void);		/* SETUP RESIDENT TIMS */
void	Start_Fade_Up(UBYTE speed);		/* START THE SCREEN FADING UP */
void	Start_Fade_Down(UBYTE speed);		/* START THE SCREEN FADING DOWN */
void	Update_Fade(void);			/* UPDATE THE FADE ON THE VBL */
void	Cls(void);				/* CLEAR THE SCREEN AND START FADING UP */
LONG	Random(ULONG size);			/* GET A RANDOM NUMBER */
BYTE*	MyMalloc(ULONG);			/* MALLOC AN AREA OF MEMORY */
void	MyFree(UBYTE*);			/* DE-ALLOCATE AN AREA OF MEMORY */
void	MallocTest(void);			/* TEST THE MALLOC ROUTINE */
void	Print_Messs(const UBYTE *);		/* PRINT SMALL FONT */
void	Print_Messm(const UBYTE *);		/* PRINT MEDIUM FONT */
void	Print_Messm_Centred(int cx,int y,const UBYTE *);	/* RESTORE: Print_Messm CENTRED ON x cx (GENERAL.C) */
void	Print_Messl(const UBYTE *);		/* PRINT LARGE FONT */
void	Print_At(USHORT,USHORT);		/* MOVE PRINT CURSOR TO X,Y COORDINATE */
void	Init_Screen320x240(void);		/* INITIALISE SCREEN 320X240 */
void	Init_Screen640x240(void);		/* INITIALISE SCREEN 640X480 */
SHORT	GetSin(SHORT angle);		/* 0-1023  one quarter==256*/
SHORT	GetCos(SHORT angle);		/* 0-1023 */
void	Process_Menus(void);
void	Check_Pause_Menu(void);
void	Loading_Message(void);		/* RESTORE: PLEASE WAIT ON THE SCREEN WHILE A SCREEN LOADS */

/**************************************
*           DECLARE DEFINES           *
**************************************/

#define 	LETTERBOX_X	(1)			/* LETTERBOX X FOR PAL VERSION */
#define 	LETTERBOX_Y	(28)			/* LETTERBOX Y FOR PAL VERSION */

#define	STARTPANELX	(280)		/* START COORDINATES FOR PANEL */
#define	STARTPANELY	(84)

/**************************************
*          DECLARE VARIABLES          *
**************************************/

extern	ControllerPacket	pad1,pad2;

extern	UBYTE	finalmaster;		/* IF 1 THEN FINAL GAME */
extern	UBYTE	cdmaster;			/* 0=HARD DRIVE, 1=CD  */
extern	UBYTE	pal;				/* iF 1 THEN PAL NOT NTSC */
extern	UBYTE	multilanguage;		/* NO=ENGLISH, YES=FRENCH & GERMAN */

extern	UBYTE	language;			/* 0,1 OR 2 FOR LANGUAGE */

extern	UBYTE	showraster;		/* IF 1 THEN SHOW RASTER */
extern	UBYTE	showvram;			/* IF 1 THEN SHOW VRAM LAYOUT */
extern	UBYTE	testmessages;		/* IF 1 THEN PRINT TEST MESSAGES */
extern	UBYTE	pollhost;			/* IF 1 THEN POLLHOST IS ON */

extern	UBYTE	sfxvolume;		/* 0-127 FOR SFX VOLUME */
extern	UBYTE	musvolume;		/* 0-127 FOR MUSIC VOLUME */
extern	UBYTE	channels;			/* 0=STEREO, 1=MONO */
extern	UBYTE	difficulty;		/* 0-2 FOR DIFFICULTY */

extern	UBYTE	panelposition;		/* POSITION OF PANEL 0-4 */
extern	USHORT	panelx;			/* X COORDINATE OF PANEL */
extern	USHORT	panely;			/* Y COORDINATE OF PANEL */

extern	UBYTE	controltype;		/* 0=OPTION 1, 1=OPTION 2 */
extern	UBYTE	selectno;			/* SELECTION NUMBER CURRENTLY ON */

extern	UBYTE	level;			/* CURRENT LEVEL NUMBER */
extern	UBYTE	gameover;			/* YES=GAMEOVER */

extern	UBYTE	*filename;		/* POINTER TO FILENAME */

extern	USHORT	Pad1;			/* NON-DEBOUNCED PAD0 DATA */
extern	USHORT	Debounce1;		/* DEBOUNCED PAD0 DATA */
extern	USHORT	Pad2;			/* NON-DEBOUNCED PAD1 DATA */
extern	USHORT	Debounce2;		/* DEBOUNCED PAD1 DATA */

extern	USHORT	PAD_FIRE;			/* CURRENT PAD FIRING BUTTON VALUE */
extern	USHORT	PAD_THRUST;		/* CURRENT PAD THRUST BUTTON VALUE */
extern	USHORT	PAD_GRAB;			/* CURRENT PAD GRAB BUTTON VALUE */
extern	UBYTE	quittomenu;			/* RESTORE: owner's decision, SEE GENERAL.C */
extern	SHORT	help_cycles;			/* RESTORE: SEE GENERAL.C */
extern	UBYTE	help_shown;
void	Add_Polo(LONG r1,LONG r2,LONG start_x,LONG start_y);	/* RESTORE: THE PC'S add_polo (GENERAL.C) */
void	Text_Box_Begin(void);			/* RESTORE: NEW WORK, THE MENU BOX (GENERAL.C) */
void	Text_Box_End(void);
void	Text_Box_Grow(SHORT x,SHORT y,SHORT w,SHORT h);
extern	UBYTE	textbox_on;
void	Print_Controls_Help(int cx,int y);		/* RESTORE: THE CONTROL HELP (GENERAL.C) */
void	Draw_Pause_Screen(UBYTE paused);			/* RESTORE: THE PAUSE SCREEN (GENERAL.C) */
void	Pad_Icons_Into_Fonts(void);		/* RESTORE: THE PAD BUTTON ICONS (GENERAL.C) */
void	Pad_Icons_Init(void);

extern	UBYTE	fade;			/* FADE VALUE FOR SCREEN */
extern	UBYTE	fadespd;			/* SPEED FOR FADING UP AND DOWN */

extern	ULONG	sonymalloc;		/* GET START MALLOC FROM SONY LIBRARIES */
extern	ULONG	mallocused;		/* NUMBER OF MALLOCS USED */
extern	USHORT	maxmalloc;		/* MAXIMUM MEMORY AVAILABLE */

extern	USHORT	messx;			/* PRINT CURSOR X COORD */
extern	USHORT	messy;			/* PRINT CURSOR Y COORD */

extern	BYTE		gamma[9];			/* GAMMA CORRECTION FOR BACKGROUND */

extern	UBYTE	whichmenu;		/* 0=NULL, WHICH MENU ARE YOU IN */
extern	UBYTE	menuselection;		/* WHICH OPTION IN MENU ARE YOU ON */
extern	UBYTE	testsoundfx;		/* NUMBER OF SOUND TO TEST */

extern	UBYTE	pause;			/* 0=NULL, OTHERWISE PAUSED */

extern	USHORT	screenwidth;		/* WIDTH OF CURRENT SCREEN */
extern	USHORT	screenheight;		/* HEIGHT OF CURRENT SCREEN */

extern	ULONG	score;			/* CURRENT GAME SCORE */
extern	ULONG	scoreadder;		/* SCORE LEFT TO ADD */

extern	GsLINE	line;			/* DATA FOR LINE COMMAND */
extern	GsGLINE	gline;

#endif

