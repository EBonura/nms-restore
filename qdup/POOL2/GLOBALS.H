
#ifdef	PSX
#include <sys\file.h>
#include <sys\types.h>
#include <kernel.h>
#include <libsn.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
//#include <libgs.h>	/* the header files for graphics libraries */
#include <libspu.h>
#include <libsnd.h>
#include <libcd.h>
#include <libmath.h>
#include <cardio.h>
#include <except.h>
#include <ctrller.h>
#include <padequ.h>
#else
#include	"sgl.h"
#include	<sega_sys.h>
#include	<sega_def.h>
#include	<libsn.h>
#include	<math.h>
#endif


/* ------------------------------------------------------------------- */
#define	PCDISKFP		long fp
#define	PCERRORFP	NULL
/* ------------------------------------------------------------------- */
#include	"types.h"
#include	"escher.h"
#include	"fixball.h"		// RESTORE: fixed-point ball, cushion and pocket types (FIXBALL.C)
#include	"protos.h"
#include	"common.h"

#ifdef	PSX
#include	"psx.h"
#else
#include	"saturn.h"
#endif


#define		VRAMaddr		(SpriteVRAM+0x70000)
#define		GRaddr	0xe000

#ifndef	PSX
int	fade_val;
Uint16 *poke;
Uint16 sat_zoom;
Uint16 sat_temp;
Uint16 fade_type;
#endif


/////  MARKS GLOABLS /////


/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/*									    	 QD-UP      										 */
/*											PC-CD ROM											 */
/*									 For BMG	Interactive										 */
/*									 By NMS Software Ltd.									 */
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#define OR |
#define or |

#define US 0   /* us table */
#define UK 1   /* uk table */

#define ECTS_MODE 1

#define DIFFERENT_SHAPED_TABLES_INDEX 4
// RESTORE: the entry of ball_spot_positions and whiteball_spot_positions
// (LEVELDAT.C) for a table at or past the snooker hall (LOCATION 4, mesh_table
// order).  The arrays list potters, rackers, hall, german and snooker hall
// with three values each (0 to 14), then moon (15), ice, beach, crypt (18),
// swamp, toy (20) with one.  NMS's index, 3*4 + (LOCATION-4), counted the
// snooker hall as one entry, so every table after it read the entry two
// before its own (the Crypt, LOCATION 8, got the ice rink's, the Square,
// LOCATION 10, the Crypt's, a rack and white spot outside its 1.34 m table).
#define SPOT_ROOM(_l_)	(DIFFERENT_SHAPED_TABLES_INDEX*3 + ((_l_)-DIFFERENT_SHAPED_TABLES_INDEX) + (((_l_) > DIFFERENT_SHAPED_TABLES_INDEX) ? 2 : 0))
#define TOURNAMENT_TRACK 7

#define ZOOM_BACK_TO_CUE_BALL	0
#define MOVE_BACK_TO_CUE_BALL	12
#define MAX_MOUSE_SPEED  180
#define BOARD_SIZE 240
#define	hiscorebuffsize	418

/*-----------------------------------------------------------------*/
#define POOL 0
#define SNOOKER 1
#define BILLARDS 2
/* ------------------------------------ */
#define READY2_SHOOT_MODE 35
#define AIM_MODE   36
#define MOVE_CAMERA_MODE 37
#define AMPLIFY_SHOOT_MODE 38
#define CAMERA_ZOOM_MODE 39
#define APPLY_ENGLISH 40
#define RAISE_BUTT 41
#define MOVING_BALL 42

/* ------------------------------------ */
/* table culours */
enum	{	RED_BAIZE ,  GREEN_BAIZE , BLUE_BAIZE 
};

/* ---------------------------------------------------------------- */
/* sound effect list for front end, so far */
enum {
		SELECT1,	SELECT2,	SELECT3,	SELECT4,	SELECT5,	SELECT6,	SELECT7,	SELECT8,
		SELECT9,	SELECT10,	SELECT11, SELECT12,	SELECT13,
		PHONE0,	PHONE1A,	PHONE2,	PHONE3,	PHONE4,	PHONE5,	PHONE6,	PHONE7,
		PHONE8,	PHONE9, CONNECT_PHONE, PHONE1,PROJECTOR, BLACKBOARD
};
/* ------------------------------------ */
/* cd tracks on game disk */
enum	{
			JAZZ1 , WRAPPER , JAZZ3 , FUNK,
			ARENA, TOURNAMENT , JINGLE1,JINGLE2,
 			LOSE, WIN
};
/* ------------------------------------ */
/* 		 pool sample list 				 */
enum {
				CLAPPING,	HIT_WHITE,	BALL_HIT,	CUSHHIT,	 PANDAM,
 UNNAMED1,	UNNAMED2,	POT_WHITE,	POT_BALL,	UNNAMED5,	UNNAMED6,	UNNAMED7,
 COUGH1, COUGH2, COUGH3, FART_SOUND 
};
/* ------------------------------------ */
#define	MAX_CD_VOLUME	255

#define	SIZE_OF_SAVE_SHOT_DATA 	6600

/* ------------------------------------ */
#define SVGA_Y_POS 14
#define	CLIPPED	40

#define	PANEL_HEIGHT	0
#define	player2namesoffset	16

#ifdef	__PC_H
#include	<stdio.h>
#include	<stdlib.h>
#include <dos.h>
#include	<fcntl.h> 
#include	<io.h>
#include	<string.h>
#include	<conio.h>
#include	<direct.h>
#include	<time.h>
#include	<i86.h>
#include	<math.h>
#include	"pmode.h"
#include	"escher.h"
#include	"protos.h"
#endif

/* ------------------------------------------------------------------- */
#define	GRID_SIZE	22

#define	ROTATE_CAMERA_VIEW	1
#define	MOVING_BALL_IN_HAND	2
#define	RAISE_BUTT_OF_CUE		3
#define	CAMERA_ZOOM				4
#define	TAKING_SHOT				5
#define	ADD_MASSE				6
#define	RETURN_TO_AIM			7
#define	ROTATE_CAMERA_VIEW_BY_180			8
#define  MOVE_CAMERA 10

/* defines for sound effects */
#define	LEFT_PAN	1
#define	RIGHT_PAN	2

/* ------------------------------------------------------------------- */
/* in game control keys */

#define	MOVE_A_BALL_KEY	M
#define	RAISE_BUTT_KEY		B	/* apply masse to cue ball, raise cue butt */
#define	ADD_ENGLISH_KEY	E /* apply english to cue ball */
#define	TAKE_SHOT_KEY		S	/* hold s and move cue in and out */
#define	REPLAY_KEY			kR	/* replay last shot */
#define	OVERHEAD_VIEW_KEY kF2
#define	RETURN_BACK_FROM_OVERHEAD_VIEW_KEY	kF3
#define	TURBO_SHOT_KEY		kLEFTSHIFT
#define	TURBO_SHOT_KEY1	kRIGHTSHIFT
/* ------------------------------------------------------------------- */

#define	_WAIT_ENTER	wait_for_enter_key()
#define	_WAIT_SPACE	wait_for_space_key()
/* ------------------------------------------------------------------- */
#define	MAX_KILLER_PLAYERS	8
#define	MAX_LIVES				3

#define	POTTERS_CLUB	0
#define	RACKERS			1
#define	GERMAN			3
#define	ARENA	  	      3
#define  POOL_HALL      2
/* ------------------------------------------------------------------- */

#define	STRIPE			10
#define	SPOT				11
#define	SPOT_BLACK		12

#define	WHITEBALL	0
#define	WHITE       0
#define	YELLOWBALL  1
#define	YELLOW      1
#define	BLUEBALL		2
#define	RED			3
#define	PURPLE		4
#define	ORANGE		5
#define	GREEN			6
#define	BROWN			7
#define	BLACK			8

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define	PLAYER_SELECT_ROOM	0
#define	PROJECTOR_ROOM			1
#define	COMPUTER_ROOM			2
#define	SAVEGAME_ROOM			3
#define	LOADGAME_ROOM			4
#define	TOURNAMENT_ROOM		5
#define	TRICK_SHOTS				6
#define	HISCORES_ROOM			7
#define	RULE_BOOK				8
#define	HITIME_ROOM				9
#define  TELLY_ROOM           12
#define  TITLE_ROOM           13

#define	CAMERA_SPEED	.070
#define	CAMERA_SPEED_LONG	40

#define	CAMERA_SPEED1	40
#define	DEFAULT_ZOOM	750

#define	DEFAULT_ZOOM_FOR_SHOT	-5*2
#define	DEFAULT_OVERHEAD_ZOOM	-62*2

#define	DEFAULT_FOLLOW_BALL	-40*2.3
#define	MAX_ZOOM	60*2.3
#define	DEFAULT_SPEED	10

/* ------------------------------------------------------------------- */
//GAME TYPES ARE::--
//	0 = UK pool 15 balls + white
//	1 = US pool  9 balls + white
//	2 = US pool  8 balls + white
//	3 = 14 - 1 continues pool
//	4 = Speed Pool
//	5 = Killer Pool

#define	UK_POOL		0
#define	US_8BALL		1
#define	US_POOL	 	2
#define	CONTINUOUS	3
#define	SPEED_POOL 	4
#define	KILLER_POOL 5
#define	NMS_POOL		6
#define	TRICK_SHOT 	7

#define	DEFAULT_GAME_TYPE UK_POOL

/* ------------------------------------------------------------------- */
#define	LOW  	0
#define	HIGH 	1
/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
#define	COLLISION_SIZE 18
#define	MAX_SPEED 	128
#define	MAX_SPEED1 	30
#define	BALL_SCALE	16
#define	scanline		320
#define	SCALE			56
/* ------------------------------------------------------------------- */
#define	HEAD_STRING	35

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* For SVGA.C */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

flag  use_UNIVBE;

//----------------------------------------------------------------------------
long	pointerx,pointery;
//ext	long	save_frame;
long	end_tab,old_opt;
long	game_over;
char	*game_pal;
long	ran1,ran2;
/* ------------------------------------------------------------------- */
//	KEYBOARD SCANNING WORKSPACE
ubyte	keys_repeat	[ 256 ];
ubyte	keys_jp		[ 256 ];
ubyte	keys_lastjp	[ 256 ];
char	keys		[ 256 ];
/* ------------------------------------------------------------------- */
long		player;
long		start_time;

	ubyte	*left_name_buff;
	ubyte	*right_name_buff;
  long TOURNAMENT_PLAY;

ubyte	*panel;
ubyte	*txt;
ubyte	*collision_pointer;
ubyte	*bits_pointer;
ubyte	*cue_table;
ubyte	*font_image;
ubyte	*gfx_addr;
unsigned char *old_background;
long height;

double	cuex,cuey;
double	oldcuex,oldcuey;
long		first_shot,potted_flag;
long		show_shot_flag;

float		sin_tab		[ 360 ];

int		oldmouse_xdiff, oldmouse_ydiff;
int		oldmouse_buttons;
int		mouse_x, mouse_y;
int		mouse_xdiff, mouse_ydiff;
int		mouse_xold, mouse_yold;
int		mouse_buttons;
int		oldmouse_buttons;
word		mbuttons_jp;

long		CAMERA_SET;
/* ------------------------------------------------------------------- */
ubyte	*bits_buffer;	
ubyte	*bits_buffer_off;	
ubyte	*screen_buffer;	
ubyte	*screen_buffer1;	
ubyte	*flash_buffer;	
ubyte	*font_buffer;	

long	frame_count;

/* ------------------------------------------------------------------- */

long	line_col;
long	view_line;

//----------------------------------------------------------------------------

long	save_time_value[10];
long	time_value[10];

//----------------------------------------------------------------------------
long	scene_type;
/* ------------------------------------------------------------------- */

#define write 0x3c8
#define datas  0x3c9

#define SA			(ubyte*)0xa0000
#define VGAMODE 18
#define MCGAMODE 19
#define SCREENXS 320
#define SCREENYS 200

#define VRL	(SCREENXS*SCREENYS)

#define	DI()		_disable();
#define	EI()		_enable();

/* ------------------------------------------------------------------- */
//	PC KEYBOARD EQUATES

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

#define	LEFT_BUTTON 			1
#define	RIGHT_BUTTON			2
#define	CENTER_BUTTON			4
/* ------------------------------------------------------------------- */
//#define	alphabetandnumbers 37

#define	KEYS2SCAN	53					//	A-Z 0-9 /

/* ------------------------------------------------------------------- */
#define	cue_width		14
#define	cue_tip_size	10

/* ------------------------------------------------------------------- */
#define	WHITE_BALL	0	//position in object list
#define	BLACK_BALL	0 //13	//position in object list

#define	default_line_size	80
#define	max_line_size		180

#define	off_tab 190
#define	white_y 60
/* ------------------------------------------------------------------- */
#define	MAXBALLS 22

// RESTORE: ballstruct is in FIXBALL.H, fixed point (BALL.C's was doubles)


//----------------------------------------------------------------------------
#define	PLAYER1	0

	long	game_type;
	long	LAST_BALL;

	long	video_mode;
	double	speed_meter;

//----------------------------------------------------------------------------

//ext		long	CAMERA_MODE;

//----------------------------------------------------------------------------
long	COMPUTER_TURN;
int 	page;
long	width,height;
long	FX_FLAG;
long	Player_Active;
long	butt;
long	Cue_mode;

//----------------------------------------------------------------------------

long	ORDER_COUNT;
long	Continuous_Score[20][2];
long	top_bumper_value,hmess,frame_check,pocket_countdel,save_cx,save_cy,save_pocketball;
long	vgamode,poting_flag;
long	save_nine_ball_count,nine_ball_count,D_LEVEL,save_aim,LOWEST_BALL_HIT_FIRST_FLAG;
long	comp_del,left_or_right,got_best_angle,com_frac;
long	step_balls;
double	track_pan;


//-----------------------------------------------------
//-----------------------------------------------------
/* counter for number of shots player has */

/* counter for amount of objects in ach room */
long	room_cnt;
long	contrast;
/* flag for foul message print */
long   oldm,erase_del;

/* room type */
long	Location;

/* flag for ball hitting cushion/rail */
long	cushion_hit,white_hit_cushion;

/* dont keep on re tracking to next ball */
long	aim_done;

/* flag reset after breaking, first shot */
long	break_flag;

/* continuous pool ball nominate counter */
long	ball_nominate;

/* nominated pocket counter */
long	designated_pocket;

/* buffer pointer to save balls xy position in triangle */
long	*continuous_xy_rack;

/* rendering size */
long rendx,rendy,screen_size;

/* flag to say choosen a new game within a game */
long	new_game_selected;

/* killer game player turn counter */
long	killer_turn;

/* killer pool lives counters */
long	lives_left[MAX_KILLER_PLAYERS];

unsigned char 	palette[256*3];
byte 	remap_palette[256*3];
ubyte 	pallettebuf[256*3];

int 	iff_width,iff_height;

/* counter for number of players in killer at once, max of 8 */
int	killer_players;

/* counter mfor erasing old turn marker */
long	old_killer_turn;

long	return_x,return_width;

/* flags for checking conditions in pre-calculated ball movement */

long	FIRST_COLLISION;
long	FIRST_CUSH;

/* flag to say do low res 2d drawing conversions */
long	out_of_menu;

/* size of scan line 320 or 640 */
long	line_size;

long	check_load_res;

/* frame number counter */
long	frame_num;

/* counter for number of frames to be played */
long	set_frames;
/* flag to detect if cursor over an option ?*/
long	cursor_active;


long	escape_position_x;
long	escape_position_y;


struct ball_setup_st {
	double x,y;
	unsigned char ball_type;
};

/* joystick axis checks */
word	jx,jy,jc;
word	JOY_RIGHT;
word	JOY_LEFT;
word	JOY_UP;
word	JOY_DOWN;

word	LEFTJOYVAL;
word	RIGHTJOYVAL;
word	UPJOYVAL;
word	DOWNJOYVAL;

byte	bmask;
long	X_PAD,Y_PAD;

/* 2d ball position structures */

ballstruct ball[MAXBALLS];
ballstruct saveball[MAXBALLS];
ballstruct saveball_lines[MAXBALLS];
ballstruct replayball[MAXBALLS];

long	*balls_xy;		// RESTORE: 8.24, was double

/* flag for ball tracking lines show */
long	BALL_TRACKING;

/* 640/480 font draw buffers */
//ext	int  ptable[1256];
char buffer[65536];

/* andys timer flags */
unsigned char timer_active;
unsigned long timer_frame_count;
unsigned long frames;

long x_hit_pos, y_hit_pos;


long	ac,diam;

#define	VSA			(( ubyte* ) 0xa0000 )
  long	SVGA_logical_length;
long	current_video_mode;
long	last_sy_written_to_vga;
long	VBLcount;
long	numberofVBLSsincelast;
long	svgadoublebuffer;

char	baize_pen_number;

long	safetorestartcdmusicdelay;
flag	isitsafetorestartcdmusic;
ubyte	*RAMallocatedbase_addr;
long	ramavailable;
long	available_ram;
long	TOTALramallocated;
long	free_bytes_left;
char	*dummy;
long	ramused;

#define	MAXRAMBLOCKS	384
void	*allocated_blocks		[ MAXRAMBLOCKS ];
long	allocated_blocks_length	[ MAXRAMBLOCKS ];
long	how_many_blocks;


flag		joy_selected;
short	joy_leftflip;
short	joy_rightflip;
signed short  joy1_x;
signed short  joy1_y;
signed short  joy2_x;
signed short  joy2_y;
unsigned short joy_pad;
flag	joy1_is_left;
flag	joy1_is_right;
flag	joy1_is_up;
flag	joy1_is_down;
flag	joy1_left_click;
flag	joy1_right_click;
flag	joy1_up_click;
flag	joy1_down_click;

flag	joy_button1_on;
flag	joy_button2_on;
flag	joy_button1_click;
flag	joy_button2_click;

long Last_Handle;

flag	mouse_connected;
long	VBL_COUNTER_FOR_CD;
flag	music_enabled;
flag	sb_present;
#define	VID640X480X16	18					/*	VGAMODE	*/
#define	VID320X200X256	19					/*	MCGAMODE	*/
#define	svga640x400x256	0x100
#define	svga640x480x256	0x101
#define	svga800x600x256	0x103				/*	MULTIBALL MODE	*/

#define DAC_WRITE_ADR	0x3c8				/*	DAC WRITE ADDRESS	*/
#define DAC_DATA  		0x3c9				/*	DAC DATA REGISTER	*/
flag	recentkeypress;


#define POT_RED_BALL      1  
#define POT_COLOUR_BALL 2

#define	THREE_FOULS_FOUL	0
#define	HIT_BLACK_FOUL		1
#define	PAUSE_FOUL			2
#define	BALL_IN_HAND_FOUL	3
#define	TIME_FOUL_FOUL		4
#define	HIT_YELLOW_WRONG_BALL_FOUL	 5
#define	HIT_RED_BALL_FOUL	 6
#define	MISSED_ALL_BALLS_FOUL  7
#define	POT_BLACK_WRONG_FOUL		8
#define	POT_WRONG_BALL_FOUL		9
#define	TWO_SHOTS_FOUL		10
#define	POT_WHITE_WRONG_BALL_FOUL		11
#define	POT_8_BALL_WRONG_FOUL	12
#define	POT_9_BALL_WRONG_FOUL	13
#define	MISSED_RAIL_FOUL		14
#define	POT_YELLOW_WRONG		15
#define	LOSE_LIFE_FOUL		16
#define	GAME_OVER_FOUL		17
#define  HIT_SPOT_WRONG_BALL_FOUL 18
#define  HIT_STRIPE_WRONG_BALL_FOUL 19
#define  POT_BLACK_WRONG_FOUL_OTHER_PLAYER_WINS 21
#define  POT_BLACK_CORRECT 22
#define  LOWEST_BALL_NOT_HIT_FIRST 23
#define  MISSED_RAIL_US8BALL  24
#define  MISSED_BALL_NO_BALL_IN_HAND 25
#define  MISSED_BALL_LOSE_A_LIFE 26
#define  REPLAY_LAST_SHOT 27
#define  REPLAY_LAST_SAVED_SHOT 28
#define  CHOOSE_COLOUR          29
#define	POT_WHITE_WRONG_BALL_FOUL_CHOOSE_COLOUR	30
#define  HIT_COLOURED_BALL_FIRST 31
#define  POT_COLOUR_BALL_WRONG_FOUL 32
#define  CHOOSE_POCKET 33
#define  HIT_BLACK_NO_BALL_IN_HAND 34
#define 	HIT_RED_BALL_FOUL_NO_IN_HAND 43
#define  CHOOSE_COLOUR_SNOOKER        44

//----------------------------------------------------------------------------
/* types of fouls allowed */
#define	GOOD_SHOT        	0
#define	HIT_WRONG_BALL   	1
#define	MISSED_ALL_BALLS 	2
#define	POTTED_WRONG_BALL	4
#define	POTTED_CORRECT_BALL 5
#define	POTTED_BLACK_WRONG	6
#define	GAME_OVER			8
#define	POTTED_BLACK		8
#define	ALL_BALLS_GONE		9
#define	POTTED_HI_BALL		10
#define	WHITE_BALL_POTTED	11
#define	NO_BALL_HIT_CUSHION	12


long	first_hit;
long	computer_counter;

double	tmppan_angle;

long	UNDO_SHOT,save_frame_count;

long		startx;
long		starty;
	long		startx1;
long		starty1;

long		shoot_mode;

unsigned char *old_background, *viewport_screen_buffer;
unsigned char *cue_old_background;

double	collision_angle_to_ball;
								
long	render_cue,cue_index,update_cue;
long	bounce_tb;
long	bonus_pocket;

/* ----------------------------- */
/* start updating game clock flag*/
long	draw_clock,draw_cue;

/* ----------------------------- */
/*change controls to vertual pool*/
long	VIRTUAL_FLAG;
/* ----------------------------- */
  long	force_exit;
/* ----------------------------- */
/* all replay last shot varibles */		
double	*replay_data;
double	replay_pan_angle;
double	replay_speed_meter;
long	replay_butt;
long	replay_x_hit_pos;
long	replay_y_hit_pos;
long	replay_avalible;
/* ----------------------------- */

double max_cush_x;
 double min_cush_x;
 double max_cush_y;
 double min_cush_y;

long	computer_first_shot;
long	move_ball_mode;
long	ball_to_move;

long	Frames_played[10][10];
long	turbo_shot_mode;


/*---------------------------------*/
long 	currentband;

typedef	struct	{
						long		obj_type;
						
						long		foul_type;
						long 		ball_type;
						long		potted_count;
						long		speed_foul;
						long	points;

				} playersstruct;

#define MAXPLAYERS 2

playersstruct players[MAXPLAYERS];

#define	PANEL_ON_TIME	75
long	show_panel;
long fsize;
#define	PCFP		FILE*
long	iffread_width;
long	iffread_height;
ubyte	*bak_screen;

double	PI2,PI,deg2rad;


char	player_names[32][32];

long viewport_width, viewport_height;


#define	COPY_TO_RAM	128
#define	COPY_TO_SCREEN	129
#define	DUMP_TO_SCREEN	130
ubyte	*ball_data;
ubyte	*foul_data;
ubyte	*foulboard_data;
ubyte	*icon_mask;
long	foul;

/* -------------------------------------------- */
/* table containing optional values */
	long 	oldGeneral_Rules[50];
	long	General_Rules[50];
/* -------------------------------------------- */
#define	TABLE_FRICTION						General_Rules[0]
#define  FLIP_CONTROLS                 General_Rules[1] /*done*/
#define	DIFFICULTY							General_Rules[2]
#define  FRAME_REPLAYS                 General_Rules[3]
#define	UK_TWO_SHOTS						General_Rules[4] /*done*/
#define	UK_1_SHOT_CARRY 					General_Rules[5] /*done*/
#define	UK_FREE_BALL						General_Rules[6] /*done*/
#define	UK_NOMINATE_BLACK					General_Rules[7]

#define	US_NOMINATE_8BALL_POCKET		General_Rules[8]
#define  MUST_HIT_RAIL                 General_Rules[9] 
#define  US_8_RESPOT_ON_ILLIGAL_POT    General_Rules[10] /*done*/

#define	KILLER_IN_OFF						General_Rules[11] /*done*/
#define	NMS_RESPOT_ON_ILLEGAL_POT		General_Rules[12] /*done*/
#define	SPEED_PENALTY_ON_FOULS			General_Rules[13]	/*done*/

#define  detail_level                  General_Rules[14]	/*done*/
#define  zoom_after_shot_flag          General_Rules[15]	/*done*/
#define  SPORT_TYPE                    General_Rules[16]	/*done*/
#define  zoom_flag                     General_Rules[17]	/*done*/
#define  tournament_game_type          General_Rules[18]	/*done*/


#define	DETAIL_FLAG                   General_Rules[19] 
#define	NUMBER_OF_LIVES_IN_KILLER		General_Rules[20]	/*done*/
#define	NUMBER_OF_PLAYERS_IN_KILLER	General_Rules[21] /*done*/
#define  NUMBER_OF_FRAMES              General_Rules[22] /*done*/
#define	CONTINUOUS_POINTS             General_Rules[23] /*done*/


/* -------------------------------------------- */
/* all game foul types */

#define	MODEM_PLAY		2
#define	SERIAL_PLAY		3
#define	NETWORK_PLAY	4

/* ----------------------------- */
/* option flags */

#define	TABLE_TYPE General_Rules[24] 
#define	LOCATION General_Rules[25] 
#define	RESOLUTION General_Rules[26] 
#define	TABLESIZE_TYPE General_Rules[27] 
#define	GAME_TYPE General_Rules[28]
#define	whichroom General_Rules[29]
#define	right_shift	General_Rules[30]
#define	left_shift	General_Rules[31]
#define 	rright_shift General_Rules[32]
#define	rleft_shift	General_Rules[33] 
#define	play_flag General_Rules[34] 
#define	check_player General_Rules[35] 
#define	PLAYERS_SELECTED General_Rules[36] 

#define	mouse_speed General_Rules[37] 
#define  line_length General_Rules[38]
#define	left_player	General_Rules[39] 
#define	right_player General_Rules[43] 
#define  frame_set General_Rules[44]
#define	RESOLUTION_FLAG General_Rules[45] 

long	replay_flag	;
long	exitscreen	;


long	lasttilt,lastcamz;
long	cam_tilt,dummys,tilt;
long	camx,camy,camz,oldcamz;

double	pan_angle,com_pan,pan,lastpan;

long	Scores[20][6];

int	potted_ball[32];
long	saved_ball_count;
long	pot_count;
long	first_foul;

ubyte	*killer_gfx;


flag	playing_cd_music;
long	Track2_Length;
long	VTOC_buf[100];
long	CDlength;
long	CDtracks;
long	Two_Shots;

ubyte	shad_tab[256];
ubyte	shad_tab_icon[256];

long	left_arrow_pressed;
long	right_arrow_pressed;
long	up_arrow_pressed;
long	down_arrow_pressed;
long	left_button_pressed;
long	right_button_pressed;
long	cd_track,cd_frame_count;
long	current_trackplaying,CD_Tracks;

long	NO_CD;


ubyte	*serial_buffer;
long	oldplayers_scores[16];
long	players_scores[16];
long	players_total_scores[32];
long	killer_winner;
ubyte	*player_buffer;
ubyte	*replay_panel;


ubyte	*proj_data;
ubyte	*collision_screen1;
ubyte	*computer_buffer;

#define	LOWRES_PLAYBUTT proj_data+900000
#define	LOWRES_FFWBUTT  proj_data+900000+50000
#define	LOWRES_REWBUTT  proj_data+900000+100000
#define	LOWRES_SAVEBUTT proj_data+900000+160000
#define	LOWRES_WHICH	 proj_data+900000+260000
#define	SKULL	proj_data



#define	MAX_PLAYERS	16
struct	player_stats	{
		long	handicap;

		long	potting_ratio;		/* number of balls in pocket */
		long	avg_shot_power;	/* taken from speed meter in game */
		long	trick_shots;		/* number of good shots, determined by number
							            of cushions object ball bounces off before	
										   being potted..*/										
		long	fluke_rating;		/* fluke rating , how many snookers you get out
										   of */
		long	last_total_score;	/* total of points from last game */
		long	shots_taken;		/* total number of shots taken in all game types*/
		double	ball_mileage;		/* how far cue ball has travelled in all game type */

};

struct	player_stats	player_info[MAX_PLAYERS*2];

long	oldpointerx[2],oldpointery[2],pname;

long	nms_colour,nms_pocket;/*which 3 pockets for colours to go down */
long	BALL_SET_IS,exit_frame;

//#define	UK	1
//#define	US	2

char sound_active;
long	sample;
long	interlace;
long	STEVES_3D;
long	speed_pot;
long	RENDER_TIME;
long	total[10][2];

#define	MASTER	0
#define	SLAVE	1

long	master;
long	master_counter;
long	extern_master_counter;
long	trigger_master_counter;
long	old_player_turn,old_turn_count;

long	extern_event[50];

long	shot_taken;

long	white_ball_potted_flag;
long	missed_all_balls_flag;
long	hit_black_first_flag;
long	hit_yellow_ball_flag;
long	hit_red_ball_flag;
long	ball_in_hand_flag;
long	potted_red_wrong_flag;
long	two_shots_flag;
long	potted_good_ball_flag;
long	potted_hi_ball_flag;;
long	good_shot_flag;
long	potted_black_wrong_flag ;
long	potted_yellow_wrong_flag ;
long	MUSIC_FLAG;
long	FXS_FLAG;
long	LINK_OPTION;
long	quitgame,oldquit;
double	cue_pan,old_cue_pan,extern_cue_pan[3];

/* sound effect vars */

#define 	MAX_VOLUME		127

long	CDvolume;
long	sfx_volume;
long	new_cd,Track2_Length;


flag	sb_present;
long	sb_card;
long	sb_port;
long	sb_int;
long	sb_dma;
flag	are_intro_flics_on_the_hard_disk;
flag	are_in_game_flics_on_the_hard_disk;
flag	got_vesa_videocard;
long	vesa_card_ram;
int	cue_angle;

long	current_drive;
text	drive_and_path[100];
long	wrapper_flag;

/* buffer where i write players shorten names to */

char	tmp_name_str[80];
char	*name_ptr;
long	killer_pot,only_once,end_of_killer_game;
long	lastcallcounter;
long	killer_order[8];
double	external_ball_x;
double	external_ball_y;
double	extern_cue_ball[2];
flag		extern_syn,syn_flag;
flag		extern_quit;
flag		extern_framequit;
flag	update_cue_ball;
long	whichcol,count;
long	extern_whichcol;
ubyte	*collision_screen;
long	*gfx_link_list,link_pos;
long	VBLsecondspassed ;
long	cdlengthinseconds;
long	oldroom;
long	in_players_room,name_index;
ubyte	*clearsource;
long	lastframenum;
flag	fade_out_cd;
 long	save_cdvolume,volume_frac;
 long	fade_status;
ubyte	*connect_data;
ubyte	*save_shot_buffer;
long	show_saved_shot_flag,show_trick_shot;
long	replay_status,replay_active;
int	num_frames_camera_still;
int	num_frames_still;
ubyte	*backscreen_buffer,*backscreen_buffer1;
char *steves_memory;
long	slot;
char	*textures,*brightness_table;
long	UK_OR_US_BALL_SET,camera_mode;
long	lined_up_shot,fade_flag,trick_name_selected,tracking_lines;
double	save_cue_speed;
long	frame_rate,random_value;	// RESTORE: frame_rate whole frames per second, random_value 8.24 (were double)


long	ROOM;
  OBJECT *ball_ptr[22];
  long	line_pointer_index[22];
long	line_pos[22];
  VERTEX1 line_pointers[22][600];
  long   line_markers[22][600];

long	activated_lines,save_ball_count;
  short ball_hits_pos;
long	flash_count,num_lines;
long	flash_ball1,ball_to_pocket;
char	pocket_num;
ubyte	*tournament_buffer;
flag	no_tournament;
  long	VESA_wingran;							//	VESA GRANULARITY IN KBYTES

/*---------------------------------*/

//#define NUM_CUSH_POINTS		256

// RESTORE: struct cush_st is in FIXBALL.H, fixed point as NMS's gcushst
// was (the double copy is gone, and with it the gcushions_* tables)

#define NUM_POCKETS		6

// RESTORE: struct pockets_st is in FIXBALL.H, fixed point as NMS's gpocketsst
struct pockets_st pockets[NUM_POCKETS];

/* Co-ord points for cushions */

#define NUM_CUSH_POINTS_X		2000
#define NUM_CUSH_POINTS_Y	   1000


long save_table_type;
long save_resolution;




struct cush_st cushions_x3[NUM_CUSH_POINTS_X];
struct cush_st cushions_x2[NUM_CUSH_POINTS_X];
struct cush_st cushions_y1[NUM_CUSH_POINTS_Y];
struct cush_st cushions_y4[NUM_CUSH_POINTS_Y];
int cush_list_pos[4];
long max_cush_y2;		// RESTORE: these five 8.24, were double
long min_cush_y3;
long max_cush_x4;
long min_cush_x1;
long bumper_scale;
flag   refresh_whole_screen,check_game_controls;
double cue_position;
int lmb,rmb;
#define MOUSE_X(c) (signed short)((c&0xFFFF0000)>>16)
#define MOUSE_Y(c) (signed short)(c&0xFFFF)
unsigned char mouse_data_ready;
int mouse_x_avg[10];
int mouse_y_avg[10];
int table_position;
VERTEX	temp_vertex,camera_pos,camera_rot;
VERTEX	fixed_camera_pos,fixed_camera_rot;
VERTEX  save_undo_camera_rot,save_undo_camera_pos,save_undo_cue_rot;
int  delay_after_shot;
double cue_weight;
long ball_mass;		// RESTORE: 8.24, was double
int	dont_draw_pointer_flag;
flag amplify;
double cue_delta_table[1],cue_scale;
long NUMBER_OF_LOCATIONS;


long ball_spot_positions[];		// RESTORE: 8.24, were double
long whiteball_spot_positions[];

long TOURNAMENT_ROUND;

long special_table;
long camera_in_pocket;
long DEMO_PLAY;
long ENABLED_SPECIAL_TABLES;
long TOURNAMENT_TABLE_SETUP;
long quit_frame;
long save_frame_for_delay_after_shot;
long tourn_right_player,tourn_left_player;
long in_atract_mode,save_status,atract_sequence;
word demo_pos;
word *demo_buffer;
word tourn_board_scroll_delay;
long *replay_last_shot_buffer;
char *amplify_icon;
word clear_replay_panel_count;
flag ICON_FLAG;
long	debounce_mouse;

long nms_pockets[6];
flag show_replay_flag;
flag end_of_frame_flag;
flag BUILD_LOW_TEXT;
flag BUILD_LOW_NUMS;

flag QUIT_GAME_FLAG;
flag moved_cue_ball_to_head_string_flag;

#define BALL_COL_LIST_SIZE			64
short ball_col_list_pos;

struct ball_col_st ball_col[BALL_COL_LIST_SIZE];	// RESTORE: struct in FIXBALL.H, 8.24

flag BUILD_LOW_TEXT_FOUL;
flag BUILD_HIRES_TEXT;
flag BUILD_HIRES_NUMS;
flag BUILD_HIRES_TEXT_FOUL;
flag shoot_mode_availble;
flag used_free_ball;
long save_two_shots;
flag nominate_colour;
long last_butt;
ubyte *shots_icon;
flag draw_foul_shot_icons;
long camera_frame_count;
word	oldevent_flag;
word	event_flag;


flag SNOOKER_MODE;
ubyte *txt_foul;
word foul_occured;
flag lowest_ball_not_hit_first;
int	len;
VERTEX camera_rot_tmp ;
VERTEX camera_pos_tmp ;
VERTEX old_camera_pos, old_camera_rot;
long   frame_counter;
int ease_speed;
long cue_position_table_ptr;
long replay_cue_position_table_ptr;
double cue_position_table[200];
int replay_cue_angle;
VERTEX cue_rot,cue_pos,fixed_cue_rot;
long save_res;
char	oldbutton_stat[15];
char	button_stat[15];
long panel_scrolly;
long	SCREEN2	;
long	SCR2	;
char cheat_buffer[100];
long cheat_pos;
flag SPEED_POOL_CHEAT_LINES;
flag	lines_on;
#define	MAX_NUMBER_OF_PLAYERS_IN_TOURNAMENT	32
long Tourn_Frames[ MAX_NUMBER_OF_PLAYERS_IN_TOURNAMENT ];
long  Tournemnt_Human_Player_Turn,p1_scratched,p2_scratched;
long  oldTournemnt_Human_Player_Turn,p1_round,p2_round;
  flag panel_flag;
long player_who_won_tournament,max_frame;
flag altkey,shootkey,trackkey;
flag SNOOKER_SHOOT_MODE;
flag calculating_computer_shot;
long	number_of_tracks;
flag points_game,good_trick_shot;
long	hi_index,flash_pos,flashit,extra;
ubyte	*player_font;
ubyte	*board_buffer;
ubyte	*bufferfnt,*hi_txt;
ubyte	*loadsave;
long	buffsize;
long	bordname_index,bordpos;
ubyte	*cancel_data;
long	flash_slot;
flag BUILD_ASCII_TABLE;
long	select_slot,selected_slot;
ubyte  *end_txt,*quitgamegfx;
ubyte	*savgame_buffer;
char	saved_names[6][60];

#define	ERASE_MOUSE		231
#define	MAX_LEN1			12
#define	MAX_LEN2			14
#define	WHITE_PEN		9	//pen colour for erasing background of players
#define	ERASE_PLAYERS_IN_PICTURE_FRAME	138
#define	ATRACT_DELAY	400 
#define	DRAW_HI_SCORE_PAGE	273
#define	DRAW_BLACKBOARD		274
#define	DRAW_SAVEDGAME_TEXT	275
#define	DRAW_SAVEDGAME_TEXT_AND_FLASH	276
#define	DRAW_TRICKSHOT_TEXT 	277
#define	DRAW_AND_FLASH_TRICKSHOT_TEXT 	278
#define	DRAW_SCALE_BAR 279
#define	DRAW_SCALE_BAR1 280
#define	DRAW_KILLER_BACKGROUND 290
 long  quitkey;
 long flash_name_1,flash_name_2;
 long name_frame_diff;
#define FLASH_NAME_TIME 110
 long draw_twice;
 long pocket_nominated;
 long pocket_selected;
 long	first_replay;
 ubyte *rule_screen;
 long quit_flash_count;
 int highest_raise_angle;
 text		*tiltsaverootname;
 text	dirandrootnametouse[ 100 ];
 text		filename[ 100 ];
 ubyte *swing_data,*rbook_data,*snktab_data;
 long last_killer_turn;
	ubyte	*tabs_data;
 VERTEX camera_target , camera_position ;
 ubyte *speedtxt;
 long len_foul;
 long swapping_wrapper;
 long	exit_atract;
 long frame_count_hiscore;
 ubyte	*hi_buffer;
 ubyte	*hi_score_buff;
 long	hiscore_entered[16];
 long	hitime_entered[16];
 char	time_buffer[10];
 long	hi_name;
 flag NEW_GAME_SETUP;
 long oldfoul;
 long save_foul;
 long save_undo_foul;
 long save_move_foul;
 long save_hand_foul;
 long save_zoom_foul;
 long save_english_foul;
 long save_butt_foul;
 long save_game,save_table;
 long last_time_ball_type;
 long ball_hits_count;
 flag first_screen;
 long p1_stored_pot_type,p2_stored_pot_type;
 long shot_position,save_wrapper_track;
 flag POOL_CD;
 long line_set[22];
 long new_count;

