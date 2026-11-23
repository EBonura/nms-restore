/******************************************************************/
//
// VDP1_2.C
//
// Contains panel stuff + static sprite setup
//
/******************************************************************/

#include <sgl.h>
#include <libsn.h>
#include "includes\defines.h"
#include	"includes\types.h"
#include <stdio.h>

//#include	"names.h"

/* Misc ***********************************************************/

enum	{
			IMAGEOFFSET=0,
			WIDTH,
			HEIGHT,
			OFFSETX,
			OFFSETY,
			SLOTSIZE,
			CMPFLAG
		};

enum	{
			BUCKSHOT=0,
			COOLSHOT,
			EBALL,
			LASER,
			MISSILE,
			MINE,
		};


/* Globals ********************************************************/

Uint8		shipframe		=	0;
Uint8		missileframe	=	0;
Uint8		barslframe		=	0;
Uint8		barsrframe		=	0;

//Wireframe ship
Uint8		shipwire[7331];

//Weapon wire frames
Uint8		bkwire[2690];
Uint8		cmnwir[4648];
Uint8		ebwire[2872];
Uint8		lzwire[2914];
Uint8		misswire[2062];
Uint8		mnwire[3714];

//status bars
Uint8		barsl[2362];
Uint8		barsr[2362];

/* ProtoTypes *****************************************************/

void		InitPanel(void);
void		PrintPanel(void);
void		DecodeGfx(Uint8 frame,Uint32 *src,Uint8 *dst,Uint16 width,Uint16 height,SpriteType *Sprite);
void		DisplayWeaponWireframe(void);
void 		Bit2raw(Uint8 *source,Uint8 *dest);
void		Rle2raw(Uint8 *source,Uint8 *dest);

/* Externals ******************************************************/

extern	void			memset(void *dst,Uint32 val,Uint32 len);

extern	void			ReadFile(Uint8 *name,Uint8 *target);

extern	void 			shBit2raw(Uint8 *source,Uint8 *dest);
extern	void			shRle2raw(Uint8 *source,Uint8 *dest);

extern	long			testname;

extern	char			stat_str[];
extern	Uint8			DecompBuffer[];
extern	Uint8			BigBuffer[];
extern	Uint32		*DD;
extern	Uint32		score;
extern	void			PrintLARGE(Uint16 x,Uint16 y,Uint8 *string);
extern	void			PrintMEDIUM(Uint16 x,Uint16 y,Uint8 *string);
extern	void			PrintSMALL(Uint16 x,Uint16 y,Uint8 *string);
extern	Uint8			GamePauseFlag;
extern	Uint8			incflag;
extern	Uint8			weapontype;

/******************************************************************/
/******************************************************************/

void	InitPanel(void)
{
	Uint8		border[592];
	Uint8		*vram	=	(Uint8 *)SpriteVRAM;


	//Load ship wireframe
	ReadFile("sprites/s_swire.scs",shipwire);

	//Load weapon wireframes
	ReadFile("sprites/s_bkwire.scs",bkwire);
	ReadFile("sprites/s_cmnwir.scs",cmnwir);
	ReadFile("sprites/s_ebwire.scs",ebwire);
	ReadFile("sprites/s_lzwire.scs",lzwire);
	ReadFile("sprites/s_misswr.scs",misswire);
	ReadFile("sprites/s_mnwire.scs",mnwire);
	
	//I can decode and get rid of this
	ReadFile("bin/border.bin",border);
	shRle2raw(&border[32],(vram+BORDERSTART));

	//Copy the Cluts to VRAM
	memcpy((Uint8 *)SpriteVRAM+BORDERCLUT,border,32);
	
	ReadFile("sprites/barsl.scs",barsl);
	ReadFile("sprites/barsr.scs",barsr);

	memcpy(vram+BARSLCLUT,&barsl[368],32);
	memcpy(vram+BARSRCLUT,&barsr[368],32);

	shipwire[1796+2]=0x8f;
	shipwire[1796+3]=0x00;

  	memcpy(vram+SHIPWIRECLUT,&shipwire[1796],32);

	misswire[452+2]=0x8f;
	misswire[452+3]=0x00;

	memcpy(vram+MISSWIRECLUT,&misswire[452],32);

}

/******************************************************************/

void	SetUpStaticSprites(void)
{

	ReadFile("sprites/s_v1_1.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL1CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL1START);

	ReadFile("sprites/s_v1_2.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL2CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL2START);

	ReadFile("sprites/s_v1_3.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL3CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL3START);

	ReadFile("sprites/s_v1_4.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL4CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL4START);

	ReadFile("sprites/s_v1_5.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL5CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL5START);

	ReadFile("sprites/s_v1_6.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL6CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL6START);

	ReadFile("sprites/s_v1_7.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL7CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL7START);

	ReadFile("sprites/s_v1_8.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL8CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL8START);

	ReadFile("sprites/s_v1_9.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL9CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL9START);

	ReadFile("sprites/s_v1_10.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL10CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL10START);

	ReadFile("sprites/s_v1_11.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL11CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL11START);

	ReadFile("sprites/s_v1_12.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+VIL12CLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+VIL12START);

	ReadFile("sprites/portdl.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+PORTDLCLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+PORTDLSTART);

	ReadFile("sprites/portdu.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+PORTDUCLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+PORTDUSTART);

	ReadFile("sprites/portll.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+PORTLLCLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+PORTLLSTART);

	ReadFile("sprites/portlu.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+PORTLUCLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+PORTLUSTART);

	ReadFile("sprites/portrl.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+PORTRLCLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+PORTRLSTART);

	ReadFile("sprites/portru.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+PORTRUCLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+PORTRUSTART);

	ReadFile("sprites/portul.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+PORTULCLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+PORTULSTART);

	ReadFile("sprites/portuu.scs",BigBuffer);
	memcpy((Uint8 *)SpriteVRAM+PORTUUCLUT,&BigBuffer[(8*4)],32);
	shRle2raw(&BigBuffer[((8*4)+32)],(Uint8 *)SpriteVRAM+PORTUUSTART);

}

/******************************************************************/

void	PrintPanel(void)
{
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,0x0000,No_Window|No_Gouraud,CL16Look|ECdis|MESHoff|CL_Replace|SPenb,sprNoflip);
	FIXED			Pos[4];
	Uint8			*vram		=	(Uint8 *)SpriteVRAM;
	SpriteType	Sprite;
	Uint16		x,y;
	char			score_str[7];

	//Print the border
	Pos[0]	=	280*65536;
	Pos[1]	=	42*65536;
	Pos[2]	=	170*65536;
	Pos[3]	=	ORIGINAL*65536;

	Normal.texno	=	BORDERSPRITE;
	Normal.colno	=	(BORDERCLUT/8);
	slDispSprite(Pos,&Normal,0);
	
	//Decode and print the bars
	DecodeGfx(barslframe,(Uint32 *)barsl,(vram+BARSLSTART),16,27,&Sprite);

	Pos[0]	=	263*65536;
	Pos[1]	=	25*65536;

	Normal.texno	=	BARSLSPRITE;
	Normal.colno	=	(BARSLCLUT/8);
	slDispSprite(Pos,&Normal,0);

	DecodeGfx(barsrframe,(Uint32 *)barsr,(vram+BARSRSTART),16,27,&Sprite);

	Pos[0]	=	298*65536;
	Pos[1]	=	25*65536;

	Normal.texno	=	BARSRSPRITE;
	Normal.colno	=	(BARSRCLUT/8);
	slDispSprite(Pos,&Normal,0);

	//Decode and print the wireframe ship
	DecodeGfx(shipframe,(Uint32 *)shipwire,(vram+SHIPWIRESTART),24,16,&Sprite);

	x	=	296;
	x	+=	Sprite.offx;

	y	=	36;
	y	+=	Sprite.offy;
	y	+=	Sprite.cry;

	Pos[0]	=	x*65536;
	Pos[1]	=	y*65536;

	Normal.texno	=	SHIPWIRESPRITE;
	Normal.colno	=	(SHIPWIRECLUT/8);
	slDispSprite(Pos,&Normal,0);

	DisplayWeaponWireframe();

	if(GamePauseFlag!=PAUSED && (incflag==1))
	{
		shipframe++;
		shipframe &= 63;

		missileframe++;

		if(weapontype==MISSILE)
		{
			missileframe &=15;
		}
		else
		{
			missileframe &=31;
		}

		barslframe++;
		if(barslframe>12) barslframe	=	0;
		barsrframe++;
		if(barsrframe>12) barsrframe	=	0;
	}

	sprintf(score_str,"%07d",(int)score);
	
	PrintSMALL(270,65,score_str);

}

/******************************************************************/

void	DisplayWeaponWireframe(void)
{
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,(MISSWIRECLUT/8),No_Window|No_Gouraud,CL16Look|ECdis|MESHoff|CL_Replace|SPenb,sprNoflip);
	FIXED			Pos[4];
	Uint8			*vram		=	(Uint8 *)SpriteVRAM;
	SpriteType	Sprite;
	Uint16		x,y;
	Uint16		current_weapon	=	weapontype;

	Pos[2]	=	170*65536;
	Pos[3]	=	ORIGINAL*65536;

	switch(current_weapon)
	{
		case	MISSILE:
		{
			if(missileframe >= 16) missileframe = 0;
			DecodeGfx(missileframe,(Uint32 *)misswire,(vram+MISSWIRESTART),24,14,&Sprite);
			Normal.texno	=	MISSWIRESPRITE;
			break;
		}

		case	LASER:
		{
			DecodeGfx(missileframe,(Uint32 *)lzwire,(vram+LZWIRESTART),24,12,&Sprite);
			Normal.texno	=	LZWIRESPRITE;
			break;
		}

		case	BUCKSHOT:
		{
			//Decode and print the wireframe missile
			DecodeGfx(missileframe,(Uint32 *)bkwire,(vram+BKWIRESTART),16,14,&Sprite);
			Normal.texno	=	BKWIRESPRITE;
			break;
		}

		case	COOLSHOT:
		{
			DecodeGfx(missileframe,(Uint32 *)cmnwir,(vram+CMNWIRESTART),32,15,&Sprite);
			Normal.texno	=	CMNWIRESPRITE;
			break;
		}

		case	EBALL:
		{
			DecodeGfx(missileframe,(Uint32 *)ebwire,(vram+EBWIRESTART),16,13,&Sprite);
			Normal.texno	=	EBWIRESPRITE;
			break;
		}

		case	MINE:
		{
			DecodeGfx(missileframe,(Uint32 *)mnwire,(vram+MNWIRESTART),16,14,&Sprite);
			Normal.texno	=	MNWIRESPRITE;
			break;
		}

		default:
		{
			break;
		}

	}

	x	=	290;
	x	+=	Sprite.offx;

	y	=	56;
	y	+=	Sprite.offy;
	y	+=	Sprite.cry;

	Pos[0]	=	x*65536;
	Pos[1]	=	y*65536;

	slDispSprite(Pos,&Normal,0);


}

/******************************************************************/

void	DecodeGfx(Uint8 frame,Uint32 *src,Uint8 *dst,Uint16 swidth,Uint16 sheight,SpriteType *SpriteInfo)
{

	Uint16		NumFrames;
	Uint16		ClearCol;
	Uint32		ImageOffset;
	Uint32		Width;
	Uint32		Height;
	Sint32		OffsetX;
	Sint32		OffsetY;
	Uint32		CmpFlag;
	Uint32		*source	=	(Uint32 *)src;
	Uint8			y;
	Uint8			*isrc,*dsrc;
	Uint32		spnumt;
	register		Uint32		i;


	NumFrames	=	*((Uint16 *)source);

	ClearCol		=	*((Uint16 *)source+1);

	spnumt		=	frame;

	source++;
	
	spnumt		*=	7;

	source		+=	spnumt;	//offset to the frame data in the header

	Height		=	*((src+1)+HEIGHT);	  //src+1 to skip first longword

	SpriteInfo->cry	=	(Height/2);

	OffsetX		=	*((Sint32 *)(source+OFFSETX));
	OffsetY		=	*((Sint32 *)(source+OFFSETY));
	Width			=	*(source+WIDTH);

	SpriteInfo->offx	=	(Sint16)OffsetX;
	SpriteInfo->offy	=	(Sint16)OffsetY;

	ImageOffset	=	*(source+IMAGEOFFSET);
	CmpFlag		=	*(source+CMPFLAG);

	isrc			=	(Uint8 *)src;
	
	isrc			+=	(ImageOffset);
	dsrc			=	dst;

	//Clear data reception areas
	for(i=0;i<((swidth*sheight)/2);i++)
	{
		*(DecompBuffer+i)	=	0;
		*(dst+i)	=	0;
	}

	switch(CmpFlag)
	{
		case	RUNLENGTH:
		{
			shRle2raw(isrc,DecompBuffer);
			break;
		}
	
		case	BITSTREAM:
		{
			shBit2raw(isrc,DecompBuffer);
			break;
		}
	}
	
	isrc	=	DecompBuffer;

	for(y=0;y<sheight;y++)
	{
		memcpy(dsrc,isrc,(Width/2));
		isrc+=Width/2;
		dsrc+=swidth/2;
	}

}
				  
/******************************************************************/


