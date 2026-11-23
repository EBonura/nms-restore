
// 01902 351222
// DALTON ARRANGED IT
// BUDGET RENT A CAR

/**************************************
*        360X SONY PLAYSTATION        *
*     (C) NMS SOFTWARE LTD. 1996      *
*     CONVERTED BY PAUL GRIFFITHS     *
*          STARTED ON 12-03-96        *
**************************************/

/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"
#ifdef CDMASTER
#include <psxfmv.h>
#include <psxsplash.h>
#include "film.h"			/* RESTORE: written by tools/x360_film.py */
#endif

/**************************************
*       START OF THE MAIN CODE        *
**************************************/

void
main()
{

BYTE	temppad;					/* TEMP JOYPAD VARIABLE */

/**************************************
*      SET TO FINAL MASTER HERE       *
**************************************/

#ifdef CDMASTER					/* RESTORE: owner's decision (2026-11-13: "gate the debug printout"). THE DISC IS A FINAL MASTER: NMS'S OWN SWITCH TURNS OFF ITS "Loading File" LINES (FILES.C), THE VRAM VIEWER (L2, INTERUPT.C) AND POLLHOST. THE PC-LINK BUILDS STAY TEST RIGS */
	finalmaster=YES;
#else
	finalmaster=NO;				/* NO=TEST RIG, YES=FINAL MASTER */
#endif
#ifdef CDMASTER					/* RESTORE: 360x/build.sh cd builds the disc version */
	cdmaster=YES;
#else
	cdmaster=NO;					/* NO=HARD DRIVE, YES=CD */
#endif
	pal=YES;						/* NO=NTSC, YES=PAL */
	multilanguage=YES;				/* NO=ENGLISH, YES=FRENCH & GERMAN */

	maxmalloc=2048-729;				/* RESTORE: was 2048-384 (2048-725 until 2026-11-22: 4 KB went to the program, the pause screen's icons and the SDK of that night, docs/DECISIONS.md "the heap block"; Mission 1's peak still leaves 56 KB). Platform: the program takes 668 KB of the 2 MB (602 KB at first, 34 KB for the layer's film player and 49 KB for the July 1996 creature logic, less what the build left out), the stack keeps 16 KB (360x/build.sh), and on the disc this block also lies over the film player's 25 KB, which only the boot uses (psyq/psyq-boot.ld). 1328 KB can be had; Mission 1 on the disc uses 1309.5 KB at its peak */

	if (finalmaster==NO)			/* CHECK FOR FINAL MASTER */
	{
		showraster=NO;				/* 1=SHOW PROCESSING TIME */
		showvram=YES;				/* 1=SHOW VRAM IF NEEDED */
		testmessages=NO;			/* 1=TEST MESSAGES TO SCREEN */
		pollhost=YES;				/* 1=TURN POLLHOST ON FOR DEBUGGING */
	}

	if (finalmaster==YES)			/* CHECK FOR FINAL MASTER */
	{
		showraster=NO;				/* 1=SHOW PROCESSING TIME */
		showvram=NO;				/* 1=SHOW VRAM IF NEEDED */
		testmessages=NO;			/* 1=TEST MESSAGES TO SCREEN */
		pollhost=NO;				/* 1=TURN POLLHOST ON FOR DEBUGGING */
	}

/**************************************
*     CARRY ON WITH THE MAIN GAME     *
**************************************/

	ResetCallback();				/* RESET ALL OF THE CALLBACKS */
/*	Initialise_Malloc(); */		/* RESTORE: the second call below takes the memory again; on 2 MB there is room for one */
	InitialiseFiles();				/* INITIALISE FILE SUBSYSTEM */
	Init_Order_Table();				/* INITIALISE MAIN SPRITE TABLE */

	ev0=OpenEvent(SwCARD,EvSpIOE,EvMdNOINTR,NULL);	/* I/O ERROR */
	ev1=OpenEvent(SwCARD,EvSpERROR,EvMdNOINTR,NULL);	/* ERROR */
	ev2=OpenEvent(SwCARD,EvSpTIMOUT,EvMdNOINTR,NULL);	/* TIME OUT */
	ev3=OpenEvent(SwCARD,EvSpNEW,EvMdNOINTR,NULL);	/* NEW CARD */
	ev10=OpenEvent(HwCARD,EvSpIOE,EvMdNOINTR,NULL);	/* I/O ERROR */
	ev11=OpenEvent(HwCARD,EvSpERROR,EvMdNOINTR,NULL);	/* ERROR */
	ev12=OpenEvent(HwCARD,EvSpTIMOUT,EvMdNOINTR,NULL);/* TIME OUT */
	ev13=OpenEvent(HwCARD,EvSpNEW,EvMdNOINTR,NULL);	/* NEW CARD */

	InitPAD(&pad1,MAX_CONTROLLER_BYTES,&pad2,MAX_CONTROLLER_BYTES);
	StartPAD();

	InitCARD(1);					/* INITIALISE CARD WITH PAD */
	StartCARD();					/* START CARD WORKING */
	_bu_init();					/* INITIALISE FILE SYSTEM */

	ChangeClearPAD(0);

	ResetCallback();				/* RESET ALL OF THE CALLBACKS */
	Initialise_Malloc();			/* INITIALISE THE MALLOC ROUTINE */
#ifdef X360_TEST_CD_FAULT			/* RESTORE: TEST BUILDS ONLY (tests/x360-cd-fail.toml): THE NEXT READ WAITS, THEN FAILS THIS MANY TRIES */
	CdTestFault(X360_TEST_CD_FAULT,X360_TEST_CD_DELAY);
#endif
	InitialiseFiles();				/* INITIALISE FILE SUBSYSTEM */

	EnableEvent(ev0);				/* ENABLE CARD EVENTS */
	EnableEvent(ev1);
	EnableEvent(ev2);
	EnableEvent(ev3);
	EnableEvent(ev10);
	EnableEvent(ev11);
	EnableEvent(ev12);
	EnableEvent(ev13);

	if (pal==YES) SetVideoMode(MODE_PAL);	/* SET PAL MODE IF NEEDED */

	ResetGraph(0);					/* RESET GRAPHICS SUBSYSTEM (0:COLD, 1:WARM) */
	SetGraphDebug(0);				/* SET DEBUG MODE */
	SsInit();						/* INITIALISE SOUND */
	VSyncCallBack((ULONG)VblCallback);		/* SETUP VSYNC CALLBACK */

#ifdef CDMASTER
	/* RESTORE: owner's decision (2026-11-13: "Standard splash, new line"). THE DISC OPENS WITH THE STANDARD PSOXIDE STUDIO SPLASH, THE LINE READING "Rebuilt with PSoXide", BEFORE THE INTRO FILM. NMS'S GAME HAD NONE. THE PAD IS PORT 1 AS StartPAD ABOVE FILLS pad1; Cross, Circle OR Start SKIPS IT. IT RUNS FROM THE LAYER'S BOOT-ONLY MEMORY (psyq/psyq-boot.ld), LIKE THE FILM, SO THE HEAP KEEPS ALL OF ITS BLOCK. THE FILM AND Init_Screen640x240 SET THE SCREEN UP AGAIN AFTER IT */
	PsxSplash("Rebuilt with PSoXide");
#endif

/**************************************
*     SET INITIAL START VARIABLES     *
**************************************/

	PAD_FIRE=PaDRleft|PaDRright;		/* SETUP INITIAL FIRE BUTTON (RESTORE: was PaDRleft||PaDRright, which is 1) */
	PAD_THRUST=PaDRdown;			/* SETUP INITIAL THRUST BUTTON */
	PAD_GRAB=PaDRup;				/* SETUP INITIAL SMART BOMB */

	testsoundfx=0;					/* TEST SOUNDFX NUMBER */
	sfxvolume=127;					/* SET FX VOLUME TO MAXIMUM */
	musvolume=127;					/* SET MUSIC VOLUME TO MAXIMUM */
	channels=STEREO;				/* SET TO STEREO */

	panelposition=1;				/* POSITION FOR PANEL */
	panelx=STARTPANELX;				/* X COORDINATE FOR PANEL */
	panely=STARTPANELY;				/* Y COORDINATE FOR PANEL */

	Reset_Weapon_Ammo();			/* RESTORE: NMS'S AMOUNTS, NOW IN Reset_Weapon_Ammo (FIRE.C), ALSO GIVEN AT EVERY LIFE (LEVEL.C Init_Level) */

	controltype=0;					/* SET INITIAL CONTROL TYPE */
	difficulty=1;

	gamma[0]=128;					/* SET GAMMA CORRECTION FOR LEVELS */
	gamma[1]=128;
	gamma[2]=128;
	gamma[3]=128;
	gamma[4]=128;
	gamma[5]=128;
	gamma[6]=128;
	gamma[7]=128;
	gamma[8]=128;

	level=0;

#ifdef CDMASTER
	/* RESTORE: owner's decision. NMS's intro film SAT2 plays before the title,
	   where the Saturn's main has its call (20SEPT96.ZIP MAIN.C:173,
	   `//play_anim(0, "SAT2");`, in the boot loop before the game loads). The
	   PS1 stream player (STREAM.C) cannot decode the Saturn's SAT2.PAT (other
	   opcode table, mask bit order, 320 wide), so the film is NMS's SAT2.FLC
	   converted by tools/x360_film.py to the SDK's STR format and played by
	   the layer's FmvPlay (psyq/src/fmv.rs). Start or Cross skips it, as in
	   NMS's Tanx player (WEBPLAY.C: PaDRdown|PaDstart). It streams from the
	   disc, so only the disc build has it. */
	{
		BYTE	*filmmem;
		filmmem=MyMalloc(FmvMemoryBytes(FILM_WIDTH,FILM_HEIGHT,FILM_MAX_CHUNKS,FILM_RLE_WORDS));
		FmvPlay("SAT2.STR",FILM_FRAMES,FILM_WIDTH,FILM_HEIGHT,FILM_MAX_CHUNKS,FILM_RLE_WORDS,
			filmmem,&pad1,PaDstart|PaDRdown);
		MyFree(filmmem);
	}
#endif

	Init_Screen640x240();			/* SETUP THE SCREEN */

	Start_Menu_Sound();				/* RESTORE: owner's decision, MENU MUSIC FROM THE START (SOUND.C) */

	if (multilanguage==YES) Select_Language();	/* IF MULTI LANGUAGE THEN SELECT */

	for (;;)
	{

		Start_Menu_Sound();			/* RESTORE: owner's decision, AGAIN AFTER A GAME (MARCH: TOP OF THIS LOOP) */

		selectno=INTOTITLE;
		if (quittomenu==YES) selectno=INTOGAME;	/* RESTORE: owner's decision (GENERAL.C quittomenu): AFTER QUIT, STRAIGHT TO THE MAIN MENU (Game_Options) */
		quittomenu=NO;
		while(selectno!=INTOGAME)
		{
			if (selectno==INTOTITLE) Title_Screen();
			if (selectno==INTOSHIP)
			{
				Ship_Description();
				Start_Menu_Sound();	/* RESTORE: owner's decision, THE SHIP DEMO STARTED ITS OWN SOUND */
			}
			if (selectno==INTOHIGH) High_Scores();
			if (selectno==INTOCREDITS) Display_Credits();
		}

		Game_Options();
		Init_Newgame();
		Show_Planets();
		Warp_To_Planet();

		/* RESTORE: owner's decision. The menu music stops before the game,
		   NMS's March 1996 way (NMS.zip NMS/360WORK MAIN.C:109-111: soundfx and
		   music NO, InitialiseSound). March then freed the four buffers
		   (MAIN.C:113-116); here the screens since Start_Menu_Sound have
		   given that memory back already (Jalloc_Init), so there is nothing
		   left to free. */
		soundfx=NO;					/* DO YOU NEED VAB BUFFER? */
		music=NO;						/* DO YOU NEED SEQ BUFFER? */
		InitialiseSound();				/* STOP THE MUSIC */

		while (craftremaining!=0)
		{
			Load_Level();

			VAB_HADDR=LoadFile("../SOUND/OPTIONS.VH");
			VAB_BADDR=0;					/* RESTORE: platform, was VAB_BADDR=LoadFile("../SOUND/OPTIONS.VB"); */
			VAB_BNAME="../SOUND/OPTIONS.VB";	/* RESTORE: platform, InitialiseSound streams it */
			SEQ_ADDR=LoadFile("../SOUND/OPTPB.SEQ");
			soundfx=YES;					/* DO YOU NEED VAB BUFFER? */
			music=YES;					/* DO YOU NEED SEQ BUFFER? */
			InitialiseSound();				/* START THE MUSIC */
			/* RESTORE: completion. NMS's in-mission sound, commented out in
			   July 1996, runs again: the OPTIONS bank (the one bank holding
			   programs 102 to 111, the SFX_ numbers of SOUND.H) and OPTPB.SEQ
			   as the mission's music, started for ever by InitialiseSound.
			   Platform (2 MB): OPTIONS.VB (175,472 bytes) is not loaded whole.
			   Mission 1 leaves 63 KB of the heap free here, so its body goes
			   to sound RAM 16 KB at a time (Stream_Vab_Body, FILES.C). The
			   header and the song stay in the heap, as NMS had them. */

			Reset_Craft();				/* RESTORE: bug fix, THE CRAFT STARTS EVERY LIFE AT ITS START (CONTROL.C) */
			if (help_shown==NO)			/* RESTORE: owner's decision, THE CONTROL HELP ONCE, AT THE FIRST MISSION (GENERAL.C) */
			{
				help_cycles=250;
				help_shown=YES;
			}
			craft=add_sprite(NAME_CSHIP,ACTION_NONE,craftx,crafty);
			sprites[craft].priority=5;
			Add_Level_Pod();				/* RESTORE: NEW WORK, MISSION 6'S ONE POD (CONTROL.C; owner's decision, 2026-11-14) */
#ifdef X360_TEST_POD_X
			{ extern void Test_Add_Pod(void); Test_Add_Pod(); }	/* RESTORE: TEST BUILDS ONLY (CONTROL.C) */
#endif

			Move_Craft();

			/* RESTORE: bug fix. NMS LOADED THE FIRST SCREEN AT HALF THE CRAFT'S POSITION ((craftx/2)&0xffffffe0), THEN Update_Scroll
			   MOVED THE VIEW ONTO THE CRAFT IN ONE STEP; Do_Scroll_Download BRINGS IN ONE ROW AND ONE COLUMN OF BLOCKS A FRAME, SO
			   THE REST OF THE SCREEN KEPT THE MAP FROM WHERE IT WAS LOADED (MISSION 1: 288 PIXELS HIGHER) UNTIL THE CRAFT HAD FLOWN A
			   SCREEN, AND THE WALLS DRAWN WERE NOT THE ONES THE CRAFT HIT. THE FIRST SCREEN IS NOW LOADED WHERE Update_Scroll PUTS
			   THE VIEW: THE CRAFT AT ITS CENTRE (craft-160, craft-120), KEPT ON THE MAP (Update_Scroll's LIMITS). THE MAP'S SIZE IS READ FROM ITS
			   HEADER, AS Sort_Map DOES: mapwidth AND mapheight ARE SET THERE, AFTER THIS. */
			startscreenx=craftx-160;
			if (startscreenx>(((UBYTE)levelmap[0])*16-320)) startscreenx=((UBYTE)levelmap[0])*16-320;
			if (startscreenx<0) startscreenx=0;

			startscreeny=crafty-120;
			if (startscreeny>(((UBYTE)levelmap[2])*16-240)) startscreeny=((UBYTE)levelmap[2])*16-240;
			if (startscreeny<0) startscreeny=0;

			screenx=startscreenx;
			screeny=startscreeny;
			scrollxoff=0;
			scrollyoff=0;

			Sort_Map();
			scrollxoff=320;
			scrollyoff=256;

			Init_Level();

			Cls();

			while (fade!=0)
			{

				Check_Pause_Menu();

				Start_Loop();

				if (pause==ON) Draw_Pause_Screen(YES);	/* RESTORE: NEW WORK (owner's decision 2026-11-22), SORTED BEFORE THE GAME SO IT IS DRAWN OVER IT, THE GAME DIMMED (GENERAL.C) */
				else if (help_cycles>0) Draw_Pause_Screen(NO);	/* RESTORE: owner's decision, THE CONTROL HELP AT THE FIRST MISSION UNTIL A BUTTON OR 250 CYCLES, DRAWN AS THE PAUSE SCREEN (2026-11-22) */

				Get_Maptable_Data();
				Do_Scroll_Download();
				Print_Panel();
				Print_The_Map();
				Draw_Glow_Lines();		/* RESTORE: THE LASER'S BEAM, OVER THE MAP, UNDER THE SPRITES (GENERAL.C) */
				Draw_Pod_Rope();		/* RESTORE: THE POD'S ROPE, WHEN A POD IS IN HAND (GENERAL.C) */

				Print_Sprites();
				Draw_Particles();

				if ((pause==OFF) && (help_cycles>0) && (whichmenu==0))	/* RESTORE: THE MISSION START'S HELP (Draw_Pause_Screen, ABOVE) COUNTS DOWN, AND A BUTTON CLOSES IT */
				{
					help_cycles--;
					if (Debounce1!=0) help_cycles=0;
				}
				if (survivor_message>0)	/* RESTORE: owner's standing rule: NO NMS VERSION RESCUES A SURVIVOR (CONTROL.C Move_Craft) */
				{
					Print_Messm_Centred(160,156,"RESCUE NOT AVAILABLE");
					if (pause==OFF) survivor_message--;
				}

				if ((whichmenu==0) && (pause==OFF))
				{
					Process_Sprites();
					Move_Craft();
					Move_Particles();
#ifdef X360_TEST_FIRECOST
					{	/* RESTORE: TEST BUILDS ONLY (FIRE.C Test_Fill_Shots) */
						extern void Test_Fill_Shots(void);
						extern LONG testfiret[4];
						USHORT	t0,t1;
						if (gamecycle==100) Test_Fill_Shots();
						t0=*(volatile USHORT *) 0x1F801120;
						Move_Fire();
						t1=*(volatile USHORT *) 0x1F801120;
						if (gamecycle>120)
						{
							t1=(USHORT)(t1-t0);
							testfiret[0]++;
							testfiret[1]+=t1;
							if (t1>testfiret[2]) testfiret[2]=t1;
						}
					}
#else
					Move_Fire();
#endif
					process_collisions();		/* RESTORE: NEW WORK (owner's decision): NMS'S, REVIVED */
					Check_Level_Completed();	/* RESTORE: NEW WORK */
					Update_Scroll(craftx,crafty);

					temppad=GetType(&pad1);	/* CHECK FOR NO CONTROLLERS */
					if (temppad==STD_PAD)
					{
						if (GoodData(&pad1)==FALSE) pause=YES;
					}

					if ((gamecycle MOD 1000)==0) scoreadder+=250;
				}
				else
				{
					Text_Box_Begin();	/* RESTORE: NEW WORK, THE MENU IN A BOX (GENERAL.C) */
					Process_Menus();
					Text_Box_End();
				}

				End_Loop();

				if ((craftdestroyed==YES) && (deadtimer==0)) break;
				if (levelcompleted==YES) break;
				if (changelevel==YES) break;	/* RESTORE: A TELEPORTER (PROCESS.C COMMAND_SET_LEVEL) */
			}

			VSync(0);
			SetDispMask(OFF);

			/* RESTORE: owner's decision. The mission's music stops when the
			   mission ends, NMS's March 1996 way (NMS.zip NMS/360WORK MAIN.C:
			   109-111: soundfx and music NO, InitialiseSound; 113-116: the
			   buffers freed). The bank's body was never loaded (VAB_BADDR 0,
			   sent in pieces) and July has one song, so the header and the
			   song are what is freed. */
			soundfx=NO;					/* DO YOU NEED VAB BUFFER? */
			music=NO;					/* DO YOU NEED SEQ BUFFER? */
			InitialiseSound();			/* STOP THE MUSIC */
			MyFree(VAB_HADDR);
			MyFree(SEQ_ADDR);

			Init_Screen640x240();		/* SETUP THE SCREEN */

			if (changelevel==YES)
			{
				/* RESTORE: A TELEPORTER SENT THE CRAFT TO ANOTHER MISSION (level IS SET): NO DEBRIEFING, THE LOOP LOADS IT */
				level=newlevel;
			}
			else if (craftdestroyed==NO)
			{
				Exit_Level();
			}
			else
			{
				if (quittomenu==YES) craftremaining=1;	/* RESTORE: owner's decision: QUIT ENDS THE GAME, NO HIGH SCORE ENTRY, BACK TO THE MAIN MENU (GENERAL.C quittomenu) */
				craftremaining--;
				if (quittomenu==YES) continue;
				if (craftremaining==0) Check_High_Score();
			}
		}
	}
}

