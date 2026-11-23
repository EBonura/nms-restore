/******************************************************************/
//
// VDP1_3.C
//
// Contains SCS data decoder
//
/******************************************************************/

#include <sgl.h>
#include <libsn.h>
#include "includes\defines.h"
#include	"includes\types.h"
#include	<stdio.h>
#include	"names.h"

/* Globals ********************************************************/

//Uint8	Cluts[(MAXSPRITENUM*32)];

/* ProtoTypes *****************************************************/

void			DecodeSCS(Uint16 tframe,Uint32 *src,Uint8 *dst,Uint16 Check,SpriteType *Sprite);
void			Rle2raw(Uint8 *source,Uint8 *dest);
void 			Bit2raw(Uint8 *source,Uint8 *dest);
void			DownLoadCluts(Uint8	*Data,Uint16 	Offset);

/* Externals ******************************************************/

extern	Uint8			DecompBuffer[];

extern	Uint32		*DD;

extern	Uint8			GFXBuffer[];

extern	Uint16		DCount;
	
extern	SlotType		Slots;

extern	Uint32		SpriteMAX;
extern	Uint32		Goffset[];

extern	TransType	TransferList[];
extern	Uint32		TransIndex;
extern	Uint32		SourceOffset;


extern	SlotFunc		GetSlot[];
extern	CXS			CheckSprite[];

//extern	SwapType 	GetEmptySlot(Uint8 slotsize, SlotType *SlotData);

extern	void			qmemcpy(void *dst,void *src,Uint32 len);
extern	void			qmemset(void *dst,Uint32 val,Uint32 len);
extern	void			memset(void *dst,Uint32 val,Uint32 len);
extern	void 			shBit2raw(Uint8 *source,Uint8 *dest);
extern	void			shRle2raw(Uint8 *source,Uint8 *dest);
extern	void 			msg(char *ptr);

extern	char			stat_str[];

extern	Uint16		spnum;

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

/******************************************************************/
/******************************************************************/

void	DecodeSCS(Uint16 tframe,Uint32 *src,Uint8 *dst,Uint16 Check,SpriteType *Sprite)
{
	Uint16		NumFrames;
	Uint16		ClearCol;
	Uint32		ImageOffset;
	Uint32		Width;
	Uint32		Height;
	Sint32		OffsetX;
	Sint32		OffsetY;
	Uint32		SlotSize;
	Uint32		CmpFlag;
	Uint32		*source	=	src;
	Uint8			y;
	Uint8			*isrc,*dsrc;
	SwapType		data;
	RetType		result;
	Uint32		spnumt;
	register		Uint32		i;
	Uint16		frame	=	tframe;
	Uint8			*Dest	=	(Uint8 *)SpriteVRAM+SpriteStart;
	



	NumFrames	=	*((Uint16 *)source);

	ClearCol		=	*((Uint16 *)source+1);

	if(frame >= NumFrames)
	{
		frame=	0;
	}

	spnumt		=	frame;
	
	spnumt		*=	7;

	source		= 	(Uint32 *)(src + 1);

	source		+=	spnumt;	//offset to the frame data in the header

	Height		=	*((src+1)+HEIGHT);	  //src+1 to skip first longword

	Sprite->cry	=	(Height/2);

	OffsetX		=	*((Sint32 *)(source+OFFSETX));
	OffsetY		=	*((Sint32 *)(source+OFFSETY));
	SlotSize		=	*(source+SLOTSIZE);
	Width			=	*(source+WIDTH);
	Height		=	*(source+HEIGHT);


	Sprite->offx	=	(Sint16)OffsetX;
	Sprite->offy	=	(Sint16)OffsetY;


	CheckSprite[SlotSize](Check,&Slots,&result);
 
	if(result.Return == -1)
	{
		
		//Read this in here as it is only required if decompression is to
		//take place

		ImageOffset	=	*(source+IMAGEOFFSET);
		CmpFlag		=	*(source+CMPFLAG);

		GetSlot[(SlotSize)](&Slots,Check,&data);
		
		if(data.offset	==	0xffffffff)
		{
			sprintf(stat_str,"Slot Allocation Error:%d",(int)SlotSize);
			msg(stat_str);
		}

		Sprite->texno=	data.texno;
		
		isrc			=	(Uint8 *)src;
		
		isrc			+=	(ImageOffset);
		
		Dest	+=	data.offset;

		//Clear data decompress areas + Clear VRAM
		for(i=0;i<data.ClearArea;i++)
		{	
			*(DecompBuffer+i)	=	0x0;
			*(Dest+i)			=	0x0;
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
		
		//Got the image so now copy to GFX Buffer
		
		isrc		=	DecompBuffer;
		dsrc		=	dst+data.offset;

		for(y=0;y<(result.clx);y++)
		{
			memcpy(dsrc,isrc,(Width/2));
			isrc+=Width/2;
			dsrc+=data.Delta;
		}
		
		isrc			=	(Uint8 *)src;
		isrc			+=	((NumFrames*28)+4);	//calculate address of CLUT data
		
		DownLoadCluts(isrc,data.texno);
		
	}
	else
	{
		Sprite->texno=	result.Return;
	}

	Sprite->cx	=	result.cx;
	Sprite->cy	=	result.cy;
	
	//return(Sprite);
}

/******************************************************************/

void	DownLoadCluts(Uint8	*Data,Uint16 Offset)
{

	Uint8		*i	=	(Uint8 *)(SpriteVRAM+CLUTSTART+(Offset*32));

	slTransferEntry(Data,i,32);

}

/******************************************************************/



