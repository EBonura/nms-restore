
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	TEXTTOP	(-16)			/* TOP BORDER FOR TEXT */
#define	TEXTBOT	(240)			/* BOTTOM BORDER FOR TEXT */

/**************************************
*       DECLARE STRUCTURES            *
**************************************/

typedef struct	{
				UBYTE	fonttype;
				SHORT	x;
				SHORT	y;
				UBYTE	*textstuff;
				}CREDITS;

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Print_Credits(void);

/**************************************
*          DECLARE VARIABLES          *
**************************************/

USHORT	credittime;

/**************************************
*       MAIN CREDITS FOR 360X         *
**************************************/

CREDITS credits[]={1,35*8, 10,"CREDITS",
				1,30*8, 50,"FOR NMS SOFTWARE",
				0,30*8, 90,"PROGRAMMING",
				0,30*8,120,"PAUL GRIFFITHS",
				0,30*8,140,"PRATIK PATEL",
				0,30*8,160,"PHILLIP TRELFORD",
				1,30*8,190,"ARTISTS",
				0,30*8,220,"IAN ",
				0,30*8,240,"COLIN ",
				1,30*8,270,"SOUND AND MUSIC",
				0,30*8,300,"THE MUSIC BODS",
				1,30*8,330,"MANAGEMENT",
				0,30*8,360,"RICHARD CHAPPELLS",
				0,30*8,390,"STEVE BRUCE"};

/**************************************
*       PRINT MAIN GAME CREDITS       *
**************************************/

void 
Display_Credits(void)
{

UBYTE	*title;
SPRITE*	spr_TITLE;

SHORT	dropout;

USHORT	j;

UBYTE	used_vram;

	Jalloc_Init(&JALLOC,sonymalloc,(maxmalloc*1024)); /* INITIALISE NEW MALLOC ROUTINE */

	Init_TPages();
	Reset_Cluts();
	Resident_Tims();
	Loading_Message();			/* RESTORE: owner's decision, PLEASE WAIT while this screen loads (GENERAL.C) */

	used_vram=Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);

	Xor_VRAM_Hole(((0+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((1+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((2+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((3+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((4+11)<<16)+(used_vram & 0x00ff0000),32,28);

	title=LoadFile("../INTRO/HIGH.PSX");
	spr_TITLE = (SPRITE*)LoadFile("../INTRO/HIGH.SPR");
	Init_Spr_List(spr_TITLE,(ULONG)title);

	Init_Star_List();

	selectno=0;
	credittime=0;

	dropout=60*5;

	Cls();

	while (fade!=0)
	{

		if (((Debounce1 & PaDstart)!=0) && (fade==128))
		{
			Start_Fade_Down(-16);
			SsUtKeyOn(vab,SFX_EXPLOSION2,0,FXPITCH,0,sfxvolume,sfxvolume);
		}

		Start_Loop();

		Print_Sprite(spr_TITLE,320,240,64);

		Add_Star();
		Plot_Stars(16);

		Print_Credits();
		End_Loop();

	}

	selectno=INTOTITLE;

	VSync(0);
	SetDispMask(OFF);

}

/**************************************
*     PRINT THE CREDITS ON SCREEN     *
**************************************/

void 
Print_Credits(void)
{

USHORT	j;

		credittime++;
		if (credittime>1000 && (fade==128)) Start_Fade_Down(-16);

		for (j=0;j<sizeof(credits)/sizeof(CREDITS);j++)
		{
			if (((256+(credits[j].y)-credittime)>TEXTTOP)
				&& ((256+(credits[j].y)-credittime)<TEXTBOT))
			{
				Print_At(((credits[j].x)+16),(256+(credits[j].y)-credittime));
				if (credits[j].fonttype==0)
					Print_Messm(credits[j].textstuff);
				else
					Print_Messl(credits[j].textstuff);
			}
		}
}

