
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	MAX_PARTICLES	(512)

#define	MAX_STARS		256
#define	MAX_ACTIVE	256

/**************************************
*        DECLARE STRUCTURES           *
**************************************/

struct	starstruct
{
	SHORT	x,y,z;
};

typedef struct starstruct	STARTYPE;

/**************************************
*          DECLARE VARIABLES          *
**************************************/

ULONG	particle_list_mask[MAX_PARTICLES/32];
LONG	particle_list_x[MAX_PARTICLES];
LONG	particle_list_y[MAX_PARTICLES];
LONG	particle_list_dx[MAX_PARTICLES];
LONG	particle_list_dy[MAX_PARTICLES];
LONG	particle_list_ddx[MAX_PARTICLES];
LONG	particle_list_ddy[MAX_PARTICLES];
LONG	particle_list_counter[MAX_PARTICLES];
LONG	particle_list_delay[MAX_PARTICLES];
LONG	particle_list_halflife[MAX_PARTICLES];

USHORT	activestars;

STARTYPE	starlist[MAX_STARS];

/**************************************
*     INITIALISE PARTICLE LIST        *
**************************************/

void 
Init_Particle_List(void)
{

LONG	i;

	for(i=0;i<(MAX_PARTICLES/32);i++) particle_list_mask[i]=0;

	for(i=0;i<MAX_PARTICLES;i++)
	{
		particle_list_x[i]=-1;
		particle_list_y[i]=-1;
		particle_list_dx[i]=-1;
		particle_list_dy[i]=-1;
		particle_list_counter[i]=0;
		particle_list_delay[i]=0;
		particle_list_halflife[i]=0;
	}
}

/**************************************
*        UPDATE PARTICLE LIST         *
**************************************/

void 
Move_Particles(void)
{

LONG	i,j,k,x,y;

	for (j=0;j<(MAX_PARTICLES/32);j++)
	{
		if(particle_list_mask[j]!=0)
		{
			i=j*32;
			for(k=0;k<32;k++)
			{
				if(particle_list_counter[i]>0)
				{
					if (particle_list_delay[i]>0) particle_list_delay[i]--;
					else
					{	
						particle_list_counter[i]--;
						particle_list_delay[i]=particle_list_halflife[i];
					}

					x=particle_list_x[i];
					y=particle_list_y[i];
					x+=particle_list_dx[i];
					y+=particle_list_dy[i];
					particle_list_dx[i]+=particle_list_ddx[i];
					particle_list_dy[i]+=particle_list_ddy[i];

				   	particle_list_x[i]=x;
					particle_list_y[i]=y;

					x=x/65536;
					y=y/65536;

					if (Collision_At(x,y)==TRUE)	/* RESTORE: bug fix, WAS levelcol[(x+mapwidth*y)>>3] WITH NO BOUNDS: ROWS ARE colwidth APART (SCROLLER.C), AND A PARTICLE OFF THE MAP READ PAST THE FILE */
					{
					 	particle_list_counter[i]=0;
						particle_list_x[i]=-1;
						particle_list_y[i]=-1;
						particle_list_mask[j]&=(0xffffffff-(1<<(i&31)));
					}
				}
				else particle_list_mask[j]&=(0xffffffff-(1<<(i&31)));
				i++;
			}
		}
	}
}

/**************************************
*   ADD A PARTICLE FOR BACK OF SHIP   *
**************************************/

void 
Add_Particle(long x,long y,long dx,long dy)
{

LONG	i,j,k;

	for (j=0;j<(MAX_PARTICLES/32);j++)
	{
		if (particle_list_mask[j]!=0xffffffff)
		{
			i=j*32;
			for (k=0;k<32;k++)
			{
				if (particle_list_counter[i]==0)
				{
					particle_list_mask[j]|=(1<<(i&31));
					particle_list_counter[i]=64;
					particle_list_x[i]=x<<16;
					particle_list_y[i]=y<<16;
					particle_list_dx[i]=dx;
					particle_list_dy[i]=dy;
					particle_list_ddx[i]=0;
					particle_list_ddy[i]=2048;
					particle_list_delay[i]=0;
					particle_list_halflife[i]=0;
					return;
				}
				i++;
			}
		}
	}
}

/**************************************
*   ADD A PARTICLE WITH NO GRAVITY    *
**************************************/

void 
Add_Particle_Acceleration(long x,long y,long dx,long dy,long ddx,long ddy)
{

LONG	i,j,k;

	for (j=0;j<(MAX_PARTICLES/32);j++)
	{
		if (particle_list_mask[j]!=0xffffffff)
		{
			i=j*32;
			for (k=0;k<32;k++)
			{
				if(particle_list_counter[i]==0)
				{
					particle_list_mask[j]|=(1<<(i&31));
					particle_list_counter[i]=64;
					particle_list_x[i]=x<<16;
					particle_list_y[i]=y<<16;
					particle_list_dx[i]=dx;
					particle_list_dy[i]=dy;
					particle_list_ddx[i]=ddx;	// 64
					particle_list_ddy[i]=ddy;	// 0
					particle_list_delay[i]=0;
					particle_list_halflife[i]=8;
					return;
				}
				i++;
			}
		}
	}
}

/**************************************
*    PLOT THE PARTICLES TO SCREEN     *
**************************************/

void 
Draw_Particles(void)
{

GsSPRITE spr = {0, 0,0,1,1,0,0,0,0,240+256,128,128,128,0,0,4096,4096,0};

LONG	i,j,k;

	for (j=0;j<(MAX_PARTICLES/32);j++)
	{
		if (particle_list_mask[j]!=0)
		{
			i=j*32;
			for (k=0;k<32;k++)
			{
				if (particle_list_counter[i]!=0)
				{
					if (particle_list_counter[i]<128)
					{
						maxpolycount--;
						spr.x=(particle_list_x[i]>>16)-screenx-160;
						spr.y=(particle_list_y[i]>>16)-screeny-120;

						spr.r=particle_list_counter[i]+32;
						spr.g=particle_list_counter[i]+32;
						spr.b=particle_list_counter[i]+32;

						spr.tpage=7;
						spr.u=209;
						spr.v=1;
						GsSortFastSprite(&spr,&Wot[screentoggle],255-255);
					}
				}
				i++;
			}
		}
	}
}

/**************************************
*        INITIALISE STAR LIST         *
**************************************/

void
Init_Star_List(void)
{

USHORT	j,i;
SHORT	x,y;

	for(i=0;i<MAX_STARS;i++)
	{
		starlist[i].x=-1;
		starlist[i].y=-1;
		starlist[i].z=-1;
	}
	activestars=0;

	for (i=0;i<MAX_STARS;i++)
	{
		if(starlist[i].x==-1)
		{
			x=(rand() MOD 310)-155;
			y=(rand() MOD 120)-60;

			starlist[i].x=x;
			starlist[i].y=y;
			starlist[i].z=(rand() MOD 2048)+2048;
			activestars++;
		}
	}
}

/**************************************
*       ADD A STAR TO THE LIST        *
**************************************/

void
Add_Star(void)
{

USHORT	i;
SHORT	x,y;

	for (i=0;i<MAX_STARS;i++)
	{
		if(starlist[i].x==-1)
		{
			x=(rand() MOD 320)-160;
			y=(rand() MOD 120)-60;

			if (x==0) x=1;
			if (y==0) y=1;

			starlist[i].x=x;
			starlist[i].y=y;
			starlist[i].z=4096;
			activestars++;
		}
	}
}

/**************************************
*           PLOT THE STARS            *
**************************************/

void
Plot_Stars(SHORT starspeed)
{

USHORT	index;
SHORT	x,y,z;
SHORT	sx,sy;
SHORT	zx,zy;

	for (index=0;index<MAX_STARS;index++)
	{
		if(starlist[index].x!=-1)
		{
			x=starlist[index].x;
			y=starlist[index].y;
			z=starlist[index].z;

			sx=(4096*x)/z;
			sy=(4096*y)/z;

			zx=(4096*x)/(z+(starspeed*8));
			zy=(4096*y)/(z+(starspeed*8));

			starlist[index].z-=starspeed;

			if( (sx < -400) || (sx > 400) || (sy < -200) || (sy > 200) || (z==0) )
			{
				starlist[index].x=-1;
				activestars--;
			}
			else
			{

				if ((index MOD 4)==0)
				{
					gline.r0=255;
					gline.g0=255;
					gline.b0=255;
				}

				if ((index MOD 4)==1)
				{
					gline.r0=0;
					gline.g0=0;
					gline.b0=255;
				}

				if ((index MOD 4)==2)
				{
					gline.r0=0;
					gline.g0=255;
					gline.b0=0;
				}

				if ((index MOD 4)==3)
				{
					gline.r0=0;
					gline.g0=255;
					gline.b0=255;
				}

				gline.r1=255;
				gline.g1=0;
				gline.b1=0;

				gline.x0=sx;
				gline.y0=sy;
				gline.x1=zx;
				gline.y1=zy;
				GsSortGLine(&gline,&Wot[screentoggle],255-120);
			}
		}
	}
}

