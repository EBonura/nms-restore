/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Saturn Globals */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#ifndef	ext
#define	ext	extern
#endif

//#define VIEW_COLLISION ON

ext char 		filename[128];

#define	NEWLOGIC

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Structures */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Defines */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Flags */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

ext	flag music_on;
ext	long selected_cdtrack;
ext	long last_cdtrack;
ext	long track_pos;

ext	flag escape;
ext	flag safe_mode;
ext	flag use_UNIVBE;
ext	flag UNIVBE_installed;
ext	flag bursting;
ext	flag burst_scroll;
ext	long burst_x,burst_y;
ext 	long buf;
ext	long bank_changes;
ext	long rope_length;
ext	long ship_mass;
ext	long ball_mass;
ext	long gravity_factor;
ext	double rope_max;
ext	double rope_min;

ext	long left_scroll;
ext	long right_scroll;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Hash Tables */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

ext	long	y_mod[1];
ext	long	y_mod640[1];
ext	char  bak_screen[1];

ext	long	edge_offs[1];
ext	long	edge_array[1];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Arrays */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

ext	long	scroll_x,scroll_y;
ext	long	old_scroll_x,old_scroll_y;
ext	long	logical_screen;
ext	long	bufx[2],bufy[2];
ext	char	*blk_buf;
ext	long	*map;

extern	long		particle_list_mask[];
extern	long		particle_list_x[];
extern	long		particle_list_y[];
extern	long		particle_list_dx[];
extern	long		particle_list_dy[];
extern	long		particle_list_ddx[];
extern	long		particle_list_ddy[];
extern	long		particle_list_counter[];
extern	long		particle_list_delay[];
extern	long		particle_list_halflife[];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Globals for ship */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

extern	char 	*coll_buf;
extern   long  craftx,crafty;
extern	long	craft;
extern	long	pod_no;
extern	Uint8	Level;
extern	flag	change_level;
extern	flag	escape;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Globals for FIRE */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define MAX_FIRE 12

ext sprite_sequence fire;
ext long  fire_on[MAX_FIRE];
ext long  fire_frame[MAX_FIRE];
ext long  fire_x[MAX_FIRE];
ext long  fire_y[MAX_FIRE];
ext long  fire_dx[MAX_FIRE];
ext long  fire_dy[MAX_FIRE];
ext long  fire_bx[MAX_FIRE];
ext long  fire_by[MAX_FIRE];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Lines */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define MAX_LINES 4

typedef struct
{
	long 	line_on;
	long	x1[2];
	long	y1[2];
	long	x2[2];
	long	y2[2];
} linestruc;

ext linestruc lines[MAX_LINES];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Structure */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define MAX_SPRITES 256

#ifndef	NEWLOGIC
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
		obj_end
};

typedef struct
{
	long  obj[obj_end];
	long	move_ptr;
	long	thread_ptr;
	long	ret_ptr;
	long  on_hit_ptr;
	long  child;
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
	long	startframe,endframe;
	long	normal_startframe,normal_endframe;
	long	left_startframe,left_endframe;
	long	right_startframe,right_endframe;
	long	up_startframe,up_endframe;
	long	down_startframe,down_endframe;
	long	wounded_startframe,wounded_endframe;
	long	death_startframe,death_endframe;
	int	framecounter;
	int	anim_delay_rate;
	int	flashing;
	int	dying;
	int	buf_x[2];
	int	buf_y[2];
	int	buf_frame[2];
	flag	buf_rem[2];

} spritestruct;

#else

enum {

		obj_colx,
		obj_coly,
		obj_angle,
		obj_angle2,
		obj_children,
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
		obj_temp9,
		obj_temp10,
		obj_temp11,
		obj_temp12,
		obj_temp13,
		obj_temp14,
		obj_temp15,
		obj_temp16,
		obj_end
};

typedef struct
{
	long	name;
	long	path,node,map_index;
	long  obj[obj_end];
	long	move_ptr,thread_ptr,ret_ptr,on_hit_ptr;
	long  child,parent,queen;
	long	y1;
	long	priority;
	long	action;
	long	start_x,start_y;
	long	x,y;
	long	frame;
	long	startframe,endframe;
	long	framecounter;
	long	anim_delay_rate;
	long	flashing;
	long	dying;
	long	buf_x[2];
	long	buf_y[2];
	long	buf_frame[2];
	flag	buf_rem[2];
	flag	on,onscreen,hit,visible,clipped,shootable,still;

} spritestruct;

typedef struct
{
	long	normal_startframe,normal_endframe;
	long	left_startframe,left_endframe;
	long	right_startframe,right_endframe;
	long	up_startframe,up_endframe;
	long	down_startframe,down_endframe;
	long	wounded_startframe,wounded_endframe;
	long	death_startframe,death_endframe;

} spriteinfo;

#endif

ext spritestruct sprites[MAX_SPRITES];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Names */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

enum 	{

	NAME_NULL=0,
	NAME_CSHIP,
	NAME_ACIDSPT,
	NAME_BOUNCER1,
	NAME_BOUNCER2,
	NAME_BOUNCER3,
	NAME_CHAPL1,
	NAME_FLOAT,
	NAME_FLOPPY,
	NAME_L1C1,
	NAME_L1C2,
	NAME_L1C3,
	NAME_L1C4,
	NAME_L1C5,
	NAME_LRTAD,
	NAME_MRSPIT,
	NAME_MSSPIT,
	NAME_MUSSEL,
	NAME_PLUNGER,
	NAME_RCK01,
	NAME_RCK02,
	NAME_RCK03,
	NAME_RCKMORPH,
	NAME_STAR,
	NAME_STARFSH,
	NAME_SURVRS,
	NAME_TAD,
	NAME_TENTAC1,
	NAME_TENTAC2,
	NAME_TONSL1,
	NAME_TUBEL1,
	NAME_TURRET1,
	NAME_VINEL11,
	NAME_VINES12,
	NAME_VINE13,
	NAME_VINE14,
	NAME_VINE15,
	NAME_VINE16,
	NAME_WINDY,
	NAME_END

};


ext sprite_sequence seqdata[NAME_END];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Actions */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

enum	{
			ACTION_NONE,
			ACTION_CYCLE,
			ACTION_PLAYONCE,
			ACTION_PINGPONG,
			ACTION_PATH,
		};

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE File Names */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

typedef	struct
{
			char fname[8+1];

} fnames_list;

//--------------------------------------------------------------------------------------------------------------------------------
ext 		long iff_width;
ext 		long iff_height;

ext		long fsize;
ext		fnames_list *sprite_fnames;


enum	{
			COMMAND_WAIT,
			COMMAND_STOP,
			COMMAND_PAUSE,
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
			COMMAND_MULI,
			COMMAND_MULV,
			COMMAND_DIVI,
			COMMAND_DIVV,
			COMMAND_NEG,
			COMMAND_ADD_CHILD,
			COMMAND_ADOPT_INRANGE_CHILD,
			COMMAND_QUEEN_ORDER,
			COMMAND_GET_QUEENS,
			COMMAND_GET_PARENTV,
			COMMAND_GET_CHILDV,
			COMMAND_GET_PLAYER_ID,
			COMMAND_IFEQI,
			COMMAND_IFEQV,
			COMMAND_IFNEI,
			COMMAND_IFNEV,
			COMMAND_IFLTI,
			COMMAND_IFLTV,
			COMMAND_IFLEI,
			COMMAND_IFLEV,
			COMMAND_IFGTI,
			COMMAND_IFGTV,
			COMMAND_IFGEI,
			COMMAND_IFGEV,
			COMMAND_FRAMEINC,
			COMMAND_UPDATE_XY,
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
			COMMAND_ANIMPLAYFORWARDONCE,
			COMMAND_ANIMPLAYBACKONCE,
			COMMAND_ANIMPLAYLOOPONCE,
			COMMAND_ANIMSPEED,
			COMMAND_ANIMSPEEDV,
			COMMAND_INRANGE,
			COMMAND_BG_COLLIDE,
			COMMAND_POINT_COLLISION,
			COMMAND_FIND_FLOOR,
			COMMAND_SCAN,
			COMMAND_DIE,
			COMMAND_JMP,
			COMMAND_ARRAY,
			COMMAND_ADD_PARTICLE,
			COMMAND_ON_HIT,
			COMMAND_SHOOTABLE_ON,
			COMMAND_SHOOTABLE_OFF,
			COMMAND_VISIBLE_ON,
			COMMAND_VISIBLE_OFF,
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

