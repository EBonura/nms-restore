
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE ROUTINES           *
**************************************/

/**************************************
*          DECLARE ROUTINES           *
**************************************/

/**************************************
*          DECLARE STRUCTURES         *
**************************************/

/**************************************
*         DECLARE VARIABLES           *
**************************************/

TEXT leveltext1[]={22, 40, 3*7+3,-1,1,"MISSION 1 : THE LUNAR MIRE",0,
				22, 40, 7*7+3,-1,1,"NUMBER OF SURVIVORS : UNKNOWN",0,
				22, 40, 9*7+3,-1,1,"NUMBER OF PODS : 1",0,
				22, 40,11*7+3,-1,1,"TIME LIMIT : 5 MINUTES",0,
				22, 40,13*7+3,-1,1,"ALIEN INTELLIGENCE : MINIMAL",0,
				22, 40,16*7+3,-1,1,"MISSION INFORMATION :",0,
				22, 40,19*7+3,-1,1,"RESCUE ALL SURVIVORS FROM LUNAR SURFACE. DESTROY ALL MAIN",0,
				22, 40,21*7+3,-1,1,"ENEMY RESISTANCE AND RETURN TO BASE. A MINIMAL NUMBER OF",0,
				22, 40,23*7+3,-1,1,"RESCUED SURVIVORS ARE NEEDED TO COMPLETE THE MISSION.",0,
				22, 40,26*7+3,-1,1,"GOOD LUCK!",0
				};

TEXT leveltext2[]={22, 40, 3*7+3,-1,1,"MISSION 2 : AJAX WEAPON RESEARCH ASTEROID",0,
				22, 40, 7*7+3,-1,1,"NUMBER OF SURVIVORS : 16",0,
				22, 40, 9*7+3,-1,1,"NUMBER OF PODS : 1",0,
				22, 40,11*7+3,-1,1,"TIME LIMIT : 10 MINUTES",0,
				22, 40,13*7+3,-1,1,"ALIEN INTELLIGENCE : LOW",0,
				22, 40,16*7+3,-1,1,"MISSION INFORMATION :",0,
				22, 40,19*7+3,-1,1,"RESCUE ALL SURVIVING SCIENTISTS FROM EXPERIMENTAL RESEARCH",0,
				22, 40,21*7+3,-1,1,"AREA. REMOVE ENEMY INSTALLATIONS FROM ALL VITAL AREAS. DESTROY",0,
				22, 40,23*7+3,-1,1,"ALL FLYING DEBRIS FROM FAILED EXPIREMENT.",0,
				22, 40,26*7+3,-1,1,"GOOD LUCK!",0
				};

TEXT leveltext3[]={22, 40, 3*7+3,-1,1,"MISSION 3 : ALIEN ASTEROID",0,
				22, 40, 7*7+3,-1,1,"NUMBER OF SURVIVORS : 4",0,
				22, 40, 9*7+3,-1,1,"NUMBER OF PODS : 1",0,
				22, 40,11*7+3,-1,1,"TIME LIMIT : 5 MINUTES",0,
				22, 40,13*7+3,-1,1,"ALIEN INTELLIGENCE : MEDIUM",0,
				22, 40,16*7+3,-1,1,"MISSION INFORMATION :",0,
				22, 40,19*7+3,-1,1,"RESCUE ASTRONAUTS FROM ASTEROID SURFACE. DESTROY DEBRIS",0,
				22, 40,21*7+3,-1,1,"FROM THEIR CRAFT TO PREVENT ALIEN RESEARCH. TAKE OUT THE",0,
				22, 40,23*7+3,-1,1,"ALIEN CONTROL CENTRE IN THE HEART OF THE ASTEROID.",0,
				22, 40,26*7+3,-1,1,"GOOD LUCK!",0
				};

TEXT leveltext4[]={22, 40, 3*7+3,-1,1,"MISSION 4 : NEW WORLD",0,
				22, 40, 7*7+3,-1,1,"NUMBER OF COLONISTS : 12",0,
				22, 40, 9*7+3,-1,1,"NUMBER OF PODS : 1",0,
				22, 40,11*7+3,-1,1,"TIME LIMIT : 15 MINUTES",0,
				22, 40,13*7+3,-1,1,"ALIEN INTELLIGENCE : VARIOUS",0,
				22, 40,16*7+3,-1,1,"MISSION INFORMATION :",0,
				22, 40,19*7+3,-1,1,"PICK UP COLONISTS FROM SURFACE WITH SAMPLE PLANET TISSUE.",0,
				22, 40,21*7+3,-1,1,"FIND ALL HOSTILE LIFE FORMS AND REMOVE THEM FROM THE",0,
				22, 40,23*7+3,-1,1,"PLANETS SURFACE READY FOR FULL COLONISATION.",0,
				22, 40,26*7+3,-1,1,"GOOD LUCK!",0
				};

TEXT leveltext5[]={22, 40, 3*7+3,-1,1,"MISSION 5 : CUNAI ALIEN PLANETOID",0,
				22, 40, 7*7+3,-1,1,"NUMBER OF SURVIVORS : 20",0,
				22, 40, 9*7+3,-1,1,"NUMBER OF PODS : 2",0,
				22, 40,11*7+3,-1,1,"TIME LIMIT : 9 MINUTES",0,
				22, 40,13*7+3,-1,1,"ALIEN INTELLIGENCE : HIGH",0,
				22, 40,16*7+3,-1,1,"MISSION INFORMATION :",0,
				22, 40,19*7+3,-1,1,"ENTER PLANETOID AND RESCUE FAILED ASSAULT TEAM",0,
				22, 40,21*7+3,-1,1,"BEFORE ENEMY ERADICATES THEM. DESTROY ALL",0,
				22, 40,23*7+3,-1,1,"HOSTILE ENEMIES BEFORE LEAVING THE AREA.",0,
				22, 40,26*7+3,-1,1,"GOOD LUCK!",0
				};

TEXT leveltext6[]={22, 40, 3*7+3,-1,1,"MISSION 6 : ALIEN COMMAND BASE",0,
				22, 40, 7*7+3,-1,1,"NUMBER OF SURVIVORS : 5",0,
				22, 40, 9*7+3,-1,1,"NUMBER OF PODS : 1",0,
				22, 40,11*7+3,-1,1,"TIME LIMIT : 12 MINUTES",0,
				22, 40,13*7+3,-1,1,"ALIEN INTELLIGENCE : VERY HIGH",0,
				22, 40,16*7+3,-1,1,"MISSION INFORMATION :",0,
				22, 40,19*7+3,-1,1,"FIND AND DESTROY ALIEN HEAD OF OPERATIONS. PICK",0,
				22, 40,21*7+3,-1,1,"UP LEAD PARTY FROM HIDDEN ENCAMPMENTS ON THE",0,
				22, 40,23*7+3,-1,1,"SURFACE. WREAK HAVOC ON THE ALIEN SCUM!",0,
				22, 40,26*7+3,-1,1,"GOOD LUCK!",0
				};

TEXT leveltext7[]={22, 40, 3*7+3,-1,1,"BONUS MISSION 1 : ASTEROID FIELD",0,
				22, 40, 7*7+3,-1,1,"NUMBER OF SURVIVORS : N/A",0,
				22, 40, 9*7+3,-1,1,"NUMBER OF PODS : N/A",0,
				22, 40,11*7+3,-1,1,"TIME LIMIT : 5 MINUTES",0,
				22, 40,13*7+3,-1,1,"ALIEN INTELLIGENCE : MINIMAL",0,
				22, 40,16*7+3,-1,1,"MISSION INFORMATION :",0,
				22, 40,19*7+3,-1,1,"DESTROY ALL ASTEROIDS TO ENABLE YOUR FLEET TO",0,
				22, 40,21*7+3,-1,1,"CONTINUE TO NEXT AREA. FIND AND DESTROY ANY",0,
				22, 40,23*7+3,-1,1,"HIDDEN ENEMY ENCAMPMENTS IN THE AREA.",0,
				22, 40,26*7+3,-1,1,"GOOD LUCK!",0
				};

TEXT leveltext8[]={22, 40, 3*7+3,-1,1,"BONUS MISSION 2 : THE FISH POND",0,
				22, 40, 7*7+3,-1,1,"NUMBER OF SURVIVORS : N/A",0,
				22, 40, 9*7+3,-1,1,"NUMBER OF PODS : N/A",0,
				22, 40,11*7+3,-1,1,"TIME LIMIT : 4 MINUTES",0,
				22, 40,13*7+3,-1,1,"ALIEN INTELLIGENCE : SWARMING",0,
				22, 40,16*7+3,-1,1,"MISSION INFORMATION :",0,
				22, 40,19*7+3,-1,1,"THE MOTHERSHIP HAS RUN LOW ON FOOD SUPPLIES.",0,
				22, 40,21*7+3,-1,1,"SHOOT THE SWARMING FISH LIKE CREATURES SO",0,
				22, 40,23*7+3,-1,1,"COLLECTION CAN BE MADE BY THE MOTHERSHIP.",0,
				22, 40,26*7+3,-1,1,"HAPPY HUNTING!",0
				};

TEXT leveltext9[]={22, 40, 3*7+3,-1,1,"BONUS MISSION 3 : THE WIND TUNNEL",0,
				22, 40, 7*7+3,-1,1,"NUMBER OF SURVIVORS : 1",0,
				22, 40, 9*7+3,-1,1,"NUMBER OF PODS : 2",0,
				22, 40,11*7+3,-1,1,"TIME LIMIT : 8 MINUTES",0,
				22, 40,13*7+3,-1,1,"ALIEN INTELLIGENCE : PITIFUL",0,
				22, 40,16*7+3,-1,1,"MISSION INFORMATION :",0,
				22, 40,19*7+3,-1,1,"A SCIENTIST HAS CRASH LANDED IN THE HEART OF A",0,
				22, 40,21*7+3,-1,1,"GIANT WINDTUNNEL. FIND HIM AND RETURN HIM TO",0,
				22, 40,23*7+3,-1,1,"THE MOTHERSHIP IN ONE PIECE.",0,
				22, 40,26*7+3,-1,1,"GOOD LUCK!",0
				};

TEXT levelstatus[]={22,40, 3*7+3,-1,1,"MISSION STATUS",0,
				22, 40, 7*7+3,-1,1,"TOTAL NUMBER OF SURVIVORS : AAAA",0,
				22, 40, 9*7+3,-1,1,"SURVIVORS RESCUED : AAAA",0,
				22, 40,11*7+3,-1,1,"NUMBER OF SHOTS FIRED: AAAA",0,
				22, 40,13*7+3,-1,1,"SHOTS ON TARGET : AAAA",0,
				22, 40,15*7+3,-1,1,"OVERALL STATUS : POOPED",0
				};

ULONG	*levelbmf;

USHORT	craftremaining;
FLAG	levelcompleted;
FLAG	changelevel;		/* RESTORE: COMPLETION: A TELEPORTER (set_level) SENT THE CRAFT TO ANOTHER MISSION: LEAVE THIS ONE AT ONCE (MAIN.C) */
LONG	newlevel;		/* RESTORE: THE MISSION IT SENT THE CRAFT TO (0 TO 8) */

USHORT	totalsurvivors;
USHORT	rescuedsurvivors;
USHORT	killedsurvivors;

USHORT	shotsfired;
USHORT	shotsontarget;
USHORT	shotsaccuracy;

USHORT	alienskilled;

USHORT	weaponbonus;

USHORT	timebonus;

/* RESTORE: NEW WORK, not NMS's (owner's decision). NMS's July code tests
   levelcompleted (MAIN.C) but never sets it, and no NMS source defines when
   a mission is complete. Here a mission completes when every shootable
   creature its LEVELn.BMF placed has been destroyed (its sprite switched
   off by the logic's "die", or its slot taken by another sprite). */

USHORT	shootslot[MAX_SPRITES];		/* RESTORE: NEW WORK: SPRITE SLOT OF EACH */
USHORT	shootname[MAX_SPRITES];		/* RESTORE: NEW WORK: SHOOTABLE PLACEMENT */
USHORT	shootcount;			/* RESTORE: NEW WORK */

/**************************************
*         ENUMERATION TABLES          *
**************************************/

enum{
	OFFSET_NAME=0,
	OFFSET_X,
	OFFSET_Y,
	OFFSET_NORMAL_STARTFRAME,
	OFFSET_NORMAL_ENDFRAME,
	OFFSET_DEATH_STARTFRAME,
	OFFSET_DEATH_ENDFRAME,
	OFFSET_WOUNDED_STARTFRAME,
	OFFSET_WOUNDED_ENDFRAME,
	OFFSET_UP_STARTFRAME,
	OFFSET_UP_ENDFRAME,
	OFFSET_DOWN_STARTFRAME,
	OFFSET_DOWN_ENDFRAME,
	OFFSET_LEFT_STARTFRAME,
	OFFSET_LEFT_ENDFRAME,
	OFFSET_RIGHT_STARTFRAME,
	OFFSET_RIGHT_ENDFRAME,
	OFFSET_SPEED_DELAY,
	OFFSET_HIT_POINTS,
	OFFSET_PRIORITY,
	OFFSET_DAMAGE,
	OFFSET_START_DELAY,
	OFFSET_RANDOM_STARTDELAY,
	OFFSET_INITIAL_STARTFRAME,
	OFFSET_INITIAL_VARIANCE,
	OFFSET_CLIPPED,
	OFFSET_SCORE,
	OFFSET_PLAYTYPE
	};

/**************************************
*        LOAD LEVEL SPRITES           *
**************************************/

FLAG
Load_Level_Sprites(char *fname)
{

#if 0

long	fsize;
volatile	USHORT	i;
Uint32	name	=	0;
volatile	ULONG	BuffCount	=	0;

	for (i=0;i<MAX_SPRITES;i++)
	{
		if(sprites[i].on==TRUE)
		{
			name=sprites[i].name;		/* GET NUMBER OF SPRITE */

			Goffset[i]	=	BuffCount;

			ReadFile(fnames_new[name],(BigBuffer+Goffset[i]));

			BuffCount+=fsize;

			if((BuffCount > LoadSize) || ( (fsize % 4) !=0))
			{
				slPrint("Out of GFX BUFFER",slLocate(1,10));
				break;
			}
		}

	}
	
	return TRUE;
	#endif
}

/**************************************
*    PROCESS SPRITE DATA FOR LEVEL    *
**************************************/

FLAG 
Process_Script(BYTE *fname)
{

ULONG	spriteindex,numentries;

	levelbmf=(ULONG *)LoadFile(fname);	/* RESTORE: was a cast as lvalue, (UBYTE *)levelbmf=... */
	numentries=levelbmf[0];

	spriteindex=1;
	shootcount=0;					/* RESTORE: NEW WORK */

	while(spriteindex<(numentries*28))
	{
		Process_Linedata(levelbmf+spriteindex);
		spriteindex+=28;
	}

	MyFree((UBYTE *)levelbmf);

	return TRUE;

}

/* RESTORE: a number 0 placement is drawn as the tube (CONTROL.C, LOADDATA.C)
   only where GRAPHICS/MAPS/MAPS/LEVELn.MAP names a tube for it (owner's
   decision). MAP2BMF writes 0 for every name its table lacks (MAP2BMF.C:
   391-392), so in missions 3 and 6 some 0s are not tubes; these are their
   LEVELn.BMF entries (counted from 0, in the .MAP's order), hidden here.
   Missions 2, 4, 5, 7, 8 and 9 have no tube and draw no number 0 at all;
   Mission 1's 0s are all tubel1. */

typedef struct
{
	UBYTE	level;
	USHORT	entry;
} NOTUBE;

NOTUBE notube[]=
{
	{2, 10},{2, 70},		/* MISSION 3: vinel32 (415,301) (580,152) */
	{2, 48},{2, 82},{2, 89},{2,140},	/* tonsl3 (877,66) (932,292) (317,302) (461,543) */
	{2, 79},			/* l3rocklr (458,194) */
	{2,167},{2,168},{2,169},{2,170},{2,171},{2,172},	/* vtestl3 */
	{5, 61},{5, 62},{5,100},{5,146},	/* MISSION 6: tonsl6 (200,274) (748,276) (432,503) (550,804) */
	{255,0}
};

/**************************************
*        PROCESS SINGLE SPRITE        *
**************************************/

FLAG 
Process_Linedata(ULONG *data)
{

LONG	sprite_no;
LONG	sprite_name;
LONG	x,y;
LONG	normal_startframe,normal_endframe;
LONG	death_startframe,death_endframe;
LONG	wounded_startframe,wounded_endframe;
LONG	up_startframe,up_endframe;
LONG	down_startframe,down_endframe;
LONG	left_startframe,left_endframe;
LONG	right_startframe,right_endframe;
LONG	speed_delay;
LONG	damage;
LONG	hit_points;
LONG	priority;
LONG	score;
LONG	start_delay;
LONG	random_startdelay;
LONG	initial_startframe;
LONG	initial_variance;
LONG	clipped;
LONG	playtype;
USHORT	i;					/* RESTORE: FOR notube[] */

	sprite_name		=	data[OFFSET_NAME];	/* GET DATA FOR SPRITE */
	x				=	data[OFFSET_X]; 
	y				=	data[OFFSET_Y]; 
	normal_startframe	=	data[OFFSET_NORMAL_STARTFRAME]; 
	normal_endframe	=	data[OFFSET_NORMAL_ENDFRAME]; 
	death_startframe	=	data[OFFSET_DEATH_STARTFRAME]; 
	death_endframe		=	data[OFFSET_DEATH_ENDFRAME]; 
	wounded_startframe	=	data[OFFSET_WOUNDED_STARTFRAME]; 
	wounded_endframe	=	data[OFFSET_WOUNDED_ENDFRAME]; 
	up_startframe		=	data[OFFSET_UP_STARTFRAME]; 
	up_endframe		=	data[OFFSET_UP_ENDFRAME]; 
	down_startframe	=	data[OFFSET_DOWN_STARTFRAME]; 
	down_endframe		=	data[OFFSET_DOWN_ENDFRAME]; 
	left_startframe	=	data[OFFSET_LEFT_STARTFRAME]; 
	left_endframe		=	data[OFFSET_LEFT_ENDFRAME]; 
	right_startframe	=	data[OFFSET_RIGHT_STARTFRAME]; 
	right_endframe		=	data[OFFSET_RIGHT_ENDFRAME]; 
	speed_delay		=	data[OFFSET_SPEED_DELAY]; 
	hit_points		=	data[OFFSET_HIT_POINTS]; 
	priority			=	data[OFFSET_PRIORITY]; 
	damage			=	data[OFFSET_DAMAGE]; 
	start_delay		=	data[OFFSET_START_DELAY]; 
	random_startdelay	=	data[OFFSET_RANDOM_STARTDELAY]; 
	initial_startframe	=	data[OFFSET_INITIAL_STARTFRAME]; 
	initial_variance	=	data[OFFSET_INITIAL_VARIANCE]; 
	clipped			=	data[OFFSET_CLIPPED]; 
	score			=	data[OFFSET_SCORE]; 
	playtype			=	data[OFFSET_PLAYTYPE]; 

	if (sprite_name==0) normal_endframe=24;	/* FRIG FOR TUBEL1 */

	if(playtype==0) 
	{
		sprite_no=add_sprite(sprite_name,ACTION_CYCLE,x,y);
	}
	else
	{
		sprite_no=add_sprite(sprite_name,ACTION_PINGPONG,x,y);
	}

	if(sprite_no!=-1)
	{
		sprites[sprite_no].framecounter=(start_delay+(rand()&3));
		sprites[sprite_no].frame=normal_startframe;
		sprites[sprite_no].startframe=normal_startframe;
		sprites[sprite_no].endframe=normal_endframe;
		sprites[sprite_no].priority=priority;
		sprites[sprite_no].shootable=Placed_Shootable(sprite_name);	/* RESTORE: SEE PROCESS.C sprites_shoot */
		sprites[sprite_no].clipped=clipped;

		/* RESTORE: the placement's other frame ranges, which the July 1996
		   logic selects (setanimleft, setanimdeath...). The July PS1 code
		   read them above and dropped them; stored as the PC version stores
		   them (SOURCE/MAKEBACK/LEVEL.C process_line). */
		sprites[sprite_no].normal_startframe=normal_startframe;
		sprites[sprite_no].normal_endframe=normal_endframe;
		sprites[sprite_no].left_startframe=left_startframe;
		sprites[sprite_no].left_endframe=left_endframe;
		sprites[sprite_no].right_startframe=right_startframe;
		sprites[sprite_no].right_endframe=right_endframe;
		sprites[sprite_no].up_startframe=up_startframe;
		sprites[sprite_no].up_endframe=up_endframe;
		sprites[sprite_no].down_startframe=down_startframe;
		sprites[sprite_no].down_endframe=down_endframe;
		sprites[sprite_no].wounded_startframe=wounded_startframe;
		sprites[sprite_no].wounded_endframe=wounded_endframe;
		sprites[sprite_no].death_startframe=death_startframe;
		sprites[sprite_no].death_endframe=death_endframe;

		/* RESTORE: owner's decision (2026-11-21, "doublecheck the enemy death ... code in the Saturn/DOS version we forgot"), REPLACING THE
		   2026-11-10 USE OF THE BMF'S HIT POINTS: NO NMS VERSION USES THEM (THE PC'S LEVEL.C READS hit_points AND NEVER STORES IT, ITS
		   SCRATCH.C:52 STILL SAYS "hit points etc..."; THE SATURN'S LOGIC/LEVEL.C:333 READS AND DROPS IT): THE FIRST HIT RUNS THE CREATURE'S
		   on_hit. WITH THEM A SPITTER OR A PLUNGER TOOK 2 TO 10 SHOTS WITH NO SIGN OF ANY, AND LOOKED UNKILLABLE. */
		hitsleft[sprite_no]=1;		/* (WAS hit_points, AT MOST 255) */
		/* RESTORE: NEW WORK (owner's decision, 2026-11-10): REMEMBER EACH
		   SHOOTABLE PLACEMENT WHOSE JULY 1996 LOGIC CAN REACH "die" (PROCESS.C
		   Can_Die); ONLY THOSE COUNT TOWARD COMPLETION */
		if ((sprites[sprite_no].shootable==YES) &&
			(Can_Die(sprites[sprite_no].move_ptr)==YES))
		{
			shootslot[shootcount]=sprite_no;
			shootname[shootcount]=sprite_name;
			shootcount++;
		}
		if (sprite_name==0)		/* RESTORE: NOT A TUBE IN THE .MAP? */
		{
			for (i=0;notube[i].level!=255;i++)
			{
				if ((notube[i].level==level) &&
					(notube[i].entry==(data-levelbmf-1)/28))
					{
						sprites[sprite_no].visible=FALSE;
						sprites[sprite_no].shootable=NO;	/* RESTORE: NOT HIT EITHER */
					}
			}
		}
		return PASS;
	}
	return FAIL;
}

/**************************************
*      SHOW DESCRIPTION OF LEVEL      *
**************************************/

void
Level_Description(BYTE *mappsx,BYTE *mapspr,TEXT *levelinfo,SHORT nolines)
{

UBYTE	*levelgrid;
SPRITE*	spr_LEVELGRID;

UBYTE	*levelmap;
SPRITE*	spr_LEVELMAP;

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

	levelgrid=LoadFile("../INTRO/LEVLGRD.PSX");
	spr_LEVELGRID = (SPRITE*)LoadFile("../INTRO/LEVLGRD.SPR");
	Init_Spr_List(spr_LEVELGRID,(ULONG)levelgrid);

	levelmap=LoadFile(mappsx);
	spr_LEVELMAP = (SPRITE*)LoadFile(mapspr);
	Init_Spr_List(spr_LEVELMAP,(ULONG)levelmap);

	Init_Teletype();

	Cls();

	while (fade!=0)
	{
		if (((Debounce1 & PaDstart)!=0) && (fade==128))
		{
			Start_Fade_Down(-16);
			SsUtKeyOn(vab,SFX_EXPLOSION2,0,FXPITCH,0,sfxvolume,sfxvolume);
		}

		Start_Loop();

		sprctrl.red=64;
		sprctrl.green=64;
		sprctrl.blue=64;

		Print_Sprite(spr_LEVELMAP,320,240-8,252);

		sprctrl.red=96;
		sprctrl.green=96;
		sprctrl.blue=96;

		Print_Sprite(spr_LEVELGRID,320,240,254);

		sprctrl.red=0;
		sprctrl.green=96;
		sprctrl.blue=0;

		Print_Teletype(levelinfo,nolines);

		sprctrl.red=128;
		sprctrl.green=128;
		sprctrl.blue=128;

		End_Loop();
	}

	VSync(0);
	SetDispMask(OFF);

}

/**************************************
*      LOAD THE LEVEL SELECTED        *
**************************************/

void 
Load_Level(void)
{
	switch (level)
	{
		case 0:
		Level_Description("../INTRO/LEV1.PSX","../INTRO/LEV1.SPR",leveltext1,TELETYPELEVEL1);

		Init_Map("../MAPS/LEV1.BIN",
				"../MAPS/LEV1.MAP",
				"../MAPS/LEV1.PAL",
				"../MAPS/LEVEL1.BMF",
				"../MAPS/LEVEL1C.BIN");
		break;

		case 1:
		Level_Description("../INTRO/LEV2.PSX","../INTRO/LEV2.SPR",leveltext2,TELETYPELEVEL2);

		Init_Map("../MAPS/LEV2.BIN",
				"../MAPS/LEV2.MAP",
				"../MAPS/LEV2.PAL",
				"../MAPS/LEVEL2.BMF",
				"../MAPS/LEVEL2C.BIN");
		break;

		case 2:
		Level_Description("../INTRO/LEV3.PSX","../INTRO/LEV3.SPR",leveltext3,TELETYPELEVEL3);

		Init_Map("../MAPS/LEV3.BIN",
				"../MAPS/LEV3.MAP",
				"../MAPS/LEV3.PAL",
				"../MAPS/LEVEL3.BMF",
				"../MAPS/LEVEL3C.BIN");


		break;

		case 3:
		Level_Description("../INTRO/LEV4.PSX","../INTRO/LEV4.SPR",leveltext4,TELETYPELEVEL4);

		Init_Map("../MAPS/LEV4.BIN",
				"../MAPS/LEV4.MAP",
				"../MAPS/LEV4.PAL",
				"../MAPS/LEVEL4.BMF",
				"../MAPS/LEVEL4C.BIN");

		break;

		case 4:
		Level_Description("../INTRO/LEV5.PSX","../INTRO/LEV5.SPR",leveltext5,TELETYPELEVEL5);

		Init_Map("../MAPS/LEV5.BIN",
				"../MAPS/LEV5.MAP",
				"../MAPS/LEV5.PAL",
				"../MAPS/LEVEL5.BMF",
				"../MAPS/LEVEL5C.BIN");

		break;

		case 5:
		Level_Description("../INTRO/LEV6.PSX","../INTRO/LEV6.SPR",leveltext6,TELETYPELEVEL6);

		Init_Map("../MAPS/LEV6.BIN",
				"../MAPS/LEV6.MAP",
				"../MAPS/LEV6.PAL",
				"../MAPS/LEVEL6.BMF",
				"../MAPS/LEVEL6C.BIN");

		break;

		case 6:
		Level_Description("../INTRO/LEV7.PSX","../INTRO/LEV7.SPR",leveltext7,TELETYPELEVEL7);

		Init_Map("../MAPS/LEV7.BIN",
				"../MAPS/LEV7.MAP",
				"../MAPS/LEV7.PAL",
				"../MAPS/LEVEL7.BMF",
				"../MAPS/LEVEL7C.BIN");
		break;

		case 7:
		Level_Description("../INTRO/LEV8.PSX","../INTRO/LEV8.SPR",leveltext8,TELETYPELEVEL8);
		Init_Map("../MAPS/LEV8.BIN",
				"../MAPS/LEV8.MAP",
				"../MAPS/LEV8.PAL",
				"../MAPS/LEVEL8.BMF",
				"../MAPS/LEVEL8C.BIN");

		break;

		case 8:
		Level_Description("../INTRO/LEV9.PSX","../INTRO/LEV9.SPR",leveltext9,TELETYPELEVEL9);

		Init_Map("../MAPS/LEV9.BIN",
				"../MAPS/LEV9.MAP",
				"../MAPS/LEV9.PAL",
				"../MAPS/LEVEL9.BMF",
				"../MAPS/LEVEL9C.BIN");
		break;
	}
}

/**************************************
*  RESTORE: NEW WORK: LEVEL COMPLETE? *
**************************************/

void
Check_Level_Completed(void)
{

USHORT	i;
USHORT	left;
USHORT	gone;

#ifdef X360_TEST_COMPLETE
	/* RESTORE: NEW WORK, TEST BUILDS ONLY (X360_CFLAGS=-DX360_TEST_COMPLETE=N):
	   AFTER N GAME CYCLES SWITCH EVERY SHOOTABLE PLACEMENT OFF, AS "die" DOES */
	if (gamecycle==X360_TEST_COMPLETE)
		for (i=0;i<shootcount;i++) sprites[shootslot[i]].on=FALSE;
#endif

#ifdef X360_TEST_HITALL
	/* RESTORE: NEW WORK, TEST BUILDS ONLY (X360_CFLAGS=-DX360_TEST_HITALL=N):
	   AT GAME CYCLE N EVERY SHOOTABLE CREATURE WITH ART TAKES HITS UNTIL ITS
	   HIT POINTS ARE GONE, AS SHOTS WOULD GIVE THEM (PROCESS.C Hit_Sprite) */
	if (gamecycle==X360_TEST_HITALL)
	{
		for (i=0;i<MAX_SPRITES;i++)
		{
			if ((sprites[i].on==TRUE) && (sprites[i].shootable==YES) && (sprtable[i]!=NULL))
			{
#ifdef X360_TEST_HITALL_BURST		/* -DX360_TEST_HITALL_BURST: AS A SHOT'S KILL, THE BURST AND EXPLOSION (FIRE.C Death_Burst) */
				if (Can_Die(sprites[i].move_ptr)==YES) Death_Burst(sprites[i].x,sprites[i].y,0);
#endif
				while (hitsleft[i]>1) Hit_Sprite(i);
				Hit_Sprite(i);
			}
		}
	}
#endif

	if (shootcount==0) return;

	/* RESTORE: NEW WORK (owner's decision): COUNTED ARE THE SHOOTABLE PLACEMENTS
	   WITH ART WHOSE LOGIC CAN REACH "die" (Process_Linedata). ONE WITH NO ART
	   CANNOT BE SEEN OR HIT. THE MISSION COMPLETES WHEN SOME HAVE BEEN DESTROYED AND NONE
	   IS LEFT; WITH NONE TO COUNT IT NEVER COMPLETES (DEATH OR QUIT, AS NMS) */
	left=0;
	gone=0;
	for (i=0;i<shootcount;i++)
	{
		if ((sprites[shootslot[i]].on==TRUE) &&
			(sprites[shootslot[i]].name==shootname[i]))
		{
			if (sprtable[shootslot[i]]!=NULL) left++;
		}
		else gone++;
	}

	if ((left==0) && (gone!=0)) levelcompleted=YES;

}

/**************************************
*  RESTORE: NEW WORK: STATUS NUMBERS  *
**************************************/

void
Put_Status_Number(BYTE *text,USHORT number)
{

BYTE	*a;
BYTE	*b;
BYTE	digits[6];
SHORT	n;

	b=NULL;					/* AFTER THE LAST ": " (NMS'S "AAAA" ON */
	for (a=text;*a!=0;a++)			/* THE FIRST VISIT, OUR NUMBER AFTER) */
		if ((a[0]==':') && (a[1]==' ')) b=a+2;
	if (b==NULL) return;
	a=b;

	n=0;
	do
	{
		digits[n++]='0'+(number MOD 10);
		number/=10;
	} while (number!=0);

	while (n>0) *a++=digits[--n];
	*a=0;

}

/**************************************
*      EXIT THE CURRENT LEVEL         *
**************************************/

void 
Exit_Level(void)
{
	/* RESTORE: NEW WORK: FILL THE LINE A COUNTER EXISTS FOR; THE OTHERS KEEP
	   NMS'S PLACEHOLDERS (NO SURVIVOR COUNT OR OVERALL STATUS IS DEFINED, AND
	   SHOTS ON TARGET IS COUNTED BY THE REVIVED process_collisions IN FIRE.C, SHOWN BELOW) */
	Put_Status_Number(levelstatus[3].textstuff,shotsfired);
	Put_Status_Number(levelstatus[4].textstuff,shotsontarget);	/* RESTORE: NEW WORK (owner's decision) */

	switch (level)
	{

	case 0:
	Level_Description("../INTRO/LEV1.PSX","../INTRO/LEV1.SPR",levelstatus,TELETYPESTATUS);
	break;

	case 1:
	Level_Description("../INTRO/LEV2.PSX","../INTRO/LEV2.SPR",levelstatus,TELETYPESTATUS);
	break;

	case 2:
	Level_Description("../INTRO/LEV3.PSX","../INTRO/LEV3.SPR",levelstatus,TELETYPESTATUS);
	break;

	case 3:
	Level_Description("../INTRO/LEV4.PSX","../INTRO/LEV4.SPR",levelstatus,TELETYPESTATUS);
	break;

	case 4:
	Level_Description("../INTRO/LEV5.PSX","../INTRO/LEV5.SPR",levelstatus,TELETYPESTATUS);
	break;

	case 5:
	Level_Description("../INTRO/LEV6.PSX","../INTRO/LEV6.SPR",levelstatus,TELETYPESTATUS);
	break;

	case 6:
	Level_Description("../INTRO/LEV7.PSX","../INTRO/LEV7.SPR",levelstatus,TELETYPESTATUS);
	break;

	case 7:
	Level_Description("../INTRO/LEV8.PSX","../INTRO/LEV8.SPR",levelstatus,TELETYPESTATUS);
	break;

	case 8:
	Level_Description("../INTRO/LEV9.PSX","../INTRO/LEV9.SPR",levelstatus,TELETYPESTATUS);
	break;
	}
}

/**************************************
*        INITIALISE NEW GAME          *
**************************************/

void
Init_Newgame(void)
{

	/* RESTORE: owner's decision. NMS'S THREE TESTS ALL READ difficulty==0 AND A FRIG THEN FORCED 1 SHIP, SO THE
	   EASY/NORMAL/HARD CHOICE (OPTIONS.C, difficulty 0/1/2) CHANGED NOTHING. READ AS A TYPO FOR 0/1/2, IN THE
	   ORDER NMS WROTE THE NUMBERS: EASY 7, NORMAL 5, HARD 3; THE FRIG IS DROPPED. */
	if (difficulty==0) craftremaining=7;	/* INITIALISE NUMBER OF SHIPS */
	if (difficulty==1) craftremaining=5;
	if (difficulty==2) craftremaining=3;

	score=15000;				/* RESTORE: bug fix (owner's decision 2026-11-21), MOVED FROM Init_Level, WHICH RUNS FOR EVERY LIFE: THE SCORE
						   WENT BACK TO 15000 AT EACH NEW LIFE, SO THE HIGH SCORE TABLE SAW ONLY THE LAST LIFE. THE SATURN SETS ITS SCORE ONCE (GAME.C:153). */
}

/**************************************
*        INITIALISE A LEVEL           *
**************************************/

void
Init_Level(void)
{

	scoreadder=0;

	/* RESTORE: bug fix (owner's decision 2026-11-21). THE AMMUNITION WAS SET ONCE, AT POWER-ON (MAIN.C, BEFORE THE GAME LOOP), AND NOTHING EVER
	   GAVE IT BACK: NOT A NEW LIFE, A NEW MISSION OR A NEW GAME, SO THE LASER, BUCKSHOT, PROTON AND CHAIN RAN DRY FOR GOOD. THE PC AND THE SATURN
	   HAVE NO AMMUNITION AT ALL. NMS'S AMOUNTS (MAIN.C) ARE NOW GIVEN AT THE START OF EVERY LIFE. */
	Reset_Weapon_Ammo();

	fuel=12*FUELMULT;				/* INITIALISE THE FUEL */
	shields=FULLSHIELDS;
	craftdestroyed=NO;
	deadtimer=0;
	levelcompleted=NO;				/* RESTORE: NEW WORK */
	changelevel=NO;
	shotsfired=0;					/* RESTORE: NEW WORK */
	shotsontarget=0;				/* RESTORE: NEW WORK (owner's decision) */

	Init_Ball();
	Init_Particle_List();
	Init_Fire();

}

