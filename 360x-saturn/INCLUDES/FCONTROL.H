
#ifndef	__H_CONTROL
#define	__H_CONTROL

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Print_Sprites(void);
void	Move_Craft(void);
void	Print_Tests(void);
void	andy_thrust(int ball_num, FIXED angle, FIXED force, FIXED time);
void	update_all(flag pod_attached);
void	ship_pickup_pod(flag *pod_attached);
void	gravity(int ball_num, FIXED time);
void	friction(int ball_num, FIXED time, FIXED fric);
void	move(int ball_num, FIXED time);
void	Init_Ball(void);

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define TIME_STEP	0.0017

#define	PI		3.141592654
#define	pi		3.141592654

extern	LONG		craft;
extern	LONG		sprtemp;

extern	SHORT	sprx;
extern	SHORT	spry;

extern	SPRITE	*spr;

extern	char 		*sample_ptr;
extern	long		sample_length;
extern	flag		light_on;
extern	flag		collision_on;
extern	flag		pod_attached;
extern	char		*source;
extern	char		*shippal_buf;
extern	flag		test_level;
extern	char 		*test_name;
extern	FIXED 	andy_angle;
extern	int		boost;
extern	int		i,iy;
extern	int		craftn;
extern	int		linex1[2],liney1[2];
extern	int		linex2[2],liney2[2];
extern	int		physical_screen;
extern	int 	   angle;
extern	int		rand_factor;
extern	int		ball_x,ball_y;
extern	FIXED	tdx,tdy;
extern	FIXED	adx,ady;
extern	FIXED	x,y,dx,dy;
extern	FIXED 	old_x,old_y;
extern	FIXED 	temp;
extern	int		firen;
extern	int		rotated;
extern	int		bank_factor;
extern	int		bank_factor2;
extern	FIXED	fx,fy,fdx,fdy;
extern	int		fire_delay;
extern	flag		craft_flashing;
extern	int		craft_flash_count;
extern	int		last_scroll_x,last_scroll_y;
extern	long		pod_no;
extern	int		ships_fuel;
extern	int		ships_shield;
extern	int		ships_fuel_deplete;
extern	int		ships_fuel_add;
extern	int		ships_shield_add;
extern	int		ships_gravity;
extern	long		thrust_factor;
extern	FIXED	friction_factor;

#endif
