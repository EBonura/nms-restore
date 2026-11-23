
#ifndef	__H_CONTROL
#define	__H_CONTROL

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Print_Sprites(void);
extern	UBYTE	sprframe[];		/* RESTORE: THE FRAME Print_Sprites DREW */
extern	SPRITE	*sprtable[];		/* RESTORE: NEW WORK (owner's decision) */
void	Move_Craft(void);
void	andy_thrust(int ball_num, double angle, double force, double time);
void	update_all(flag pod_attached);
void	ship_pickup_pod(flag *pod_attached);
void	Pickups_Tick(void);		/* RESTORE: THE PC'S PICK-UPS (CONTROL.C) */
extern	int		ships_fuel_add;
extern	int		ships_shield_add;
void	ball_collision2(short ball1, short ball2);	/* RESTORE: THE ROPE (CONTROL.C) */
void	gravity(int ball_num, double time);
void	friction(int ball_num, double time, double fric);
void	move(int ball_num, double time);
void	Init_Ball(void);
void	Add_Level_Pod(void);		/* RESTORE: NEW WORK, MISSION 6'S ONE POD (owner's decision, 2026-11-14) */
void	Reset_Craft(void);		/* RESTORE: THE CRAFT'S START POSITION, EVERY LIFE (PC AND SATURN DO THE SAME) */

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define TIME_STEP	0.0017

#define	PI		3.141592654
#define	pi		3.141592654

#define	FUELMULT		16
#define	FULLSHIELDS	127

/**************************************
*          DECLARE DEFINES            *
**************************************/

extern	LONG		craft;
extern	LONG		sprtemp;

extern	SHORT	sprx;
extern	SHORT	spry;

extern	SPRITE	*spr;

extern	flag		pod_attached;
extern	char		*source;
extern	double 	andy_angle;
extern	int		boost;
//extern	int		i,iy;
extern	int		craftn;
extern	SHORT	survivor_message;	/* RESTORE: SEE CONTROL.C */
extern	int		linex1[2],liney1[2];
extern	int		linex2[2],liney2[2];
extern	int		physical_screen;
extern	int 		angle;
extern	int		rand_factor;
extern	int		ball_x,ball_y;
extern	double	tdx,tdy;
extern	double	adx,ady;
extern	double	x,y,dx,dy;
extern	double 	old_x,old_y;
extern	double 	temp;
extern	int		firen;
extern	int		rotated;
extern	int		bank_factor;
extern	int		bank_factor2;
extern	double	fx,fy,fdx,fdy;
extern	int		fire_delay;
extern	flag		craft_flashing;
extern	int		craft_flash_count;
extern	int		last_scroll_x,last_scroll_y;
extern	long		pod_no;
extern	int		ships_gravity;
extern	long		thrust_factor;
extern	double	friction_factor;

extern	USHORT	fuel;
extern	USHORT	shields;
extern	FLAG		craftdestroyed;
extern	USHORT	deadtimer;

#endif

