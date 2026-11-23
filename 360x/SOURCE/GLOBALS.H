
#ifndef	__H_GLOBALS
#define	__H_GLOBALS

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Pointers to function types */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define	ext	extern

typedef	unsigned char	flag;	    // unsigned 8-bit quantity

#define MAX_SPRITES 256

flag 		burst_scroll;
long 		burst_x,burst_y;
long 		bank_changes;
long 		rope_length;
long 		ship_mass;
long 		ball_mass;
long 		gravity_factor;
double		rope_max;
double		rope_min;

//long		edge_offs[1];
//long		edge_array[1];

long  	craftx,crafty;

typedef struct {
	int	*map;
	char	*data;
	long	frames;
} sprite_sequence ;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Structure */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

enum {
		obj_x,
		obj_y,
		obj_temp1,
		obj_temp2,
		obj_temp3,
		obj_temp4,
		obj_temp5,
		obj_temp6,
		obj_temp7,
		obj_temp8,
		obj_temp9,		/* RESTORE: THE VARIABLES THE JULY 1996 LOGIC */
		obj_temp10,		/* DECLARES (AI.DAT, "declare:"), IN ITS ORDER, */
		obj_temp11,		/* AS THE PC VERSION'S GLOBALS.H HAS THEM */
		obj_temp12,
		obj_temp13,
		obj_temp14,
		obj_temp15,
		obj_temp16,
		obj_children,
		obj_angle,
		obj_angle2,
		obj_end
};

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Names */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

enum 	{
			NAME_NULL=0,
			NAME_CSHIP,
			NAME_MISSILE,
			NAME_FLOPPY,
			NAME_bouncer3,
			NAME_s,
			NAME_flawa,
			NAME_nuray,

			NAME_chisler,
			NAME_stship2,
			NAME_0pod,
			
			NAME_flop2,
			NAME_spiral3,
			NAME_g2,
			NAME_fork01,
			NAME_fork02,
			NAME_flawi,
			NAME_mrspit,
			NAME_jshel,
			NAME_tomi,
			NAME_float,

			NAME_bug,
			NAME_star,
			NAME_starfsh,
			NAME_turret1,
			NAME_acidspt,
			NAME_chapie,
			NAME_twinspin,
			NAME_l,
			NAME_survrs,

			NAME_FUEL,
			NAME_EXPLOSION,
			NAME_LEAPER,
			NAME_POD,
			NAME_SHIELD,
			NAME_LASER,
			NAME_GRAVITY,

			NAME_SPIKE,
			NAME_SHELL,
	
			NAME_FLOATER,
			NAME_HAIR1,
			NAME_BOUNCER1,
			NAME_BOUNCER2,
			NAME_FORK01,
			NAME_FORK02,
			NAME_BFIRE,
			NAME_POD0,
			NAME_FLOAT,
			NAME_SCATTER,
			NAME_CLAW,
			NAME_FROG,
			NAME_FUZZ,
			NAME_RCK1,
			NAME_RCK2,
			NAME_RCK3,
			NAME_BUG,
			NAME_STAR,
			NAME_TURRET,
			NAME_SPIRAL,
			NAME_TENTAC1,
			NAME_TENTAC2,
			NAME_MINE,
			NAME_MUSSEL,
			NAME_RCKMORPH,
			NAME_CHAPIE,
			NAME_STARFSH,
			NAME_ACIDSPT,

			NAME_BORDSML2,
			NAME_MISSWIRE,
			NAME_BARS01,
			NAME_SHIPWIRE,

			NAME_fflop,
			NAME_l2c1,
			NAME_l2c2,
			NAME_l2c3,
			NAME_l2c4,
			NAME_l2c5,
			NAME_block1,
			NAME_block2,
			NAME_block3,
			NAME_block4,
			NAME_windtun,
			NAME_bloba,
			NAME_tad,
			NAME_spfire,
			NAME_l2rock1,

			NAME_bordsml,
			NAME_bordsml3,
			NAME_overlay,
			NAME_overly2,
			NAME_shipwir2,

			NAME_tubel1,
			NAME_chapl1,
			NAME_vinel11,
			NAME_vines12,
			NAME_vine13,
			NAME_vine14,
			NAME_vine15,
			NAME_vine16,
			NAME_s_l1c1,
			NAME_s_l1c2,
			NAME_s_l1c3,
			NAME_s_l1c4,
			NAME_s_l1c5,
			NAME_tonsl1,
			NAME_l1c1,
			NAME_l1c2,
			NAME_l1c3,
			NAME_l1c4,
			NAME_l1c5,
			NAME_lazerl1,
			NAME_msspit,
			NAME_lrtad,

			NAME_vinel21,
			NAME_vinel22,
			NAME_vinel23,
			NAME_vinel24,
			NAME_vinel26,
			NAME_s_l2c1,
			NAME_s_l2c2,
			NAME_s_l2c3,
			NAME_s_l2c4,
			NAME_s_l2c5,
			NAME_l2tentl,
			NAME_l2tentr,
			NAME_plungl2,
			NAME_chapl2,
			NAME_lazerl2,
			NAME_vtestl2,

			NAME_vtestl4,
			NAME_vinl41,
			NAME_vinel42,
			NAME_vinel43,
			NAME_vinel44,
			NAME_vinel46,
			NAME_s_l4c1,
			NAME_s_l4c2,
			NAME_s_l4c3,
			NAME_s_l4c4,
			NAME_s_l4c5,
			NAME_tubes,
			NAME_l4rock1,
			NAME_l4c1,
			NAME_l4c3,
			NAME_l4c4,
			NAME_l4c5,
			NAME_plungl4,

			NAME_vinel61,
			NAME_vinel62,
			NAME_vinel63,
			NAME_vinel64,
			NAME_vinel65,
			NAME_vinel66,
			NAME_s_l6c1,
			NAME_s_l6c2,
			NAME_s_l6c3,
			NAME_s_l6c4,
			NAME_s_l6c5,
			NAME_l6c1,
			NAME_l6c2,
			NAME_l6c3,
			NAME_l6c4,
			NAME_l6c5,
			NAME_l61bt,
			NAME_l62bt,
			NAME_l63bt,
			NAME_l64bt,
			NAME_l65bt,
			NAME_l6rock1,
			NAME_plungl6,
			NAME_lazerl6,
			NAME_podtemp,
			NAME_chapl6,
			NAME_vtestl6,

			NAME_vinel51,
			NAME_vinel52,
			NAME_vinel53,
			NAME_vinel54,
			NAME_vinel56,
			NAME_s_l5c1,
			NAME_s_l5c2,
			NAME_s_l5c3,
			NAME_s_l5c4,
			NAME_s_l5c5,
			NAME_l5rock1,
			NAME_vtestl5,
			NAME_l5c1,
			NAME_l5c2,
			NAME_l5c3,
			NAME_l5c4,
			NAME_l5c5,
			NAME_lazerl5,
			NAME_chapl5,
			NAME_plungl5,
			NAME_l51bt,
			NAME_l52bt,
			NAME_l53bt,
			NAME_l54bt,

			NAME_vinel31,
			NAME_vinel33,
			NAME_vinel34,
			NAME_vinel36,
			NAME_s_l3c1,
			NAME_s_l3c2,
			NAME_s_l3c3,
			NAME_s_l3c4,
			NAME_s_l3c5,
			NAME_l3rock1,
			NAME_plungl3,
			NAME_mussl3,
			NAME_chapl3,
			NAME_l3c1,
			NAME_l3c2,
			NAME_l3c3,
			NAME_l3c4,
			NAME_l3c5,
			NAME_l31bt,
			NAME_l32bt,
			NAME_lazer3,
			NAME_vtest3l,

			NAME_l7c1,
			NAME_l7c2,
			NAME_l7c3,
			NAME_l7c4,
			NAME_l7c5,
			NAME_l71bt,
			NAME_l72bt,
			NAME_l73bt,
			NAME_l74bt,

			NAME_l8c1,
			NAME_l81bt,
			NAME_l82bt,
			NAME_l83bt,
			NAME_l84bt,
	
			NAME_l9c1,
			NAME_l91bt,
			NAME_l92bt,
			NAME_l93bt,
			NAME_l94bt,

			NAME_donkey,
			NAME_kong,

			NAME_BUCK01,
			NAME_BUCK02,
			NAME_EBALL,
			NAME_AGMINE,

			/* RESTORE: NAMES FOR THE SATURN PORT'S SPRITES THE PS1 BUILD ADDS (LOADDATA.C),
			   AFTER EVERY NAME A LEVELn.BMF NUMBER CAN BE (0 TO 240). THEY ARE THE SATURN
			   PORT'S: LOGIC/FIRECODE.C STARTS exp4, exp7 AND exp6 WHERE A MISSILE HITS
			   A WALL; THE LASER SHOT IS THE spfire OF THE PS1 FIRE.C (A PS1 NAME
			   ALREADY, BUT ONE A BMF NUMBER ALSO HAS) */
			NAME_exp4,
			NAME_exp6,
			NAME_exp7,
			NAME_laserfire,

			/* RESTORE: THE CHAIN WEAPON'S SHOT (docs/DECISIONS.md). THE PC NAMES IT NAME_CHAINB (MAKEBACK/FIRE.C:175); IT IS NEVER PLACED IN A MAP, SO ONE ENTRY AT THE END
			   RENUMBERS NO PLACEMENT */
			NAME_CHAINB,

			NAME_END
	   };

typedef struct
{
	long  obj[obj_end];
	long	move_ptr;
	long	thread_ptr;
	long	ret_ptr;
	long  on_hit_ptr;
	long  child;
	/* RESTORE: the fields the July 1996 logic uses, from the PC version's
	   sprite structure (SOURCE/MAKEBACK/GLOBALS.H). The frame ranges are
	   short here, not long: 2 MB (platform). */
	long	parent;
	long	queen;
	double	angle;
	short	normal_startframe,normal_endframe;
	short	death_startframe,death_endframe;
	short	wounded_startframe,wounded_endframe;
	short	up_startframe,up_endframe;
	short	down_startframe,down_endframe;
	short	left_startframe,left_endframe;
	short	right_startframe,right_endframe;
	flag	nopath;			/* RESTORE: SEE PROCESS.C sprites_nopath */
	flag	on;
	flag	onscreen;
	flag	hit;
	flag	visible;
	flag	clipped;
	flag	shootable;
	long	x1,y1,x2,y2;
	long	priority;
	int	name;
	int	action;
	int	lastaction;
	int	x;
	int	y;
	int	x8;
	int	y8;
	int	dx;
	int	dy;
	int	cos;
	int	sin;	
	int	dir;
	int	frame;
	long	startframe;
	long	endframe;
	int	framecounter;
	int	anim_delay_rate;
	int	flashing;
	int	dying;
	int	buf_x[2];
	int	buf_y[2];
	int	buf_frame[2];
	flag	buf_rem[2];

} spritestruct;

ext spritestruct sprites[MAX_SPRITES];

spritestruct sprites[MAX_SPRITES];


ext	flag bursting;
ext	flag burst_scroll;
ext	long burst_x,burst_y;
ext	long rope_length;
ext	long ship_mass;
ext	long ball_mass;
ext	long gravity_factor;
ext	double rope_max;
ext	double rope_min;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Actions */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

enum	{
			ACTION_NONE,
			ACTION_CYCLE,
			ACTION_RND_CYCLE,
			ACTION_PLAYONCE,
			ACTION_PINGPONG,
			ACTION_PICKUP_FUEL,
			ACTION_PICKUP_SHIELD,
			ACTION_PICKUP_GRAVITY,
			ACTION_POD_WAIT,
			ACTION_PICKUP_POD,
			ACTION_FALL,
			ACTION_HOME,
			ACTION_FLOAT,
			ACTION_ASTEROID,
			ACTION_HOMELINE,
			ACTION_FLASH,
			ACTION_SPIRAL,
			ACTION_GRAVITY,
			ACTION_DYING
		};

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE File Names */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

typedef	struct
{
			char fname[8+1];

} fnames_list;

ext		long fsize;
ext		fnames_list *sprite_fnames;

enum	{
			COMMAND_WAIT,
			COMMAND_STOP,
			COMMAND_CALL,
			COMMAND_RETURN,
			COMMAND_THREAD,
			COMMAND_MOVERND,
			COMMAND_MOVEI,
			COMMAND_MOVEV,
			COMMAND_ADDI,
			COMMAND_ADDV,
			COMMAND_SUBI,
			COMMAND_SUBV,
			COMMAND_INDEX_ARRAY,
			COMMAND_SHL,
			COMMAND_SHR,
			COMMAND_ADD_CHILD,
			COMMAND_IFEQI,
			COMMAND_IFEQV,
			COMMAND_IFNEI,
			COMMAND_IFNEV,
			COMMAND_FRAMEINC,
			COMMAND_UPDATE_XY,
			COMMAND_ANIMPLAYFORWARDONCE,
			COMMAND_ANIMPLAYBACKONCE,
			COMMAND_ANIMSPEED,
			COMMAND_ANIMSPEEDV,
			COMMAND_INRANGE,
			COMMAND_BG_COLLIDE,
			COMMAND_DIE,
			COMMAND_JMP,
			COMMAND_ARRAY,
			COMMAND_ADD_PARTICLE,
			COMMAND_ON_HIT,
			COMMAND_SHOOTABLE_ON,
			COMMAND_SHOOTABLE_OFF,
			COMMAND_VISIBLE_ON,
			COMMAND_VISIBLE_OFF,

			/* RESTORE: THE COMMANDS THE JULY 1996 LOGIC ADDS (AI.DAT OF 17 JULY;
			   THE PC VERSION'S PROCESS.C AND THE SATURN PORT'S COMMAND.H) */
			COMMAND_PAUSE,
			COMMAND_MULI,
			COMMAND_MULV,
			COMMAND_DIVI,
			COMMAND_DIVV,
			COMMAND_NEG,
			COMMAND_ADOPT_INRANGE_CHILD,
			COMMAND_QUEEN_ORDER,
			COMMAND_GET_QUEENS,
			COMMAND_GET_PARENTV,
			COMMAND_GET_CHILDV,
			COMMAND_GET_PLAYER_ID,
			COMMAND_IFLTI,
			COMMAND_IFLTV,
			COMMAND_IFLEI,
			COMMAND_IFLEV,
			COMMAND_IFGTI,
			COMMAND_IFGTV,
			COMMAND_IFGEI,
			COMMAND_IFGEV,
			COMMAND_SET_FRAME,
			COMMAND_SETANIMRANGE,
			COMMAND_GETANIMRANGE,
			COMMAND_SETANIMNORMAL,
			COMMAND_SETANIMLEFT,
			COMMAND_SETANIMRIGHT,
			COMMAND_SETANIMUP,
			COMMAND_SETANIMDOWN,
			COMMAND_SETANIMWOUNDED,
			COMMAND_SETANIMDEATH,
			COMMAND_ANIMPLAYLOOPONCE,
			COMMAND_POINT_COLLISION,
			COMMAND_FIND_FLOOR,
			COMMAND_SCAN,
			COMMAND_SET_PRIORITY,
			COMMAND_GET_FLOOD_VAL,
			COMMAND_GET_VECTOR,
			COMMAND_GET_QUEEN_ID,
			COMMAND_GET_PARENT_ID,
			COMMAND_GET_POD_ID,
			COMMAND_GET_IDS_VAR,
			COMMAND_SET_IDS_VAR,
			COMMAND_GET_CHILD_ID,
			COMMAND_GET_ANGLE_TO_ID,
			COMMAND_SET_VEL,
			COMMAND_GET_DISTANCE_TO_ID,
			COMMAND_GET_ROTATION_TO_ID,
			COMMAND_ADD_TO_ANGLE,
			COMMAND_GET_ANGLE,
			COMMAND_SET_ANGLE,
			COMMAND_NEXT_NODE,
			COMMAND_GET_SPRITES_COORDS,
			COMMAND_ADD_POLO,
			COMMAND_ADD_LINE,
			COMMAND_GET_RET,
			COMMAND_SET_RET,
			COMMAND_GET_REF_POINT,
			COMMAND_OUTPUT_VAR,
			COMMAND_GET_LEVEL,
			COMMAND_SET_LEVEL,
			COMMAND_END
		};



sprite_sequence seqdata[NAME_END];
extern	sprite_sequence seqdata[NAME_END];

#endif

