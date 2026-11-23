#include	"globals.h"

// RESTORE: the game each of the option screen's ball racks starts (main).
// The owner's decision, not NMS's code: nothing in NMS's files reads
// BallType.  By the rack model each choice shows (COMMON.C ModelNames:
// "9ball", "triangle", "triangle", "trick"); both triangles load the same
// file (models/pmd/triangle.pmd) and nothing in the files tells them apart,
// so UK pool for the first and US 8-ball for the second is the owner's
// order too.  (The fourth model, "trick", is off the icon: the owner dropped
// TRICK_SHOT, see MENU.C DecodePad.)
static	long	ball_game_type[BALLCHOICES] = { US_POOL, UK_POOL, US_8BALL };

// RESTORE: bug fix (a regression of this restoration).  NMS's main starts
// every game as DEFAULT_GAME_TYPE (GLOBALS.H: UK_POOL), but BallType starts
// at 0, the 9-ball rack, and the line below overwrote that default with
// US_POOL.  The first game now starts on the rack that maps to UK_POOL
// (index 1, the first triangle), so the balls icon shows what is played.
#define	DEFAULT_BALL_TYPE	1

//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
#ifdef	PSX
#include	<psxsplash.h>	// RESTORE: PsxSplash, the boot splash (psyq/src/splash.rs)
#endif
void main(void)
{
	int	i;
	char  *screen;
	char	mess[100];


//	SwapSinTab();

#ifdef	PSX
	// RESTORE: owner's decision (2026-11-13: "Standard splash, new line"). The game opens with the standard
	// PSoXide studio splash, the line reading "Rebuilt with PSoXide", before its title picture. NMS's game
	// had none. It comes first, before grab_available_ram_for_us takes the whole heap and clears it: the
	// disc build keeps the splash in a scene image the front end's then replaces (qdup/scenes.txt), so
	// the splash costs the heap nothing, and nothing allocates before it returns. Cross, Circle or Start
	// skips it (port 1, read through the layer's own pad buffer, as InitSystem's InitPAD has not run).
	// The video mode is the one InitVideo sets later; InitVideo draws the screen after the splash.
	SetVideoMode(VIDEO_MODE);
	PsxSplash("Rebuilt with PSoXide");
#endif

	grab_available_ram_for_us();

	InitSystem();

	slTVOff();

	first_replay = NO;

	BallType = DEFAULT_BALL_TYPE;	// RESTORE: see DEFAULT_BALL_TYPE above

	InitFileSystem();

   PI=3.1415927;
   PI2=PI*2;
   deg2rad=PI/180;

	while(1)
		{

//g		init_cd_and_sound();	// initialise cd & sound subsystems


		FntLoad(960, 256);
		SetDumpFnt(FntOpen(16, 16, 512, 256, 0, 768));

		draw_title_screen();	// load & display title screen
								// or video sequence.

//g		read_pool_ini();		// this will try to read scores
								// & stuff from memory card.

		Ram_Load();

// RESTORE: was commented out (//		Enter_Main_Options();).  NMS's PS1 option
// screen, MENU.C's #ifdef PSX branch; the 1996 POOL.CPE links it (POOL.MAP
// 8001266C) and shows it when called.  Its choices (PlayType, BallType,
// TableType) are read nowhere in NMS's files, so they change nothing.
		Enter_Main_Options();

		Show_Loading();	// RESTORE: owner's decision, PLEASE WAIT while the game loads (PSX.C)
#ifdef	QDUP_TEST_CD_FAULT	// RESTORE: test builds only (tests/qdup-cd-retry.toml, qdup-cd-fail.toml): the next read waits, then fails this many tries
		CdTestFault(QDUP_TEST_CD_FAULT, QDUP_TEST_CD_DELAY);
#endif

		SetVideoRes(LO_RES);

		GAME_TYPE = DEFAULT_GAME_TYPE;

// RESTORE: the owner's decision, not NMS's code (nothing in NMS's files
// reads PlayType or sets PLAYERS_SELECTED).  The players icon shows the
// "head" model (PlayType 0) or the "computer" model (PlayType 1); the head
// is two players (PLAYERS_SELECTED 0, as main leaves it), the computer a
// game against the computer (PLAYERS_SELECTED 2: GAME.C gives player 2's
// turns to computer_shot when it is 2).  The balls icon picks the game
// (ball_game_type above).
		PLAYERS_SELECTED = PlayType ? 2 : 0;
		GAME_TYPE = ball_game_type[BallType % BALLCHOICES];

// RESTORE: TABLESIZE_TYPE (General_Rules[27]) was never assigned, so every
// game played on the US size table (0), though NMS's code handles the UK
// size (STV3D.C room index, FIXBALL.C ball radius, GAME.C
// setup_table_friction, GAMETYPE.C spot index) and the UK spot values
// (0.42, -0.65) only fit the UK table.  Owner's decision: UK pool is played
// on the UK size table (Potters, POTTERUK.ESH); the other game types stay on
// the US size table, as before.
		TABLESIZE_TYPE = (GAME_TYPE == UK_POOL) ? UK : US;
		LOCATION = POTTERS_CLUB;

// RESTORE: the table icon's four models are uktable, ustable, coffin and
// square (COMMON.C ModelNames).  The first two are the Potters table, its
// size following the game type (just above).  The coffin is the Crypt
// (LOCATION 8, below) and the square the Square (LOCATION 10).
// LOCATION is the index of the table in LEVELDAT.C's mesh_table, the one
// init_3d, init_cushions and the palette tables use (potters 0, rackers 1,
// hall 2, german 3, snooker 4, moon 5, ice 6, beach 7, crypt 8, swamp 9,
// toy 10).  NMS's code gives the Crypt two more numbers, which only decide:
// 7 (GAME.C setup_CD_track_for_fun_tables and the demo shot file names, the
// CD tune and demo files this build never plays or has; STV3D.C's low detail
// room set-up and setup_end_of_frame_camera, whose coffin_room() scenery and
// camera_positions_coffin therefore stay unused) and 10 (the ball-spot
// formula, DIFFERENT_SHAPED_TABLES_INDEX*3+(LOCATION-4): its index 18 is the
// Crypt's own entry in ball_spot_positions and whiteball_spot_positions, 0.92
// and -1.005).  The formula's index is not the loader's number: with LOCATION
// 8 it reads index 16, the ice rink's entry, and the Square (LOCATION 10)
// would read the Crypt's, outside its 1.34 m table.  The formula is fixed
// instead (GLOBALS.H SPOT_ROOM: the arrays give the snooker hall three
// entries, which it counted as one), so each table reads its own entry.
//
// TABLESIZE_TYPE (the ball size set and the table constants) per table comes
// from its data, as the Potters pair does.  The collision files give the
// playing area and the pocket mouth: Potters US 2.42 x 1.15 m and pocket
// radius 0.0822 (2.9 times the US ball radius), Potters UK 1.44 x 0.69 and
// 0.0384 (1.5 times the UK ball radius); Square 1.34 x 1.35 and 0.0781
// (2.7 times the US ball radius, 3.1 the UK's); Crypt 4.04 x 1.02 and
// 0.0675 (2.4 times the US ball radius, 2.7 the UK's).  The Square's mesh
// carries the US table's objects (corner1..6, endleft..endbottom) and its
// pocket against the ball matches the US table's within 5 percent, so it is
// played with the US size.  The Crypt fits neither size (a 4 m long, 1 m wide
// table; its mesh carries the UK table's side1..6 bumpers, its pockets are
// nearer the US ratio), so the choice is left open: until the owner picks one
// the table icon's Crypt plays the Potters table.  A test build
// (-DQDUP_CRYPT_SIZE=0 for US, 1 for UK) plays it for checking.
		switch(TableType)
			{
			case 3:
					LOCATION = 10;
					TABLESIZE_TYPE = US;
					break;
#ifdef	QDUP_CRYPT_SIZE
			case 2:
					LOCATION = 8;
					TABLESIZE_TYPE = QDUP_CRYPT_SIZE;
					break;
#endif
			}

// RESTORE: the baize icon sets the baize colour: TABLE_TYPE is the index of
// the colour group in LEVELDAT.C's room_palettes and poolpal_tab (0 blue,
// 1 green, 2 red).  (GLOBALS.H's enum RED_BAIZE, GREEN_BAIZE, BLUE_BAIZE
// numbers them the other way; the tables are what the loaders read.)
		TABLE_TYPE = BaizeType;

		copy_ball();

		Run_Game();

		}

}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	InitSystem(void)
{
#ifdef	PSX
	
	ResetCallback();

	ev0 = OpenEvent(SwCARD, EvSpIOE, EvMdNOINTR, NULL);					/* I/O ERROR */
	ev1 = OpenEvent(SwCARD, EvSpERROR, EvMdNOINTR, NULL);			/* ERROR */ 
	ev2 = OpenEvent(SwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);		/* TIME OUT */
	ev3 = OpenEvent(SwCARD, EvSpNEW, EvMdNOINTR, NULL);					/* NEW CARD */

	ev10 = OpenEvent(HwCARD, EvSpIOE, EvMdNOINTR, NULL);				/* I/O ERROR */
	ev11 = OpenEvent(HwCARD, EvSpERROR, EvMdNOINTR, NULL);		/* ERROR */
	ev12 = OpenEvent(HwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);	/* TIME OUT */
	ev13 = OpenEvent(HwCARD, EvSpNEW, EvMdNOINTR, NULL);				/* NEW CARD */

	currentcard = CARD1;

	InitPAD(&pad1,MAX_CONTROLLER_BYTES,&pad2,MAX_CONTROLLER_BYTES);
	StartPAD();

	InitCARD(1);					/* INITIALISE CARD WITH PAD */
	StartCARD();					/* START CARD WORKING */
	_bu_init();					/* INITIALISE FILE SYSTEM */

	ChangeClearPAD(0);

	EnableEvent(ev0);			/* ENABLE CARD EVENTS */
	EnableEvent(ev1);
	EnableEvent(ev2);
	EnableEvent(ev3);
	EnableEvent(ev10);
	EnableEvent(ev11);
	EnableEvent(ev12);
	EnableEvent(ev13);
	VblInit();
	InitVideo(MED_RES);

#else
	int	i;

	slInitSystem(TV_640x480,Texture,1);
	slWindow(0,0,640,480,65535,0,0);
	slColRAMMode(CRM16_2048);
	
	slBitMapNbg0(BM_1024x512,COL_TYPE_256,(void *)VDP2_VRAM_A0);
	slPrioritySpr0(7);
	slPriorityNbg0(6);	//Debug screen
	slPriorityNbg1(0);	//background screen
	slPriorityNbg2(0);
	slScrAutoDisp(NBG0ON|NBG1ON);
	slZdspLevel(3);
	slIntFunction(Vblank);
	slColRAMOffsetSpr(1);
//	slDMACopy(ballgfx,(Uint8 *)icon0,16*16);

#endif
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
#ifdef	PSX
void	VblInit( void )
{
	/* Turn of interrupts */
	EnterCriticalSection();

	/* Open the vbl event */
	Evbl = OpenEvent(RCntCNT3, EvSpINT, EvMdINTR, Vblank);

	/* And enable it */
	EnableEvent(Evbl);

	/* Start counting.... */
	#ifdef	PAL_256
	StartRCnt(RCntCNT3);
	#else
	StartRCnt(RCntCNT2);
	#endif

	/* Enable interrupts again */
	ExitCriticalSection();
}
#endif
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void SetupGame(void)
{

}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	Fade_up(void)
{
#ifdef	PSX
#else
	if (fade_type==1) return;
	sat_zoom=0;
	fade_type=2;
	while(1)
		{
		if (fade_type==1) break;
		slSynch();
		}
#endif
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	Fade_down(void)
{
#ifdef	PSX
#else
	if (fade_type==0) return;
	sat_zoom=255;
	fade_type=3;
	while(1)
		{
		if (fade_type==0) break;
		slSynch();
		}
#endif
}

//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	Vblank(void)
{
#ifdef	PSX
	frames++;
	pollhost();

#else
	pollhost();

	slDMACopy(PaletteBuffer,VDP2_COLRAM,512);

//// DO FADE ////

	*((Uint16 *) 0x5f80110)=fade_val;

	if (fade_type==0)	sat_temp=0x100;
	if (fade_type==1)	sat_temp=0x1ff;
	if (fade_type==2)
		{
		sat_temp=256+sat_zoom;
		sat_zoom+=10;
		if (sat_zoom==260) fade_type=1,sat_zoom=0;
		}

	if (fade_type==3)
		{
		sat_temp=256+sat_zoom;
		sat_zoom-=10;
		if (sat_zoom==5) sat_temp=256,fade_type=0,sat_zoom=0;
		}

	poke=(Uint16 *) 0x5f80114;	/*  red offset */ 
	*poke=sat_temp;
	poke=(Uint16 *) 0x5f80116;	/* green offset */
	*poke=sat_temp;	
	poke=(Uint16 *) 0x5f80118;	/* blue offset */
	*poke=sat_temp;


#endif
}
//лллллллллллллллпппппппппппппппппппппппппппппппппллллллллллллллл
//ллллллллллллллл This Is Where Crowes Code Goes. ллллллллллллллл
//лллллллллллллллмммммммммммммммммммммммммммммммммллллллллллллллл
//void	Swap_Player(void)
//{
//#define	MAX_PLAYERS_IN_GAME	2
//
//	bonus_pocket = rand();
//	bonus_pocket%=6;
//
//	// exit on these conditions
//
//	if	(GAME_TYPE == SPEED_POOL) return;
//
//	// do killer pool play turns
//
//	if	(GAME_TYPE == KILLER_POOL)	return;
//
//	if	(	PLAYERS_SELECTED== 2) return;
//
//	COMPUTER_TURN=NO;
//	old_player_turn=Player_Active;
//   old_turn_count=2;
//
//	Player_Active++;
//	Player_Active%=MAX_PLAYERS_IN_GAME;
//
//	show_panel=PANEL_ON_TIME;
//}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//void	pause_game( void )
//{
//
//}
