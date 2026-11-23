/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Phillip Trelford's (360X) SVGA */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Mouse control is really cool
/* MODULES
	MAIN.C 		- this file
	SCRATCH.C	- bodge code... (needs a new file and a new name)
	PROCESS.C	- sprite processing code
	LBM.C			- LBM decompress
	HARDWARE.C	- calls to PC specific hardware drivers
	SVGA.C		- SVGA screen drivers
	VESA.C		- VESA library
	SPRITE.C		- sprite handlers
	SPRTLIST.C	- sprite list handlers
	SCROLL.C		- scroll handlers
	PARTICLE.C	- particle handlers
	FIRE.C		- fireing handlers
	MENU.C		- in game menu code
	MISSION.C	- mision breifing
	SELECT.C		- level selection code
	GLOW.C		- colour addition code
	POLY.C		- polygon colour addtion code
	LEVEL.C		- level sprites reading code
	ATTRIB.C		- load sprites attribs reading code
	LINE.C		- line drawing code
	PATH.C		- sprite's path handling code
	PANELS.C		- sprite's panel overlay code plus map editing
	WINDOW.C		- window stuff...
	CODE.ASM		- assembler bits
	CAMERA.ASM	- iff screen saver
*/
	
#define ext
#include	"globals.h"
// Should be in seperate modules...
#include	"ram.c"
#include	"general.c"
#include	"split.c"

void initialise_level(void );
void play_level(void);

long	current_line;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Print String */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Loads the Cartoon Font into memory...

void load_text(void )
{
	unsigned char *dummy;

	dummy=load_file("FONT.BIN");
	font_table=(long *) dummy;
	font_buffer=dummy+(256*4);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Misc Globals */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

char 		string[128];
flag 		burstingb[2];

char		*lbm_buf;
int  		table[256];
int  		index;
long		thrust_factor;
double	friction_factor;

const	double pi=3.14159265;

#include	"scratch.c"

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Pod attached code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */


//#define FALSE	0
//#define TRUE	1

//#define STIFF

void ball_collision2(short ball1, short ball2)
{
	double collision_ang;

	double vel_ball1, vel_ball2;
	double perp_vel_ball1, perp_vel_ball2;
	double new_vel_ball1, new_vel_ball2;
	double momentum_ball1, momentum_ball2;
	double initial_vel_diff, total_initial_momentum;

/* calculate angle of collsion between 2 balls */
	collision_ang=atan2(ball[ball2].x-ball[ball1].x,
							  ball[ball2].y-ball[ball1].y);

/* get ball velocities along this angle */
	vel_ball1=ball[ball1].vx*sin(collision_ang)+ball[ball1].vy*cos(collision_ang);
	vel_ball2=ball[ball2].vx*sin(collision_ang)+ball[ball2].vy*cos(collision_ang);
	initial_vel_diff=vel_ball2-vel_ball1;

/* get ball velocities at perpendicular */
	perp_vel_ball1=-ball[ball1].vy*sin(collision_ang)+ball[ball1].vx*cos(collision_ang);
	perp_vel_ball2=-ball[ball2].vy*sin(collision_ang)+ball[ball2].vx*cos(collision_ang);

/* get momentum along angle */
	momentum_ball1=vel_ball1*ball[ball1].mass;
	momentum_ball2=vel_ball2*ball[ball2].mass;
	total_initial_momentum=momentum_ball1+momentum_ball2;

/* velocity along collision angle will now be same for both balls */
/* get new velocities */

	new_vel_ball1=total_initial_momentum/(ball[ball1].mass+ball[ball2].mass);
	new_vel_ball2=new_vel_ball1;

/* put these new velocities in ball structure */
	ball[ball1].vx=new_vel_ball1*sin(collision_ang)+perp_vel_ball1*cos(collision_ang);
	ball[ball1].vy=new_vel_ball1*cos(collision_ang)-perp_vel_ball1*sin(collision_ang);

	ball[ball2].vx=new_vel_ball2*sin(collision_ang)+perp_vel_ball2*cos(collision_ang);
	ball[ball2].vy=new_vel_ball2*cos(collision_ang)-perp_vel_ball2*sin(collision_ang);

	return;
}

#define GRAVITY		(800)

void gravity(int ball_num, double time)
{
	ball[ball_num].vy+=gravity_factor*time;
}

void friction(int ball_num, double time, double fric)
{
	ball[ball_num].vy-=ball[ball_num].vy*fric*time;
	ball[ball_num].vx-=ball[ball_num].vx*fric*time;
}

void move(int ball_num, double time)
{
	ball[ball_num].y+=ball[ball_num].vy*time;
	ball[ball_num].x+=ball[ball_num].vx*time;
}

void andy_thrust(int ball_num, double angle, double force, double time)
{
	double acc_x, acc_y;

	acc_x=force*sin(angle)/ball[ball_num].mass;
	acc_y=force*cos(angle)/ball[ball_num].mass;

	ball[ball_num].vx+=acc_x*time;
	ball[ball_num].vy-=acc_y*time;

	return;
}

#define TIME_STEP	0.0017
//#define ROPE_MAX	26.0
//#define ROPE_MIN	25.7
// Current system for changing rope size doesn't work

void update_all(flag pod_attached)
{
	double delta_x,delta_y;
	double hyp,error;

	if(ships_gravity==0)
	{
		gravity(0,TIME_STEP);
		gravity(1,TIME_STEP);
	}
	else
	{
		gravity(0,-TIME_STEP);
		gravity(1,-TIME_STEP);
		ships_gravity--;
	}
	friction(0,TIME_STEP,friction_factor);
	friction(1,TIME_STEP,friction_factor);

	delta_x=ball[0].x-ball[1].x;
	delta_y=ball[0].y-ball[1].y;

	if(pod_attached==TRUE)
	{
		if (sqrt((delta_x*delta_x)+(delta_y*delta_y))>rope_max)
			ball_collision2(0,1);
#ifdef STIFF
		if (sqrt((delta_x*delta_x)+(delta_y*delta_y))<rope_min) {
			ball_collision2(0,1);
#endif

		move(1,TIME_STEP);
	}
	move(0,TIME_STEP);

	// Fudge... (should share the error a bit more angley...)

	if(pod_attached==TRUE)
	{
	delta_x=ball[0].x-ball[1].x;
	delta_y=ball[0].y-ball[1].y;

	hyp=sqrt((delta_x*delta_x)+(delta_y*delta_y));
  
	if(hyp>rope_max)
	{
		error=(hyp-rope_max)/4;
		if( (ball[0].x) > (ball[1].x) ) 
		{
			ball[0].x-=error;
			ball[1].x+=error;
		}
		else
		{
			ball[0].x+=error;
			ball[1].x-=error;
		}
		if( (ball[0].y) > (ball[1].y) ) 
		{
			ball[0].y-=error;
			ball[1].y+=error;
		}
		else
		{
			ball[0].y+=error;
			ball[1].y-=error;
		}
	}
	}
	return;
}

void ship_pickup_pod(flag *pod_attached)
{
	int i;

	if(*pod_attached==YES)
	{
		sprites[pod_no].obj[obj_temp1]=0;
		*pod_attached=NO;
		sprites[pod_no].obj[obj_x]=sprites[pod_no].x;
		sprites[pod_no].obj[obj_y]=sprites[pod_no].y;
		sprites[pod_no].action=ACTION_FALL;
		sprites[pod_no].obj[obj_temp7]=0;
		sprites[pod_no].obj[obj_temp8]=32768+16384;
		sprites[pod_no].obj[obj_temp9]=0;
		sprites[pod_no].obj[obj_temp10]=1024+512;
		return;
	}

// Draw two lines vertically to show pickup action...

	*pod_attached=NO;

	// Scan for PODs to pick up
	for(i=0;i<MAX_SPRITES;i++)
	{
//		if( sprites[i].action==ACTION_PICKUP_POD ) break;
		if( sprites[i].name==NAME_pod4 )
		{
			if(sprites[i].obj[obj_temp1]!=0) break;
		}
	}

	if(i==MAX_SPRITES) return;
	sprites[i].obj[obj_temp1]=2;


//	sprites[i].action=ACTION_DYING;
	sprites[i].action=ACTION_NONE;

	*pod_attached=YES;
	ball[1].x=sprites[i].x;
	ball[1].y=sprites[i].y;
	ball[1].vx=0;
	ball[1].vy=0;

	pod_no=i;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* For .INI File Read */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag	decode_if_yes_or_no( text chr )
{
	switch( chr )
		{
		case	'Y':
		case	'y':
				return YES;
				break;

		case	'N':			 
		case	'n':
				return NO;
				break;

		default:
				return NEITHER;
		}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Read 360X.INI File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag read_ini_file(void)
{
	FILE	*fp;
	char	tempstrg[ 80 ];

	if (( fp = fopen( "360X.INI", "r" )) == NULL )
	{
		printf( "\nINI file not present\n" );
		return FAIL;
	}

 	fscanf( fp, "%s", tempstrg );	//	read	SAFE:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	if (( use_UNIVBE = decode_if_yes_or_no( tempstrg[ 0 ] )) == NEITHER ) return FAIL;

 	fscanf( fp, "%s", tempstrg );	//	read	DOS4GWPRO:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	if (( use_DOS4GWPRO = decode_if_yes_or_no( tempstrg[ 0 ] )) == NEITHER ) return FAIL;

 	fscanf( fp, "%s", tempstrg );	//	read	SAFE:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	if (( safe_mode = decode_if_yes_or_no( tempstrg[ 0 ] )) == NEITHER ) return FAIL;

 	fscanf( fp, "%s", tempstrg );	//	read	HIRES:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	if (( full_screen = decode_if_yes_or_no( tempstrg[ 0 ] )) == NEITHER ) return FAIL;

 	fscanf( fp, "%s", tempstrg );	//	read	SCROLL:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	if (( burst_scroll = decode_if_yes_or_no( tempstrg[ 0 ] )) == NEITHER ) return FAIL;

 	fscanf( fp, "%s", tempstrg );	//	read	ROPE:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	rope_length = atoi( tempstrg );

 	fscanf( fp, "%s", tempstrg );	//	read	THRUST:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	thrust_factor = atoi( tempstrg );

 	fscanf( fp, "%s", tempstrg );	//	read	GRAVITY:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	gravity_factor = atoi( tempstrg );

 	fscanf( fp, "%s", tempstrg );	//	read	FRICTION:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	friction_factor = atoi( tempstrg );

 	fscanf( fp, "%s", tempstrg );	//	read	MASS 0:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	ship_mass=ball[0].mass = atoi( tempstrg );

 	fscanf( fp, "%s", tempstrg );	//	read	MASS 1:
	if ( tempstrg[ strlen ( tempstrg ) -1 ] != ':' ) return FAIL;
 	fscanf( fp, "%s", tempstrg );
	ball_mass=ball[1].mass = atoi( tempstrg );

	return TRUE;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Process Keys */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	long		bank_factor;
	long		bank_factor2;
	double 	andy_angle=PI;
	long		craft_flash_count;
	flag		craft_flashing;
	long		ball_x,ball_y;

//	long		frac_craft_x;
//	long		frac_craft_y;

void process_keys(void )
{
	long		rotated;
	double 	temp;
	double	x,y,dx,dy;
	double	tdx,tdy;
	double	adx,ady;
//	double	fdx,fdy;
	double 	old_x,old_y;
	long		boost;
	long		rand_factor;

	scan_for_keysjp();

	if(keys[kESC]==128) escape=1;

	if( keys_jp[kT]==128)
	{
		if(mouse_connected==TRUE)
		{
			if(mouse_mode==TRUE) mouse_mode=FALSE;
			else mouse_mode=TRUE;
		}
	}

	if( keys_jp[kP]==128)
	{
		if(mouse_connected==TRUE)
		{
			if(path_mode==TRUE) path_mode=FALSE;
			else
			{
				path_mode=TRUE;
				current_line=1;
			}
		}
	}

	if(mouse_mode==FALSE)
	{

	x=frac_craft_x;
	y=frac_craft_y;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Shoot */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	Fire_Weapons();

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Rotate Left & Right */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	rotated=0;
	bank_factor=0;
	if(bank_factor2>0) 
	{
		bank_factor2-=64;
		if(bank_factor2==0) bank_factor=0;
	}
	if(keys[kA]==128 || keys[kLARROW]==128) 
	{
		rotated--;
		craft_angle--;
		bank_factor=64;
		bank_factor2+=64;
	}
	if(keys[kD]==128 || keys[kRARROW]==128)
	{
		rotated++;
		craft_angle++;
		bank_factor=64*4;
		bank_factor2+=64;
	}
	if(rotated!=0)
	{
		craft_angle&=63;
		craftn=((craft_angle+16)&63); //+1+bank_factor+bank_factor2;
		andy_angle=(craft_angle)*((PI*2)/64);
	}
	if(bank_factor2>0 && rotated==0) 
	{
		bank_factor2-=64;
		if(bank_factor2==0) bank_factor=0;
	}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Cheat Keys */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	if(keys_jp[kC]==128)								// 'C' = Collision Toggle
	{
		if(collision_on==TRUE) collision_on=FALSE;
		else collision_on=TRUE;
	}

	if(keys_jp[kL]==128)								// 'L' = Turn Light On
	{
		if(light_on==TRUE) light_on=FALSE;
		else light_on=TRUE;
	}

	if(keys_jp[kS]==128)								// 'S' = Scroll Toggle
	{
		if(burst_scroll==TRUE)
		{
			burst_scroll=FALSE;
			burst_x=0;
			burst_y=0;
		}
		else burst_scroll=TRUE;
	}

	if(keys_jp[kM]==128)								// 'M' = Menu
	{
		process_menu();
		rope_max=rope_length;
		rope_min=rope_length-0.3;
		ball[0].mass = ship_mass;
		ball[1].mass = ball_mass;
	}
		
	if(keys[kDARROW]==128)
	{
		dx=0;
		dy=0;
		ball[0].vx=0;
		ball[0].vy=0;
	}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Thrust */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	/*
	if(keys_jp[kZ]==128)
	{
		if (pod_attached==TRUE)
		{
			pod_attached=FALSE;
			sprites[pod_no].action=ACTION_FALL;
			sprites[pod_no].dx=0;
			sprites[pod_no].dy=16384;
		}
		else
		{
			pod_attached=TRUE;
			sprites[pod_no].visible=TRUE;
		}
	}
	*/

	old_x=x;
	old_y=y;

	boost=0;
	if(keys[kRIGHTSHIFT]==128 || keys[kUARROW]==128) boost=1;
	if(keys[kB]==128) boost=4;

	if(boost>0)
	{
		play_sample(1,samples_start[FX_THRUSTE],samples_len[FX_THRUSTE]);

		andy_angle=((craft_angle+16)&63)*((PI*2)/64);
		andy_thrust(0,andy_angle,thrust_factor*boost, TIME_STEP);
		rand_factor=(newrand()*6)>>15;
		temp=((craft_angle+29+rand_factor)&63)*((pi*2)/64);
		tdx=cos(temp)*1.5;
		tdy=sin(temp)*1.5;
		temp=((craft_angle+16)&63)*((pi*2)/64);
		adx=cos(temp)*7;
		ady=sin(temp)*7;
		add_particle((int ) craftx+adx,(int ) crafty+ady,(int ) (tdx*65536),(int ) (tdy*65536));
		add_particle((int ) craftx-adx,(int ) crafty-ady,(int ) (tdx*65536),(int ) (tdy*65536));
			
		if(ships_fuel>0)
		{
			if(ships_fuel_deplete==0)
			{
				ships_fuel_deplete=16;
				ships_fuel_add-=boost;
			}
			else ships_fuel_deplete--;
		}
		temp=(craft_angle)*((pi*2)/64);
	}

	old_x=x; old_y=y;

	update_all(pod_attached);
		
	if(keys_jp[kG]==128)
	{
		ship_pickup_pod(&pod_attached);
		sprites[pod_no].visible=TRUE;
	}

	x=ball[0].x;
	y=ball[0].y;

	if(collision_on==TRUE)
	{
		if(sprite_collision(&seqdata[NAME_CSHIP],craftn,x,y)==TRUE )
		{
			x=old_x;
			y=old_y;
			ball[0].x=x;
			ball[0].y=y;
			ball[0].vx=-ball[0].vx+(ball[0].vx/3);
			ball[0].vy=-ball[0].vy+(ball[0].vy/3);
			play_sample(3,samples_start[FX_SIDCSH1],samples_len[FX_SIDCSH1]);
		}
	}

	if(collision_on==TRUE)
	{
		if(collision_with(&seqdata[NAME_CSHIP],craftn,craftx,crafty)==TRUE)
		{
			if(craft_flashing==NO)
			{
				if(ships_shield>0) ships_shield_add=-4;
				craft_flashing=YES;
				craft_flash_count=16;
				//add_sprite(NAME_EXPLOSION,ACTION_PLAYONCE,x+16,y+16);
			}
		}
	}	

	frac_craft_x=x;
	frac_craft_y=y;

	craftx=ball[0].x;
	crafty=ball[0].y;
	ball_x=ball[1].x;
	ball_y=ball[1].y;

	}
}
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Main Procedure */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void main( byte argc, text* argv[2] )
{
	flag		exit_to_DOS;
	flag		test_level;
	long		plays;
	int		i;

	init_heap(1);

//	load_attributes();
//	exit(-1);

	generate_cos_sin_tables();

	pod_attached=FALSE;
	collision_on=TRUE;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Bodge level? */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
 	
	test_level=FALSE;
	if ( argc == 2 )
	{
		test_level=TRUE;
		level_fname=argv[1];
	}

	ball[0].mass=100;
	ball[1].mass=50;
	gravity_factor=800;
	thrust_factor=500000;
	friction_factor=0.5;
	rope_length=32;
	safe_mode=YES;
	full_screen=NO;
	burst_scroll=TRUE;
	use_UNIVBE=NO;
	use_DOS4GWPRO=NO;
	read_ini_file();
	if(safe_mode==NO) full_screen=NO;
	rope_max=rope_length;
	rope_min=rope_length-0.3;

	init_hardware();

//	view_globe();

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Initialize misc variables */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	make_remapper();

	init_line_list();

	init_triangle();

	ships_fuel_deplete=0;
	ships_fuel_add=32*8;
	ships_fuel=0;
	ships_shield_add=32*8;
	ships_shield=0;
	ships_gravity=0;

 	load_fonts();
	load_text();

	exit_to_DOS=FALSE;
	plays=0;
	// ------------------------------------------------------------
	while(exit_to_DOS==FALSE)
	{
		if(test_level==FALSE)
		{
			i=select();
			if(i>=0) level_fname=get_level_name(i);
			else break;
//			if(i>=0)	mission_briefing(&level_fnames[i].fname);
		}

		initialise_level();

		init_windows();

		play_level();
		plays++;

		if(test_level==TRUE) exit_to_DOS=TRUE;
	}
	// ------------------------------------------------------------

	SVGA_uninit();
	
	clear_sound();

	clearkey();
	mode3();
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Level Initialisation */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void initialise_level(void )
{
	char		*level_pal;
	long 		i;

	clear_sprites();

	scroll_x=0; 
	scroll_y=0;

	clear_heap();									// Clear Memory

	init_particle_list();

	sprintf(filename,"PHIL/%s/LEVEL.PAL",level_fname);
	level_pal=heap_load_file(filename);

	/* Load Level background data */

	for(i=0;i<NAME_END;i++)	seqdata[i].frames=-1;

	load_sprites_resource("COMMON");

	load_level(level_fname);

	place_craft();

	make_edge_array();

//	make_block_collision_array();

	if(full_screen==YES)
	{
	 	SVGA_set_mode(0x103);
//	 	SVGA_set_mode(0x105);
	}
	else
	{
	 	SVGA_set_mode(0x101);
	}
  	asm_set_logical_length(SVGA_logical_length);

	memcpy(iff_palette,level_pal,128*3);
	set_palette(iff_palette,0,256);
	set_pen(0,0,0,0);
	set_pen(80,0,0,0);

	init_scroll();

	init_fire();

	initialise_panels();
	initialise_pauls_panels();

	validate_sprite_list();

	for(i=0;i<8;i++) debug_variable[i]=i;

	view_heap_allocation();
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Play Level Loop */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void 		process_pod_and_craft(void );
void 		update_pod_position(void );
long		linex1[2],liney1[2];
long		linex2[2],liney2[2];

void play_level(void)
{
	long		i;
	for(i=0;i<2;i++)
	{ 
		linex1[i]=craftx; liney1[i]=crafty;
		linex2[i]=craftx; liney2[i]=crafty;
	}
	
	escape=FALSE;
	while (escape==FALSE)
	{
		process_keys();	

		process_pod_and_craft();

		if(mouse_mode==FALSE) update_scroll_xy(craftx,crafty);

		// ...Remove GFX...

		clear_dump_line_list();
		clear_particle_list();

		update_panels();
		update_pauls_panels();

		remove_light();

		process_collisions();

		// Should there be stuff b4 this
	
		update_scroll_edge();


		remove_glow_list();
		init_glow_list();

		process_sprites();

		remove_lines(buf);

		remove_pixels(buf,logical_screen+split_height,bufx[buf],bufy[buf]);
		remove_sprites(buf);

		remove_windows();

		// ...

		if( (bursting==YES) && (safe_mode==YES) )
		{
			dump_the_screen(logical_screen,scroll_x,scroll_y);
		}
		last_scroll_x=scroll_x;
		last_scroll_y=scroll_y;

		move_particles(buf);

		move_fire(buf);

		// ...Draw Graphics...

	  	draw_pixels(buf,logical_screen+split_height,bufx[buf],bufy[buf]);

		draw_windows();
		
		draw_glow_list();

		draw_light();

		draw_lines(buf);
		
		draw_sprites(buf);

		use_dump_line_list();			// draw everything

		set_scroll_position();			// set scroll position
	}

	for(i=0;i<NAME_END;i++)	seqdata[i].frames=-1;

	clear_sprites();
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Some Pod stuff */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void process_pod_and_craft(void )
{
 	if(pod_attached==TRUE)
	{
		sprites[pod_no].x=linex2[buf];
		sprites[pod_no].y=liney2[buf];
		sprites[pod_no].obj[obj_x]=linex2[buf];
		sprites[pod_no].obj[obj_y]=liney2[buf];
	}

	sprites[craft].x=craftx;
	sprites[craft].y=crafty;
	sprites[craft].frame=craftn;
	sprites[craft].visible=TRUE;
	if(craft_flashing==YES)
	{
		craft_flash_count--;
		if(craft_flash_count==0) craft_flashing=NO;
		if( (craft_flash_count&3)==0) sprites[craft].visible=FALSE;
	}
}

void update_pod_position(void )
{
  	if(pod_attached==TRUE)
  	{
  		linex1[buf]=craftx;
  		liney1[buf]=crafty;
  		linex2[buf]=ball_x;
  		liney2[buf]=ball_y;
	}

 	if(pod_attached==TRUE)
	{
		sprites[pod_no].frame=(atan2(ball[0].x-ball[1].x, ball[1].y-ball[0].y)*64)
									/(PI*2);
		sprites[pod_no].frame&=63;
		
		lines[0].x1[buf]=linex1[buf];
		lines[0].y1[buf]=liney1[buf];
		lines[0].x2[buf]=linex2[buf];
		lines[0].y2[buf]=liney2[buf];
		lines[0].line_on=0;
		sprites[pod_no].visible=TRUE;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
