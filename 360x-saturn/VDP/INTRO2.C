/******************************************************************/
//
// VDP1_5.C
//
// Contains special effects code
//
/******************************************************************/

#include <sgl.h>
#include <libsn.h>
#include "defines.h"
#include	"types.h"
#include	<stdio.h>
#include "globals.h"
#include <math.h>

/* Misc ***********************************************************/

#define		MAX_OPTICS		400

#define		MAX_STARS		1024

#define		MAX_GRAVITONS	10
#define 		X_RAND			600
#define 		Y_RAND			240
#define 		VX_RAND			4
#define 		VY_RAND			4
#define 		MASS_RAND		32

#define 		X_RES				640
#define 		Y_RES				240

#define		OPTIC_Z			256

/* Globals ********************************************************/

StarType		*StarList;
Fibre			*Optics;

Uint16		ActiveStars;
Uint16		LastStar;

#define	POINTS	4

FIXED	PointList[]=
{
	toFIXED(-150),toFIXED(-50),
	toFIXED(150) ,toFIXED(-50),
	toFIXED(150) ,toFIXED(50),
	toFIXED(-150),toFIXED(50),
};

/* ProtoTypes *****************************************************/

void			InitStarList(void);
void			AddStar(void);
void			PlotStars(void);
void			EndStars(void);

void			InitRotateStarList(void);
void			AddRotateStar(void);
void			PlotRotateStars(void);
void			EndRotateStars(void);

void			DoPlasmaField(void);
void			DoFixedPlasmaField1(void);
void			DoFixedPlasmaField2(void);
void 			setbpixel(long x, long y, char col);
void 			setbpixel2(long x, long y, char col);
void 			setspixel(long x, long y, char col);
void			PlasmaVblank(void);

/* Externals ******************************************************/

extern		Uint8		BigBuffer[];
extern		int		rand(void);
extern		void		PrintLARGE(Uint16 x,Uint16 y,Uint8 *string);
extern		void		PrintMEDIUM(Uint16 x,Uint16 y,Uint8 *string);
extern		void		PrintSMALL(Uint16 x,Uint16 y,Uint8 *string);
extern		void		memset(Uint8 *,Uint32,Uint32);
extern		void		LoadScreen(Uint32	addr,Uint8 palno,Uint8	*fname,Uint8 flag);

/******************************************************************/
/******************************************************************/
/******************************************************************/

void	InitStarList(void)
{
	register	Uint16	i;

	StarList		=	(StarType *)BigBuffer;
	ActiveStars	=	0;
	LastStar		=	0;

	for(i=0;i<MAX_STARS;i++)
	{
		StarList[i].x	=	-1;
		StarList[i].y	=	-1;
		StarList[i].z	=	-1;
	}

}

/******************************************************************/

void	AddStar(void)
{
	register	Uint16	i				=	LastStar;
	register	Uint16	iteration	=	0;
	Uint8		found						=	FALSE;
	register	Sint16	x,y;

	while(iteration < MAX_STARS)
	{

		if(StarList[i].x == -1)
		{
			LastStar	=	i;
			found=TRUE;
			break;
		}

		i++;
		i&=MAX_ACTIVE-1;
		
		iteration++;

	}

	if(found==TRUE)
	{
		x	=	((rand() % 640)-320);
		y	=	((rand() % 240)-120);

		StarList[i].x			=	x;
		StarList[i].y			=	y;
		StarList[i].z			=	(rand() % 4096) & 0xfe0;
		StarList[i].radius	=	(rand() % 5)*16;

		ActiveStars++;
	}

}

/******************************************************************/

void	PlotStars(void)
{

	Uint16	i=0;
	Uint16	index=0;
	Sint16	x,y;
	Uint16	z;
	Sint16	sx,sy;
	Sint16	lx,ly;
	SPRITE	Star[1];
	Uint16	col;
	Uint16	kill=0;


	Star[0].CTRL	=	FUNC_Line|0;
	Star[0].PMOD	=	No_Window|ECdis|SPdis|CL_Gouraud;
	Star[0].GRDA	=	CLUTSTART/8;
	Star[0].DMMY	=	0;


	while(i<ActiveStars)
	{

		if(StarList[index].x != -1)
		{

			x	=	StarList[index].x;
		  	y	=	StarList[index].y;
			z	=	StarList[index].z;

			//Nasty piece of code to offset into a palette
			col 	=	StarList[index].radius;
			col 	+=	(z>>8);

			//Calculate screen co-ordinates
			sx		=	(4096*x)/z;
		  	sy		=	(4096*y)/z;

			//Calcuate trailer point
			if(z<2048)
			{
				lx		=	(4096*x)/(z+64);
		  		ly		=	(4096*y)/(z+64);
			}
			else
			{
				lx	=	sx;
				ly	=	sy;
			}
			
			//Decrease z for next loop
			StarList[index].z	-= 64; 

			i++;

			//Kill any stars out of range
			if( (sx < -320) || (sx > 320) || (sy < -120) || (sy > 120) || (z==0) )
			{
				StarList[index].x	=	-1;
				kill++;
			}
			else
			{
				//Plot the star and its trail in req'd color
				Star[0].XA		=	sx;
				Star[0].YA		=	sy;
				Star[0].XB		=	lx;
				Star[0].YB		=	ly;			   
				Star[0].COLR	=	0x8010+col;

				slSetSprite(Star,169*65536);
			}

		}

		index++;
	}

	ActiveStars-=kill;

}

/******************************************************************/

void	EndStars(void)
{
	Uint16	i;

	for(i=0;i<MAX_STARS;i++)
	{
		StarList[i].x			=	-1;
		StarList[i].y			=	-1;
		StarList[i].z			=	-1;
	}

	ActiveStars	=	0;
	LastStar		=	0;
}



/******************************************************************/
/******************************************************************/

void	InitRotateStarList(void)
{
	register	Uint16	i;

	StarList	=	(StarType *)BigBuffer;
	ActiveStars	=	0;
	LastStar		=	0;

	for(i=0;i<MAX_STARS;i++)
	{
		StarList[i].x			=	-1;
		StarList[i].y			=	-1;
		StarList[i].z			=	-1;
		StarList[i].radius	=	-1;
		StarList[i].angle		=	-1;
	}

}

/******************************************************************/

void	AddRotateStar(void)
{
	register	Uint16	i	=	LastStar;
	register	Uint16	iteration	=	0;
	Uint8		found		=	FALSE;

	while(iteration < MAX_STARS)
	{

		if(StarList[i].radius == -1)
		{
			LastStar	=	i;
			found=TRUE;
			break;
		}

		i++;
		i&=MAX_ACTIVE-1;
		
		iteration++;

	}

	if(found==TRUE)
	{
		StarList[i].z			=	(rand() & 3)*16;
		StarList[i].radius	=	(rand() % 320)+10;

		StarList[i].angle	=	((slRandom()));

		ActiveStars++;
	}

}

/******************************************************************/

void	PlotRotateStars(void)
{

	Uint16	i=0;
	Uint16	index=0;
	Sint16	x,y,r,ang;
	Sint16	sx,sy;
	SPRITE	Star[1];
	Uint16	col;
	Uint16	kill=0;

	Star[0].CTRL	=	FUNC_Line|0;
	Star[0].PMOD	=	No_Window|ECdis|SPdis|CL_Replace;
	Star[0].GRDA	=	0xffff;		
	Star[0].DMMY	=	0;


	while(i<ActiveStars)
	{
										  
		if(StarList[index].radius != -1)
		{

			ang	=	StarList[index].angle;
			r		=	StarList[index].radius;

			//col 	=	(8-(r/20))+StarList[index].z;
			col 	=	(16-(r/20));

			x		=	slMulFX(r<<16,(slCos(ang))) >> 16; //Cos
			y		=	slMulFX(r<<16,(slSin(ang))) >> 16;	//Sin

			StarList[index].radius+=2;
			StarList[index].angle+=(163/2);
			//StarList[index].angle%=360;

			i++;

			//Centre to screen 
			sx	=	x+320;
			sy	=	y+120;

			//Kill any stars out of range
		  	if( (sx < -100) || (sx > 800) || (sy < -100) || (sy > 400) || (r<-100) )
			{
				StarList[index].radius	=	-1;
				kill++;
			}
			else
			{
				Star[0].XA		=	sx;
				Star[0].YA		=	sy;
				Star[0].XB		=	sx;
				Star[0].YB		=	sy;			   
				Star[0].COLR	=	0x8010+col;
				//Star[0].COLR	=	0x8010+testcolor;
				
				//Plot the star
				slSetSprite(Star,169*65536);
			}

		}

		index++;
	}

	ActiveStars-=kill;

}


/******************************************************************/

void	EndRotateStars(void)
{
	Uint16	i;

	for(i=0;i<MAX_STARS;i++)
	{
		StarList[i].radius	=	-1;
		StarList[i].angle		=	-1;
	}

	ActiveStars	=	0;
	LastStar		=	0;
}

/******************************************************************/

#if 0
#define SPEEDCOMP	3.3	

void	DoPlasmaField(void)
{

	long i,j;
	float	angle1=0, angle2=0, angle3=0;
	float	angle1bak=0, angle2bak=0, angle3bak=0;
	long	value;


	memset((Uint8 *)VDP2_VRAM_A0,0,(1024*256));

	while (1)
	{

		pollhost();

		angle1bak=angle1;

		angle2bak=angle2;

		angle3bak=angle3;

		for (i=0; i<128; i++)
		{

			angle1-=6.284/128;

			angle2+=2*6.284/128;

			angle3+=4*6.284/128;

			value=16*sin(angle1) + 8*sin(angle2) + 4*sin(angle3); 

			//setbpixel(i+100, 32 + 16*sin(angle1) + 8*sin(angle2) + 4*sin(angle3), 15);

			for (j=0; j<128; j++)
			{
				setbpixel(i+100,j+70, 64+value);	
				value=value+1;
			}						

		}							

		angle1=angle1bak+((1*6.284/320)*SPEEDCOMP/2);

		angle2=angle2bak+((3*6.284/320)*SPEEDCOMP/2);

		angle3=angle3bak+((2*6.284/320)*SPEEDCOMP/2);

		slSynch();

	}

}
#endif

/******************************************************************/

void setbpixel(long x, long y, char col)
{
	//Uint8 *screen	=	(Uint8 *)VDP2_VRAM_A0;
	Uint8 *screen	=	(Uint8 *)BigBuffer;

	*(screen + (y*1024) + x) = col;	
}

/******************************************************************/

void setbpixel2(long x, long y, char col)
{
	//Uint8 *screen	=	(Uint8 *)VDP2_VRAM_B0;
	Uint8 *screen	=	&BigBuffer[1024*240];

	*(screen + (y*1024) + x) = col;	
}


/******************************************************************/

void setspixel(long x, long y, char col)
{

	SPRITE	plasma[1];

	plasma[0].CTRL	=	FUNC_Line|0;
	plasma[0].PMOD	=	No_Window|ECdis|SPdis|CL_Replace;
	plasma[0].COLR	=	0x8000+col; //0x8011
	plasma[0].GRDA	=	0xffff;		
	plasma[0].DMMY	=	0;

	plasma[0].XA		=	x;
	plasma[0].YA		=	y;
	plasma[0].XB		=	x;
	plasma[0].YB		=	y;
	
	slSetSprite(plasma,169*65536);
}

/******************************************************************/

ANGLE		angle1=0, angle2=0, angle3=0, angle4=0, angle5=0;
ANGLE		angle1bak=0, angle2bak=0, angle3bak=0, angle4bak=0, angle5bak=0;

// Very simple plasma with one compound sine wave
void	DoFixedPlasmaField1(void)
{

	register	Uint32 	i,j,vy,vy2;
  	FIXED		sinresult1=0,sinresult2=0,sinresult3=0,sinresult4=0,sinresult5=0;


	//while (1)
	//{
	//	pollhost();

		angle1bak	=	angle1;
		angle2bak	=	angle2;
		angle3bak	=	angle3;
		angle4bak	=	angle4;
		angle5bak	=	angle5;

		for (i=0; i<640; i++)
		{
			sinresult1	=	( slSin(angle1) >> 10 );
			sinresult2	=	( slSin(angle2) >> 10 );
			sinresult3	=	( slSin(angle3) >> 11 );

			sinresult4	=	( slSin(angle4) >> 10 );
			sinresult5	=	( slSin(angle5) >> 12 );

			vy	=	sinresult1+sinresult2+sinresult3;

			vy2	=	sinresult4+sinresult5;
			
			for(j=0;j<240;j++)
			{
				setbpixel(i,j, vy);
				setbpixel2(i,240-j, vy2);
				vy++;vy2++;
				//if(vy > 254 ) break;
			}
						
		  	angle1	+=	DEGtoANG(1);
		  	angle2	+=	DEGtoANG(2);
		  	angle3	+=	DEGtoANG(6);

		  	angle4	+=	DEGtoANG(1);
		  	angle5	+=	DEGtoANG(7);
		}

		angle1	=	(angle1bak+DEGtoANG(1)*11);
		angle2	=	(angle2bak+DEGtoANG(2)*2);
		angle3	=	(angle3bak+DEGtoANG(6)*5);

		angle4	=	(angle4bak+DEGtoANG(1)*10);
		angle5	=	(angle5bak+DEGtoANG(7)*20);

		slDMACopy(BigBuffer,(Uint8 *)VDP2_VRAM_A0,1024*240);
		slDMACopy(&BigBuffer[1024*240],(Uint8 *)VDP2_VRAM_B0,1024*240);

	//	slSynch();

	//}
}

/******************************************************************/

void	DoFixedPlasmaField2(void)
{

	register	Uint32 	i,j,vy;
	ANGLE		angle1=0, angle2=0, angle3=0;
	ANGLE		angle1bak=0, angle2bak=0, angle3bak=0;
  	FIXED		sinresult1=0,sinresult2=0,sinresult3=0;


	memset((Uint8 *)VDP2_VRAM_A0,0,(1024*256));

	while (1)
	{
		pollhost();

		angle1bak	=	angle1;
		angle2bak	=	angle2;
		angle3bak	=	angle3;

		//DbCol(0,8,0);

		for (i=0; i<640; i++)
		{
			sinresult1	=	( slSin(angle1) >> 10 );
			sinresult2	=	( slSin(angle2) >> 10 );
			sinresult3	=	( slSin(angle3) >> 11 );

			vy	=	sinresult1+sinresult2+sinresult3;
			
			for(j=0;j<240;j++)
			{
				setbpixel(i,j, vy&255);
				vy++;
			}
						
		  	angle1	+=	DEGtoANG(1);
		  	angle2	+=	DEGtoANG(2);
		  	angle3	+=	DEGtoANG(6);
		}

		angle1	=	(angle1bak+DEGtoANG(1)*10);
		angle2	=	(angle2bak+DEGtoANG(2)*2);
		angle3	=	(angle3bak+DEGtoANG(6)*5);

		//DbCol(0,0,0);

		slSynch();

	}
}

/******************************************************************/















































