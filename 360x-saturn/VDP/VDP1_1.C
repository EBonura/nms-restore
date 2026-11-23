/******************************************************************/
//
// VDP1_1.C
//
// Contains VDP1 VRAM manager
//
//
/******************************************************************/

#include	<sgl.h>
#include <libsn.h>
#include "includes\defines.h"
#include	"includes\types.h"

/* ProtoTypes *****************************************************/

void		InitialiseSpriteArrays(SlotType *SlotData);
void		ClearSlotAllocationArrays(SlotType *SlotData);

void		Get8x8Slot(SlotType *SlotData,Uint16 Sprite,SwapType *SwapInfo);
void		Get16x16Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo);
void		Get24x24Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo);
void		Get32x32Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo);
void		Get48x48Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo);
void		Get64x64Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo);
void		Get80x80Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo);
void		Get96x96Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo);
void		Get128x128Slot(SlotType *SlotData,Uint16 Sprite,SwapType *SwapInfo);

void		CheckSprite8x8(Sint16 Frame,SlotType *SlotData,RetType *RetInfo);
void		CheckSprite16x16(Sint16 Frame,SlotType *SlotData,RetType *RetInfo);
void		CheckSprite24x24(Sint16 Frame,SlotType *SlotData,RetType *RetInfo);
void		CheckSprite32x32(Sint16 Frame,SlotType *SlotData,RetType *RetInfo);
void		CheckSprite48x48(Sint16 Frame,SlotType *SlotData,RetType *RetInfo);
void		CheckSprite64x64(Sint16 Frame,SlotType *SlotData,RetType *RetInfo);
void		CheckSprite80x80(Sint16 Frame,SlotType *SlotData,RetType *RetInfo);
void		CheckSprite96x96(Sint16 Frame,SlotType *SlotData,RetType *RetInfo);
void		CheckSprite128x128(Sint16 Frame,SlotType *SlotData,RetType *RetInfo);

/* Globals ********************************************************/

SlotFunc	GetSlot[]=		//Array of functions
{
	Get8x8Slot,
	Get16x16Slot,
	Get24x24Slot,
	Get32x32Slot,
	Get48x48Slot,
	Get64x64Slot,
	Get80x80Slot,
	Get96x96Slot,
	Get128x128Slot,
};

CXS		CheckSprite[]=
{
	CheckSprite8x8,
	CheckSprite16x16,
	CheckSprite24x24,
	CheckSprite32x32,
	CheckSprite48x48,
	CheckSprite64x64,
	CheckSprite80x80,
	CheckSprite96x96,
	CheckSprite128x128,
};

/* Externals ******************************************************/


/******************************************************************/
/******************************************************************/
/******************************************************************/

void	InitialiseSpriteArrays(SlotType *SlotData)
{

	register	Uint8		cnt;

	/* 8x8 */
	for(cnt=0;cnt<Num24x24;cnt++)
	{
		SlotData -> S8x8[cnt].charno	=	-1;
		SlotData -> S8x8[cnt].OnScreen	=	0;
	}

	/* 16x16 */
	for(cnt=0;cnt<Num16x16;cnt++)
	{
		SlotData -> S16x16[cnt].charno	=	-1;
		SlotData -> S16x16[cnt].OnScreen	=	0;
	}

	/* 24x24 */
	for(cnt=0;cnt<Num24x24;cnt++)
	{
		SlotData -> S24x24[cnt].charno	=	-1;
		SlotData -> S24x24[cnt].OnScreen	=	0;
	}

	/* 32x32 */
	for(cnt=0;cnt<Num32x32;cnt++)
	{
		SlotData -> S32x32[cnt].charno	=	-1;
		SlotData -> S32x32[cnt].OnScreen	=	0;
	}

	/* 48x48 */
	for(cnt=0;cnt<Num48x48;cnt++)
	{
		SlotData -> S48x48[cnt].charno	=	-1;
		SlotData -> S48x48[cnt].OnScreen	=	0;
	}

	/* 64x64 */
	for(cnt=0;cnt<Num64x64;cnt++)
	{
		SlotData -> S64x64[cnt].charno	=	-1;
		SlotData -> S64x64[cnt].OnScreen	=	0;
	}

	/* 80x80 */
	for(cnt=0;cnt<Num80x80;cnt++)
	{
		SlotData -> S80x80[cnt].charno	=	-1;
		SlotData -> S80x80[cnt].OnScreen	=	0;
	}

	/* 96x96 */
	for(cnt=0;cnt<Num96x96;cnt++)
	{
		SlotData -> S96x96[cnt].charno	=	-1;
		SlotData -> S96x96[cnt].OnScreen=	0;
	}

	for(cnt=0;cnt<Num128x128;cnt++)
	{
		SlotData -> S128x128[cnt].charno	=	-1;
		SlotData -> S128x128[cnt].OnScreen=	0;
	}

}

/***************************************************************/

void	ClearSlotAllocationArrays(SlotType *SlotData)
{

	register	Uint8	index;

	for(index=0;index<Num8x8;index++)
	{
		if(SlotData->S8x8[index].OnScreen	==	0) //if slot was not used the last time
		{
			SlotData->S8x8[index].charno	=	-1;
		}
		SlotData->S8x8[index].OnScreen	=	0;
	}

	for(index=0;index<Num16x16;index++)
	{
		if(SlotData->S16x16[index].OnScreen	==	0) //if slot was not used the last time
		{
			SlotData->S16x16[index].charno	=	-1;
		}
		SlotData->S16x16[index].OnScreen	=	0;
	}

	for(index=0;index<Num24x24;index++)
	{
		if(SlotData->S24x24[index].OnScreen	==	0) //if slot was not used the last time
		{
			SlotData->S24x24[index].charno	=	-1;
		}
		SlotData->S24x24[index].OnScreen	=	0;
	}

	for(index=0;index<Num32x32;index++)
	{
		if(SlotData->S32x32[index].OnScreen	==	0) //if slot was not used the last time
		{
			SlotData->S32x32[index].charno	=	-1;
		}
		SlotData->S32x32[index].OnScreen	=	0;
	}

	for(index=0;index<Num48x48;index++)
	{
		if(SlotData->S48x48[index].OnScreen	==	0) //if slot was not used the last time
		{
			SlotData->S48x48[index].charno	=	-1;
		}
		SlotData->S48x48[index].OnScreen	=	0;
	}

	for(index=0;index<Num64x64;index++)
	{
		if(SlotData->S64x64[index].OnScreen	==	0) //if slot was not used the last time
		{
			SlotData->S64x64[index].charno	=	-1;
		}
		SlotData->S64x64[index].OnScreen	=	0;
	}

	for(index=0;index<Num80x80;index++)
	{
		if(SlotData->S80x80[index].OnScreen	==	0) //if slot was not used the last time
		{
			SlotData->S80x80[index].charno	=	-1;
		}
		SlotData->S80x80[index].OnScreen	=	0;
	}

	for(index=0;index<Num96x96;index++)
	{
		if(SlotData->S96x96[index].OnScreen	==	0) //if slot was not used the last time
		{
			SlotData->S96x96[index].charno	=	-1;
		}
		SlotData->S96x96[index].OnScreen	=	0;
	}

	for(index=0;index<Num128x128;index++)
	{
		if(SlotData->S128x128[index].OnScreen	==	0) //if slot was not used the last time
		{
			SlotData->S128x128[index].charno	=	-1;
		}
		SlotData->S128x128[index].OnScreen	=	0;
	}



}

/***************************************************************/

void	Get8x8Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo)
{

	register	Uint8		cnt	=	0;
	register	Sint16	retval=	-1;
	//SwapType	output;

	while(cnt<Num8x8)
	{
		if( (SlotData)->S8x8[cnt].OnScreen == 0)	//Check it's not already
		{															//used

			//If it was used in the previous frame
			//it may well be used again so record it,
			//but look for one that hasn't been used
			//in the previous frame

			SwapInfo->offset		=	(Start8x8-SpriteStart) + (cnt*((8*8)/2));
			SwapInfo->ClearArea	=	(8*8)/2;
			SwapInfo->Delta		=	8/2;
			SwapInfo->texno		=	Sprite8x8+cnt;

			retval	=	cnt;

		 	if((SlotData)->S8x8[cnt].charno	==	-1)
			{	
				//Found a slot not used in the last frame
				(SlotData)->S8x8[cnt].charno	=	Sprite;
				(SlotData)->S8x8[cnt].OnScreen	=	1;
				break;
	 		}
		}
		
		cnt++;
	}

	//Haven't found any slots that were not used in the last frame
	//so register the last free slot found	or return -1 if no slots 
	//found
	if(retval != -1)
	{
	 	(SlotData)->S8x8[cnt].charno	=	Sprite;
	 	(SlotData)->S8x8[cnt].OnScreen	=	1;
	}
	else
	{
		SwapInfo->offset	=	0xffffffff;

		//No slots free here so try next size up
		//output	=	Get16x16Slot(SlotData,Sprite);
	}

	//return(output);

}

/***************************************************************/

void	Get16x16Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo)
{

	register	Uint8		cnt	=	0;
	register	Sint16	retval=	-1;
  //	SwapType	output;

	while(cnt<Num16x16)
	{
		if( (SlotData)->S16x16[cnt].OnScreen == 0)	//Check it's not already
		{															//used

			//If it was used in the previous frame
			//it may well be used again so record it,
			//but look for one that hasn't been used
			//in the previous frame

			SwapInfo->offset		=	(Start16x16-SpriteStart) + (cnt*((16*16)/2));
			SwapInfo->ClearArea	=	(16*16)/2;
			SwapInfo->Delta		=	16/2;
			SwapInfo->texno		=	Sprite16x16+cnt;

			retval	=	cnt;

		 	if((SlotData)->S16x16[cnt].charno	==	-1)
			{	
				//Found a slot not used in the last frame
				(SlotData)->S16x16[cnt].charno	=	Sprite;
				(SlotData)->S16x16[cnt].OnScreen	=	1;
				break;
	 		}
		}
		
		cnt++;
	}

	//Haven't found any slots that were not used in the last frame
	//so register the last free slot found	or return -1 if no slots 
	//found
	if(retval != -1)
	{
	 	(SlotData)->S16x16[cnt].charno	=	Sprite;
	 	(SlotData)->S16x16[cnt].OnScreen	=	1;
	}
	else
	{
		SwapInfo->offset	=	0xffffffff;

		//No slots free here so try next size up
		//output	=	Get24x24Slot(SlotData,Sprite);
	}

	//return(output);

}

/***************************************************************/

void	Get24x24Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo)
{

	register	Uint8		cnt	=	0;
	register	Sint16	retval=	-1;
	//SwapType	output;

	while(cnt<Num24x24)
	{
		if( (SlotData)->S24x24[cnt].OnScreen == 0)	//Check it's not already
		{															//used

			//If it was used in the previous frame
			//it may well be used again so record it,
			//but look for one that hasn't been used
			//in the previous frame

			SwapInfo->offset		=	(Start24x24-SpriteStart) + (cnt*((24*24)/2));
			SwapInfo->ClearArea	=	(24*24)/2;
			SwapInfo->Delta		=	24/2;
			SwapInfo->texno		=	Sprite24x24+cnt;

			retval	=	cnt;

		 	if((SlotData)->S24x24[cnt].charno	==	-1)
			{	
				//Found a slot not used in the last frame
				(SlotData)->S24x24[cnt].charno	=	Sprite;
				(SlotData)->S24x24[cnt].OnScreen	=	1;
				break;
	 		}
		}
		
		cnt++;
	}

	//Haven't found any slots that were not used in the last frame
	//so register the last free slot found	or return -1 if no slots 
	//found
	if(retval != -1)
	{
	 	(SlotData)->S24x24[cnt].charno	=	Sprite;
	 	(SlotData)->S24x24[cnt].OnScreen	=	1;
	}
	else
	{
		SwapInfo->offset	=	0xffffffff;

		//No slots free here so try next size up
		//output	=	Get32x32Slot(SlotData,Sprite);
	}

	//return(output);
}	

/***************************************************************/

void Get32x32Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo)
{

	register	Uint8		cnt	=	0;
	register	Sint16	retval=	-1;
	//SwapType	output;

	while(cnt<Num32x32)
	{
		if( (SlotData)->S32x32[cnt].OnScreen == 0)	//Check it's not already
		{															//used

			//If it was used in the previous frame
			//it may well be used again so record it,
			//but look for one that hasn't been used
			//in the previos frame

			SwapInfo->offset		=	(Start32x32-SpriteStart) + (cnt*((32*32)/2));
			SwapInfo->ClearArea	=	(32*32)/2;
			SwapInfo->Delta		=	32/2;
			SwapInfo->texno		=	Sprite32x32+cnt;

			retval	=	cnt;

		 	if((SlotData)->S32x32[cnt].charno	==	-1)
		 	{	
				//Found a slot not used in the last frame
				(SlotData)->S32x32[cnt].charno	=	Sprite;
				(SlotData)->S32x32[cnt].OnScreen	=	1;
				break;
		 	}
		}
		
		cnt++;
	}

	//Haven't found any slots that were not used in the last frame
	//so register the last free slot found	or return -1 if no slots 
	//found
	if(retval != -1)
	{
	 	(SlotData)->S32x32[cnt].charno	=	Sprite;
	 	(SlotData)->S32x32[cnt].OnScreen	=	1;
	}
	else
	{
		//No slots free here so try next size up
		//output	=	Get48x48Slot(SlotData,Sprite);
		SwapInfo->offset	=	0xffffffff;
	}

	//return(output);
}	

/***************************************************************/

void Get48x48Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo)
{

	register	Uint8		cnt	=	0;
	register	Sint16	retval=	-1;
	//SwapType	output;

	while(cnt<Num48x48)
	{
		if( (SlotData)->S48x48[cnt].OnScreen == 0)	//Check it's not already
		{															//used

			//If it was used in the previous frame
			//it may well be used again so record it,
			//but look for one that hasn't been used
			//in the previos frame

			SwapInfo->offset		=	(Start48x48-SpriteStart) + (cnt*((48*48)/2));
			SwapInfo->ClearArea	=	(48*48)/2;
			SwapInfo->Delta		=	48/2;
			SwapInfo->texno		=	Sprite48x48+cnt;

			retval	=	cnt;

		 	if((SlotData)->S48x48[cnt].charno	==	-1)
		 	{	
				//Found a slot not used in the last frame
				(SlotData)->S48x48[cnt].charno	=	Sprite;
				(SlotData)->S48x48[cnt].OnScreen	=	1;
				break;
		 	}
		}
		
		cnt++;
	}

	//Haven't found any slots that were not used in the last frame
	//so register the last free slot found	or return -1 if no slots 
	//found
	if(retval != -1)
	{
	 	(SlotData)->S48x48[cnt].charno	=	Sprite;
	 	(SlotData)->S48x48[cnt].OnScreen	=	1;
	}
	else
	{
		//No slots free here so try next size up
		//output	=	Get64x64Slot(SlotData,Sprite);
		SwapInfo->offset	=	0xffffffff;
	}

	//return(output);
}	

/***************************************************************/

void Get64x64Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo)
{

	register	Uint8		cnt	=	0;
	register	Sint16	retval=	-1;
	//SwapType	output;

	while(cnt<Num64x64)
	{
		if( (SlotData)->S64x64[cnt].OnScreen == 0)	//Check it's not already
		{															//used

			//If it was used in the previous frame
			//it may well be used again so record it,
			//but look for one that hasn't been used
			//in the previos frame

			SwapInfo->offset		=	(Start64x64-SpriteStart) + (cnt*((64*64)/2));
			SwapInfo->ClearArea	=	(64*64)/2;
			SwapInfo->Delta		=	64/2;
			SwapInfo->texno		=	Sprite64x64+cnt;

			retval	=	cnt;

		 	if((SlotData)->S64x64[cnt].charno	==	-1)
		 	{	
				//Found a slot not used in the last frame
				(SlotData)->S64x64[cnt].charno	=	Sprite;
				(SlotData)->S64x64[cnt].OnScreen	=	1;
				break;
		 	}
		}
		
		cnt++;
	}

	//Haven't found any slots that were not used in the last frame
	//so register the last free slot found	or return -1 if no slots 
	//found
	if(retval != -1)
	{
	 	(SlotData)->S64x64[cnt].charno	=	Sprite;
	 	(SlotData)->S64x64[cnt].OnScreen	=	1;
	}
	else
	{
		//No slots free here so try next size up
		//output	=	Get80x80Slot(SlotData,Sprite);
		SwapInfo->offset	=	0xffffffff;
	}

	//return(output);
}	

/***************************************************************/

void Get80x80Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo)
{

	register	Uint8		cnt	=	0;
	register	Sint16	retval=	-1;
	//SwapType	output;

	while(cnt<Num80x80)
	{
		if( (SlotData)->S80x80[cnt].OnScreen == 0)	//Check it's not already
		{															//used

			//If it was used in the previous frame
			//it may well be used again so record it,
			//but look for one that hasn't been used
			//in the previos frame

			SwapInfo->offset		=	(Start80x80-SpriteStart) + (cnt*((80*80)/2));
			SwapInfo->ClearArea	=	(80*80)/2;
			SwapInfo->Delta		=	80/2;
			SwapInfo->texno		=	Sprite80x80+cnt;

			retval	=	cnt;

		 	if((SlotData)->S80x80[cnt].charno	==	-1)
		 	{	
				//Found a slot not used in the last frame
				(SlotData)->S80x80[cnt].charno	=	Sprite;
				(SlotData)->S80x80[cnt].OnScreen	=	1;
				break;
		 	}
		}
		
		cnt++;
	}

	//Haven't found any slots that were not used in the last frame
	//so register the last free slot found	or return -1 if no slots 
	//found
	if(retval != -1)
	{
	 	(SlotData)->S80x80[cnt].charno	=	Sprite;
	 	(SlotData)->S80x80[cnt].OnScreen	=	1;
	}
	else
	{
		//No slots free here so try next size up
		//output	=	Get96x96Slot(SlotData,Sprite);
		SwapInfo->offset	=	0xffffffff;
	}

	//return(output);
}	

/***************************************************************/

void Get96x96Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo)
{

	register	Uint8		cnt	=	0;
	register	Sint16	retval=	-1;
	//SwapType	output;

	while(cnt<Num96x96)
	{
		if( (SlotData)->S96x96[cnt].OnScreen == 0)	//Check it's not already
		{															//used

			//If it was used in the previous frame
			//it may well be used again so record it,
			//but look for one that hasn't been used
			//in the previos frame

			SwapInfo->offset		=	(Start96x96-SpriteStart) + (cnt*((96*96)/2));
			SwapInfo->ClearArea	=	(96*96)/2;
			SwapInfo->Delta		=	96/2;
			SwapInfo->texno		=	Sprite96x96+cnt;

			retval	=	cnt;

		 	if((SlotData)->S96x96[cnt].charno	==	-1)
		 	{	
				//Found a slot not used in the last frame
				(SlotData)->S96x96[cnt].charno	=	Sprite;
				(SlotData)->S96x96[cnt].OnScreen	=	1;
				break;
		 	}
		}
		
		cnt++;
	}

	//Haven't found any slots that were not used in the last frame
	//so register the last free slot found	or return -1 if no slots 
	//found
	if(retval != -1)
	{
	 	(SlotData)->S96x96[cnt].charno	=	Sprite;
	 	(SlotData)->S96x96[cnt].OnScreen	=	1;
	}
	else
	{
		//No slots free here so try next size up
		//output	=	Get128x128Slot(SlotData,Sprite);
		SwapInfo->offset	=	0xffffffff;
	}

	//return(output);
}	

/***************************************************************/

void Get128x128Slot(SlotType *SlotData,Uint16	Sprite,SwapType *SwapInfo)
{

	register	Uint8		cnt	=	0;
	register	Sint16	retval=	-1;
	//SwapType	output;

	while(cnt<Num128x128)
	{
		if( (SlotData)->S128x128[cnt].OnScreen == 0)	//Check it's not already
		{															//used

			//If it was used in the previous frame,
			//it may well be used again so record it
			//but keep looking for one that hasn't been used
			//in the previous frame

			SwapInfo->offset		=	(Start128x128-SpriteStart) + (cnt*((128*128)/2));
			SwapInfo->ClearArea	=	(128*128)/2;
			SwapInfo->Delta		=	128/2;
			SwapInfo->texno		=	Sprite128x128+cnt;

			retval	=	cnt;

		 	if((SlotData)->S128x128[cnt].charno	==	-1)
		 	{	
				//Found a slot not used in the last frame
				(SlotData)->S128x128[cnt].charno		=	Sprite;
				(SlotData)->S128x128[cnt].OnScreen	=	1;
				break;
		 	}
		}
		
		cnt++;
	}

	//Haven't found any slots that were not used in the last frame
	//so register the last free slot found	or return -1 if no slots 
	//found
	if(retval != -1)
	{
	 	(SlotData)->S128x128[cnt].charno	=	Sprite;
	 	(SlotData)->S128x128[cnt].OnScreen	=	1;
	}
	else
	{
		//End of the line	-	no slots free so return error code
		SwapInfo->offset	=	0xffffffff;
	}

	//return(output);

}	


/***************************************************************
*  Function to check if a sprite is already in VDP1 VRAM:		*
*	Returns SATURN SPRITE NUMBER if yes, -1 if it is not			*
****************************************************************/

void CheckSprite8x8(Sint16 Frame,SlotType *SlotData,RetType *RetInfo)
{
	register	Uint8		cnt	=	0;
	register	Sint16	barf	=	-1;
	//RetType	retval;

	while(cnt<Num8x8)
	{
		if( (SlotData)->S8x8[cnt].charno == Frame)
		{
	 		(SlotData)->S8x8[cnt].OnScreen	=	1;
			barf	=	Sprite8x8+cnt;
			RetInfo->cx	=	(8/2);
			RetInfo->cy	=	(8/2);
			break;
		}
		cnt++;
	}

	RetInfo->Return	=	barf;
	RetInfo->clx		=	8;

	if(barf == -1)
	{
		RetInfo->cx	=	(8/2);
		RetInfo->cy	=	(8/2);
	}

	//return(retval);		

}

/***************************************************************/

void CheckSprite16x16(Sint16 Frame,SlotType *SlotData,RetType *RetInfo)
{
	register	Uint8		cnt	=	0;
	register	Sint16	barf	=	-1;
	//RetType	retval;

	while(cnt<Num16x16)
	{
		if( (SlotData)->S16x16[cnt].charno == Frame)
		{
	 		(SlotData)->S16x16[cnt].OnScreen	=	1;
			barf	=	Sprite16x16+cnt;
			RetInfo->cx	=	(16/2);
			RetInfo->cy	=	(16/2);
			break;
		}
		cnt++;
	}

	RetInfo->Return	=	barf;
	RetInfo->clx		=	16;

	if(barf == -1)
	{
		RetInfo->cx	=	(16/2);
		RetInfo->cy	=	(16/2);
	}

	//return(retval);		

}

/***************************************************************/

void CheckSprite24x24(Sint16 Frame,SlotType *SlotData,RetType *RetInfo)
{
	register	Uint8		cnt	=	0;
	register	Sint16	barf	=	-1;
	//RetType	retval;

	while(cnt<Num24x24)
	{
		if( (SlotData)->S24x24[cnt].charno == Frame)
		{
	 		(SlotData)->S24x24[cnt].OnScreen	=	1;
			barf	=	Sprite24x24+cnt;
			RetInfo->cx	=	(24/2);
			RetInfo->cy	=	(24/2);
			break;
		}
		cnt++;
	}

	RetInfo->Return	=	barf;
	RetInfo->clx		=	24;

	if(barf == -1)
	{
		RetInfo->cx	=	(24/2);
		RetInfo->cy	=	(24/2);
	}

	//return(retval);		
}

/***************************************************************/

void CheckSprite32x32(Sint16 Frame,SlotType *SlotData,RetType *RetInfo)
{
	register	Uint8		cnt	=	0;
	register	Sint16	barf	=	-1;
	//RetType	retval;

	while(cnt<Num32x32)
	{
		if( (SlotData)->S32x32[cnt].charno == Frame)
		{
	 		(SlotData)->S32x32[cnt].OnScreen	=	1;
			barf	=	Sprite32x32+cnt;
			RetInfo->cx	=	(32/2);
			RetInfo->cy	=	(32/2);
			break;
		}
		cnt++;
	}

	RetInfo->Return	=	barf;
	RetInfo->clx		=	32;

	if(barf == -1)
	{
		RetInfo->cx	=	(32/2);
		RetInfo->cy	=	(32/2);
	}
	
	//return(retval);		
}

/***************************************************************/

void CheckSprite48x48(Sint16 Frame,SlotType *SlotData,RetType *RetInfo)
{
	register	Uint8		cnt	=	0;
	register	Sint16	barf	=	-1;
	//RetType	retval;

	while(cnt<Num48x48)
	{
		if( (SlotData)->S48x48[cnt].charno == Frame)
		{
	 		(SlotData)->S48x48[cnt].OnScreen	=	1;
			barf	=	Sprite48x48+cnt;
			RetInfo->cx	=	(48/2);
			RetInfo->cy	=	(48/2);
			break;
		}
		cnt++;
	}

	RetInfo->Return	=	barf;
	RetInfo->clx		=	48;


	if(barf == -1)
	{
		RetInfo->cx	=	(48/2);
		RetInfo->cy	=	(48/2);
	}

	//return(retval);		

}

/***************************************************************/

void CheckSprite64x64(Sint16 Frame,SlotType *SlotData,RetType *RetInfo)
{
	register	Uint8		cnt	=	0;
	register	Sint16	barf	=	-1;
	//RetType	retval;

	while(cnt<Num64x64)
	{
		if( (SlotData)->S64x64[cnt].charno == Frame)
		{
	 		(SlotData)->S64x64[cnt].OnScreen	=	1;
			barf	=	Sprite64x64+cnt;
			RetInfo->cx	=	(64/2);
			RetInfo->cy	=	(64/2);
			break;
		}
		cnt++;
	}
	
	RetInfo->Return	=	barf;
	RetInfo->clx		=	64;

	if(barf == -1)
	{
		RetInfo->cx	=	(64/2);
		RetInfo->cy	=	(64/2);
	}

	//return(retval);		
}

/***************************************************************/

void CheckSprite80x80(Sint16 Frame,SlotType *SlotData,RetType *RetInfo)
{
	register	Uint8		cnt	=	0;
	register	Sint16	barf	=	-1;
	//RetType	retval;

	while(cnt<Num80x80)
	{
		if( (SlotData)->S80x80[cnt].charno == Frame)
		{
			(SlotData)->S80x80[cnt].OnScreen	=	1;
			barf	=	Sprite80x80+cnt;
			RetInfo->cx	=	(80/2);
			RetInfo->cy	=	(80/2);
			break;
		}
		cnt++;
	}
	
	RetInfo->Return	=	barf;
	RetInfo->clx		=	80;

	if(barf == -1)
	{
		RetInfo->cx	=	(80/2);
		RetInfo->cy	=	(80/2);
	}

	//return(retval);

}

/***************************************************************/

void CheckSprite96x96(Sint16 Frame,SlotType *SlotData,RetType *RetInfo)
{
	register	Uint8		cnt	=	0;
	register	Sint16	barf	=	-1;
	//RetType	retval;

	while(cnt<Num96x96)
	{
		if( (SlotData)->S96x96[cnt].charno == Frame )
		{
			(SlotData)->S96x96[cnt].OnScreen	=	1;
			barf	=	Sprite96x96+cnt;
			RetInfo->cx	=	(96/2);
			RetInfo->cy	=	(96/2);
			break;
		}
		cnt++;
	}

	RetInfo->Return	=	barf;
	RetInfo->clx		=	96;

	if(barf == -1)
	{
		RetInfo->cx	=	(96/2);
		RetInfo->cy	=	(96/2);
	}

	//return(retval);
}

/***************************************************************/

void CheckSprite128x128(Sint16 Frame,SlotType *SlotData,RetType *RetInfo)
{
	register	Uint8		cnt	=	0;
	register	Sint16	barf	=	-1;
	//RetType	retval;

	while(cnt<Num128x128)
	{
		if( (SlotData)->S128x128[cnt].charno == Frame )
		{
			(SlotData)->S128x128[cnt].OnScreen	=	1;
			barf	=	Sprite128x128+cnt;
			RetInfo->cx	=	(128/2);
			RetInfo->cy	=	(128/2);
			break;
		}
		cnt++;
	}

	RetInfo->Return	=	barf;
	RetInfo->clx		=	128;

	if(barf == -1)
	{
		RetInfo->cx	=	(128/2);
		RetInfo->cy	=	(128/2);
	}

	//return(retval);
}

/***************************************************************/


