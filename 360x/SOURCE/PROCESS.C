
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"
/* RESTORE: AI.DAT is now NMS's logic of 17 July 1996, from the Saturn
   port's tree (20SEPT96 INCLUDES/AI.DAT, unchanged); the PS1 tree held the
   one of 27 February. Its last table, ai_named, needs this type (Saturn
   INCLUDES/TYPES.H). */

typedef struct
{
	char 	*name;
	long	logic;
} names_and_indexs;

#include <math.h>			/* RESTORE: atan2, FOR get_angle_to_id */
#include "AI.DAT"

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Process_Sprites(void);
void	Clear_Sprites(void);
long	add_sprite(int name,int action,int x,int y);
long	process_sprite(long i,long logic_index);	/* RESTORE: AS THE PC VERSION */
void	adopt_inrange_child(long father,long range);
long	find_floor(long x,long y,long max_scan);
unsigned long sqr_root(unsigned long val);

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	HIGH		1
#define	LOW		0

//extern	int	rand(void);

/**************************************
*        DECLARE STRUCTURES           *
**************************************/

typedef struct
{
	long name;
	long action;
} actionstruc;

/* RESTORE: which logic each creature runs. The July PS1 table had eight
   rows for the February logic (EXPLOSION, windtun, l2c1..5, RCK1). These
   are the rows of the PC version's table (SOURCE/MAKEBACK/PROCESS.C
   sprites_actions, 12 June 1996) for every creature the PS1 maps place,
   with the same routine names in the July logic. A creature is given by
   its number in LEVELn.BMF, the index of its name in the map converter's
   table (GRAPHICS/MAPS/GLOBALS.H fnames_ian): the PS1 NAME_ enum is one
   off from 91 on (see LOADDATA.C), so the names are in the comments.

   Rows marked ATR are not in the PC table (owner's decision, delegated,
   2026-11-10): the July logic has a routine named for the creature, and
   the Saturn port's attribute file (BIN/ATTRIBS.ATR, 14 August 1996, read
   with the PC version's names) gives that creature a routine at that
   place in its own, later logic. Inferred from the name match.

   A creature in neither table keeps the animation its map entry asks for
   (ai_cycle or ai_pingpong), as before. */

actionstruc sprites_actions[]=
{
	{NAME_EXPLOSION,ai_explode},	/* (JULY PS1 AND PC) */

	{  3,ai_floppy},		/* FLOPPY */
	{ 29,ai_new_man},		/* survrs */
	{ 59,ai_tentacle},		/* TENTAC1 */
	{ 60,ai_tentacle},		/* TENTAC2 */
	{123,ai_tentacle},		/* l2tentr */
	{ 62,ai_mussel},		/* MUSSEL */
	{ 91,ai_mussel},		/* chapl1 */
	{103,ai_tonsl1},		/* tonsl1 */
	{ 83,ai_flock},			/* tad */
	{ 52,ai_asteroid},		/* RCK01 (PC NAME_RCK1) */
	{ 53,ai_asteroid},		/* RCK02 (PC NAME_RCK2) */
	{ 85,ai_asteroid},		/* l2rock1 */
	{207,ai_asteroid},		/* l3rock1 (OWNER'S DECISION, 2026-11-10: THE ATR GIVES THE ROCKS OF */
	{140,ai_asteroid},		/* l4rock1    MISSIONS 3 TO 6 THE ASTEROID ROUTINE, AS PC JUNE'S TABLE DOES */
	{184,ai_asteroid},		/* l5rock1    MISSIONS 1 AND 2'S) */
	{168,ai_asteroid},		/* l6rock1 */
	{ 81,ai_wind_tunnel},		/* windtun */

	{ 72,ai_crystal},		/* l2c1 */
	{ 73,ai_crystal},		/* l2c2 */
	{ 74,ai_crystal},		/* l2c3 */
	{ 75,ai_crystal},		/* l2c4 */
	{ 76,ai_crystal},		/* l2c5 */
	{104,ai_crystal},		/* l1c1 */
	{105,ai_crystal},		/* l1c2 */
	{106,ai_crystal},		/* l1c3 */
	{107,ai_crystal},		/* l1c4 */
	{108,ai_crystal},		/* l1c5 */
	{134,ai_crystal},		/* s_l4c1 */
	{135,ai_crystal},		/* s_l4c2 */
	{136,ai_crystal},		/* s_l4c3 */
	{137,ai_crystal},		/* s_l4c4 */
	{138,ai_crystal},		/* s_l4c5 */
	{141,ai_crystal},		/* l4c1 */
	{142,ai_crystal},		/* l4c3 */
	{143,ai_crystal},		/* l4c4 */
	{144,ai_crystal},		/* l4c5 */

	{109,ai_lazer},			/* lazerl1 */
	{126,ai_lazer},			/* lazerl2 */
	{191,ai_lazer},			/* lazerl5 */
	{170,ai_lazer},			/* lazerl6 */
	/* RESTORE: PROBABLE, decided by the coordinator overnight on 2026-11-13, for the owner to confirm: lazer3 (BMF 218, Mission 3, two placements) has no row in the PC table but the same frames as its four siblings, so it runs ai_lazer */
	{218,ai_lazer},			/* lazer3 */

	{194,ai_soft_rock},		/* l51bt */
	{195,ai_soft_rock},		/* l52bt */
	{196,ai_soft_rock},		/* l53bt */
	{197,ai_soft_rock},		/* l54bt */
	{163,ai_soft_rock},		/* l61bt */
	{164,ai_soft_rock},		/* l62bt */
	{165,ai_soft_rock},		/* l63bt */
	{166,ai_soft_rock},		/* l64bt */

	{ 92,ai_none},			/* vinel11 */
	{ 93,ai_none},			/* vines12 */
	{ 95,ai_none},			/* vine14 */
	{ 96,ai_none},			/* vine15 */
	{ 97,ai_none},			/* vine16 */
	{113,ai_none},			/* vinel22 */
	{114,ai_none},			/* vinel23 */
	{115,ai_none},			/* vinel24 */
	{116,ai_none},			/* vinel26 */
	{127,ai_none},			/* vtestl2 */
	{198,ai_none},			/* vinel31 */
	{200,ai_none},			/* vinel34 */
	{201,ai_none},			/* vinel36 */
	{130,ai_none},			/* vinel42 */
	{131,ai_none},			/* vinel43 */
	{132,ai_none},			/* vinel44 */
	{133,ai_none},			/* vinel46 */
	{128,ai_none},			/* vtestl4 */
	{174,ai_none},			/* vinel51 */
	{175,ai_none},			/* vinel52 */
	{176,ai_none},			/* vinel53 */
	{177,ai_none},			/* vinel54 */
	{178,ai_none},			/* vinel56 */
	{185,ai_none},			/* vtestl5 */
	{147,ai_none},			/* vinel61 */
	{148,ai_none},			/* vinel62 */
	{149,ai_none},			/* vinel63 */
	{150,ai_none},			/* vinel64 */
	{152,ai_none},			/* vinel66 */
	{173,ai_none},			/* vtestl6 */

	{ 17,ai_mrspit},		/* ATR mrspit */
	{110,ai_msspit},		/* ATR msspit */
	{ 32,ai_plunger},		/* ATR PLUNGER */
	{124,ai_plunger},		/* ATR plungl2 */
	{208,ai_plunger},		/* ATR plungl3 */
	{145,ai_plunger},		/* ATR plungl4 */
	{193,ai_plunger},		/* ATR plungl5 */
	{169,ai_plunger},		/* ATR plungl6 */
	{ 22,ai_star},			/* ATR star */
	{ 23,ai_starfsh},		/* ATR starfsh */

	/* RESTORE: completion (owner's decision 2026-11-13: crystals the later logic binds by name). THE SATURN'S ATTRIBUTE FILE GIVES THESE
	   TEN CRYSTAL NAMES, ALL PLACED IN THE PS1 MAPS, THE ROUTINE ai_stream_crystal: THE ENTRY'S ROUTINE NUMBER IS EXACTLY ITS START IN
	   THE JULY AI.DAT (2899), AND THE SATURN'S LOADER USES IT AS THE SPRITE'S OWN ROUTINE (SNSYSTEM/PROCESS.C:137, LOGIC/PROCESS.C:157).
	   BEFORE, THEY KEPT THEIR MAP ANIMATION. */
	{220,ai_stream_crystal},	/* ATR l7c1 */
	{221,ai_stream_crystal},	/* ATR l7c2 */
	{223,ai_stream_crystal},	/* ATR l7c4 */
	{202,ai_stream_crystal},	/* ATR s_l3c1 */
	{158,ai_stream_crystal},	/* ATR l6c1 */
	{160,ai_stream_crystal},	/* ATR l6c3 */
	{156,ai_stream_crystal},	/* ATR s_l6c4 */
	{157,ai_stream_crystal},	/* ATR s_l6c5 */
	{189,ai_stream_crystal},	/* ATR l5c4 */
	{190,ai_stream_crystal},	/* ATR l5c5 */

	/* RESTORE: completion, rebuilt from the PC version's table (MAKEBACK/PROCESS.C, NAME_pod4,ai_pod). THE POD RUNS ai_pod (AI.DAT): IT STANDS UNTIL
	   THE CRAFT IS WITHIN 128 PIXELS, FLASHES WHILE WITHIN 160, FALLS WHEN DROPPED. THE PS1 MAPS' POD IS podtemp (171, Saturn art S_POD, loaded in
	   Mission 6 only). NO MISSION PLACES A POD AT A REAL POSITION: Mission 6's one podtemp IS AT 0,0 (add_sprite, BELOW, KEEPS IT STILL) */
	{171,ai_pod},

	{NAME_END,ai_none},
};

/* RESTORE: creatures whose logic flies them along a path. The PC version
   loads a level's paths from a .PAF file (SOURCE/MAKEBACK/LEVEL.C
   load_level_paf, PATH.C) and a placement's path from extra fields of its
   map line; no archive copy holds a .PAF and LEVELn.BMF has no such
   fields. With no path, ai_flock gives a tad no leader and flies it by an
   angle nothing sets, straight down and off the map. So for these the
   logic runs (frames, on_hit, die) and its moves are not applied: the
   creature stays where the map puts it, as on NMS's Saturn build, whose
   set_vel and get_angle_to_id are switched off (LOGIC/PROCESS.C). */

long sprites_nopath[]=
{
	83,				/* tad */
	NAME_END
};


/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

typedef struct
{
	flag	load;
	flag	shootable;
	long	priority;
} attribstruc;

/* RESTORE: realigned by its comment names (owner's decision). In the
   July table each row's comment names its creature, but the rows are
   not in GLOBALS.H's order: an extra NAME_mussel row at index 26 (no
   PS1 name has that spelling) moves every later row on by one, and the
   FUEL..SHELL group is in another order (FUEL, EXPLOSION, SPIKE, SHELL,
   LEAPER, POD.. against the enum's FUEL, EXPLOSION, LEAPER, POD, SHIELD,
   LASER, GRAVITY, SPIKE, SHELL), so the game read e.g. bouncer1 as
   shootable and tentac1 not. Each row now sits at its comment's name
   (C designators); names the table has no row for are NO,NO,LOW. */

attribstruc sprites_attribs[NAME_END]=
{
	[NAME_NULL]={NO,NO,LOW,},		//NAME_NULL,
	[NAME_CSHIP]={YES,NO,LOW,},		//NAME_CSHIP,
	[NAME_MISSILE]={YES,NO,LOW,},		//NAME_MISSILE,
	[NAME_FLOPPY]={NO,NO,LOW,},		//NAME_FLOPPY,
	[NAME_bouncer3]={NO,NO,LOW,},		//NAME_bouncer3,
	[NAME_s]={NO,NO,LOW,},		//NAME_s,
	[NAME_flawa]={NO,NO,LOW,},		//NAME_flawa,
	[NAME_nuray]={NO,NO,LOW,},		//NAME_nuray,

	[NAME_chisler]={NO,NO,LOW,},		//NAME_chisler,
	[NAME_stship2]={NO,NO,LOW,},		//NAME_stship2,
	[NAME_0pod]={NO,NO,LOW,},		//NAME_0pod,
	[NAME_flop2]={NO,NO,LOW,},		//NAME_flop2,
	[NAME_spiral3]={NO,NO,LOW,},		//NAME_spiral3,
	[NAME_g2]={NO,NO,LOW,},		//NAME_g2,
	[NAME_fork01]={NO,NO,LOW,}, 		//NAME_fork01,
	[NAME_fork02]={NO,NO,LOW,},		//NAME_fork02,
	[NAME_flawi]={NO,NO,LOW,},		//NAME_flawi,
	[NAME_mrspit]={NO,NO,LOW,},		//NAME_mrspit,
	[NAME_jshel]={NO,NO,LOW,},		//NAME_jshel,
	[NAME_tomi]={NO,NO,LOW,},		//NAME_tomi,
	[NAME_float]={NO,NO,LOW,},		//NAME_float,

	[NAME_bug]={NO,YES,LOW,},		//NAME_bug,
	[NAME_star]={NO,NO,LOW,},		//NAME_star,
	[NAME_starfsh]={NO,NO,LOW,},		//NAME_starfsh,
	[NAME_turret1]={NO,NO,LOW,},		//NAME_turret1,
	[NAME_acidspt]={NO,NO,LOW,},		//NAME_acidspt,
//{NO,NO,LOW,},		//NAME_mussel,	/* RESTORE: NO PS1 NAME; NO,NO,LOW = THE DEFAULT */
	[NAME_chapie]={NO,NO,LOW,},		//NAME_chapie,
	[NAME_twinspin]={NO,NO,LOW,},		//NAME_twinspin,
	[NAME_l]={NO,NO,LOW,},		//NAME_l,
	[NAME_survrs]={NO,NO,LOW,},		//NAME_survrs,
	
	[NAME_FUEL]={NO,NO,LOW,},		//NAME_FUEL,
	[NAME_EXPLOSION]={YES,NO,HIGH,},	//NAME_EXPLOSION,
	[NAME_SPIKE]={NO,YES,HIGH,},	//NAME_SPIKE,
	[NAME_SHELL]={NO,YES,HIGH,},	//NAME_SHELL,
	[NAME_LEAPER]={NO,YES,HIGH,},	//NAME_LEAPER,
	[NAME_POD]={NO,NO,LOW,},		//NAME_POD,
	[NAME_SHIELD]={NO,NO,LOW,},		//NAME_SHIELD,
	[NAME_LASER]={NO,NO,LOW,},		//NAME_LASER,
	[NAME_GRAVITY]={NO,NO,LOW,},		//NAME_GRAVITY,
	[NAME_FLOATER]={NO,YES,HIGH,},	//NAME_FLOATER,
	[NAME_HAIR1]={NO,YES,HIGH,},	//NAME_HAIR1,
	[NAME_BOUNCER1]={NO,NO,HIGH,},		//NAME_BOUNCER1,
	[NAME_BOUNCER2]={NO,NO,HIGH,},		//NAME_BOUNCER2,
	[NAME_FORK01]={NO,YES,HIGH,},	//NAME_FORK01,
	[NAME_FORK02]={NO,YES,HIGH,},	//NAME_FORK02,
	[NAME_BFIRE]={NO,YES,HIGH,},	//NAME_BFIRE,
	[NAME_POD0]={NO,NO,LOW,},		//NAME_POD0,
	[NAME_FLOAT]={NO,YES,HIGH,},	//NAME_FLOAT,
	[NAME_SCATTER]={NO,YES,HIGH,},	//NAME_SCATTER,
	[NAME_CLAW]={NO,NO,HIGH,},		//NAME_CLAW,
	[NAME_FROG]={NO,YES,HIGH,},	//NAME_FROG,
	[NAME_FUZZ]={NO,YES,HIGH,},	//NAME_FUZZ,
	[NAME_RCK1]={NO,YES,HIGH,},	//NAME_RCK1,
  	[NAME_RCK2]={NO,YES,HIGH,},	//NAME_RCK2,
	[NAME_RCK3]={NO,YES,HIGH,},	//NAME_RCK3,
	[NAME_BUG]={NO,YES,HIGH,},	//NAME_BUG,
	[NAME_STAR]={NO,YES,HIGH,},	//NAME_STAR,
	[NAME_TURRET]={NO,YES,HIGH,},	//NAME_TURRET1,	/* RESTORE: NAME_TURRET1 IS NOT A PS1 NAME; ITS PLACE, AFTER NAME_STAR, IS NAME_TURRET */
	[NAME_SPIRAL]={NO,NO,LOW,},		//NAME_SPIRAL,
	[NAME_TENTAC1]={NO,YES,HIGH,},	//NAME_TENTAC1,
	[NAME_TENTAC2]={NO,YES,HIGH,},	//NAME_TENTAC2,
	[NAME_MINE]={NO,YES,HIGH,},	//NAME_MINE,
	[NAME_MUSSEL]={NO,YES,HIGH,},	//NAME_MUSSEL,
	[NAME_RCKMORPH]={NO,NO,HIGH,},		//NAME_RCKMORPH,
	[NAME_CHAPIE]={NO,NO,LOW,},		//NAME_CHAPIE,
	[NAME_STARFSH]={NO,YES,HIGH,},	//NAME_STARFSH,
	[NAME_ACIDSPT]={NO,YES,HIGH,},	//NAME_ACIDSPT,

	[NAME_fflop]={NO,YES,HIGH,},	//NAME_fflop,
	[NAME_l2c1]={NO,YES,HIGH,},	//NAME_l2c1,
	[NAME_l2c2]={NO,YES,HIGH,},	//NAME_l2c2,
	[NAME_l2c3]={NO,YES,HIGH,},	//NAME_l2c3,
	[NAME_l2c4]={NO,YES,HIGH,},	//NAME_l2c4,
	[NAME_l2c5]={NO,YES,HIGH,},	//NAME_l2c5,
	[NAME_block1]={NO,YES,HIGH,},	//NAME_block1,
	[NAME_block2]={NO,YES,HIGH,},	//NAME_block2,
	[NAME_block3]={NO,YES,HIGH,},	//NAME_block3,
	[NAME_block4]={NO,YES,HIGH,},	//NAME_block4,
	[NAME_windtun]={NO,YES,HIGH,},	//NAME_windtun,
	[NAME_bloba]={NO,YES,HIGH,},	//NAME_bloba,
	[NAME_tad]={NO,YES,HIGH,},	//NAME_tad,
	[NAME_spfire]={NO,YES,HIGH,},	//NAME_spfire,
	[NAME_l2rock1]={NO,YES,HIGH,},	//NAME_l2rock1,

	[NAME_bordsml]={NO,YES,HIGH,},	//NAME_bordsml,
	[NAME_bordsml3]={NO,YES,HIGH,},	//NAME_bordsml3,
	[NAME_overly2]={NO,YES,HIGH,},	//NAME_overly2,
	[NAME_shipwir2]={NO,YES,HIGH,},	//NAME_shipwir2,

//	NO,//	NO,LOW	//NAME_END
};

/* RESTORE: which placed creature is shootable (owner's decision, 2026-11-10;
   replaces the realigned table above for a placement, which was indexed by
   a BMF number as if it were a PS1 name: the two numberings part after 90).
   A mix of two sources, by BMF number:
   J  the 12 June 1996 name-keyed list (SOURCE/MAKEBACK/ATTRIBS.S: "Shootable
      sprites in Level 1"): every creature it names is shootable
   A  for a creature it does not name, the Saturn port's BIN/ATTRIBS.ATR
      (14 August 1996), shootable byte of the row at the creature's name read
      with the PC version's name list (SOURCE/MAKEBACK/GLOBALS.H); the rows
      make sense up to name 222. INFERRED FROM THE DECODE, NOT READ FROM A
      TABLE: the file was made against a later name list
   C  past that, or for a name with no row, the shootable byte of the ATR rows
      that give the routine the creature runs here (all rows of one routine
      agree: plungers, asteroids, lasers, crystals, soft rocks, tentacles
      yes, wind tunnels no). Inferred in the same way
   A creature in no row keeps the table above (names up to 90, which a number
   and a PS1 name share) or is not shootable (later numbers: the realigned
   table has no row there). */

typedef struct
{
	long	name;
	long	shootable;
} shootrow;

shootrow sprites_shoot[]=
{
	{  0,YES},		/* J TUBEL1 */
	{  3,YES},		/* J FLOPPY */
	{  8,YES},		/* A chisler */
	{ 17,YES},		/* J mrspit */
	{ 20,YES},		/* A float */
	{ 22,YES},		/* A star */
	{ 23,YES},		/* A starfsh */
	{ 24,YES},		/* J turret1 */
	{ 25,YES},		/* J acidspt */
	{ 29,YES},		/* J survrs */
	{ 32,YES},		/* J PLUNGER */
	{ 52,YES},		/* J RCK01 */
	{ 59,YES},		/* J TENTAC1 */
	{ 60,YES},		/* A TENTAC2 */
	{ 62,YES},		/* J MUSSEL */
	{ 71,YES},		/* A fflop */
	{ 72,YES},		/* A l2c1 */
	{ 73,YES},		/* A l2c2 */
	{ 74,YES},		/* A l2c3 */
	{ 76,YES},		/* A l2c5 */
	{ 81,NO },		/* C windtun */
	{ 83,YES},		/* A tad */
	{ 85,YES},		/* A l2rock1 */
	{ 91,YES},		/* J chapl1 */
	{103,YES},		/* J tonsl1 */
	{104,YES},		/* J l1c1 */
	{105,YES},		/* J l1c2 */
	{109,YES},		/* J lazerl1 */
	{110,YES},		/* J msspit */
	{117,YES},		/* A s_l2c1 */
	{120,YES},		/* A s_l2c4 */
	{122,YES},		/* A l2tentl */
	{123,YES},		/* A l2tentr */
	{124,YES},		/* A plungl2 */
	{125,YES},		/* A chapl2 */
	{126,YES},		/* A lazerl2 */
	{128,NO },		/* A vtestl4 */
	{135,YES},		/* A s_l4c2 */
	{136,YES},		/* A s_l4c3 */
	{138,YES},		/* A s_l4c5 */
	{139,YES},		/* A tubes */
	{140,YES},		/* C l4rock1 */
	{145,YES},		/* C plungl4 */
	{168,YES},		/* C l6rock1 */
	{169,YES},		/* C plungl6 */
	{170,YES},		/* C lazerl6 */
	{184,YES},		/* C l5rock1 */
	{191,YES},		/* C lazerl5 */
	{193,YES},		/* C plungl5 */
	{207,YES},		/* C l3rock1 */
	{208,YES},		/* C plungl3 */
	{NAME_END,NO}
};

FLAG
Placed_Shootable(long name)
{

USHORT	j;

	for (j=0;sprites_shoot[j].name!=NAME_END;j++)
		if (sprites_shoot[j].name==name) return sprites_shoot[j].shootable;
	if (name<=90) return sprites_attribs[name].shootable;
	return NO;
}

/**************************************
*  RESTORE: NEW WORK (owner's decision)  *
**************************************/

/* How many words follow each command, for Can_Die below: the count each
   case of process_sprite reads. JUMPS: the last word is a place in the
   logic the command may go to. ENDS: the logic does not go on to the next
   command (jmp; wait, which stays until a hit moves the creature's logic;
   return, whose way on is the call's). */

#define	JUMPS	0x10
#define	ENDS	0x20

UBYTE	command_words[COMMAND_END]=
{
	[COMMAND_WAIT]=ENDS,
	[COMMAND_STOP]=0,
	[COMMAND_PAUSE]=1,
	[COMMAND_JMP]=1|JUMPS|ENDS,
	[COMMAND_CALL]=1|JUMPS,
	[COMMAND_RETURN]=ENDS,
	[COMMAND_THREAD]=1|JUMPS,
	[COMMAND_MOVERND]=2,
	[COMMAND_MOVEI]=2,
	[COMMAND_MOVEV]=2,
	[COMMAND_ADDI]=2,
	[COMMAND_ADDV]=2,
	[COMMAND_SUBI]=2,
	[COMMAND_SUBV]=2,
	[COMMAND_INDEX_ARRAY]=3,
	[COMMAND_SHL]=3,
	[COMMAND_SHR]=3,
	[COMMAND_MULI]=3,
	[COMMAND_MULV]=3,
	[COMMAND_DIVI]=3,
	[COMMAND_DIVV]=3,
	[COMMAND_NEG]=1,
	[COMMAND_ADD_CHILD]=2,
	[COMMAND_ADOPT_INRANGE_CHILD]=1,
	[COMMAND_QUEEN_ORDER]=1,
	[COMMAND_GET_QUEENS]=1,
	[COMMAND_GET_PARENTV]=1,
	[COMMAND_GET_CHILDV]=1,
	[COMMAND_GET_PLAYER_ID]=1,
	[COMMAND_IFEQI]=3|JUMPS,
	[COMMAND_IFEQV]=3|JUMPS,
	[COMMAND_IFNEI]=3|JUMPS,
	[COMMAND_IFNEV]=3|JUMPS,
	[COMMAND_IFLTI]=3|JUMPS,
	[COMMAND_IFLTV]=3|JUMPS,
	[COMMAND_IFLEI]=3|JUMPS,
	[COMMAND_IFLEV]=3|JUMPS,
	[COMMAND_IFGTI]=3|JUMPS,
	[COMMAND_IFGTV]=3|JUMPS,
	[COMMAND_IFGEI]=3|JUMPS,
	[COMMAND_IFGEV]=3|JUMPS,
	[COMMAND_FRAMEINC]=0,
	[COMMAND_UPDATE_XY]=0,
	[COMMAND_SET_FRAME]=1,
	[COMMAND_SETANIMRANGE]=2,
	[COMMAND_GETANIMRANGE]=2,
	[COMMAND_SETANIMNORMAL]=0,
	[COMMAND_SETANIMLEFT]=0,
	[COMMAND_SETANIMRIGHT]=0,
	[COMMAND_SETANIMUP]=0,
	[COMMAND_SETANIMDOWN]=0,
	[COMMAND_SETANIMWOUNDED]=0,
	[COMMAND_SETANIMDEATH]=0,
	[COMMAND_ANIMPLAYFORWARDONCE]=0,
	[COMMAND_ANIMPLAYBACKONCE]=0,
	[COMMAND_ANIMPLAYLOOPONCE]=0,
	[COMMAND_ANIMSPEED]=1,
	[COMMAND_ANIMSPEEDV]=1,
	[COMMAND_INRANGE]=2|JUMPS,
	[COMMAND_BG_COLLIDE]=1|JUMPS,
	[COMMAND_POINT_COLLISION]=3|JUMPS,
	[COMMAND_FIND_FLOOR]=3,
	[COMMAND_SCAN]=4,
	[COMMAND_DIE]=ENDS,
	[COMMAND_ARRAY]=1,			/* AND AS MANY AS THAT WORD SAYS */
	[COMMAND_ADD_PARTICLE]=6,
	[COMMAND_ON_HIT]=1|JUMPS,
	[COMMAND_SHOOTABLE_ON]=0,
	[COMMAND_SHOOTABLE_OFF]=0,
	[COMMAND_VISIBLE_ON]=0,
	[COMMAND_VISIBLE_OFF]=0,
	[COMMAND_SET_PRIORITY]=1,
	[COMMAND_GET_FLOOD_VAL]=1,
	[COMMAND_GET_VECTOR]=2,
	[COMMAND_GET_QUEEN_ID]=1,
	[COMMAND_GET_PARENT_ID]=1,
	[COMMAND_GET_POD_ID]=1,
	[COMMAND_GET_IDS_VAR]=3,
	[COMMAND_SET_IDS_VAR]=3,
	[COMMAND_GET_CHILD_ID]=1,
	[COMMAND_GET_ANGLE_TO_ID]=1,
	[COMMAND_SET_VEL]=3,
	[COMMAND_GET_DISTANCE_TO_ID]=2,
	[COMMAND_GET_ROTATION_TO_ID]=2,
	[COMMAND_ADD_TO_ANGLE]=1,
	[COMMAND_GET_ANGLE]=1,
	[COMMAND_SET_ANGLE]=1,
	[COMMAND_NEXT_NODE]=3,
	[COMMAND_GET_SPRITES_COORDS]=4,
	[COMMAND_ADD_POLO]=4,
	[COMMAND_ADD_LINE]=3,
	[COMMAND_GET_RET]=1,
	[COMMAND_SET_RET]=1|JUMPS,
	[COMMAND_GET_REF_POINT]=3,
	[COMMAND_OUTPUT_VAR]=2,
	[COMMAND_GET_LEVEL]=1,
	[COMMAND_SET_LEVEL]=1,
};

/* RESTORE: NEW WORK (owner's decision, delegated, 2026-11-10): can the logic
   that starts at "start" reach the command "die"? Every way on is followed:
   the next command, each jump, call, thread and on_hit. A creature counts
   toward completing a mission only if its logic can (LEVEL.C), and a hit
   bursts a creature only if its on_hit logic can (Hit_Sprite). */

#define	AI_WORDS	(sizeof(ai_data)/sizeof(ai_data[0]))
#define	MAX_TODO	64

FLAG
Can_Die(long start)
{

static	UBYTE	seen[(AI_WORDS+7)/8];
LONG	todo[MAX_TODO];
LONG	todos;
LONG	at;
LONG	command;
LONG	words;
ULONG	k;

	for (k=0;k<sizeof(seen);k++) seen[k]=0;

	todos=0;
	todo[todos++]=start;

	while (todos>0)
	{
		at=todo[--todos];

		while ((at>=0) && (at<AI_WORDS) && ((seen[at>>3] & (1<<(at&7)))==0))
		{
			seen[at>>3]|=(1<<(at&7));

			command=ai_data[at++];
			if (command==COMMAND_DIE) return YES;
			if ((command<0) || (command>=COMMAND_END)) break;

			words=command_words[command];
			if (command==COMMAND_ARRAY) at+=ai_data[at];

			if (((words & JUMPS)!=0) && (todos<MAX_TODO))
				todo[todos++]=ai_data[at+(words&15)-1];

			if ((words & ENDS)!=0) break;
			at+=(words&15);
		}
	}

	return NO;

}

/* A shot has hit sprite i (FIRE.C process_collisions). The creature's on_hit
   logic runs at the first hit, as NMS's process_collisions has it
   (move_ptr=on_hit_ptr). (2026-11-21: the BMF hit points, needed first
   until then, are no longer used, as in the PC and the Saturn: LEVEL.C.)

   RESTORE (owner's decision, delegated, 2026-11-10): with the July 1996
   logic a creature with no on_hit is left as it is. Before, NMS's ai_death
   ran for it; now only the logic NMS wrote decides what a hit does: the
   tentacles draw back, the mussels play their death frames and stay, the
   crystals do nothing (their ai_crystal begins with "wait"), tads,
   asteroids and plungers die. Since 2026-11-21 a creature whose logic
   sets no on_hit has ai_polo, as the PC and the Saturn give it (add_sprite):
   NMS's expanding ring (add_polo, GENERAL.C Add_Polo), then it stays still. */

UBYTE	hitsleft[MAX_SPRITES];			/* SET BY LEVEL.C: 1 (THE BMF'S HIT POINTS ARE NOT USED, SEE ABOVE) */

FLAG
Hit_Sprite(long i)				/* YES WHEN THE HIT DESTROYS IT */
{

	if (hitsleft[i]>1)
	{
		hitsleft[i]--;
		return NO;
	}

	if (sprites[i].on_hit_ptr==ai_none) return NO;

	sprites[i].move_ptr=sprites[i].on_hit_ptr;	/* NMS'S OWN */
	return Can_Die(sprites[i].on_hit_ptr);

}

/**************************************
*        PROCESS THE SPRITES          *
**************************************/

/* RESTORE: the interpreter for NMS's July 1996 logic. The July PS1 code
   had the February one: 37 commands, in one loop, and a thread was stored
   but never run. This is the PC version's (SOURCE/MAKEBACK/PROCESS.C
   process_sprites and process_sprite, 12 June 1996: "NEED TO IMPLEMENT
   PHILS CODE", SCHED.TXT), with the commands the Saturn port adds for the
   July logic (pause is in both; set_ret, get_distance_to_id, get_level,
   set_level from 360x-saturn LOGIC/PROCESS.C). Each case is NMS's code
   with the PS1's names; where the PS1 forces a change the case says so:

   - particles, the background test and the floor search call the PS1's
     own Add_Particle_Acceleration, Sprite_Collision and Collision_At;
   - sin_table/cos_table and asm_mul_div become the PS1's GetSin/GetCos
     (the same 1024 steps and scale) and a double multiply;
   - left out, their words skipped, because the PS1 tree has no code or
     data for them: scan (add_polo, the rings, was here until 2026-11-21:
     rebuilt from the PC, GENERAL.C Add_Polo),
     get_flood_val and get_vector (the PC's flood map), get_sprites_coords
     and get_ref_point (the PC's sprite data), output_var (debug),
     set_level (teleporters, none placed), and next_node's path (no .PAF
     file survives: every sprite has no path, the PC's own case). */

#define	AI_PI	3.14267214		/* (THE PC VERSION'S PI; CONTROL.H HAS ITS OWN) */

long	foster_parent;

#ifdef X360_TEST_TELE
/* RESTORE: NEW WORK, TEST BUILDS ONLY (X360_CFLAGS=-DX360_TEST_TELE=1 OR 2: THE ai_tele1 OR ai_tele2 ROUTINE): ONCE, AT GAME CYCLE 100 OF THE FIRST
   MISSION PLAYED, A TELEPORTER (A CREATURE OF THAT MISSION'S ART STANDS IN FOR THE PICTURE, NO VERSION HAS ONE FOR IT ON THE PS1) IS PUT AT THE CRAFT.
   FOR tests/gameplay/x360-tele.sh. NO MAP PLACES ONE. */
void
Test_Add_Tele(void)
{
	static int	done=0;
	LONG	n;

	if (done!=0) return;
	done=1;
	n=add_sprite(3,ACTION_NONE,craftx,crafty);
	if (n==-1) return;
	sprites[n].shootable=NO;
	sprites[n].move_ptr=(X360_TEST_TELE==1)?ai_tele1:ai_tele2;
}
#endif

void 
Process_Sprites(void)
{

LONG	i;

	Clear_Glow_Lines();			/* RESTORE: THE BEAM'S LINES ARE ADDED AGAIN EVERY CYCLE (PC MAKEBACK/MAIN.C: init_glow_list) */

	for(i=0;i<MAX_SPRITES;i++)
	{
		if(sprites[i].on==TRUE)
		{
			if(sprites[i].move_ptr==0)
			{
				sprites[i].move_ptr=ai_none;
			}
 			sprites[i].move_ptr=process_sprite(i,sprites[i].move_ptr);
 			sprites[i].thread_ptr=process_sprite(i,sprites[i].thread_ptr);
		}
	}
}

long
process_sprite(long i,long logic_index)
{

double	dtemp;
FLAG	end_slice;
LONG	*logic;
LONG	command;
LONG	temp1,temp2,temp3,temp4,temp5,temp6;

	end_slice=FALSE;
	logic=ai_data;

	while(end_slice==FALSE)
	{
		command=logic[logic_index++];
		switch(command)
		{
			case COMMAND_WAIT:
				logic_index--;
				end_slice=TRUE;
				break;
			case COMMAND_STOP:
				end_slice=TRUE;
				break;
			case COMMAND_PAUSE:
				temp1=logic[logic_index];
				if(sprites[i].obj[temp1]>0)
				{
					sprites[i].obj[temp1]--;
					end_slice=TRUE;
					logic_index--;
				}
				else logic_index++;
				break;
			case COMMAND_JMP:
				logic_index=logic[logic_index];
				break;
			case COMMAND_CALL:
				temp1=logic[logic_index++];
				sprites[i].ret_ptr=logic_index;
				logic_index=temp1;
				break;
			case COMMAND_RETURN:
				logic_index=sprites[i].ret_ptr;
				break;
			case COMMAND_THREAD:
				sprites[i].thread_ptr=logic[logic_index++];
				break;
			case COMMAND_MOVERND:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]=(rand()*temp1)>>15;
				break;
			case COMMAND_MOVEI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]=temp1;
				break;
			case COMMAND_MOVEV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]=sprites[i].obj[temp1];
				break;
			case COMMAND_ADDI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]+=temp1;
				break;
			case COMMAND_ADDV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]+=sprites[i].obj[temp1];
				break;
			case COMMAND_SUBI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]-=temp1;
				break;
			case COMMAND_SUBV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]-=sprites[i].obj[temp1];
				break;
			case COMMAND_INDEX_ARRAY:
				temp1=logic[logic_index++];			// var index
				temp1=sprites[i].obj[temp1];
				temp2=logic[logic_index++];			// array
				if(temp1>logic[temp2+1]) temp1=0;	// validate
				temp1=logic[temp2+temp1+2];
				temp2=logic[logic_index++];			// var dest
				sprites[i].obj[temp2]=temp1;
				break;
			case COMMAND_SHL:
				temp1=logic[logic_index++];		// shift
				temp2=logic[logic_index++];		// shift var source
				temp1=sprites[i].obj[temp2]<<temp1;				 
				temp2=logic[logic_index++];		// shift var dest
				sprites[i].obj[temp2]=temp1;
				break;
			case COMMAND_SHR:
				temp1=logic[logic_index++];		// shift
				temp2=logic[logic_index++];		// shift var source
				temp1=sprites[i].obj[temp2]>>temp1;
				temp2=logic[logic_index++];		// shift var dest
				sprites[i].obj[temp2]=temp1;
				break;
			case COMMAND_MULI:
				temp1=logic[logic_index++];		// mul1
				temp2=logic[logic_index++];		// mul2
				temp1=sprites[i].obj[temp2]*temp1;
				temp2=logic[logic_index++];		
				sprites[i].obj[temp2]=temp1;		// result
				break;
			case COMMAND_MULV:
				temp1=logic[logic_index++];		// mul1
				temp2=logic[logic_index++];		// mul2
				temp1=sprites[i].obj[temp2]*sprites[i].obj[temp1];
				temp2=logic[logic_index++];		
				sprites[i].obj[temp2]=temp1;		// result
				break;
			case COMMAND_DIVI:
				temp1=logic[logic_index++];		// divider
				temp2=logic[logic_index++];		// 
				if(temp1!=0) temp1=sprites[i].obj[temp2]/temp1;
				temp2=logic[logic_index++];		// result
				sprites[i].obj[temp2]=temp1;
				break;
			case COMMAND_DIVV:
				temp1=logic[logic_index++];		// divider
				temp2=logic[logic_index++];		// 
				if(sprites[i].obj[temp1]!=0)
					 temp1=sprites[i].obj[temp2]/sprites[i].obj[temp1];
				temp2=logic[logic_index++];		// result
				sprites[i].obj[temp2]=temp1;
				break;
			case COMMAND_NEG:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=-sprites[i].obj[temp1];
				break;
			case COMMAND_ADD_CHILD:			/* (THE SATURN PORT'S, WHICH STARTS THE CHILD'S LOGIC) */
				temp1=logic[logic_index++];		// sprites name
				temp2=logic[logic_index++];		// sprites pc
				temp3=add_sprite(temp1,ACTION_CYCLE,
										sprites[i].x,sprites[i].y);
				if(temp3!=-1)
				{
					sprites[i].child=temp3;
					sprites[temp3].move_ptr=temp2;
					sprites[temp3].parent=i;
				}
				break;
			case COMMAND_ADOPT_INRANGE_CHILD:
				temp1=logic[logic_index++];
				if(sprites[i].child==-1)
				{
					foster_parent=i;
					sprites[i].obj[obj_children]=0;
					adopt_inrange_child(i,temp1);
				}
				break;
			case COMMAND_QUEEN_ORDER:
				temp1=logic[logic_index++];
				temp2=i;
				while(sprites[temp2].child!=-1)
				{
					temp2=sprites[temp2].child;
					sprites[temp2].obj[temp1]=sprites[i].obj[temp1];
				}
				break;
			case COMMAND_GET_QUEENS:
				temp1=logic[logic_index++];
				temp2=sprites[i].queen;
				if(temp2!=-1)
					sprites[i].obj[temp1]=sprites[temp2].obj[temp1];
				break;
			case COMMAND_GET_PARENTV:
				temp1=logic[logic_index++];
				temp2=sprites[i].parent;
				if(temp2!=-1)
					sprites[i].obj[temp1]=sprites[temp2].obj[temp1];
				break;
			case COMMAND_GET_CHILDV:
				temp1=logic[logic_index++];
				temp2=sprites[i].child;
				if(temp2!=-1)
					sprites[i].obj[temp1]=sprites[temp2].obj[temp1];
				break;			/* RESTORE: THE PC FALLS INTO ifeqi HERE; THE SATURN PORT DOES NOT */
			case COMMAND_IFEQI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] == temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFEQV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] == sprites[i].obj[temp1])
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFNEI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] != temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFNEV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] != sprites[i].obj[temp1])
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFLEI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] <= temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFLEV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] <= sprites[i].obj[temp1])
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFLTI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] < temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFLTV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] < sprites[i].obj[temp1])
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFGTI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] > temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFGTV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] > sprites[i].obj[temp1])
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFGEI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] >= temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_IFGEV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] >= sprites[i].obj[temp1])
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
			case COMMAND_FRAMEINC:
 				if ( (sprites[i].framecounter--)<=0 )
				{
					sprites[i].framecounter=sprites[i].anim_delay_rate;
				 	temp1=sprites[i].frame;
					temp1++;
					if( temp1>=sprites[i].endframe ) temp1=sprites[i].startframe;
					sprites[i].frame=temp1;
				}
				break;
			case COMMAND_UPDATE_XY:
				if(sprites[i].nopath==NO)	/* RESTORE: SEE sprites_nopath */
				{
					sprites[i].x=sprites[i].obj[obj_x];
					sprites[i].y=sprites[i].obj[obj_y];
				}
				break;
			case	COMMAND_SET_FRAME:
				temp1=logic[logic_index++];
				sprites[i].frame=sprites[i].obj[temp1];
				break;
			case	COMMAND_SETANIMRANGE:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].startframe=sprites[i].obj[temp1];
				sprites[i].endframe=sprites[i].obj[temp2];
				break;
			case	COMMAND_GETANIMRANGE:		/* (THE SATURN PORT'S; THE JULY LOGIC USES IT) */
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp1]=sprites[i].startframe;
				sprites[i].obj[temp2]=sprites[i].endframe;
				break;
			case	COMMAND_SETANIMNORMAL:
				sprites[i].startframe=sprites[i].normal_startframe;
				sprites[i].endframe=sprites[i].normal_endframe;
				sprites[i].frame=sprites[i].startframe;
				break;
			case	COMMAND_SETANIMLEFT:
				sprites[i].startframe=sprites[i].left_startframe;
				sprites[i].endframe=sprites[i].left_endframe;
				sprites[i].frame=sprites[i].startframe;
				break;
			case	COMMAND_SETANIMRIGHT:
				sprites[i].startframe=sprites[i].right_startframe;
				sprites[i].endframe=sprites[i].right_endframe;
	 			sprites[i].frame=sprites[i].startframe;
				break;
			case	COMMAND_SETANIMUP:
				sprites[i].startframe=sprites[i].up_startframe;
				sprites[i].endframe=sprites[i].up_endframe;
		 		sprites[i].frame=sprites[i].startframe;
				break;
			case	COMMAND_SETANIMDOWN:
				sprites[i].startframe=sprites[i].down_startframe;
				sprites[i].endframe=sprites[i].down_endframe;
		 		sprites[i].frame=sprites[i].startframe;
				break;
			case	COMMAND_SETANIMWOUNDED:
				sprites[i].startframe=sprites[i].wounded_startframe;
				sprites[i].endframe=sprites[i].wounded_endframe;
		 		sprites[i].frame=sprites[i].startframe;
				break;
			case	COMMAND_SETANIMDEATH:
				sprites[i].startframe=sprites[i].death_startframe;
				sprites[i].endframe=sprites[i].death_endframe;
				sprites[i].frame=sprites[i].startframe;
				break;
			case COMMAND_ANIMPLAYFORWARDONCE:
				if ( (sprites[i].framecounter--)<=0 )
				{
					sprites[i].framecounter=sprites[i].anim_delay_rate;
				 	sprites[i].frame++;
				}

				if(sprites[i].frame<=(sprites[i].endframe-1))
				{ 
					logic_index--;
					end_slice=TRUE;
				}

				if(sprites[i].frame<sprites[i].startframe)
					sprites[i].frame=sprites[i].startframe;
				if(sprites[i].frame>=sprites[i].endframe)
					sprites[i].frame=sprites[i].endframe-1;
				if(sprites[i].frame<0) sprites[i].frame=0;
				break;
			case COMMAND_ANIMPLAYBACKONCE:
				if ( (sprites[i].framecounter--)<=0 )
				{
					sprites[i].framecounter=sprites[i].anim_delay_rate;
				 	sprites[i].frame--;
				}
				if(sprites[i].frame>=sprites[i].startframe)
				{ 
					logic_index--;
					end_slice=TRUE;
				}

				if(sprites[i].frame<sprites[i].startframe)
					sprites[i].frame=sprites[i].startframe;
				if(sprites[i].frame>=sprites[i].endframe)
					sprites[i].frame=sprites[i].endframe-1;

				if(sprites[i].frame<0) sprites[i].frame=0;
				break;
			case COMMAND_ANIMPLAYLOOPONCE:
				if ( (sprites[i].framecounter--)<=0 )
				{
					sprites[i].framecounter=sprites[i].anim_delay_rate;
				 	sprites[i].frame++;
				}

				if(sprites[i].frame<=(sprites[i].endframe-1))
				{ 
					logic_index--;
					end_slice=TRUE;
				}

				if(sprites[i].frame<sprites[i].startframe)
					sprites[i].frame=sprites[i].startframe;
				if(sprites[i].frame>=sprites[i].endframe)
					sprites[i].frame=sprites[i].startframe;
				if(sprites[i].frame<0) sprites[i].frame=0;
				break;
			case COMMAND_ANIMSPEED:
				sprites[i].anim_delay_rate=logic[logic_index++];
				break;
			case COMMAND_ANIMSPEEDV:
				temp1=logic[logic_index++];
				sprites[i].anim_delay_rate=sprites[i].obj[temp1];
				break;
			case COMMAND_INRANGE:
				temp1=sprites[i].x-craftx;
				temp2=sprites[i].y-crafty;
				if(logic[logic_index++] > ((temp1*temp1)+(temp2*temp2)) )
					logic_index=logic[logic_index];
				else logic_index++;
				break;
			case COMMAND_BG_COLLIDE:
				temp1=logic[logic_index++];
				/* RESTORE: live again, as in the PC version (it was commented
				   out in the July PS1 code). Sprite_Collision is the PS1's own
				   (SPRITE2.C), which tests the sprite's position, not each
				   line of its frame as the PC's does from its sprite data. */
				if(Sprite_Collision(seqdata[(sprites[i].name)],
											sprites[i].frame,
											sprites[i].x,
											sprites[i].y) )
				{
					logic_index=temp1;
				}
				break;
			case COMMAND_POINT_COLLISION:
				temp1=logic[logic_index++];		//x
				temp2=logic[logic_index++];		//y
				temp1=sprites[i].obj[temp1];
				temp2=sprites[i].obj[temp2];
				temp3=logic[logic_index++];
				/* RESTORE: Collision_At is the PS1's test of the same bit
				   (the PC reads coll_buf itself) */
				if(Collision_At(temp1,temp2)==TRUE) logic_index=temp3;
				break;
			case COMMAND_FIND_FLOOR:
				temp1=logic[logic_index++];		//x
				temp2=logic[logic_index++];		//y
				temp3=logic[logic_index++]; 		//max scan + return var
				sprites[i].obj[temp3]=find_floor(sprites[i].obj[temp1],
															sprites[i].obj[temp2],
															sprites[i].obj[temp3]);
				break;
			case COMMAND_SCAN:			/* RESTORE: LEFT OUT (THE PC'S screen_clip_point) */
				logic_index+=4;
				break;
			case COMMAND_DIE:
				end_slice=TRUE;
				logic_index--;
				if(sprites[i].dying==0)	sprites[i].dying=2;
				else
				{
					sprites[i].dying--;
					if(sprites[i].dying==0)
					{ 
						sprites[i].on=FALSE;

						// Fix linked list

					 	if(sprites[i].parent!=-1)
						{
							temp1=sprites[i].parent;
							sprites[temp1].child=sprites[i].child;
											
							if(sprites[i].child!=-1)
							{
								temp1=sprites[i].child;
								sprites[temp1].parent=sprites[i].parent;
							}
						}
						if(sprites[i].queen!=-1)
						{
							temp1=sprites[i].queen;
							sprites[temp1].obj[obj_children]--;
						}
						if(sprites[i].obj[obj_children]>0)
						{
							if(sprites[i].child!=-1)
							{
								for(temp1=0;temp1<MAX_SPRITES;temp1++)
								{
									if(sprites[temp1].queen==i)
										sprites[temp1].queen=sprites[i].child;
								}
								temp1=sprites[i].child;
								sprites[temp1].parent=-1;
								sprites[temp1].obj[obj_children]=sprites[i].obj[obj_children];
								/* (THE PC ALSO HANDS ON path AND node HERE: NO PATHS ON THE PS1) */
							}
						}
					}
				}
				break;
			case COMMAND_ARRAY:
				temp1=logic[logic_index++];
				logic_index+=temp1;
				break;
			case COMMAND_ADD_PARTICLE:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=logic[logic_index++];
				temp4=logic[logic_index++];
				temp5=logic[logic_index++];
				temp6=logic[logic_index++];
				Add_Particle_Acceleration(sprites[i].obj[temp1],	/* (PARTICLE.C, THE PS1'S add_particle_acceleration) */
												sprites[i].obj[temp2],
												sprites[i].obj[temp3],
												sprites[i].obj[temp4],
												sprites[i].obj[temp5],														
												sprites[i].obj[temp6]);
				break;
			case COMMAND_ON_HIT:
				sprites[i].on_hit_ptr=logic[logic_index++];
				break;
			case COMMAND_SHOOTABLE_ON:
				sprites[i].shootable=TRUE;
				break;
			case COMMAND_SHOOTABLE_OFF:
				sprites[i].shootable=FALSE;
				break;
			case COMMAND_VISIBLE_ON:
				sprites[i].visible=TRUE;
				break;
			case COMMAND_VISIBLE_OFF:		 
				sprites[i].visible=FALSE;
				break;
			case COMMAND_SET_PRIORITY:
				temp1=logic[logic_index++];
				sprites[i].priority=temp1;		 
				break;
			case COMMAND_GET_FLOOD_VAL:		/* RESTORE: LEFT OUT (THE PC'S FLOOD MAP) */
				logic_index+=1;
				break;
			case COMMAND_GET_VECTOR:		/* RESTORE: LEFT OUT (THE PC'S FLOOD MAP) */
				logic_index+=2;
				break;
			case COMMAND_GET_QUEEN_ID:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=sprites[i].queen;
				break;
			case COMMAND_GET_PARENT_ID:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=sprites[i].parent;
				break;
			case COMMAND_GET_CHILD_ID:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=sprites[i].child;
				break;
			case COMMAND_GET_PLAYER_ID:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=craft;
				break;
			case COMMAND_GET_POD_ID:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=pod_no;
				break;
			case COMMAND_GET_IDS_VAR:
				temp1=logic[logic_index++];		// ID
				temp2=logic[logic_index++];		// VAR wanted
				temp3=logic[logic_index++];		// VAR dest
				sprites[i].obj[temp3]=sprites[(sprites[i].obj[temp1])].obj[temp2];
				break;
			case COMMAND_SET_IDS_VAR:
				temp1=logic[logic_index++];		// Var source
				temp2=logic[logic_index++];		// ID
				temp3=logic[logic_index++];		// VAR dest
				sprites[(sprites[i].obj[temp2])].obj[temp3]=sprites[i].obj[temp1];
				break;
			case COMMAND_GET_ANGLE_TO_ID:
				temp1=logic[logic_index++];
				temp1=sprites[i].obj[temp1];
				if(temp1!=-1)
				{
					sprites[i].angle=atan2(sprites[temp1].x-sprites[i].x,
												  sprites[temp1].y-sprites[i].y);
					dtemp=sprites[i].angle;
					if(dtemp<0) dtemp=(AI_PI*2)+dtemp;
					sprites[i].obj[obj_angle]=(dtemp*1024)/(AI_PI*2);
				}
				break;
			case COMMAND_SET_VEL:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=logic[logic_index++];
				/* RESTORE: the PC's asm_mul_div(sin_table[angle],speed,32768)
				   with the PS1's GetSin/GetCos and a double for the 64-bit
				   product */
				dtemp=sprites[i].obj[temp1];
				sprites[i].obj[temp2]=(dtemp*GetSin((sprites[i].obj[obj_angle])&1023))/32768;
				sprites[i].obj[temp3]=(dtemp*GetCos((sprites[i].obj[obj_angle])&1023))/32768;
				break;
			case COMMAND_GET_DISTANCE_TO_ID:	/* (THE SATURN PORT'S) */
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=sprites[i].x-sprites[(sprites[i].obj[temp1])].x;
				temp4=sprites[i].y-sprites[(sprites[i].obj[temp1])].y;
				temp5=(temp3*temp3)+(temp4*temp4);
				sprites[i].obj[temp2]=sqr_root(temp5);
				break;
			// Wont work anymore...
			case COMMAND_GET_ROTATION_TO_ID:
				dtemp=sprites[i].angle;
				while(dtemp<0) dtemp+=AI_PI*2;
				while(dtemp>(AI_PI*2)) dtemp-=AI_PI*2;

				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp1=sprites[i].obj[temp1];
				dtemp=atan2(sprites[temp1].x-sprites[i].x,
						   	sprites[temp1].y-sprites[i].y);
				dtemp=sprites[i].angle-dtemp;
				if(dtemp<0)
				{
					dtemp=-dtemp;
					if(dtemp<AI_PI) temp1=1;
					else temp1=-1;
				}
				else
				{
					if(dtemp<AI_PI) temp1=-1;
					else temp1=1;
				}
				sprites[i].obj[temp2]=temp1;
				break;
			case COMMAND_ADD_TO_ANGLE:
				temp1=logic[logic_index++];
				temp1=sprites[i].obj[temp1];
				sprites[i].obj[obj_angle]+=temp1*4;
				sprites[i].obj[obj_angle]&=1023;
				break;
			case COMMAND_GET_ANGLE:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=sprites[i].obj[obj_angle];
				break;
			case COMMAND_SET_ANGLE:
				temp1=logic[logic_index++];
				sprites[i].angle=(sprites[i].obj[temp1]*AI_PI*2)/256;
				sprites[i].obj[obj_angle]=(temp1&0xff)*4;
				break;
			case COMMAND_NEXT_NODE:
				temp4=logic[logic_index++];
				temp5=logic[logic_index++];
				temp6=logic[logic_index++];
				/* RESTORE: the PC's case for a sprite with no path, which on
				   the PS1 is every sprite (no path data survives) */
				sprites[i].obj[temp4]=-1;			
				sprites[i].obj[temp5]=-1;			
				sprites[i].obj[temp6]=-1;
				break;
			case COMMAND_GET_SPRITES_COORDS:	/* RESTORE: LEFT OUT (THE PC'S SPRITE DATA) */
				logic_index+=4;
				break;
			case COMMAND_ADD_POLO:			/* RESTORE: completion, rebuilt from the PC version (owner's decision 2026-11-21; PC MAKEBACK/GLOW.C:159 add_polo): A RING, OUTER AND INNER RADIUS, CENTRE x, y, FROM FOUR VARIABLES (GENERAL.C Add_Polo) */
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=logic[logic_index++];
				temp4=logic[logic_index++];
				Add_Polo(sprites[i].obj[temp1],sprites[i].obj[temp2],sprites[i].obj[temp3],sprites[i].obj[temp4]);
				break;
			case COMMAND_ADD_LINE:			/* RESTORE: completion, rebuilt from the PC version (PC MAKEBACK/PROCESS.C:1254-1261): ONE LINE OF THE LASER'S BEAM, x, y AND LENGTH FROM THREE VARIABLES (GENERAL.C Add_Glow_Line) */
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=logic[logic_index++];
				Add_Glow_Line(sprites[i].obj[temp1],sprites[i].obj[temp2],sprites[i].obj[temp3]);
				break;
			case COMMAND_GET_RET:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=sprites[i].ret_ptr;
				break;
			case COMMAND_SET_RET:			/* (THE SATURN PORT'S) */
			 	sprites[i].ret_ptr=logic[logic_index++];
				break;
			case COMMAND_GET_REF_POINT:		/* RESTORE: LEFT OUT (THE PC'S SPRITE DATA) */
				logic_index+=3;
				break;
			case COMMAND_OUTPUT_VAR:		/* RESTORE: LEFT OUT (DEBUG) */
				logic_index+=2;
				break;
			case COMMAND_GET_LEVEL:			/* (THE SATURN PORT'S) */
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=level;
				break;
			case COMMAND_SET_LEVEL:			/* RESTORE: COMPLETION, THE SATURN'S (LOGIC/PROCESS.C:1559 AI_COMMAND_SET_LEVEL: level_no=n; change_level=TRUE; escape=TRUE) */
				temp1=logic[logic_index++];
				if ((temp1>=0) && (temp1<9))
				{
					newlevel=temp1;
					changelevel=YES;
				}
				end_slice=TRUE;
				break;
	
			}
		}

	return logic_index;
}

/* Make a linked list of inrange sprites */

void adopt_inrange_child(long father,long range)
{
	long i;
	long x,y;

	for(i=0;i<MAX_SPRITES;i++)
	{
		if( (sprites[i].on==TRUE) && (i!=father) )
		{
			if(sprites[father].name==sprites[i].name)
			{
				if( (sprites[i].parent==-1) && (sprites[i].child==-1) )
				{
					x=sprites[father].x-sprites[i].x;
					y=sprites[father].y-sprites[i].y;
					if( ((x*x)+(y*y)) < range )
					{
						sprites[i].parent=father;
						sprites[i].queen=foster_parent;
						sprites[father].child=i;
						sprites[foster_parent].obj[obj_children]++;
						adopt_inrange_child(i,range);
						return;
					}
				}
			}
		}
	}
}

/* (THE PC VERSION'S, SPRITE.C; collision_at IS THE PS1'S Collision_At) */

long find_floor(long x,long y,long max_scan)
{
	long height;
	
	height=0;
	if( Collision_At(x,y)==TRUE )
	{
		// scan up
		while(max_scan>0)
		{
			height--;
			if( Collision_At(x,y+height)==FALSE ) break;
			max_scan--;
		}
	}
	else
	{
		// scan down
		while(max_scan>0)
		{
			height++;
			if( Collision_At(x,y+height)==TRUE ) break;
			max_scan--;
		}
		height--;
	}
	return height;
}

/* Integer Square Root (Approximation Method) (THE PC VERSION'S, GENERAL.C) */

unsigned long sqr_root(unsigned long val)
{
	int	i;
	unsigned long	upper,lower,mid;
	unsigned long	sqr;

	upper=val;
	lower=0;

	if(upper>65536) upper=65536;

	for(i=0;i<16;i++)
	{
		mid=lower+((upper-lower)>>1);
		sqr=mid*mid;
		if(sqr==val) break;
		if(sqr>val) upper=mid;
		else lower=mid;
	}
	return(mid);
}

/**************************************
*        CLEAR THE SPRITE LIST        *
**************************************/

void 
Clear_Sprites(void)
{

USHORT	i;

	for(i=0;i<MAX_SPRITES;i++)
	{
		sprites[i].on=FALSE;
	}
}

/**************************************
*   ADD A SPRITE TO THE SPRITE LIST   *
**************************************/

LONG
add_sprite(int name,int action,int x,int y)
{

int		i,j;

	for(i=0;i<MAX_SPRITES;i++)		/* FIND A FREE SPRITE */
	{
		if(sprites[i].on==FALSE) break;
	}
	
	if(i==MAX_SPRITES)				/* CHECK FOR END OF LIST */
	{
		return (-1);
	}
	else
	{
		switch (action)			/* ASSIGN AI DATA */
		{
			case ACTION_NONE:
				sprites[i].move_ptr=ai_none;
				break;
			case ACTION_CYCLE:
				sprites[i].move_ptr=ai_cycle;
				break;
			case ACTION_PINGPONG:
				sprites[i].move_ptr=ai_pingpong;
				break;
			case ACTION_PLAYONCE:
				sprites[i].move_ptr=ai_explode;
				break;
			default:
				sprites[i].move_ptr=ai_cycle;
				break;
		}

		j=0;						/* FIND ACTION FOR SPRITE */
		while(sprites_actions[j].name!=NAME_END)
		{
			if(name==sprites_actions[j].name)
			{
				sprites[i].move_ptr=sprites_actions[j].action;
				break;
			}

			j++;
		}

		/* RESTORE: NEW WORK (owner's decision, 2026-11-14: one pod in Mission 6, at 585,441). Mission 6's one podtemp IS AT 0,0, A MAP EDITOR'S PLACEHOLDER, NOT A POD:
		   IT STOOD STILL (ai_cycle) AT THE CORNER. THAT PLACEMENT IS THE ONE POD NOW: PUT AT 585,441 AS IT IS ADDED, WITH ai_pod, THE TABLE'S ROUTINE FOR ITS NAME. IT IS THE MAP'S OWN
		   add_sprite CALL, NOT ANOTHER ONE: A SECOND CALL TAKES TWO rand() VALUES AND CHANGES THE RANDOM SEQUENCE THE MISSION'S ROCKS USE (Add_Level_Pod, CONTROL.C; docs/DECISIONS.md) */
		if ((level==5) && (name==171) && (x==0) && (y==0))
		{
			x=585;
			y=441;
		}

		/* RESTORE: the fields the July 1996 logic uses, set as the PC
		   version's add_sprite sets them (SOURCE/MAKEBACK/PROCESS.C). Not
		   taken then: on_hit_ptr=ai_polo; taken since 2026-11-21, with add_polo
		   rebuilt (GENERAL.C Add_Polo), below. */
		sprites[i].thread_ptr=ai_none;
		sprites[i].queen=-1;
		sprites[i].child=-1;
		sprites[i].parent=-1;
		sprites[i].obj[obj_children]=0;
		sprites[i].obj[obj_angle]=0;		/* (THE SATURN PORT'S add_sprite) */
		sprites[i].obj[obj_angle2]=0;
		sprites[i].angle=0;
		sprites[i].normal_startframe=0;
		sprites[i].normal_endframe=seqdata[name].frames-2;

		sprites[i].nopath=NO;
		j=0;
		while(sprites_nopath[j]!=NAME_END)
		{
#ifndef X360_TEST_FLOCKFLY				/* RESTORE: TEST BUILDS ONLY: -DX360_TEST_FLOCKFLY APPLIES THE MOVES */
			if(name==sprites_nopath[j]) sprites[i].nopath=YES;
#endif
			j++;
		}

		sprites[i].hit=FALSE;		/* INSERT SPRITE DATA */
		sprites[i].on=TRUE;
		sprites[i].visible=TRUE;
		sprites[i].shootable=sprites_attribs[name].shootable;
		sprites[i].name=name;
		sprites[i].action=action;
		sprites[i].on_hit_ptr=ai_polo;	/* RESTORE: completion (owner's decision 2026-11-21): AS THE PC (MAKEBACK/PROCESS.C:1676) AND THE SATURN
						   (LOGIC/PROCESS.C:197) SET IT: A HIT ON A CREATURE WHOSE SCRIPT GIVES NO on_hit OF ITS OWN PLAYS NMS'S ai_polo, AN EXPANDING
						   RING (add_polo, GENERAL.C Add_Polo) AFTER WHICH THE CREATURE STAYS STILL. WAS ai_none: SUCH A HIT DID NOTHING VISIBLE. */
		sprites[i].x=x;
		sprites[i].y=y;
		sprites[i].obj[obj_x]=x;
		sprites[i].obj[obj_y]=y;
		sprites[i].dx=0;
		sprites[i].dy=0;
		sprites[i].anim_delay_rate=4;
		sprites[i].frame=0;
		sprites[i].dying=0;
		hitsleft[i]=0;				/* RESTORE: NEW WORK (owner's decision) */
		sprites[i].dir=rand()&3;

		sprites[i].startframe=0;
		sprites[i].endframe=seqdata[name].frames-2;

		if(name==NAME_SPIKE)		/* BODGE THE SPIKES */
		{
			sprites[i].framecounter=8+((rand()*128)>>15);
		}
		else 
		{
			sprites[i].framecounter=0;
		}

		sprites[i].framecounter+=rand()&3;

		return (i);				/* RETURN SPRITE NUMBER */
	}
}

