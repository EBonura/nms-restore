
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE VARIABLES          *
**************************************/

MOVEIMAGE	*InteruptGPU[MAXINTERGPUS];	/* DATA FOR LIVE DOWNLOADED SPRITES */
USHORT		intergpu;				/* DATA FOR INTERUPTGPU */

//MOVEIMAGE 	*pointmove;			/* MOVE DATA FROM A TO B */
SPRITEIMAGE	*pointsprite;			/* LOAD SPRITE FROM MEMORY */

GsOT		Wot[2];				/* ORDERING TABLE HANDLERS */
GsOT_TAG		zsorttable[2][1<<OTLENGTH];	/* ORDERING TABLES */

PACKET		out_packet[2][PACKMAX2];		/* PACKET FOR SPRITES */

ULONG 		Evbl;				/* EVENT FOR VBLANK OCCURANCE */

volatile 	ULONG	vblank;		/* THIS IS THE CURRENT VBLANK */
volatile 	ULONG	ovblank;		/* THIS IS THE OLD VBLANK */

BYTE		screentoggle;			/* WHICH SCREEN YOU ARE CURRENTLY WORKING ON */
LONG		gamecycle;			/* GAME CYCLE COUNTER, UPDATED EVERY GAME TURN */

USHORT		currentlive;			/* CURRENT LIVE DOWNLOAD SPRITE */
UBYTE		*lastfree;			/* CHECK FOR LIVE DOWNLOADING HAS BEEN FREED */

/**************************************
*    INITIALISE THE ORDER TABLE       *
**************************************/

void 
Init_Order_Table()
{

	Wot[0].length=OTLENGTH;			/* SET RESOLUTION OF ORDERING TABLE */
	Wot[0].org=zsorttable[0];		/* SET ENTITY OF ORDERING TABLE */
	Wot[1].length=OTLENGTH;			/* SET RESOLUTION OF ORDERING TABLE */
	Wot[1].org=zsorttable[1];		/* SET ENTITY OF ORDERING TABLE */

}

/**************************************
*       VBLANK CALLBACK ROUTINE       *
**************************************/

void
VblCallback()
{
	vblank++;						/* INCREMENT VBLANK COUNTER */
	UpdateTheSound();				/* UPDATE THE SOUND */
	/* RESTORE: completion. NMS's sound tick, commented out in July 1996,
	   runs again. InitialiseSound (SOUND.C) sets SS_NOTICK, "USER VBL
	   UPDATE": without this call no key-on and no song reaches the sound
	   chip. NMS's March 1996 tree called it here (NMS.zip NMS/360WORK
	   INTERUPT.C:60), and its build plays OPTPA.SEQ so. */
	if (pollhost==YES) pollhost();		/* CHECK FOR DEVELOPMENT BOARD */
}

/**************************************
*       VSYNC CALLBACK ROUTINE        *
**************************************/

void
VSyncCallBack(ULONG func)
{

	vblank=1;						/* RESET VBLANK COUNTER */
	ovblank=0;					/* RESET VBLANK COUNTER */
	EnterCriticalSection();			/* TURN OFF INTERRUPTS */
	Evbl=OpenEvent(RCntCNT3,EvSpINT,EvMdINTR,func);	/* ENABLE EVENTS */
	EnableEvent(Evbl);				/* OPEN VBLCALLBACK */
	StartRCnt(RCntCNT3);			/* START COUNTING */
	ExitCriticalSection();			/* ENABLE INTERRUPTS */

}

/**************************************
*     DO STUFF AT START OF FRAME      *
**************************************/

void	
Start_Loop()
{

	maxpolycount=MAXPOLYS;			/* RESET MAXIMUM POLYGON COUNT */
	screentoggle=GsGetActiveBuff();		/* GET ACTIVE BUFFER */
	GsSetWorkBase((PACKET*)out_packet[screentoggle]);	/* SET THE CURRENT WORKBASE */
	GsClearOt(0,0,&Wot[screentoggle]);		/* CLEAR ORDERING TABLE */
	lastfree=0;					/* RESET LAST FREE BODGE */

}

/**************************************
*      DO STUFF AT END OF FRAME       *
**************************************/

void
End_Loop()
{

USHORT	j;

	Read_Pads();					/* READ JOYPADS */
	Update_Fade();					/* UPDATE FADE IF NEEDED */

	if (showraster==YES) Solid_Line();		/* END OF MAIN CODE */
	if (testmessages==YES) MallocTest();	/* CHECK AMOUNT OF MALLOCS, ETC */

	if (((Debounce1 & PaDld)!=0)		/* SHOW VRAM IN DEBUGGING MODE */
		&& (showvram==YES)) Show_VRAM();

	DrawSync(0);					/* WAIT FOR GRAPHICS TO FINISH DUMPING */
	DrawSyncCallback(&Rts);			/* RESET DRAWSYNC CALLBACK */
	while(currentlive!=0)			/* CHECK FOR FINISHED DOWNLOADING */
	{
		DrawSync(0);				/* WAIT FOR FINISHED DOWNLOADING */
		Interupt_GPU_Stuff();		/* DOWNLOAD SOME MORE DATA */
	}
	DrawSyncCallback(&Rts);			/* RESET DRAWSYNC CALLBACK */

	if (showraster==YES) Flicker_Line();	/* FLICKER LINE AFTER DOWNLOADED FINISHED */

	if (vblank==ovblank);			/* TEST FOR CHANGE IN VBLANK */
	VSync(0);						/* IF SO THEN UPDATE IT */
	ovblank=vblank;				/* AND SAVE THE LAST VALUE */

	if (gamefirstpass==NO)			/* DELETE LAST DOWNLOADED SPRITES */
		Delete_Last_Downloaded_sprites();

	gamefirstpass=NO;				/* SET TO DELETE LAST DOWNLOADED */

	GsSwapDispBuff();				/* EXCHANGE DISPLAY AND DRAWING BUFFER */

	GsDrawOt(&Wot[screentoggle]);		/* START DRAWING PACKETS REGISTERED IN OT */
	intergpu=0;					/* RESET INTERGPU */
	DrawSyncCallback(&Interupt_GPU_Stuff);	/* SET CALLBACKS FOR LIVE DOWNLOADING */

	if ((whichmenu==0) && (pause==OFF)) gamecycle++;	/* INCREMENT GAME CYCLE */
}

/**************************************
*  INTERUPT GPU DOWNLOADING ROUTINES  *
**************************************/

void
Interupt_GPU_Stuff()
{

USHORT	j;
RECT	*temprect;
ULONG	*tempdata;

	if (DrawSync(1)>1) return;		/* CHECK FOR SAFE AREA */

	if (lastfree!=0)				/* CHECK FOR SPRITE ALREADY */
	{
		MyFree(lastfree);			/* FREE LAST SPRITE RAM */
		lastfree=0;				/* AND RESET COUNTER */
	}

	if (currentlive==0) return;		/* RETURN IF NO SPRITES LEFT */

	j=--currentlive;				/* GET LAST LIVE DOWNLOADED */

	if (InteruptGPU[j]->type==LIVEDOWNLOAD)
	{
		pointsprite=(SPRITEIMAGE*)InteruptGPU[j];
		InteruptGPU[j]=0;
		if (pointsprite->pal!=0)
		{
			tempdata=pointsprite->pal;
			temprect=&pointsprite->rectpal;
			DrawSyncCallback(&Rts);
			LoadImage(temprect,tempdata);
			DrawSyncCallback(&Interupt_GPU_Stuff);
		}
		tempdata=pointsprite->data;
		temprect=&pointsprite->rect;

		lastfree=(UBYTE*)pointsprite->data;

//		MyFree((BYTE*)pointsprite->data);

		if (pointsprite->free!=0)
		{
			MyFree((BYTE*)pointsprite->free);
		}

		LoadImage(temprect,tempdata);
		InteruptGPU[j]=0;				/* CLEAR FLAG */
	}
}

