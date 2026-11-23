/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Print_Sprites(void);
void	Move_Craft(void);
void	Print_Tests(void);
void	andy_thrust(int ball_num, ANGLE angle, FIXED force, FIXED time);
void	update_all(flag pod_attached);
void	ship_pickup_pod(flag *pod_attached);
void	gravity(int ball_num, FIXED time);
void	friction(int ball_num, FIXED time, FIXED fric);
void	move(int ball_num, FIXED time);
void	Init_Ball(void);

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define 	TIME_STEP	(0.0017*65536.0)

#define	PI		3.141592654
#define	pi		3.141592654

extern	LONG		craft;

