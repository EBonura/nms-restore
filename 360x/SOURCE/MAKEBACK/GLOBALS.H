/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Globals */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include	<stdlib.h>
#include	<stdio.h>
#include <dos.h>
#include	<fcntl.h> 
#include	<io.h>
#include	<string.h>
#include	<conio.h>
#include <direct.h>
#include	<math.h>
#include	<time.h>
#include	<i86.h>

#ifndef	ext
#define	ext	extern
#endif

#include	"types.h"

ext	flag		light_on;
ext	long  level_no;
ext	char	message[128];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* For level.c and attribs.c */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

ext	long lineno;
ext	char *script;
ext   char line[256];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* FLC structures */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

struct FLI {
		long int fli_size;
		unsigned int fli_type;
		int frames;
		int width;
		int height;
		int depth;
		int flags;
		long speed;
		int res_1;
		long created;
		long creator;
		long updated;
		long updater;
		int aspectx;
		int aspecty;
		int res_2[19];
		long oframe1;
		long oframe2;
		int res_3[20];
		};

struct CHUNK_HEADER {
		long chunk_size;
		unsigned int chunk_type;
		unsigned int nsubchunks;
		int res1[8];
		};

ext	struct FLI fliheader;


ext	long	level_data_loaded;
ext	long	level_data_blocks;

typedef struct
{
	long name;
	long id;
	long frame;
	long frame_delay;
	long x;
	long y;
} panelstruct;


//#define VIEW_COLLISION ON

ext char 		filename[128];

#define DPMI_INT 0x31

ext long			cos_table[256*4];
ext long			sin_table[256*4];

ext long			debug_variable[8];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Structures */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

typedef struct {
	char	*start_address;
	long	*map;
	char	*data;
	char	*org;
	long	frames;
	flag	used;
} sprite_sequence ;

typedef struct 
{
	long	faces;
	long	colour;
	long	x1,y1;
	long	x2,y2;
	long	x3,y3;
} triangle;

ext triangle screen_tri[2];

ext	char *path_data;
ext	char *vfont1;
ext	char *vfont2;
ext	char *vfont3;
ext	char *vfont4;
ext	char *vfont5;
ext	char *vfont_pal;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Defines */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define	PI		3.141592654
ext		long split_height;
//#define	split_height 96
#define	screen_width 640
#define	screen_height (480-96)
#define	clip_start_x 0
#define	clip_end_x (screen_width)
//#define	clip_end_x (viewable_width)
#define	clip_start_y 0
#define	clip_end_y (screen_height)
//#define	clip_end_y (viewable_height)
#define  blk_dim 16
#define 	blk_size (blk_dim*blk_dim)

ext	long physical_screen_start;
ext 	long physical_screen_x;
ext 	long physical_screen_y;

ext 	char bg_fname[128];

ext	flag	path_mode;
ext	flag	mouse_mode;
ext	flag	mouse_connected;
ext	long	mouse_movements;
ext	short	mouse_flash;
ext	short last_mouse_x;
ext	short last_mouse_y;
ext	short mouse_x;
ext	short mouse_y;
ext	short mouse_dx;
ext	short mouse_dy;
ext	short	clicked_mouse;
ext	short	clicked_mouse_x;
ext	short	clicked_mouse_y;
ext	short clicked_buttons;
ext	flag 	clicked_left_button;
ext	flag 	clicked_right_button;
ext	flag 	clicked_middle_button;
ext	short mouse_button_status;
ext	flag 	mouse_left_button;
ext	flag 	mouse_right_button;
ext	flag 	mouse_middle_button;

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
ext	flag use_DOS4GWPRO;
ext	flag full_screen;
ext	flag UNIVBE_installed;
ext	long SVGA_currentmode;
ext	long SVGA_logical_length;
ext	long viewable_width;
ext	long viewable_height;
ext	flag bursting;
ext	flag burst_scroll;
ext	flag collision_on;
ext	flag pod_attached;
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

ext	long	y_mod[4096];
ext	long	y_mod640[4096];
ext	char  bak_screen[640*480];

ext	long	edge_offs[4096];
ext	long	edge_array[8*4096];

//ext	char	collision_block_array[256*256];
ext	char	*collision_block_array;

#define FLOOD_RADIUS 24
#define FLOOD_DIAMETER (FLOOD_RADIUS*2)

ext	char flood_array[FLOOD_DIAMETER*FLOOD_DIAMETER];
ext	long flood_x,flood_y;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Arrays */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

ext	char	*level_fname;
ext	long	screen_xor;
ext	long	scroll_x,scroll_y;
ext	long	last_scroll_x,last_scroll_y;
ext	long	old_scroll_x,old_scroll_y;
ext	long	physical_screen;
ext	long	logical_screen;
ext	long	bufx[2],bufy[2];
ext	char	*blk_buf;
ext	long	*map;
ext	char 	*coll_buf;
ext	char	dump_blk_buf[(1024*(768+blk_dim))+4096];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Globals for ship */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

ext   long  craftn,craft,craftx,crafty;
ext	long craft_angle;
ext	long	pod_no;

ext	long		frac_craft_x;
ext	long		frac_craft_y;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Globals for Keyboard */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

ext char keys[256];
ext unsigned char keys_jp[256];
ext unsigned char keys_lastjp[256];
ext char keys_repeat[256];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Globals for FIRE */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

ext long  freeweaponslots;
ext long  fire_speed;

ext long	 firen;
ext long  fire_delay;

ext long	weapontype;

#define MAX_FIRE 8
#define WEAPON_END 6

ext sprite_sequence fire;
ext long  fire_on[MAX_FIRE];
ext long  fire_frame[MAX_FIRE];
ext long  fire_x[MAX_FIRE];
ext long  fire_y[MAX_FIRE];
ext long  fire_dx[MAX_FIRE];
ext long  fire_dy[MAX_FIRE];
ext long  fire_bx[2][MAX_FIRE];
ext long  fire_by[2][MAX_FIRE];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Lines */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define MAX_LINES 8

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

enum {
		obj_angle,
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

#define MAX_SPRITES 256

typedef struct
{
	long	name;
	long	path,node,map_index;
	long  obj[obj_end];
	long	move_ptr,thread_ptr,ret_ptr,on_hit_ptr;
	long  child,parent,queen;
	long	y1; //x1,y1,x2,y2;
	long	priority;
	long	action;
	long	start_x,start_y;
	long	x,y;
	long	frame;
	long	startframe,endframe;

	// These should not be here
	long	normal_startframe,normal_endframe;
	long	left_startframe,left_endframe;
	long	right_startframe,right_endframe;
	long	up_startframe,up_endframe;
	long	down_startframe,down_endframe;
	long	wounded_startframe,wounded_endframe;
	long	death_startframe,death_endframe;

	long	framecounter;
	long	anim_delay_rate;
	long	flashing;
	long	dying;
	long	buf_x[2];
	long	buf_y[2];
	long	buf_frame[2];
	flag	buf_rem[2];
	flag	on,onscreen,hit,visible,clipped,shootable,still;
	double angle;
} spritestruct;

ext spritestruct sprites[MAX_SPRITES];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Names */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

enum 	{
			NAME_NULL=0,
			NAME_CSHIP,
			NAME_FIRE01,
			NAME_CURSOR1,
			NAME_BOX,
			NAME_BAR_L,
			NAME_BAR_R,
			NAME_pod4,
			NAME_AGMINE,		
			NAME_CHAIN3,
			NAME_CHAINA,	
			NAME_CHAINB,
			NAME_EBALL,
			NAME_SPFIRE,

			NAME_GUIDED,
			NAME_BUCK01,
			NAME_EBALLL,
			NAME_EBALL_M,
			NAME_EBALL_S,
			NAME_SMINE1,
			NAME_SMINE2,
			NAME_SMINE3,

			NAME_R1_HIT,
			NAME_R2_HIT,
			NAME_R3_HIT,	
			NAME_R4_HIT,
			NAME_G1_HIT,
			NAME_G2_HIT,
			NAME_G3_HIT,
			NAME_G4_HIT,
			NAME_W1_HIT,
			NAME_W2_HIT,
			NAME_W3_HIT,	
			NAME_W4_HIT,

 			NAME_XB_FLOP1,
			NAME_XB_FLOP2,
			NAME_XB_FLOP3,
			NAME_XB_FLOP4,
			NAME_XB_FLOP5,
			NAME_XB_FLOP6,

 			NAME_NXA,
 			NAME_NXB,
 			NAME_NXC,
 			NAME_NXD,
 			NAME_NXE,
 			NAME_NXF,
 			NAME_NXG,
 			NAME_NXH,
 			NAME_NXI,

			NAME_BORDER1,
			NAME_SMWIRE,
			NAME_GUIDWIRE,
			NAME_EBWIRE,
			NAME_MINIWIRE,
			NAME_LASWIRE,
			NAME_AGMWIRE,
			NAME_BCKWIRE,

			NAME_JELLO,

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

			NAME_BORDSML,
			NAME_BORDSML2,
			NAME_BORDSML3,

			NAME_SHIPWIRE,
			NAME_SHIPWIRR,
			NAME_SHIPWIR2,
			NAME_SHIPWIR3,
			NAME_SHIPWIR4,

			NAME_MISSWIRE,
			NAME_BARS01,

			NAME_portdu,
			NAME_portuu,
			NAME_portlu,
			NAME_portru,
			NAME_portul,
			NAME_portur,
			NAME_portdl,
			NAME_portll,
			NAME_portrl,
			NAME_windy,

			NAME_lmspit,
			NAME_sperm,
			NAME_sperm2,
			NAME_windl2,

			NAME_mine2,
			NAME_windl4,
			NAME_lazl4,
			NAME_l42bt,

			NAME_tentc1l5,
			NAME_tentc2l5,
			NAME_turrl6,

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
			NAME_l4c2,
			NAME_l4c3,
			NAME_l4c4,
			NAME_l4c5,
			NAME_l4bt,
			NAME_plungl4,
			NAME_lazerl4,

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
			NAME_tonsl6,
			NAME_tubel6,

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
			NAME_tonsl5,

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

			NAME_lmrspit,

			NAME_l2rock2,
			NAME_l2rock3,

			NAME_tubel3,
			NAME_windl3,
			NAME_l3rock2,
			NAME_l3rock3,
			NAME_l33bt,
			NAME_turrl3,
			NAME_tonsl3,
			NAME_vtestfl3,
			NAME_vinfl3_3,
			NAME_vtestl3,

			NAME_l4rock2,
			NAME_l4rock3,

			NAME_l5rock2,
			NAME_l5rock3,

			NAME_tentc1l6,
			NAME_l6rock2,
			NAME_l6rock3,

			NAME_l7rock1,
			NAME_l7rock2,
			NAME_l7rock3,
			NAME_l7rock4,

			NAME_bugger,

 			NAME_spermneo,
			NAME_spermne2,
			NAME_spermne3,

			NAME_l8blob,
			NAME_s_l8c3,

			NAME_flipl8_1,
			NAME_flipl8_d,
			NAME_flipl8_u,
			NAME_flipl8_r,

			NAME_donkey,
			NAME_kong,

			NAME_END
	   };

ext sprite_sequence seqdata[NAME_END];

typedef struct
{
	flag	defined;
	flag	shootable;
} attribstruc;

ext attribstruc sprites_attribs[NAME_END];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Actions */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

enum	{
			ACTION_NONE,
			ACTION_CYCLE,
			ACTION_RND_CYCLE,
			ACTION_PLAYONCE,
			ACTION_PINGPONG,
			ACTION_PATH,
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

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Font */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define CHAR_HEIGHT 32 	// This must be a multiple of 8
ext long *font_table;
ext char *font_buffer;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Defines */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */


#define SCREEN_AREA 0xa000
#define screen_lin_addr ((SCREEN_AREA) << 4)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define WEB_WIDTH 320
#define WEB_HEIGHT 200

ext	long		currentband;
//--------------------------------------------------------------------------------------------------------------------------------
//	PC HARDWARE EQUATES
//--------------------------------------------------------------------------------------------------------------------------------

#define SC_INDEX		0x3c4				//	INDEX REGISTER FOR SEQ CTRL
//#define 0x3c5
#define DAC_WRITE_ADR	0x3c8				//	DAC WRITE ADDRESS
#define DAC_DATA  		0x3c9				//	DAC DATA REGISTER
#define CRTSTATUS		0x3da				//	STATUS PORT - FOR VBLANK WAIT

//#define	NORM80X25XTEXT	3					//	80*25 mode
#define	NORM80X25XTEXT	0x54					//	140*43 mode
#define	NORM132X43XTEXT	0x54				//	132*43 mode(glenn mode)

#define	VID640X480X16	18					//	VGAMODE
#define	VID320X200X256	19					//	MCGAMODE
#define	svga640x400x256	0x100
#define	svga640x480x256	0x101

//--------------------------------------------------------------------------------------------------------------------------------
//	PC KEYBOARD EQUATES
//--------------------------------------------------------------------------------------------------------------------------------

#define	KEYSREPEATMAX		10

#define	kSYSREQ				0x54
#define	kCAPSLOCK			0x3A
#define	kNUMLOCK             0x45
#define	kSCROLLLOCK          0x46
#define	kLEFTCTRL            0x1D
#define	kLEFTALT             0x38
#define	kLEFTSHIFT           0x2A
#define	kRIGHTCTRL           0x9D
#define	kRIGHTALT            0xB8
#define	kRIGHTSHIFT          0x36
#define	kESC                 0x01
#define	kBACKSPACE           0x0E
#define	kENTER               0x1C
#define	kSPACE               0x39
#define	kTAB                 0x0F
#define	kF1                  0x3B
#define	kF2                  0x3C
#define	kF3                  0x3D
#define	kF4                  0x3E
#define	kF5                  0x3F
#define	kF6                  0x40
#define	kF7                  0x41
#define	kF8                  0x42
#define	kF9                  0x43
#define	kF10                 0x44
#define	kF11                 0x57
#define	kF12                 0x58
#define	kA                   0x1E
#define	kB                   0x30
#define	kC                   0x2E
#define	kD                   0x20
#define	kE                   0x12
#define	kF                   0x21
#define	kG                   0x22
#define	kH                   0x23
#define	kI                   0x17
#define	kJ                   0x24
#define	kK                   0x25
#define	kL                   0x26
#define	kM                   0x32
#define	kN                   0x31
#define	kO                   0x18
#define	kP                   0x19
#define	kQ                   0x10
#define	kR                   0x13
#define	kS                   0x1F
#define	kT                   0x14
#define	kU                   0x16
#define	kV                   0x2F
#define	kW                   0x11
#define	kX                   0x2D
#define	kY                   0x15
#define	kZ                   0x2C
#define	k1                   0x02
#define	k2                   0x03
#define	k3                   0x04
#define	k4                   0x05
#define	k5                   0x06
#define	k6                   0x07
#define	k7                   0x08
#define	k8                   0x09
#define	k9                   0x0A
#define	k0                   0x0B
#define	kMINUS               0x0C
#define	kEQUAL               0x0D
#define	kLBRACKET            0x1A
#define	kRBRACKET            0x1B
#define	kSEMICOLON           0x27
#define	kTICK                0x28
#define	kAPOSTROPHE          0x29
#define	kBACKSLASH           0x2B
#define	kCOMMA               0x33
#define	kPERIOD              0x34
#define	kSLASH               0x35
#define	kINS                 0xD2
#define	kDEL                 0xD3
#define	kHOME                0xC7
#define	kEND                 0xCF
#define	kPGUP                0xC9
#define	kPGDN                0xD1
#define	kLARROW              0xCB-128
#define	kRARROW              0xCD-128
#define	kUARROW              0xC8-128
#define	kDARROW              0xD0-128
#define	kKEYPAD0             0x52
#define	kKEYPAD1             0x4F
#define	kKEYPAD2             0x50
#define	kKEYPAD3             0x51
#define	kKEYPAD4             0x4B
#define	kKEYPAD5             0x4C
#define	kKEYPAD6             0x4D
#define	kKEYPAD7             0x47
#define	kKEYPAD8             0x48
#define	kKEYPAD9             0x49
#define	kKEYPADDEL           0x53
#define	kKEYPADSTAR          0x37
#define	kKEYPADMINUS         0x4A
#define	kKEYPADPLUS          0x4E
#define	kKEYPADENTER         0x9C-128
#define	kCTRLPRTSC           0xB7
#define	kSHIFTPRTSC          0xB7
#define	kKEYPADSLASH         0xB5

//--------------------------------------------------------------------------------------------------------------------------------

//#define	KEYS2SCAN	38					//	A-Z 0-9 /

//--------------------------------------------------------------------------------------------------------------------------------
ext 		long iff_width;
ext 		long iff_height;
ext 		unsigned char	remap[256];
ext		unsigned char 	palette[256*3];
ext		unsigned char 	psx_palette[512];
ext		unsigned char 	pal_32bit[256*4];
ext		unsigned char 	iff_palette[256*3];

ext		long fsize;
ext		fnames_list *sprite_fnames;

enum	{
			FX_BUB1A,    
			FX_BUB2,    
			FX_DROP1,    
			FX_EXP1,     
			FX_EXP2,     
			FX_EXP3,     
			FX_EXP4,     
			FX_LASER,    
			FX_LASER1,   
			FX_LASER2,   
			FX_LASER3,   
			FX_LASER4,   
			FX_MISSILE,  
			FX_SCRAPE1,  
			FX_SIDCSH1,  
			FX_SINERES,  
			FX_SONAR1,  
			FX_SONAR2,   
			FX_THRUSTA,  
			FX_THRUSTB,  
			FX_THRUSTC,  
			FX_THRUSTD,  
			FX_THRUSTE,  
			FX_END
  		};

ext char *samples_start[FX_END];
ext long	samples_len[FX_END];




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
			COMMAND_OUTPUT_VAR
		};

typedef struct  {
	double	x;
	double	y;
	double	vx;
	double	vy;
	double	mass;
} ball_st;

ext ball_st ball[2]; 

#include	"protos.h"
