/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Saturn Globals */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#ifndef	ext
#define	ext	extern
#endif

ext char filename[];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
// Sound play back
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

extern	Uint8		play_sample(Uint8	channel,Uint8 pitch);
extern	Uint8		play_sample_with_volume(Uint8	voice,Uint8 pitch,Uint8 volume);
extern	Uint8		play_sample_with_volume_and_direction(Uint8	voice,Uint8 pitch,Uint8 volume,Sint8 direction);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Defines */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

enum	
{
		WEAPONMISSILE=0,
		WEAPONLASER,
		WEAPONBUCKSHOT,
		WEAPONPROTON,
		WEAPONCHAIN,
		WEAPONPRONG,
		WEAPONHOMING,
		WEAPONCIRCLE,
		WEAPONSHIELD
};

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

ext	Uint16		K_THRUST;
ext	Uint16		K_FIRE;
ext	Uint16		K_LEFT;
ext	Uint16		K_RIGHT;
ext	Uint16		K_WEAPON_INC;
ext	Uint16		K_WEAPON_DEC;

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

ext	char	stat_str[];
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

#define MAX_FIRE 8

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
//	long	buf_x[2];
//	long	buf_y[2];
//	long	buf_frame[2];
//	flag	buf_rem[2];
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

ext 		spritestruct 	sprites[];
extern	spriteinfo		info_index[];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Names */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

enum 	{
			NAME_NULL=0,
			NAME_POD,
			NAME_CSHIP,
			NAME_FIRE01,
			NAME_CURSOR1,
			NAME_BOX,
			NAME_BAR_L,
			NAME_BAR_R,
			NAME_AGMINE,		
			NAME_CHAIN3,
			NAME_CHAINA,	
			NAME_CHAINB,
			NAME_EBALL,

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
			NAME_l4tenbl,

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
			NAME_chapl6,
			NAME_vtestl6,
			NAME_tonsl6,
			NAME_tubel6,
			NAME_l6blob,

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

 			NAME_spermneo,
			NAME_spermne2,
			NAME_spermne3,

			NAME_l8blob,
			NAME_s_l8c3,
			NAME_jellol8,

			NAME_flipl8_1,
			NAME_flipl8_d,
			NAME_flipl8_u,
			NAME_flipl8_r,

			NAME_lavapit,
			NAME_lavaprt,

			NAME_tele1,
			NAME_tele2,
			NAME_teletemp,

			NAME_bugger,
			NAME_buggerm,

			NAME_exp1,
			NAME_exp2,
			NAME_exp3,
			NAME_exp4,
			NAME_exp5,
			NAME_exp6,
			NAME_exp7,
			NAME_exp8,

			NAME_lavbit1,
			NAME_lavbit2,
			NAME_lavbit3,
			NAME_lavbit4,

			NAME_lavbit1a,
			NAME_lavbit2a,

			NAME_tubef2,
							
			NAME_v1_1,
			NAME_v1_2,
			NAME_v1_3,
			NAME_v1_4,
			NAME_v1_5,
			NAME_v1_6,
			NAME_v1_7,
			NAME_v1_8,
			NAME_v1_9,
			NAME_v1_10,
			NAME_v1_11,
			NAME_v1_12,
			NAME_v1_13,

			NAME_v2_1,
			NAME_v2_2,
			NAME_v2_3,
			NAME_v2_4,
			NAME_v2_5,
			NAME_v2_6,
			NAME_v2_7,
			NAME_v2_8,
			NAME_v2_9,
			NAME_v2_10,

			NAME_v4_1,
			NAME_v4_2,
			NAME_v4_3,
			NAME_v4_4,
			NAME_v4_5,
			NAME_v4_6,
			NAME_v4_7,
			NAME_v4_8,
			NAME_v4_9,
			NAME_v4_10,
			NAME_v4_11,
			NAME_v4_12,

			NAME_v5_1,
			NAME_v5_2,
			NAME_v5_3,
			NAME_v5_4,
			NAME_v5_5,
			NAME_v5_6,
			NAME_v5_7,
			NAME_v5_8,
			NAME_v5_9,
			NAME_v5_10,
			NAME_v5_11,
			NAME_v5_12,

			NAME_v6_1,
			NAME_v6_2,
			NAME_v6_3,
			NAME_v6_4,
			NAME_v6_5,
			NAME_v6_6,
			NAME_v6_7,
			NAME_v6_8,
			NAME_v6_9,
			NAME_v6_10,
			NAME_v6_11,
			NAME_v6_12,

			NAME_boss1,
			NAME_boss4,
			NAME_boss4bit,

			NAME_slot1,
			NAME_slot2,
			NAME_slot3,
			NAME_slot4,
			NAME_slot5,
			NAME_slot6,
			NAME_slot7,
			NAME_slot8,


			NAME_donkey,
			NAME_kong,

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