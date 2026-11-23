/******************************************************************/
/* 																					*/
/* 360X Saturn																		*/
/* 																					*/
/* (c)1996 NMS Software															*/
/* 																					*/
/* Author: Pratik Patel															*/
/* 																					*/
/* Date Started : 20/FEB/96													*/
/* 																					*/
/* This is the start up stub that controls the whole program		*/
/* 																					*/
/******************************************************************/
 
#include <sgl.h>
#include <libsn.h>
#include	<stdio.h>
#include <sega_snd.h>
#include "defines.h"
#include	"types.h"
#include "globals.h"
#include "print.h"

/* ProtoTypes *****************************************************/

void		InitSaturn(void);
void		ClearVDP1VRAM(void);
void		ClearVDP2VRAM(void);
void		InitBackGrounds(void);
void		InitPalette(Uint16 *palette,Uint8 bankno);
void		SetSpritePalette(void);
void		InitFonts(void);
void		InitGameScreen(void);
void		InitNormalTitleScreen(void);
void		InitRotateTitleScreen(void);
void		InitSound(void);
void		SetTextureTable(TEXTURE *Textures);
Uint8		play_sample(Uint8	channel,Uint8 pitch);
Uint8		play_sample_with_volume(Uint8	voice,Uint8 pitch,Uint8 volume);
Uint8		play_sample_with_volume_and_direction(Uint8	voice,Uint8 pitch,Uint8 volume,Sint8 direction);
void		PrintLARGE(Uint16 x,Uint16 y,Uint8 *string);
void		PrintMEDIUM(Uint16 x,Uint16 y,Uint8 *string);
void		PrintSMALL(Uint16 x,Uint16 y,Uint8 *string);
void		InitHiResFontClut(void);
void		InitLowResFontClut(void);
void 		msg(char *ptr);
void 		clearmsg(void);

/* Externals ******************************************************/

extern	TEXTURE 	*FormTbl;

extern	void		InitialiseSpriteArrays(SlotType *SlotData);
extern	void		InitFileSystem(void);
extern	void		ReadFile(Uint8 *name,Uint8 *target);
extern	void		InitBackupRam(void);
extern	Sint32	ReadHiscores(void);
extern	void 		clearmsg(void);
extern	void		init_level(void);
extern	void		play_level(void);

extern	Uint8		*loadaddress;
extern	long		fsize;
extern	Uint8		BigBuffer[];
extern	Uint16	palette[];

extern	void		PlayCDAudio(Uint16	trackno);
extern	void		PauseCDAudio(void);
extern	void		ResumeCDAudio(void);
extern	void		StopCDAudio(void);
extern	void		Control(void);
extern	void		Game(void);

extern	void		InitAnimationPlaybackScreen(void);
extern	void 		play_anim(int dest, char *fname);

/* Globals ********************************************************/

//SGL textures
TEXTURE		Textures[MAXSPRITENUM];

//KEYS
Uint16		K_THRUST=PER_DGT_TB;
Uint16		K_FIRE=PER_DGT_TC;
Uint16		K_LEFT=PER_DGT_KL;
Uint16		K_RIGHT=PER_DGT_KR;
Uint16		K_WEAPON_INC=PER_DGT_KL;
Uint16		K_WEAPON_DEC=PER_DGT_KR;

//General purpose string array
char			stat_str[80];
Uint8			Level		=	0;

//Default hiscore table
HIGH highest[]  = 
{
		{500000,"PRATIK",0},
		{400000,"PAUL",0},
		{300000,"PHIL",0},
		{200000,"IAN",0},
		{50000,"ANDY",0},
		{40000,"CHRIS",0},
		{30000,"GLENN",0},
		{25000,"STEVE",0},
		{15000,"COLIN",0},
		{7500,"DICKIE",0},
};

SlotType		Slots; 			//Slot allocation data
Uint16		palette[256];

Uint8	soundmap[]=
{
	0x00,0x00,0xB0,0x00,0x00,0x00,0xb4,0xe6,0xff,0xff,
};

Uint8		midi_channel	=	0;
Uint8		SystemStatus	=	0;

//#define	HACK

/******************************************************************/

void	ss_main(void)
{			
	#ifdef	HACK
	ANGLE	ang;
	int	i;
	FIXED	result;
	Uint32	x,y;
	#endif

	#ifndef	USECD
	enable_nmis(PSYQ_NMI_MASTER);
	#endif

	InitFileSystem();

	#ifdef	HACK
	for(i=0;i<360;i++)
	{
		ang		=	0x271c;
		result	=	slSin(ang);
		x	=	slSquart(i^2);
		sprintf(stat_str," 0x%08X,",result);
		msg(stat_str);
		if((i%4)==3) msg("\n");
	}

	while(1)
	{
		pollhost();
	}
	#endif
	
	InitSaturn();

	sprintf(stat_str,"Free VDP1 VRAM:%d \n",(int)VRAMUSED);
	msg(stat_str);

	while(1)
	{
		//InitNormalTitleScreen();

		//Control Program
		//ReadFile("intro.bin",loadaddress);
		//Control();

		//ReadFile("PLAYANIM.BIN",loadaddress);

		//InitAnimationPlaybackScreen();

		//play_anim(0, "SAT2");
		
		//Load the game overlay
		ReadFile("game.bin",loadaddress);

		init_level();

		play_level();

		PrintLARGE(10,10,"GAME OVER");
		slSynch();
	}

}

/******************************************************************/

void	InitSaturn(void)
{

	SetTextureTable(Textures);

	ClearVDP1VRAM();

	ClearVDP2VRAM();

	slInitSystem(TV_320x240,Textures,1);

	InitBackupRam();

	InitSound();

	slColRAMMode(CRM16_1024);

	slWindow(0,0,320,240,65535,0,0);

	clearmsg();

	InitFonts();

}

/******************************************************************/

void	ClearVDP1VRAM(void)
{
	register	Uint32	x;

	for(x=0;x<(512*1024)-SpriteStart;x++)
	{
		*((Uint8 *)((SpriteVRAM + SpriteStart)+x)) = 0;
	}
}

/******************************************************************/

void	ClearVDP2VRAM(void)
{
	register	Uint32	x;

	//This will clear all banks
	for(x=0;x<(512*1024);x++)
	{
		*((Uint8 *)((VDP2_VRAM_A0)+x)) = 0;
	}
}

/******************************************************************/

void	InitGameScreen(void)
{
	int		y;
	Uint8		*l	=	LowRam;

	slInitSystem(TV_320x240,Textures,1);
	slWindow(0,0,320,240,65535,0,0);

	ClearVDP2VRAM();

	//RBG0 is main game screen NBG0 for the soft rock
	slBitMapRbg0(BM_512x256,COL_TYPE_256,(void *)VDP2_VRAM_A0);
	slBMPaletteRbg0(1);
	slBitMapNbg0(BM_512x256,COL_TYPE_256,(void *)VDP2_VRAM_B1);
	slBMPaletteNbg0(1);

	slRparaInitSet((void *)VDP2_VRAM_B0);
	slOverRA(0);

	slPriorityRbg0(6);
	slDispCenterR(0*65536,0*65536);
	slLookR(0*65536,0*65536);

	slSpriteType(4);
	slPrioritySpr4(7);
	slColOffsetOn(SPRON|RBG0ON);
	
	{	
		slColorCalc(CC_RATE|RBG0ON|SPRON|NBG0ON);
		slSpecialCCalcModeRbg0(spCCAL_MSB);
		slSpriteCCalcCond(CC_MSB|RBG0ON);
		slGradationOn(grdSPR);
		slColorCalcOn(RBG0ON|SPRON|NBG0ON);
		slColRateSpr4(8);
		slColRateRbg0(8);
	}

	slSpriteColMode(SPR_PAL_RGB);

	slScrAutoDisp(RBG0ON|NBG0ON);

	InitLowResFontClut();

	FONTCOL(0,20,0);

	{

		ReadFile("pictures/sat.sat",(Uint8 *)palette);
		InitPalette(palette,1);

		//Loading screen
		slTVOff();

		ReadFile("pictures/sat.bin",LowRam);

		for(y=0;y<240;y++)
		{
			slDMACopy( (l + (y*320)) , (Uint8 *)(VDP2_VRAM_A0+(y*512)) , 320);
		}

		slDMAWait();

		slTVOn();

	}

	PrintLARGE(120,120,"LOADING 360X");
	slSynch();

}

/******************************************************************/

void	InitRotateTitleScreen(void)
{

	slInitSystem(TV_640x240,Textures,1);

	ClearVDP2VRAM();

	slVRAMMode(Div_B);

	//Initialise the backgrounds

	slRparaInitSet((void *)VDP2_VRAM_B0);
	slOverRA(0);

	slBitMapRbg0(BM_512x512,COL_TYPE_256,(void *)VDP2_VRAM_A0);
	slBMPaletteRbg0(1);

	slDispCenterR(toFIXED(320.0) , toFIXED(120.0));

	slLookR(toFIXED(256.0) , toFIXED(256.0));

	slPrioritySpr0(7);
	slPriorityRbg0(6);
	slPriorityNbg1(5);

	slWindow(0,0,640,240,65535,0,0);

	slColOffsetOn(RBG0ON|SPRON|NBG1ON);

	slColRAMMode(CRM16_1024);

	slScrAutoDisp(RBG0ON|NBG1ON);

	slColRAMOffsetSpr(3);	//SPRITES ARE ALL LOCKED TO PALETTE 3!!!!
	
	InitHiResFontClut();
	
	slTVOff();
}

/******************************************************************/

void	InitNormalTitleScreen(void)
{

	slInitSystem(TV_640x240,Textures,1);

	slVRAMMode(Div_A);

	slTVOff();
	ClearVDP2VRAM();

	//Initialise the backgrounds

	slBitMapNbg0(BM_1024x256,COL_TYPE_256,(void *)VDP2_VRAM_A0);
	slBMPaletteNbg0(1);

	slBitMapNbg1(BM_1024x256,COL_TYPE_256,(void *)VDP2_VRAM_B0);
	slBMPaletteNbg1(2);

	slPrioritySpr0(7);
	slPriorityNbg0(6);	//Debug screen
	slPriorityNbg1(5);	//Background screen

	slWindow(0,0,640,240,65535,0,0);

	slColOffsetOn(NBG0ON|SPRON|NBG1ON);

	slColRAMMode(CRM16_1024);

	slScrAutoDisp(NBG0ON|NBG1ON);

//	slColRAMOffsetSpr(3);	//SPRITES ARE ALL LOCKED TO PALETTE 3!!!!

}

/******************************************************************/

void	InitPalette(Uint16 *palette,Uint8 bankno)
{
	Uint16	x;
	volatile	Uint16	*CRAM	=	(Uint16 *)VDP2_COLRAM;

	if(bankno>3)
	{
		bankno	=	3;
	}

	CRAM	+=	(bankno * 256);	//Number of words to offset to the correct bank

	for(x=0;x<256;x++)
	{
		*(CRAM+x)	=	palette[x];
	}
}

/******************************************************************/

void	InitFonts(void)
{
	#ifdef	USECD
	//Load and initialise the palette
	ReadFile("FONTS.SAT",(Uint8 *)palette);
	InitPalette(palette,3);
	
	//Load the font into low ram and transfer to VRAM
	ReadFile("FONTS.BIN",LowRam);
	slDMACopy(LowRam,(Uint8 *)SpriteVRAM+BIGFONTSTART,0x6000);
	
	#else

	//Load and initialise the palette
	ReadFile("bin/fonts.sat",(Uint8 *)palette);
	InitPalette(palette,3);
	
	//Load the font into low ram and transfer to VRAM
	ReadFile("bin/fonts.bin",LowRam);
	slDMACopy(LowRam,(Uint8 *)SpriteVRAM+BIGFONTSTART,0x6000);

	#endif
}

/******************************************************************/

void	InitHiResFontClut(void)
{
	Uint16	clut[16],x;

	clut[0]	=	0;
	clut[1]	=	1;
	clut[2]	=	2;

	for(x=3;x<16;x++)
	{
		clut[x]	=	0;
	}

	slDMACopy(clut,(void *)SpriteVRAM+FONTCLUT,16*2);
}

/******************************************************************/

void	InitLowResFontClut(void)
{
	Uint16	clut[16],x;

	clut[0]	=	0;
	clut[1]	=	0xffff;
	clut[2]	=	0x8000;

	for(x=3;x<16;x++)
	{
		clut[x]	=	0;
	}

	slDMACopy(clut,(void *)SpriteVRAM+FONTCLUT,16*2);
	
}

/******************************************************************/

void	InitSound(void)
{
   SndIniDt snd_init;

	#ifdef	USECD
	ReadFile("SNDDRV.BIN",LowRam);
	#else
	ReadFile("sound/snddrv.bin",LowRam);
	#endif

   SND_INI_PRG_ADR(snd_init) 	= 		(Uint16 *)RamLow;   			/* 68k snd drv program */
   SND_INI_PRG_SZ(snd_init) 	=		fsize;							/* sound driver size */
   SND_INI_ARA_ADR(snd_init) 	= 		(Uint16 *)soundmap;  		/* sound area map */
   SND_INI_ARA_SZ(snd_init) 	= 		10; 								/* snd area map size */

   SND_Init(&snd_init);
   SND_ChgMap(0);																/* change sound area map; start with map 0 */

	#ifdef	USECD
	ReadFile("SFX1.BIN",LowRam);
	#else
	ReadFile("sound/sfx1.bin",LowRam);
	#endif

   SND_MoveData((Uint16 *)RamLow,(Uint32)fsize,SND_KD_TONE,0);

	SND_ChgEfct(0);
	SND_ChgMix(0,0);
	SND_SetTlVl(15);

	SND_SetCdDaLev(7,7);
	SND_SetCdDaPan(0,0);
}

/******************************************************************/

Uint8	play_sample(Uint8	voice,Uint8 pitch)
{
	Uint8		retval	=	midi_channel;
	SndRet	a;

	//change channel and voice number
  	a=SND_CtrlDirMidi(0x00,0x00,0x04,midi_channel,voice,0x00);

	//set volume
  	a=SND_CtrlDirMidi(0x00,0x00,0x03,midi_channel,7,127);

	//set pan
  	a=SND_CtrlDirMidi(0x00,0x00,0x03,midi_channel,10,0x40); //set to centre

	//play the sample
   a=SND_CtrlDirMidi(0x00,0x00,0x01,midi_channel,pitch,127);

	midi_channel++;
	midi_channel&=31;

	return(retval);

}

/******************************************************************/

Uint8	play_sample_with_volume(Uint8	voice,Uint8 pitch,Uint8 volume)
{
	Uint8	retval	=	midi_channel;
	SndRet	a;

	//change channel and voice
  	a=SND_CtrlDirMidi(0x00,0x00,0x04,midi_channel,voice,0x00);

	//set volume
  	a=SND_CtrlDirMidi(0x00,0x00,0x03,midi_channel,7,volume);

	//set pan
  	a=SND_CtrlDirMidi(0x00,0x00,0x03,midi_channel,10,0x40); //set to centre

	//play the sample
   a=SND_CtrlDirMidi(0x00,0x00,0x01,midi_channel,pitch,127);
	
	midi_channel++;
	midi_channel&=31;

	return(retval);
}

/******************************************************************/

Uint8	play_sample_with_volume_and_direction(Uint8	voice,Uint8 pitch,Uint8 volume,Sint8 direction)
{
	Uint8	retval	=	midi_channel;
	SndRet	a;

	//change channel and voice
  	a=SND_CtrlDirMidi(0x00,0x00,0x04,midi_channel,voice,0x00);

	//change volume
  	a=SND_CtrlDirMidi(0x00,0x00,0x03,midi_channel,0x7,volume);

	//change pan direction
  	a=SND_CtrlDirMidi(0x00,0x00,0x03,midi_channel,0x10,direction);
	
	//play the sample
   a=SND_CtrlDirMidi(0x00,0x00,0x01,midi_channel,pitch,volume);
	
	midi_channel++;
	midi_channel&=31;

	return(retval);
}

/******************************************************************/

void	SetTextureTable(TEXTURE *Textures)
{
	/* This function initialises the texture table that will set up VDP1
		VRAM for the dynamic image loading manager */

	register	Uint32	index=0;
	register	Uint32	acc=0;
	register	Uint32	zulu=0;


	/* 8x8 */
	zulu	=	0;
	acc	=	(index+Num8x8);
	while(index <  acc)
	{
		Textures[index].Hsize		=	8;
		Textures[index].Vsize		=	8;
		Textures[index].CGadr		=	((Start8x8) + ((((8*8)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize		=	8<<5 | 8;
		
		index++;
		zulu++;

	}

	/* 16x16 */
	zulu	=	0;
	acc	=	(index+Num16x16);
	while(index <  acc)
	{
		Textures[index].Hsize		=	16;
		Textures[index].Vsize		=	16;
		Textures[index].CGadr		=	((Start16x16) + ((((16*16)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize		=	16<<5 | 16;
		
		index++;
		zulu++;

	}

	/* 24x24 */
	zulu	=	0;
	acc	=	(index+Num24x24);
	while(index <  acc)
	{
		Textures[index].Hsize		=	24;
		Textures[index].Vsize		=	24;
		Textures[index].CGadr		=	((Start24x24) + ((((24*24)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize		=	24<<5 | 24;
		
		index++;
		zulu++;

	}



	/* 32x32 */
	zulu	=	0;
	acc	=	(index+Num32x32);
	while(index <  acc)
	{
		Textures[index].Hsize		=	32;
		Textures[index].Vsize		=	32;
		Textures[index].CGadr		=	((Start32x32) + ((((32*32)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize		=	32<<5 | 32;
		
		index++;
		zulu++;
	}



	/* 48x48 */
	zulu	=	0;
	acc	=	(index+Num48x48);
	while(index <  acc)
	{
		Textures[index].Hsize		=	48;
		Textures[index].Vsize		=	48;
		Textures[index].CGadr		=	((Start48x48) + ((((48*48)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize		=	48<<5 | 48;

		index++;
		zulu++;
	}



	/* 64x64 */
	zulu	=	0;
	acc	=	(index+Num64x64);
	while(index <  acc)
	{
		Textures[index].Hsize		=	64;
		Textures[index].Vsize		=	64;
		Textures[index].CGadr		=	((Start64x64) + ((((64*64)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize		=	64<<5 | 64;

		index++;
		zulu++;
	}



	/* 80x80 */
	zulu	=	0;
	acc	=	(index+Num80x80);
	while(index <  acc)
	{
		Textures[index].Hsize		=	80;
		Textures[index].Vsize		=	80;
		Textures[index].CGadr		=	((Start80x80) + ((((80*80)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize		=	80<<5 | 80;

		index++;
		zulu++;
	}



	/* 96x96 */
	zulu	=	0;
	acc	=	(index+Num96x96);
	while(index <  acc)
	{
		Textures[index].Hsize		=	96;
		Textures[index].Vsize		=	96;
		Textures[index].CGadr		=	((Start96x96) + ((((96*96)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize		=	96<<5 | 96;

		index++;
		zulu++;
	}



	/* 128x128 */
	zulu	=	0;
	acc	=	(index+Num128x128);
	while(index <  acc)
	{
		Textures[index].Hsize		=	128;
		Textures[index].Vsize		=	128;
		Textures[index].CGadr		=	((Start128x128) + ((((128*128)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize		=	128<<5 | 128;

		index++;
		zulu++;
	}

	//******* THE FONTS *********

	//BIGFONT
	zulu	=	0;
	acc	=	(index+BIGFONTNUM);
	while(index <  acc)
	{
		Textures[index].Hsize	=	16;
		Textures[index].Vsize	=	16;
		Textures[index].CGadr	=	((BIGFONTSTART) + ((((16*16)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize	=	16<<5 | 16;
		
		index++;
		zulu++;
	}
	

	//MEDIUM FONT
	zulu	=	0;
	acc	=	(index+MEDFONTNUM);
	while(index <  acc)
	{
		Textures[index].Hsize	=	16;
		Textures[index].Vsize	=	16;
		Textures[index].CGadr	=	((MEDFONTSTART) + ((((16*16)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize	=	16<<5 | 16;
		
		index++;
		zulu++;
	}

	//SMALL FONT
	zulu	=	0;
	acc	=	(index+SMLFONTNUM);
	while(index <  acc)
	{

		Textures[index].Hsize	=	16;
		Textures[index].Vsize	=	16;
		Textures[index].CGadr	=	((SMLFONTSTART) + ((((16*16)*zulu)*4) >> (myPAL))) /8;
		Textures[index].HVsize	=	16<<5 | 16;
		
		index++;
		zulu++;
	}

	zulu	=	0;

	
	// These sprites are stored after the slots and before the cluts

	//BORDER
	Textures[index].Hsize	=	56;
	Textures[index].Vsize	=	68;
	Textures[index].CGadr	=	(BORDERSTART) /8;
//	Textures[index].CGadr	=	((BORDERSTART) + ((((56*68)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	56<<5 | 68;

	index++;

	//SHIPWIRE
	Textures[index].Hsize	=	24;
	Textures[index].Vsize	=	16;
	Textures[index].CGadr	=	((SHIPWIRESTART) + ((((24*16)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	24<<5 | 16;

	index++;

	//******* WEAPON WIREFRAMES *********

	//BKWIRE
	Textures[index].Hsize	=	16;
	Textures[index].Vsize	=	14;
	Textures[index].CGadr	=	((BKWIRESTART) + ((((16*14)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	16<<5 | 14;
	index++;

	//CMNWIRE
	Textures[index].Hsize	=	32;
	Textures[index].Vsize	=	15;
	Textures[index].CGadr	=	((CMNWIRESTART) + ((((32*15)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	32<<5 | 15;
	index++;

	//EBWIRE
	Textures[index].Hsize	=	16;
	Textures[index].Vsize	=	13;
	Textures[index].CGadr	=	((EBWIRESTART) + ((((16*13)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	16<<5 | 13;
	index++;

	//LZWIRE
	Textures[index].Hsize	=	24;
	Textures[index].Vsize	=	12;
	Textures[index].CGadr	=	((LZWIRESTART) + ((((24*12)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	24<<5 | 12;
	index++;

	//MISSWIRE
	Textures[index].Hsize	=	24;
	Textures[index].Vsize	=	14;
	Textures[index].CGadr	=	((MISSWIRESTART) + ((((24*14)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	24<<5 | 14;
	index++;

	//MNWIRE
	Textures[index].Hsize	=	16;
	Textures[index].Vsize	=	14;
	Textures[index].CGadr	=	((MNWIRESTART) + ((((24*14)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	16<<5 | 14;
	index++;

	//******* STATUS BARS *********

	//BARSL
	Textures[index].Hsize	=	16;
	Textures[index].Vsize	=	27;
	Textures[index].CGadr	=	((BARSLSTART) + ((((16*27)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	16<<5 | 27;

	index++;

	//BARSR
	Textures[index].Hsize	=	16;
	Textures[index].Vsize	=	27;
	Textures[index].CGadr	=	((BARSRSTART) + ((((16*27)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	16<<5 | 27;

	index++;

	//******* VINES *********

	//VIL1
	Textures[index].Hsize	=	56;
	Textures[index].Vsize	=	91;
	Textures[index].CGadr	=	((VIL1START) + ((((56*91)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	56<<5 | 91;

	index++;

	//VIL2
	Textures[index].Hsize	=	32;
	Textures[index].Vsize	=	86;
	Textures[index].CGadr	=	((VIL2START) + ((((32*86)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	32<<5 | 86;

	index++;

	//VIL3
	Textures[index].Hsize	=	32;
	Textures[index].Vsize	=	50;
	Textures[index].CGadr	=	((VIL3START) + ((((32*50)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	32<<5 | 50;

	index++;

	//VIL4
	Textures[index].Hsize	=	32;
	Textures[index].Vsize	=	38;
	Textures[index].CGadr	=	((VIL4START) + ((((32*38)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	32<<5 | 38;

	index++;

	//VIL5
	Textures[index].Hsize	=	16;
	Textures[index].Vsize	=	36;
	Textures[index].CGadr	=	((VIL5START) + ((((16*36)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	16<<5 | 36;

	index++;

	//VIL6
	Textures[index].Hsize	=	24;
	Textures[index].Vsize	=	101;
	Textures[index].CGadr	=	((VIL6START) + ((((24*101)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	24<<5 | 101;

	index++;

	//VIL7
	Textures[index].Hsize	=	16;
	Textures[index].Vsize	=	27;
	Textures[index].CGadr	=	((VIL7START) + ((((16*27)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	16<<5 | 27;

	index++;

	//VIL8
	Textures[index].Hsize	=	32;
	Textures[index].Vsize	=	58;
	Textures[index].CGadr	=	((VIL8START) + ((((32*58)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	32<<5 | 58;

	index++;

	//VIL9
	Textures[index].Hsize	=	24;
	Textures[index].Vsize	=	36;
	Textures[index].CGadr	=	((VIL9START) + ((((24*36)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	24<<5 | 36;

	index++;

	//VIL10
	Textures[index].Hsize	=	24;
	Textures[index].Vsize	=	66;
	Textures[index].CGadr	=	((VIL10START) + ((((24*66)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	24<<5 | 66;

	index++;

	//VIL11
	Textures[index].Hsize	=	24;
	Textures[index].Vsize	=	28;
	Textures[index].CGadr	=	((VIL11START) + ((((24*28)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	24<<5 | 28;

	index++;

	//VIL12
	Textures[index].Hsize	=	32;
	Textures[index].Vsize	=	44;
	Textures[index].CGadr	=	((VIL12START) + ((((32*44)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	32<<5 | 44;

	index++;

	//VIL13
	Textures[index].Hsize	=	32;
	Textures[index].Vsize	=	44;
	Textures[index].CGadr	=	((VIL13START) + ((((32*44)*zulu)*4) >> (myPAL))) /8;
	Textures[index].HVsize	=	32<<5 | 44;

	index++;
	
	//******* THE PORTS *********
	
	//PORTDL
	Textures[index].Hsize	=	40;
	Textures[index].Vsize	=	9;
	Textures[index].CGadr	=	((PORTDLSTART)) /8;
	Textures[index].HVsize	=	40<<5 | 9;

	index++;
	
	//PORTDU
	Textures[index].Hsize	=	48;
	Textures[index].Vsize	=	14;
	Textures[index].CGadr	=	((PORTDUSTART)) /8;
	Textures[index].HVsize	=	48<<5 | 14;

	index++;
	
	//PORTLL
	Textures[index].Hsize	=	16;
	Textures[index].Vsize	=	35;
	Textures[index].CGadr	=	((PORTLLSTART)) /8;
	Textures[index].HVsize	=	16<<5 | 35;

	index++;
	
	//PORTLU
	Textures[index].Hsize	=	24;
	Textures[index].Vsize	=	49;
	Textures[index].CGadr	=	((PORTLUSTART)) /8;
	Textures[index].HVsize	=	24<<5 | 49;

	index++;
	
	//PORTRL
	Textures[index].Hsize	=	8;
	Textures[index].Vsize	=	36;
	Textures[index].CGadr	=	((PORTRLSTART)) /8;
	Textures[index].HVsize	=	8<<5 | 36;

	index++;
	
	//PORTRU
	Textures[index].Hsize	=	24;
	Textures[index].Vsize	=	51;
	Textures[index].CGadr	=	((PORTRUSTART)) /8;
	Textures[index].HVsize	=	24<<5 | 51;

	index++;
	
	//PORTUL
	Textures[index].Hsize	=	32;
	Textures[index].Vsize	=	8;
	Textures[index].CGadr	=	((PORTULSTART)) /8;
	Textures[index].HVsize	=	32<<5 | 8;

	index++;

	//PORTUU
	Textures[index].Hsize	=	48;
	Textures[index].Vsize	=	16;
	Textures[index].CGadr	=	((PORTUUSTART)) /8;
	Textures[index].HVsize	=	48<<5 | 16;

	index++;

}

/******************************************************************/

void msg(char *ptr)
{
	PCwrite(-1,ptr,strlen(ptr));
}

/******************************************************************/

void clearmsg(void)
{
//	PCwrite(-1,"\f",1);
}

/******************************************************************/

void	PrintLARGE(Uint16 x,Uint16 y,Uint8 *string)
{
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,(FONTCLUT/8),No_Window|No_Gouraud,CL16Look|ECdis|MESHoff|SPenb,sprNoflip);
	FIXED			Pos[4];
	register		Uint8			chr;
	register		Uint16		count=0;
	register		Uint16		t=0;
	

	Pos[1]	=	y*65536;
	Pos[2]	=	170*65536;							  
	Pos[3]	=	ORIGINAL*65536;
	
	x	-=	8;

	while((*string)!=0)
	{
		chr	=	*(string++);
		chr	-=	32;

		t		=	x;
		t		+=	count;
		count	+=	large[chr];

		Pos[0]			=	t*65536;

		Normal.texno	=	(chr+BIGFONTSPRITE);
		slDispSprite(Pos,&Normal,0);

	}

}

/******************************************************************/

void	PrintMEDIUM(Uint16 x,Uint16 y,Uint8 *string)
{
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,(FONTCLUT/8),No_Window|No_Gouraud,CL16Look|ECdis|MESHoff|SPenb,sprNoflip);
	FIXED			Pos[4];
	register		Uint8			chr;
	register		Uint16		t=0,count=0;
	
	Pos[1]	=	y*65536;
	Pos[2]	=	170*65536;
	Pos[3]	=	ORIGINAL*65536;
	
	x	-=	8;

	while((*string)!=0)
	{
		chr	=	*(string++);
		chr	-=	32;

		t		=	x;
		t		+=	count;
		count	+=	med[chr];

		Pos[0]			=	t*65536;

		Normal.texno	=	(chr+MEDFONTSPRITE);
		slDispSprite(Pos,&Normal,0);

	}

}

/******************************************************************/

void	PrintSMALL(Uint16 x,Uint16 y,Uint8 *string)
{
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,(FONTCLUT/8),No_Window|No_Gouraud,CL16Look|ECdis|MESHoff|SPenb,sprNoflip);
	FIXED			Pos[4];
	register		Uint8			chr;
	register		Uint16		t=0,count=0;
	
	Pos[1]	=	y*65536;
	Pos[2]	=	170*65536;
	Pos[3]	=	ORIGINAL*65536;
	
	x	-=	8;

	while((*string)!=0)
	{
		chr	=	*(string++);
		chr	-=	32;

		t		=	x;
		t		+=	count;
		count	+=	small[chr];

		Pos[0]			=	t*65536;

		Normal.texno	=	(chr+SMLFONTSPRITE);
		slDispSprite(Pos,&Normal,0);

	}

}

/******************************************************************/


