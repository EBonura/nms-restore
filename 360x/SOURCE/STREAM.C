
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*           DECLARE DEFINES           *
**************************************/

void	Cbdataready();				/* CALLBACK FOR NEXT SECTOR READY */
void	PrintStream(void *data);

#define	SLOWFMVFRAMES		(24)		/* NUMBER OF FRAMES ABLE TO STORE */
#define	FASTFMVFRAMES		(48)		/* NUMBER OF FRAMES ABLE TO STORE */

#define	SLOWLEADIN		(8)		/* SLOW LEAD IN AT START OF FLIC */
#define	FASTLEADIN		(12)		/* FAST LEAD IN AT START OF FLIC */

/**************************************
*          DECLARE VARIABLES          *
**************************************/

BYTE	*DecompressBuffer;			/* BUFFER TO DECOMPRESS DATA TO */
BYTE	*RingBuffer;				/* RINGBUFFER FOR CD LOADING */

volatile	LONG		RingIn;			/* WHERE YOU ARE LOADING NEXT SECTOR */

LONG	fmvframe;					/* FMV FRAME CURRENTLY WORKING ON */
LONG	framesleft;				/* CHECK FOR END OF FLICK */
BYTE	faststreaming;				/* 2 DIFFERENT PLAY SPEEDS */
BYTE	leadin;					/* LEAD IN FOR START OF THE FLIC */

UBYTE	param[4];					/* PARAMETERS FOR CD LOADING */

volatile	LONG	readframe;			/* TOTAL NUMBER OF FRAMES READ IN */
volatile	LONG	decomframe;			/* TOTAL NUMBER OF FRAMES DECOMPRESSED */

/**************************************
*       START STREAMING VIDEO         *
**************************************/

RECT	rect;

void
PlayStream(BYTE *fmvname,BYTE *palname)
{

	Init_TPages();					/* INITIALISE TEXTURE PAGES */
	Reset_Cluts();					/* RESET CLUTS */
	Resident_Tims();				/* INSERT RESIDENT GRAPHICS */

//	CdInit();						/* INITIALISE CD */
	copy_deviant3tab2cache();		/* INITIALISE DECOMPRESSER */

	if (faststreaming==YES) leadin=FASTLEADIN;	/* SETUP THE LEAD IN TIME */
	else leadin=SLOWLEADIN;

	DecompressBuffer=MyMalloc(65536);	/* ALLOCATE MEMORY FOR DECOMPRESSION BUFFER */
	if (faststreaming==YES) RingBuffer=MyMalloc(20480*FASTFMVFRAMES);
	if (faststreaming==NO) RingBuffer=MyMalloc(20480*SLOWFMVFRAMES);

	if (CdSearchFile(&fp,palname)==0)	/* SEARCH FOR FILE */
	{
		CRASH;					/* CRASH IF COULDN'T FIND FILE */
	}

    if (!CdControlB(CdlSeekL,(UBYTE*)&fp.pos,0))
	{
		CRASH;
	}

	mode=CdlModeSpeed;

	if (CdRead(1,(ULONG *)DecompressBuffer,mode)==0)	/* READ 1ST SECTOR OF FILE */
	{
		CRASH;					/* CRASH IF CAN'T READ IT */
	}

	CdReadSync(0,result);

	DrawSync(0);
	setRECT(&rect,0,255,256,1);		/* DOWNLOAD THE PALETTE */
	LoadImage(&rect,(ULONG *)DecompressBuffer);
	DrawSync(0);

	if (CdSearchFile(&fp,fmvname)==0)	/* SEARCH FOR FILE */
	{
		CRASH;					/* CRASH IF COULDN'T FIND FILE */
	}

    if (!CdControlB(CdlSeekL,(unsigned char *)&fp.pos, 0))
	{
		CRASH;
	}

	if (faststreaming==YES)			/* CHECK FOR SPEED OF FMV */
	{
		mode=CdlModeSpeed;			/* SET CDREAD2 TO DOUBLE SPEED */
	}
	else mode=0;					/* SET CDREAD2 TO SINGLE SPEED */

	RingIn=0;						/* RESET RING IN */
	fmvframe=0;					/* RESET FRAME NUMBER */

	readframe=0;
	decomframe=0;

	CdReadSync(0,result);
	CdReadyCallback(Cbdataready);		/* START CALLBACK */
	CdRead2(mode);					/* SET CONTINUOUS STREAMING */

	while(RingIn<(20480*leadin))	{}	/* DO DELAY WHILE PREREADING */

//	CdReadyCallback(Rts);		/* RESET CALLBACK */

	Cls();						/* TURN SCREEN ON AND FADE UP */

	fade=128;

	while (fade!=0)
	{

		if (((Debounce1 & PADstart)!=0) && (fade==128))
		{
			Start_Fade_Down(-16);
		}

		Start_Loop();

		framesleft=*((LONG *)(RingBuffer+(20480*((fmvframe+1) MOD 48))));

		if (framesleft!=-1)
		{

			Decompress288(RingBuffer+(20480*fmvframe),DecompressBuffer);
			fmvframe++;				/* MOVE ONTO NEXT FRAME */
			decomframe++;

			if (faststreaming==YES) if (fmvframe==FASTFMVFRAMES) fmvframe=0;
			if (faststreaming==NO) if (fmvframe==SLOWFMVFRAMES) fmvframe=0;
		}

		if ((framesleft==-1) && (fade==128))
		{
			Start_Fade_Down(-16);
			CdReadyCallback(Rts);		/* RESET CALLBACK */
		}

		PrintStream(DecompressBuffer);

//		sprctrl.red=128;
//		sprctrl.green=0;
//		sprctrl.blue=0;

//		Print_At(24,168);
//		Print_Messm("CD Speed: ");
//		if (faststreaming==YES) Print_Messm("Double");
//		else Print_Messm("Single");

//		#if 0

		Print_At(24,184);
		Print_Messm("TotalD: ");
		Print_Decm(decomframe);

		Print_At(24,200);
		Print_Messm("TotalL: ");
		Print_Decm(readframe/20480);

		Print_At(128,184);
		Print_Messm("Decomp: ");
		Print_Decm(fmvframe);

		Print_At(128,200);
		Print_Messm("Loading: ");
		Print_Decm(RingIn/20480);

//		#endif

//		sprctrl.red=128;
//		sprctrl.green=128;
//		sprctrl.blue=128;

		if (faststreaming==YES)		/* CHECK FOR FAST OR SLOW CD READING */
		{

//			if (decomframe<8) VSync(0);
//			else
//			{
				if (((readframe/20480)-16) < (decomframe))
			{
//#if 0
					Print_At(24,152);
					Print_Messm("VSync Skip");
//#endif
					VSync(0);
				}
//			}

			VSync(0);				/* SMALL DELAY FOR FAST CD READING */
			VSync(0);
		}
		else
		{
			VSync(0);				/* LARGE DELAY FOR SLOW CD READING */
			VSync(0);
			VSync(0);
			VSync(0);
			VSync(0);

			if ((gamecycle MOD 8)<3)
			{
				Print_At(24,152);
				Print_Messm("VSync Skip");
			}
			else VSync(0);

		}

		End_Loop();
	}

	VSync(0);
	SetDispMask(OFF);

	CdReadyCallback(Rts);		/* RESET CALLBACK */

	MyFree(RingBuffer);				/* DE-ALLOCATE MEMORY FOR RINGBUFFER */
	MyFree(DecompressBuffer);		/* DE-ALLOCATE MEMORY FOR DECOMPRESSION */

}

/**************************************
*    PRINT THE DECOMPRESSED STREAM    *
**************************************/

void
PrintStream(void *data)
{

RECT	rect;
SPRITE  spr={0,0,0,0,0,184,-160,-120,(255<<8),5,COL256};

	setRECT(&rect,640,0,144,184);
	LoadImage(&rect,data);
	DrawSync(0);

	Print_Sprite(&spr,160+16,120+28,128);
	spr.w=288-256;
	spr.tpage=7;
	Print_Sprite(&spr,160+256+16,120+28,128);

}

/**************************************
*       START STREAMING VIDEO         *
**************************************/

void
Cbdataready(int intr, u_char *result)
{
	if (intr==CdlDataReady)			/* CHECK FOR READY FOR NEXT SECTOR */
	{
		CdGetSector(&RingBuffer[RingIn],2048/4);
		RingIn+=2048;				/* MOVE ONTO NEXT SECTOR */
		readframe+=2048;

//		if (faststreaming==YES)
//		{
			if(RingIn==(20480*FASTFMVFRAMES))	/* CHECK FOR END OF RINGBUFFER */
			{
				RingIn=0		;	/* MOVE BACK TO START OF RINGBUFFER */
			}
//		}
//		else
//		{
//			if(RingIn==(20480*SLOWFMVFRAMES))	/* CHECK FOR END OF RINGBUFFER */
//			{
//				RingIn=0		;	/* MOVE BACK TO START OF RINGBUFFER */
//			}
//		}

	}
}

