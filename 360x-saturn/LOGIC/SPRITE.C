/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Sprite Draw & Remove Functions (SATURN VERSION!) */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include	<sgl.h>
#include "defines.h"
#include "types.h"
#include "globals.h"
#include <stdio.h>
#include <libsn.h>

/* ﬂ Prototypes ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void		DrawBackGroundSprites(void);
void		DrawSpecialSprites(void);
flag 		collision_at(long x,long y);

/* ﬂ Externals ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

extern	Uint16		ScrollX;
extern	Uint16		ScrollY;
extern	Uint16		LastScrollX;
extern	Uint16		LastScrollY;

extern	Uint8			GFXBuffer[];		//Buffer for GFX transfer
extern	Uint8			BigBuffer[];		//GFX Load area
extern	Uint32		Goffset[];
							
extern	Uint32		SpriteNumberOffset[];

extern	void			DecodeSCS(Uint16 tframe,Uint32 *src,Uint8 *dst,Uint16 Check,SpriteType *Sprite);
extern	Uint32		SourceOffset;
extern	void 			msg(char *ptr);

extern	char			stat_str[];
extern	Uint16		spnum;
extern	Uint16		ShipX;
extern	Uint16		ShipY;
extern	long			testname;
extern	Uint16		ActiveSprites;
extern	Uint8			*collmap;
extern	Uint16		MapWidth;
extern	flag 			SHcollision_at(long x,long y);

/* ﬂ Globals ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

#ifdef	DEBUG
Uint16		DCount	=	0;
#endif

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
#define	BOX	40
//|MSBon

void	DrawBackGroundSprites(void)
{
	register		Uint16		i=0,x,y;
	register		Sint32		screenx,screeny;
	static		SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,0x0000,No_Window|No_Gouraud,CL16Look|ECdis|MESHoff|CL_Replace,sprNoflip);
	FIXED			Pos[4];
	register		Uint32		*src;
	register		Uint8			*dst;
	SpriteType	Sprite;
	long			name;
	Uint16		Check;
	Uint16		frame;
	Uint16		priority;
	flag			decoded;
	spritestruct	*lsprites	=	sprites;		//local pointer for faster indexing of the global array
	#ifdef		DEBUG
	Uint16		PCount	=	0;
	#endif

	Pos[3]	=	ORIGINAL*65536;

	
	screenx	=	ScrollX;
	screeny	=	ScrollY;

	for(i=0;i<MAX_SPRITES;i++)
	{
		if((lsprites[i].on == TRUE) && (lsprites[i].visible == YES) && (lsprites[i].still==NO) )
		{
			decoded	=	FALSE;

			x	=	lsprites[i].x;
			y	=	lsprites[i].y;
	 															 
			//Check if sprite is in view
			if( (x	<	(screenx+(SCREEN_WIDTH+BOX))) &&
				 (x   >  (screenx-BOX)) &&
				 (y   >  (screeny-BOX)) &&
				 (y	<  (screeny+(SCREEN_HEIGHT+BOX))) )
			{
			
				#ifdef	DEBUG
				PCount++;
				#endif
			
				name	=	lsprites[i].name;
				frame	=	lsprites[i].frame;
			 
				(char *)src	=	seqdata[name].data;
				Check				=	seqdata[name].offset;
				Check				+=	frame;
				dst				=	((Uint8 *)SpriteVRAM+SpriteStart);
						
				if(src!=(void *)0xffffffff)
				{
 					DecodeSCS(frame,src,dst,Check,&Sprite);
					Normal.colno	=	((CLUTSTART+(Sprite.texno*32))/8);
					decoded=TRUE;
				}
				else
				{
					sprintf(stat_str,"Bad sprite data for :%d\n",(int)name);
					msg(stat_str);
				}
	
				if(decoded==TRUE)
				{
					Normal.texno	=	Sprite.texno;

					x	-=	screenx;
					y	-=	screeny;
				
					x	+=	Sprite.offx;
					y	+=	Sprite.offy;
				
			  		x	+=	Sprite.cx;
			  		y	+=	Sprite.cy;
				
			  		y	+=	Sprite.cry;

					priority	=	(170 + lsprites[i].priority);
				
					Pos[0]	=	x*65536;
					Pos[1]	=	y*65536;
					Pos[2]	=	priority*65536;

					slDispSprite(Pos,&Normal,0);
				}
			}
		}
	}

	#ifdef DEBUG

	sprintf(stat_str,"Print Count:%d  ",PCount);
	msg(stat_str);
	sprintf(stat_str,"Decode Count:%d  ",DCount);
	msg(stat_str);

	#endif
}


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

//Routine to draw 256 colour and still sprites
void	DrawSpecialSprites(void)
{
	register		Uint16		i=0,x,y;
	register		Sint32		screenx,screeny;
	static		SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,0x0000,No_Window|No_Gouraud,CL16Look|ECdis|MESHoff|CL_Replace,sprNoflip);
	FIXED			Pos[4];
//	register		Uint32		*src;
//	register		Uint8			*dst;
	SpriteType	Sprite;
	long			name;
//	Uint16		Check;
	Uint16		frame;
	Uint16		priority;
	flag			decoded;
	spritestruct	*lsprites	=	sprites;		//local pointer for faster indexing of the global array
	#ifdef		DEBUG
	Uint16		PCount	=	0;
	#endif

	Pos[3]	=	ORIGINAL*65536;

	screenx	=	ScrollX;
	screeny	=	ScrollY;

	for(i=0;i<MAX_SPRITES;i++)
	{
		if((lsprites[i].on	== TRUE) && (lsprites[i].visible == YES) && (lsprites[i].still==YES) )
		{
			decoded	=	FALSE;

			x	=	lsprites[i].x;
			y	=	lsprites[i].y;
	 															 
			//Check if sprite is in view
			if( (x	<	(screenx+(SCREEN_WIDTH+BOX))) &&
				 (x   >  (screenx-BOX)) &&
				 (y   >  (screeny-BOX)) &&
				 (y	<  (screeny+(SCREEN_HEIGHT+BOX))) )
			{
			
				#ifdef	DEBUG
				PCount++;
				#endif
			
				name	=	lsprites[i].name;
				frame	=	lsprites[i].frame;
				
				switch(name)
				{
					case 	NAME_vinel11:
					case	NAME_v1_1:
					{
						Normal.colno	=	(VIL1CLUT/8);
						Sprite.texno	=	VIL1SPRITE;
						Sprite.offx		=	-28;
						Sprite.offy		=	-91;
						Sprite.cx		=	(56/2);
						Sprite.cy		=	(91/2);
						Sprite.cry		=	(91/2);
						decoded=TRUE;
						break;
					}

					case	NAME_vines12:
					case	NAME_v1_2:
					{
						Normal.colno	=	(VIL2CLUT/8);
						Sprite.texno	=	VIL2SPRITE;
						Sprite.offx		=	-16;
						Sprite.offy		=	-86;
						Sprite.cx		=	(32/2);
						Sprite.cy		=	(86/2);
						Sprite.cry		=	(86/2);

						decoded=TRUE;
						break;
					}

					case	NAME_vine13:
					case	NAME_v1_3:
					{
						Normal.colno	=	(VIL3CLUT/8);
						Sprite.texno	=	VIL3SPRITE;
						Sprite.offx		=-16;
						Sprite.offy		=-50;
						Sprite.cx		=(32/2);
						Sprite.cy		=(50/2);
						Sprite.cry		=(50/2);
						decoded=TRUE;
						break;
					}

					case	NAME_vine14:
					case	NAME_v1_4:
					{
						Normal.colno	=	(VIL4CLUT/8);
						Sprite.texno	=	VIL4SPRITE;
						Sprite.offx		=-16;
						Sprite.offy		=-38;
						Sprite.cx		=(32/2);
						Sprite.cy		=(38/2);
						Sprite.cry		=(38/2);
						decoded=TRUE;
						break;
					}

					case	NAME_vine15:
					case	NAME_v1_5:
					{
						Normal.colno	=	(VIL5CLUT/8);
						Sprite.texno	=	VIL5SPRITE;
						Sprite.offx		=-8;
						Sprite.offy		=-36;
						Sprite.cx		=(16/2);
						Sprite.cy		=(36/2);
						Sprite.cry		=(36/2);
						decoded=TRUE;
						break;
					}

					case	NAME_vine16:
					case	NAME_v1_6:
					{
						Normal.colno	=	(VIL6CLUT/8);
						Sprite.texno	=	VIL6SPRITE;
						Sprite.offx		=-12;
						Sprite.offy		=-101;
						Sprite.cx		=(24/2);
						Sprite.cy		=(101/2);
						Sprite.cry		=(101/2);
						decoded=TRUE;
						break;
					}

					//case	NAME_vine17:
					case	NAME_v1_7:
					{
						Normal.colno	=	(VIL7CLUT/8);
						Sprite.texno	=	VIL7SPRITE;
						Sprite.offx		=-8;
						Sprite.offy		=-27;
						Sprite.cx		=(16/2);
						Sprite.cy		=(27/2);
						Sprite.cry		=(27/2);
						decoded=TRUE;
						break;
					}

					//case	NAME_vine18:
					case	NAME_v1_8:
					{
						Normal.colno	=	(VIL8CLUT/8);
						Sprite.texno	=	VIL8SPRITE;
						Sprite.offx		=-16;
						Sprite.offy		=-58;
						Sprite.cx		=(32/2);
						Sprite.cy		=(58/2);
						Sprite.cry		=(58/2);
						decoded=TRUE;
						break;
					}
					
					//case	NAME_vine19:
					case	NAME_v1_9:
					{
						Normal.colno	=	(VIL9CLUT/8);
						Sprite.texno	=	VIL9SPRITE;
						Sprite.offx		=-12;
						Sprite.offy		=-36;
						Sprite.cx		=(24/2);
						Sprite.cy		=(36/2);
						Sprite.cry		=(36/2);
						decoded=TRUE;
						break;
					}

					case	NAME_v1_10:
					{
						Normal.colno	=	(VIL10CLUT/8);
						Sprite.texno	=	VIL10SPRITE;
						Sprite.offx		=-12;
						Sprite.offy		=-66;
						Sprite.cx		=(24/2);
						Sprite.cy		=(66/2);
						Sprite.cry		=(66/2);
						decoded=TRUE;
						break;
					}

					case	NAME_v1_11:
					{
						Normal.colno	=	(VIL11CLUT/8);
						Sprite.texno	=	VIL11SPRITE;
						Sprite.offx		=-12;
						Sprite.offy		=-28;
						Sprite.cx		=(24/2);
						Sprite.cy		=(28/2);
						Sprite.cry		=(28/2);
						decoded=TRUE;
						break;
					}

					case	NAME_v1_12:
					{
						Normal.colno	=	(VIL12CLUT/8);
						Sprite.texno	=	VIL12SPRITE;
						Sprite.offx		=-16;
						Sprite.offy		=-44;
						Sprite.cx		=(32/2);
						Sprite.cy		=(44/2);
						Sprite.cry		=(44/2);
						decoded=TRUE;
						break;
					}

					case	NAME_v1_13:
					{
						break;
					}

					case	NAME_portdl:
					{
						Normal.colno	=	(PORTDLCLUT/8);
						Sprite.texno	=	PORTDLSPRITE;
						Sprite.offx		=PORTDLOX;
						Sprite.offy		=PORTDLOY;
						Sprite.cx		=(40/2);
						Sprite.cy		=(9/2);
						Sprite.cry		=(9/2);
						decoded=TRUE;
						break;

					}

					case	NAME_portdu:
					{
						Normal.colno	=	(PORTDUCLUT/8);
						Sprite.texno	=	PORTDUSPRITE;
						Sprite.offx		=PORTDUOX;
						Sprite.offy		=PORTDUOY;
						Sprite.cx		=(48/2);
						Sprite.cy		=(14/2);
						Sprite.cry		=(14/2);
						decoded=TRUE;
						break;

					}

					case	NAME_portll:
					{
						Normal.colno	=	(PORTLLCLUT/8);
						Sprite.texno	=	PORTLLSPRITE;
						Sprite.offx		=PORTLLOX;
						Sprite.offy		=PORTLLOY;
						Sprite.cx		=(16/2);
						Sprite.cy		=(35/2);
						Sprite.cry		=(35/2);
						decoded=TRUE;
						break;

					}

					case	NAME_portlu:
					{
						Normal.colno	=	(PORTLUCLUT/8);
						Sprite.texno	=	PORTLUSPRITE;
						Sprite.offx		=PORTLUOX;
						Sprite.offy		=PORTLUOY;
						Sprite.cx		=(24/2);
						Sprite.cy		=(49/2);
						Sprite.cry		=(49/2);
						decoded=TRUE;
						break;

					}

					case	NAME_portrl:
					{
						Normal.colno	=	(PORTRLCLUT/8);
						Sprite.texno	=	PORTRLSPRITE;
						Sprite.offx		=PORTRLOX;
						Sprite.offy		=PORTRLOY;
						Sprite.cx		=(8/2);
						Sprite.cy		=(36/2);
						Sprite.cry		=(36/2);
						decoded=TRUE;
						break;

					}

					case	NAME_portru:
					{
						Normal.colno	=	(PORTRUCLUT/8);
						Sprite.texno	=	PORTRUSPRITE;
						Sprite.offx		=PORTRUOX;
						Sprite.offy		=PORTRUOY;
						Sprite.cx		=(24/2);
						Sprite.cy		=(51/2);
						Sprite.cry		=(51/2);
						decoded=TRUE;
						break;

					}

					case	NAME_portul:
					{
						Normal.colno	=	(PORTULCLUT/8);
						Sprite.texno	=	PORTULSPRITE;
						Sprite.offx		=PORTULOX;
						Sprite.offy		=PORTULOY;
						Sprite.cx		=(32/2);
						Sprite.cy		=(8/2);
						Sprite.cry		=(8/2);
						decoded=TRUE;
						break;

					}

					case	NAME_portuu:
					{
						Normal.colno	=	(PORTUUCLUT/8);
						Sprite.texno	=	PORTUUSPRITE;
						Sprite.offx		=PORTUUOX;
						Sprite.offy		=PORTUUOY;
						Sprite.cx		=(48/2);
						Sprite.cy		=(16/2);
						Sprite.cry		=(16/2);
						decoded=TRUE;
						break;

					}

					case	NAME_portur:
					{
						break;
					}

					default:
					{
						sprintf(stat_str,"Static sprite not printed :%d\n",(int)name);
						msg(stat_str);
					 	break;
					}
				}
				
				if(decoded==TRUE)
				{
					Normal.texno	=	Sprite.texno;

					x	-=	screenx;
					y	-=	screeny;
				
					x	+=	Sprite.offx;
					y	+=	Sprite.offy;
				
			  		x	+=	Sprite.cx;
			  		y	+=	Sprite.cy;
				
			  		y	+=	Sprite.cry;

					priority	=	(170 + lsprites[i].priority);
				
					Pos[0]	=	x*65536;
					Pos[1]	=	y*65536;
					Pos[2]	=	priority*65536;

					slDispSprite(Pos,&Normal,0);
				}
			}
		}
	}

}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

#if 0
flag collision_at(long x,long y)
{
	Uint8 	mask;
	Uint8 	bit;

	mask	=	collmap[( (x+y*MapWidth)>>3 )];

	bit=1<<(7-(x&7));
	
	if( (mask&bit)!=0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	return(SHcollision_at(x,y));

}

#endif

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Sprite Collision with background */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag sprite_collision(sprite_sequence *seq,int sprite_no,int x,int y)
{
	return FALSE;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

long find_nearest_sprite(long x,long y)
{
	register	long i;
	register	long dx,dy,dist;
	register	long best,best_dist;	

	best=-1;
	best_dist=(320*320)+(240*240);
	
	for(i=0;i<MAX_SPRITES;i++)
	{
		if( (sprites[i].on==TRUE) && (sprites[i].shootable==YES) )
		{
			dx=sprites[i].x-x;
			dy=sprites[i].y-y;
			dist=(dx*dx)+(dy*dy);
			if(dist<best_dist)
			{
				best_dist=dist;
				best=i;
			}
		}
	}
	return best;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of file */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */










					#if 0
					case	NAME_v1_13:
					{
						Normal.colno	=	(VIL13CLUT/8);
						Sprite.texno	=	VIL13SPRITE;
						Sprite.offx=-20;
						Sprite.offy=-26;
						Sprite.cx=(40/2);
						Sprite.cy=(26/2);
						Sprite.cry=(26/2);
						decoded=TRUE;
						break;
					}

					case	NAME_donkey:
					{
						(char *)src	=	seqdata[name].data;
						Check				=	seqdata[name].offset;
						Check				+=	frame;
						dst				=	((Uint8 *)SpriteVRAM+SpriteStart);
						
						if(src!=(void *)0xffffffff)
						{
 							DecodeSCS(frame,src,dst,Check,&Sprite);
							Normal.colno	=	((CLUTSTART+(Sprite.texno*32))/8);
							decoded=TRUE;
						}
						else
						{
							sprintf(stat_str,"Bad sprite data for :%d\n",(int)name);
							msg(stat_str);
						}

						break;
					}
					#endif


					//case	NAME_tele1:
					//case	NAME_tele2:
					//case	NAME_teletemp:
					//{
						//This will need a different decode routine for 256 colour sprites
						//break;
					//}
