
#ifndef	__H_PARTICLE
#define	__H_PARTICLE

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Init_Particle_List(void);
void	Move_Particles(void);
void	Add_Particle(long x,long y,long dx,long dy);
void	Add_Particle_Acceleration(long x,long y,long dx,long dy,long ddx,long ddy);
void	Draw_Pixels(void);

#endif

