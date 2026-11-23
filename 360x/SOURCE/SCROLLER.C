
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE ROUTINES           *
**************************************/

/**************************************
*           DECLARE DEFINES           *
**************************************/

#define	bit24	(0x1000000)		/* BITS FOR DEFINING SPRITE DEFINITION */

/**************************************
*          DECLARE VARIABLES          *
**************************************/

BYTE	*levelbin;				/* POINTER TO CURRENT LEVEL BIN */
BYTE	*levelmap;				/* POINTER TO CURRENT LEVEL MAP */
BYTE	*levelpal;				/* POINTER TO CURRENT LEVEL PAL */
BYTE	*levelcol;				/* POINTER TO CURRENT LEVEL COLL */
LONG	levelcolsize;				/* RESTORE: ITS LENGTH IN BYTES */
SHORT	colwidth;				/* RESTORE: bug fix, THE COLLISION MAP'S WIDTH IN PIXELS (Sort_Map) */

ULONG	*levelmaplong;

BYTE	*downloadbufferx;			/* POINTER TO VERTICAL STRIP */
BYTE	*downloadbuffery;			/* POINTER TO HORIZONTAL STRIP */

LONG	startscreenx;				/* START X POSITION ON THIS LEVEL */
LONG	startscreeny;				/* START Y POSITION ON THIS LEVEL */

LONG	screenx;					/* PIXEL POSITION ACROSS MAP */
LONG	screeny;					/* PIXEL POSITION DOWN MAP */

LONG	scrollxoff;				/* X OFFSET FOR WHERE TO GET DATA FROM */
LONG	scrollyoff;				/* X OFFSET FOR WHERE TO GET DATA FROM */

SHORT	mapwidth;					/* HEIGHT OF THIS MAP */
SHORT	mapheight;				/* WIDTH OF THIS MAP */

MAPDATA	maptextures[21*17];			/* TEXTURE LIST FOR THE SCROLLER */

LONG	spritenumber;

/**************************************
*          INITIALISE LEVEL           *
**************************************/

void
Init_Map(BYTE *binfile,BYTE *mapfile,BYTE *palfile,BYTE *bmffile,BYTE *colfile)
{

RECT	rect;					/* LOADIMAGE INFO */

UBYTE	scroll_vram;

	Init_Screen320x240();

	spritenumber=0;

	Jalloc_Init(&JALLOC,sonymalloc,(maxmalloc*1024));	/* INITIALISE NEW MALLOC ROUTINE */

	Init_TPages();
	Reset_Cluts();
	Resident_Tims();
	Loading_Message();			/* RESTORE: owner's decision, PLEASE WAIT while the mission loads (GENERAL.C) */
	Clear_Sprites();
	Process_Script(bmffile);

	scroll_vram=Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(26,32);

	Xor_VRAM_Hole(((0+11)<<16)+(scroll_vram & 0x00ff0000),32,4);
	Xor_VRAM_Hole(((1+11)<<16)+(scroll_vram & 0x00ff0000),32,4);
	Xor_VRAM_Hole(((2+11)<<16)+(scroll_vram & 0x00ff0000),26,4);

	downloadbufferx=MyMalloc(16*16*17);	/* ALLOCATE MEMORY FOR DECOMPRESSION BUFFER */
	downloadbuffery=MyMalloc(16*16*21);	/* ALLOCATE MEMORY FOR DECOMPRESSION BUFFER */

	levelmap=LoadFile(mapfile);
	levelbin=LoadFile(binfile);
	levelpal=LoadFile(palfile);
	levelcol=LoadFile(colfile);
	levelcolsize=lastfilelength;			/* RESTORE: FOR Collision_At'S BOUNDS (bug fix) */

	cluts256[0]=1;					/* SETUP 1ST 256 CLUT AS SCROLLER */
	rect.x=0;
	rect.y=240;
	rect.w=256;
	rect.h=1;
	LoadImage(&rect,(ULONG *)levelpal);
	DrawSync(0);

	MyFree(levelpal);				/* FREE UP PALETTE MEMORY */

	Load_Data();

}

/* RESTORE: bug fix. THE VIEW NOW STOPS AT THE MAP'S EDGE AS THE SATURN AND PC VERSIONS STOP IT (SEE Update_Scroll), SO THE
   SCROLLER'S BUFFER (21 x 17 BLOCKS, ONE MORE EACH WAY THAN THE SCREEN) CAN ASK FOR A BLOCK ONE PAST THE LAST COLUMN OR ROW,
   WHICH IS NEVER ON THE SCREEN: THAT BLOCK IS TAKEN FROM THE LAST COLUMN OR ROW, NOT READ FROM THE NEXT ROW OR PAST THE MAP. */
static ULONG
Map_Block(LONG bx,LONG by)
{
	if (bx>=(mapwidth/16)) bx=(mapwidth/16)-1;
	if (by>=(mapheight/16)) by=(mapheight/16)-1;
	if (bx<0) bx=0;
	if (by<0) by=0;
	return levelmaplong[(by*(mapwidth/16))+bx];
}

/**************************************
* SORT MAP INTO BLOCKS STRIP AT TIME  *
**************************************/

void
Sort_Map(void)
{

RECT	rect;					/* LOADIMAGE INFO */

SHORT	ystrip;					/* YSTRIP CURRENTLY SORTING */
SHORT	xalong;					/* CURRENT BLOCK SORTING */
SHORT	xtemp;					/* X POSITION IN BLOCK */
SHORT	ytemp;					/* Y POSITION IN BLOCK */

ULONG	*currentblock;
ULONG	block;

	mapwidth=*(levelmap)*16;			/* GET WIDTH OF MAP */
	levelmap+=2;
	mapheight=*(levelmap)*16;		/* GET HEIGHT OF MAP */
	levelmap+=2;

	/* RESTORE: bug fix. THE COLLISION MAP IS ONE BIT A PIXEL, AS HIGH AS THE MAP, BUT NOT ALWAYS AS WIDE: MISSIONS 7 TO 9'S
	   (LEVELnC.BIN, 88200 BYTES) ARE 980 PIXELS WIDE FOR A 992 PIXEL MAP. READ WITH THE MAP'S WIDTH, EACH ROW SLIPPED 12 PIXELS
	   AND THE WALLS LAY IN DIAGONAL STRIPES ACROSS THE LEVEL. EVERY TEST OF THE MAP NOW STEPS A ROW BY ITS OWN WIDTH. */
	colwidth=(SHORT )((levelcolsize*8)/mapheight);
	if ((colwidth<=0) || (colwidth>mapwidth)) colwidth=mapwidth;

	levelmaplong=(ULONG *)levelmap;

	for (ytemp=0;ytemp<17;ytemp++)		/* DOWNLOAD INITIAL SCREEN */
	{
		for (xtemp=0;xtemp<21;xtemp++)
		{
			rect.x=320+((((screenx/16)*8)+(xtemp*8)) MOD (336/2));
			rect.y=0+((((screeny/16)*16)+(ytemp*16)) MOD 272);
			rect.w=8;
			rect.h=16;

			block=Map_Block((screenx/16)+xtemp,(screeny/16)+ytemp);	/* RESTORE: bug fix, KEPT ON THE MAP (Map_Block) */

			currentblock=(ULONG *)(levelbin+(block*16*16));

			LoadImage(&rect,(ULONG *)currentblock);
			DrawSync(0);
		}
	}
}

/**************************************
*     PRINT THE MAP TO THE SCREEN     *
**************************************/

void
Print_The_Map(void)
{

SHORT	xpos,ypos;
SHORT	xsmall,ysmall;

GsSPRITE spr = {bit24, 0,0,16,16,5,0,0,0,240,128,128,128,0,0,4096,4096,0};

	xsmall=(screenx & 15);
	ysmall=(screeny & 15);

	spr.w=16;						/* SET WIDTH TO 16 PIXELS */
	spr.h=16;						/* SET HEIGHT TO 16 PIXELS */

	for (ypos=0;ypos<17;ypos++)		/* 17 BLOCKS HIGH */
	{
		for (xpos=0;xpos<21;xpos++)		/* 21 BLOCKS WIDE */
		{
			if (maxpolycount!=0)
			{
				maxpolycount--;
				spr.x=xpos*16-xsmall-160;
				spr.y=ypos*16-ysmall-120;

				spr.r=gamma[level];
				spr.g=gamma[level];
				spr.b=gamma[level];

				spr.tpage=maptextures[(ypos*21)+xpos].page;
//				spr.tpage+=5;
				spr.u=maptextures[(ypos*21)+xpos].xoff;
				spr.v=maptextures[(ypos*21)+xpos].yoff;
				GsSortFastSprite(&spr,&Wot[screentoggle],220);
			}
		}
	}
}

/**************************************
*     DOWNLOAD SCROLLER TO VIDRAM     *
**************************************/

void
Do_Scroll_Download(void)
{

RECT	rect;
SHORT	temp;
BYTE	*download;				/* POINTER TO HORIZONTAL STRIP */

	rect.x=320+((((screenx/16)*8)+(scrollxoff/2)) MOD (336/2));
	rect.y=0;
	rect.w=8;
	rect.h=16*17;					/* 17 BLOCKS DOWN */
	LoadImage(&rect,(ULONG *)downloadbufferx);

	for (temp=0;temp<21;temp++)
	{
		download=downloadbuffery+(temp*16*16);

		rect.x=320+(temp*8);
		rect.y=0+((((screeny>>4)*16)+(scrollyoff)) MOD 272);
		rect.w=8;					/* 21 BLOCKS ACROSS */
		rect.h=16;
		LoadImage(&rect,(ULONG *)download);
	}
}

/**************************************
*     CALCULATE TEXTURES FOR MAP      *
**************************************/

void
Get_Maptable_Data(void)
{

SHORT	datax,datay;
UBYTE	pagex,pagey,page;
UBYTE	startpagex,startpagey;
SHORT	x,y;
SHORT	startx,starty;

SHORT	downloadx,downloady;

SHORT	downloadoff;

ULONG	*currentblock;
ULONG	*thisblock;
ULONG	block;

SHORT	blockheight;

	x=((screenx MOD 336)>>4)*16;
	y=((screeny MOD 272)>>4)*16;

	if ((screenx MOD 336)< 256) pagex=5;
	else
	{
		pagex=7;
		x-=256;
	}

	if ((screeny MOD 272)< 256) pagey=0;
	else
	{
		pagey=16;
		y-=256;
	}

	startpagex=pagex;
	startpagey=pagey;
	startx=x;
	starty=y;

	for (datay=0;datay<17;datay++)
	{
		for (datax=0;datax<21;datax++)
		{
			maptextures[(datay*21)+datax].xoff=x;
			maptextures[(datay*21)+datax].yoff=y;
			maptextures[(datay*21)+datax].page=pagex+pagey;

			x+=16;

			if ((pagex==7) && (x==80))	/* CHECK FOR RIGHT EDGE */
			{
				pagex=5;
				x=0;
			}

			if ((pagex==5) && (x==256)) 	/* CHECK FOR 2ND PAGE */
			{
				pagex=7;
				x=0;
			}
		}

		pagex=startpagex;
		x=startx;

		y+=16;

		if ((pagey==0) && (y==256))		/* CHECK FOR BOTTOM EDGE */
		{
			pagey=16;
			y=0;
		}

		if ((pagey==16) && (y==16))		/* CHECK FOR 2ND PAGE */
		{
			pagey=0;
			y=0;
		}
	}

	downloadx=((screeny MOD 272)/16);

	for (x=0;x<17;x++)
	{
		block=Map_Block((screenx+scrollxoff)/16,(screeny/16)+x);	/* RESTORE: bug fix, KEPT ON THE MAP (Map_Block) */
		currentblock=(ULONG *)(levelbin+(block*16*16));
		memcpy(downloadbufferx+(downloadx*16*16),currentblock,16*16);

		downloadx++;
		if (downloadx==17) downloadx=0;
	}

	downloady=((screenx MOD 336)>>4);

	for (x=0;x<21;x++)
	{

		block=Map_Block((screenx>>4)+x,(screeny+scrollyoff)/16);	/* RESTORE: bug fix, KEPT ON THE MAP (Map_Block) */
		currentblock=(ULONG *)(levelbin+(block*16*16));
		memcpy(downloadbuffery+(downloady*16*16),currentblock,16*16);

		downloady++;
		if (downloady==21) downloady=0;
	}
}

/**************************************
*  MAKE THE SCROLLER CHASE THE CRAFT  *
**************************************/

void
Update_Scroll(LONG craftx,LONG crafty)
{

LONG	diffx,diffy;

	if (craftdestroyed==YES) return;

	/* RESTORE: bug fix. NMS STOPPED THE VIEW 336 x 272 PIXELS FROM THE MAP'S FAR EDGES, THE SIZE OF THE SCROLLER'S BUFFER, NOT OF
	   THE 320 x 240 SCREEN: THE LAST 16 COLUMNS AND 32 ROWS OF EVERY MAP WERE NEVER SHOWN AND THE CRAFT COULD SINK BELOW THE
	   SCREEN AT THE BOTTOM WALL. THE SATURN (VDP/VDP2_1.C HandleScrollParameters, MapWidth-320) AND THE PC (MAKEBACK/SCROLL.C
	   update_scroll_xy, iff_width-viewable_width) STOP IT AT THE SCREEN'S SIZE; SO DOES THIS (THE BUFFER'S EXTRA BLOCK: Map_Block). */
	diffx=(craftx-160)-screenx;
	diffy=(crafty-120)-screeny;

	if (diffx<0)
	{
		if (screenx!=0)
		{
			if (scrollxoff==0)
			{
				screenx+=diffx;
			}
			scrollxoff=0;
		}
		if (screenx<0) screenx=0;
	}

	if (diffx>0)
	{
		if (screenx!=mapwidth-320)
		{
			if (scrollxoff==320)
			{
				screenx+=diffx;
			}
			scrollxoff=320;
		}
		if (screenx>(mapwidth-320)) screenx=mapwidth-320;
	}

	if (diffy<0)
	{
		if (screeny!=0)
		{
			if (scrollyoff==0)
			{
				screeny+=diffy;
			}
			scrollyoff=0;
		}
		if (screeny<0) screeny=0;
	}

	if (diffy>0)
	{
		if (screeny!=mapheight-240)
		{
			if (scrollyoff==256)
			{
				screeny+=diffy;
			}
			scrollyoff=256;
		}
		if (screeny>(mapheight-240)) screeny=mapheight-240;
	}
}

