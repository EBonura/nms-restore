
/**************************************
*           INCLUDE HEADERS           *
**************************************/

#include "globals.h"

/**************************************
*          DECLARE VARIABLES          *
**************************************/

const double pi=3.14159265;

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

typedef struct
{
	flag		on;
	flag		hit;
	short	type;
	short	dirangle;
	short	livetimer;
	short	target;
	long		id;
} projectilestruct;

projectilestruct projectile[MAX_FIRE];

/**************************************
*       INITIALISE THE FIRING         *
**************************************/

void init_fire(void )
{

	int i,id;

	weapontype=0;
	fire_speed=3;
	fire_delay=0;
	freeweaponslots=16;

	for(i=0;i<MAX_FIRE;i++)
	{
		fire_on[i]=0;
		fire_x[i]=0;
		fire_y[i]=0;
		fire_dx[i]=0;
		fire_dy[i]=0;
		fire_bx[0][i]=0;
		fire_by[0][i]=0;
		fire_bx[1][i]=0;
		fire_by[1][i]=0;

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

				projectile[i].livetimer=350;

				sprites[id].name=NAME_FIRE01;
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

				sprites[id].name=NAME_SPFIRE;
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

				case WEAPONPROTON:

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
				return;

				case WEAPONCHAIN:

				projectile[i].livetimer=350;

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

				case WEAPONHOMING:

				projectile[i].livetimer=100;

				sprites[id].name=NAME_FIRE01;
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

				sprites[id].name=NAME_FIRE01;
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
				sprites[id].endframe=seqdata[(sprites[id].name)].frames-2;
				if(sprites[id].endframe<0) sprites[id].endframe=0;
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

/**************************************
*          MOVE THE MISSILES          *
**************************************/

void move_fire(int buf )
{

int		i,id,j,x=0,y=0,f=0;
int		rand_factor;
long	temp1;
double	temp,tdx,tdy;
short	homex,homey,homesprite;

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

				add_particle(x+tempx,y+tempy,(int) (0*65536),(int) (0*65536));

				if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
					sprites[id].x,sprites[id].y)==TRUE
					|| x<0 || x>=iff_width
					|| y<0 || y>=iff_height )
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
						add_particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}
					add_sprite(NAME_NXA,ACTION_PLAYONCE,x,y);
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

				if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
					sprites[id].x,sprites[id].y)==TRUE
					|| x<0 || x>=iff_width
					|| y<0 || y>=iff_height )
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
						add_particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
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

				if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
					sprites[id].x,sprites[id].y)==TRUE
					|| x<0 || x>=iff_width
					|| y<0 || y>=iff_height
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
						add_particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
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

				if ((rand()&3)==0)
				{
					tdx=(rand()&7)-4;
					tdy=(rand()&7)-4;
					add_particle_acceleration(x+tdx,y+tdy,
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
						add_particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}

					add_sprite(NAME_NXC,ACTION_PLAYONCE,x,y);

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

				if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
					sprites[id].x,sprites[id].y)==TRUE
					|| x<0 || x>=iff_width
					|| y<0 || y>=iff_height )
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

						add_particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
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

				if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
					sprites[id].x,sprites[id].y)==TRUE
					|| x<0 || x>=iff_width
					|| y<0 || y>=iff_height
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
						add_particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));

					}

					add_sprite(NAME_NXC,ACTION_PLAYONCE,x,y);
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
					add_particle(homex+tempx,homey+tempy,(int) (tempx*65536),(int) (tempy*65536));

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
				add_particle(x+tempx,y+tempy,(int) (0*65536),(int) (0*65536));

				if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
					sprites[id].x,sprites[id].y)==TRUE
					|| x<0 || x>=iff_width
					|| y<0 || y>=iff_height )
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
						add_particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}
					add_sprite(NAME_NXA,ACTION_PLAYONCE,x,y);
				}

				if (projectile[i].livetimer==0)
				{
					for(j=0;j<64;j++)
					{
						temp=((projectile[i].dirangle-16+29+j)&63)*((pi*2)/64);
						tdx=cos(temp)*(rand() % 5);
						tdy=sin(temp)*(rand() % 5);
						add_particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}

					add_sprite(NAME_NXC,ACTION_PLAYONCE,x,y);

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

				add_circle((128-projectile[i].livetimer)/6,x,y);

				if(sprite_collision(&seqdata[(sprites[id].name)],sprites[id].frame,
					sprites[id].x,sprites[id].y)==TRUE
					|| x<0 || x>=iff_width
					|| y<0 || y>=iff_height )
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
						add_particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
					}
					add_sprite(NAME_NXA,ACTION_PLAYONCE,x,y);
				}
				break;

				case WEAPONSHIELD:

			  	temp=(projectile[i].livetimer+1)*((pi*2)/64);
				tdx=cos(temp)*32;
				tdy=sin(temp)*32;

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
					add_particle(x,y,(int) (0),(int) (0));
				}

				projectile[i].livetimer++;
				if (projectile[i].livetimer==64) projectile[i].livetimer=0;

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
	char *gfxdata;
	short *gfxdataw;
	int ptr;
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
			frame=sprites[i].frame;
	
			ptr=seqdata[name].map[frame];
			gfxdata=seqdata[name].data;
			gfxdataw=(short *) (gfxdata+ptr);
			x+=(int ) gfxdataw[0];
			y+=(int ) gfxdataw[1];
			width=gfxdataw[2];
			height=gfxdataw[3];

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
							sprites[i].move_ptr=sprites[i].on_hit_ptr;
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

panelstruct ppanels[]={
	NAME_GUIDWIRE	,0,0,0,	640-64,112,
	NAME_LASWIRE	,0,0,0,	640-64,112,
	NAME_BCKWIRE	,0,0,0,	640-64,112,
	NAME_EBWIRE		,0,0,0,	640-64,112,
	NAME_SMWIRE		,0,0,0,	640-64,112,
	NAME_AGMWIRE	,0,0,0,	640-64,112,
	NAME_GUIDWIRE	,0,0,0,	640-64,112,
	NAME_LASWIRE	,0,0,0,	640-64,112,
	NAME_BCKWIRE	,0,0,0,	640-64,112,

	NAME_MINIWIRE	,0,0,0,	640-64,128,
	NAME_BORDER1	,0,0,0,	640-64,96,
 	NAME_END,		-1,-1,-1, -1,-1
};

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

	if (keys[k1]==128) weapontype=0;
	if (keys[k2]==128) weapontype=1;
	if (keys[k3]==128) weapontype=2;
	if (keys[k4]==128) weapontype=3;
	if (keys[k5]==128) weapontype=4;
	if (keys[k6]==128) weapontype=5;
	if (keys[k7]==128) weapontype=6;
	if (keys[k8]==128) weapontype=7;
	if (keys[k9]==128) weapontype=8;

	if ((keys_jp[kCOMMA]==128) && (weapontype!=0)) weapontype--;
	if ((keys_jp[kPERIOD]==128) && (weapontype!=8)) weapontype++;

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

/**************************************
*       CHECK FOR FIRE WEAPON         *
**************************************/

void Fire_Weapons(void)
{
	double 	temp;
	double	x,y;
	double	fdx,fdy;

	x=frac_craft_x;
	y=frac_craft_y;

	if(fire_delay>0) fire_delay--;
	if( (keys[kSPACE] || keys[kENTER]) && fire_delay==0)
	{
		switch (weapontype)
		{

			case WEAPONMISSILE:

			if (freeweaponslots>0)
			{
				freeweaponslots--;
				fire_delay=16;
				firen=(craft_angle+16)&63;
			  	temp=(craft_angle)*((pi*2)/64);
				fdx=cos(temp)*4;
				fdy=sin(temp)*4;
				add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				play_sample(0,samples_start[FX_MISSILE],samples_len[FX_MISSILE]);
			}
			break;

			case WEAPONLASER:

			if (freeweaponslots>0)
			{
				freeweaponslots--;
				fire_delay=16;
				firen=(craft_angle+16)&63;
			  	temp=(craft_angle)*((pi*2)/64);
				fdx=cos(temp)*3;
				fdy=sin(temp)*3;
				add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				play_sample(0,samples_start[FX_MISSILE],samples_len[FX_MISSILE]);
			}
			break;

			case WEAPONBUCKSHOT:

			if (freeweaponslots>2)
			{
				freeweaponslots-=3;
				fire_delay=16;
				firen=(craft_angle+16)&63;
			  	temp=(craft_angle-4)*((pi*2)/64);
				fdx=cos(temp)*3;
				fdy=sin(temp)*3;
				add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));

				fire_delay=16;
				firen=(craft_angle+16)&63;
			  	temp=(craft_angle)*((pi*2)/64);
				fdx=cos(temp)*3;
				fdy=sin(temp)*3;
				add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));

				fire_delay=16;
				firen=(craft_angle+16)&63;
			  	temp=(craft_angle+4)*((pi*2)/64);
				fdx=cos(temp)*3;
				fdy=sin(temp)*3;
				add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));

				play_sample(0,samples_start[FX_MISSILE],samples_len[FX_MISSILE]);
			}
			break;

			case WEAPONPROTON:

			if (freeweaponslots>0)
			{
				freeweaponslots--;
				fire_delay=16;
				firen=(craft_angle+16)&63;
			  	temp=(craft_angle)*((pi*2)/64);
				fdx=cos(temp)*4;
				fdy=sin(temp)*4;
				add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*49152),(long ) (fdy*49152));
				play_sample(0,samples_start[FX_MISSILE],samples_len[FX_MISSILE]);
			}
			break;

			case WEAPONCHAIN:

			if (freeweaponslots>0)
			{
				freeweaponslots--;
				fire_delay=16;
				firen=(craft_angle+16)&63;
			  	temp=(craft_angle)*((pi*2)/64);
				fdx=cos(temp)*4;
				fdy=sin(temp)*4;
				add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				play_sample(0,samples_start[FX_MISSILE],samples_len[FX_MISSILE]);
			}
			break;

			case WEAPONPRONG:

			if (freeweaponslots>0)
			{
				freeweaponslots--;
				fire_delay=16;
				firen=(craft_angle+16+32)&63;
			  	temp=(craft_angle+32)*((pi*2)/64);
				fdx=cos(temp)*2;
				fdy=sin(temp)*2;
				add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*4096),(long ) (fdy*4096));
				play_sample(0,samples_start[FX_MISSILE],samples_len[FX_MISSILE]);
			}
			break;

			case WEAPONHOMING:

			if (freeweaponslots>0)
			{
				freeweaponslots--;
				fire_delay=16;
				firen=(craft_angle+16)&63;
			  	temp=(craft_angle)*((pi*2)/64);
				fdx=cos(temp)*3;
				fdy=sin(temp)*3;
				add_fire(firen,(long ) (x+fdx*4),(long ) (y+fdy*4),(long ) (fdx*65536),(long ) (fdy*65536));
				play_sample(0,samples_start[FX_MISSILE],samples_len[FX_MISSILE]);
			}
			break;

			case WEAPONCIRCLE:

			if (freeweaponslots>2)
			{
				freeweaponslots-=3;

				fire_delay=16;
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

				play_sample(0,samples_start[FX_MISSILE],samples_len[FX_MISSILE]);
			}
			break;

			case WEAPONSHIELD:

			if (freeweaponslots>2)
			{
				freeweaponslots-=3;
				fire_delay=16;
				add_fire(21,(long ) (x),(long ) (y),(long ) (0),(long ) (0));
				add_fire(42,(long ) (x),(long ) (y),(long ) (0),(long ) (0));
				add_fire(63,(long ) (x),(long ) (y),(long ) (0),(long ) (0));
				play_sample(0,samples_start[FX_MISSILE],samples_len[FX_MISSILE]);
			}
			break;
		}
	}
}

