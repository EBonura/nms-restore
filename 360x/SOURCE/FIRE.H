
#ifndef	__H_FIRE
#define	__H_FIRE

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Init_Fire(void);
void	Add_Fire(int frame,int x,int y,int dx,int dy);
void	Move_Fire(void);
void	process_collisions(void);
void	Death_Burst(LONG x,LONG y,LONG dirangle);	/* RESTORE: NEW WORK (owner's decision) */		/* RESTORE: NMS'S, REVIVED */

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	MAX_FIRE	12

/**************************************
*         DECLARE STRUCTURES          *
**************************************/

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

projectilestruct	projectile[MAX_FIRE];

/**************************************
*         DECLARE VARIABLES           *
**************************************/

extern	UBYTE	weaponammo[9];	/* RESTORE: was [8], see FIRE.C */
extern	void	Reset_Weapon_Ammo(void);	/* RESTORE: NMS'S AMOUNTS (FIRE.C) */

extern	LONG		fire_on[MAX_FIRE];
extern	LONG		fire_frame[MAX_FIRE];
extern	LONG		fire_x[MAX_FIRE];
extern	LONG		fire_y[MAX_FIRE];
extern	LONG		fire_dx[MAX_FIRE];
extern	LONG		fire_dy[MAX_FIRE];
extern	LONG		fire_bx[MAX_FIRE];
extern	LONG		fire_by[MAX_FIRE];

extern	SHORT	weapontype;
extern	SHORT	freeweaponslots;
extern	SHORT	fire_speed;

#endif

