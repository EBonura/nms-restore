
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Option_Menu(void);
void	Select_Controls(void);

/**************************************
*          DECLARE DEFINES            *
**************************************/

// LINE COORDINATES

#define	LINE1X1		316-(320)		/* WEAPON SELECTION */
#define	LINE1Y1		40-(120)
#define	LINE1X2		276-(320)
#define	LINE1Y2		64-(120)

#define	LINE2X1		316-(320)		/* WEAPON SELECTION */
#define	LINE2Y1		40-(120)
#define	LINE2X2		356-(320)
#define	LINE2Y2		64-(120)

#define	LINE3X1		200-(320)		/* MOVEMENT */
#define	LINE3Y1		75-(120)
#define	LINE3X2		268-(320)
#define	LINE3Y2		87-(120)

#define	LINE8X1		268-(320)		/* OPTIONS */
#define	LINE8Y1		132-(120)
#define	LINE8X2		298-(320)
#define	LINE8Y2		94-(120)

#define	LINE9X1		352-(320)		/* PAUSE */
#define	LINE9Y1		132-(120)
#define	LINE9X2		330-(320)
#define	LINE9Y2		94-(120)

//

#define	LINE4X1		442-(320)		/* GRAB/RELEASE TYPE A */
#define	LINE4Y1		60-(120)
#define	LINE4X2		364-(320)
#define	LINE4Y2		78-(120)

#define	LINE5X1		442-(320)		/* FIRE 1 TYPE A */
#define	LINE5Y1		76-(120)
#define	LINE5X2		350-(320)
#define	LINE5Y2		87-(120)

#define	LINE6X1		442-(320)		/* FIRE 2 TYPE A */
#define	LINE6Y1		108-(120)
#define	LINE6X2		376-(320)
#define	LINE6Y2		87-(120)

#define	LINE7X1		442-(320)		/* THRUST TYPE A */
#define	LINE7Y1		124-(120)
#define	LINE7X2		363-(320)
#define	LINE7Y2		94-(120)

//

#define	LINE10X1		442-(320)		/* THRUST TYPE B */
#define	LINE10Y1		76-(120)
#define	LINE10X2		376-(320)
#define	LINE10Y2		87-(120)

#define	LINE11X1		442-(320)		/* GRAB/RELEASE TYPE B */
#define	LINE11Y1		108-(120)
#define	LINE11X2		350-(320)
#define	LINE11Y2		87-(120)

UBYTE	*joypad;
SPRITE*	spr_JOYPAD;

UBYTE	inmenu;

UBYTE	*backdrop;
SPRITE*	spr_BACKDROP;

/**************************************
*        SHOW THE TITLE SCREEN        *
**************************************/

void
Game_Options(void)
{


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

	backdrop=LoadFile("../INTRO/NEB.PSX");
	spr_BACKDROP = (SPRITE*)LoadFile("../INTRO/NEB.SPR");
	Init_Spr_List(spr_BACKDROP,(ULONG)backdrop);

	joypad=LoadFile("../INTRO/JOYPAD.PSX");
	spr_JOYPAD = (SPRITE*)LoadFile("../INTRO/JOYPAD.SPR");
	Init_Spr_List(spr_JOYPAD,(ULONG)joypad);

	Init_Star_List();

	selectno=0;
	inmenu=0;

	Cls();

	stardelay=96;

	while (fade!=0)
	{
		if (inmenu==0) Option_Menu();
		if (inmenu==1) Load_High_Scores();
		if (inmenu==2) Save_High_Scores();
		if (inmenu==3) Game_Difficulty();
		if (inmenu==4) Select_Controls();
	}

	VSync(0);
	SetDispMask(OFF);

}

/**************************************
*        PRINT THE OPTION MENU        *
**************************************/

void
Option_Menu(void)
{

	Start_Loop();

	if (((Debounce1 & PaDenter)!=0) && (fade==128))	/* RESTORE: owner's decision, Start or Cross (PaDenter, 360X.H) */
	{
		if (selectno==0)
		{
			Start_Fade_Down(-16);
			SsUtKeyOn(vab,SFX_EXPLOSION2,0,FXPITCH,0,sfxvolume,sfxvolume);
		}

		if (selectno==1)
		{
			inmenu=1;
			selectno=0;
		}

		if (selectno==2)
		{
			inmenu=2;
			selectno=0;
		}

		if (selectno==3)
		{
			inmenu=3;
			selectno=0;
		}

		if (selectno==4)
		{
			inmenu=4;
			selectno=0;
		}
	}

	Print_At(260,96);
	if (selectno==0)
	{
		if ((gamecycle MOD 16)<8) Print_Messl("NEW  GAME");
	}
	else Print_Messl("NEW  GAME");

	Print_At(224,112);
	if (selectno==1)
	{
		if ((gamecycle MOD 16)<8) Print_Messl("LOAD HIGH SCORES");
	}
	else Print_Messl("LOAD HIGH SCORES");

	Print_At(224,128);
	if (selectno==2)
	{
		if ((gamecycle MOD 16)<8) Print_Messl("SAVE HIGH SCORES");
	}
	else Print_Messl("SAVE HIGH SCORES");

	Print_At(256,144);
	if (selectno==3)
	{
		if ((gamecycle MOD 16)<8) Print_Messl("DIFFICULTY");
	}
	else Print_Messl("DIFFICULTY");

	Print_At(260,160);
	if (selectno==4)
	{
		if ((gamecycle MOD 16)<8) Print_Messl("CONTROLS");
	}
	else Print_Messl("CONTROLS");


	Print_Sprite(spr_BACKDROP,320,120,32);

	if (((Debounce1 & PaDLup)!=0) && (selectno!=0))
	{
		selectno--;
	}

	if (((Debounce1 & PaDLdown)!=0) && (selectno!=4))
	{
		selectno++;
	}

	Add_Star();
	Plot_Stars(16);

	End_Loop();

}

/**************************************
*        SELECT GAME CONTROLS         *
**************************************/

void
Select_Controls(void)
{

	Start_Loop();

	Print_Sprite(spr_BACKDROP,320,120,32);
	Print_Sprite(spr_JOYPAD,320,128,253);

	Print_At(264,24);
	Print_Messs("WEAPON SELECTION");
	Draw_Line(LINE1X1,LINE1Y1,LINE1X2,LINE1Y2,0,128,0,255);
	Draw_Line(LINE2X1,LINE2Y1,LINE2X2,LINE2Y2,0,128,0,255);

	Print_At(136,64);
	Print_Messs("MOVEMENT");
	Draw_Line(LINE3X1,LINE3Y1,LINE3X2,LINE3Y2,0,128,0,255);

	Print_At(248,128);
	Print_Messs("OPTIONS");
	Draw_Line(LINE8X1,LINE8Y1,LINE8X2,LINE8Y2,0,128,0,255);

	Print_At(336,128);
	Print_Messs("PAUSE");
	Draw_Line(LINE9X1,LINE9Y1,LINE9X2,LINE9Y2,0,128,0,255);

	if (controltype==0)
	{
		Print_At(448,48);
		Print_Messs("GRAB/RELEASE");
		Draw_Line(LINE4X1,LINE4Y1,LINE4X2,LINE4Y2,0,128,0,255);

		Print_At(448,64);
		Print_Messs("FIRE");
		Draw_Line(LINE5X1,LINE5Y1,LINE5X2,LINE5Y2,0,128,0,255);

		Print_At(448,96);
		Print_Messs("FIRE");
		Draw_Line(LINE6X1,LINE6Y1,LINE6X2,LINE6Y2,0,128,0,255);

		Print_At(448,112);
		Print_Messs("THRUST");
		Draw_Line(LINE7X1,LINE7Y1,LINE7X2,LINE7Y2,0,128,0,255);

	}
	else
	{
		Print_At(448,48);
		Print_Messs("FIRE");
		Draw_Line(LINE4X1,LINE4Y1,LINE4X2,LINE4Y2,0,128,0,255);

		Print_At(448,64);
		Print_Messs("THRUST");
		Draw_Line(LINE10X1,LINE10Y1,LINE10X2,LINE10Y2,0,128,0,255);

		Print_At(448,96);
		Print_Messs("GRAB/RELEASE");
		Draw_Line(LINE11X1,LINE11Y1,LINE11X2,LINE11Y2,0,128,0,255);

		Print_At(448,112);
		Print_Messs("FIRE");
		Draw_Line(LINE7X1,LINE7Y1,LINE7X2,LINE7Y2,0,128,0,255);
	}

	Print_At(274,168);
	if (selectno==0)
	{
		if ((gamecycle MOD 16)<8) Print_Messl("GO BACK");
	}
	else Print_Messl("GO BACK");

	Print_At(224,184);
	if (selectno==1)
	{
		if ((gamecycle MOD 16)<8)
		{
			Print_Messl("CONTROLS : ");
			if (controltype==0) Print_Messl("TYPE A");
			if (controltype==1) Print_Messl("TYPE B");
		}
	}
	else
	{
		Print_Messl("CONTROLS : ");
		if (controltype==0) Print_Messl("TYPE A");
		if (controltype==1) Print_Messl("TYPE B");
	}

	if (((Debounce1 & PaDLup)!=0) && (selectno!=0))
	{
		selectno--;
		SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
	}

	if (((Debounce1 & PaDLdown)!=0) && (selectno!=1))
	{
		selectno++;
		SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
	}

	if ((((Debounce1 & PaDenter)!=0) && (selectno==0)) || ((Debounce1 & PaDback)!=0))	/* RESTORE: owner's decision, Start or Cross on GO BACK, Circle from anywhere on the screen (PaDback, 360X.H) */
	{
		inmenu=0;
		selectno=0;
	}

	if (((Debounce1 & PaDLleft)!=0) && (controltype!=0) && (selectno==1))
	{
		controltype--;
		PAD_FIRE=PaDRleft|PaDRright;	/* SETUP INITIAL FIRE BUTTON (RESTORE: was PaDRleft||PaDRright, which is 1) */
		PAD_THRUST=PaDRdown;	/* SETUP INITIAL THRUST BUTTON */
		PAD_GRAB=PaDRup;		/* SETUP INITIAL SMART BOMB */
	}

	if (((Debounce1 & PaDLright)!=0) && (controltype!=1) && (selectno==1))
	{
		controltype++;
		PAD_FIRE=PaDRup|PaDRdown;	/* SETUP INITIAL FIRE BUTTON */
		PAD_THRUST=PaDRright;	/* SETUP INITIAL THRUST BUTTON */
		PAD_GRAB=PaDRleft;		/* SETUP INITIAL SMART BOMB */
	}

	Add_Star();
	Plot_Stars(16);

	End_Loop();

}

/**************************************
*       SELECT GAME DIFFICULTY        *
**************************************/

void
Game_Difficulty(void)
{

	Start_Loop();

	Print_Sprite(spr_BACKDROP,320,120,32);

	Print_At(274,120);
	if (selectno==0)
	{
		if ((gamecycle MOD 16)<8) Print_Messl("GO BACK");
	}
	else Print_Messl("GO BACK");

	Print_At(224,136);
	if (selectno==1)
	{
		if ((gamecycle MOD 16)<8)
		{
			Print_Messl("DIFFICULTY : ");
			if (difficulty==0) Print_Messl("EASY");
			if (difficulty==1) Print_Messl("NORMAL");
			if (difficulty==2) Print_Messl("HARD");
		}
	}
	else
	{
		Print_Messl("DIFFICULTY : ");
		if (difficulty==0) Print_Messl("EASY");
		if (difficulty==1) Print_Messl("NORMAL");
		if (difficulty==2) Print_Messl("HARD");
	}

	if (((Debounce1 & PaDLup)!=0) && (selectno!=0))
	{
		selectno--;
		SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
	}

	if (((Debounce1 & PaDLdown)!=0) && (selectno!=1))
	{
		selectno++;
		SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
	}

	if ((((Debounce1 & PaDenter)!=0) && (selectno==0)) || ((Debounce1 & PaDback)!=0))	/* RESTORE: owner's decision, Start or Cross on GO BACK, Circle from anywhere on the screen (PaDback, 360X.H) */
	{
		inmenu=0;
		selectno=0;
	}

	if (((Debounce1 & PaDLleft)!=0) && (selectno==1) && (difficulty!=0))
	{
		difficulty--;
	}

	if (((Debounce1 & PaDLright)!=0) && (selectno==1) && (difficulty!=2))
	{
		difficulty++;
	}

	Add_Star();
	Plot_Stars(16);

	End_Loop();

}

