//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: header for the fixed-point ball physics (FIXBALL.C, FIXMATH.C).
// Not NMS code.  It holds the fixed-point ball, cushion and pocket types, the
// fixed-point maths and the physics prototypes.  GLOBALS.H includes it (the
// game's ball data were doubles, for BALL.C) and declares the storage as
// before.  With FIXBALL_STANDALONE (tests/qdup-physics, which builds FIXBALL.C
// without the rest of the game) it also declares the constants, the storage
// and the game state FIXBALL.C touches, as GLOBALS.H and PROTOS.H do.
//
// Number formats
//   8.24  (1.0 = 0x1000000)  positions, distances (m), velocities (m/s),
//         times (s), masses (kg), forces, friction and loss factors; this is
//         NMS's format (mul64, the 0x1000000 conversions in COMMON.C).
//   16.16 (1.0 = 0x10000)    angular velocity avx/avy/avz (rad/s) and the
//         angular positions ax/ay/az (rad).  A rolling ball spins at v/r,
//         about 260 rad/s at 7.35 m/s, past the 128 that 8.24 holds.
//   cushion angles (cush_st.cush_ang): degrees in 16.16, as COMMON.C makes
//         gcushions_*[].ang.
//   ANGLE: SGL angle, 65536 = 360deg; slSin/slCos return 16.16 (FIXED).
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#ifndef	FIXBALL_H
#define	FIXBALL_H

#ifdef	FIXBALL_STANDALONE		// in the game GLOBALS.H has included these
#ifndef	PSX
#define	PSX
#endif
#include	"types.h"
#include	"escher.h"
#endif

typedef	short	ANGLE;			// SGL angle, 65536 = 360deg
typedef	long	FIXED;			// SGL 16.16

// 8.24 and 16.16 constants from decimal (constant expressions, folded by the
// compiler; never use them on variables)
#define	FIX24(_n_)	((long)((_n_)*16777216.0 + (((_n_)<0) ? -0.5 : 0.5)))
#define	FIX16(_n_)	((long)((_n_)*65536.0 + (((_n_)<0) ? -0.5 : 0.5)))
#define	FIX24_ONE	0x1000000

// the game code that keeps doubles (shot input, link play) meets the 8.24
// ball data through these (restoration)
#define	FIX24_TO_DOUBLE(_v_)	((double)(_v_)/16777216.0)
#define	DOUBLE_TO_FIX24(_d_)	((long)((_d_)*16777216.0))
// 8.24 metres to and from Escher world units (256000 to the metre, the
// game's *256000 and /256000.0); 32x32 to 64-bit product, one MIPS mult
#define	FIX24_TO_WORLD(_v_)		((long)(((long long)(_v_) * 256000) >> 24))
#define	WORLD_TO_FIX24(_w_)		((long)(((long long)(_w_) << 24) / 256000))

// whole degrees of the vector (x, y), as the game's (int)(atan2(y,x)/deg2rad)
// (truncated toward zero, -179..180), and whole degrees to an SGL angle
#define	FIX_ATAN2_DEG(_y_,_x_)	((fix_atan2(_y_,_x_)*360)/65536)
#define	DEG_TO_ANG(_d_)			((ANGLE)(((_d_)*65536)/360))

// cushion angle (degrees, 16.16) to SGL angle: deg*65536/360 = deg16/360
#define	DEG16_TO_ANG(_d_)	((ANGLE)((_d_)/360))

//----------------------------------------------------------------------------
// fixed-point ball, cushion and pocket data (BALL.C had them in doubles)

typedef	struct	{
						long	graphic_type;
						long	obj_type;
						long	ball_colour;
						long	foul_flag;
						long	hitcush;
						long	numstruct;
						long	respot_flag;
						long	num;
						long pocket_ball_went_down;

	long	vx1 ,vy1 ,vz1;					// velocity, 8.24
	long	x ,y ,z;						// position, 8.24
	long	dist_x ,dist_y ,dist_z;			// distance rolled, 8.24
	long	x_prev ,y_prev ,z_prev;			// previous positions, 8.24
	long	vx ,vy ,vz;						// velocity, 8.24
	long	ax ,ay ,az;						// angular position, 16.16
	long	old_ax ,old_ay ,old_az;			// angular position, 16.16
	long	avx,	avy,	avz;				// angular velocity, 16.16
	long	diameter_squared;				// 8.24
	long	diameter;						// 8.24
	long	radius;							// 8.24
	long	mass;							// 8.24
	long	mi;								// moment of inertia, 8.24
	long	inv_mass;						// 1/mass, 8.24 (restoration)
	long	spin_scale;						// radius/(5*mi) = 1/(2*mass*radius), 16.16 (restoration)
	unsigned char	moving;				// is ball moving?
	unsigned char	active;				// is ball moving?

				} 	ballstruct;

struct cush_st {
	long x;					// 8.24
	long y;					// 8.24
	long cush_ang;			// degrees, 16.16
};

struct pockets_st {
	long x,y;				// 8.24
	long radius;			// 8.24
};

struct ball_col_st {
	short ball_num;
	long x;
	long y;
};

typedef long fmatrix4x4[4][4];		// 8.24 (restoration)
typedef long fmatrix4x1[4];

//----------------------------------------------------------------------------
// (a * b) >> 24: product of two 8.24 numbers (CODE.S mul64), and
// (a * b) >> 16 (SGL.S slMulFX).  One MIPS mult each; inline because the
// physics calls them in every inner loop.
static inline long	mul64(long a, long b)
{
	return((long)(((long long)a * b) >> 24));
}

static inline long	slMulFX(long a, long b)
{
	return((long)(((long long)a * b) >> 16));
}

// |a|, inline (the game is built with -fno-builtin, so abs is a library
// call); FIXBALL.C takes abs as this
static inline long	fix_abs(long a)
{
	return((a < 0) ? -a : a);
}

// FIXMATH.C
long	get_moment(long mass, long radius);
long	fix_sqrt(long val);
long	fixdiv(long a, long b);
long	fix_atan2(long y, long x);
long	dbl_to_fix24(double d);		// FIXMATH.C, the 3D view
// SGL.S (NMS): sin and cos of an SGL angle, 16.16
FIXED	slSin(ANGLE angle);
FIXED	slCos(ANGLE angle);
long	fix_hypot(long x, long y);

// FIXBALL.C
void	add_col_to_list( short ball_num, long x, long y);
void	reset_col_list(void);
void	make_sin_cos_table( void );
void	pocketed_ball(long ball_num, long pock, long ang);
void	continuous_check(long ball_num, long pock);
void	nms_pot_check(long ball_num, long pock);
void	uk_pot_check( long ball_num );
void	us_8ball_check( long ball_num);
void	us_pot_check(long ball_num);
void	snooker_pot_check(long ball_num);
void	update_ball_movement( long time_step, int ball_num);
void	hit_ball( long ball_num, long x_hit, long y_hit, long ang_z, long ang_y, long vel);
void	init_ball_position( int ball_num );
void	check_cushions_float( int ball_num, long current_time_step);
void	check_cushions_x1(int ball_num, long current_time_step);
void	check_cushions_y2(int ball_num, long current_time_step);
void	check_cushions_y3(int ball_num, long current_time_step);
void	check_cushions_x4(int ball_num, long current_time_step);
long	get_cush_ang(int bump_num, long x, long y);
long	get_cush_x(int	bump_num, long x, long y);
long	get_cush_y(int bump_num, long x, long y);
void	cush_rebound_x( int ball_num, long angle, int cush_num);
void	cush_rebound_y( int ball_num, long angle, int cush_num);
long	get_ball_grad(int ball_num);
long	get_ball_intercept(int ball_num, long grad);
long	find_nearest_x_dist(int ball_num);
long	find_nearest_y_dist(int ball_num);
void	update_all_balls( long time_step );
long	get_max_time_step( long time_step );
void	ball_collision2(short ball1, short ball2);
void	rotate2(ANGLE roll, ANGLE pitch, ANGLE yaw, fmatrix4x4 spin);
flag	any_balls_moving(void);
void	check_all_pockets_x1(long ball_num, long bumper);
void	check_all_pockets_x4(long ball_num, long bumper);
void	check_all_pockets_y2(long ball_num, long bumper);
void	check_all_pockets_y3(long ball_num, long bumper);
void	test_camers_in_pocket( long ballnum );
long	check_solid_pocket(long ball_num, long oldx, long oldy);
int	load_cushions( char *name );

//----------------------------------------------------------------------------
// FIXBALL_STANDALONE: what GLOBALS.H and PROTOS.H give FIXBALL.C in the game
#ifdef	FIXBALL_STANDALONE

#ifndef	TRUE
#define	TRUE		1
#define	FALSE		0
#endif
#define	or |					// as GLOBALS.H

// C library functions FIXBALL.C uses (Psy-Q's libc had them)
void	*memcpy(void *d, const void *s, unsigned int n);
#ifndef	abs
#define	abs(_x_)	__builtin_labs(_x_)
#endif

//----------------------------------------------------------------------------
// constants, values as GLOBALS.H
#define	MAXBALLS	22
#define	WHITE_BALL	0
#define	WHITEBALL	0
#define	NUM_POCKETS		6
#define	NUM_CUSH_POINTS_X		2000
#define	NUM_CUSH_POINTS_Y	   1000
#define	BALL_COL_LIST_SIZE			64
#define	DIFFERENT_SHAPED_TABLES_INDEX 4

#define	LEFT_PAN	1
#define	RIGHT_PAN	2

#define	YELLOW      1
#define	BLUEBALL		2
#define	RED			3
#define	PURPLE		4
#define	GREEN			6
#define	BROWN			7
#define	BLACK			8

#define	UK_POOL		0
#define	US_8BALL		1
#define	US_POOL	 	2
#define	CONTINUOUS	3
#define	SPEED_POOL 	4
#define	KILLER_POOL 5
#define	NMS_POOL		6

#define	PANEL_ON_TIME	75
#define	POT_RED_BALL      1
#define	MAXPLAYERS 2
#define	MAX_PLAYERS	16

enum {
				CLAPPING,	HIT_WHITE,	BALL_HIT,	CUSHHIT,	 PANDAM,
 UNNAMED1,	UNNAMED2,	POT_WHITE,	POT_BALL,	UNNAMED5,	UNNAMED6,	UNNAMED7,
 COUGH1, COUGH2, COUGH3, FART_SOUND
};

ext	long	General_Rules[50];
#define	UK_FREE_BALL						General_Rules[6]
#define	LOCATION General_Rules[25]
#define	TABLESIZE_TYPE General_Rules[27]
#define	GAME_TYPE General_Rules[28]
#define	left_player	General_Rules[39]
#define	right_player General_Rules[43]

// storage, as GLOBALS.H
ext	ballstruct ball[MAXBALLS];
ext	long	LAST_BALL;
ext	struct cush_st cushions_x3[NUM_CUSH_POINTS_X];
ext	struct cush_st cushions_x2[NUM_CUSH_POINTS_X];
ext	struct cush_st cushions_y1[NUM_CUSH_POINTS_Y];
ext	struct cush_st cushions_y4[NUM_CUSH_POINTS_Y];
ext	long max_cush_y2;
ext	long min_cush_y3;
ext	long max_cush_x4;
ext	long min_cush_x1;
ext	long bumper_scale;
ext	long ball_mass;
ext	struct pockets_st pockets[NUM_POCKETS];
ext	short ball_col_list_pos;
ext	struct ball_col_st ball_col[BALL_COL_LIST_SIZE];
ext	short ball_hits_pos;

//----------------------------------------------------------------------------
// game state used by FIXBALL.C, declared as in GLOBALS.H

typedef	struct	{
						long		obj_type;

						long		foul_type;
						long 		ball_type;
						long		potted_count;
						long		speed_foul;
						long	points;

				} playersstruct;

struct	player_stats	{
		long	handicap;
		long	potting_ratio;
		long	avg_shot_power;
		long	trick_shots;
		long	fluke_rating;
		long	last_total_score;
		long	shots_taken;
		double	ball_mileage;
};

ext	playersstruct players[MAXPLAYERS];
ext	struct	player_stats	player_info[MAX_PLAYERS*2];
ext	OBJECT *ball_ptr[22];
ext	long	activated_lines;
ext	long	calculating_computer_shot;
ext	long	potted_flag;
ext	long	break_flag;
ext	int	potted_ball[32];
ext	long	pot_count;
ext	long	saved_ball_count;
ext	long	nine_ball_count;
ext	char	pocket_num;
ext	long	show_shot_flag;
ext	long	show_panel;
ext	flag	SNOOKER_MODE;
ext	flag	SNOOKER_SHOOT_MODE;
ext	long	speed_pot;
ext	long	start_time;
ext	int	num_frames_camera_still;
ext	long	nms_pockets[6];
ext	long	potted_good_ball_flag;
ext	long	potted_hi_ball_flag;
ext	long	potted_red_wrong_flag;
ext	long	potted_black_wrong_flag;
ext	long	potted_yellow_wrong_flag;
ext	long	p1_stored_pot_type,p2_stored_pot_type;
ext	long	Player_Active;
ext	long	cushion_hit,white_hit_cushion;
ext	long	Two_Shots;
ext	flag	used_free_ball;
ext	long	LOWEST_BALL_HIT_FIRST_FLAG;
ext	long	draw_twice;
ext	flag	lowest_ball_not_hit_first;
ext	long	hit_red_ball_flag;
ext	long	missed_all_balls_flag;
ext	long	ball_hits_count;
ext	flag	end_of_frame_flag;
ext	long	butt;
ext	long	x_hit_pos, y_hit_pos;
ext	long	new_count;
ext	long	camera_in_pocket;
ext	long	xpoints;					// COMMON.H
ext	long	ypoints;

// game functions FIXBALL.C calls (PROTOS.H)
int	add_sample(int number, int pan);
int	add_sample_with_volume(int ballnum,int number, int pan,int vol);
void	Add_Score( long score);
void	Add_Score1( long score);
void	Add_Score_2_other_player( long score);
void	check_hits8ball( long ball_num );
void	check_hits_snooker( long ball_num );
void	check_hitsuk( long ball_num );
void	check_hitsus( long ball_num );
void	DrawScreen(void);
void	get_new_screen_pos(long loop);
void	setup_camera_in_pocket(long pocket);
ubyte	*LoadFile(text *name);
void	Free_Ram_Ptr( ubyte *addr );

#endif	// FIXBALL_STANDALONE

#endif
