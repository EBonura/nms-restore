/**************************************
*            INCBIN HEADERS           *
**************************************/

#include	<sgl.h>
#include "defines.h"
#include "types.h"
#include "globals.h"
#include <stdio.h>
#include <libsn.h>

/***************************************************
*   Prototypes                                     *
****************************************************/

void	Init_Particle_List(void);
void	Move_Particles(void);
void	Add_Particle(long x,long y,long dx,long dy);
void	Add_Particle_Acceleration(long x,long y,long dx,long dy,long ddx,long ddy);
void	Draw_Pixels(void);

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	MAX_PARTICLES	(512)

/**************************************
*          DECLARE VARIABLES          *
**************************************/

long		particle_list_mask[MAX_PARTICLES/32];
long		particle_list_x[MAX_PARTICLES+1];
long		particle_list_y[MAX_PARTICLES+1];
long		particle_list_dx[MAX_PARTICLES+1];
long		particle_list_dy[MAX_PARTICLES+1];
long		particle_list_ddx[MAX_PARTICLES+1];
long		particle_list_ddy[MAX_PARTICLES+1];
long		particle_list_counter[MAX_PARTICLES+1];
long		particle_list_delay[MAX_PARTICLES+1];
long		particle_list_halflife[MAX_PARTICLES+1];

extern	Uint16		ScrollX;
extern	Uint16		ScrollY;
extern	int			rand(void);
extern	Uint8			*collmap;
extern	Uint16		MapWidth;
extern	flag			SHcollision_at(long x,long y);

SPRITE	main_particle={FUNC_Line|0,0,Window_In|ECdis|SPdis,0xffff,0x0000,0x0000,0,0,0,0,0,0,0,0,0,0};

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
		particle_list_x[i]			=-1;
		particle_list_y[i]			=-1;
		particle_list_dx[i]			=-1;
		particle_list_dy[i]			=-1;
		particle_list_counter[i]	=0;
		particle_list_delay[i]		=0;
		particle_list_halflife[i]	=0;
	}

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
					return;
				}

				i++;
			}
		}
	}
}

/**************************************
*        UPDATE PARTICLE LIST         *
**************************************/

void Move_Particles(void)
{

	Uint32	i,j,k,x,y;

	for (j=0;j<(MAX_PARTICLES/32);j++)
	{
		if(particle_list_mask[j]!=0)
		{
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

					x>>=16;
					y>>=16;

					//if( SHcollision_at(x,y)==TRUE )
					if( (collmap[((x+MapWidth*y)>>3)] & (1<<(7-(x&7))) ) !=0 )
					{
					 	particle_list_counter[i]=0;
						particle_list_x[i]=-1;
						particle_list_y[i]=-1;
						particle_list_mask[j]&=(0xffffffff-(1<<(i&31)));
					}

				}
				else
				{
					particle_list_mask[j]&=(0xffffffff-(1<<(i&31)));
				}

				i++;
			}
		}

	}

}

// 15.35

/**************************************
*    PLOT THE PARTICLES TO SCREEN     *
**************************************/

void Draw_Pixels(void)
{

	long			ax,ay,pax,pay,i,j,k;
	SPRITE		*particle	=	&main_particle;
	Uint16		r,g,b;
	

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
							
							particle[0].COLR	=	C_RGB(r,g,b);
				
							particle_list_counter[i]--;
							
							slSetSprite(particle,170*65536);
						}
						else
						{
							particle_list_counter[i]=-2;
						}
					}
				}

				i++;
			}
		}

	}

}


