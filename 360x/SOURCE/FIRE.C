
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE DEFINES            *
**************************************/

/**************************************
*        DECLARE VARIABLES            *
**************************************/

/* THE SHIELD WEAPON'S ORBIT RADIUS, PIXELS. THE JULY PS1 CODE HAS 16, THE PC (MAKEBACK/FIRE.C:755) AND THE SATURN (LOGIC/FIRECODE.C:843) 32: KEPT AT THE PS1'S, THE OWNER'S QUESTION (docs/DECISIONS.md).
   -DX360_SHIELD_RADIUS=32 BUILDS THE OTHER, FOR A FRAME. */
#ifndef X360_SHIELD_RADIUS
#define X360_SHIELD_RADIUS	16
#endif
#define SHIELD_RADIUS	X360_SHIELD_RADIUS

UBYTE	weaponammo[9];		/* RESTORE: was [8]; the shield, WEAPONSHIELD, is entry 8 */

/* RESTORE: THE PC'S IMPACT PICTURES (OWNER'S DECISION 2026-11-14: "DO WHATEVER THE PC VERSION DOES"). THE PC ADDS NAME_NXA OR NAME_NXC, PLAYED ONCE (ACTION_PLAYONCE:
   ai_explode, MAKEBACK/AI.S:1850, priority 0, animspeed 2, die at the last frame), WHERE A WEAPON ENDS: NXA ON A WALL HIT OF THE MISSILE (MAKEBACK/FIRE.C:318, KEPT AS THE
   SATURN'S OWN EXPLOSIONS, BELOW), THE HOMING SHOT (:688) AND THE RING (:748); NXC AT THE END OF THE PROTON BALL'S LIFE (:488), THE PRONG (:589) AND THE HOMING SHOT'S
   END OF LIFE (:701). NO SOUND IS PLAYED THERE. THE TWO PICTURES ARE NOT IN ANY ARCHIVE (docs/DECISIONS.md), SO EACH KIND IS DRAWN BY AN EXPLOSION THE GAME ALREADY LOADS, CHOSEN
   UNDER THE OWNER'S DECISION: NXA (16 PARTICLES AT 1.5) BY exp4, 16 BY 20 PIXELS, 23 FRAMES; NXC (64 PARTICLES AT 0 TO 4) BY NAME_EXPLOSION (S_EXP1), 32 BY 34 PIXELS,
   23 FRAMES: THE SAME LENGTH, ABOUT TWICE THE SIZE, AS THE SECOND KIND'S PARTICLES FLY UP TO ABOUT TWICE AS FAST (4 AGAINST 1.5). A STAND-IN; THE NAMES HERE ARE THE ONLY PLACE TO CHANGE IT. */
#define IMPACT_NXA	NAME_exp4
#define IMPACT_NXC	NAME_EXPLOSION
#ifdef X360_TEST_FIRE		/* RESTORE: TEST BUILDS ONLY: IMPACT PICTURES ADDED, BY EVENT: PROTON END, PRONG, HOMING WALL, HOMING END, RING */
LONG testimp[8];			/* [5] CALLS, [6] 7 THE LAST ONE'S x y */
#endif

LONG	fire_on[MAX_FIRE];
LONG	fire_frame[MAX_FIRE];
LONG	fire_x[MAX_FIRE];
LONG	fire_y[MAX_FIRE];
LONG	fire_dx[MAX_FIRE];
LONG	fire_dy[MAX_FIRE];
LONG	fire_bx[MAX_FIRE];
LONG	fire_by[MAX_FIRE];

SHORT	weapontype;
SHORT	freeweaponslots;
SHORT	fire_speed;

enum	{
		WEAPONMISSILE=0,
		WEAPONLASER,
		WEAPONBUCKSHOT,
		WEAPONPROTON,
		WEAPONCHAIN,
		WEAPONPRONG,
		WEAPONHOMING,
		WEAPONCIRCLE,
		WEAPONSHIELD};

enum	{
		NAME_GUIDWIRE,
		NAME_LASWIRE,
		NAME_BCKWIRE,
		NAME_EBWIRE,
		NAME_SMWIRE,
		NAME_AGMWIRE,

		NAME_MINIWIRE,
		NAME_BORDER1};

/**************************************
*       INITIALISE THE FIRING         *
**************************************/

void
Init_Fire(void)
{

LONG	i,id;

	weapontype=0;
	fire_speed=3;
	freeweaponslots=12;

	for(i=0;i<MAX_FIRE;i++)
	{
		fire_on[i]=0;
		fire_x[i]=0;
		fire_y[i]=0;
		fire_dx[i]=0;
		fire_dy[i]=0;
		fire_bx[i]=0;
		fire_by[i]=0;

		id=add_sprite(NAME_CSHIP,ACTION_NONE,-99,-99);

		if(id!=-1)
		{
			projectile[i].id=id;
			projectile[i].on=NO;
			projectile[i].dirangle=0;
			projectile[i].target=-1;
			sprites[id].visible=NO;
		}
	}
}

/**************************************
*           ADD A NEW MISSILE         *
**************************************/

void
Add_Fire(int frame,int x,int y,int dx,int dy)
{

LONG	i,id;

	for(i=0;i<MAX_FIRE;i++)
	{
		if ((projectile[i].on==NO) && (projectile[i].id!=-1))
		{

			projectile[i].on=YES;
			shotsfired++;				/* RESTORE: NEW WORK: STATUS SCREEN */
			projectile[i].hit=NO;
			projectile[i].type=weapontype;
			projectile[i].dirangle=frame;
			projectile[i].target=-1;

			id=projectile[i].id;

			/* RESTORE: NEW WORK (owner's decision). No PS1 code plays a sound
			   on firing. SOUND.H names two: SFX_FIREGUN, "SOUND FOR FIRING
			   GUN", and SFX_FIRELASER. The Saturn port plays its firing
			   sample here, once per projectile and for every weapon
			   (360x-saturn LOGIC/FIRECODE.C:164-307, add_fire). The laser
			   takes SFX_FIRELASER, every other weapon SFX_FIREGUN, at the
			   pitch and volume NMS's other effects use. */
			if (projectile[i].type==WEAPONLASER) SsUtKeyOn(vab,SFX_FIRELASER,0,FXPITCH,0,sfxvolume,sfxvolume);
			else SsUtKeyOn(vab,SFX_FIREGUN,0,FXPITCH,0,sfxvolume,sfxvolume);

			sprites[id].name=NAME_CSHIP;	/* REPLACE WITH PROPER SPRITES */

			switch (projectile[i].type)
			{
				case WEAPONMISSILE:

				projectile[i].livetimer=350;

				sprites[id].name=NAME_MISSILE;
				sprites[id].frame=frame;
				sprites[id].visible=YES;
				sprites[id].x=x;
				sprites[id].y=y;
				sprites[id].obj[obj_temp1]=x*65536;
				sprites[id].obj[obj_temp2]=y*65536;
				sprites[id].obj[obj_temp3]=dx;
				sprites[id].obj[obj_temp4]=dy;
				return;

				case WEAPONLASER:

				projectile[i].livetimer=350;

				/* RESTORE: NMS'S COMMENTED LINE, WITH THE SATURN PORT'S S_SPFIRE (40 FRAMES;
				   GAME.C:438 NAME_spfire) AS THE LASER SHOT (OWNER'S DECISION); THE NAME
				   IS NAME_laserfire BECAUSE A BMF NUMBER HAS NAME_spfire'S VALUE TOO.
				   THE SHOT WAS DRAWN AS THE CRAFT (NAME_CSHIP, SET ABOVE). */
				sprites[id].name=NAME_laserfire;
				sprites[id].frame=0;
				sprites[id].startframe=0;
				sprites[id].endframe=seqdata[(sprites[id].name)].frames-2;
				if(sprites[id].endframe<0) sprites[id].endframe=0;
				sprites[id].visible=YES;
				sprites[id].x=x;
				sprites[id].y=y;
				sprites[id].obj[obj_temp1]=x*65536;
				sprites[id].obj[obj_temp2]=y*65536;
				sprites[id].obj[obj_temp3]=dx;
				sprites[id].obj[obj_temp4]=dy;
				return;

				case WEAPONBUCKSHOT:

				projectile[i].livetimer=350;

				sprites[id].name=NAME_BUCK01;
				sprites[id].frame=0;
				sprites[id].startframe=0;
				sprites[id].endframe=32-2;
//				sprites[id].endframe=seqdata[(sprites[id].name)].frames-2;
//				if(sprites[id].endframe<0) sprites[id].endframe=0;
				sprites[id].visible=YES;
				sprites[id].x=x;
				sprites[id].y=y;
				sprites[id].obj[obj_temp1]=x*65536;
				sprites[id].obj[obj_temp2]=y*65536;
				sprites[id].obj[obj_temp3]=dx;
				sprites[id].obj[obj_temp4]=dy;
				return;

				case WEAPONPROTON:

				projectile[i].livetimer=350;

				sprites[id].name=NAME_EBALL;
				sprites[id].frame=0;
				sprites[id].startframe=0;
				sprites[id].endframe=64-2;
//				sprites[id].endframe=seqdata[(sprites[id].name)].frames-2;
//				if(sprites[id].endframe<0) sprites[id].endframe=0;
				sprites[id].visible=YES;
				sprites[id].x=x;
				sprites[id].y=y;
				sprites[id].obj[obj_temp1]=x*65536;
				sprites[id].obj[obj_temp2]=y*65536;
				sprites[id].obj[obj_temp3]=dx;
				sprites[id].obj[obj_temp4]=dy;
				return;

				case WEAPONCHAIN:

				projectile[i].livetimer=350;

				/* RESTORE: THE CHAIN SHOT IS THE CHAIN LINK (MISC/CHLINK, 16 FRAMES), AS THE PC NAMES IT: NAME_CHAINB (MAKEBACK/FIRE.C:175; THE LINE ABOVE WAS COMMENTED OUT) */
				sprites[id].name=NAME_CHAINB;
				sprites[id].frame=0;
				sprites[id].startframe=0;
				sprites[id].endframe=seqdata[(sprites[id].name)].frames-2;
				if(sprites[id].endframe<0) sprites[id].endframe=0;
				sprites[id].visible=YES;
				sprites[id].x=x;
				sprites[id].y=y;
				sprites[id].obj[obj_temp1]=x*65536;
				sprites[id].obj[obj_temp2]=y*65536;
				sprites[id].obj[obj_temp3]=dx;
				sprites[id].obj[obj_temp4]=dy;
				return;

				case WEAPONPRONG:

				projectile[i].livetimer=240;

				sprites[id].name=NAME_AGMINE;
				sprites[id].frame=0;
				sprites[id].startframe=0;
				sprites[id].endframe=32-2;
//				sprites[id].endframe=seqdata[(sprites[id].name)].frames-2;
//				if(sprites[id].endframe<0) sprites[id].endframe=0;
				sprites[id].visible=YES;
				sprites[id].x=x;
				sprites[id].y=y;
				sprites[id].obj[obj_temp1]=x*65536;
				sprites[id].obj[obj_temp2]=y*65536;
				sprites[id].obj[obj_temp3]=dx;
				sprites[id].obj[obj_temp4]=dy;
				return;

				case WEAPONHOMING:

				projectile[i].livetimer=100;

				sprites[id].name=NAME_MISSILE;
				sprites[id].visible=YES;
				sprites[id].frame=frame;
				sprites[id].x=x;
				sprites[id].y=y;
				sprites[id].obj[obj_temp1]=x*65536;
				sprites[id].obj[obj_temp2]=y*65536;
				sprites[id].obj[obj_temp3]=dx;
				sprites[id].obj[obj_temp4]=dy;
				return;

				case WEAPONCIRCLE:

				projectile[i].livetimer=frame;

				sprites[id].name=NAME_MISSILE;
				sprites[id].visible=NO;
				sprites[id].frame=0;
				sprites[id].x=x;
				sprites[id].y=y;
				sprites[id].obj[obj_temp1]=x*65536;
				sprites[id].obj[obj_temp2]=y*65536;
				sprites[id].obj[obj_temp3]=dx;
				sprites[id].obj[obj_temp4]=dy;
				return;

				case WEAPONSHIELD:

				projectile[i].livetimer=frame;

				sprites[id].name=NAME_BUCK01;
				sprites[id].frame=0;
				sprites[id].startframe=0;
				sprites[id].endframe=32-2;
//				sprites[id].endframe=seqdata[(sprites[id].name)].frames-2;
//				if(sprites[id].endframe<0) sprites[id].endframe=0;
				sprites[id].visible=YES;
				sprites[id].x=x;
				sprites[id].y=y;
				sprites[id].obj[obj_temp1]=x*65536;
				sprites[id].obj[obj_temp2]=y*65536;
				return;
			}
		}
	}
}

/* RESTORE: completion, rebuilt from the PC version (owner's decision 2026-11-13, "every gameplay feature completed in other ports"). THE PC'S
   find_nearest_sprite (MAKEBACK/SPRITE.C:985), UNCHANGED: THE NEAREST ON AND SHOOTABLE SPRITE WITHIN A SCREEN'S DIAGONAL (320*320+240*240, SQUARED) OF
   THE SHOT, OR -1. THE JULY PS1 CODE HAD THE CALL COMMENTED OUT (WEAPONHOMING, BELOW), SO THE HOMING SHOT NEVER HOMED. */
#ifdef X360_TEST_FIRE	/* RESTORE: TEST BUILDS ONLY: THE PROTON BALL (x, y, dx, dy, cycles left) FOR tests/gameplay/x360-weapons.sh */
LONG testball[8];
#endif
#ifdef X360_TEST_FIRECOST
/* RESTORE: NEW WORK, TEST BUILDS ONLY (X360_CFLAGS=-DX360_TEST_FIRECOST=W, W THE WEAPON 0 TO 6): AT GAME CYCLE 100 ALL TWELVE SHOT SLOTS ARE FILLED WITH SHOTS OF THAT
   WEAPON, SLOW ENOUGH TO STAY IN OPEN GROUND FOR THE WHOLE RUN, AND EVERY Move_Fire IS TIMED WITH ROOT COUNTER 2 (SYSTEM CLOCK / 8) INTO testfiret (CALLS, SUM, MAX, IN
   COUNTER TICKS, 8 CPU CYCLES EACH): THE WORST CASE OF THE SHOT SHAPE TESTS, ALL 12 TESTED EVERY CYCLE, NONE ENDING EARLY ON A HIT. */
LONG testfiret[4];
void
Test_Fill_Shots(void)
{
	LONG	k;
	double	a,ddx,ddy;

	weapontype=X360_TEST_FIRECOST;
	for (k=0;k<MAX_FIRE;k++)
	{
		a=k*((pi*2)/MAX_FIRE);
		ddx=cos(a)*0.02;
		ddy=sin(a)*0.02;
		Add_Fire((k*5+16)&63,(long ) craftx,(long ) crafty,(long ) (ddx*65536),(long ) (ddy*65536));
	}
	*(volatile USHORT *) 0x1F801124=0x0200;		/* ROOT COUNTER 2: SYSTEM CLOCK / 8, FREE RUNNING */
}
#endif
#ifdef X360_TEST_FIRE	/* RESTORE: TEST BUILDS ONLY: A SHIELD PIECE (x, y, the craft's x, y, its place in the orbit) */
LONG testshield[8];
#endif
#ifdef X360_TEST_FIRE	/* RESTORE: TEST BUILDS ONLY: WHERE THE LAST MISSILE ENDED (x, y) */
LONG testdeath[2];
LONG testchain[8];		/* RESTORE: TEST BUILDS ONLY: THE CHAIN SHOT (x, y, frame, 1 if its name is NAME_CHAINB, then where it ended x, y) */
#endif
#ifdef X360_TEST_FIRE	/* RESTORE: TEST BUILDS ONLY: THE HOMING SHOT (x, y, target sprite, target x, y, dx, dy) FOR tests/gameplay/x360-weapons.sh */
LONG testfire[8];
#endif
static LONG
find_nearest_sprite(LONG x,LONG y)
{
	LONG	i;
	LONG	dx,dy,dist;
	LONG	best,best_dist;

#ifdef X360_TEST_NOHIT		/* RESTORE: TEST BUILDS ONLY (-DX360_TEST_NOHIT): NO CREATURE IS A TARGET, SO A HOMING SHOT FLIES STRAIGHT */
	return -1;
#endif
	best=-1;
	best_dist=(320*320)+(240*240);
	for(i=0;i<MAX_SPRITES;i++)
	{
		if ((sprites[i].on==TRUE) && (sprites[i].shootable==YES))
		{
			dx=sprites[i].x-x;
			dy=sprites[i].y-y;
			dist=(dx*dx)+(dy*dy);
			if(dist<best_dist)
			{
				best_dist=dist;
				best=i;
			}
		}
	}
	return best;
}

/* RESTORE: ONE IMPACT PICTURE, AT THE PLACE THE SHOT ENDED (event IS ONLY FOR THE TEST COUNTS) */
static void
Impact(int name,int event,int x,int y)
{
	LONG	n;

#ifdef X360_TEST_NOIMPACT		/* RESTORE: TEST BUILDS ONLY: THE BUILD WITHOUT THE PICTURES, FOR THE REVIEW FRAMES' "TODAY" */
	return;
#endif
	n=add_sprite(name,ACTION_PLAYONCE,x,y);
#ifdef X360_TEST_FIRE
	if (n>=0) testimp[event]++;
	testimp[5]++; testimp[6]=x; testimp[7]=y;
#endif
}

/**************************************
*          MOVE THE MISSILES          *
**************************************/

void
Move_Fire(void)
{

int		i,id,j,x=0,y=0,f=0;
int		rand_factor;
long	temp1;
double	temp,tdx,tdy;
short	homex,homey,homesprite;
#ifdef X360_TEST_FIRE
#ifndef X360_TEST_FIRE_DX
#define X360_TEST_FIRE_DX	86
#endif
#ifndef X360_TEST_FIRE_DY
#define X360_TEST_FIRE_DY	(-34)	/* THE PLUNGER'S HEIGHT RELATIVE TO THE CRAFT (-DX360_TEST_FIRE_DY=0 PUTS IT IN THE LINE OF FIRE) */
#endif
	/* RESTORE: TEST BUILDS ONLY (X360_CFLAGS=-DX360_TEST_FIRE WITH THE WARP FLAGS OF tests/gameplay/x360-weapons.sh): AT GAME CYCLE 110 ONE PLUNGER (NAME 32) IS PUT 86 PIXELS RIGHT OF AND
	   34 ABOVE THE CRAFT (THE WARP'S): THE NEAREST CREATURE TO THE HOMING SHOT WHEN IT LOOKS, AND OFF THE LINE OF A STRAIGHT ONE */
	{
		LONG	tf;
		if (gamecycle==110)
		{
			tf=add_sprite(32,ACTION_NONE,craftx+X360_TEST_FIRE_DX,crafty+X360_TEST_FIRE_DY);
			if (tf!=-1) { sprites[tf].move_ptr=0; sprites[tf].shootable=YES; sprites[tf].frame=0; sprites[tf].startframe=0; sprites[tf].endframe=0; }
		}
	}
#endif

int		tempx,tempy;

	for(i=0;i<MAX_FIRE;i++)
	{
		if(projectile[i].on==YES)
		{
			id=projectile[i].id;
			switch (projectile[i].type)
			{

				case WEAPONMISSILE:

				x=sprites[id].obj[obj_temp1];
				y=sprites[id].obj[obj_temp2];
				x+=sprites[id].obj[obj_temp3];
				y+=sprites[id].obj[obj_temp4];
				sprites[id].obj[obj_temp1]=x;
				sprites[id].obj[obj_temp2]=y;

				x=x/65536;
				y=y/65536;
				sprites[id].x=x;
				sprites[id].y=y;

				tempx=rand_factor=(rand()&3)-2;
				tempy=rand_factor=(rand()&3)-2;

				Add_Particle(x+tempx,y+tempy,(int) (0*65536),(int) (0*65536));

//				if (Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x,sprites[id].y)==TRUE

				if ((Shot_Mask_Collision(&shotguided_mask,x,y,sprites[id].frame)==TRUE)
					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight)
				{
#ifdef X360_TEST_FIRE
					testdeath[0]=x; testdeath[1]=y;
#endif
					projectile[i].on=NO;
					sprites[id].x=-99;
					sprites[id].y=-99;
					sprites[id].visible=NO;
					freeweaponslots++;

					for(j=0;j<16;j++)
					{
						rand_factor=(rand()*16)>>15;
						temp=((projectile[i].dirangle-16+29+rand_factor)&63)*((pi*2)/64);
						tdx=cos(temp)*1.5;
						tdy=sin(temp)*1.5;
						Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}
//					add_sprite(NAME_NXA,ACTION_PLAYONCE,x,y);
					/* RESTORE: THE SATURN PORT'S OWN EXPLOSION AT A MISSILE'S WALL HIT
					   (LOGIC/FIRECODE.C:387-389, WITH ITS OFFSETS); exp5 THERE IS COMMENTED OUT */
					add_sprite(NAME_exp4,ACTION_PLAYONCE,x-5,y-5);
					add_sprite(NAME_exp7,ACTION_PLAYONCE,x,y);
					add_sprite(NAME_exp6,ACTION_PLAYONCE,x+5,y-5);
				}
				break;

				case WEAPONLASER:

				x=sprites[id].obj[obj_temp1];
				y=sprites[id].obj[obj_temp2];
				x+=sprites[id].obj[obj_temp3];
				y+=sprites[id].obj[obj_temp4];
				sprites[id].obj[obj_temp1]=x;
				sprites[id].obj[obj_temp2]=y;

				if ((sprites[id].framecounter--)<=0)
				{
					sprites[id].framecounter=sprites[id].anim_delay_rate;
				 	temp1=sprites[id].frame;
					temp1++;
					if( temp1>=sprites[id].endframe ) temp1=sprites[id].startframe;
					sprites[id].frame=temp1;
				}

				x=x/65536;
				y=y/65536;
				sprites[id].x=x;
				sprites[id].y=y;

//				if(Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x,sprites[id].y)==TRUE

				if ((Shot_Mask_Collision(&shotspfire_mask,x,y,sprites[id].frame)==TRUE)


					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight)
				{
					projectile[i].on=NO;
					sprites[id].x=-99;
					sprites[id].y=-99;
					sprites[id].visible=NO;
					freeweaponslots++;

					for(j=0;j<8;j++)
					{
						rand_factor=(rand()*16)>>15;
						temp=((f-16+29+rand_factor)&63)*((pi*2)/64);
						tdx=cos(temp)*1.5;
						tdy=sin(temp)*1.5;
						Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}
				}
				break;

				case WEAPONBUCKSHOT:

				x=sprites[id].obj[obj_temp1];
				y=sprites[id].obj[obj_temp2];
				x+=sprites[id].obj[obj_temp3];
				y+=sprites[id].obj[obj_temp4];
				sprites[id].obj[obj_temp1]=x;
				sprites[id].obj[obj_temp2]=y;

				if ((sprites[id].framecounter--)<=0)
				{
					sprites[id].framecounter=sprites[id].anim_delay_rate;
					temp1=sprites[id].frame;
					temp1++;
					if( temp1>=sprites[id].endframe ) temp1=sprites[id].startframe;
					sprites[id].frame=temp1;
				}

				x=x/65536;
				y=y/65536;
				sprites[id].x=x;
				sprites[id].y=y;

//				if(Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x,sprites[id].y)==TRUE

				if ((Shot_Mask_Collision(&shotbuck01_mask,x,y,sprites[id].frame)==TRUE)

					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight
					||sprites[id].x==-99)
				{
					projectile[i].on=NO;
					sprites[id].x=-99;
					sprites[id].y=-99;
					sprites[id].visible=NO;
					freeweaponslots++;

					for(j=0;j<8;j++)
					{
						rand_factor=(rand()*16)>>15;
						temp=((f-16+29+rand_factor)&63)*((pi*2)/64);
						tdx=cos(temp)*1.5;
						tdy=sin(temp)*1.5;
						Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}
				}
				break;

				case WEAPONPROTON:

				x=sprites[id].obj[obj_temp1];
				y=sprites[id].obj[obj_temp2];
				x+=sprites[id].obj[obj_temp3];
				y+=sprites[id].obj[obj_temp4];
				sprites[id].obj[obj_temp1]=x;
				sprites[id].obj[obj_temp2]=y;

				if ((sprites[id].framecounter--)<=0)
				{
					sprites[id].framecounter=sprites[id].anim_delay_rate;
				 	temp1=sprites[id].frame;
					temp1++;
					if( temp1>=sprites[id].endframe ) temp1=sprites[id].startframe;
					sprites[id].frame=temp1;
				}

				x=x/65536;
				y=y/65536;
				sprites[id].x=x;
				sprites[id].y=y;
#ifdef X360_TEST_FIRE
				testball[0]=x; testball[1]=y; testball[2]=sprites[id].obj[obj_temp3]; testball[3]=sprites[id].obj[obj_temp4]; testball[4]=projectile[i].livetimer;
#endif

				if ((rand()&3)==0)
				{
					tdx=(rand()&7)-4;
					tdy=(rand()&7)-4;
					Add_Particle_Acceleration(x+tdx,y+tdy,
						(int) (tdx*(65536/512)),(int) (tdy*(65536/512)),
						(int) (0-tdx*(65536/256)),(int) (0-tdy*(65536/256)));
				}

//				if(Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x-4,sprites[id].y)==TRUE

				/* RESTORE: completion, from the PC version (MAKEBACK/FIRE.C:446-470, ITS FOUR sprite_collision TESTS 4 PIXELS LEFT, RIGHT, ABOVE AND BELOW THE BALL).
				   THE JULY PS1 CODE TESTED THE BALL'S OWN POINT FOUR TIMES, SO A WALL FLIPPED ITS SPEED TWICE AND IT PASSED THROUGH. A POINT, NOT THE SHAPE: SEE THE
				   WALL-BY-SHAPE ROW. */
				if ((Shot_Mask_Collision(&shoteball_mask,x-4,y,sprites[id].frame)==TRUE)
					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight)
				{
					sprites[id].obj[obj_temp3]=-sprites[id].obj[obj_temp3];
				}

//				if(Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x+4,sprites[id].y)==TRUE

				if ((Shot_Mask_Collision(&shoteball_mask,x+4,y,sprites[id].frame)==TRUE)
					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight)
				{
					sprites[id].obj[obj_temp3]=-sprites[id].obj[obj_temp3];
				}

//				if(Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x,sprites[id].y-4)==TRUE


				if ((Shot_Mask_Collision(&shoteball_mask,x,y-4,sprites[id].frame)==TRUE)
					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight)
				{
					sprites[id].obj[obj_temp4]=-sprites[id].obj[obj_temp4];
				}


//				if(Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x,sprites[id].y+4)==TRUE


				if ((Shot_Mask_Collision(&shoteball_mask,x,y+4,sprites[id].frame)==TRUE)
					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight)
				{
					sprites[id].obj[obj_temp4]=-sprites[id].obj[obj_temp4];
				}

				if (projectile[i].livetimer!=0) projectile[i].livetimer--;
				else
				{
					for(j=0;j<64;j++)
					{
						temp=((projectile[i].dirangle-16+29+j)&63)*((pi*2)/64);
						tdx=cos(temp)*(rand() % 5);
						tdy=sin(temp)*(rand() % 5);
						Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}

					Impact(IMPACT_NXC,0,x,y);	/* RESTORE: THE PC'S NAME_NXC (MAKEBACK/FIRE.C, PROTON BALL END :488), A STAND-IN (ABOVE) */

					projectile[i].on=NO;
					sprites[id].x=-99;
					sprites[id].y=-99;
					sprites[id].visible=NO;
					freeweaponslots++;

				}
				break;

				case WEAPONCHAIN:

				x=sprites[id].obj[obj_temp1];
				y=sprites[id].obj[obj_temp2];
				x+=sprites[id].obj[obj_temp3];
				y+=sprites[id].obj[obj_temp4];
				sprites[id].obj[obj_temp1]=x;
				sprites[id].obj[obj_temp2]=y;

				if ((sprites[id].framecounter--)<=0)
				{
					sprites[id].framecounter=sprites[id].anim_delay_rate;
				 	temp1=sprites[id].frame;
					temp1++;
					if( temp1>=sprites[id].endframe ) temp1=sprites[id].startframe;
					sprites[id].frame=temp1;
				}

				x=x/65536;
				y=y/65536;
				sprites[id].x=x;
				sprites[id].y=y;

//				if(Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x,sprites[id].y)==TRUE

#ifdef X360_TEST_FIRE
				testchain[0]=x; testchain[1]=y; testchain[2]=sprites[id].frame; testchain[3]=(sprites[id].name==NAME_CHAINB);
#endif
				/* RESTORE: THE PC TESTS THE CHAIN SHOT'S SHAPE, AS THE OTHER SHOTS' (MAKEBACK/FIRE.C:522, sprite_collision ON THE SHOT'S FRAME); THE POINT TEST WAS THE PS1'S */
				if ((Shot_Mask_Collision(&shotchlink_mask,x,y,sprites[id].frame)==TRUE)
					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight)
				{
#ifdef X360_TEST_FIRE
					testchain[4]=x; testchain[5]=y;
#endif
					projectile[i].on=NO;
					sprites[id].x=-99;
					sprites[id].y=-99;
					sprites[id].visible=NO;
					freeweaponslots++;

					for(j=0;j<8;j++)
					{
						rand_factor=(rand()*16)>>15;
						temp=((f-16+29+rand_factor)&63)*((pi*2)/64);
						tdx=cos(temp)*1.5;
						tdy=sin(temp)*1.5;

						Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}
				}
				break;

				case WEAPONPRONG:

				x=sprites[id].obj[obj_temp1];
				y=sprites[id].obj[obj_temp2];
				x+=sprites[id].obj[obj_temp3];
				y+=sprites[id].obj[obj_temp4];
				sprites[id].obj[obj_temp1]=x;
				sprites[id].obj[obj_temp2]=y;

				if ((sprites[id].framecounter--)<=0)
				{
					sprites[id].framecounter=sprites[id].anim_delay_rate;
				 	temp1=sprites[id].frame;
					temp1++;
					if( temp1>=sprites[id].endframe ) temp1=sprites[id].startframe;
					sprites[id].frame=temp1;
				}
	
				x=x/65536;
				y=y/65536;
				sprites[id].x=x;
				sprites[id].y=y;

//				if(Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x,sprites[id].y)==TRUE

				if ((Shot_Mask_Collision(&shotmine_mask,x,y,sprites[id].frame)==TRUE)
					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight
					|| projectile[i].livetimer==0)
				{
					projectile[i].on=NO;
					sprites[id].x=-99;
					sprites[id].y=-99;
					sprites[id].visible=NO;
					freeweaponslots++;

					for(j=0;j<64;j++)
					{
						temp=((projectile[i].dirangle-16+29+j)&63)*((pi*2)/64);
						tdx=cos(temp)*(rand() % 5);
						tdy=sin(temp)*(rand() % 5);
						Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));

					}

					Impact(IMPACT_NXC,1,x,y);	/* RESTORE: THE PC'S NAME_NXC (MAKEBACK/FIRE.C, PRONG :589), A STAND-IN (ABOVE) */
				}
				else projectile[i].livetimer--;
				break;

				case WEAPONHOMING:

				x=sprites[id].obj[obj_temp1];
				y=sprites[id].obj[obj_temp2];
				x+=sprites[id].obj[obj_temp3];
				y+=sprites[id].obj[obj_temp4];
				sprites[id].obj[obj_temp1]=x;
				sprites[id].obj[obj_temp2]=y;

				x=x/65536;
				y=y/65536;
				sprites[id].x=x;
				sprites[id].y=y;

				homesprite=projectile[i].target;

				if (projectile[i].livetimer==75)
				{
					if (homesprite==-1)
					{
						homesprite=find_nearest_sprite(x,y);	/* RESTORE: THE PC'S CALL (MAKEBACK/FIRE.C:614), WAS COMMENTED OUT */
						if (homesprite!=-1)
						{
							projectile[i].target=homesprite;
						}
					}
				}

				if (projectile[i].livetimer!=0)
				projectile[i].livetimer--;

				if (homesprite!=-1)
				{
					homex=sprites[homesprite].x;
					homey=sprites[homesprite].y;

					tempx=rand_factor=(rand()&3)-2;
					tempy=rand_factor=(rand()&3)-2;
					Add_Particle(homex+tempx,homey+tempy,(int) (tempx*65536),(int) (tempy*65536));

#ifdef X360_TEST_FIRE
					testfire[0]=x; testfire[1]=y; testfire[2]=homesprite; testfire[3]=homex; testfire[4]=homey; testfire[5]=sprites[id].obj[obj_temp3]; testfire[6]=sprites[id].obj[obj_temp4];
#endif
					if (x!=homex)
					{
						if (x>homex)
						{
							if (sprites[id].obj[obj_temp3]/65536>=-fire_speed)
							sprites[id].obj[obj_temp3]-=65536/3;
						}
						else
						{
							if (sprites[id].obj[obj_temp3]/65536<=fire_speed)
							sprites[id].obj[obj_temp3]+=65536/3;
						}
					}

					if (y!=homey)
					{
						if (y>homey)
						{
							if (sprites[id].obj[obj_temp4]/65536>=-fire_speed)
							sprites[id].obj[obj_temp4]-=65536/3;
						}
						else
						{
							if (sprites[id].obj[obj_temp4]/65536<=fire_speed)
							sprites[id].obj[obj_temp4]+=65536/3;
						}
					}
				}

				tempx=rand_factor=(rand()&3)-2;
				tempy=rand_factor=(rand()&3)-2;
				Add_Particle(x+tempx,y+tempy,(int) (0*65536),(int) (0*65536));

//				if(Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x,sprites[id].y)==TRUE

				if ((Shot_Mask_Collision(&shotguided_mask,x,y,sprites[id].frame)==TRUE)
					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight)
				{
					projectile[i].on=NO;
					projectile[i].target=-1;

					sprites[id].x=-99;
					sprites[id].y=-99;
					sprites[id].visible=NO;
					freeweaponslots++;

					for(j=0;j<16;j++)
					{
						rand_factor=(rand()*16)>>15;
						temp=((projectile[i].dirangle-16+29+rand_factor)&63)*((pi*2)/64);
						tdx=cos(temp)*1.5;
						tdy=sin(temp)*1.5;
						Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}
					Impact(IMPACT_NXA,2,x,y);	/* RESTORE: THE PC'S NAME_NXA (MAKEBACK/FIRE.C, HOMING WALL HIT :688), A STAND-IN (ABOVE) */
				}

				if (projectile[i].livetimer==0)
				{
					for(j=0;j<64;j++)
					{
						temp=((projectile[i].dirangle-16+29+j)&63)*((pi*2)/64);
						tdx=cos(temp)*(rand() % 5);
						tdy=sin(temp)*(rand() % 5);
						Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}

					Impact(IMPACT_NXC,3,x,y);	/* RESTORE: THE PC'S NAME_NXC (MAKEBACK/FIRE.C, HOMING END OF LIFE :701), A STAND-IN (ABOVE) */

					projectile[i].on=NO;
					sprites[id].x=-99;
					sprites[id].y=-99;
					sprites[id].visible=NO;
					freeweaponslots++;
				}
				break;

				case WEAPONCIRCLE:

				if (projectile[i].livetimer!=0) projectile[i].livetimer--;

				x=sprites[id].obj[obj_temp1];
				y=sprites[id].obj[obj_temp2];
				x+=sprites[id].obj[obj_temp3];
				y+=sprites[id].obj[obj_temp4];
				sprites[id].obj[obj_temp1]=x;
				sprites[id].obj[obj_temp2]=y;

				x=x/65536;
				y=y/65536;
				sprites[id].x=x;
				sprites[id].y=y;

				Add_Ring((128-projectile[i].livetimer)/6,x,y);	/* RESTORE: THE PC'S add_circle (MAKEBACK/FIRE.C:727), WAS COMMENTED OUT */

//				if(Sprite_Collision(&seqdata[(sprites[id].name)],sprites[id].frame,
//					sprites[id].x,sprites[id].y)==TRUE


				if ((Shot_Mask_Collision(&shotguided_mask,x,y,sprites[id].frame)==TRUE)
					|| x<0 || x>=mapwidth
					|| y<0 || y>=mapheight)
				{
					projectile[i].on=NO;
					sprites[id].x=-99;
					sprites[id].y=-99;
					sprites[id].visible=NO;
					freeweaponslots++;

					for(j=0;j<16;j++)
					{
						rand_factor=(rand()*16)>>15;
						temp=((projectile[i].dirangle-16+29+rand_factor)&63)*((pi*2)/64);
						tdx=cos(temp)*1.5;
						tdy=sin(temp)*1.5;
						Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}
					Impact(IMPACT_NXA,4,x,y);	/* RESTORE: THE PC'S NAME_NXA (MAKEBACK/FIRE.C, RING WALL HIT :748), A STAND-IN (ABOVE) */
				}
				break;

				case WEAPONSHIELD:

			  	temp=(projectile[i].livetimer+1)*((pi*2)/64);
				tdx=cos(temp)*SHIELD_RADIUS;
				tdy=sin(temp)*SHIELD_RADIUS;

				x=craftx*65536;
				y=crafty*65536;

				x+=(tdx*65536);
				y+=(tdy*65536);

				sprites[id].obj[obj_temp1]=x;
				sprites[id].obj[obj_temp2]=y;

				x=x/65536;
				y=y/65536;

				sprites[id].x=x;
				sprites[id].y=y;
#ifdef X360_TEST_FIRE
				testshield[0]=x; testshield[1]=y; testshield[2]=craftx; testshield[3]=crafty; testshield[4]=projectile[i].livetimer;
#endif

				if ((sprites[id].framecounter--)<=0)
				{
					sprites[id].framecounter=sprites[id].anim_delay_rate;
				 	temp1=sprites[id].frame;
					temp1++;
					if( temp1>=sprites[id].endframe ) temp1=sprites[id].startframe;
					sprites[id].frame=temp1;
				}

				if ((rand()&3)==0)
				{
					Add_Particle(x,y,(int) (0),(int) (0));
				}

				projectile[i].livetimer++;
				if (projectile[i].livetimer==64) projectile[i].livetimer=0;

				break;
			}
		}
	}
}

/**************************************
*  RESTORE: NEW WORK (owner's decision)  *
**************************************/

/* A destroyed creature bursts as a missile does on a wall (Move_Fire,
   WEAPONMISSILE: 16 particles fanned back from the shot's direction), at
   the middle of the creature's frame. No new art. */

void
Death_Burst(LONG x,LONG y,LONG dirangle)
{

int	j;
int	rand_factor;
double	temp;
double	tdx,tdy;

	for(j=0;j<16;j++)
	{
		rand_factor=(rand()*16)>>15;
		temp=((dirangle-16+29+rand_factor)&63)*((pi*2)/64);
		tdx=cos(temp)*1.5;
		tdy=sin(temp)*1.5;
		Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
	}
	/* RESTORE: NMS'S DEATH SCRIPT STARTED AN EXPLOSION (FEBRUARY AND JUNE ai_death:
	   "add_child NAME_EXPLOSION,ai_explode"; the July one is only "die"), the Saturn
	   port draws NAME_EXPLOSION with S_EXP1 (GAME.C:421): OWNER'S DECISION */
	add_sprite(NAME_EXPLOSION,ACTION_PLAYONCE,x,y);
}

/**************************************
*      PROCESS WEAPON COLLISIONS      *
**************************************/

void
process_collisions(void)
{
#ifdef X360_TEST_NOHIT		/* RESTORE: TEST BUILDS ONLY (-DX360_TEST_NOHIT): NO SHOT HITS A CREATURE, SO EVERY SHOT ENDS BY ITS TIME OR A WALL */
	return;
#endif

/* RESTORE: NMS's hit test, under #if 0 and uncalled in July 1996, revived
   (owner's decision); MAIN.C calls it after Move_Fire, as the PC version
   calls it before process_sprites (MAKEBACK/MAIN.C:842). The PC took each
   frame's box from seqdata (PC sprite data the PS1 never loads); here it
   comes from the SPRITE table Print_Sprites draws the creature with: the
   frame's u,v offset and w,h size, moved down by the (h/2)+2 Print_Sprites
   adds to y. Everything else is NMS's code, except the lines marked. */

	SPRITE	*fspr;					/* RESTORE: NEW WORK (owner's decision) */
	short width,height;
	int i,j,x,y,id;
	int name,frame;

	for(i=0;i<MAX_SPRITES;i++)
 	{
		if( (sprites[i].on==TRUE) && (sprites[i].shootable==YES) )
		{		
			name=sprites[i].name;
			x=sprites[i].x;
			y=sprites[i].y;
			frame=sprframe[i];
	
			fspr=sprtable[i];				/* RESTORE: NEW WORK (owner's decision): WAS seqdata[name] */
			if (fspr==NULL) continue;			/* NOT DRAWN, NOTHING TO HIT */
			y+=(fspr->h/2)+2;				/* AS PRINT_SPRITES DRAWS IT */
			fspr+=frame;
			x+=fspr->u;
			y+=fspr->v;
			width=fspr->w;
			if (width==0) width=256;
			height=fspr->h;
			if (height==0) height=256;

			for(j=0;j<MAX_FIRE;j++)
			{
				if(projectile[j].on==YES)
				{
					id=projectile[j].id;
					if(sprites[id].x>=x && sprites[id].x<(x+width))
					{
						if(sprites[id].y>=y && sprites[id].y<(y+height)) 
						{
		 				 	projectile[j].on=NO;
							sprites[id].visible=NO;
							sprites[id].x=-99;
							sprites[id].y=-99;
							freeweaponslots++;
							shotsontarget++;		/* RESTORE: NEW WORK (owner's decision) */
							/* RESTORE: NEW WORK (owner's decision): SOUND.H's SFX_GOTHOT,
							   "SOUND FOR HOTSPOT HIT", where a shot hits a creature, as the
							   Saturn port plays one here (360x-saturn LOGIC/FIRECODE.C:1185,
							   itself under #if 0 in September 1996). */
							SsUtKeyOn(vab,SFX_GOTHOT,0,FXPITCH,0,sfxvolume,sfxvolume);
							if (Hit_Sprite(i)==YES)	/* RESTORE: NEW WORK (owner's decision): WAS move_ptr=on_hit_ptr, SEE PROCESS.C */
								Death_Burst(x+width/2,y+height/2,projectile[j].dirangle);
							break;
						}
					}
				}
			}
		}
	}

}

/**************************************
*    PANEL WEAPON SPRITE STRUCTURES   *
**************************************/
#if 0
panelstruct ppanels[]={
	NAME_GUIDWIRE	,0,0,0,	640-64,112,
	NAME_LASWIRE	,0,0,0,	640-64,112,
	NAME_BCKWIRE	,0,0,0,	640-64,112,
	NAME_EBWIRE	,0,0,0,	640-64,112,
	NAME_SMWIRE	,0,0,0,	640-64,112,
	NAME_AGMWIRE	,0,0,0,	640-64,112,
	NAME_GUIDWIRE	,0,0,0,	640-64,112,
	NAME_LASWIRE	,0,0,0,	640-64,112,
	NAME_BCKWIRE	,0,0,0,	640-64,112,

	NAME_MINIWIRE	,0,0,0,	640-64,128,
	NAME_BORDER1	,0,0,0,	640-64,96,
 	NAME_END,		-1,-1,-1, -1,-1
};

#endif

/**************************************
*       CHECK FOR FIRE WEAPON         *
**************************************/

void
Fire_Weapons(void)
{

double 	temp;
double	x,y;
double	fdx,fdy;

	x=craftx;
	y=crafty;

	if(fire_delay>0) fire_delay--;

	if (((Pad1 & PAD_FIRE)!=0) && fire_delay==0)	/* RESTORE: was PaDRleft; fire follows the controls setting (owner's decision) */
	{
		switch (weapontype)
		{

			case WEAPONMISSILE:
			if (freeweaponslots>0)
			{
				freeweaponslots--;
				fire_delay=16;
				firen=(angle+16)&63;
			  	temp=(angle)*((pi*2)/64);
				fdx=cos(temp)*4;
				fdy=sin(temp)*4;
				Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
			}
			break;

			case WEAPONLASER:

			if (weaponammo[WEAPONLASER]!=0)
			{
				/* RESTORE: bug fix (2026-11-21): THE ROUND IS TAKEN ONLY WHEN A SHOT GOES (BELOW); NMS TOOK IT FIRST, SO WITH NO FREE SLOT EVERY FRAME FIRE WAS HELD COST A ROUND */
				if (freeweaponslots>0)
				{
					weaponammo[WEAPONLASER]-=1;
					freeweaponslots--;
					fire_delay=16;
					firen=(angle+16)&63;
				  	temp=(angle)*((pi*2)/64);
					fdx=cos(temp)*3;
					fdy=sin(temp)*3;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				}
			}
			break;

			case WEAPONBUCKSHOT:

			if (weaponammo[WEAPONBUCKSHOT]!=0)
			{
				/* RESTORE: bug fix (2026-11-21): THE ROUND IS TAKEN ONLY WHEN A SHOT GOES (BELOW); NMS TOOK IT FIRST, SO WITH NO FREE SLOT EVERY FRAME FIRE WAS HELD COST A ROUND */
				if (freeweaponslots>2)
				{
					weaponammo[WEAPONBUCKSHOT]-=1;
					freeweaponslots-=3;
					fire_delay=16;
					firen=(angle+16)&63;
				  	temp=(angle-4)*((pi*2)/64);
					fdx=cos(temp)*3;
					fdy=sin(temp)*3;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));

					fire_delay=16;
					firen=(angle+16)&63;
				  	temp=(angle)*((pi*2)/64);
					fdx=cos(temp)*3;
					fdy=sin(temp)*3;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));

					fire_delay=16;
					firen=(angle+16)&63;
				  	temp=(angle+4)*((pi*2)/64);
					fdx=cos(temp)*3;
					fdy=sin(temp)*3;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				}
			}
			break;

			case WEAPONPROTON:

			if (weaponammo[WEAPONPROTON]!=0)
			{
				/* RESTORE: bug fix (2026-11-21): THE ROUND IS TAKEN ONLY WHEN A SHOT GOES (BELOW); NMS TOOK IT FIRST, SO WITH NO FREE SLOT EVERY FRAME FIRE WAS HELD COST A ROUND */
				if (freeweaponslots>0)
				{
					weaponammo[WEAPONPROTON]-=1;
					freeweaponslots--;
					fire_delay=16;
					firen=(angle+16)&63;
				  	temp=(angle)*((pi*2)/64);
					fdx=cos(temp)*4;
					fdy=sin(temp)*4;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*49152),(long ) (fdy*49152));
				}
			}
			break;

			case WEAPONCHAIN:

			if (weaponammo[WEAPONCHAIN]!=0)
			{
				/* RESTORE: bug fix (2026-11-21): THE ROUND IS TAKEN ONLY WHEN A SHOT GOES (BELOW); NMS TOOK IT FIRST, SO WITH NO FREE SLOT EVERY FRAME FIRE WAS HELD COST A ROUND */
				if (freeweaponslots>0)
				{
					weaponammo[WEAPONCHAIN]-=1;
					freeweaponslots--;
					fire_delay=16;
					firen=(angle+16)&63;
				  	temp=(angle)*((pi*2)/64);
					fdx=cos(temp)*4;
					fdy=sin(temp)*4;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				}
			}
			break;

			case WEAPONPRONG:

			if (weaponammo[WEAPONPRONG]!=0)
			{
				/* RESTORE: bug fix (2026-11-21): THE ROUND IS TAKEN ONLY WHEN A SHOT GOES (BELOW); NMS TOOK IT FIRST, SO WITH NO FREE SLOT EVERY FRAME FIRE WAS HELD COST A ROUND */
				if (freeweaponslots>0)
				{
					weaponammo[WEAPONPRONG]-=1;
					freeweaponslots--;
					fire_delay=16;
					firen=(angle+16+32)&63;
				  	temp=(angle+32)*((pi*2)/64);
					fdx=cos(temp)*2;
					fdy=sin(temp)*2;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*4096),(long ) (fdy*4096));
				}
			}
			break;

			case WEAPONHOMING:

			if (weaponammo[WEAPONHOMING]!=0)
			{
				/* RESTORE: bug fix (2026-11-21): THE ROUND IS TAKEN ONLY WHEN A SHOT GOES (BELOW); NMS TOOK IT FIRST, SO WITH NO FREE SLOT EVERY FRAME FIRE WAS HELD COST A ROUND */
				if (freeweaponslots>0)
				{
					weaponammo[WEAPONHOMING]-=1;
					freeweaponslots--;
					fire_delay=16;
					firen=(angle+16)&63;
				  	temp=(angle)*((pi*2)/64);
					fdx=cos(temp)*3;
					fdy=sin(temp)*3;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				}
			}
			break;

			case WEAPONCIRCLE:

			if (weaponammo[WEAPONCIRCLE]!=0)
			{
				/* RESTORE: bug fix (2026-11-21): THE ROUND IS TAKEN ONLY WHEN A SHOT GOES (BELOW); NMS TOOK IT FIRST, SO WITH NO FREE SLOT EVERY FRAME FIRE WAS HELD COST A ROUND */
				if (freeweaponslots>2)
				{
					weaponammo[WEAPONCIRCLE]-=1;
					freeweaponslots-=3;

					fire_delay=16;
					firen=(angle+16)&63;

					firen=96;

				  	temp=(angle)*((pi*2)/64);
					fdx=cos(temp)*2.5;
					fdy=sin(temp)*2.5;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));

					firen=(angle+16)&63;

					firen=112;

				  	temp=(angle)*((pi*2)/64);
					fdx=cos(temp)*2;
					fdy=sin(temp)*2;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));

					firen=(angle+16)&63;

					firen=128;

				  	temp=(angle)*((pi*2)/64);
					fdx=cos(temp)*1.5;
					fdy=sin(temp)*1.5;
					Add_Fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				}
			}
			break;

			case WEAPONSHIELD:

			/* RESTORE: completion (owner's decision 2026-11-13: every gameplay feature completed in another port works on the PS1). IN THE PC (MAKEBACK/FIRE.C:845-855) AND THE SATURN
			   (LOGIC/FIRECODE.C:1081-1087) THE SHIELD WEAPON FIRES WITHOUT AMMUNITION: THREE PIECES, WHILE THREE SHOT SLOTS ARE FREE, ORBITING THE CRAFT UNTIL ONE HITS A CREATURE. NEITHER
			   VERSION HAS AMMUNITION AT ALL; IT IS A JULY PS1 ADDITION. MAIN.C'S STOCK LIST STOPS AT WEAPON 7 (THE ARRAY WAS ONE ENTRY SHORT, docs/DECISIONS.md), SO weaponammo[WEAPONSHIELD] WAS NEVER SET AND THE
			   WEAPON NEVER FIRED: IT IS NOW OUTSIDE THE COUNT, UNLIMITED AS IN THE PC AND THE SATURN (THE SLOTS STILL LIMIT IT TO FOUR PRESSES AT ONCE). */
			{
				if (freeweaponslots>2)
				{
					freeweaponslots-=3;
					fire_delay=16;
					Add_Fire(21,(long ) (x),(long ) (y),(long ) (0),(long ) (0));
					Add_Fire(42,(long ) (x),(long ) (y),(long ) (0),(long ) (0));
					Add_Fire(63,(long ) (x),(long ) (y),(long ) (0),(long ) (0));
				}
			}
			break;
		}
	}
}


/* RESTORE: NMS'S AMMUNITION AMOUNTS, MOVED FROM MAIN.C (SEE LEVEL.C Init_Level) */
void
Reset_Weapon_Ammo(void)
{
	weaponammo[0]=99;				/* INITITALISE WEAPON AMMO */
	weaponammo[1]=50;
	weaponammo[2]=25;
	weaponammo[3]=10;
	weaponammo[4]=50;
	weaponammo[5]=20;
	weaponammo[6]=4;
	weaponammo[7]=3;
	weaponammo[8]=99;			/* RESTORE: THE SHIELD WEAPON FIRES WITHOUT AMMUNITION (FIRE.C): THE PANEL'S COUNTER SHOWS THE MISSILE'S 99 AND NEVER FALLS, INSTEAD OF 00 */
}
