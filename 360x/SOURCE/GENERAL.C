
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE VARIABLES          *
**************************************/

ControllerPacket	pad1,pad2;

UBYTE	finalmaster;				/* IF 1 THEN FINAL GAME */
UBYTE	cdmaster;					/* 0=HARD DRIVE, 1=CD */
UBYTE	pal;						/* IF 1 THEN PAL NOT NTSC */
UBYTE	multilanguage;				/* NO=ENGLISH, YES=FRENCH & GERMAN */

UBYTE	language;					/* 0,1 OR 2 FOR LANGUAGE */

UBYTE	showraster;				/* IF 1 THEN SHOW RASTER */
UBYTE	showvram;					/* IF 1 THEN SHOW VRAM LAYOUT */
UBYTE	testmessages;				/* IF 1 THEN PRINT TEST MESSAGES */
UBYTE	pollhost;					/* IF 1 THEN POLLHOST IS ON */

UBYTE	sfxvolume;				/* 0-127 FOR SFX VOLUME */
UBYTE	musvolume;				/* 0-127 FOR MUSIC VOLUME */
UBYTE	channels;					/* 0=STEREO, 1=MONO */
UBYTE	difficulty;				/* 0-2 FOR DIFFICULTY */

UBYTE	panelposition;				/* POSITION OF PANEL 0-4 */
USHORT	panelx;					/* X COORDINATE OF PANEL */
USHORT	panely;					/* Y COORDINATE OF PANEL */

UBYTE	controltype;				/* 0=OPTION 1, 1=OPTION 2 */
UBYTE	selectno;					/* SELECTION NUMBER CURRENTLY ON */

UBYTE	level;					/* CURRENT LEVEL NUMBER */
UBYTE	gameover;					/* YES=GAMEOVER */

UBYTE	*filename;				/* POINTER TO FILENAME */

USHORT	Pad1;					/* NON-DEBOUNCED PAD0 DATA */
USHORT	Debounce1;				/* DEBOUNCED PAD0 DATA */
USHORT	Pad2;					/* NON-DEBOUNCED PAD1 DATA */
USHORT	Debounce2;				/* DEBOUNCED PAD1 DATA */

USHORT	PAD_FIRE;					/* CURRENT PAD FIRING BUTTON VALUE */
USHORT	PAD_THRUST;				/* CURRENT PAD THRUST BUTTON VALUE */
USHORT	PAD_GRAB;					/* CURRENT PAD GRAB BUTTON VALUE */
UBYTE	quittomenu;				/* RESTORE: owner's decision 2026-11-18: QUIT ON THE PAUSE MENU GOES TO THE MAIN MENU (MAIN.C) */
SHORT	help_cycles;				/* RESTORE: owner's decision 2026-11-18, GAME CYCLES LEFT TO SHOW THE CONTROL HELP (MAIN.C) */
UBYTE	help_shown;				/* RESTORE: THE HELP IS SHOWN ONCE, AT THE FIRST MISSION */

UBYTE	fade;					/* FADE VALUE FOR SCREEN */
UBYTE	fadespd;					/* SPEED FOR FADING UP AND DOWN */

ULONG	sonymalloc;				/* GET START MALLOC FROM SONY LIBRARIES */
ULONG	mallocused;				/* NUMBER OF MALLOCS USED */
USHORT	maxmalloc;				/* MAXIMUM MEMORY AVAILABLE */

USHORT	messx;					/* PRINT CURSOR X COORD */
USHORT	messy;					/* PRINT CURSOR Y COORD */

BYTE	gamma[9];					/* GAMMA CORRECTION FOR BACKGROUND */

UBYTE	whichmenu;				/* 0=NULL, WHICH MENU ARE YOU IN */
UBYTE	menuselection;				/* WHICH OPTION IN MENU ARE YOU ON */
UBYTE	testsoundfx;				/* NUMBER OF SOUND TO TEST */

UBYTE	pause;					/* 0=NULL, OTHERWISE PAUSED */

USHORT	screenwidth;				/* WIDTH OF CURRENT SCREEN */
USHORT	screenheight;				/* HEIGHT OF CURRENT SCREEN */

ULONG	score;					/* CURRENT GAME SCORE */
ULONG	scoreadder;				/* SCORE LEFT TO ADD */

GsLINE	line;					/* DATA FOR LINE COMMAND */
GsGLINE	gline;					/* DATA FOR LINE COMMAND */

/**************************************
*     INITIALISE MALLOC ROUTINE       *
**************************************/

void
Initialise_Malloc()
{

	sonymalloc=0;					/* RESET THE NEW MALLOC */
	mallocused=0;					/* RESET MALLOCS USED */

	sonymalloc=malloc(maxmalloc*1024);		/* TRY TO MALLOC ALL MEMORY */
	if (sonymalloc==0)				/* IF FAILED THEN HANG */
	{
		printf("Main malloc has failed\n");
		CRASH;
	}

	Jalloc_Init(&JALLOC,sonymalloc,(maxmalloc*1024)); /* INITIALISE NEW MALLOC ROUTINE */

}

/**************************************
*      GENERAL QUIT SUBROUTINE        *
**************************************/

void
Rts()
{
}

/**************************************
*      FLICKER LINE FOR TESTING       *
**************************************/

void
Flicker_Line()
{

BYTE	j;

	for (j=0;j<50;j++)
	{
		SetDispMask(0);			/* 1=ENABLE DISPLAY, 0=DISABLE */
		SetDispMask(1);			/* 1=ENABLE DISPLAY, 0=DISABLE */
	}
}

/**************************************
* DRAW A SOLID LINE ON SCREEN (TEST)  *
**************************************/

void
Solid_Line()
{

ULONG	j;

	SetDispMask(0);				/* 1=ENABLE DISPLAY, 0=DISABLE */
	for (j=0;j<50;j++) Rts();
	SetDispMask(1);				/* 1=ENABLE DISPLAY, 0=DISABLE */

}

/**************************************
*      PUT DEBOUNCE ON BOTH PADS      *
**************************************/

void
Read_Pads(void)					/* CALLED EVERY FRAME TO LOOK AT VALUES IN */
{								/* Pad1, Pad2, Debounce1, Debounce2 */

USHORT	old;
USHORT	temp;
BYTE	padtype1,padtype2;

	padtype1=GetType(&pad1);			/* GET CONTROLLER TYPE IN PORT 1 */
	padtype2=GetType(&pad2);			/* GET CONTROLLER TYPE IN PORT 2 */

	if (padtype1==STD_PAD)
	{
		if (GoodData(&pad1)==TRUE)
		{
			old=Pad1;
			Pad1=pad1.data.pad^0xffff;	/* COPY DATA INTO PAD 1 */
			temp=(Pad1 XOR old);
			Debounce1=(temp & Pad1);
		}
	}

	if (padtype2==STD_PAD)
	{
		if (GoodData(&pad2)==TRUE)
		{
			old=Pad2;
			Pad2=pad2.data.pad^0xffff;	/* COPY DATA INTO PAD 2 */
			temp=(Pad2 XOR old);
			Debounce2=(temp & Pad2);
		}
	}
}

/**************************************
*        DRAW A LINE ON SCREEN        *
**************************************/

void
Draw_Line(USHORT x,USHORT y,USHORT xl,USHORT yl,BYTE red,BYTE green,BYTE blue,BYTE zed)
{

	if (maxpolycount==0) return;
	maxpolycount--;

	line.r=red;
	line.g=green;
	line.b=blue;

	line.x0=x;
	line.y0=y;
	line.x1=xl;
	line.y1=yl;
	GsSortLine(&line,&Wot[screentoggle],255-zed);

}

/**************************************
*        DRAW A LINE ON SCREEN        *
**************************************/

void
Draw_GLine(USHORT x,USHORT y,USHORT xl,USHORT yl,
			BYTE red0,BYTE green0,BYTE blue0,
			BYTE	red1,BYTE green1,BYTE blue1,BYTE zed)
{

	if (maxpolycount==0) return;
	maxpolycount--;

	gline.attribute|=0x60000000;

	gline.r0=red0;
	gline.g0=green0;
	gline.b0=blue0;

	gline.r1=red1;
	gline.g1=green1;
	gline.b1=blue1;

	gline.x0=x;
	gline.y0=y;
	gline.x1=xl;
	gline.y1=yl;
	GsSortGLine(&gline,&Wot[screentoggle],255-zed);

}

/**************************************
*      SHOW PICTURE OF VIDEO RAM      *
**************************************/

void
Show_VRAM(void)
{

USHORT	slidex,slidey;

	slidex=380;
	slidey=0;

	DrawSync(0);
	VSync(0);

	GsInitGraph(640,480,2,0,0);		/* SET THE MODE */
	SetDispMask(1);				/* 1=ENABLE, 0=DISABLE */
	while (0==0)
	{
	  	GsDefDispBuff(slidex,slidey,slidex,slidey);
		Start_Loop();

		Read_Pads();  
		if ((Pad1 & PaDLleft)!=0) if (slidex>3)slidex-=4;
		if ((Pad1 & PaDLright)!=0) if (slidex<=380)slidex+=4;

		if ((Pad1 & PaDLup)!=0) if (slidey>3)slidey-=4;
		if ((Pad1 & PaDLdown)!=0) if (slidey<=28)slidey+=4;

		if (pollhost==YES) pollhost();

		VSync(0);					/* WAIT FOR VBLANK */
		gamecycle++;				/* UPDATE FRAME COUNTER */
		GsSwapDispBuff();			/* EXCHANGE DISPLAY AND DRAWING BUFFER */
		ResetGraph(1);				/* RESET GPU */

		GsDrawOt(&Wot[screentoggle]);

		if ((Debounce1 & PaDld)!=0)
		{
			GsInitGraph(screenwidth,screenheight,0,0,0);
  			GsDefDispBuff(0,256,0,0);
			GsInit3D();
			return;
		}
	}
}

/**************************************
*          PRINT HEX NUMBER           *
**************************************/

void
Print_Hex(ULONG number)
{

UBYTE	nibble[8];			/* RESTORE: was [7]; eight hex digits, nibble[0] to nibble[7] */
UBYTE	string[2]={0,0};		
UBYTE	j;
ULONG	num;

	num=number;
	nibble[7]=num&15;	
	num=number;
	nibble[6]=(num&0xF0)/0x10;
	num=number;
	nibble[5]=(num&0xF00)/0x100;
	num=number;
	nibble[4]=(num&0xF000)/0x1000;
	num=number;
	nibble[3]=(num&0xF0000)/0x10000;
	num=number;
	nibble[2]=(num&0xF00000)/0x100000;
	num=number;
	nibble[1]=(num&0xF000000)/0x1000000;
	num=number;
	nibble[0]=(num&0xF0000000)/0x10000000;
	for (j=0;j!=8;j++)
	{
		string[0]=nibble[j]+48;
		if (string[0]>57) string[0]=string[0]+65-58;	
		Print_Messm(string);
	}
}

/**************************************
*        PRINT DECIMAL NUMBER         *
**************************************/

void
Print_Decl(LONG number)
{

UBYTE	string[2]={0,0};
UBYTE	nibble[10];
ULONG	num;
UBYTE	j;
UBYTE	flag=0;

	if (number<0)
	{
		number=-number;
		Print_Messm("-");
	}
	num=number;
	nibble[0]=num/1000000000;
	number=number-nibble[0]*1000000000;
	num=number;
	nibble[1]=num/100000000;
	number=number-nibble[1]*100000000;
	num=number;
	nibble[2]=num/10000000;
	number=number-nibble[2]*10000000;
	num=number;
	nibble[3]=num/1000000;
	number=number-nibble[3]*1000000;
	num=number;
	nibble[4]=num/100000;
	number=number-nibble[4]*100000;
	num=number;
	nibble[5]=num/10000;
	number=number-nibble[5]*10000;
	num=number;
	nibble[6]=num/1000;
	number=number-nibble[6]*1000;
	num=number;
	nibble[7]=num/100;
	number=number-nibble[7]*100;
	num=number;
	nibble[8]=num/10;
	number=number-nibble[8]*10;
	num=number;
	nibble[9]=num;

	for (j=0;j!=10;j++)
	{
		if (j==9) flag=1;
		if (nibble[j]!=0) flag=1;
		string[0]=nibble[j]+48;
		if (flag!=0) Print_Messl(string);
	}
}

/**************************************
*        PRINT DECIMAL NUMBER         *
**************************************/

void
Print_Decm(LONG number)
{

UBYTE	string[2]={0,0};
UBYTE	nibble[10];
ULONG	num;
UBYTE	j;
UBYTE	flag=0;

	if (number<0)
	{
		number=-number;
		Print_Messm("-");
	}
	num=number;
	nibble[0]=num/1000000000;
	number=number-nibble[0]*1000000000;
	num=number;
	nibble[1]=num/100000000;
	number=number-nibble[1]*100000000;
	num=number;
	nibble[2]=num/10000000;
	number=number-nibble[2]*10000000;
	num=number;
	nibble[3]=num/1000000;
	number=number-nibble[3]*1000000;
	num=number;
	nibble[4]=num/100000;
	number=number-nibble[4]*100000;
	num=number;
	nibble[5]=num/10000;
	number=number-nibble[5]*10000;
	num=number;
	nibble[6]=num/1000;
	number=number-nibble[6]*1000;
	num=number;
	nibble[7]=num/100;
	number=number-nibble[7]*100;
	num=number;
	nibble[8]=num/10;
	number=number-nibble[8]*10;
	num=number;
	nibble[9]=num;

	for (j=0;j!=10;j++)
	{
		if (j==9) flag=1;
		if (nibble[j]!=0) flag=1;
		string[0]=nibble[j]+48;
		if (flag!=0) Print_Messm(string);
	}
}

/**************************************
*        PRINT DECIMAL NUMBER         *
**************************************/

void
Print_Decs(LONG number)
{

UBYTE	string[2]={0,0};
UBYTE	nibble[10];
ULONG	num;
UBYTE	j;
UBYTE	flag=0;

	if (number<0)
	{
		number=-number;
		Print_Messm("-");
	}
	num=number;
	nibble[0]=num/1000000000;
	number=number-nibble[0]*1000000000;
	num=number;
	nibble[1]=num/100000000;
	number=number-nibble[1]*100000000;
	num=number;
	nibble[2]=num/10000000;
	number=number-nibble[2]*10000000;
	num=number;
	nibble[3]=num/1000000;
	number=number-nibble[3]*1000000;
	num=number;
	nibble[4]=num/100000;
	number=number-nibble[4]*100000;
	num=number;
	nibble[5]=num/10000;
	number=number-nibble[5]*10000;
	num=number;
	nibble[6]=num/1000;
	number=number-nibble[6]*1000;
	num=number;
	nibble[7]=num/100;
	number=number-nibble[7]*100;
	num=number;
	nibble[8]=num/10;
	number=number-nibble[8]*10;
	num=number;
	nibble[9]=num;

	for (j=0;j!=10;j++)
	{
		if (j==9) flag=1;
		if (nibble[j]!=0) flag=1;
		string[0]=nibble[j]+48;
		if (flag!=0) Print_Messs(string);
	}
}

/* RESTORE: NEW WORK (owner's decision, 2026-11-22: "real pictures of the controller buttons" in the control help, one line a
   command). NO NMS VERSION HAS BUTTON PICTURES. THE ICONS ARE DRAWN FOR THIS GAME IN THE STYLE OF ITS MEDIUM FONT (360x/art/padicons.txt,
   ONE PIXEL STROKES ON A DARK OUTLINE, THE FONT CELL'S 16 ROWS) AND tools/x360_padicons.py MAKES padicons.h FROM THE DRAWING. THEY LIVE IN
   THE FONT PAGE: FONTS.TIM (256 x 256, 4 BIT) HOLDS ITS THREE FONTS IN ROWS 0 TO 191 AND NOTHING BELOW, SO THE ICON STRIP IS COPIED INTO
   ROWS 192 ON OF THE TIM IN MEMORY BEFORE THE PAGE IS LOADED (Add_Res_Page LOADS IT ONCE, FROM THAT MEMORY), AND THE ICONS ARE RESIDENT
   SPRITES ON THE SAME PAGE WITH THEIR OWN 16 COLOUR PALETTE: NO VRAM, NO HEAP. THE STRIP IS KEPT AT 2 BITS A PIXEL (0.6 KB; EVERY ICON HAS
   AT MOST FOUR COLOURS: TRANSPARENT, OUTLINE, BODY AND ITS OWN) AND UNPACKED INTO THE PAGE. */
#include "padicons.h"

/* RESTORE: THE CONTROL HELP'S CODE RUNS ONLY WHILE PAUSED AND AT THE MISSION START: IT IS LINKED AFTER THE LAYER (psyq/psyq.ld, ".psx_cold").
   THE LAYER'S HOT FUNCTIONS ARE PLACED AT I-CACHE LINE 252, AFTER THE GAME'S CODE; GAME CODE UP TO THAT LINE COSTS NO MEMORY (IT FILLS THE
   PADDING), CODE PAST IT PUSHES THEM A WHOLE 4 KB ON, AND THE WEAPONS TEST BUILD (ITS TEST CODE IN FRONT TOO) THEN HAD NO ROOM FOR ITS HEAP.
   COLD CODE COSTS ITS SIZE, SO ONLY ENOUGH IS MOVED FOR EVERY BUILD TO STAY UNDER THE LINE. */
#define	PAUSE_COLD	__attribute__((section(".psx_cold.x360_pause")))

#define	FONTS_TIM_PIXELS	(32+2*16)		/* FONTS.TIM'S PIXELS START HERE (SPRLIST.C RESPIC) */
#define	FONTS_TIM_ROWBYTES	128			/* 256 PIXELS OF 4 BITS */

SPRITE	spr_PADICONS[PADICON_COUNT];

void
Pad_Icons_Into_Fonts(void)
{
UBYTE	*row;
int	y,x,n,v;

	for (y=0;y<16;y++)				/* THE CELL'S 16 ROWS: THE TIM FILLS THEM WITH INDEX 1, OPAQUE IN THE ICONS' PALETTE */
	{
		row=(UBYTE *)&fonts+FONTS_TIM_PIXELS+(PADICON_PAGE_Y+y)*FONTS_TIM_ROWBYTES;
		for (x=0;x<PADICON_STRIP_PIXELS;x+=2) row[x>>1]=0;
		if ((y<PADICON_FIRST_ROW) || (y>=PADICON_FIRST_ROW+PADICON_STRIP_ROWS)) continue;
		for (n=0;n<PADICON_COUNT;n++)
			for (x=padicon_xwc[n][0];x<padicon_xwc[n][0]+padicon_xwc[n][1];x++)
			{
				v=(padicon_strip[y-PADICON_FIRST_ROW][x>>2]>>((x&3)*2))&3;
				if (v==1) v=PADICON_OUTLINE;
				else if (v==2) v=PADICON_BODY;
				else if (v==3) v=padicon_xwc[n][2];
				row[x>>1]|=v<<((x&1)*4);	/* 4 BITS, THE LEFT PIXEL LOW */
			}
	}
}

void
Pad_Icons_Init(void)
{
SPRITE	*spr;
int	i;

	for (i=0;i<PADICON_COUNT;i++)
	{
		spr=&spr_PADICONS[i];
		spr->image=(ULONG)&fonts+FONTS_TIM_PIXELS;	/* THE FONT PAGE (Add_Res_Page FINDS IT LOADED) */
		spr->pal=(ULONG)padicon_pal;
		spr->x=padicon_xwc[i][0];
		spr->y=PADICON_PAGE_Y;
		spr->w=padicon_xwc[i][1];
		spr->h=16;
		spr->u=0;
		spr->v=0;
		spr->clut=0;
		spr->tpage=0;
		spr->flags=RESPAGE;
	}
	spr_PADICONS[0].flags|=FIRSTSPR;
	spr_PADICONS[PADICON_COUNT-1].flags|=LASTSPR;
	Init_Spr_List(&spr_PADICONS[0],0);		/* ADDRESSES ARE ABSOLUTE: NO LOAD OFFSET */
}

/**************************************
*  SETUP ALL RESIDENT TIMS (2 PAGES)  *
**************************************/

void 
Resident_Tims(void)
{

USHORT *pointer;

	Sprite_Off(&spr_FONTLARGE[0]);
	Sprite_Off(&spr_FONTMEDIUM[0]);
	Sprite_Off(&spr_FONTSMALL[0]);

	pointer=(USHORT *)&fonts+20/2;
	Pad_Icons_Into_Fonts();			/* RESTORE: NEW WORK, BEFORE THE FONT PAGE GOES TO VRAM (BELOW) */
	Init_Spr_List(&spr_FONTLARGE[0],(ULONG)&fonts);
	Pad_Icons_Init();

}

/**************************************
*       START TO FADE SCREEN UP       *
**************************************/

void
Start_Fade_Up(UBYTE speed)
{

	fade=speed;
	fadespd=speed;

}

/**************************************
*      START TO FADE SCREEN DOWN      *
**************************************/

void
Start_Fade_Down(UBYTE speed)
{

	if (fade!=128) return;
	fade=128+speed;
	fadespd=speed;

}

/**************************************
*   UPDATE THE FADING OF THE SCREEN   *
**************************************/

void
Update_Fade()
{

	if (fade==128) return;
	if (fade==0) return;
	fade+=fadespd;

}

/**************************************
*          CLEAR THE SCREEN           *
**************************************/

void
Cls(void)
{

USHORT	j;

	DrawSyncCallback(&Rts);
	SetDispMask(OFF);
	sprctrl.red=sprctrl.green=sprctrl.blue=128;
	fade=0;
	Start_Fade_Up(16);

	for (j=0;j<MAXINTERGPUS;j++) InteruptGPU[j]=0;	/* CLEAR ALL LIVE DOWNLOADED */
	currentlive=0;					/* AND RESET COUNTER */

	Reset_Spr_Paras();
	VSync(0);
	SetDispMask(ON);				/* 1=ENABLE DISPLAY, 0=DISABLE */
	gamecycle=0;
	Read_Pads();

}

/**************************************
*        GET A RANDOM NUMBER          *
**************************************/

LONG
Random(ULONG size)
{

	return (rand() MOD size);

}

/**************************************
*   GET MEMORY FROM CUSTOM MALLOC     *
**************************************/

BYTE*
MyMalloc(ULONG size)
{

UBYTE	*ptr;

	ptr=Jalloc_Alloc(&JALLOC,size);	/* TRY AND MALLOC AREA USING JOHNS MALLOC */

	if (ptr!=0)
	{
		mallocused++;
	}

	return ptr;

}

/**************************************
*   FREE MEMORY FROM CUSTOM MALLOC    *
**************************************/

void
MyFree(UBYTE* mem)
{

	Jalloc_Free(&JALLOC,mem);		/* FREE USING CUSTOM 'FREE' */
	mallocused--;

}

/**************************************
*  TEST TO SEE IF MALLOC IS WORKING   *
**************************************/

void
MallocTest(void)
{

		Print_At(8,208);
		Print_Messm("MALLOCS USED : ");
		Print_Decm(mallocused);
		Print_At(8,224);
		Print_Messm("POLYCOUNT : ");	
		Print_Decm(MAXPOLYS-maxpolycount);

}

/**************************************
*   PRINT MESSAGE TO SCREEN (SMALL)   *
**************************************/

void 
Print_Messs(const UBYTE* mess)
{

UBYTE	letter;

	while ((letter=*mess++)!=0)
	{
		Print_Sprite(&spr_FONTSMALL[letter-32],messx,messy,255);
		messx+=spr_FONTSMALL[letter-32].w;
	}
}

/**************************************
*  PRINT MESSAGE TO SCREEN (MEDIUM)   *
**************************************/

void 
Print_Messm(const UBYTE * mess)
{

UBYTE	letter;

	while ((letter=*mess++)!=0)
	{
		Print_Sprite(&spr_FONTMEDIUM[letter-32],messx,messy,255);
		if (textbox_on==YES) Text_Box_Grow(messx,messy,spr_FONTMEDIUM[letter-32].w,spr_FONTMEDIUM[letter-32].h);	/* RESTORE: NEW WORK, THE MENU BOX (BELOW) */
		messx+=spr_FONTMEDIUM[letter-32].w;
	}
}

/* RESTORE: NEW WORK (owner's decision, 2026-11-21: "when pressing start the pause menu text should be in some sort of box, try to take
   inspiration from the game graphics"). NO NMS VERSION DRAWS ONE: THE JULY PS1, THE PC AND THE SATURN PRINT THE PAUSE AND MENU TEXT
   STRAIGHT OVER THE MAP. THE BOX IS DRAWN IN THE STYLE OF NMS'S OWN IN-GAME PANEL (PANEL.C, spr_BORDER1): A ONE PIXEL GREEN FRAME,
   0,148,0 AS THE PANEL'S, AROUND A DARK GREEN GLASS (THE MAP HALVED, A LITTLE GREEN ADDED). IT FITS WHATEVER TEXT IS PRINTED BETWEEN
   Text_Box_Begin AND Text_Box_End (Print_Messm GROWS IT), SO EVERY SCREEN OF THE MENU GETS ITS OWN SIZE; IT SITS BEHIND THE TEXT
   AND THE PANEL (PRIORITY 1; THEY ARE AT 0) AND IN FRONT OF THE MAP AND THE SPRITES; ITS FRAME IS AT 0, SO THE GLASS NEVER DIMS IT. */
UBYTE	textbox_on=NO;
static SHORT	textbox_x0,textbox_y0,textbox_x1,textbox_y1;

void
Text_Box_Grow(SHORT x,SHORT y,SHORT w,SHORT h)
{
	if (x<textbox_x0) textbox_x0=x;
	if (y<textbox_y0) textbox_y0=y;
	if ((x+w)>textbox_x1) textbox_x1=x+w;
	if ((y+h)>textbox_y1) textbox_y1=y+h;
}

void
Text_Box_Begin(void)
{
	textbox_on=YES;
	textbox_x0=textbox_y0=32767;
	textbox_x1=textbox_y1=-32767;
}

/* RESTORE: NEW WORK (owner's decision, 2026-11-22: "darken the background behind the menu"). dim=YES IS THE PAUSE SCREEN
   (Draw_Pause_Screen): EVERYTHING IS AT PRIORITY 0, WHERE THE GAME'S PANEL AND PARTICLES ALSO ARE; THE TABLE DRAWS A PRIORITY'S PRIMITIVES
   LAST SORTED FIRST, SO THE PAUSE SCREEN IS SORTED BEFORE THE GAME (MAIN.C) AND HERE IN THE ORDER TEXT, FRAME, GLASS, DIMMER: DRAWN AS
   THE GAME, THE DIMMER (THE WHOLE SCREEN HALVED, PANEL INCLUDED), THE GLASS, THE FRAME, THE TEXT. */
static void
Text_Box_Frame(SHORT x0,SHORT y0,SHORT x1,SHORT y1)
{
	SHORT	c[5][2];
	int	i;

	c[0][0]=c[3][0]=c[4][0]=x0;	c[1][0]=c[2][0]=x1;
	c[0][1]=c[1][1]=c[4][1]=y0;	c[2][1]=c[3][1]=y1;
	line.attribute=0;
	line.r=0;
	line.g=148;
	line.b=0;
	for (i=0;i<4;i++)
	{
		line.x0=c[i][0];	line.y0=c[i][1];	line.x1=c[i+1][0];	line.y1=c[i+1][1];
		GsSortLine(&line,&Wot[screentoggle],0);
	}
}

static void
Text_Box_Sort(UBYTE dim)
{
GsBOXF	box;
SHORT	x0,y0,x1,y1;

	textbox_on=NO;
	if (textbox_x1<textbox_x0) return;			/* NOTHING PRINTED */
	x0=textbox_x0-10-160;				/* SCREEN TO DRAWING COORDINATES, AS Print_Sprite */
	y0=textbox_y0-8-120;
	x1=textbox_x1+9-160;
	y1=textbox_y1+7-120;

	if (dim==YES) Text_Box_Frame(x0,y0,x1,y1);	/* SORTED BEFORE THE GLASS: DRAWN AFTER IT */

	box.attribute=GsALON|GsAZERO;			/* HALF THE MAP, HALF THE GLASS */
	box.x=x0;
	box.y=y0;
	box.w=x1-x0;
	box.h=y1-y0;
	box.r=0;
	box.g=24;
	box.b=0;
	GsSortBoxFill(&box,&Wot[screentoggle],(dim==YES)?0:1);

	if (dim==YES)
	{
		box.x=-160;					/* THE WHOLE SCREEN HALVED */
		box.y=-120;
		box.w=320;
		box.h=240;
		box.g=0;
		GsSortBoxFill(&box,&Wot[screentoggle],0);
	}
	else Text_Box_Frame(x0,y0,x1,y1);
}

void
Text_Box_End(void)
{
	Text_Box_Sort(NO);
}

/* RESTORE: NEW WORK, Print_Messm CENTRED ON x cx, FOR THE SHORT MESSAGES OF THE OWNER'S RULES (MAIN.C, HELP) */
void
Print_Messm_Centred(int cx,int y,const UBYTE * mess)
{
	const UBYTE	*m;
	int		w;

	w=0;
	for (m=mess;*m!=0;m++) w+=spr_FONTMEDIUM[*m-32].w;
	Print_At(cx-(w/2),y);
	Print_Messm(mess);
}

/* PRINT ONE ICON AT x,y (THE SAME y AS A LINE OF Print_Messm TEXT), GROWING THE MENU BOX; RETURNS ITS WIDTH */
PAUSE_COLD static int
Print_Pad_Icon(int n,int x,int y)
{
	Print_Sprite(&spr_PADICONS[n],x,y,255);
	if (textbox_on==YES) Text_Box_Grow(x,y,spr_PADICONS[n].w,16);
	return spr_PADICONS[n].w;
}

/* RESTORE: Print_Messm's WIDTH OF A MESSAGE */
PAUSE_COLD static int
Messm_Width(const UBYTE * mess)
{
	int	w;

	w=0;
	for (;*mess!=0;mess++) w+=spr_FONTMEDIUM[*mess-32].w;
	return w;
}

/* RESTORE: NEW WORK (owner's decision, 2026-11-18: "add some control help"; 2026-11-22: one line a command, button pictures). THE
   BUTTONS AS THE CODE READS THEM: LEFT AND RIGHT TURN (CONTROL.C Move_Craft), PAD_THRUST, PAD_FIRE AND PAD_GRAB AS THE CONTROLS SETTING
   SETS THEM (MAIN.C, OPTIONS.C Select_Controls: TYPE A CROSS, SQUARE OR CIRCLE, TRIANGLE; TYPE B CIRCLE, TRIANGLE OR CROSS, SQUARE), R2
   THE BRAKE (THE SATURN'S, CONTROL.C Move_Craft), L1 AND R1 THE WEAPON (PANEL.C), START PAUSE AND SELECT THE MENU (Check_Pause_Menu).
   TWO COLUMNS, THE ICONS AND THE WORDS, CENTRED ON x cx AS ONE BLOCK; A LINE EVERY HELP_LINE PIXELS FROM y. SHOWN WHILE PAUSED
   (Draw_Pause_Screen) AND ONCE WHEN THE FIRST MISSION STARTS (MAIN.C). */
#define	HELP_LINES	8
#define	HELP_LINE	15			/* THE ICONS ARE 13 ROWS HIGH: ONE ROW BETWEEN THEM, TWO BETWEEN THE WORDS' 11 */
#define	HELP_ICONGAP	2			/* BETWEEN TWO ICONS ON ONE LINE */
#define	HELP_COLGAP	6			/* BETWEEN THE ICONS AND THE WORDS */

PAUSE_COLD void
Print_Controls_Help(int cx,int y)
{
	static const UBYTE	help_a[HELP_LINES][2]={
		{PADICON_DPAD_LR,255},{PADICON_CROSS,255},{PADICON_SQUARE,PADICON_CIRCLE},{PADICON_TRIANGLE,255},
		{PADICON_R2,255},{PADICON_L1,PADICON_R1},{PADICON_START,255},{PADICON_SELECT,255}};
	static const UBYTE	help_b[HELP_LINES][2]={
		{PADICON_DPAD_LR,255},{PADICON_CIRCLE,255},{PADICON_TRIANGLE,PADICON_CROSS},{PADICON_SQUARE,255},
		{PADICON_R2,255},{PADICON_L1,PADICON_R1},{PADICON_START,255},{PADICON_SELECT,255}};
	static const char	*help_words[HELP_LINES]={
		"TURN","THRUST","FIRE","GRAB POD","BRAKE","WEAPON","PAUSE","MENU"};
	const UBYTE	(*icons)[2];
	int		iconsw,wordsw,w,x,i;

	icons=(controltype==0)?help_a:help_b;
	iconsw=wordsw=0;
	for (i=0;i<HELP_LINES;i++)
	{
		w=spr_PADICONS[icons[i][0]].w;
		if (icons[i][1]!=255) w+=HELP_ICONGAP+spr_PADICONS[icons[i][1]].w;
		if (w>iconsw) iconsw=w;
		w=Messm_Width((const UBYTE *)help_words[i]);
		if (w>wordsw) wordsw=w;
	}
	x=cx-(iconsw+HELP_COLGAP+wordsw)/2;
	for (i=0;i<HELP_LINES;i++)
	{
		w=Print_Pad_Icon(icons[i][0],x,y+i*HELP_LINE);
		if (icons[i][1]!=255) Print_Pad_Icon(icons[i][1],x+w+HELP_ICONGAP,y+i*HELP_LINE);
		Print_At(x+iconsw+HELP_COLGAP,y+i*HELP_LINE);
		Print_Messm((const UBYTE *)help_words[i]);
	}
}

/* RESTORE: NEW WORK (owner's decision, 2026-11-22: "darken the background behind the menu, center the pause menu"; later the same day,
   the same for the help at the mission start). THE PAUSE SCREEN (paused=YES: PAUSED AND THE CONTROL HELP) OR THE MISSION START'S HELP
   (paused=NO: CONTROLS AND THE HELP) IN THE BOX, THE BLOCK CENTRED ON THE SCREEN (320 x 240: x 160, AND y 120 HALFWAY BETWEEN THE TOP OF THE
   TITLE'S LETTERS, ROW 4 OF ITS CELL, AND THE FOOT OF THE LAST LINE'S ICONS, ROW 15), THE GAME
   DIMMED BEHIND IT (Text_Box_Sort). CALLED BEFORE THE GAME IS SORTED (MAIN.C). */
#define	PAUSE_TITLE_GAP	22			/* PAUSED TO THE FIRST HELP LINE */

PAUSE_COLD void
Draw_Pause_Screen(UBYTE paused)
{
	int	y;

	Text_Box_Begin();
	y=120-(4+PAUSE_TITLE_GAP+(HELP_LINES-1)*HELP_LINE+16)/2;
	Print_Messm_Centred(160,y,(paused==YES)?"PAUSED":"CONTROLS");	/* THE MISSION START'S HEADER: THE OWNER, 2026-11-22, "add an header" */
	Print_Controls_Help(160,y+PAUSE_TITLE_GAP);
	Text_Box_Sort(YES);
}

/**************************************
*   PRINT MESSAGE TO SCREEN (LARGE)   *
**************************************/

void 
Print_Messl(const UBYTE * mess)
{

UBYTE	letter;

	while ((letter=*mess++)!=0)
	{
		Print_Sprite(&spr_FONTLARGE[letter-32],messx,messy,255);
		messx+=spr_FONTLARGE[letter-32].w;
	}
}

/**************************************
*      PRINT AT ROUTINE FOR TEXT      *
**************************************/

void
Print_At(USHORT x,USHORT y)
{
	messx=x;
	messy=y;
}

/**************************************
*      INITIALISE SCREEN 320X240      *
**************************************/

void
Init_Screen320x240(void)
{

	screenwidth=320;
	screenheight=240;

  	GsDefDispBuff(0,256,0,0);
	GsDISPENV.screen.w=256;

	if (pal==YES)					/* CHECK FOR PAL MODE */
	{
		GsDISPENV.screen.x=LETTERBOX_X;
		GsDISPENV.screen.y=LETTERBOX_Y;
		GsDISPENV.screen.w=256;
		GsDISPENV.screen.h=SCREENHEIGHT;
	}

	GsInitGraph(screenwidth,screenheight,0,0,0);
	GsInit3D();					/* INITIALISE 3D SYSTEM */

}

/**************************************
*      INITIALISE SCREEN 640X240      *
**************************************/

void
Init_Screen640x240(void)
{

	screenwidth=640;
	screenheight=240;

  	GsDefDispBuff(0,256,0,0);
	GsDISPENV.screen.w=256;

	if (pal==YES)					/* CHECK FOR PAL MODE */
	{
		GsDISPENV.screen.x=LETTERBOX_X;
		GsDISPENV.screen.y=LETTERBOX_Y;
		GsDISPENV.screen.w=256;
		GsDISPENV.screen.h=SCREENHEIGHT;
	}

	GsInitGraph(screenwidth,screenheight,0,0,0);
	GsInit3D();					/* INITIALISE 3D SYSTEM */

}

/**************************************
*        GET SIN OF A CIRCLE          *
**************************************/

SHORT 
GetSin(SHORT angle)				/*0-1023  one quarter==256*/
{								/*return -32768 to 32768 */

SHORT	result;
UBYTE	quad;

	quad=angle>>8; /*Calculate quadrent*/
	switch(quad)
	{
		case 0: return sintable[angle]; 
		break;
		case 1: return sintable[511-angle];
		break;
		case 2: return -(sintable[angle-512]);
		break;
		case 3: return -(sintable[1023-angle]);
		break;
		case 4: return sintable[angle-1024];
		break;
	}
	return result;

}

/**************************************
*        GET COS OF A CIRCLE          *
**************************************/

SHORT 
GetCos(SHORT angle)				/* 0-1023 */
{

SHORT	result; 

	result=GetSin(angle+256);
	return result;

}

/**************************************
*      PROCESS THE INGAME MENU        *
**************************************/

void
Process_Menus(void)
{
	if (whichmenu==1)
	{
		if (menuselection==0) sprctrl.red=sprctrl.blue=80;
		Print_At(132,8*12);
		Print_Messm("RESUME");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (menuselection==1) sprctrl.red=sprctrl.blue=80;
		Print_At(136,9*12);
		Print_Messm("GAMMA");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (menuselection==2) sprctrl.red=sprctrl.blue=80;
		Print_At(136,10*12);
		Print_Messm("SOUND");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (menuselection==3) sprctrl.red=sprctrl.blue=80;
		Print_At(136,11*12);
		Print_Messm("PANEL");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (menuselection==4) sprctrl.red=sprctrl.blue=80;
		Print_At(144,12*12);
		Print_Messm("QUIT");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (((Debounce1 & PaDLup)!=0) && (menuselection!=0)) menuselection--;
		if (((Debounce1 & PaDLdown)!=0) && (menuselection!=4)) menuselection++;

		if (((Debounce1 & PaDenter)!=0) && (menuselection==0))	/* RESTORE: owner's decision, Start or Cross (PaDenter, 360X.H) */
		{
			whichmenu=OFF;
			menuselection=0;
		}

		if (((Debounce1 & PaDenter)!=0) && (menuselection==1))	/* RESTORE: owner's decision, Start or Cross (PaDenter, 360X.H) */
		{
			whichmenu=2;
			menuselection=0;
			return;
		}

		if (((Debounce1 & PaDenter)!=0) && (menuselection==2))	/* RESTORE: owner's decision, Start or Cross (PaDenter, 360X.H) */
		{
			whichmenu=3;
			menuselection=0;
			return;
		}

		if (((Debounce1 & PaDenter)!=0) && (menuselection==3))	/* RESTORE: owner's decision, Start or Cross (PaDenter, 360X.H) */
		{
			whichmenu=4;
			menuselection=0;
			return;
		}

		if (((Debounce1 & PaDenter)!=0) && (menuselection==4))	/* RESTORE: owner's decision, Start or Cross (PaDenter, 360X.H) */
		{
			whichmenu=OFF;
			menuselection=0;
			Start_Fade_Down(-16);
			score=0;
			craftdestroyed=YES;
			quittomenu=YES;	/* RESTORE: owner's decision (2026-11-18, "hitting Quit in the in-game menu brings you back to the briefing screen, it should bring you to the main menu"). NMS'S PC QUIT ENDS THE MISSION AND RETURNS TO ITS FRONT END, NOT TO A BRIEFING (MAKEBACK/MENU.C:539-542, MAIN.C:711-725); THE JULY PS1 COST A LIFE AND RELOADED THE MISSION. MAIN.C */
		}

		/* RESTORE: owner's decision, Circle leaves the menu as RESUME does (PaDback, 360X.H) */
		if ((Debounce1 & PaDback)!=0)
		{
			whichmenu=OFF;
			menuselection=0;
		}
	}







	if (whichmenu==2)
	{

		if (menuselection==0) sprctrl.red=sprctrl.blue=80;
		Print_At(130,9*12);
		Print_Messm("GO BACK");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (menuselection==1) sprctrl.red=sprctrl.blue=80;
		Print_At(118,10*12);
		Print_Messm("GAMMA: ");
		Print_Decm(gamma[level]);
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (((Debounce1 & PaDLup)!=0) && (menuselection!=0)) menuselection--;
		if (((Debounce1 & PaDLdown)!=0) && (menuselection!=1)) menuselection++;

		if ((((Debounce1 & PaDenter)!=0) && (menuselection==0)) || ((Debounce1 & PaDback)!=0))	/* RESTORE: owner's decision, Start or Cross on GO BACK, Circle anywhere in the menu (PaDback, 360X.H) */
		{
			whichmenu=1;
			menuselection=0;
			return;
		}

		if (menuselection==1)
		{
			if (((Debounce1 & PaDLleft)!=0) && gamma[level]!=64)
			{
				gamma[level]-=16;
			}
			if (((Debounce1 & PaDLright)!=0) && gamma[level]!=192)
			{
				gamma[level]+=16;
			}
		}
	}

	if (whichmenu==3)
	{

		if (menuselection==0) sprctrl.red=sprctrl.blue=80;
		Print_At(130,7*12);
		Print_Messm("GO BACK");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (menuselection==1) sprctrl.red=sprctrl.blue=80;
		Print_At(104,8*12);
		Print_Messm("SOUND TEST: ");
		Print_Decm(testsoundfx+1);
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (menuselection==2) sprctrl.red=sprctrl.blue=80;
		Print_At(96,9*12);
		Print_Messm("FX VOLUME: ");
		if (sfxvolume==127-64) Print_Messm("LOW");
		if (sfxvolume==127-32) Print_Messm("MEDIUM");
		if (sfxvolume==127) Print_Messm("HIGH");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (menuselection==3) sprctrl.red=sprctrl.blue=80;
		Print_At(84,10*12);
		Print_Messm("MUSIC VOLUME: ");
		if (musvolume==127-64) Print_Messm("LOW");
		if (musvolume==127-32) Print_Messm("MEDIUM");
		if (musvolume==127) Print_Messm("HIGH");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

 		if (menuselection==4) sprctrl.red=sprctrl.blue=80;
		Print_At(104,11*12);
		Print_Messm("MIXING: ");
		if (channels==MONO) Print_Messm("MONO");
		if (channels==STEREO) Print_Messm("STEREO");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (((Debounce1 & PaDLup)!=0) && (menuselection!=0)) menuselection--;
		if (((Debounce1 & PaDLdown)!=0) && (menuselection!=4)) menuselection++;

		if ((((Debounce1 & PaDenter)!=0) && (menuselection==0)) || ((Debounce1 & PaDback)!=0))	/* RESTORE: owner's decision, Start or Cross on GO BACK, Circle anywhere in the menu (PaDback, 360X.H) */
		{
			whichmenu=1;
			menuselection=0;
			return;
		}

		/* RESTORE: completion. The sound menu's lines below were commented
		   out in July 1996 with the rest of the in-mission sound (MAIN.C,
		   INTERUPT.C) and run again: a volume change starts the sound again
		   at the new volume (InitialiseSound: the music from its start, at
		   musvolume) and every change clicks (SFX_CLICK, at sfxvolume).
		   Mixing now calls SsSetMono/SsSetStereo, which NMS's lines named
		   without calling. */
		if (menuselection==2)
		{
			if (((Debounce1 & PaDLleft)!=0) && sfxvolume!=127-64)
			{
				sfxvolume-=32;
				soundfx=YES;		/* DO YOU NEED VAB BUFFER? */
				music=YES;		/* DO YOU NEED SEQ BUFFER? */
				InitialiseSound();		/* START THE MUSIC */
				SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
			}
			if (((Debounce1 & PaDLright)!=0) && sfxvolume!=127)
			{
				sfxvolume+=32;
				soundfx=YES;		/* DO YOU NEED VAB BUFFER? */
				music=YES;		/* DO YOU NEED SEQ BUFFER? */
				InitialiseSound();		/* START THE MUSIC */
				SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
			}
		}

		if (menuselection==3)
		{
			if (((Debounce1 & PaDLleft)!=0) && musvolume!=127-64)
			{
				musvolume-=32;
				soundfx=YES;		/* DO YOU NEED VAB BUFFER? */
				music=YES;		/* DO YOU NEED SEQ BUFFER? */
				InitialiseSound();		/* START THE MUSIC */
				SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
			}
			if (((Debounce1 & PaDLright)!=0) && musvolume!=127)
			{
				musvolume+=32;
				soundfx=YES;		/* DO YOU NEED VAB BUFFER? */
				music=YES;		/* DO YOU NEED SEQ BUFFER? */
				InitialiseSound();		/* START THE MUSIC */
				SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
			}
		}

		if (menuselection==4)
		{
			if (((Debounce1 & PaDLleft)!=0) && channels!=MONO)
			{
				channels=MONO;
				SsSetMono();	/* RESTORE: bug fix, was SsSetMono; (names the function, does not call it) */
				SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
			}
			if (((Debounce1 & PaDLright)!=0) && channels!=STEREO)	/* RESTORE: bug fix, was musvolume!=STEREO (always true) */
			{
				channels=STEREO;
				SsSetStereo();	/* RESTORE: bug fix, was SsSetStereo; */
				SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
			}
		}

		if (((Debounce1 & PaDLleft)!=0) && (menuselection==1))
		{
			if (testsoundfx!=0) testsoundfx--;
		}

		if (((Debounce1 & PaDLright)!=0) && (menuselection==1))
		{
			if (testsoundfx!=9) testsoundfx++;
		}

		if (((Debounce1 & PaDenter)!=0) && (menuselection==1))	/* RESTORE: owner's decision, Start or Cross (PaDenter, 360X.H) */
		{
			SsUtKeyOn(vab,102+testsoundfx,0,FXPITCH,0,sfxvolume,sfxvolume);
		}
	}

	if (whichmenu==4)
	{

		if (menuselection==0) sprctrl.red=sprctrl.blue=80;
		Print_At(130,8*12);
		Print_Messm("GO BACK");
		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (menuselection==1) sprctrl.red=sprctrl.blue=80;
		Print_At(104,9*12);
		Print_Messm("PANEL: ");
		if (panelposition==0) Print_Messm("TYPE A");
		if (panelposition==1) Print_Messm("TYPE B");
		if (panelposition==2) Print_Messm("TYPE C");
		if (panelposition==3) Print_Messm("TYPE D");

		sprctrl.red=sprctrl.green=sprctrl.blue=128;

		if (((Debounce1 & PaDLup)!=0) && (menuselection!=0)) menuselection--;
		if (((Debounce1 & PaDLdown)!=0) && (menuselection!=1)) menuselection++;

		if ((((Debounce1 & PaDenter)!=0) && (menuselection==0)) || ((Debounce1 & PaDback)!=0))	/* RESTORE: owner's decision, Start or Cross on GO BACK, Circle anywhere in the menu (PaDback, 360X.H) */
		{
			whichmenu=1;
			menuselection=0;
			return;
		}

		if (menuselection==1)
		{
			if (((Debounce1 & PaDLleft)!=0) && panelposition!=0)
			{
				panelposition--;

			}

			if (((Debounce1 & PaDLright)!=0) && panelposition!=3)
			{
				panelposition++;
			}
		}

		if (panelposition==0)
		{
			panelx=40;
			panely=84;
		}

		if (panelposition==1)
		{
			panelx=STARTPANELX;
			panely=STARTPANELY;
		}

		if (panelposition==2)
		{
			panelx=40;
			panely=224;
		}

		if (panelposition==3)
		{
			panelx=280;
			panely=224;
		}
	}
}

/**************************************
*  CHECK FOR PAUSE & MENU SELECTION   *
**************************************/

void
Check_Pause_Menu(void)
{

	if (whichmenu==OFF)
	{
		if (pause==OFF)
		{
			if ((Debounce1 & PaDstart)!=0)
			{
				pause=ON;
			}
		}
		else
		{
			if ((Debounce1 & PaDstart)!=0)
			{
				pause=OFF;
			}
		}
	}

	if (pause==OFF)
	{
		if (whichmenu==OFF)
		{
			if ((Debounce1 & PaDselect)!=0)
			{
				whichmenu=ON;
			}
		}
	}
}


/**************************************
*   SHOW "PLEASE WAIT" WHILE LOADING  *
**************************************/

extern	UBYTE	*please_w[3];			/* LOADSAVE.C: NMS'S OWN MESSAGE, THREE LANGUAGES */

/* RESTORE: owner's decision (2026-11-13: "add a loading text or something to tell the user that we're
   loading something"). Each front-end screen loads its files before it first shows anything, and the
   screen stays black for that time (one to three seconds on the disc). Called once the screen's
   resident graphics (the font) are in, this puts NMS's own "PLEASE WAIT" (the message LOADSAVE.C shows
   while it checks the memory cards, in NMS's three languages) on the screen, centred, before the
   screen starts loading; the screen's own Cls() takes it away. No new strings. */
void
Loading_Message(void)
{

USHORT	x,i;
UBYTE	letter;
const UBYTE	*mess;

	mess=please_w[language];
	x=0;
	for (i=0;(letter=mess[i])!=0;i++) x+=spr_FONTLARGE[letter-32].w;

	Cls();
	fade=128;
	for (i=0;i<2;i++)				/* BOTH BUFFERS */
	{
		Start_Loop();
		Print_At((screenwidth/2)-(x/2),(screenheight/2)-8);
		Print_Messl(mess);
		End_Loop();
	}

}


/**************************************
*   THE LASER CREATURE'S BEAM (GLOW)   *
**************************************/

/* RESTORE: completion, rebuilt from NMS's PC version (owner's decision 2026-11-13, "every gameplay feature completed in other ports
   functional on the PS1"). THE JULY LOGIC'S ai_lazer (AI.DAT) FIRES A BEAM WITH add_line: FIVE LINES, y-2 TO y+2 OF THE CREATURE, FROM
   ITS x, 320 PIXELS LONG, EVERY CYCLE IT FIRES. THE PC (MAKEBACK/PROCESS.C:1254, GLOW.C:40 add_a_glow_line_clipped, :87 draw_glow_list)
   KEEPS THE LINES OF A CYCLE AND REDRAWS THE BACKGROUND UNDER EACH THROUGH A COLOUR TABLE (CODE.ASM asm_remap_; POLY.C make_remapper:
   COLOUR i BECOMES COLOUR i+48, 255 AT MOST) BEFORE THE SPRITES: A RECOLOURED STRIP OF THE BACKGROUND, NOT A FLAT COLOUR. THAT TABLE
   MEANS SOMETHING ONLY IN THE PC'S PALETTE; THE PS1'S LEVEL PALETTES (LEVn.PAL) ARE ORDERED DIFFERENTLY (NO SHIFT OF THEIRS LIGHTENS
   A COLOUR IN MORE THAN ABOUT 40% OF THE CASES), SO THE SAME TABLE ON THEM WOULD NOT MAKE A BEAM. THE PS1 DRAWS EACH LINE AS A
   SEMI-TRANSPARENT LINE OVER THE BACKGROUND, BEHIND THE SPRITES: THE COLOUR AND THE BLEND ARE THE OWNER'S DECISION OF 2026-11-13
   ("GREY", FROM THREE LOOKS; docs/DECISIONS.md). THE BEAM IS DRAWN ONLY: NOTHING IN THE PC'S OR THE SATURN'S CODE MAKES IT HURT THE CRAFT. A LINE IS CLIPPED TO THE SCREEN
   AS THE PC'S IS. */

/* THE BEAM'S COLOUR: ONE LINE (red, green, blue; 128 IS THE BACKGROUND'S OWN WEIGHT IN THE LAYER'S DEFAULT BLEND) */
static const UBYTE	glowrgb[3]={150,150,150};
/* RESTORE: THE RING WEAPON'S COLOUR (WEAPONCIRCLE, FIRE.C): ONE LINE, red, green, blue. THE COORDINATOR'S DECISION FOR THE NIGHT OF 2026-11-13, AND THE OWNER'S TO CHANGE:
   THE SAME GREY 150 THE OWNER CHOSE FOR THE LASER'S BEAM. */
static const UBYTE	ringrgb[3]={150,150,150};
#define	GLOW_LINES_MAX	320		/* WAS 64 (THE BEAMS ALONE); THREE RINGS OF UP TO 40 ROWS ARE ADDED; 320 FOR add_polo'S RINGS (UP TO 48 PIXELS) */

static LONG	glowx[GLOW_LINES_MAX];
static LONG	glowy[GLOW_LINES_MAX];
static LONG	gloww[GLOW_LINES_MAX];
static UBYTE	glowring[GLOW_LINES_MAX];	/* 0 THE BEAM'S COLOUR, 1 THE RING'S */
static SHORT	glowcount=0;

void
Clear_Glow_Lines(void)
{
	glowcount=0;
}

static void
Add_Line(LONG x,LONG y,LONG width,UBYTE ring)
{
	LONG	x1;

	if (glowcount>=GLOW_LINES_MAX) return;
	x1=x-screenx;
	if ((y<screeny) || (y>=screeny+240)) return;
	if ((x1>=320) || ((x1+width)<=0)) return;
	if (x1<0)						/* CLIP THE LEFT EDGE */
	{
		width+=x1;
		x-=x1;
	}
	if ((x-screenx+width)>320) width=320-(x-screenx);	/* AND THE RIGHT */
	if (width<=0) return;
	glowx[glowcount]=x;
	glowy[glowcount]=y;
	gloww[glowcount]=width;
	glowring[glowcount]=ring;
	glowcount++;
}

void
Add_Glow_Line(LONG x,LONG y,LONG width)
{
	Add_Line(x,y,width,0);
}

/* RESTORE: completion, rebuilt from the PC version (owner's decision 2026-11-13): THE RING WEAPON'S DISC. THE PC's add_circle (MAKEBACK/GLOW.C:282) FILLS A DISC OF
   THE GIVEN RADIUS ROW BY ROW (draw_circle, draw_circle_clipped: TWO LINES A ROW, y-row AND y+row-1, 2*x WIDE, x FROM THE SAME INTEGER LOOP), THROUGH THE
   COLOUR TABLE THAT MAKES THE LASER'S BEAM. HERE, AS THE BEAM IS, EACH ROW IS A SEMI-TRANSPARENT LINE IN ringrgb. */
void
Add_Ring(LONG radius,LONG start_x,LONG start_y)
{
	LONG	x,y;
	LONG	sq_x,sq_y,sq_radius;

	if (radius<2) return;
	sq_radius=radius*radius;
	x=radius;
	sq_x=radius*radius;
	for (y=1;y<radius;y++)
	{
		sq_y=y*y;
		while ((sq_y+sq_x)>sq_radius)
		{
			x--;
			sq_x=x*x;
		}
		Add_Line(start_x-x,start_y-y,x*2,1);
		Add_Line(start_x-x,start_y+y-1,x*2,1);
	}
}

/* RESTORE: completion, rebuilt from the PC version (owner's decision 2026-11-21): NMS'S add_polo (MAKEBACK/GLOW.C:159-345), THE RING ai_polo
   (AI.DAT 1992) DRAWS WHEN A CREATURE IS HIT, A PLUNGER DIES (ai_plunger_dies) OR A SPITTER IS HIT (ai_die_spit). THE PC FILLS THE BAND BETWEEN
   RADIUS r2 AND r1 ROW BY ROW (draw_polo, draw_polo_clipped: A WHOLE ROW WHERE THE ROW IS PAST r2, ELSE ITS TWO EDGES, ON ROWS y-row AND
   y+row-1, x FROM THE SAME INTEGER LOOP) THROUGH THE COLOUR TABLE OF THE LASER'S BEAM; HERE, AS Add_Ring, EACH PIECE IS A SEMI-TRANSPARENT
   LINE IN ringrgb. THE PC CLIPS TO THE SCREEN AS Add_Line DOES. */
void
Add_Polo(LONG r1,LONG r2,LONG start_x,LONG start_y)
{
	LONG	x1,x2,y1,edge,gap;
	LONG	sq_x1,sq_x2,sq_y,sq_r1,sq_r2;

	if ((r1<2) || (r1>255)) return;
	if (r2<0) r2=0;
	sq_r1=r1*r1;
	sq_r2=r2*r2;
	x1=r1;
	x2=r2;
	sq_x1=r1*r1;
	sq_x2=r2*r2;
	for (y1=1;y1<r1;y1++)
	{
		sq_y=y1*y1;
		while ((sq_y+sq_x1)>sq_r1)
		{
			x1--;
			sq_x1=x1*x1;
		}
		if (y1>r2)
		{
			Add_Line(start_x-x1,start_y-y1,x1*2,1);
			Add_Line(start_x-x1,start_y+y1-1,x1*2,1);
		}
		else
		{
			while ((sq_y+sq_x2)>sq_r2)
			{
				x2--;
				sq_x2=x2*x2;
			}
			edge=((x1*2)-(x2*2))/2;
			gap=x2*2;
			if (edge<=0) continue;
			Add_Line(start_x-x1,start_y-y1,edge,1);
			Add_Line(start_x-x1+edge+gap,start_y-y1,edge,1);
			Add_Line(start_x-x1,start_y+y1-1,edge,1);
			Add_Line(start_x-x1+edge+gap,start_y+y1-1,edge,1);
		}
	}
}

void
Draw_Glow_Lines(void)
{
	SHORT	n;

	for (n=0;n<glowcount;n++)
	{
		if (maxpolycount==0) return;
		maxpolycount--;
		line.attribute=0x40000000;			/* SEMI-TRANSPARENT */
		if (glowring[n]!=0)
		{
			line.r=ringrgb[0];
			line.g=ringrgb[1];
			line.b=ringrgb[2];
		}
		else
		{
			line.r=glowrgb[0];
			line.g=glowrgb[1];
			line.b=glowrgb[2];
		}
		line.x0=glowx[n]-screenx-160;		/* THE DRAWING ORIGIN IS THE CENTRE (Print_The_Map) */
		line.y0=glowy[n]-screeny-120;
		line.x1=line.x0+gloww[n];
		line.y1=line.y0;
		GsSortLine(&line,&Wot[screentoggle],200);	/* BEHIND THE SPRITES (25 AND LESS), IN FRONT OF THE MAP (220) */
	}
}


/**************************************
*        THE POD'S ROPE (LINE)         *
**************************************/

/* RESTORE: completion, rebuilt from the PC version (MAKEBACK/MAIN.C:940-945): A LINE FROM THE CRAFT TO THE POD IN HAND, COLOUR 255 OF THE PC'S PALETTE
   (draw_line(255,...), LINE.C:47), WHICH THE PS1'S PALETTE ORDERS DIFFERENTLY: WHITE. */
void
Draw_Pod_Rope(void)
{
	if (pod_attached!=TRUE) return;
	if (maxpolycount==0) return;
	maxpolycount--;
	line.attribute=0;
	line.r=255;
	line.g=255;
	line.b=255;
	line.x0=craftx-screenx-160;
	line.y0=crafty-screeny-120;
	line.x1=ball_x-screenx-160;
	line.y1=ball_y-screeny-120;
	GsSortLine(&line,&Wot[screentoggle],200);
}
