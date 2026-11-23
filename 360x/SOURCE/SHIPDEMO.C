
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	TELETYPESHIP	(1+12+10)

// BOX 1 COORDINATES

#define	BOX1POINT1X	48-(320)
#define	BOX1POINT1Y	200-(120)

#define	BOX1POINT2X	144-(320)
#define	BOX1POINT2Y	200-(120)

#define	BOX1POINT3X	144-(320)
#define	BOX1POINT3Y	216-(120)

#define	BOX1POINT4X	48-(320)
#define	BOX1POINT4Y	216-(120)

// BOX 2 COORDINATES

#define	BOX2POINT1X	40-(320)
#define	BOX2POINT1Y	26-(120)

#define	BOX2POINT2X	216-(320)
#define	BOX2POINT2Y	26-(120)

#define	BOX2POINT3X	216-(320)
#define	BOX2POINT3Y	96-(120)

#define	BOX2POINT4X	40-(320)
#define	BOX2POINT4Y	96-(120)

// BOX 2 INBETWEEN

#define	BOX2POINT5X	40-(320)
#define	BOX2POINT5Y	48-(120)

#define	BOX2POINT6X	216-(320)
#define	BOX2POINT6Y	48-(120)

// BOX 3 COORDINATES

#define	BOX3POINT1X	40+352-(320)
#define	BOX3POINT1Y	26-(120)

#define	BOX3POINT2X	248+352-(320)
#define	BOX3POINT2Y	26-(120)

#define	BOX3POINT3X	248+352-(320)
#define	BOX3POINT3Y	88-(120)

#define	BOX3POINT4X	40+352-(320)
#define	BOX3POINT4Y	88-(120)

// BOX 3 INBETWEEN

#define	BOX3POINT5X	392-(320)
#define	BOX3POINT5Y	48-(120)

#define	BOX3POINT6X	600-(320)
#define	BOX3POINT6Y	48-(120)

// BOX 4 FOR WEAPON STATS

#define	BOX4POINT1X	368-(320)
#define	BOX4POINT1Y	200-(120)

#define	BOX4POINT2X	592-(320)
#define	BOX4POINT2Y	200-(120)

#define	BOX4POINT3X	592-(320)
#define	BOX4POINT3Y	216-(120)

#define	BOX4POINT4X	368-(320)
#define	BOX4POINT4Y	216-(120)

/**************************************
*    TEXT FOR DESCRIPTION OF SHIP     *
**************************************/

TEXT shiptext[] = {22, 48+4,199,-1,1,"VIPER ST50",0,

				22, 48, 22,-1,0,"TYPE: CLASS C",0,
				22, 92, 30,-1,0,"STEALTH FIGHTER",0,
				22, 48, 45,-1,0,"MAX SPEED",0,
				22,160, 45,-1,0,"900 KPS",0,
				22, 48, 53,-1,0,"SHIELDING",0,
				22,160, 53,-1,0,"650 UVS",0,
				22, 48, 61,-1,0,"FUEL CAPACITY",0,
				22,160, 61,-1,0,"760 MGL",0,
				22, 48, 69,-1,0,"TOTAL MASS",0,
				22,160, 69,-1,0,"420 T",0,
				22, 48, 77,-1,0,"BOOST FACTOR",0,
				22,160, 77,-1,0,"2:1",0,

				22,400, 22,-1,0,"MAKE: MERCURY",0,
				22,443, 30,-1,0,"STARSHIPS LTD",0,
				22,400, 45,-1,0,"WEAPON MOUNTINGS",0,
				22,536, 45,-1,0,"6",0,
				22,400, 53,-1,0,"ENGINE TYPE",0,
				22,536, 53,-1,0,"PLASMA",0,
				22,400, 61,-1,0,"FUEL TYPE",0,
				22,536, 61,-1,0,"UNLEADED",0,
				22,400, 69,-1,0,"CREW",0,
				22,536, 69,-1,0,"1",0
				};


/**************************************
*      SHOW DESCRIPTION OF LEVEL      *
**************************************/

void
Ship_Description(void)
{

LONG	zoom;

UBYTE	*shipgrid;
SPRITE*	spr_SHIPGRID;

UBYTE	*ship;
SPRITE*	spr_SHIP;

SPRITE*	spr;

UBYTE	used_vram;

USHORT	timer;

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

	shipgrid=LoadFile("../INTRO/SHIPGRD.PSX");
	spr_SHIPGRID = (SPRITE*)LoadFile("../INTRO/SHIPGRD.SPR");
	Init_Spr_List(spr_SHIPGRID,(ULONG)shipgrid);

	ship=LoadFile("../INTRO/OVSHIP2.PSX");
	spr_SHIP = (SPRITE*)LoadFile("../INTRO/OVSHIP2.SPR");
	Add_Spr_Loadoffset(spr_SHIP,(ULONG)ship);

	selectno=0;

	VAB_HADDR=LoadFile("../SOUND/B1_5.VH");
	VAB_BADDR=LoadFile("../SOUND/B1_5.VB");
	soundfx=YES;					/* DO YOU NEED VAB BUFFER? */
	music=YES;					/* DO YOU NEED SEQ BUFFER? */
	InitialiseSound();				/* START THE MUSIC */

	SsUtKeyOn(vab,0,0,65,0,sfxvolume,sfxvolume);

	Init_Teletype();

	zoom=0;
	selectno=0;
	timer=0;

	Cls();

	while (fade!=0)
	{
		if (((Debounce1 & PaDstart)!=0) && (fade==128))
		{
			Start_Fade_Down(-16);
			SsUtKeyOn(vab,SFX_EXPLOSION2,0,FXPITCH,0,sfxvolume,sfxvolume);
			selectno=INTOTITLE;
		}

		Start_Loop();

		if ((gamecycle MOD 200)==0) timer++;
		if ((gamecycle==1000) && (fade==128))
		{
			Start_Fade_Down(-16);
			selectno=INTOHIGH;
		}

		Print_Sprite(spr_SHIPGRID,320,240,200);

		sprctrl.green=96;

		if (zoom!=4096) zoom+=32;

		sprctrl.scalex=zoom;
		sprctrl.scaley=zoom;

		spr=Find_Spr(spr_SHIP,(gamecycle MOD 128)/2);
		Print_Sprite(spr,320,128,230);

		sprctrl.scalex=4096;
		sprctrl.scaley=4096;

		sprctrl.green=128;

		sprctrl.red=0;
		sprctrl.green=96;
		sprctrl.blue=0;

		if (timer==1)
		{
			ship[2]=0x1F;
			ship[3]=0X80;

			Print_At(376,199);
			Print_Messm("360 DEGREE THERMITE SHELL");

			Draw_Line(BOX4POINT1X,BOX4POINT1Y,BOX4POINT2X,BOX4POINT2Y,0,128,0,254);
			Draw_Line(BOX4POINT2X,BOX4POINT2Y,BOX4POINT3X,BOX4POINT3Y,0,128,0,254);
			Draw_Line(BOX4POINT3X,BOX4POINT3Y,BOX4POINT4X,BOX4POINT4Y,0,128,0,254);
			Draw_Line(BOX4POINT4X,BOX4POINT4Y,BOX4POINT1X,BOX4POINT1Y,0,128,0,254);
		}
		else
		{
			ship[2]=0x20;
			ship[3]=0x83;
		}

		if (timer==2)
		{
			ship[4]=0x1F;
			ship[5]=0x80;

			Print_At(376,199);
			Print_Messm("DYNAMIC RELECTOR SHIELDS");

			Draw_Line(BOX4POINT1X,BOX4POINT1Y,BOX4POINT2X,BOX4POINT2Y,0,128,0,254);
			Draw_Line(BOX4POINT2X,BOX4POINT2Y,BOX4POINT3X,BOX4POINT3Y,0,128,0,254);
			Draw_Line(BOX4POINT3X,BOX4POINT3Y,BOX4POINT4X,BOX4POINT4Y,0,128,0,254);
			Draw_Line(BOX4POINT4X,BOX4POINT4Y,BOX4POINT1X,BOX4POINT1Y,0,128,0,254);
		}
		else
		{
			ship[4]=0x20;
			ship[5]=0x83;
		}

		if (timer==3)
		{
			ship[6]=0x1F;
			ship[7]=0x80;

			Print_At(376,199);
			Print_Messm("QUANTUM PROPULSION UNITS");

			Draw_Line(BOX4POINT1X,BOX4POINT1Y,BOX4POINT2X,BOX4POINT2Y,0,128,0,254);
			Draw_Line(BOX4POINT2X,BOX4POINT2Y,BOX4POINT3X,BOX4POINT3Y,0,128,0,254);
			Draw_Line(BOX4POINT3X,BOX4POINT3Y,BOX4POINT4X,BOX4POINT4Y,0,128,0,254);
			Draw_Line(BOX4POINT4X,BOX4POINT4Y,BOX4POINT1X,BOX4POINT1Y,0,128,0,254);
		}
		else
		{
			ship[6]=0x20;
			ship[7]=0x83;
		}

		if (timer==4)
		{
			ship[8]=0x1F;
			ship[9]=0x80;

			Print_At(376,199);
			Print_Messm("PLASMA CORE PULSE BEAMS");

			Draw_Line(BOX4POINT1X,BOX4POINT1Y,BOX4POINT2X,BOX4POINT2Y,0,128,0,254);
			Draw_Line(BOX4POINT2X,BOX4POINT2Y,BOX4POINT3X,BOX4POINT3Y,0,128,0,254);
			Draw_Line(BOX4POINT3X,BOX4POINT3Y,BOX4POINT4X,BOX4POINT4Y,0,128,0,254);
			Draw_Line(BOX4POINT4X,BOX4POINT4Y,BOX4POINT1X,BOX4POINT1Y,0,128,0,254);
		}
		else
		{
			ship[8]=0x20;
			ship[9]=0x83;
		}

		if (teletypenumber>0)
		{
			Draw_Line(BOX1POINT1X,BOX1POINT1Y,BOX1POINT2X,BOX1POINT2Y,0,128,0,254);
			Draw_Line(BOX1POINT2X,BOX1POINT2Y,BOX1POINT3X,BOX1POINT3Y,0,128,0,254);
			Draw_Line(BOX1POINT3X,BOX1POINT3Y,BOX1POINT4X,BOX1POINT4Y,0,128,0,254);
			Draw_Line(BOX1POINT4X,BOX1POINT4Y,BOX1POINT1X,BOX1POINT1Y,0,128,0,254);
		}

		if (teletypenumber>2)
		{
			Draw_Line(BOX2POINT1X,BOX2POINT1Y,BOX2POINT2X,BOX2POINT2Y,0,128,0,254);
			Draw_Line(BOX2POINT2X,BOX2POINT2Y,BOX2POINT3X,BOX2POINT3Y,0,128,0,254);
			Draw_Line(BOX2POINT3X,BOX2POINT3Y,BOX2POINT4X,BOX2POINT4Y,0,128,0,254);
			Draw_Line(BOX2POINT4X,BOX2POINT4Y,BOX2POINT1X,BOX2POINT1Y,0,128,0,254);

			Draw_Line(BOX2POINT5X,BOX2POINT5Y,BOX2POINT6X,BOX2POINT6Y,0,128,0,254);
		}

		if (teletypenumber>14)
		{
			Draw_Line(BOX3POINT1X,BOX3POINT1Y,BOX3POINT2X,BOX3POINT2Y,0,128,0,254);
			Draw_Line(BOX3POINT2X,BOX3POINT2Y,BOX3POINT3X,BOX3POINT3Y,0,128,0,254);
			Draw_Line(BOX3POINT3X,BOX3POINT3Y,BOX3POINT4X,BOX3POINT4Y,0,128,0,254);
			Draw_Line(BOX3POINT4X,BOX3POINT4Y,BOX3POINT1X,BOX3POINT1Y,0,128,0,254);

			Draw_Line(BOX3POINT5X,BOX3POINT5Y,BOX3POINT6X,BOX3POINT6Y,0,128,0,254);
		}

		Print_Teletype(shiptext,TELETYPESHIP);

		sprctrl.red=128;
		sprctrl.green=128;
		sprctrl.blue=128;

		End_Loop();
	}

	VSync(0);
	SetDispMask(OFF);

}

