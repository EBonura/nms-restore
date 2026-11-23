/******************************************************************/
//
// VDP1_4.C
//
// Contains font printing and wrapper code
//
/******************************************************************/
 
#include <sgl.h>
#include <sega_bup.h>
#include <libsn.h>
#include "defines.h"
#include	"types.h"
#include	<stdio.h>
#include "globals.h"


/* Globals ********************************************************/

//#include "print.h"

Uint8		flash=0;
Uint8		Pflag=FALSE;
Sint8		FadeFlag=NOFADE;

Sint16	r=0,g=0,b=0;

Uint32	gamecycle=0;
Uint8		retval=0;
Sint8		exitcode=0;

Uint16	testcolor=0;

Uint16	optext[]=
{
	40,
	60,
	80,
	100,
	120,
	150,
};

Uint8	OpText[][15]=
{
	"DIFFICULTY",
	"CONTROL",
	"AUDIO",
	"MUSIC TEST",
	"SFX TEST",
	"EXIT",
};

Uint8	Screen	=	TITLE;

Uint16	Gouraud[]=
{
	C_RGB(0,31,0),
	C_RGB(0,8,0),
	C_RGB(0,0,8),
	C_RGB(8,8,0),
};

/* ProtoTypes *****************************************************/

void	LoadScreen(Uint32	addr,Uint8 palno,Uint8	*fname,Uint8 flag);
void	PrintTele(Uint16 x,Uint16 y,Uint8 string);
void	PrintScaled(Sint16 x,Sint16 y,Uint8 *string);

void	Control(void);
void	TitleScreen(void);
void	LevelSelect(void);
void	TitleVBL(void);
void	PlanetLevelSelect(void);
void	CopyScreen(Uint8	*addr);
void	CopyRotateScreen(Uint8	*addr);
void 	DisplayMissionBrief(void);
void	TransferPlanetCluts(Uint8 *addr,Uint8 *tgt);
void	DisplayCredits(void);
void 	DisplayHiscores(void);
void	OptionScreen(void);
void	HandleOptions(void);


Uint32 sqr_root(Uint16 val);

/* Externals ******************************************************/

extern	Uint8			large[];
extern	Uint8 		med[];
extern	Uint8			small[];

extern	char			stat_str[];
extern	Uint16		spnum;
extern	long			testname;
extern	Uint16		palette[];
extern	void			InitPalette(Uint16 *palette,Uint8 bankno);
extern	void			InitBackGrounds(void);
extern	void			ReadFile(Uint8 *name,Uint8 *target);
extern	Uint8			Level;
//extern	Uint16		OldPadData;
extern	Uint8			BigBuffer[];
extern	SpriteType	DecodeGfx(Uint8 frame,Uint32 *src,Uint8 *dst,Uint16 swidth,Uint16 sheight);
extern	int			rand(void);

extern	void			PlayCDAudio(Uint16	trackno);
extern	void			PauseCDAudio(void);
extern	void			ResumeCDAudio(void);
extern	void			StopCDAudio(void);

extern	void			BackUpInit(BupConfig *cntb);
extern	Sint32		BackUpWrite(Uint32 device, BupDir *dir, Uint8 *data, Uint8 sw);
extern	Sint32		BackUpDelete(Uint32 device,Uint8 *filename);
extern	Sint32		BackUpFormat(Uint32 device);
extern	void 			UpdateBackupRam(void);

extern	void			InitNormalTitleScreen(void);
extern	void			InitRotateTitleScreen(void);

extern	void			InitFibreOptics(void);
extern	void			UpdateOptics(void);
extern	void			PlotOptics(void);

extern	void			InitStarList(void);
extern	void			AddStar(void);
extern	void			PlotStars(void);
extern	void			EndStars(void);

extern	void			InitRotateStarList(void);
extern	void			AddRotateStar(void);
extern	void			PlotRotateStars(void);
extern	void			EndRotateStars(void);

extern	void			InitGravitons(void);
extern	void			UpdateGravitons(FIXED time_step);
extern	void			PlotGravitons(void);

extern	void			Game(void);

extern	Uint16		ActiveStars;
extern	Uint16		LastStar;

extern	StarType		*StarList;
extern	gravitype	*Gravitons;
extern	Fibre			*Optics;

extern	void			DoFixedPlasmaField1(void);
extern	void			DoFixedPlasmaField2(void);
extern	void			DoPlasmaField(void);

extern	HIGH			highest[];
extern	Uint8			SystemStatus;

extern	void			PrintLARGE(Uint16 x,Uint16 y,Uint8 *string);
extern	void			PrintMEDIUM(Uint16 x,Uint16 y,Uint8 *string);
extern	void			PrintSMALL(Uint16 x,Uint16 y,Uint8 *string);

/* Misc ***********************************************************/

#define	TEXTTOP	(-16)			/* TOP BORDER FOR TEXT */
#define	TEXTBOT	(240)			/* BOTTOM BORDER FOR TEXT */

#define	STARTBUTTON		0
#define	OPTIONBUTTON	1

#define	SCREEN_DELAY	10

Uint16	teletypex;
Uint16	teletypey;
Uint16	teletypenumber;
Uint16	teletypepoint;
Uint16	teletypedelay;
Uint16	credittime;

#include "level.h"

TEXT	*leveldesc[]=
{
	leveltext1,
	leveltext2,
	leveltext3,
	leveltext4,
	leveltext5,
	leveltext6,
};

Uint8	desclen[]=
{
	10,
	10,
	10,
	10,
	5,
	5,
	5,
};

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Init_Teletype(void);
void	Print_Teletype(TEXT *data,Uint16 lines);
	
/******************************************************************/
/******************************************************************/
/******************************************************************/

void LoadScreen(Uint32	addr,Uint8 palno,Uint8	*fname,Uint8 flag)
{

	Uint8		*address	=	(Uint8 *)addr;

	//Read the bitmap image and copy into VDP2
	sprintf(stat_str,"bin/%s.bin",fname);
	ReadFile(stat_str,LowRam);

	if(flag==NORM)
	{
		CopyScreen(address);
	}
	else
	{
		CopyRotateScreen(address);
	}

	//Read and initialise the palette
	sprintf(stat_str,"bin/%s.sat",fname);
	ReadFile(stat_str,(Uint8 *)palette);
	InitPalette(palette,palno);

}

/******************************************************************/

void	PrintScaled(Sint16 x,Sint16 y,Uint8 *string)
{
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,0x0300,No_Window|No_Gouraud,CL256Bnk|ECdis|MESHoff,sprNoflip);
	FIXED			Pos[4];
	register		Uint8		chr;
	register		Uint16	count=0;
	register		Sint16	t=0;
	

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
		count	+=	(large[chr]);

		Pos[0]			=	t*65536;

		Normal.texno	=	(chr+BIGFONTSPRITE);
		slPutSprite(Pos,&Normal,0);

	}

}

/******************************************************************/

void	PrintTele(Uint16 x,Uint16 y,Uint8 string)
{
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,0x0300,No_Window|No_Gouraud,CL256Bnk|ECdis|MESHoff,sprNoflip);
	FIXED			Pos[4];
	register		Uint8			chr;
	register		Uint16		count=0;
	register		Uint16		t=0;
	
	Pos[1]	=	y*65536;
	Pos[2]	=	170*65536;
	Pos[3]	=	ORIGINAL*65536;
	
	x	-=	8;

	chr	=	string;
	chr	-=	32;

	t		=	x;
	t		+=	count;
	count	+=	med[chr];

	Pos[0]			=	t*65536;

	Normal.texno	=	(chr+MEDFONTSPRITE);

	slDispSprite(Pos,&Normal,0);

}

#if 0
/******************************************************************/

void	PrintLARGE(Uint16 x,Uint16 y,Uint8 *string)
{
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,0x0300,No_Window|No_Gouraud,CL256Bnk|ECdis|MESHoff,sprNoflip);
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
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,0x0300,No_Window|No_Gouraud,CL256Bnk|ECdis|MESHoff,sprNoflip);
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
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,0x0300,No_Window|No_Gouraud,CL256Bnk|ECdis|MESHoff,sprNoflip);
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
#endif

void	Control(void)
{


	while(1)
	{

		switch(Screen)
		{
			case	TITLE:
			{

				TitleScreen();
				break;
			}
		
			case	SELECT:
			{
				LevelSelect();
				break;
			}
		
			case	OPTION:
			{
				OptionScreen();
				break;
			}
		
			case	HANDLEOPTION:
			{
				HandleOptions();
				break;
			}
		
			case	CONTROL:
			{
				//ControlPad();
				break;
			}
		
			case	HISCORE:
			{
				DisplayHiscores();
				break;
			}
		
			case	CREDITSCREEN:
			{
				DisplayCredits();
				break;
			}
		
			case	ENTERNAME:
			{
				//EnterName();
				break;
			}
		
			case	BRIEF:
			{
				DisplayMissionBrief();
				break;
			}

			case	PLAYGAME:
			{
				SystemStatus=1;
				break;
			}

			default:
			{
				SystemStatus=0;
				break;
			}

		}

	}

}

/******************************************************************/

void	TitleScreen(void)
{

	Uint16	data;
	Uint8		flag=0;

	if(exitcode==0)
	{

		slTVOff();

		LoadScreen(VDP2_VRAM_A0,1,"360x",NORM);

		slIntFunction(TitleVBL);

		DOFADEUP;

		slTVOn();

		Pflag	=	FALSE;

		gamecycle=0;

		PlayCDAudio(1);

	}

	while(1)
	{

		pollhost();
		
		data	=	Smpc_Peripheral[0].pull;

		if(Pflag==TRUE)
		{
			PrintLARGE(240,210,"PRESS START BUTTON");
		}

		if((data & PER_DGT_ST)==0 )
		{
			Screen	=	OPTION;
			flag		=	1;
			gamecycle=0;
			break;
		}

		if(gamecycle>=(SCREEN_DELAY*50))
		{
			Screen	=	HISCORE;
			break;
		}

		slSynch();

	}

	if(flag==0)
	{
		DOFADEDOWN;
		WAITFADE;
	}

}

/******************************************************************/

void	OptionScreen(void)
{
	//BupConfig	conf[3];
	Uint16		data;
	Uint16		Option	=	0;

	//BackUpInit(conf);

	slIntFunction(TitleVBL);

	Pflag	=	FALSE;

	exitcode	=	0;

	//Wait for the START button to be released
	while(1)
	{

		data	=	Smpc_Peripheral[0].data;

		if((data & PER_DGT_ST)!=0) break;

		slSynch();

	}

	while(1)
	{

		pollhost();

		data	=	Smpc_Peripheral[0].push;

		if(Option	==	0)
		{
			if(Pflag==TRUE)
			{
				PrintLARGE(270,180,"BEGIN MISSION");
			}
		}
		else
		{
			PrintLARGE(270,180,"BEGIN MISSION");
		}

		if(Option	==	1)
		{
			if(Pflag==TRUE)
			{
				PrintLARGE(300,200,"OPTIONS");
			}
		}
		else
		{
			PrintLARGE(300,200,"OPTIONS");
		}


		if((data & PER_DGT_KU)==0 && (Option==1) )
		{
			Option	=	0;
		}

		if((data & PER_DGT_KD)==0 && (Option==0) )
		{
			Option	=	1;
		}
											  
		if((data & PER_DGT_TB)==0 )
		{
			Screen	=	TITLE;
			gamecycle=	0;
			exitcode	=	0xff;
		  	break;
		}

		//Begin mission?
		if( (data & PER_DGT_ST)==0 || (data & PER_DGT_TA)==0 )
		{
			if(Option==0)
			{
				Screen	=	BRIEF;
				exitcode=0x0;
				break;
			}
		}

		#if 0
		//Options?
		if( ((data & PER_DGT_ST)==0 || (data & PER_DGT_TA)==0) && (OldPadData!=data))
		{
			if(Option==0)
			{
				Screen	=	HANDLEOPTION;
				//HandleOptions();
				break;
			}
		}

		#endif

		//OldPadData=data;

		slSynch();
	}

	if(exitcode==0)
	{
		DOFADEDOWN;

		WAITFADE;
	}

}
				 
/******************************************************************/

void	HandleOptions(void)
{

	Uint16		data;
//	Uint16		Option	=	0;
	Uint8			i;

	//BackUpInit(conf);

	slIntFunction(TitleVBL);

	Pflag	=	FALSE;

	exitcode	=	0;


	while(1)
	{

		pollhost();

		data	=	Smpc_Peripheral[0].data;
		
		PrintLARGE(290,20,"360X OPTIONS");

		for(i=0;i<6;i++)
		{
			PrintLARGE(110,optext[i]+20,OpText[i]);
		}

		PrintLARGE(90,optext[0]+20,">");

		if( (data & PER_DGT_ST)==0 )
		{
			Screen	=	TITLE;
		 	break;
		}

		slSynch();

	}

}

/******************************************************************/

void	PlanetLevelSelect(void)
{
	Uint16		data;
	Uint8 		*poffset[6];
	Uint8			index=0;
	Uint8			*dst	=	BigBuffer;
	SpriteType	Sprite;
	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(Planet1Sprite,(CLUTSTART/8),No_Window|No_Gouraud,CL16Look|ECdis|MESHoff|CL_Replace|SPenb,sprNoflip);
	FIXED			Pos[4];
	Uint8			frame=0;

	slTVOff();
	

	Pos[0]	=	280*65536;
	Pos[1]	=	42*65536;
	Pos[2]	=	170*65536;
	Pos[3]	=	ORIGINAL*65536;



	//Read background screen
	ReadFile("bin/neb.bin",LowRam);
	CopyScreen((Uint8 *)VDP2_VRAM_A0);
	ReadFile("bin/neb.sat",(Uint8 *)palette);
	InitPalette(palette,1);

	fsize	=	0;
	
	poffset[index++]	=	dst;
	ReadFile("gfx/planet1.scs",dst);
	dst+=fsize;
	poffset[index++]	=	dst;
	ReadFile("gfx/planet2.scs",dst);
	dst+=fsize;
	poffset[index++]	=	dst;
	ReadFile("gfx/planet3.scs",dst);
	dst+=fsize;
	poffset[index++]	=	dst;
	ReadFile("gfx/planet4.scs",dst);
	dst+=fsize;
	poffset[index++]	=	dst;
	ReadFile("gfx/planet5.scs",dst);
	dst+=fsize;
	poffset[index++]	=	dst;
	ReadFile("gfx/planet6.scs",dst);
	dst+=fsize;


	slIntFunction(TitleVBL);

	Pflag	=	FALSE;

	DOFADEUP;

	slTVOn();

	TransferPlanetCluts(poffset[1],(Uint8 *)SpriteVRAM+CLUTSTART);

	while(1)
	{

		DbCol(8,0,0);

		pollhost();

		Sprite	=	DecodeGfx(frame,(Uint32 *)poffset[1],(Uint8 *)SpriteVRAM+Planet1Start,80,80);

		PrintLARGE(280,210,"SELECT MISSION");

		PrintMEDIUM(128,8,"ALEIUS");
		PrintMEDIUM(376,16,"NIXA");
		PrintMEDIUM(552,56,"PROLI");
		PrintMEDIUM(128,136,"BOZAI");
		PrintMEDIUM(400,88,"CUNAI");
		PrintMEDIUM(448,176,"SELIUS");

		data	=	Smpc_Peripheral[0].data;

		if((data & PER_DGT_ST)==0)
		{
			break;
		}

		slDispSprite(Pos,&Normal,0);

		DbCol(0,0,0);

		frame++;
		frame&=31;

		slSynch();

	}

	DbCol(0,0,0);

	DOFADEDOWN;

	WAITFADE;

	//DisplayMissionBrief();

	Screen	=	BRIEF;

}


/******************************************************************/

void DisplayMissionBrief(void)
{

	Uint16	data;
	Uint8		Bflag=FALSE;

	slTVOff();
	
	slPriorityNbg0(6);

	slPriorityNbg1(5);

	LoadScreen(VDP2_VRAM_A0,1,"grid",NORM);

	LoadScreen(VDP2_VRAM_B0,2,"lev1",NORM);

	slIntFunction(TitleVBL);

	Pflag	=	FALSE;

	DOFADEUP;

	slScrPosNbg1(toFIXED(-1),toFIXED(10));

	Init_Teletype();

	FONTCOL(0,20,0);
	 
	slTVOn();


	while(1)
	{

		pollhost();

		data	=	Smpc_Peripheral[0].push;

		Print_Teletype(leveldesc[Level],desclen[Level]);

		if((data & PER_DGT_ST)==0)
		{
			if(Bflag==TRUE)
			{
				break;
			}
			else
			{
				Bflag=TRUE;
				teletypenumber=desclen[Level];
			}
		}
	  
		if(teletypenumber==10)
		{
			Bflag=TRUE;
		}
		
		slSynch();

	}

	DOFADEDOWN;

	WAITFADE;

	Screen	=	PLAYGAME;

}
/******************************************************************/

void	TransferPlanetCluts(Uint8 *addr,Uint8 *tgt)
{
	Uint32	i	=	(28*34)+4;

	slDMACopy((addr+i),tgt,32);

}

/******************************************************************/

void	TitleVBL(void)
{
	flash++;
	
	if(flash==12)
	{
		Pflag	^=	TRUE;

		flash=0;
	}

	switch(FadeFlag)
	{
		case	FADEUP:
		{
			//r++;g++;b++;
			r+=20;g+=20;b+=20;
			slColOffsetA(r,g,b);

			break;
		}

		case	FADEDOWN:
		{
			r-=20;g-=20;b-=20;
			slColOffsetA(r,g,b);
			break;
		}

		case	NOFADE:
		{
			break;
		}
	}
	
	if(r==0 && g==0 && b==0)
	{
		FadeFlag=NOFADE;
	}

	if(r==-260 && g==-260 && b==-260)
	{
		FadeFlag=NOFADE;
	}

	gamecycle++;

}

/******************************************************************/

void	CopyRotateScreen(Uint8	*addr)
{
	register	Uint16	y;

	//Copy screen into VRAM
	for(y=0;y<512;y++)
	{
		slDMACopy((LowRam+(y*512)),(addr+(y*512)),512);
	}

}

/******************************************************************/

void	CopyScreen(Uint8	*addr)
{
	register	Uint16	y;

	//Copy screen into VRAM
	for(y=0;y<240;y++)
	{
		slDMACopy((LowRam+(y*640)),(addr+(y*1024)),640);
	}

}

/******************************************************************/

void	LevelSelect(void)
{

	Uint16	data;
	
	Level	=	1;

	slIntFunction(TitleVBL);

	Pflag	=	FALSE;

	DOFADEUP;


	while(1)
	{
		pollhost();
	
		PrintLARGE(220,40,"SELECT LEVEL TO PLAY");

		data	=	Smpc_Peripheral[0].push;

		if(Level==1)
		{
			if(Pflag==TRUE)
			{
				PrintLARGE(300,80,"LEVEL 1");
			}
		}
		else
		{
			PrintLARGE(300,80,"LEVEL 1");
		}
		if(Level==2)
		{
			if(Pflag==TRUE)
			{
				PrintLARGE(300,100,"LEVEL 2");
			}
		}
		else
		{
			PrintLARGE(300,100,"LEVEL 2");
		}
		if(Level==3)
		{
			if(Pflag==TRUE)
			{
				PrintLARGE(300,120,"LEVEL 3");
			}
		}
		else
		{
			PrintLARGE(300,120,"LEVEL 3");
		}
		if(Level==4)
		{
			if(Pflag==TRUE)
			{
				PrintLARGE(300,140,"LEVEL 4");
			}
		}
		else
		{
			PrintLARGE(300,140,"LEVEL 4");
		}
		if(Level==5)
		{
			if(Pflag==TRUE)
			{
				PrintLARGE(300,160,"LEVEL 5");
			}
		}
		else
		{
			PrintLARGE(300,160,"LEVEL 5");
		}
		if(Level==6)
		{
			if(Pflag==TRUE)
			{
				PrintLARGE(300,180,"LEVEL 6");
			}
		}
		else
		{
			PrintLARGE(300,180,"LEVEL 6");
		}


		if( (data  & PER_DGT_KU)==0)
		{
			if(Level > 1)
			{
				Level--;
				flash	=	0;
			}
		}
		if( (data  & PER_DGT_KD)==0)
		{
			if(Level < 6)
			{
				Level++;
				flash	=	0;
			}
		}

		if((data & PER_DGT_ST)==0)
		{
			break;
		}
	
		//OldPadData	=	data;

		slSynch();

	}

	Level--;

	DOFADEDOWN;

	WAITFADE;

	Screen	=	BRIEF;

}

/**************************************
*       INITIALISE THE TELETYPE       *
**************************************/


void	Init_Teletype(void)
{

	teletypenumber=0;
	teletypepoint=0;
	teletypedelay=0;

}

/**************************************
*     PRINT THE CREDITS ON SCREEN     *
**************************************/


void Print_Teletype(TEXT *data,Uint16 lines)
{				  
	
	Uint16	j;
	Uint8		*mess;
	Uint16	tx=0,ty=0;
	char		letter;

	if(teletypenumber!=0)
	{
		for (j=0;j<teletypenumber;j++)
		{
			if((data[j].control1)==22)
			{	
				tx	=	data[j].control2;
				ty	=	data[j].control3;

				if (data[j].fonttype==0)
				{
					PrintSMALL(tx,ty,data[j].datastuff);
				}
				if (data[j].fonttype==1)
				{
					PrintMEDIUM(tx,ty,data[j].datastuff);
				}
				if (data[j].fonttype==2)
				{
					PrintLARGE(tx,ty,data[j].datastuff);
		 		}
			}
		}
	}

	if(teletypenumber!=lines)
	{
		mess=data[teletypenumber].datastuff;
					  
		if((data[teletypenumber].control1)==22)
		{
			tx	=	data[teletypenumber].control2;
			ty	=	data[teletypenumber].control3;
		}

						   
		for (j=0;j<teletypepoint;++j)
		{
			letter=*(mess++);

			if (data[teletypenumber].fonttype==0)
			{
				PrintLARGE(tx,ty,&letter);
				tx+=large[((letter)-32)];
			}

			if (data[teletypenumber].fonttype==1)
			{
				PrintTele(tx,ty,letter);
				tx+=med[((letter)-32)];
			}

			if (data[teletypenumber].fonttype==2)
			{
				PrintSMALL(tx,ty,&letter);
				tx+=small[((letter)-32)];
			}
		}

		if (letter==0)
		{
			teletypepoint=0;
			if (teletypenumber!=lines)
			{	
				teletypenumber++;
			}
		}
		else
		{
			if ((gamecycle % 1)==0)
			{
				teletypepoint++;
			}
		}

	}

}

/******************************************************************/

void	DisplayCredits(void)
{

	Uint16		data;
	Uint16		j,tx,ty;
	

	slTVOff();

	ReadFile("bin/plasma1.sat",(Uint8 *)palette);
	InitPalette(palette,1);

	ReadFile("bin/plasma2.sat",(Uint8 *)palette);
	InitPalette(palette,2);
		
//	LoadScreen(VDP2_VRAM_A0,1,"neb",NORM);

//	InitRotateStarList();

//	while(ActiveStars < MAX_ACTIVE) 
//	{
//		AddRotateStar();
//	}

	slColorCalcOn(NBG0ON|NBG1ON);
	slColRateNbg0(25);
	slColRateNbg1(15);

	slIntFunction(TitleVBL);

	Pflag	=	FALSE;

	DOFADEUP;

	slTVOn();

	gamecycle=0;
	credittime	=	0;

//	while(1)
//	{
//		pollhost();
//		DoFixedPlasmaField1();
//		slSynch();
//	}


	while(1)
	{

		pollhost();

		data	=	Smpc_Peripheral[0].pull;

		if((data & PER_DGT_ST)==0)
		{
			DOFADEDOWN;
			WAITFADE;
			break;
		}

		if(credittime>=1000)
		{
	  		DOFADEDOWN;
	  		WAITFADE;
	  		break;
		}

		credittime+=2;
		     
		for (j=0;j<sizeof(credits)/sizeof(CREDITS);j++)
		{
			if(((256+(credits[j].y)-credittime)>TEXTTOP) && ((256+(credits[j].y)-credittime)<TEXTBOT))
			{

				//Print_At(((credits[j].x)+16),(256+(credits[j].y)-credittime));

				tx	=	((credits[j].x)+16);
				ty	=	(256+(credits[j].y)-credittime);

				if (credits[j].fonttype==0)
				{
					PrintMEDIUM(tx,ty,credits[j].datastuff);
				}
				else
				{
					PrintLARGE(tx,ty,credits[j].datastuff);
				}

			}
		}

		#if 0
		while(ActiveStars < MAX_ACTIVE) 
		{
			AddRotateStar();
		}

		PlotRotateStars();
		#endif

		DoFixedPlasmaField1();

		slSynch();

	}
	
	//EndStars();


	Screen	=	TITLE;

}

/******************************************************************/

void DisplayHiscores(void)
{

	Uint16		data;
	Uint16		ty,j;
	FIXED			sx=0,scale=-40,scale2=-60*65536,x=0,y=0;
	ANGLE			ang=0,ax=0,scz=DEGtoANG(120.0);
	Uint8			flag=FALSE;



	slTVOff();
	
	slInitMatrix();

	InitRotateTitleScreen();

	LoadScreen(VDP2_VRAM_A0,1,"neb2",ROTATE);

	InitStarList();

	while(ActiveStars < MAX_ACTIVE) 
	{
		AddStar();
	}


	slIntFunction(TitleVBL);

	Pflag	=	FALSE;

	DOFADEUP;

	slTVOn();

	gamecycle=0;

	exitcode	=	0;

	slDMACopy(Gouraud,(Uint8 *)SpriteVRAM+CLUTSTART,8);

	DbCol(0,0,0);

	slColorCalcOn(RBG0ON);
	slColRateRbg0(29);

	slGradationOn(grdRBG0);

	slWindow(0,0,639,239,0x7fff,320,120);
	

	while(1)
	{

		pollhost();
		

		data	=	Smpc_Peripheral[0].push;

		if((data & PER_DGT_ST)==0)
		{
			exitcode=1;
			break;
		}
	
		if(gamecycle>=SCREEN_DELAY*50)
		{
		  	break;
		}


		if(scale< (500<<16))
		{
			slPushMatrix();
			{
				scale	+=	(4<<16);
				slTranslate(0,0,scale);
				PrintScaled(-30,0,"TOP GUNS");
				//slRotZ(scale);
			}
			slPopMatrix();
		}


		slPushMatrix();
		{

			if( (scale2 < (120<<16)) && (flag==FALSE)   )
			{
				scale2	+=	(2<<16);
			}

			slTranslate(0,0,scale2);

			for (j=0;j<10;j++)
			{
				ty	=	56-3+(j*16);
				ty	-=120;
				PrintScaled(-96,ty,highest[j].highname);
				sprintf(stat_str,"%d",(int)highest[j].highscore);
				PrintScaled(420-320,ty,stat_str);
			}

		}
		slPopMatrix();


	 	while(ActiveStars < MAX_ACTIVE) 
	 	{
	 		AddStar();
	 	}
	 
	 	PlotStars();

		ax	+=10;
		ax	%=360;

		ang = ((DEGtoANG(ax)));
		slZrotR(ang);

		sx	=	DEGtoANG(ax);
		sx	=	slSin(sx);

		slZoomR(scz,scz);

		scz	-=	DEGtoANG(2.0);

		if(scz < (DEGtoANG(0.5)))
		{
			scz	=	DEGtoANG(120);
			x		=	((rand()%640)-320)<<16;
			y		=	((rand()%240)-120)<<16;
			slDispCenterR(x,y);
		}

		slSynch();

	}

	slColorCalcOn(!(RBG0ON|SPRON|NBG1ON));

	slColOffsetOn(RBG0ON|SPRON|NBG1ON);

	DOFADEDOWNCC;

	WAITFADE;

	EndStars();

	InitNormalTitleScreen();

	if(exitcode==1)
	{
		Screen	=	TITLE;
		LoadScreen(VDP2_VRAM_A0,1,"360x",NORM);
		exitcode	=	0;
	}
	else
	{
		Screen	=	CREDITSCREEN;
	}

}

/******************************************************************/

