/**************************************
*            INCBIN HEADERS           *
**************************************/

#include	<sgl.h>
#include "defines.h"
#include "types.h"
#include "globals.h"
#include <libsn.h>

#include "control.h"

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void		Print_Sprites(void);
void		Move_Craft(void);
void		Print_Tests(void);
void		andy_thrust(int ball_num, ANGLE angle, FIXED force, FIXED time);
void		update_all(flag pod_attached);
void		ship_pickup_pod(flag *pod_attached);
void		gravity(int ball_num, FIXED time);
void		friction(int ball_num, FIXED time, FIXED fric);
void		move(int ball_num, FIXED time);
void		Init_Ball(void);
void		ProcessFire(Uint16	pad);
void		ProcessRotation(Uint16 pad);
void 		ProcessThrust(Uint16 pad);

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define 	TIME_STEP	(0.0017*65536.0)

#define	PI		3.141592654
#define	pi		3.141592654


/**************************************
*          DECLARE EXTERNALS          *
**************************************/

extern	int		rand(void);
extern	void 		Add_Fire(int frame,int x,int y,int dx,int dy);
extern	void 		Add_Particle(long x,long y,long dx,long dy);
extern	void 		Fire_Weapons(Uint16 data);
extern	LONG		craft;
extern	long		frac_craft_x,frac_craft_y;

/**************************************
*          DECLARE VARIABLES          *
**************************************/

#ifdef	PSX

typedef unsigned    char    Uint8;      
typedef signed      char    Sint8;      
typedef unsigned    short   Uint16;     
typedef signed      short   Sint16;     
typedef unsigned    long    Uint32;     
typedef signed      long    Sint32;     

typedef	    Sint16	ANGLE ;
typedef	    Sint32	FIXED ;

#define	    toFIXED(a)		((FIXED)(65536.0 * (a)))
#define	    DEGtoANG(d)	((ANGLE)((65536.0 * (d)) / 360.0))

// Yet to define - 
sin and cosine tables

#endif


// Conversion of andy_angle from angle into ANGLE -
// First convert the angle into degrees... [angle(0..63)]
// 360/64 = 5.684 degrees = 1 ship_angle
// Hence andy_angle = (angle<<16)*(5.684*65536) / 360

BALL ball[2];

#if 0
BALL ball[2]=
{
	{toFIXED(160),toFIXED(100),toFIXED(0),toFIXED(0),toFIXED(100)  }, 
	{toFIXED(160),toFIXED(125.7),toFIXED(0),toFIXED(0),toFIXED(50) }  
};
#endif

ANGLE	angles[64];

extern	LONG	craft;
LONG		sprtemp;

flag		light_on;
flag		collision_on;
flag		pod_attached;

int		boost;
int		i,iy;
int		craftn;
int		rand_factor;
int		ball_x,ball_y;

int		craft_angle=0;
ANGLE 	andy_angle;
FIXED		tdx,tdy;
FIXED		adx,ady;
FIXED		x,y,dx,dy;
FIXED 	old_x,old_y;
FIXED 	temp;
FIXED		fx,fy,fdx,fdy;
FIXED		friction_factor;
FIXED		mul;

flag		craft_flashing;
int		craft_flash_count;
//long		pod_no;
int		ships_fuel;
int		ships_shield;
int		ships_fuel_deplete;
int		ships_fuel_add;
int		ships_shield_add;
int		ships_gravity;
long		thrust_factor;


/**************************************
*        MOVE THE CRAFT ABOUT         *
**************************************/

void Move_Craft(void)
{
	Uint16	data	=	Smpc_Peripheral[0].data;

	x=craftx;
	y=crafty;

	Fire_Weapons(data);

	ProcessRotation(data);

	old_x=x;
	old_y=y;

	boost=0;
		 
	ProcessThrust(data);

	old_x=x; old_y=y;

	//Brake!!
	if((data & PER_DGT_TZ)==0)
	{
		dx=0;
		dy=0;
		ball[0].vx=0;
		ball[0].vy=0;
	}

	update_all(pod_attached);

	x	=	ball[0].x;
	y	=	ball[0].y;

	craftx	=	(ball[0].x)>>16;
	crafty	=	(ball[0].y)>>16;

	frac_craft_x	=	craftx;
	frac_craft_y	=	crafty;

	ball_x	=	ball[1].x;
	ball_y	=	ball[1].y;

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

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void	ProcessRotation(Uint16 pad)
{
	Sint8	rotated=0;

	if ((pad & PER_DGT_KL)==0)
	{
		rotated--;
		craft_angle--;
	}
	if ((pad & PER_DGT_KR)==0)
	{
		rotated++;
		craft_angle++;
	}
	if(rotated!=0)
	{
		craft_angle	&=	63;
		craftn		=	craft_angle;
	}

}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void ProcessThrust(Uint16 pad)
{

	if((pad & PER_DGT_TB)==0)
	{
		boost	=	1;

		//Play thrust sample and do the thrust calculations

		andy_angle	=	angles[craft_angle];
		andy_thrust(0,andy_angle,(0x100<<16),TIME_STEP);

		//Do the particles
		rand_factor =  (rand()*6)>>15;
		temp	=	angles[(((craft_angle-16)+29+rand_factor)&63)];
		tdx	=	slMulFX( slCos(temp),toFIXED(1.5) );
		tdy	=	slMulFX( slSin(temp),toFIXED(1.5) );

		temp	=	angles[(((craft_angle-16)+14+rand_factor)&63)];
		adx	=	slMulFX(slCos(temp),7*65536);
		ady	=	slMulFX(slSin(temp),7*65536);
		
		Add_Particle((int ) craftx+(adx>>16),(int ) crafty+(ady>>16),(int ) (tdx),(int ) (tdy));
		Add_Particle((int ) craftx-(adx>>16),(int ) crafty-(ady>>16),(int ) (tdx),(int ) (tdy));
			 
		if(ships_fuel>0)
		{
			if(ships_fuel_deplete==0)
			{
				ships_fuel_deplete=16;
				ships_fuel_add-=boost;
			}
			else
			{
				ships_fuel_deplete--;
			}
		}
	}

}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void andy_thrust(int ball_num, ANGLE xangle, FIXED force, FIXED time)
{
	volatile	FIXED acc_x, acc_y,mul;

	mul	=	slMulFX(force,slSin(xangle));
	acc_x	=	slDivFX(ball[ball_num].mass,mul);
	mul	=	slMulFX(force,slCos(xangle));
	acc_y	=	slDivFX(ball[ball_num].mass,mul);


	mul	=	slMulFX(acc_x,time);
	ball[ball_num].vx	+=	mul;
	mul	=	slMulFX(acc_y,time);
	ball[ball_num].vy	-=	mul;

	return;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
  
void update_all(flag pod_attached)
{
	FIXED 	delta_x,delta_y;

	#ifdef	POD
	FIXED 	hyp,error;
	#endif

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

	#ifdef	POD
	if(pod_attached==TRUE)
	{

//		if (sqrt((delta_x*delta_x)+(delta_y*delta_y))>rope_max)
//			ball_collision2(0,1);
//#ifdef STIFF
//		if (sqrt((delta_x*delta_x)+(delta_y*delta_y))<rope_min) {
//			ball_collision2(0,1);
//#endif

		move(1,TIME_STEP);
	}
	#endif

	move(0,TIME_STEP);

	#ifdef POD
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
	#endif

	return;

}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void ship_pickup_pod(flag *pod_attached)
{
	int i;

// Draw two lines vertically to show pickup action...

//	*pod_attached=NO;

	// Scan for PODs to pick up
	for(i=0;i<MAX_SPRITES;i++)
	{
	//	if( sprites[i].action==ACTION_PICKUP_POD ) break;
	}

	if(i==MAX_SPRITES) return;

//	sprites[i].action=ACTION_DYING;

	sprites[i].action=ACTION_NONE;

	*pod_attached=YES;
	ball[1].x	=	sprites[i].x;
	ball[1].y	=	sprites[i].y;
	ball[1].vx	=	0;
	ball[1].vy	=	0;

	pod_no=i;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void gravity(int ball_num, FIXED time)
{
	FIXED	mul;

	mul	=	slMulFX(gravity_factor*65536,time);
	ball[ball_num].vy+=mul;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void friction(int ball_num, FIXED time, FIXED fric)
{

	FIXED	mul;

	mul	=	slMulFX(ball[ball_num].vy,fric);
	ball[ball_num].vy-=slMulFX(mul,time);
	mul	=	slMulFX(ball[ball_num].vx,fric);
	ball[ball_num].vx-=slMulFX(mul,time);

}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void move(int ball_num, FIXED time)
{
	FIXED	mul;

	mul	=	slMulFX(ball[ball_num].vy,time);
	ball[ball_num].y	+=	mul;
	mul	=	slMulFX(ball[ball_num].vx,time);
	ball[ball_num].x	+=	mul;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void	Init_Ball(void)
{

	Uint8	i;

	andy_angle		=	DEGtoANG(0);
	ball[0].mass	=	toFIXED(0.05);
	ball[1].mass	=	toFIXED(50);
	gravity_factor	=	800;
	thrust_factor	=	500000;
	friction_factor=	toFIXED(0.5);
	rope_length		=	32;
	rope_max			=	rope_length;
	rope_min			=	rope_length-0.3;

	for(i=0;i<63;i++)
	{
		angles[i]	=	(65536*(i*5.684))/360;
	}

}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */































































































#if 0

		if(collision_on==TRUE)
		{

//		if(sprite_collision(seqdata[NAME_CSHIP],craftn,x,y)==TRUE )
//		{
//			x=old_x;
//			y=old_y;
//			ball[0].x=x;
//			ball[0].y=y;
//			ball[0].vx=-ball[0].vx+(ball[0].vx/3);
//			ball[0].vy=-ball[0].vy+(ball[0].vy/3);
//			play_sample(3,samples_start[FX_SIDCSH1],samples_len[FX_SIDCSH1]);
//			
//		}

		}

//		if(collision_with(&seqdata[NAME_CSHIP],craftn,craftx,crafty)==TRUE)
//		{
//			if(craft_flashing==NO)
//			{
//				if(ships_shield>0) ships_shield_add=-4;
//				craft_flashing=YES;
//				craft_flash_count=16;
//				//add_sprite(NAME_EXPLOSION,ACTION_PLAYONCE,x+16,y+16);
//			}
//		}











	if ((data & PER_DGT_TA)==0)
	{
		if (pod_attached==TRUE)
		{
			pod_attached=FALSE;
			sprites[pod_no].action=ACTION_FALL;
//			sprites[pod_no].x8=sprites[pod_no].x<<16;
//			sprites[pod_no].y8=sprites[pod_no].y<<16;
			sprites[pod_no].dx=0; //(16384-rand())*8;
			sprites[pod_no].dy=16384;
		}
		else
		{
			pod_attached=TRUE;
			sprites[pod_no].visible=TRUE;
		}
	}



	update_all(pod_attached);

	if ((data & PER_DGT_TA)==0)
	{
		ship_pickup_pod(&pod_attached);
		sprites[pod_no].visible=TRUE;
	}





#endif
