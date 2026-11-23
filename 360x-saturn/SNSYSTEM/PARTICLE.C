
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include	<sgl.h>
#include "defines.h"
#include "types.h"
#include "globals.h"
#include <stdio.h>
#include <libsn.h>
#include "particle.h"

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	USEMASK
#define	MAX_PARTICLES	(512)

/**************************************
*          DECLARE VARIABLES          *
**************************************/

long		particle_list_mask[MAX_PARTICLES/32];
long		particle_list_x[MAX_PARTICLES];
long		particle_list_y[MAX_PARTICLES];
long		particle_list_dx[MAX_PARTICLES];
long		particle_list_dy[MAX_PARTICLES];
long		particle_list_ddx[MAX_PARTICLES];
long		particle_list_ddy[MAX_PARTICLES];
long		particle_list_counter[MAX_PARTICLES];
long		particle_list_delay[MAX_PARTICLES];
long		particle_list_halflife[MAX_PARTICLES];

Uint32	ActiveParticles;
Uint32	LastParticle;

extern	Uint16		ScrollX;
extern	Uint16		ScrollY;
extern	int			rand(void);
extern	Uint8			*collmap;
extern	Uint16		MapWidth;


/**************************************
*     INITIALISE PARTICLE LIST        *
**************************************/


void Init_Particle_List(void)
{
	int i;

	for(i=0;i<(MAX_PARTICLES/32);i++)
	{
		particle_list_mask[i]=0;
	}

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

	ActiveParticles	=	0;
	LastParticle		=	0;

}

/**************************************
*        UPDATE PARTICLE LIST         *
**************************************/


void Move_Particles(void)
{

	long	i,j,k,x,y,found=0,kill=0;

	for (j=0;j<(MAX_PARTICLES/32);j++)
	{
		if(particle_list_mask[j]!=0)
		{
			found++;
			i=j*32;

			for(k=0;k<32;k++)
			{
				if(particle_list_counter[i]>0)
				{
					if (particle_list_delay[i]>0)
					{
						particle_list_delay[i]--;
					}
					else
					{	
						particle_list_counter[i]--;
						particle_list_delay[i]=particle_list_halflife[i];
					}

					x	=	particle_list_x[i];
					y	=	particle_list_y[i];
					x	+=	particle_list_dx[i];
					y	+=	particle_list_dy[i];

					particle_list_dx[i]+=particle_list_ddx[i];
					particle_list_dy[i]+=particle_list_ddy[i];

				   particle_list_x[i]=x;
					particle_list_y[i]=y;

					x=x/65536;
					y=y/65536;

					if( (collmap[((x+MapWidth*y)>>3)] & (1<<(7-(x&7))) ) !=0 )
					{
					 	particle_list_counter[i]=0;
						particle_list_x[i]=-1;
						particle_list_y[i]=-1;
						particle_list_mask[j]&=(0xffffffff-(1<<(i&31)));
						kill++;
					}

				}
				else
				{
					particle_list_mask[j]&=(0xffffffff-(1<<(i&31)));
				}

				i++;
			}
		}

		if(found==ActiveParticles) break;

	}

	ActiveParticles-=kill;

}

/**************************************
*   ADD A PARTICLE FOR BACK OF SHIP   *
**************************************/


void Add_Particle(long x,long y,long dx,long dy)
{

	long	i,j,k;

	for(j=0;j<(MAX_PARTICLES/32);j++)
	{
		if(particle_list_mask[j]!=0xffffffff)
		{
			i=j*32;

			for(k=0;k<32;k++)
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
					ActiveParticles++;
					LastParticle	=	i;
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


void Add_Particle_Acceleration(long x,long y,long dx,long dy,long ddx,long ddy)
{

	long	i,j,k;

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
					ActiveParticles++;
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

void Draw_Pixels(void)
{

	long			ax,ay,pax,pay,i,j,k,found=0,kill=0;
	SPRITE		particle[1];
	Uint16		r,g,b;
	
	particle[0].CTRL	=	FUNC_Line|0;
	particle[0].PMOD	=	Window_In|ECdis|SPdis;
	particle[0].COLR	=	C_RGB(31,31,31);
	particle[0].GRDA	=	0xffff;		
	particle[0].DMMY	=	0;

	for (j=0;j<(MAX_PARTICLES/32);j++)
	{
		if (particle_list_mask[j]!=0)
		{
			i=j*32;

			found++;

			for (k=0;k<32;k++)
			{
				if (particle_list_counter[i]!=0)
				{
					if (particle_list_counter[i]<128)
					{

						//SCX,SCY ARE ALWAYS < PARTICLES!
						pax	=	(particle_list_x[i])>>16;
						pay	=	(particle_list_y[i])>>16;
				
					  	ax	=	(pax-ScrollX);
					  	ay	=	(pay-ScrollY);
				
						if((ax>=0) && (ax < 320) && (ay>=0) && (ay<240)) 
						{
							particle[0].XA		=	ax;
							particle[0].YA		=	ay;
				
							particle[0].XB		=	ax;
							particle[0].YB		=	ay;			   
				
					  		r=(particle_list_counter[i])>>1;
					  		g=(particle_list_counter[i])>>1;
					  		b=(particle_list_counter[i])>>1;
				
							particle_list_counter[i]--;
				
							particle[0].COLR	=	C_RGB(r,g,b);
							
							slSetSprite(particle,170*65536);
						}
						else
						{
							particle_list_counter[i]=-2;
							kill++;
						}
					}
				}

				i++;
			}
		}

		if(found==ActiveParticles) break;
	}

	ActiveParticles-=kill;

}


