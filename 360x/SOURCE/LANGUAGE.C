
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*           DECLARE DEFINES           *
**************************************/

/**************************************
*        SELECT YOUR LANGUAGES        *
**************************************/

void
Select_Language(void)
{

UBYTE	*title;
SPRITE*	spr_TITLE;

SHORT	dropout;

UBYTE	used_vram;

	Jalloc_Init(&JALLOC,sonymalloc,(maxmalloc*1024)); /* INITIALISE NEW MALLOC ROUTINE */

	Init_TPages();
	Reset_Cluts();
	Resident_Tims();

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

	dropout=60*20;

	Cls();

	while (fade!=0)
	{
		if (((Debounce1 & PaDenter)!=0) && (fade==128))	/* RESTORE: owner's decision, Start or Cross (PaDenter, 360X.H) */
		{
			Start_Fade_Down(-16);
			SsUtKeyOn(vab,SFX_EXPLOSION2,0,FXPITCH,0,sfxvolume,sfxvolume);
			language=selectno;
		}

		if ((dropout==0) && (fade==128))
		{
			Start_Fade_Down(-16);
			language=ENGLISH;
		}
		else dropout--;

		Start_Loop();

		Print_Sprite(spr_TITLE,320,240,64);

		Add_Star();
		Plot_Stars(16);

		Print_At(240,88);
		Print_Messl("SELECT LANGUAGE");

		Print_At(288,128);
		if (selectno==0)
		{
			if ((gamecycle MOD 32)<16) Print_Messl("ENGLISH");
		}
		else Print_Messl("ENGLISH");

		Print_At(288,144);
		if (selectno==1)
		{
			if ((gamecycle MOD 32)<16) Print_Messl("FRENCH");
		}
		else Print_Messl("FRENCH");

		Print_At(288,160);
		if (selectno==2)
		{
			if ((gamecycle MOD 32)<16) Print_Messl("GERMAN");
		}
		else Print_Messl("GERMAN");

		if (((Debounce1 & PaDLup)!=0) && (selectno!=0))
		{
			dropout=60*20;
			selectno--;
			SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
		}

		if (((Debounce1 & PaDLdown)!=0) && (selectno!=2))
		{
			dropout=60*20;
			selectno++;
			SsUtKeyOn(vab,SFX_CLICK,0,FXPITCH,0,sfxvolume,sfxvolume);
		}

		End_Loop();

	}

	VSync(0);
	SetDispMask(OFF);

}

