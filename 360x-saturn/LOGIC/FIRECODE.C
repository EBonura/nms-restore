
/**************************************
*           INCLUDE HEADERS           *
**************************************/

#include <sgl.h>
#include "defines.h"
#include "types.h"
#include "globals.h"
#include <libsn.h>
#include <stdio.h>
#include <math.h>

/**************************************
*          DECLARE VARIABLES          *
**************************************/

const double pi=3.14159265;

projectilestruct projectile[MAX_FIRE];

#if 0
LONG		fire_on[MAX_FIRE];
LONG		fire_frame[MAX_FIRE];
LONG		fire_x[MAX_FIRE];
LONG		fire_y[MAX_FIRE];
LONG		fire_dx[MAX_FIRE];
LONG		fire_dy[MAX_FIRE];
LONG		fire_bx[MAX_FIRE];
LONG		fire_by[MAX_FIRE];
#endif

short		freeweaponslots;
short		fire_speed;
short		fire_delay;
long		firen;

Uint8		weapontype;

#define	FIREDELAY	16

/**************************************
*    PANEL WEAPON SPRITE STRUCTURES   *
**************************************/

#if 0
panelstruct ppanels[]=
{
	{NAME_GUIDWIRE	,ACTION_CYCLE, 0,0,0,	640-64,112},
	{NAME_LASWIRE	,ACTION_CYCLE, 0,0,0,	640-64,112},
	{NAME_BCKWIRE	,ACTION_CYCLE, 0,0,0,	640-64,112},
	{NAME_EBWIRE	,ACTION_CYCLE, 0,0,0,	640-64,112},
	{NAME_SMWIRE	,ACTION_CYCLE, 0,0,0,	640-64,112},
	{NAME_AGMWIRE	,ACTION_CYCLE, 0,0,0,	640-64,112},
	{NAME_GUIDWIRE	,ACTION_CYCLE, 0,0,0,	640-64,112},
	{NAME_LASWIRE	,ACTION_CYCLE, 0,0,0,	640-64,112},
	{NAME_BCKWIRE	,ACTION_CYCLE, 0,0,0,	640-64,112},
	{NAME_MINIWIRE	,ACTION_CYCLE, 0,0,0,	640-64,128},
	{NAME_BORDER1	,ACTION_CYCLE, 0,0,0,	640-64,96},
 	{NAME_END,-1, -1,-1,-1, -1,-1},
};
#endif

/**************************************
*       Externals					        *
**************************************/

extern	Uint16		ScrollX;
extern	Uint16		ScrollY;
extern	char			stat_str[];
extern	long			craft;
extern	long			pod_no;
extern	Uint8			Level;
extern	flag			change_level;
extern	flag			escape;
extern	long			frac_craft_x,frac_craft_y;
extern	ANGLE			angles[];
extern	int			craft_angle;

extern	int			rand(void);
extern	void			msg(Uint8 *message);
extern	long 	 		add_sprite(int name,int action,int x,int y);
extern	void 			Add_Particle(long x,long y,long dx,long dy);
extern	void 			Add_Particle_Acceleration(long x,long y,long dx,long dy,long ddx,long ddy);
extern	flag 			sprite_collision(sprite_sequence *seq,int sprite_no,int x,int y);
extern	long 			find_nearest_sprite(long x,long y);
extern	void 			add_a_glow_line_clipped(long x,long y,long width);
extern	void 			add_a_glow_line(long x,long y,long width);
extern	void 			draw_glow_list(void );
extern	void 			remove_glow_list(void );
extern	void 			add_polo(long r1,long r2,long start_x,long start_y);
extern	void 			draw_polo_clipped(long r1,long r2,long start_x,long start_y);
extern	void 			draw_polo(long r1,long r2,long start_x,long start_y);
extern	void 			add_circle(long radius,long start_x,long start_y);
extern	void 			draw_circle_clipped(long r,long start_x,long start_y);
extern	void 			draw_circle(long r,long start_x,long start_y);
extern	flag 			SHcollision_at(long x,long y);

/**************************************
*       INITIALISE THE FIRING         *
**************************************/

void Init_Fire(void)
{

	int i,id;

	weapontype=0;
	fire_speed=3;
	fire_delay=0;
	freeweaponslots=8;

	for(i=0;i<MAX_FIRE;i++)
	{
		#if 0
		fire_on[i]=0;
		fire_x[i]=0;
		fire_y[i]=0;
		fire_dx[i]=0;
		fire_dy[i]=0;
		fire_bx[i]=0;
		fire_by[i]=0;
		fire_bx[i]=0;
		fire_by[i]=0;
		#endif

		id=add_sprite(NAME_FIRE01,ACTION_NONE,-99,-99);

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

void add_fire(int frame,int x,int y,int dx,int dy)
{
	long	i,id;

	for(i=0;i<MAX_FIRE;i++)
	{
		if ((projectile[i].on==NO) && (projectile[i].id!=-1))
		{
			projectile[i].on=YES;
			projectile[i].hit=NO;
			projectile[i].type=weapontype;
			projectile[i].dirangle=frame;
			projectile[i].target=-1;

			id=projectile[i].id;

			switch (projectile[i].type)
			{
				case WEAPONMISSILE:
				{
					play_sample(0,36);
					projectile[i].livetimer=350;
					sprites[id].name=NAME_GUIDED;
					sprites[id].frame=frame;
					sprites[id].visible=YES;
					sprites[id].x=x;
					sprites[id].y=y;
					sprites[id].obj[obj_temp1]=x*65536;
					sprites[id].obj[obj_temp2]=y*65536;
					sprites[id].obj[obj_temp3]=dx;
					sprites[id].obj[obj_temp4]=dy;
					break;
				}

				case WEAPONLASER:
				{
					play_sample(0,36);
					projectile[i].livetimer=350;
					sprites[id].name=NAME_GUIDED;
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
					break;
				}

				case WEAPONBUCKSHOT:
				{
					play_sample(0,36);
					projectile[i].livetimer=350;
					sprites[id].name=NAME_BUCK01;
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
					break;
				}

				case WEAPONPROTON:
				{
					play_sample(0,36);
					projectile[i].livetimer=350;
					sprites[id].name=NAME_EBALL;
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
					break;
				}

				case WEAPONCHAIN:
				{
					play_sample(0,36);
					projectile[i].livetimer=350;
					sprites[id].name=NAME_CHAIN3;
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
					break;
				}

				case WEAPONPRONG:
				{
					play_sample(0,36);
					projectile[i].livetimer=240;
					sprites[id].name=NAME_mine2;
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
					break;
				}

				case WEAPONHOMING:
				{
					play_sample(0,36);
					projectile[i].livetimer=100;
					sprites[id].name=NAME_GUIDED;
					sprites[id].visible=YES;
					sprites[id].frame=frame;
					sprites[id].x=x;
					sprites[id].y=y;
					sprites[id].obj[obj_temp1]=x*65536;
					sprites[id].obj[obj_temp2]=y*65536;
					sprites[id].obj[obj_temp3]=dx;
					sprites[id].obj[obj_temp4]=dy;
					break;
				}

				case WEAPONCIRCLE:
				{
					play_sample(0,36);
					projectile[i].livetimer=frame;
					sprites[id].name=NAME_mine2;
					sprites[id].visible=NO;
					sprites[id].frame=0;
					sprites[id].x=x;
					sprites[id].y=y;
					sprites[id].obj[obj_temp1]=x*65536;
					sprites[id].obj[obj_temp2]=y*65536;
					sprites[id].obj[obj_temp3]=dx;
					sprites[id].obj[obj_temp4]=dy;
					break;
				}

				case WEAPONSHIELD:
				{
					play_sample(0,36);
					projectile[i].livetimer=frame;
					sprites[id].name=NAME_mine2;
					sprites[id].frame=0;
					sprites[id].startframe=0;
					sprites[id].endframe=seqdata[(sprites[id].name)].frames-2;
					if(sprites[id].endframe<0) sprites[id].endframe=0;
					sprites[id].visible=YES;
					sprites[id].x=x;
					sprites[id].y=y;
					sprites[id].obj[obj_temp1]=x*65536;
					sprites[id].obj[obj_temp2]=y*65536;
					break;
				}
			}
		}
	}
}

/**************************************
*          MOVE THE MISSILES          *
**************************************/

void Move_Fire(int buf )
{

	int			i,id,j,x=0,y=0;   //,f=0;
	int			rand_factor;
	long			temp1;
	FIXED			temp,tdx,tdy;
	short			homex,homey,homesprite;
	int			tempx,tempy;

	for(i=0;i<MAX_FIRE;i++)
	{
		if(projectile[i].on==YES)
		{
			id=projectile[i].id;
			switch (projectile[i].type)
			{
				case WEAPONMISSILE:
				{
					x=sprites[id].obj[obj_temp1];
					y=sprites[id].obj[obj_temp2];
					x+=sprites[id].obj[obj_temp3];
					y+=sprites[id].obj[obj_temp4];
					sprites[id].obj[obj_temp1]=x;
					sprites[id].obj[obj_temp2]=y;
					
					x=x>>16;
					y=y>>16;
					
					sprites[id].x=x;
					sprites[id].y=y;
					
					tempx=rand_factor=(rand()&3)-2;
					tempy=rand_factor=(rand()&3)-2;
					
					//Add_Particle(x+tempx,y+tempy,(int) (0*65536),(int) (0*65536));
					
					//collision with background and map clipping
					if((SHcollision_at(sprites[id].x,sprites[id].y)==TRUE) || x<0 || x>=iff_width || y<0 || y>=iff_height )
					{
						play_sample(15,36);
						projectile[i].on=NO;
						sprites[id].x=-99;
						sprites[id].y=-99;
						sprites[id].visible=NO;
						freeweaponslots++;
					
						for(j=0;j<16;j++)
						{
							rand_factor=(rand()*16)>>15;
							temp=angles[((projectile[i].dirangle-16+29+rand_factor)&63)];
							tdx=slMulFX(slCos(temp),toFIXED(1.5));
							tdy=slMulFX(slSin(temp),toFIXED(1.5));
							Add_Particle(x+(tdx>>16),y+(tdy>>16),(int) (tdx),(int) (tdy));
						}

						//Start explosion sprite
						add_sprite(NAME_exp4,ACTION_PLAYONCE,x-5,y-5);
						add_sprite(NAME_exp7,ACTION_PLAYONCE,x,y);
						add_sprite(NAME_exp6,ACTION_PLAYONCE,x+5,y-5);
						
						
						//add_sprite(NAME_exp5,ACTION_PLAYONCE,x+5,y);
					}

					break;
				}


				case WEAPONLASER:
				{
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
					
					x=x>>16;
					y=y>>16;

					sprites[id].x=x;
					sprites[id].y=y;
					
					if((SHcollision_at(sprites[id].x,sprites[id].y)==TRUE) || x<0 || x>=iff_width || y<0 || y>=iff_height ||sprites[id].x==-99)
					{
						projectile[i].on=NO;
						sprites[id].x=-99;
						sprites[id].y=-99;
						sprites[id].visible=NO;
						freeweaponslots++;
					
						#if 0
						for(j=0;j<8;j++)
						{
							rand_factor=(rand()*16)>>15;
							temp=angles[((projectile[i].dirangle-16+29+rand_factor)&63)];
							tdx=slMulFX(slCos(temp),toFIXED(1.5));
							tdy=slMulFX(slSin(temp),toFIXED(1.5));
							Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
						}
						#endif
					}

					break;

				}

				case WEAPONBUCKSHOT:
				{
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
					
					x=x>>16;
					y=y>>16;

					sprites[id].x=x;
					sprites[id].y=y;
					
					if((SHcollision_at(sprites[id].x,sprites[id].y)==TRUE) || x<0 || x>=iff_width || y<0 || y>=iff_height ||sprites[id].x==-99)
					{
						projectile[i].on=NO;
						sprites[id].x=-99;
						sprites[id].y=-99;
						sprites[id].visible=NO;
						freeweaponslots++;
					
						#if 0
						for(j=0;j<8;j++)
						{
							rand_factor=(rand()*16)>>15;
							temp=angles[((projectile[i].dirangle-16+29+rand_factor)&63)];
							tdx=slMulFX(slCos(temp),toFIXED(1.5));
							tdy=slMulFX(slSin(temp),toFIXED(1.5));
							Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
						}
						#endif

					}
					break;
				}

				#if 0
				case WEAPONPROTON:
				{
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
					
					if ((rand()&3)==0)
					{
						tdx=(rand()&7)-4;
						tdy=(rand()&7)-4;
						Add_Particle_Acceleration(x+tdx,y+tdy,
							(int) (tdx*(65536/512)),(int) (tdy*(65536/512)),
							(int) (0-tdx*(65536/256)),(int) (0-tdy*(65536/256)));
					}
					
					if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
						sprites[id].x-4,sprites[id].y)==TRUE
						|| x<0 || x>=iff_width
						|| y<0 || y>=iff_height )
					{
						sprites[id].obj[obj_temp3]=-sprites[id].obj[obj_temp3];
					}
					
					if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
						sprites[id].x+4,sprites[id].y)==TRUE
						|| x<0 || x>=iff_width
						|| y<0 || y>=iff_height )
					{
						sprites[id].obj[obj_temp3]=-sprites[id].obj[obj_temp3];
					}
					
					if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
						sprites[id].x,sprites[id].y-4)==TRUE
						|| x<0 || x>=iff_width
						|| y<0 || y>=iff_height )
					{
						sprites[id].obj[obj_temp4]=-sprites[id].obj[obj_temp4];
					}
					
					if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
						sprites[id].x,sprites[id].y+4)==TRUE
						|| x<0 || x>=iff_width
						|| y<0 || y>=iff_height )
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
					
						add_sprite(NAME_NXC,ACTION_PLAYONCE,x,y);
					
						projectile[i].on=NO;
						sprites[id].x=-99;
						sprites[id].y=-99;
						sprites[id].visible=NO;
						freeweaponslots++;
					
					}
					break;
				}
				
				#endif

				case WEAPONCHAIN:
				{
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
					
					x=x>>16;
					y=y>>16;

					sprites[id].x=x;
					sprites[id].y=y;
					
					if((SHcollision_at(sprites[id].x,sprites[id].y)==TRUE) || x<0 || x>=iff_width || y<0 || y>=iff_height ||sprites[id].x==-99)
					{
						projectile[i].on=NO;
						sprites[id].x=-99;
						sprites[id].y=-99;
						sprites[id].visible=NO;
						freeweaponslots++;
					
						#if 0
						for(j=0;j<8;j++)
						{
							rand_factor=(rand()*16)>>15;
							temp=angles[((projectile[i].dirangle-16+29+rand_factor)&63)];
							tdx=slMulFX(slCos(temp),toFIXED(1.5));
							tdy=slMulFX(slSin(temp),toFIXED(1.5));
							Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
						}
						#endif

					}
					break;
				}

				case WEAPONPRONG:
				{
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
					
					x=x>>16;
					y=y>>16;
					sprites[id].x=x;
					sprites[id].y=y;
					
					if((SHcollision_at(sprites[id].x,sprites[id].y)==TRUE) || x<0 || x>=iff_width || y<0 || y>=iff_height ||sprites[id].x==-99)
					{
						projectile[i].on=NO;
						sprites[id].x=-99;
						sprites[id].y=-99;
						sprites[id].visible=NO;
						freeweaponslots++;
					
						#if 0
						for(j=0;j<64;j++)
						{
							rand_factor=(rand()*16)>>15;
							temp=angles[((projectile[i].dirangle-16+29+rand_factor)&63)];
							tdx=slMulFX(slCos(temp),toFIXED(1.5));
							tdy=slMulFX(slSin(temp),toFIXED(1.5));
							Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
						}
						#endif

						//add_sprite(NAME_NXC,ACTION_PLAYONCE,x,y);
					}
					else projectile[i].livetimer--;
					break;
				}

				case WEAPONHOMING:
				{
					x=sprites[id].obj[obj_temp1];
					y=sprites[id].obj[obj_temp2];
					x+=sprites[id].obj[obj_temp3];
					y+=sprites[id].obj[obj_temp4];
					sprites[id].obj[obj_temp1]=x;
					sprites[id].obj[obj_temp2]=y;
					
					x=x>>16;
					y=y>>16;
					sprites[id].x=x;
					sprites[id].y=y;
					
					homesprite=projectile[i].target;
					
					if (projectile[i].livetimer==75)
					{
						if (homesprite==-1)
						{
							homesprite=find_nearest_sprite(x,y);
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
					
					if((SHcollision_at(sprites[id].x,sprites[id].y)==TRUE) || x<0 || x>=iff_width || y<0 || y>=iff_height ||sprites[id].x==-99)
					{
						projectile[i].on=NO;
						projectile[i].target=-1;
					
						sprites[id].x=-99;
						sprites[id].y=-99;
						sprites[id].visible=NO;
						freeweaponslots++;
					
						#if 0
						for(j=0;j<16;j++)
						{
							rand_factor=(rand()*16)>>15;
							temp=angles[((projectile[i].dirangle-16+29+rand_factor)&63)];
							tdx=slMulFX(slCos(temp),toFIXED(1.5));
							tdy=slMulFX(slSin(temp),toFIXED(1.5));
							Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
						}
						#endif
						//add_sprite(NAME_NXA,ACTION_PLAYONCE,x,y);
					}
					
					if (projectile[i].livetimer==0)
					{
						#if 0
						for(j=0;j<64;j++)
						{
							temp=angles[((projectile[i].dirangle-16+29+j)&63)];
							tdx=slMulFX(slCos(temp), (rand() % (5*65536)) );
							tdy=slMulFX(slSin(temp), (rand() % (5*65536)) );
							Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
						}
					
						add_sprite(NAME_NXC,ACTION_PLAYONCE,x,y);
						#endif

						projectile[i].on=NO;
						sprites[id].x=-99;
						sprites[id].y=-99;
						sprites[id].visible=NO;
						freeweaponslots++;
					}
					break;
				}
	
				case WEAPONCIRCLE:
				{
					if (projectile[i].livetimer!=0) projectile[i].livetimer--;
					
					x=sprites[id].obj[obj_temp1];
					y=sprites[id].obj[obj_temp2];
					x+=sprites[id].obj[obj_temp3];
					y+=sprites[id].obj[obj_temp4];
					sprites[id].obj[obj_temp1]=x;
					sprites[id].obj[obj_temp2]=y;
					
					x=x>>16;
					y=y>>16;

					sprites[id].x=x;
					sprites[id].y=y;
					
					add_circle((128-projectile[i].livetimer)/6,x,y);
					
					if((SHcollision_at(sprites[id].x,sprites[id].y)==TRUE) || x<0 || x>=iff_width || y<0 || y>=iff_height ||sprites[id].x==-99)
					{
						projectile[i].on=NO;
						sprites[id].x=-99;
						sprites[id].y=-99;
						sprites[id].visible=NO;
						freeweaponslots++;
					
						#if 0
						for(j=0;j<16;j++)
						{
							rand_factor=(rand()*16)>>15;
							temp=angles[((projectile[i].dirangle-16+29+rand_factor)&63)];
							tdx=slMulFX(slCos(temp),toFIXED(1.5));
							tdy=slMulFX(slSin(temp),toFIXED(1.5));
							Add_Particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
						}
						//add_sprite(NAME_NXA,ACTION_PLAYONCE,x,y);
						#endif
					}
					break;
				}

				case WEAPONSHIELD:
				{
			  		temp=angles[(projectile[i].livetimer+1)&63];
					tdx=slMulFX(slCos(temp),32*65536);
					tdy=slMulFX(slSin(temp),32*65536);
					
					x=craftx*65536;
					y=crafty*65536;
					
					x+=(tdx*65536);
					y+=(tdy*65536);
					
					sprites[id].obj[obj_temp1]=x;
					sprites[id].obj[obj_temp2]=y;
					
					x=x>>16;
					y=y>>16;
					
					sprites[id].x=x;
					sprites[id].y=y;
					
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

				default:
				{
					break;
				}

			}
		}
	}
}

/**************************************
*       CHECK FOR FIRE WEAPON         *
**************************************/

void Fire_Weapons(Uint16 data)
{

	FIXED 	temp;
	FIXED		x,y;
	FIXED		fdx,fdy;
	flag		fire_pressed;

	x=frac_craft_x;
	y=frac_craft_y;

	if(fire_delay>0) fire_delay--;

	fire_pressed=FALSE;

	if( (data & PER_DGT_TC)==0 && fire_delay==0) fire_pressed=TRUE;

	if(fire_pressed==TRUE)
	{
		switch (weapontype)
		{
			case WEAPONMISSILE:
			{
				if (freeweaponslots>0)
				{
					freeweaponslots--;
					firen=craft_angle;	//the missile frame
			  		temp=angles[(craft_angle-16)&63];
					fdx=slMulFX( slCos(temp) , 3*65536);
					fdy=slMulFX( slSin(temp) , 3*65536);
					add_fire(firen,(long ) x+(fdx>>16),(long ) y+(fdy>>16),(long ) (fdx),(long ) (fdy));
					fire_delay=FIREDELAY;
				}

				break;
			}


			case WEAPONLASER:
			{
				if (freeweaponslots>0)
				{
					freeweaponslots--;
					firen=craft_angle;
			  		temp=angles[(craft_angle-16)&63];
					fdx=slMulFX( slCos(temp) , 3*65536);
					fdy=slMulFX( slSin(temp) , 3*65536);
					add_fire(firen,(long ) x+(fdx>>16),(long ) y+(fdy>>16),(long ) (fdx),(long ) (fdy));
					fire_delay=FIREDELAY;
				}
				break;
			}

			case WEAPONBUCKSHOT:
			{
				if (freeweaponslots>2)
				{
					freeweaponslots-=3;
					firen=craft_angle;

					//right
			  		temp=angles[(craft_angle-12)&63];
					fdx=slMulFX( slCos(temp) , 3*65536);
					fdy=slMulFX( slSin(temp) , 3*65536);
					add_fire(firen,(long ) x+(fdx>>16),(long ) y+(fdy>>16),(long ) (fdx),(long ) (fdy));
				
					//centre
			  		temp=angles[(craft_angle-16)&63];
					fdx=slMulFX( slCos(temp) , 3*65536);
					fdy=slMulFX( slSin(temp) , 3*65536);
					add_fire(firen,(long ) x+(fdx>>16),(long ) y+(fdy>>16),(long ) (fdx),(long ) (fdy));
				
					//left
			  		temp=angles[(craft_angle-20)&63];
					fdx=slMulFX( slCos(temp) , 3*65536);
					fdy=slMulFX( slSin(temp) , 3*65536);
					add_fire(firen,(long ) x+(fdx>>16),(long ) y+(fdy>>16),(long ) (fdx),(long ) (fdy));
					
					fire_delay=FIREDELAY;
				
				}
				break;
			}

			case WEAPONPROTON:
			{
				if (freeweaponslots>0)
				{
					freeweaponslots--;
					firen=craft_angle;
			  		temp=angles[(craft_angle-16)&63];
					fdx=slMulFX( slCos(temp) , 4*65536);
					fdy=slMulFX( slSin(temp) , 4*65536);
					add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*49152),(long ) (fdy*49152));
					fire_delay=FIREDELAY;
				}
				break;
			}
	
			case WEAPONCHAIN:
			{
				if (freeweaponslots>0)
				{
					freeweaponslots--;
					firen=craft_angle;
			  		temp=angles[(craft_angle-16)&63];
					fdx=slMulFX( slCos(temp) , 4*65536);
					fdy=slMulFX( slSin(temp) , 4*65536);
					add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
					fire_delay=FIREDELAY;
				}
				break;
			}

			case WEAPONPRONG:
			{
				if (freeweaponslots>0)
				{
					freeweaponslots--;
					firen=craft_angle;
			  		temp=angles[(craft_angle-16)&63];
					fdx=slMulFX( slCos(temp) , 4*65536);
					fdy=slMulFX( slSin(temp) , 4*65536);
					add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*4096),(long ) (fdy*4096));
					fire_delay=FIREDELAY;
				}
				break;
			}

			case WEAPONHOMING:
			{
				if (freeweaponslots>0)
				{
					freeweaponslots--;
					firen=craft_angle;
			  		temp=angles[(craft_angle-16)&63];
					fdx=slMulFX( slCos(temp) , 3*65536);
					fdy=slMulFX( slSin(temp) , 3*65536);
					add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
					fire_delay=FIREDELAY;
				}
				break;
			}

			#if 0
			case WEAPONCIRCLE:
			{
				if (freeweaponslots>2)
				{
					freeweaponslots-=3;
				
					firen=(craft_angle+16)&63;
				
					firen=96;
				
				  	temp=(craft_angle)*((pi*2)/64);
					fdx=cos(temp)*2.5;
					fdy=sin(temp)*2.5;
					add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				
					firen=(craft_angle+16)&63;
				
					firen=112;
				
				  	temp=(craft_angle)*((pi*2)/64);
					fdx=cos(temp)*2;
					fdy=sin(temp)*2;
					add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				
					firen=(craft_angle+16)&63;
				
					firen=128;
				
				  	temp=(craft_angle)*((pi*2)/64);
					fdx=cos(temp)*1.5;
					fdy=sin(temp)*1.5;
					add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
					fire_delay=FIREDELAY;
				
				}
				break;
			}
			#endif

			case WEAPONSHIELD:
			{
				if (freeweaponslots>2)
				{
					freeweaponslots-=3;
					add_fire(21,(long ) (x),(long ) (y),(long ) (0),(long ) (0));
					add_fire(42,(long ) (x),(long ) (y),(long ) (0),(long ) (0));
					add_fire(63,(long ) (x),(long ) (y),(long ) (0),(long ) (0));
					fire_delay=FIREDELAY;
				}
				break;
			}

			default:
			{
				break;
			}


		}
	}
}

/**************************************
*      PROCESS WEAPON COLLISIONS      *
**************************************/

void process_collisions(void )
{
	#if 0
	unsigned char	*gfxdata;
	short	*gfxdataw;
	long	ptr,p;
	short width,height;
	int  	i,j,x,y,id,iy;
	long 	dx,lx,tx,ax;
	int  	name,frame;
	
	for(i=0;i<MAX_SPRITES;i++)
 	{
		if( (sprites[i].on==TRUE) && (sprites[i].shootable==YES) )
		{		
			name=sprites[i].name;
			x=sprites[i].x;
			y=sprites[i].y;
			frame=sprites[i].frame;
	
			ptr=seqdata[name].map[frame];
			gfxdata=seqdata[name].data;
			gfxdataw=(short *) (gfxdata+ptr);
			x+=(int ) gfxdataw[0];
			y+=(int ) gfxdataw[1];
			width=gfxdataw[2];
			height=gfxdataw[3];
			ptr+=8;

			for(j=0;j<MAX_FIRE;j++)
			{
				if(projectile[j].on==YES)
				{
					id=projectile[j].id;
					if(sprites[id].x>=x && sprites[id].x<(x+width))
					{
						if(sprites[id].y>=y && sprites[id].y<(y+height)) 
						{
							p=ptr;
							iy=0;
							while(iy<height)
							{
								dx=(unsigned long ) gfxdata[p+0];
								lx=(unsigned long ) gfxdata[p+1];	
								tx=(unsigned long ) gfxdata[p+2];
								ax=(unsigned long ) gfxdata[p+3];
								if( sprites[id].y==(y+iy) )
								{
									if( sprites[id].x>=(x+dx) && sprites[id].x<(x+dx+lx))
									{
										if(tx==0)
										{
											if(gfxdata[p+4+(sprites[id].x-(x+dx))]!=0)
											{
												iy=-1;
												break;
											}
										}
										else 
										{
											iy=-1;
											break;
										}
									}
								}
								p+=4+ax;
								iy++;
							}
							if(iy==-1)
							{
								sprites[i].obj[obj_colx]=sprites[id].x;
								sprites[i].obj[obj_coly]=sprites[id].y;
			 				 	projectile[j].on=NO;
								sprites[id].visible=NO;
								sprites[id].x=-99;
								sprites[id].y=-99;
								freeweaponslots++;
								sprites[i].move_ptr=sprites[i].on_hit_ptr;
								the_score+=sprites_attribs[(sprites[i].name)].score;
								play_a_sound_effect(sprites_attribs[(sprites[i].name)].sfx);
								break;
							}
						}
					}
				}
			}
		}
	}

	#endif

}



#if 0
/**************************************
*   INITIALISE WEAPONS ON THE PANEL   *
**************************************/

void initialise_pauls_panels(void )
{	 
	long i;
	
	i=0;
	while(TRUE)
	{
		if(ppanels[i].name==NAME_END) break;
		ppanels[i].id=add_sprite(ppanels[i].name,ACTION_CYCLE,ppanels[i].x,ppanels[i].y);
		if(ppanels[i].id!=-1)
		{
			sprites[(ppanels[i].id)].priority=0;
			sprites[(ppanels[i].id)].visible=FALSE;
			sprites[(ppanels[i].id)].on=TRUE;
		}
		i++;
	}
}

/**************************************
*   UPDATE THE WEAPONS ON THE PANEL   *
**************************************/

void update_pauls_panels(void)
{

	long	i,id;


	i=0;
	while(TRUE)
	{
		if(ppanels[i].name==NAME_END) break;
		id=ppanels[i].id;
		if(id!=-1)
		{
			if ((weapontype==i) || (i>8))
			{
				sprites[id].visible=TRUE;
			}
			else
			{
				sprites[id].visible=FALSE;
			}

			sprites[id].x=ppanels[i].x+scroll_x;
			sprites[id].y=ppanels[i].y+scroll_y;
		}
		i++;
	}
}

#endif
