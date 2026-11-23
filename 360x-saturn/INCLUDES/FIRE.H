

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Init_Fire(void);
void	Add_Fire(int frame,int x,int y,int dx,int dy);
void	Move_Fire(void);

/**************************************
*          DECLARE DEFINES            *
**************************************/

//#define	MAX_FIRE	12

/**************************************
*         DECLARE STRUCTURES          *
**************************************/

typedef struct
{
	flag		on;
	flag		hit;
	short		type;
	short		dirangle;
	short		livetimer;
	short		target;
	long		id;
} projectilestruct;

projectilestruct	projectile[MAX_FIRE];

/**************************************
*         DECLARE VARIABLES           *
**************************************/

extern	Uint8		weaponammo[8];

extern	long		fire_on[MAX_FIRE];
extern	long		fire_frame[MAX_FIRE];
extern	long		fire_x[MAX_FIRE];
extern	long		fire_y[MAX_FIRE];
extern	long		fire_dx[MAX_FIRE];
extern	long		fire_dy[MAX_FIRE];
extern	long		fire_bx[MAX_FIRE];
extern	long		fire_by[MAX_FIRE];

extern	short		weapontype;
extern	short		freeweaponslots;
extern	short		fire_speed;


