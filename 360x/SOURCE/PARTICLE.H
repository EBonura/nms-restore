
#ifndef	__H_PARTICLE
#define	__H_PARTICLE

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Init_Particle_List(void);
void	Move_Particles(void);
void	Add_Particle(long x,long y,long dx,long dy);
void	Add_Particle_Acceleration(long x,long y,long dx,long dy,long ddx,long ddy);
void	Draw_Particles(void);
void	Init_Star_List(void);
void	Add_Star(void);
void	Plot_Stars(SHORT starspeed);

#endif

