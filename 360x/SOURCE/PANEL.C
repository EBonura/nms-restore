
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE DEFINES            *
**************************************/

/**************************************
*         DECLARE VARIABLES           *
**************************************/

BYTE	shieldred[]={	  0, 16, 32, 48, 64, 80, 96, 96,
					 96, 96, 96, 96, 96,144,192,240};

BYTE	shieldgreen[]={ 96, 96, 96, 96, 96, 96, 96, 80,
					 64, 48, 32, 16,  0,  0,  0,  0 };

BYTE	shieldblue[]={	  0,  0,  0,  0,  0,  0,  0,  0,
					  0,  0,  0,  0,  0,  0,  0,  0};

/**************************************
*          PRINT THE PANEL            *
**************************************/

void
Print_Panel(void)
{

USHORT	j;
double	temp,tdx,tdy;

	Print_Sprite(spr_BORDER1,panelx,panely,255);

	if ((fuel<(4*FUELMULT)) && (fuel>0) && ((gamecycle MOD 16)<8))
	{
		sprctrl.red=64;
		sprctrl.green=64;
		sprctrl.blue=64;
	}

	Print_Sprite(spr_BARSL+12-(fuel/FUELMULT),panelx-16,panely-38,255);

	sprctrl.red=128;
	sprctrl.green=128;
	sprctrl.blue=128;

	Print_Sprite(spr_BARSR+(12-freeweaponslots),panelx+16,panely-38,255);

	sprctrl.red=shieldred[15-(shields/8)];
	sprctrl.green=shieldgreen[15-(shields/8)];
	sprctrl.blue=shieldblue[15-(shields/8)];

	Print_Sprite(spr_SHIPWIRE+((gamecycle MOD 128)/2),panelx+3,panely-44,255);

	sprctrl.red=0;
	sprctrl.green=96;
	sprctrl.blue=0;

	Print_Sprite(spr_FONTSMALL+16+((score/1000000) MOD 10),panelx-26,panely-19,255);
	Print_Sprite(spr_FONTSMALL+16+((score/100000) MOD 10),panelx-19,panely-19,255);
	Print_Sprite(spr_FONTSMALL+16+((score/10000) MOD 10),panelx-12,panely-19,255);
	Print_Sprite(spr_FONTSMALL+16+((score/1000) MOD 10),panelx-5,panely-19,255);
	Print_Sprite(spr_FONTSMALL+16+((score/100) MOD 10),panelx+2,panely-19,255);
	Print_Sprite(spr_FONTSMALL+16+((score/10) MOD 10),panelx+9,panely-19,255);
	Print_Sprite(spr_FONTSMALL+16+((score/1) MOD 10),panelx+16,panely-19,255);

	if (weapontype==0) Print_Sprite(spr_GUIDWIRE+(gamecycle MOD 32),panelx,panely-22,253);
	if (weapontype==1) Print_Sprite(spr_LASWIRE+(gamecycle MOD 32),panelx-2,panely-19,253);
	if (weapontype==2) Print_Sprite(spr_BCKWIRE+(gamecycle MOD 32),panelx-3,panely-19,253);
	if (weapontype==3) Print_Sprite(spr_EBWIRE+(gamecycle MOD 32),panelx,panely-20,253);
	if (weapontype==4) Print_Sprite(spr_AGMWIRE+(gamecycle MOD 32),panelx,panely-19,253);
	if (weapontype==5) Print_Sprite(spr_MIN1WIRE+(gamecycle MOD 32),panelx,panely-19,253);
	if (weapontype==6) Print_Sprite(spr_GUIDWIRE+(gamecycle MOD 32),panelx,panely-22,253);
	if (weapontype==7) Print_Sprite(spr_GUIDWIRE+(gamecycle MOD 32),panelx,panely-22,253);
	if (weapontype==8) Print_Sprite(spr_BCKWIRE+(gamecycle MOD 32),panelx-3,panely-19,253);

	Print_Sprite(spr_FONTSMALL+16+((weaponammo[weapontype]/10) MOD 10),panelx-25,panely-36,255);
	Print_Sprite(spr_FONTSMALL+16+((weaponammo[weapontype]/1) MOD 10),panelx-19,panely-36,255);

	if (((Debounce1 & PaDlu)!=0) && (weapontype!=0) && (pause==OFF) && (whichmenu==0))	/* RESTORE: bug fix (2026-11-21 audit): NOT WHILE PAUSED OR IN THE MENU (Print_Panel RUNS EVERY LOOP, MAIN.C) */
	{
		sprctrl.red=128;
		sprctrl.green=128;
		sprctrl.blue=128;
		weapontype--;
	}

	Print_Sprite(spr_FONTSMALL+59,panelx+11,panely-36,255);

	sprctrl.red=0;
	sprctrl.green=96;
	sprctrl.blue=0;

	if (((Debounce1 & PaDru)!=0) && (weapontype!=8) && (pause==OFF) && (whichmenu==0))	/* RESTORE: bug fix, AS ABOVE */
	{
		sprctrl.red=128;
		sprctrl.green=128;
		sprctrl.blue=128;
		weapontype++;
	}

	Print_Sprite(spr_FONTSMALL+60,panelx+17,panely-36,255);

	sprctrl.red=128;
	sprctrl.green=128;
	sprctrl.blue=128;

	/* RESTORE: owner's standing rule (debug features are off on disc builds; 2026-11-18). HOLDING L1 DRAINS THE SHIELDS AND R1
	   REFILLS THEM: NMS'S TEST KEYS ON THE WEAPON BUTTONS. THEY RUN ONLY ON A TEST RIG NOW (finalmaster NO, MAIN.C), AS NMS'S OTHER
	   DEBUG AIDS DO (FILES.C, MAIN.C showvram); THE DISC BUILD IS A FINAL MASTER. WEAPON SELECT ON L1 AND R1 (ABOVE) IS UNCHANGED. */
	if (finalmaster==NO)
	{
	if ((Pad1 & PaDlu)!=0)
	{
		if (shields!=0) shields--;
	}

	if ((Pad1 & PaDru)!=0)
	{
		if (shields!=127) shields++;
	}
	}

#ifdef X360_TEST_DIE_AT
	{	/* RESTORE: TEST BUILDS ONLY (X360_CFLAGS=-DX360_TEST_DIE_AT=c): THE FIRST LIFE'S SHIELDS GO TO 0 AT GAME CYCLE c, SO IT ENDS AS A HIT ENDS IT (BELOW) */
		static int test_died=0;
		if ((test_died==0) && (gamecycle==X360_TEST_DIE_AT)) { shields=0; test_died=1; }
	}
#endif
	if ((shields==0) && (craftdestroyed==NO))	/* CRAFT DESTROYED */
	{
		craftdestroyed=YES;
		deadtimer=120;
		SsUtKeyOn(vab,SFX_EXPLOSION1,0,FXPITCH,0,sfxvolume,sfxvolume);	/* RESTORE: NEW WORK (owner's decision 2026-11-21): SOUND.H's SFX_EXPLOSION1, NAMED BY NMS AND PLAYED BY NOTHING, WHEN THE CRAFT IS DESTROYED */
		sprites[craft].visible=FALSE;

		for (j=0;j<256;j++)
		{
			tdx=(rand()&63)-32;
			tdy=(rand()&63)-32;
			Add_Particle(craftx,crafty,(int) (tdx*4096),(int) (tdy*4096));
		}
	}

	if ((pause==ON) || (whichmenu!=0)) return;	/* RESTORE: bug fix (2026-11-21 audit): THE DEATH COUNTDOWN AND THE SCORE COUNTER WAIT WHILE PAUSED OR IN THE MENU; A DEATH'S COUNTDOWN ENDED THE MISSION UNDER AN OPEN PAUSE MENU */
	if (deadtimer!=0) deadtimer--;

	if (scoreadder!=0)				/* CHECK FOR ADDING SCORE */
	{
		scoreadder--;
		score++;
	}
}

