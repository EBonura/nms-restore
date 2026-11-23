
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*           DECLARE DEFINES           *
**************************************/

/**************************************
*         DECLARE STRUCTURES          *
**************************************/

/* RESTORE: HIGH is in HIGH.H, for LOADSAVE.C */

/**************************************
*        TEXT FOR HIGH SCORES         *
**************************************/

HIGH highest[]  = {500000,"MAXINE",0,
				400000,"PAUL",0,
				300000,"PRATIK",0,
				200000,"GLENN",0,
				100000,"RICH",0,
				 50000,"PHIL",0,
				 40000,"IAN",0,
				 30000,"COLIN",0,
				 10000,"DOUGAL",0,
				  7500,"ROWAN",0
				};

/**************************************
*        SHOW THE HIGH SCORES         *
**************************************/

void
High_Scores(void)
{

UBYTE	*title;
SPRITE*	spr_TITLE;

SHORT	dropout;

USHORT	j;

UBYTE	used_vram;

	Jalloc_Init(&JALLOC,sonymalloc,(maxmalloc*1024)); /* INITIALISE NEW MALLOC ROUTINE */

	Init_TPages();
	Reset_Cluts();
	Resident_Tims();
	Loading_Message();			/* RESTORE: owner's decision, PLEASE WAIT while this screen loads (GENERAL.C) */

	used_vram=Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);

	Xor_VRAM_Hole(((0+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((1+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((2+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((3+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((4+11)<<16)+(used_vram & 0x00ff0000),32,28);

	title=LoadFile("../INTRO/HIGH.PSX");
	spr_TITLE = (SPRITE*)LoadFile("../INTRO/HIGH.SPR");
	Init_Spr_List(spr_TITLE,(ULONG)title);

	Init_Star_List();

	selectno=0;

	dropout=60*5;

	Cls();

	while (fade!=0)
	{
		if (((Debounce1 & PaDstart)!=0) && (fade==128))
		{
			Start_Fade_Down(-16);
			SsUtKeyOn(vab,SFX_EXPLOSION2,0,FXPITCH,0,sfxvolume,sfxvolume);
			selectno=INTOTITLE;
		}

		if ((dropout==0) && (fade==128))
		{
			Start_Fade_Down(-16);
			selectno=INTOCREDITS;
		}
		else dropout--;

		Start_Loop();

		Print_At(256,24);
		Print_Messl("HIGH SCORES");

		for (j=0;j<10;j++)
		{
			Print_At(192,56-3+(j*16));
			Print_Messl(highest[j].highname);

			Print_Sprite(spr_FONTLARGE+16+((highest[j].highscore/1000000) MOD 10),384,56-3+(j*16),255);
			Print_Sprite(spr_FONTLARGE+16+((highest[j].highscore/100000) MOD 10),384+12,56-3+(j*16),255);
			Print_Sprite(spr_FONTLARGE+16+((highest[j].highscore/10000) MOD 10),384+24,56-3+(j*16),255);
			Print_Sprite(spr_FONTLARGE+16+((highest[j].highscore/1000) MOD 10),384+36,56-3+(j*16),255);
			Print_Sprite(spr_FONTLARGE+16+((highest[j].highscore/100) MOD 10),384+48,56-3+(j*16),255);
			Print_Sprite(spr_FONTLARGE+16+((highest[j].highscore/10) MOD 10),384+60,56-3+(j*16),255);
			Print_Sprite(spr_FONTLARGE+16+((highest[j].highscore/1) MOD 10),384+72,56-3+(j*16),255);
		}

		Print_Sprite(spr_TITLE,320,240,64);

		Add_Star();
		Plot_Stars(16);

		End_Loop();
	}

	VSync(0);
	SetDispMask(OFF);

}

/**************************************
*        CHECK FOR HIGH SCORE         *
**************************************/

void
Check_High_Score(void)
{

double	temp,tdx,tdy;

int		x,y;

SHORT	targetsize;
SHORT	targetx;
SHORT	targety;

SHORT	shipframe;

UBYTE	*s_ship;
SPRITE*	spr_S_SHIP;

UBYTE	*title;
SPRITE*	spr_TITLE;

UBYTE	used_vram;

USHORT	j,k,l;
USHORT	highno;

UBYTE	namechrs[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ!.&,";

UBYTE	spacename[]=" ";
UBYTE	nullname[]="-";

UBYTE	inputname[12]="------------";
UBYTE	nameposition;

	highno=255;
	for (j=0;j<10;j++)				/* CHECK ALL HIGH SCORES */
	{
		if (score > highest[j].highscore)
		{
			if (j==9)				/* IT WAS THE LOWEST HIGH */
			{
				highest[j].highscore=score;	/* INSERT THE SCORE */
				highno=9;			/* HIGH SCORE NUMBER 9 */
				break;
			}
			else
			{
				k=8;				/* SHUNT SCORES DOWN */
				for (;;)
				{
					highest[k+1].highscore=highest[k].highscore;
					for (l=0;l<12;l++)
					{
						highest[k+1].highname[l]=highest[k].highname[l];
					}
					if (k==j) break;
					else k--;
				}
				highest[j].highscore=score;	/* INSERT THE SCORE */

				highno=j;
				break;
			}
		}
	}

	if (highno==255) return;

	Jalloc_Init(&JALLOC,sonymalloc,(maxmalloc*1024)); /* INITIALISE NEW MALLOC ROUTINE */

	Init_TPages();
	Reset_Cluts();
	Resident_Tims();
	Loading_Message();			/* RESTORE: owner's decision, PLEASE WAIT while this screen loads (GENERAL.C) */

	used_vram=Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);

	Xor_VRAM_Hole(((0+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((1+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((2+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((3+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((4+11)<<16)+(used_vram & 0x00ff0000),32,28);

	title=LoadFile("../INTRO/HIGH.PSX");
	spr_TITLE = (SPRITE*)LoadFile("../INTRO/HIGH.SPR");
	Init_Spr_List(spr_TITLE,(ULONG)title);

	s_ship=LoadFile("../MISC/S_SHIP.PSX");
	spr_S_SHIP = (SPRITE*)LoadFile("../MISC/S_SHIP.SPR");
	Init_Spr_List(spr_S_SHIP,(ULONG)s_ship);
	MyFree((UBYTE*)s_ship);

	buck01=LoadFile("../MISC/BUCK01.PSX");
	spr_BUCK01 = (SPRITE*)LoadFile("../MISC/BUCK01.SPR");
	Init_Spr_List(spr_BUCK01,(ULONG)buck01);
	MyFree((UBYTE*)buck01);

	levelcol=MyMalloc(256*256);		/* SETUP DUMMY COLL MAP */
	memset(levelcol,0,256*256);

	Init_Particle_List();
	Init_Star_List();

	screenx=0;					/* NO SCROLL SO RESET THESE */
	screeny=0;
	selectno=0;
	nameposition=0;

	Cls();

	while (fade!=0)
	{
		if (((Debounce1 & PaDstart)!=0) && (fade==128))
		{
			Start_Fade_Down(-16);
			SsUtKeyOn(vab,SFX_EXPLOSION2,0,FXPITCH,0,sfxvolume,sfxvolume);
		}

		Start_Loop();

		Print_Sprite(spr_TITLE,320,240,32);

		Add_Star();
		Plot_Stars(24);

		if (selectno<30)
		{
			if ((selectno MOD 10)<5)
			{
				shipframe=(selectno MOD 10)+59;
			}
			else
			{
				shipframe=(selectno MOD 10)-4;
			}
		}
		else
		{
			if (selectno==30) shipframe=64-10;
			if (selectno==31) shipframe=0;
			if (selectno==32) shipframe=10;
		}

		sprctrl.scalex=8192;
		Print_Sprite(spr_S_SHIP+shipframe,320,200,255);
		sprctrl.scalex=4096;

		Draw_Particles();
		Move_Particles();

		for (j=0;j<3;j++)
		{
			for (k=0;k<10;k++)
			{
				if ((selectno==((j*10)+k)) && (selectno<30))
				{
					if ((gamecycle MOD 10)<5)
					{
						sprctrl.scalex=sprctrl.scaley=4096+512;
					}
				}

				l=namechrs[(j*10)+k];
				Print_Sprite(&spr_FONTMEDIUM[l-32],(k*32)+176,(j*24)+72,255);

				sprctrl.scalex=sprctrl.scaley=4096;
			}
		}

		if ((selectno==30) && ((gamecycle MOD 10)<5))
		{
			sprctrl.scalex=sprctrl.scaley=4096+512;
		}

		Print_At(176,144);
		Print_Messm("SPACE");
		sprctrl.scalex=sprctrl.scaley=4096;

		if ((selectno==31) && ((gamecycle MOD 10)<5))
		{
			sprctrl.scalex=sprctrl.scaley=4096+512;
		}

		Print_At(288,144);
		Print_Messm("DELETE");
		sprctrl.scalex=sprctrl.scaley=4096;

		if ((selectno==32) && ((gamecycle MOD 10)<5))
		{
			sprctrl.scalex=sprctrl.scaley=4096+512;
		}

		Print_At(440,144);
		Print_Messm("END");
		sprctrl.scalex=sprctrl.scaley=4096;

		Print_At(234,24);
		Print_Messl("ENTER YOUR NAME");
		Print_At(216,40);
		Print_Messl("INTO THE HALL OF FAME");

		if (selectno<30) targetsize=12;
		else targetsize=18;

		temp=(gamecycle MOD 32+1)*((pi*2)/32);
		tdx=cos(temp)*targetsize;
		tdy=sin(temp)*targetsize;

		if (selectno<30)
		{
			x=(180+((selectno MOD 10)*32))*65536;
			y=(86+((selectno/10)*24))*65536;
		}
		else y=160*65536;

		if (selectno==30) x=200*65536;
		if (selectno==31) x=316*65536;
		if (selectno==32) x=456*65536;

		x+=(tdx*65536);
		y+=(tdy*65536);

		x=x/65536;
		y=y/65536;

		Print_Sprite(spr_BUCK01+(gamecycle MOD 32),x,y,255);

	  	temp=((gamecycle+16) MOD 32+1)*((pi*2)/32);
		tdx=cos(temp)*targetsize;
		tdy=sin(temp)*targetsize;

		if (selectno<30)
		{
			x=(180+((selectno MOD 10)*32))*65536;
			y=(86+((selectno/10)*24))*65536;
		}
		else y=160*65536;

		if (selectno==30) x=200*65536;
		if (selectno==31) x=316*65536;
		if (selectno==32) x=456*65536;

		x+=(tdx*65536);
		y+=(tdy*65536);

		x=x/65536;
		y=y/65536;

		Print_Sprite(spr_BUCK01+(gamecycle MOD 32),x,y,255);

		for (j=0;j<12;j++)
		{
			l=inputname[j];
			Print_Sprite(&spr_FONTMEDIUM[l-32],240+(j*14),208,250);
		}

		if ((gamecycle MOD 16)<8)
			Print_Sprite(&spr_FONTMEDIUM[62],240+(nameposition*14),208,255);

		if (((Debounce1 & PaDLdown)!=0) && (selectno>=20))
		{
			if (selectno<23)
			{
				selectno=30;
			SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
			}

			if (selectno<27)
			{
				selectno=31;
			SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
			}

			if (selectno<30)
			{
				selectno=32;
			SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
			}
		}

		if (((Debounce1 & PaDLleft)!=0) && ((selectno MOD 10)!=0))
		{
			selectno--;
			SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
		}

		if (((Debounce1 & PaDLright)!=0) && ((selectno MOD 10)!=9))
		{
			if (selectno!=32) selectno++;
			SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
		}

		if (((Debounce1 & PaDLup)!=0) && (selectno>=10))
		{
			if (selectno>=30)
			{
				if (selectno==30) selectno=20;
				if (selectno==31) selectno=24;
				if (selectno==32) selectno=29;
			}
			else selectno-=10;
			SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
		}

		if (((Debounce1 & PaDLdown)!=0) && (selectno<20))
		{
			selectno+=10;
			SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
		}

		if ((Debounce1 & PaDRdown)!=0)
		{

			if (selectno<30)
			{
				x=(20+((selectno MOD 10)*32));
				y=(86+((selectno/10)*24));
			}
			else y=156;

			if (selectno==30) x=40;
			if (selectno==31) x=156;
			if (selectno==32) x=296;

			for (j=0;j<64;j++)
			{
				tdx=(rand()&31)-16;
				tdy=(rand()&31)-16;
				Add_Particle(x+tdx,y+tdy,(int) (tdx*16384),(int) (tdy*16384));
			}

			if (selectno<30)
			{
				inputname[nameposition]=namechrs[selectno];
				if (nameposition<11) nameposition++;
			}

			if (selectno==30)
			{
				inputname[nameposition]=spacename[0];
				if (nameposition<11) nameposition++;
			}

			if (selectno==31)
			{
				inputname[nameposition]=nullname[0];
				if (nameposition!=0) nameposition--;
			}

			if (selectno==32) break;
		}

		End_Loop();
	}

	VSync(0);
	SetDispMask(OFF);

	for (l=0;l<12;l++)
	{
		if (inputname[l]==nullname[0])
		{
			inputname[l]=spacename[0];
		}
	}

	for (l=0;l<12;l++)				/* COPY NAME INTO TABLE */
	{
		highest[highno].highname[l]=inputname[l];
	}

	High_Scores();

}

