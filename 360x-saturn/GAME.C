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
/******************************************************************/
 
#include <sgl.h>
#include <libsn.h>
#include "includes\defines.h"
#include	"includes\types.h"
#include "includes\globals.h"
#include <sega_cdc.h>
#include	<stdio.h>
#include "360x.h"
#include <sega_snd.h>

/* ProtoTypes *****************************************************/

void		ss_main(void);
void		Game(void);
void		FillScreenWithCrap(void);
void		VblankRoutine(void);
void		LoadGraphics(void);
void		CalculateSpriteNumbers(Uint8 *Buffer);
void 		msg(char *ptr);
void 		clearmsg(void);
void 		SetFileNames(void);
void		Poll(void);
void		HandlePauseMode(void);
void		SlaveResetVbar(void);
void		SlaveSetVbar(void);
void		init_level(void);
void		play_level(void);

/* Externals ******************************************************/

extern	void			InitSaturn(void);
extern	void			InitFileSystem(void);
extern	void			ReadFile(Uint8 *name,Uint8 *target);
extern	void			ProcessPad(void);
extern	void			InitPalette(Uint16 *palette,Uint8 bankno);
extern	void			SetUpVramData(long	xpos,long	ypos);
extern	void			LoadBackgroundData(char *levtext);
extern	void			InitialiseSpriteArrays(SlotType *SlotData);
extern	void			ClearSlotAllocationArrays(SlotType *SlotData);
extern	flag 			load_level_sprites(char *fname);
extern	flag 			process_script(void);
extern	void 			process_sprites(void);
extern	void 			clear_sprites(void);
extern	flag 			SHcollision_at(long x,long y);
extern	void			Init_Ball(void);
extern	void			Init_Fire(void);
extern	void			Init_Particle_List(void);
extern	void			Draw_Pixels(void);
extern	void			Draw_Fire(void);
extern	void			Move_Craft(void);
extern	void			Move_Particles(void);
extern	void			Move_Fire(void);
extern	void			InitPanel(void);
extern	void			PrintPanel(void);
extern	void			InitGameScreen(void);
extern	void			InitNormalTitleScreen(void);
extern	void			InitRotateTitleScreen(void);
extern	void			DrawBackGroundSprites(void);
extern	void			DrawSpecialSprites(void);
extern	void			SetUpStaticSprites(void);
extern	int			memset(void *,int,int);
extern	int			rand(void);
extern	void			PrintLARGE(Uint16 x,Uint16 y,Uint8 *string);
extern	void			PrintMEDIUM(Uint16 x,Uint16 y,Uint8 *string);
extern	void			PrintSMALL(Uint16 x,Uint16 y,Uint8 *string);
extern	void			PlayCDAudio(Uint16	trackno);
extern	void			PauseCDAudio(void);
extern	void			ResumeCDAudio(void);
extern	void			StopCDAudio(void);
extern	void			Control(void);
extern	void			ClearVDP1VRAM(void);
extern	void			ClearVDP2VRAM(void);
extern	flag			LoadAttributesData(void);
extern	Uint8			play_sample(Uint8	channel,Uint8 pitch);
extern	Uint8			play_sample_with_volume(Uint8	voice,Uint8 pitch,Uint8 volume);
extern	Uint8			play_sample_with_volume_and_direction(Uint8	voice,Uint8 pitch,Uint8 volume,Sint8 direction);
extern	void			init_lists(void);
extern	void			UpdateVerticalEdge(UpdateType	*data);
extern	void			UpdateHorizontalEdge(UpdateType	*data);
extern	void 			place_craft(void);
extern	void			validate_sprite_list(void);

extern	Uint16		FRT_Count;
extern	Uint16		Xpos[];
extern	Uint8			Done;
extern	UPDATE		HorzUpdateFunctions[];
extern	UPDATE		VertUpdateFunctions[];
extern	Uint16		DCount;
extern	Sint16		r,g,b;
extern	Sint8			FadeFlag;
extern	SlotType		Slots; 			//Slot allocation data
extern	Uint16		palette[];
extern	Uint32		mapindex,vramindex;
extern	Uint32		Hmapindex,Hvramindex;
extern	long			frac_craft_x,frac_craft_y;
extern	UpdateType	HorizontalUpdateList[];
extern	UpdateType	VerticalUpdateList[];
extern	Uint8			UpdateVFlag;
extern	Uint8			UpdateHFlag;

/* Globals ********************************************************/

char			*fnames_new[NAME_END];
Uint8			DecompBuffer[((128*128)/2)];
//Uint32		*DD				=	(Uint32 *)DecompBuffer;
Uint8			*collmap			=	(Uint8 *)SoundRAM+0x5c800; //370k into soundram

Sint16		ScrollX			=	0;
Sint16		ScrollY			=	0;
Sint16		LastScrollX		=	0;
Sint16		LastScrollY		=	0;

Uint16		MapWidth			= 	0;
Uint16		MapHeight		=	0;
Uint16		MapCellWidth	= 	0;
Uint16		MapCellHeight	=	0;
Uint8			BigBuffer[LoadSize];		//GFX Load area
Uint8			*Vdst				=	(Uint8 *)SpriteVRAM+SpriteStart;

Uint32		SpriteMAX			=	0;
//Uint32		MaxSpritesLoaded	=	0;

/* Game Globals ***************************************************/

#include "includes\gamedefs.h"

Uint8		GamePauseFlag=0;
Uint8		PauseOption;

Uint8		toggle				=	0;
Uint8		incflag				=	0;

flag		change_level		=	FALSE;
flag		escape				=	FALSE;
flag		GammaFlag			=	FALSE;
flag		HandlePauseFlag	=	FALSE;
Uint8		PauseHandleMode	=	0;
Sint8		HandlePauseMenu	=	MENUMODE;
Uint8		CdTrack				=	1;
Sint16	CDAudioVolume		=	127;
Uint32	score					=	0;
Sint16	SFXVolume			=	127;
Sint16	Gamma					=	0;
Sint16	LastGamma			=	0;

/******************************************************************/
/******************************************************************/
/******************************************************************/


void	init_level(void)
{

	InitGameScreen();
	
	InitialiseSpriteArrays(&Slots);

	memset(DecompBuffer,0,8192);	//clear sprite decompression buffer

	clear_sprites();

	LoadAttributesData();
		   
	process_script();

	place_craft();

	InitPanel();

	init_lists();

	LoadGraphics();

	SetUpStaticSprites();
	
	validate_sprite_list();

	Init_Ball();

	Init_Fire();

	Init_Particle_List();

	//Register VBLANK function
	slIntFunction(VblankRoutine);

	SetUpVramData(craftx,crafty);
	
	DrawBackGroundSprites();
	
	DrawSpecialSprites();

	slSynch();
}

void	play_level(void)
{
	Uint8		r=0;

	PlayCDAudio(CdTrack);

	/* --- MAIN LOOP --- */
	while(escape==FALSE)
	{
		
	  	pollhost();

		DbCol(0,0,31);

		SHIPCOL(r,0,0);

		r++;r&=0x1f;

		PrintPanel();

		ProcessPad();

		if(GamePauseFlag!=PAUSED)
		{
		  	process_sprites();

			if(toggle==3)
			{
				ResumeCDAudio();
				toggle=0;
			}
		}

	  	Draw_Pixels();
		
		DrawBackGroundSprites();
		DrawSpecialSprites();

		if(GamePauseFlag!=PAUSED)
		{
			Move_Craft();
			Move_Fire();
			Move_Particles();

			if( SHcollision_at(craftx,crafty)==TRUE )
			{
				PrintLARGE(40,10,"ARGH!");
			}
			
			score+=10;
		}

		slSlaveFunc(&ClearSlotAllocationArrays,(SlotType *)&Slots);

		if(GamePauseFlag==PAUSED)
		{
			if(toggle==1)
			{
				PauseCDAudio();
				toggle=2;
			}

			if(HandlePauseFlag==FALSE)
			{
				PrintLARGE(142,112,"PAUSED");
			}
			else
			{
				HandlePauseMode();
			}
		}

		frac_craft_x	=	craftx;
		frac_craft_y	=	crafty;

		//slSlaveFunc(&SlaveResetVbar,NULL);
		//reset_vbar(38);

		DbCol(0,0,0);

		slSynch();
		//set_vbar(38);
		//slSlaveFunc(&SlaveSetVbar,NULL);

	}

}								                          
		
/******************************************************************/

void	VblankRoutine(void)
{
	//pollhost();
	// change background colour to white so if it runs out of time
	// while updating the edges, we should see it bleeding onto 
	// the main game screen

	DbCol(31,31,31);	//Start of VBLANK

	if(UpdateVFlag!=0)
	{
		UpdateVerticalEdge(VerticalUpdateList);
		UpdateVFlag=0;
	}

	if(UpdateHFlag!=0)
	{
		UpdateHorizontalEdge(HorizontalUpdateList);
		UpdateHFlag=0;
	}

	slLookR(ScrollX*65536,ScrollY*65536);

	//Flag to increment wireframe graphics for panel
	incflag	^=	1;

	DbCol(0,0,0);	//End of VBLANK

}

/******************************************************************/

void	LoadGraphics(void)
{
	
	SetFileNames();

 	load_level_sprites(NULL);

	CalculateSpriteNumbers(BigBuffer);

	//Load background	has to be done last as it totally fills low ram
	LoadBackgroundData(leveldata[Level]);

	iff_width	=	MapWidth;
	iff_height	=	MapHeight;
}

/******************************************************************/
// This function scans all the sprite data and gets how many frames
// there are in each block. This is then added to sprite max. That
// way, each frame from each sprite is treated as an individual
// sprite that we can use with CheckSprite() to avoid downloading
// lots of data in the vblank
/******************************************************************/

void	CalculateSpriteNumbers(Uint8 *Buffer)
{

	Uint32	x	=	0;
	Uint32	Frames;
	Uint16	*data;

	SpriteMAX	=	0;

	while(x<NAME_END)
	{

		//This is used to see what sprite number the current sprite starts
		//at

		if(seqdata[x].data != (char *)0xffffffff) //active sprite?
		{
			seqdata[x].offset	=	SpriteMAX; //texno this sprite starts at

			(char *)data		=	seqdata[x].data;

			Frames		=	*(data);

			seqdata[x].frames	=	Frames;

			SpriteMAX	+=	Frames; //add on no. of frames this sprite has
		}

		x++;

	}

}

/******************************************************************/

void SetFileNames(void)
{
	Uint16	i;

	for(i=0;i<NAME_END;i++)
	{
		fnames_new[i]	=	(void *)-1;
	}

	fnames_new[NAME_BUCK01]			=	"BUCK01.SCS";
	fnames_new[NAME_CHAIN3]			=	"CHLINK.SCS";
	fnames_new[NAME_EBALL]			=	"EBALL_S.SCS";
	fnames_new[NAME_GUIDED]			=	"GUIDED.SCS";
	fnames_new[NAME_mine2]			=	"mine_1.SCS";
//	fnames_new[NAME_exp1]			=	"S_EXP1.SCS";
	fnames_new[NAME_exp2]			=	"S_EXP2.SCS";
	fnames_new[NAME_exp3]			=	"S_EXP3.SCS";
	fnames_new[NAME_exp4]			=	"S_EXP4.SCS";
//	fnames_new[NAME_exp5]			=	"S_EXP5.SCS";
	fnames_new[NAME_exp6]			=	"S_EXP6.SCS";
	fnames_new[NAME_exp7]			=	"S_EXP7.SCS";
	fnames_new[NAME_FLOPPY]			=	"FLOPL1.SCS";
	fnames_new[NAME_l1c1]		 	=	"L1C1.SCS";
	fnames_new[NAME_l1c2]		 	=	"L1C2.SCS";
	fnames_new[NAME_l1c3]		 	=	"L1C3.SCS";
	fnames_new[NAME_l1c4]		 	=	"L1C4.SCS";
	fnames_new[NAME_l1c5]		 	=	"L1C5.SCS";
	fnames_new[NAME_SMINE1]			=	"MINE_1.SCS";
	fnames_new[NAME_LEAPER]			=	"PLUNGL1.SCS";  //frig!
	fnames_new[NAME_acidspt]		=	"S_ACIDSP.SCS";
	fnames_new[NAME_boss1]			=	"S_BOSS1.SCS";
	fnames_new[NAME_EXPLOSION]		=	"S_EXP1.SCS";
	fnames_new[NAME_FLOATER]		=	"S_FLOAT.SCS";
	fnames_new[NAME_JELLO]			=	"S_JELLO.SCS";
	fnames_new[NAME_chapl1]			=	"S_L1CHAP.SCS";
	fnames_new[NAME_MUSSEL]			=	"S_L1MUS.SCS";
	fnames_new[NAME_lazerl1]		=	"S_LASER.SCS";
	fnames_new[NAME_lrtad]			=	"S_LRTAD.SCS";
	fnames_new[NAME_lmspit]			=	"S_LMSSPT.SCS";
	fnames_new[NAME_survrs]			=	"S_MAN.SCS";
	fnames_new[NAME_mrspit]			=	"S_MRSPIT.SCS";
	fnames_new[NAME_msspit]			=	"S_MSSPIT.SCS";
	fnames_new[NAME_POD]				=	"S_POD.SCS";
	fnames_new[NAME_RCK1]	  		=	"S_RCK01.SCS";
	fnames_new[NAME_RCK2]	  		=	"S_RCK02.SCS";
	fnames_new[NAME_RCK3]	  		=	"S_RCK03.SCS";
	fnames_new[NAME_RCKMORPH]		=	"S_RCKMPH.SCS";
	fnames_new[NAME_CSHIP]		 	=	"S_SHIP.SCS";
	fnames_new[NAME_spfire]			=	"S_SPFIRE.SCS";
	fnames_new[NAME_STAR]		 	=	"S_STAR.SCS";
	fnames_new[NAME_STARFSH]		=	"S_STARFS.SCS";
	fnames_new[NAME_tad]				=	"S_TAD.SCS";
	fnames_new[NAME_teletemp] 		=	"S_TELEP.SCS";
	fnames_new[NAME_TENTAC1]		=	"S_TENTA1.SCS";
	fnames_new[NAME_TENTAC2]		=	"S_TENTA2.SCS";
	fnames_new[NAME_tonsl1]	 		=	"S_TNSL1.SCS";
	fnames_new[NAME_tubel1]			=	"S_TUBEL1.SCS";
	fnames_new[NAME_turret1]		=	"S_TURRET.SCS";
	fnames_new[NAME_windy]	  		=	"S_WINDY.SCS";
												
} 

/******************************************************************/

void	HandlePauseMode(void)
{
 	SPRITE	bar[1];

	bar[0].CTRL		=	FUNC_Polygon|0;
	bar[0].PMOD		=	CL_Gouraud|Window_In|ECdis;
	bar[0].COLR		=	C_RGB(0,0,31);
	bar[0].XA		=	160;
	bar[0].YA		=	200;
	bar[0].XB		=	160;
	bar[0].YB		=	200;
	bar[0].XC		=	160;
	bar[0].YC		=	206;
	bar[0].XD		=	160;
	bar[0].YD		=	206;
	bar[0].GRDA		=	0x2080;		// Must make proper Gouraud table later
	bar[0].DMMY		=	0;


	//Carrys out commands for the menu

	switch(HandlePauseMenu)
	{

		case	MENUMODE:
		{
			PrintMEDIUM(142,112,"RESUME");
			PrintMEDIUM(142,128,"ADJUST GAMMA");
			PrintMEDIUM(142,144,"CD MUSIC VOLUME");
			PrintMEDIUM(142,160,"SFX VOLUME");
			PrintMEDIUM(126,112+(PauseOption*16),">");
			break;
		}

		case	RESUME:
		{
			GamePauseFlag = 0;
			HandlePauseFlag=FALSE;
			HandlePauseMenu=MENUMODE;
			break;
		}
		
		case	GAMMA:
		{
			PrintMEDIUM(122,112,"ADJUST GAMMA");
			PrintSMALL(52,210,"DARKER");
			PrintSMALL(245,210,"BRIGHTER");

			bar[0].COLR		=	C_RGB(0,0,31);
			bar[0].XB		=	160+(Gamma*4);
			bar[0].XC		=	160+(Gamma*4);
			
			slSetSprite(bar,170*65536);
			
			if(Gamma!=LastGamma)
			{
				slColOffsetA(Gamma,Gamma,Gamma);
				LastGamma=Gamma;
			}

			break;
		}

		case	CD_VOLUME:
		{
			PrintMEDIUM(138,112,"CD VOLUME");
			PrintSMALL(55,210,"MIN");
			PrintSMALL(285,210,"MAX");
			bar[0].COLR		=	C_RGB(31,0,0);
			bar[0].XA		=	40;
			bar[0].XB		=	40+CDAudioVolume*2;
			bar[0].XC		=	40+CDAudioVolume*2;
			bar[0].XD		=	40;
			//slCDDAOn(CDAudioVolume,CDAudioVolume,0,0);
			SND_SetCdDaLev((CDAudioVolume/18),(CDAudioVolume/18));
			slSetSprite(bar,170*65536);
			break;
		}

		case	SFX_VOLUME:
		{
			PrintMEDIUM(132,112,"SFX VOLUME");
			PrintSMALL(55,210,"MIN");
			PrintSMALL(285,210,"MAX");
			bar[0].COLR		=	C_RGB(0,31,31);
			bar[0].XA		=	40;
			bar[0].XB		=	40+SFXVolume*2;
			bar[0].XC		=	40+SFXVolume*2;
			bar[0].XD		=	40;
			//slSndVolume(SFXVolume); Changes CDDA volume as well. Scrap
			slSetSprite(bar,170*65536);
			break;
		}

		default:
		{
			break;
		}
	}


}

/******************************************************************/

void	SlaveResetVbar(void)
{
		reset_vbar(2);
		
}

/******************************************************************/

void	SlaveSetVbar(void)
{
		set_vbar(2);
}

/******************************************************************/
