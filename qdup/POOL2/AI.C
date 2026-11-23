/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/*									    	 QD-UP      										 */
/*											PC-CD ROM											 */
/*									 For BMG	Interactive										 */
/*									 By NMS Software Ltd.									 */
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#define PAN_FRAC 8
#include	"globals.h"

long		save_nbc,alt;
double	alt_angle,alt_angle1,original_angle;
double	computer_angle;
double	pocket_dist;
double	calculated_speed;
double	angle_to_pocket;
double	angle_difference;
double	tmp_angle_difference;
double	deflection_angle;
double	save_speed_to_pocket;

/* ----------------------------------------------------------- */
/* ----------------------------------------------------------- */
/* 				   Computer player statistics	 				      */
typedef	struct	{
		int	maximum_speed;
		int	position_skill;
		int	not_potting_white_skill;
		int	applying_english_skill;
		int	getting_out_of_snookers;
		int	potting_skill;
		int	doubling_skill;
		int	plant_shot_skill;
		int	snookering_skill;
		int	applying_masse_skill;
		} Cpu_Statistics;

/* ----------------------------------------------------------- */
Cpu_Statistics Computer_Player_statistics[16] = {

				/* percentage values 0-100 */

		 80 ,100, 70 ,66, 80, 80, 50,100, 10, 30,  //player 1
		 100, 55, 50, 90,100,100,100, 80,100, 10,  //player 2
		 100, 70, 70, 99,100,100,100, 70,100, 10,  //player 3
		 100, 80, 80, 45,100,100,100, 55,100, 20,  //player 4
		 100, 30, 85, 66,100,100,100, 80, 76, 25,  //player 5
		 100, 40, 65, 12,100,100,100, 70, 35, 55,  //player 6
		 100, 60, 54, 46,100,100,100, 50, 50, 35,  //player 7
		 100, 90, 10, 47,100,100,100, 20,  0, 10,  //player 8
		 100, 85,  0, 51,100,100,100, 40, 20,  0,  //player 9
		 100, 25, 40, 27,100,100,100, 30, 65, 40,  //player 10
		 100, 35, 60, 34,100,100,100, 85, 44, 14,  //player 11
		 100, 96, 75, 88,100,100,100, 96, 20, 20,  //player 12
		 100, 40, 70, 64,100,100,100, 30, 70, 40,  //player 13
		 100, 10, 45, 12,100,100,100, 50, 50, 50,  //player 14
		 100, 30, 40, 44,100,100,100, 55, 30, 70,  //player 15
		 100, 66, 74, 82,100,100,100,100, 10, 20,  //player 16
		};
/* ----------------------------------------------------------- */
int cue_move_count=0;
ext double cue_movement[];

void	computer_shot(void)
{
		long	check_ball;
		double	xvel,yvel,angle;
     long count;
     long	stroke_start;		// RESTORE: NEW WORK 3D view, see below

/* setup camera position for computer player */

      cue_position_table_ptr=0;
      calculating_computer_shot = YES;
		setup_ai_camera();
		show_shot_flag=YES;

// RESTORE: NEW WORK 3D view (owner's decision): the camera's move to the
// computer's view eases one step per VBlank, not per drawn frame: a 3D
// frame takes 2 to 3 VBlanks, so the move took 2.3 s (median) at the 3D
// frame rate.  Each VBlank a frame took past its first gives the ease one
// more step (E_UpdateCamera, the step draw_3d makes once a frame); the
// camera ends where it did.  (The 2D build has no camera move.)
      {
      long	f0, k;

      while  ( (E_FinishedEase()==NO) )
       {
        f0 = frames;
        Handle3D();
        for(k = frames - f0 - 1; (k > 0) && (E_FinishedEase()==NO); k--) E_UpdateCamera();
       }
      }

/* if in computer mode , then check where cue ball is and bring camera close
est to it */

		check_ball = Select_Ball_To_Pot();

		if	(nine_ball_count > LAST_BALL-1) return;

/* start ai */

     get_best_shot();

     angle = cue_angle;
     cue_rot.y = angle;
     cue_rot.y&=4095;
     fixed_cue_rot.y =cue_rot.y;

     count=0;
     shoot_mode=0;
     show_shot_flag=NO;
     stroke_start = frames;		// RESTORE: NEW WORK 3D view, see below
     while ( count < 38 )
      {
      cue_position=cue_movement[count];

/* setup cue in currect positon */

      if ( cue_position < FIX24_TO_DOUBLE(ball[WHITE_BALL].radius) )    	// RESTORE: radius 8.24
       {
        cue_position = FIX24_TO_DOUBLE(ball[WHITE_BALL].radius);
       }

// RESTORE: NEW WORK 3D view (owner's decision): the cue stroke keeps the 2D
// build's pace, step count of cue_movement shown at VBlank 2 * count of the
// stroke (the 2D build draws a frame every 2 VBlanks), instead of one step
// a frame (2 to 3 VBlanks or more in 3D); steps that fall between frames
// are not drawn, the last one (the cue at the ball) always is.  The shot is
// the same: it is struck with cue_angle and speed_meter afterwards, not by
// the stroke.
      {
      long	next;

      Handle3D();
      refresh_whole_screen=2;
      next = (frames - stroke_start) / 2;
      if(next <= count) next = count + 1;
      if((count < 37) && (next > 37)) next = 37;
      count = next;
      }
     }

    calculating_computer_shot = NO;

/* got perfect angle use this to shoot */
}

#define	RANGE_OF_ANGLES	10
									 
#define	MAX_ANGLE			95	/* maximum angle to pocket before checking 
										for another pocket */

#define	FINE_ANGLE			128

#define 	AI_BALL_UPDATE	3/60.0

#define	GRIDSIZE	1.544940/2.0

#define	TOP_BOTTOM_SPIN_SCALE	0.1029933*2.5
#define	LEFT_RIGHT_SPIN_SCALE	0.0514966*2.5

/* snooker types of shot */

/* other types of shot */
#define	STRAIGHT_POT							0
#define	SNOOKER									1
#define	DOUBLE_OFF_CUSHION					2		
#define	POT_FROM_SNOOKER						3
#define	PLANT_SHOT								4
#define	DOUBLE_OFF_CUSHION_FROM_SNOOKER	5
#define	PLANT_SHOT_FROM_SNOOKER				6
#define	MAKE_SURE_NOT_SNOOKERED_ON_SECOND_OBJECT_BALL 7
#define	MASSE									8
#define	REBOUND_OFF_CUSHION				9
#define	SAFETY_AFTER_MASSE				10

 flag shot_type;
flag	take_shot(int angle);

void   get_best_shot(void) 
{

  shot_type = STRAIGHT_POT;

  switch ( shot_type )
   {
    case STRAIGHT_POT:

// RESTORE: NEW WORK (owner's decision): qdup/new/aifast.c's faster search
// over NMS's own candidates; QDUP_EXACT_AI builds keep NMS's search,
// QDUP_AI_COMPARE test builds run both (tests/qdup-ai)
#if defined(QDUP_AI_DUEL)
    { extern void get_best_shot_duel(void); get_best_shot_duel(); }
#elif defined(QDUP_AI_COMPARE)
    { extern void get_best_shot_compare(void); get_best_shot_compare(); }
#elif defined(QDUP_EXACT_AI)
    get_best_shot_for_straight_pot();
#else
    { extern void get_best_shot_fast(void); get_best_shot_fast(); }
#endif

    break;
   }
}
/* ------------------------------------------------------------------------ */
void get_best_shot_for_straight_pot(void)
{
  long shot_count,l,a;
  int angle_to_pocket_clockwise,angle_to_pocket_anticlockwise,angle_to_ball;
  
   save_nine_ball_count=nine_ball_count;
   cue_delta_table[0] = (60 * (cue_weight*1.5));

   for (l=0;l<LAST_BALL;l++)
    saveball[l] = ball[l];

   shot_count=0;
   cue_move_count=0;

  angle_to_pocket_clockwise = find_best_pocket();
  angle_to_pocket_anticlockwise = angle_to_pocket_clockwise;
  angle_to_ball = angle_to_pocket_clockwise;

  count=0;
  while ( count < 60 )
   {

    if (keys[kX]==128) break;

    if (count%2 == 1) 
      {
       cue_angle=((angle_to_pocket_clockwise * 4096) / 360)+1024; 
       angle_to_pocket_clockwise += 1;
       angle_to_pocket_clockwise%=360;
      }
    else 
      {
       cue_angle=((angle_to_pocket_anticlockwise * 4096) / 360)+1024;
       if ( (angle_to_pocket_anticlockwise -- ) < 0) angle_to_pocket_anticlockwise+=360;
		}

    if ( (take_shot(cue_angle) == TRUE) ) break;

    reset_shot();
    count++;
	}

/* if unable to pocket ball then just hit ball */

   if (count >= 45)  cue_angle=((angle_to_ball * 4096) / 360)+1024;

   if ( cue_move_count < 35)
    {
 
      for (a=cue_move_count; a<35;a++)
      {
		while	((any_balls_moving()==TRUE))	 {

/* save all ball data to restore after computer angle calculated */

  		update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24
  		update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24

/* exit ball movement on these conditions */

//     if (keys[kD]==128) break;
//       Handle3D();
//       wait_for_any_key();

			//---------------------------------------
	  }
	  }
    }
   reset_shot();
}
/* ------------------------------------------------------------------------ */
int find_best_pocket(void)
{
 long x,y,px,py;		// RESTORE: 8.24, were double; angles from fix_atan2
 int count,angle_to_ball,angle_to_pocket,best_pocket,check_ball;

 check_ball = Select_Ball_To_Pot();

 x = ball[check_ball].x - ball[0].x;
 y = ball[check_ball].y - ball[0].y;

 if	( ( angle_to_ball=FIX_ATAN2_DEG(y,x)) < 0) angle_to_ball+=360;	// RESTORE
 angle_to_ball=360-angle_to_ball;

 count=0 ;
 while (  count < 6 )
  {

/* get angle to pocket */

   px = pockets[ count ].x - ball[check_ball].x;
   py = pockets[ count ].y - ball[check_ball].y;
 
   if	( ( angle_to_pocket=FIX_ATAN2_DEG(py,px)) < 0) angle_to_pocket+=360;	// RESTORE
   angle_to_pocket=360-angle_to_pocket;

   best_pocket = angle_to_pocket - angle_to_ball;
   if ( abs(best_pocket) < 45 ) break;
   count++;
  }

  angle_to_pocket = 360 - angle_to_pocket;
  x = ball[check_ball].x - mul64(slCos(DEG_TO_ANG(angle_to_pocket))<<8, ball[0].diameter );	// RESTORE: 8.24
  y = ball[check_ball].y - mul64(slSin(DEG_TO_ANG(angle_to_pocket))<<8, ball[0].diameter );

  x -= ball[0].x;
  y -= ball[0].y;

  if	( ( angle_to_pocket=FIX_ATAN2_DEG(y,x)) < 0) angle_to_pocket+=360;	// RESTORE
  angle_to_pocket=360-angle_to_pocket;

/* get best angle to hit ball */  

  return(angle_to_pocket);
}
/* ------------------------------------------------------------------------ */
flag	take_shot(int angle)
{
double	steves_angle,tmp_angle;
static   int count;

/* clear white ball flags */

		LOWEST_BALL_HIT_FIRST_FLAG=NO;
		first_hit = NO;

 	   speed_meter=fabs(cue_delta_table[0]);
      tmp_angle = (angle-1024)&4095;
      steves_angle= (tmp_angle*360.0)/4096.0;
// RESTORE: NMS's call had an older hit_ball's argument order (angles in
// radians first, then speed, then the cue tip offsets): BALL.C's hit_ball
// took the cue angle as the player's shot passes it (cue_angle, 4096 to the
// turn), so the trial shot went off with the speed read from y_hit_pos.
// Called as the player's shot is (GAME.C set_target_for_white).
		hit_ball(WHITE_BALL, x_hit_pos, y_hit_pos, angle, butt, DOUBLE_TO_FIX24(speed_meter));

		while	((any_balls_moving()==TRUE))	 {

		count++;

/* save all ball data to restore after computer angle calculated */

  		update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24
  		update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24

/* exit ball movement on these conditions */
//     if (keys[kD]==128) break;
//       Handle3D();
//       wait_for_any_key();

			if	(ball[WHITE_BALL].active==NO) return FALSE;	// RESTORE: was ==YES, read as a typo for ==NO (the owner's decision): ==YES is true from the first step, the white being on the table, so no trial ever ran on; a trial fails when the white is potted
			if	(potted_hi_ball_flag != 0)	return TRUE;
			if	(potted_good_ball_flag != 0)	return TRUE;
		
			//---------------------------------------
	}

   
   return FALSE;
}
#if 0
// RESTORE: nothing in the game calls the functions from here to the #endif
// (the 1996 source's AI uses get_best_shot, find_best_pocket and take_shot
// only).  Their maths takes ball positions as BALL.C's doubles, which are
// 8.24 integers now (FIXBALL.C), so they are left out rather than kept with
// wrong units.
/* --------------------------------- */
/* check to see if pocketed ball as
 been been pocketed in closest pocket? */

#define	SPEED_ADD	20.55
double		best_pocket1;
double	Return_angle_to_closest_pocket(long check_ball)
{
	double	x,y,x1,y1;
	double	dist,best_dist;
	long	p,best_pocket;

	best_dist=100000;

	/* check closest pocket */

	for	(p=0; p < 6; p++)	{

	x1=ball[check_ball].x;
	y1=ball[check_ball].y;

/* calculate point of pocket ooto baize */
	switch	(p)
	{

	case	0:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	1:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	2:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	3:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	4:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	5:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	}

	x-=x1;
	y-=y1;

	if	( (angle_to_pocket=atan2(y,x)/deg2rad) < 0) angle_to_pocket+=360;

	dist = hypot(x,y);

  	if	(  dist < best_dist )  {
						best_pocket=p;
						best_dist=dist;
						best_pocket1=angle_to_pocket;
		}							 
	}

	return	best_pocket1;
}
/* ------------------------------------- */
long	Check_best_Pocket(long check_ball)
{
	double	x,y,x1,y1;
	double	angle_to_ball,angle;
	double	best_angle;
	double	speed_to_object_ball,speed_to_pocket,speed_to_object_ball2;
	long		best_pocket;
	long	p;

	best_pocket=-1;

	/* check to see if able to pot to this pocket.. within +-45 degr.*/

	x=ball[WHITE_BALL].x;
	y=ball[WHITE_BALL].y;
	x1=ball[check_ball].x-x;
	y1=ball[check_ball].y-y;

	if	( (angle_to_ball=atan2(y1,x1)/deg2rad) < 0) angle_to_ball+=360;

	best_angle=180;

	/* check closest pocket */

	for	(p=0; p < 6; p++)	{

	x1=ball[check_ball].x;
	y1=ball[check_ball].y;

/* calculate point of pocket ooto baize */
	switch	(p)
	{

	case	0:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	1:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	2:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	3:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	4:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	5:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	}

	x-=x1;
	y-=y1;

	if	( (angle_to_pocket=atan2(y,x)/deg2rad) < 0) angle_to_pocket+=360;

	if	( (angle = angle_to_ball - angle_to_pocket ) <0 )angle+=360;

	if	( angle > 180) angle=360-angle;

  	if	( (MAX_ANGLE-fabs(angle) > 0) && (fabs(angle) < best_angle))  {
						best_pocket=p;
						best_angle=fabs(angle);
						angle_difference = angle_to_pocket;
						deflection_angle = angle;
		}
	}


	/* calculate speed from Cue ball to Object ball1 to pocket */

	x1=ball[check_ball].x;
	y1=ball[check_ball].y;
	x=ball[WHITE_BALL].x-x1;
	y=ball[WHITE_BALL].y-y1;

	speed_to_object_ball = hypot(x,y)*SPEED_ADD;

	x1=ball[check_ball].x;
	y1=ball[check_ball].y;

	switch	(best_pocket)
	{

	case	0:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	case	1:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	case	2:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	case	3:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	case	4:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	case	5:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	}

	x-=x1;
	y-=y1;

	speed_to_pocket = hypot(x,y)*SPEED_ADD;
	save_speed_to_pocket = hypot(x,y)*SPEED_ADD;

/* move to next ball in order */

	if	(nine_ball_count < LAST_BALL-1)	{

	nine_ball_count++;
	check_ball = Select_Ball_To_Pot();

	x=ball[check_ball].x-x1;
	y=ball[check_ball].y-y1;

	speed_to_object_ball2 = hypot(x,y)*SPEED_ADD;

	}
	else	speed_to_object_ball2 = 3;

	calculated_speed = speed_to_object_ball + speed_to_pocket + speed_to_object_ball2;

	if	(y_hit_pos > 0)	{
	calculated_speed+=calculated_speed;
	}

/* reset nine ball count */
	nine_ball_count=save_nbc;

	return	best_pocket;
}
/* ------------------------------------- */
long	Check_best_double_Pocket(long check_ball)
{
	double	x,y,x1,y1;
	double	angle_to_ball,angle;
	double	best_angle;
	double	speed_to_object_ball,speed_to_pocket,speed_to_object_ball2;
	long	check_ball1;
	long 	best_pocket;
	long	TOP_BOTTOM_BUMPER;
	long	p,tmp_p;

	best_pocket=-1;

	/* check to see if able to pot to this pocket.. within +-45 degr.*/

	x=ball[WHITE_BALL].x;
	y=ball[WHITE_BALL].y;
	x1=ball[check_ball].x-x;
	y1=ball[check_ball].y-y;

	if	( (angle_to_ball=atan2(y1,x1)/deg2rad) < 0) angle_to_ball+=360;
	best_angle=180;

	/* check closest pocket */

	for	(p=0; p < 6; p++)	{

	x1=ball[check_ball].x;
	y1=ball[check_ball].y;

/* calculate point of pocket ooto baize */
	switch	(p)
	{

	case	0:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	1:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	2:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	3:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	4:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	case	5:
	x=pockets[p].x;
	y=pockets[p].y;
	break;

	}

	x-=x1;
	y-=y1;

	if	( (angle_to_pocket=atan2(y,x)/deg2rad) < 0) angle_to_pocket+=360;

/* check out of range */

	if	( (angle = angle_to_ball - angle_to_pocket) < 0 ) angle+=360;  
	if	( angle >= 180) angle=360-angle;

  	if	( (MAX_ANGLE-(long)(fabs(angle)) >= 0) && (abs(angle) < best_angle))  {

	/* check to see if a side cushion or top/bottom cushion */

#define	TOP_BUMPER_ANGLE	45
#define	BOTTOM_BUMPER_ANGLE 45

	TOP_BOTTOM_BUMPER =NO;

	if	((angle_to_ball <= BOTTOM_BUMPER_ANGLE) || (angle_to_ball >= 360-BOTTOM_BUMPER_ANGLE) ) TOP_BOTTOM_BUMPER=YES;
	if	((angle_to_ball >= 180-TOP_BUMPER_ANGLE) && (angle_to_ball <= 180+TOP_BUMPER_ANGLE))  TOP_BOTTOM_BUMPER=YES;
	
	if	(TOP_BOTTOM_BUMPER == YES)
	{

	/* top/bottom pocket conversion */
		switch	(p)	{

			case	0:
				tmp_p=2;
				break;
			case	5:
				tmp_p=3;	
				break;
			case	2:
				tmp_p=0;
				break;
			case	3:
				tmp_p=5;
				break;
			}

	}
	else	{

	/* side cushion pocket conversion */

		switch	(p)	{
			case	0:
				tmp_p=5;
				break;
			case	1:
 				tmp_p=4;
				break;
			case	2:
				tmp_p=3;
				break;
			case	3:
				tmp_p=2;
				break;
			case	4:
				tmp_p=1;
				break;
			case	5:
				tmp_p=0;
				break;
			}
		}
	

		best_pocket=tmp_p;

		x=pockets[p].x-x1;
		y=pockets[p].y-y1;
		if	( (angle_to_pocket=atan2(y,x)/deg2rad) < 0) angle_to_pocket+=360;
		angle = angle_to_ball - angle_to_pocket  ;
		best_angle=fabs(angle);
		}
	}

	/* calculate speed from Cue ball to Object ball1 to pocket */

	x1=ball[check_ball].x;
	y1=ball[check_ball].y;
	x=ball[WHITE_BALL].x-x1;
	y=ball[WHITE_BALL].y-y1;

	speed_to_object_ball = hypot(x,y)*SPEED_ADD;

	x1=ball[check_ball].x;
	y1=ball[check_ball].y;

	switch	(best_pocket)
	{

	case	0:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	case	1:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	case	2:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	case	3:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	case	4:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	case	5:
	x=pockets[best_pocket].x;
	y=pockets[best_pocket].y;
	break;

	}

	x-=x1;
	y-=y1;

	speed_to_pocket = hypot(x,y)*SPEED_ADD;

/* move to next ball in order */

	if	(nine_ball_count < LAST_BALL-1)	{
	nine_ball_count++;
	check_ball1 = Select_Ball_To_Pot();

	x=ball[check_ball1].x-x1;
	y=ball[check_ball1].y-y1;

	speed_to_object_ball2 = hypot(x,y)*SPEED_ADD;

	}
	else	speed_to_object_ball2 = 3;

	calculated_speed = speed_to_object_ball + speed_to_pocket + speed_to_object_ball2;

	y_hit_pos=0;
	x_hit_pos=0;

//	if	(ball[WHITE_BALL].x+GRIDSIZE*2 < ball[check_ball].x+GRIDSIZE*2) x_hit_pos=-58000;
//	else	x_hit_pos=50000;

	if	(y_hit_pos > 0)	{
	calculated_speed+=calculated_speed;
	}


/* reset nine ball count */
	nine_ball_count=save_nbc;

	return	best_pocket;
}

/* ------------------------------------- */
long	Get_good_angle_to_Object_ball_1_(void)
{
	long	check_ball;
	long	ball_test;
	double	line_test,xvel,yvel,white_posx,white_posy,line_test1;
	double	speed;
	double	angle;

/* test line of sight to see if any ball in path, if so then try new angle+
ball radius */

		check_ball = Select_Ball_To_Pot();

		xvel=ball[check_ball].x-ball[WHITE_BALL].x;
		yvel=ball[check_ball].y-ball[WHITE_BALL].y;

		if	( ( angle=(double)atan2(yvel,xvel)/deg2rad) < 0) angle+=360;
		angle=360-angle;
		if	( (angle +=90) >= 360) angle-=360;

		//set_line((angle-90.0)*2*PI/360);

		speed=(hypot(xvel,yvel))*4;

		line_test1=0;
		for	(line_test=0; line_test < speed; line_test+=.1)	{

		if	(keys[kK]==128) return FALSE;

				white_posx = ball[WHITE_BALL].x+(line_test1*ball[WHITE_BALL].vx1);
				white_posy = ball[WHITE_BALL].y+(line_test1*ball[WHITE_BALL].vy1);
				line_test1++;


				for	(ball_test=1; ball_test < LAST_BALL; ball_test++)	{
			
					xvel=(ball[ball_test].x) - (white_posx);	
					yvel=(ball[ball_test].y) - (white_posy);	

  				/*	
					save_x=ball[WHITE_BALL].x;
					save_y=ball[WHITE_BALL].y;
					ball[WHITE_BALL].x=(white_posx);	
					ball[WHITE_BALL].y=(white_posy);	
					Handle3D();

					ball[WHITE_BALL].x=save_x;
					ball[WHITE_BALL].y=save_y;
				  */	

					if	((double)hypot(xvel,yvel) <= ball[ball_test].diameter)	{
						if	(check_ball != ball_test )	return	TRUE;
						else	return	FALSE;
					}
				}
			}
	return	FALSE;
}
/* ------------------------------------- */
long	Check_Around_object_ball_1_(void)
{
	long	check_ball;
	long	ball_test;
	double	xvel,yvel;

		check_ball = Select_Ball_To_Pot();

				for	(ball_test=1; ball_test < LAST_BALL; ball_test++)	{

					if	(ball_test != check_ball)	{

					xvel=(ball[ball_test].x) - (ball[check_ball].x);
					yvel=(ball[ball_test].y) - (ball[check_ball].y);

					if	((double)hypot(xvel,yvel) < (ball[ball_test].diameter*2) )	return	TRUE;

					xvel=(ball[ball_test].x) - (ball[WHITE_BALL].x);
					yvel=(ball[ball_test].y) - (ball[WHITE_BALL].y);

					if	((double)hypot(xvel,yvel) < (ball[ball_test].diameter) )	return	TRUE;
				}
			}

	return	FALSE;
}
/* ------------------------------------- */
long	Get_Clear_Path_To_Pocket(long pocket)
{
	long	check_ball;
	long	ball_test;
	double	line_test,xvel,yvel,white_posx,white_posy,line_test1;
	double	speed,pckx,pcky,sx,sy,save_cangle;

/* test line of sight to see if any ball in path, if so then try new angle+
ball radius */

	save_cangle = computer_angle;
	
	pckx=pockets[pocket].x;
	pcky=pockets[pocket].y;

		check_ball = Select_Ball_To_Pot();

		sx=ball[WHITE_BALL].x;
		sy=ball[WHITE_BALL].y;

/* setup angle from pocket to object ball 1 */

		ball[WHITE_BALL].x=pckx;
		ball[WHITE_BALL].y=pcky;

		aim_done=NO;
		while	(aim_done!=YES)	{
	 	Aim_At_Next_Ball();
 if (keys[kA]==128) break;
		pan_angle=track_pan;
		}
		computer_angle=pan_angle;

		//set_line(((((computer_angle)-90.0))*2*PI/360));

		xvel=ball[check_ball].x-ball[WHITE_BALL].x;
		yvel=ball[check_ball].y-ball[WHITE_BALL].y;

		computer_angle=save_cangle  ;

		speed=(hypot(xvel,yvel))*2;

		line_test1=0;
		for	(line_test=0; line_test < speed; line_test+=.1)	{

				white_posx = ball[WHITE_BALL].x+(line_test1*ball[WHITE_BALL].vx1);
				white_posy = ball[WHITE_BALL].y+(line_test1*ball[WHITE_BALL].vy1);
				line_test1++;

				for	(ball_test=1; ball_test < LAST_BALL; ball_test++)	{
			
					xvel=(ball[ball_test].x) - (white_posx);	
					yvel=(ball[ball_test].y) - (white_posy);	

					
					if	(hypot(xvel,yvel) < ball[WHITE_BALL].diameter)	{

						if	(check_ball != ball_test )	{
/* reset cue ball position */
						ball[WHITE_BALL].x=sx;
						ball[WHITE_BALL].y=sy;
						return	TRUE;

						}
						else	{
						ball[WHITE_BALL].x=sx;
						ball[WHITE_BALL].y=sy;
						return	FALSE;
						}

					}
				}
			}

	ball[WHITE_BALL].x=sx;
	ball[WHITE_BALL].y=sy;

	return	FALSE;
}
/* ------------------------------------- */
long	Look_line_to_Object_ball_2(void)
{
	long	check_ball;
	long	ball_test;
	double	line_test,xvel,yvel,white_posx,white_posy,line_test1;
	double	speed,line_angle;

/* test line of sight to see if any ball in path, if so then try new angle+
ball radius */

		
		check_ball = Select_Ball_To_Pot();

		xvel=ball[check_ball].x-ball[WHITE_BALL].x;
		yvel=ball[check_ball].y-ball[WHITE_BALL].y;

		if	( (line_angle=((double)atan2((double)yvel,(double)xvel)/deg2rad)) < 0) line_angle+=360;

		speed=(hypot(xvel,yvel))*2;

		line_test1=0;
		for	(line_test=0; line_test < speed; line_test+=.1)	{

				white_posx = ball[WHITE_BALL].x+(cos(line_angle*deg2rad)*line_test1);
				white_posy = ball[WHITE_BALL].y+(sin(line_angle*deg2rad)*line_test1);
				line_test1+=.1;
	/*
				save_x=ball[WHITE_BALL].x;
				save_y=ball[WHITE_BALL].y;
				ball[WHITE_BALL].x=white_posx;
				ball[WHITE_BALL].y=white_posy;
				Handle3D();
				ball[WHITE_BALL].x=save_x;
				ball[WHITE_BALL].y=save_y;
	  */
				for	(ball_test=1; ball_test < LAST_BALL; ball_test++)	{
			
					xvel=(ball[ball_test].x) - (white_posx);	
					yvel=(ball[ball_test].y) - (white_posy);	
					
					if	(hypot(xvel,yvel) <= ball[WHITE_BALL].diameter)	{

						if	(check_ball != ball_test )		return	TRUE;
						else	return	FALSE;


					}
				}
			}


	return	FALSE;
}
/*--------------------------*/
/* get next angle to check against */
void	add_angle_for_next_shot(long	angle_to_add)
{
		alt+=1;
		alt%=2;

		if	(alt==0)	{
		alt_angle+=angle_to_add;
		if	(alt_angle >= 360<<PAN_FRAC) alt_angle-=360<<PAN_FRAC;
		computer_angle=alt_angle;
		}
		else	{
				alt_angle1-=angle_to_add;
				if	( alt_angle1/256 < 0) alt_angle1+=360<<PAN_FRAC;
				computer_angle=alt_angle1;
		}
}
/*--------------------------*/
void	add_angle_for_next_shot_checked(long	angle_to_add)
{
		alt+=1;
		alt%=2;

	/* check top left with cue ball at bottom left of screen */

		if	(alt==0)	{
		alt_angle+=angle_to_add;
		if	(alt_angle >= 360<<PAN_FRAC) alt_angle-=360<<PAN_FRAC;
		computer_angle=alt_angle;
		}
		else	{
				alt_angle1-=angle_to_add;
				if	( alt_angle1/256 < 0) alt_angle1+=360<<PAN_FRAC;
				computer_angle=alt_angle1;
		}
}
#endif	// RESTORE: unused AI functions
/*--------------------------*/
void	reset_shot(void)
{
 long l;

		reset_fouls();
		LOWEST_BALL_HIT_FIRST_FLAG=NO;
		ball_in_hand_flag=NO;
/* reset checking flag for any object ball hitting cushion */
		cushion_hit=NO;
/* reset flag for checking any object ball potted */
		white_hit_cushion=NO;
		first_hit = NO;
      for (l=0;l<LAST_BALL;l++)
       ball[l] = saveball[l];     
      nine_ball_count = save_nine_ball_count;
}

#if 0
// RESTORE: nothing in the game calls the functions from here to the #endif
// (the 1996 source's AI uses get_best_shot, find_best_pocket and take_shot
// only).  Their maths takes ball positions as BALL.C's doubles, which are
// 8.24 integers now (FIXBALL.C), so they are left out rather than kept with
// wrong units.
long	get_ball_angle( double xvel, double yvel)
{
	long		new_angle;
 	if	( (new_angle=(long)((atan2((double)yvel,(double)xvel))/deg2rad)) < 0 ) new_angle+=360;
 	return(new_angle);
}
#endif	// RESTORE: unused AI functions
//---------------------------------------------------------------
long	Select_Ball_To_Pot(void)
{
	long	c;

	c=1;

	//--------------------------------

	while	(ball[c].num != nine_ball_count)	
    {
	  c++;
	 }

	//--------------------------------

	if	(nine_ball_count==LAST_BALL-1)
    return	c;

	//--------------------------------

	c=1;
	while	(1)	
    {
     while	(ball[c].num != nine_ball_count)	{
     c++;
    }
     if	(ball[c].active==YES) break;
     c=1;
	  nine_ball_count++;
	 }

	return	c;
}
#if 0
// RESTORE: nothing in the game calls the functions from here to the #endif
// (the 1996 source's AI uses get_best_shot, find_best_pocket and take_shot
// only).  Their maths takes ball positions as BALL.C's doubles, which are
// 8.24 integers now (FIXBALL.C), so they are left out rather than kept with
// wrong units.
/* ----------------------------------------------------------------------- */
long	Aim_Cue_To_Current_Ball(void)
{
	double	x1,y1,x2,y2;
	long	check_ball,angle;
	//--------------------------------

	x1 = ball[WHITE_BALL].x;	//get white balls positions
	y1 = ball[WHITE_BALL].y;
	
	check_ball = Select_Ball_To_Pot();

	//-----------------------
	//got ball to pocket ..//
	//-----------------------

	x2 = ball[check_ball].x;
	y2 = ball[check_ball].y;
	
	//--------------------------------

	x1 = x2-x1;	
	y1 = y2-y1;
	
	angle  = get_ball_angle(x1,y1);

	if	(angle < 0) angle+=360;
	if	(angle >= 360) angle-=360;
	
	//angle%=360;

	return	angle;

	//--------------------------------
}

void	Save_ball_data(void)
{
	long	l;
		for	(l=0;l< LAST_BALL;l++)	{
		    saveball[l]=ball[l];
			}
}
void	Reset_3dball(void)
{
	long	l;

		for	(l=0;l< LAST_BALL;l++) 
		{
	  	ball[l]=saveball[l];;
		}
}

/*--------------------------*/
void	take_shot2(void)
{
//		currentband=-1;

	 	LOWEST_BALL_HIT_FIRST_FLAG=NO;
	 	first_hit = NO;
		reset_fouls();
	 	cushion_hit=NO;

		hit_ball(0, 
					((computer_angle)-90.0)*2*PI/360,
					((float) butt)*2*PI/360,
					(double) speed_meter, x_hit_pos, y_hit_pos);

		while	( (any_balls_moving()==TRUE) ) 	{

 	 		update_all_balls(AI_BALL_UPDATE);
			
/* exit ball movement on these conditions */

			if	(ball[WHITE_BALL].moving == NO)	{
			if	(potted_hi_ball_flag != 0)	break;
			if	(potted_good_ball_flag != 0)	break;
			}
		}

}
/* ----------------------------------------------------------------------- */
/* when opponent fouls - move cue ball near first object ball for easy shot */
void	move_cue_ball_to_good_position(void)
{
	long		check_ball,ball_in_way;
	double	angle;
	double	x,y,newx,newy,dist;

	if	(break_flag == YES) return;

		check_ball = Select_Ball_To_Pot();
		angle =	Return_angle_to_closest_pocket(check_ball);

	 	if	( (angle +=180) >= 360) angle-=360;
		
		ball[WHITE_BALL].x = ball[check_ball].x + (cos(angle*deg2rad) * .20);
		ball[WHITE_BALL].y = ball[check_ball].y + (sin(angle*deg2rad) * .20);

/* check to see if clear from any other object balls */

		for	(check_ball=1;check_ball<LAST_BALL;check_ball++)	{
			x=ball[WHITE_BALL].x-ball[check_ball].x;
			y=ball[WHITE_BALL].y-ball[check_ball].y;

			if	( hypot(x,y) <= ball[check_ball].diameter)	{
			ball[WHITE_BALL].x=saveball[WHITE_BALL].x ;
			ball[WHITE_BALL].y=saveball[WHITE_BALL].y ;
			}

		}

/* check to see if another ball in line to object ball 1 */
		newx=ball[WHITE_BALL].x;
		newy=ball[WHITE_BALL].y;


		aim_done=NO;

		while	(aim_done!=YES)	{
		Aim_At_Next_Ball();
		pan_angle=track_pan;
		}
		computer_angle=pan_angle;


		ball_in_way=Get_good_angle_to_Object_ball_1_();

	 	if	( ball_in_way== TRUE )	{
		newx=saveball[WHITE_BALL].x;
		newy=saveball[WHITE_BALL].y;
		}

		ball[WHITE_BALL].x = saveball[WHITE_BALL].x;
		ball[WHITE_BALL].y = saveball[WHITE_BALL].y;

		x=newx-ball[WHITE_BALL].x;
		y=newy-ball[WHITE_BALL].y;

/* get opposite angle to object ball 1 */
		if	( (angle=atan2(y,x)/deg2rad) < 0) angle+=360;

/* get distance to object ball 1  - diameter of ball */
		dist = hypot(x,y)-ball[WHITE_BALL].radius;
	
/* move cue ball to objectnew position */

		while	( dist > 0) {
		if	(keys[kK]==128) break;

		ball[WHITE_BALL].x+=(double)cos(angle*deg2rad)*.1;
		ball[WHITE_BALL].y+=(double)sin(angle*deg2rad)*.1;

		if	(max_cush_x-(ball[WHITE_BALL].x-(ball[WHITE_BALL].radius+.010)) <0.0) ball[WHITE_BALL].x =max_cush_x+(ball[WHITE_BALL].radius+.010);
		if	(min_cush_x-(ball[WHITE_BALL].x+(ball[WHITE_BALL].radius+.010)) >0.0) ball[WHITE_BALL].x =min_cush_x-(ball[WHITE_BALL].radius+.010);
		if	(max_cush_y-(ball[WHITE_BALL].y-(ball[WHITE_BALL].radius+.010)) <0.0) ball[WHITE_BALL].y =max_cush_y+(ball[WHITE_BALL].radius+.010);
		if	(min_cush_y-(ball[WHITE_BALL].y+(ball[WHITE_BALL].radius+.010)) >0.0) ball[WHITE_BALL].y =min_cush_y-(ball[WHITE_BALL].radius+.010);

		//Handle3D();

		dist-=.1;

		}

		saveball[WHITE_BALL].x = ball[WHITE_BALL].x;
		saveball[WHITE_BALL].y = ball[WHITE_BALL].y;

/* rotate cue to face current ball */
		aim_done=NO;
		while	(aim_done!=YES)	{
		if	(keys[kK]==128) break;
		Aim_At_Next_Ball();
		pan_angle=track_pan;
		com_pan=track_pan;
		//Handle3D();
		}
		computer_angle=pan_angle;

}
void	Aim_At_Next_Ball(void)
{
	double	dist,angle,a;

	if	(GAME_TYPE != US_POOL) return;

/* if in linked mode then dont line up external player not playing */

	if	(LINK_OPTION!=NULL)	{

	if	( (Player_Active == 0) && (master==SLAVE) ) return;
	if	( (Player_Active == 1) && (master==MASTER) ) return;

	}

/* check to see if aimed at ball, so even if you move white ball it dose
 not re track it self every time */

	if	(aim_done == YES) return;

	if	(CAMERA_SET == 2 && show_shot_flag == NO)	{

	dist=Aim_Cue_To_Current_Ball();//PAN_FRAC;
	dist=dist;

	if	( (dist -=90 ) < 0 ) dist+=360;
		dist = (360) - dist;
		angle=dist;		//angle to check track_pan against
		dist -= track_pan;
		dist = ((dist*CAMERA_SPEED_LONG)/100)+1;

/* a is angle to next ball in order list to pot */

		if	( (a= track_pan - angle  ) < 0) a=-a;
						
		a=a;
		if	(a <= 1) aim_done=YES;

	if	(COMPUTER_TURN == YES) {

		if	( track_pan != angle)	{
		 track_pan+=dist;

		 if	(track_pan >= 360) track_pan-=360;
		 }

		else aim_done=YES;

		return;
		}
		if	( track_pan != angle)	{
			track_pan+=dist;
			pan+=dist;
			cue_pan+=dist;

			if	(pan >= 360) pan-=360;
			if	(cue_pan >= 360) cue_pan-=360;
			if	(track_pan >= 360) track_pan-=360;
			}
  	}
  	else	{
		 	track_pan=pan;
  	}
}
#endif	// RESTORE: unused AI functions
