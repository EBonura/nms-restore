/******************************************************************/
//
//	VDP2_1.c
//
//	Contains all scroll setup and update routines
//
/******************************************************************/

#include <sgl.h>
#include <libsn.h>
#include	<sega_int.h>
#include "includes\defines.h"
#include "types.h"
#include "globals.h"
#include <stdio.h>

/* ProtoTypes *****************************************************/

void		SetUpVramData(long	xpos,long	ypos);
void		UpdateLeftEdge(UpdateType	*data);
void		UpdateRightEdge(UpdateType	*data);
void		UpdateBottomEdge(UpdateType	*data);
void		UpdateTopEdge(UpdateType	*data);
void		UpdateVerticalEdge(UpdateType	*data);
void		UpdateHorizontalEdge(UpdateType	*data);
void		HandleScrollParameters(void);

/* Externals ******************************************************/

extern	Sint16	ScrollX;
extern	Sint16	ScrollY;
extern	Uint16	MapWidth;
extern	Uint16	MapHeight;
extern	Uint16	MapCellWidth;
extern	Uint16	MapCellHeight;
extern	Uint16	palette[];
extern	BALL 		ball[];
extern	void 		msg(char *ptr);

extern	void 		shCopyBlock(Uint8 *src,Uint8 *dest);
extern	void		InitPalette(Uint16 *palette,Uint8 bankno);
extern	Sint16	LastScrollX;
extern	Sint16	LastScrollY;

/* Globals ********************************************************/

Uint8			UpdateVFlag	=	0;
Uint8			UpdateHFlag	=	0;

UpdateType	HorizontalUpdateList[18];
UpdateType	VerticalUpdateList[22];

/******************************************************************/
/******************************************************************/
/******************************************************************/

void	SetUpVramData(long	xpos,long	ypos)
{
	Uint32	i;
	Uint32	*Map	=	MapData;
	Uint32	DataOffset;
	Uint32	mapindex,vramindex;
	register	Uint16	x;
	register	Uint16	y;
	Uint8		*Dest	=	(Uint8 *)VDP2_VRAM_A0;
	register	long		ax;
	flag		found;

	slTVOff();

	//Need to adjust the scroll parameters to get the ship on the screen
	found=FALSE;
	while(found==FALSE)
	{
		ax	=	ScrollX+160;

		if((ax < xpos) ) ScrollX+=1;

		if((ax>=xpos) || (ScrollX >= (MapWidth-320))) found=TRUE;
	}

	found=FALSE;
	while(found==FALSE)
	{
		ax	=	ScrollY+120;

		if((ax < ypos) ) ScrollY+=1;

		if((ax>=ypos) || (ScrollY >= (MapHeight-240))) found=TRUE;
	}

	LastScrollX = 	ScrollX;
	LastScrollY	=	ScrollY;
	//Clear the data area
	for(i=0;i<(512*256);i++)
	{
		*(Dest+i)	=	0;
	}

	InitPalette(palette,1);

	Map	+=		(ScrollX/16) + ((ScrollY/16)*MapCellWidth);

	//Copy a screen full of data
	for(y=0;y<16;y++)
	{
		for(x=0;x<21;x++)
		{
			mapindex		=	x + (y*MapCellWidth);
			DataOffset	=	(Map[mapindex])*BlockSize;
			vramindex	=	((( (ScrollX+(x*16)) /16)*16)%512)+ (((( (ScrollY+(y*16)) /16)*16)%256)*512);

			shCopyBlock( (LowRam+DataOffset), (Dest+vramindex) );
		}
	}

	slLookR(ScrollX*65536,ScrollY*65536);

	slTVOn();

}

/******************************************************************/

void HandleScrollParameters(void)
{

	register	Uint32	ax,ay,bx,by,cx,cy;
	register	Uint32	X,Y;
	register	Uint32	*Map	=	MapData;
	register	Uint32	c;
	register	Uint32	mapindex,vramindex;

	//Calculate relative centre of screen
	ax	=	ScrollX+160;
	ay	=	ScrollY+120;

	//Get co-ordinates of craft
	bx	=	sprites[craft].x;
	by	=	sprites[craft].y;

	//Set limits on craft movement around the map
	if((bx>(MapWidth-24))||(by>(MapHeight-24))||(bx<24)||(by<24))
	{
		ball[0].vx=0;	  //kill velocities
		ball[0].vy=0;
	}

	//Get distance of craft from relative centre
	cx	=	(MAX(ax,bx))-(MIN(ax,bx));
	cy	=	(MAX(ay,by))-(MIN(ay,by));

	//Heavily optimized scroll update code -
	//Works out where the data has to be updated and the address of the data
	//for each update along the way. This is put into a list for the vblank code

	if((ScrollY>MINY) && (by < ay))
	{
		//scroll upward and update the screen
		LastScrollY		=	ScrollY;
		ScrollY	-=	cy;
		if(ScrollY<0) 	ScrollY=0;
 
		X	=	(ScrollX/16);
		Y	=	(ScrollY/16);

		mapindex	=	((Y*MapCellWidth) + X);

		X			=	((ScrollX/16)*16)%512;
		Y			=	((ScrollY/16)*16)%256;

		for(c=0;c<21;c++)
		{
			VerticalUpdateList[c].mapdata		=	LowRam+((Map[mapindex+c])*BlockSize);
			vramindex								=	(Y*512) + (X%512);
			VerticalUpdateList[c].vram_adr	=	(Uint8 *)VDP2_VRAM_A0+vramindex;
			X											+=	16;
		}

		UpdateVFlag	=	UPDATEVERTICAL;
	}
	else if((ScrollY<MAXY) && (by > ay))
	{
		//scroll downward and update the screen
		LastScrollY			=	ScrollY;
		ScrollY	+=	cy;
		if(ScrollY>MAXY) 	ScrollY=MapHeight;

		X				=	(LastScrollX/16);
		Y				=	((LastScrollY+SCREEN_HEIGHT)/16);
		mapindex		= 	((Y*MapCellWidth) + X);

		X				=	((LastScrollX/16)*16)%512;
		Y				=	(((LastScrollY+SCREEN_HEIGHT)/16)*16)%256;

		for(c=0;c<21;c++)
		{
			VerticalUpdateList[c].mapdata		=	LowRam+((Map[mapindex+c])*BlockSize);
			vramindex								=	(Y*512) + (X%512);
			VerticalUpdateList[c].vram_adr	=	(Uint8 *)VDP2_VRAM_A0+vramindex;
			X											+=	16;
		}

		UpdateVFlag	=	UPDATEVERTICAL;
	}

	if((ScrollX>MINX) && (bx < ax))
	{
		//scroll left and update the screen
		LastScrollX	=	ScrollX;
		ScrollX		-=	cx;
		if(ScrollX<0) ScrollX=0;

		X				=	(ScrollX/16);
		Y				=	(ScrollY/16);
		mapindex		=	X + (Y * MapCellWidth);

		X				=	((ScrollX/16)*16)%512;
		Y				=	((ScrollY/16)*16)%256;

		for(c=0;c<17;c++)
		{
			HorizontalUpdateList[c].mapdata	=	LowRam+((Map[mapindex+(c*MapCellWidth)])*BlockSize);
			vramindex								=	(Y*512) + X;
			HorizontalUpdateList[c].vram_adr	=	(Uint8 *)VDP2_VRAM_A0+vramindex;
			Y											+=	16;
			Y											%=	256;
		}

	  	UpdateHFlag	=	UPDATEHORIZONTAL;
	}
	else if((ScrollX<MAXX) && (bx > ax))
	{
		//scroll right and update the screen
		LastScrollX	=	ScrollX;
		ScrollX		+=	cx;
		if(ScrollX>MAXX) ScrollX=MAXX;
		
		X				=	((LastScrollX+SCREEN_WIDTH)/16);
		Y				=	(LastScrollY/16);
		mapindex		=	X + (Y * MapCellWidth);

		X				=	(((LastScrollX+SCREEN_WIDTH)/16)*16)%512;
		Y				=	((LastScrollY/16)*16)%256;

		for(c=0;c<17;c++)
		{
			HorizontalUpdateList[c].mapdata	=	LowRam+((Map[mapindex+(c*MapCellWidth)])*BlockSize);
			vramindex								=	(Y*512) + X;
			HorizontalUpdateList[c].vram_adr	=	(Uint8 *)VDP2_VRAM_A0+vramindex;
			Y											+=	16;
			Y											%=	256;
		}

	  	UpdateHFlag	=	UPDATEHORIZONTAL;
	}

}

/******************************************************************/

void	UpdateVerticalEdge(UpdateType	*data)
{
	register		Uint8		x;

	for(x=0;x<21;x++)
	{
		shCopyBlock(data[x].mapdata,data[x].vram_adr);
	}

}

/******************************************************************/

void	UpdateHorizontalEdge(UpdateType	*data)
{
	register		Uint8		x;

	for(x=0;x<16;x++)
	{
		shCopyBlock(data[x].mapdata,data[x].vram_adr);
	}
}

/******************************************************************/




























