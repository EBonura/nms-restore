//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
//								    	 QD-UP      										 */
//										PC-CD ROM											 */
//								 For BMG	Interactive										 */
//								 By NMS Software Ltd.									 */
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* 
	stuff to do!

	cushion spin effect.
	adjust cushion overshoot compensation.
	fix small cushion gaps in makecush.c
	speedups.
*/

#include	"globals.h"

#define		PIx 3.1415927 //15927

/* Matrix stuff */

/* Co-ord points for cushions */


/*

					Cush Diagram
							3
						   
					\------/ \-----/

				   \					/
				   |					|
				 0 |					| 1
				   |					|
				   /					\

					/------\ /-----\
							2
*/


void add_col_to_list(short ball_num, double x, double y)
{
	if (ball_col_list_pos>=BALL_COL_LIST_SIZE) return;

	ball_col[ball_col_list_pos].ball_num=ball_num;
	ball_col[ball_col_list_pos].x=x;
	ball_col[ball_col_list_pos].y=y;

	ball_col_list_pos++;

	return;
}
//----------------------------------------------------------------------------
void reset_col_list(void)
{
	ball_col_list_pos=0;
}

//----------------------------------------------------------------------------
void	make_sin_cos_table( void )
{
	float	a;
	long	l;
	a=0;
	for	(l=0; l < 360; l++)
		{
//      sin[l] =sin(a);
//		cos_tab[l] =cos(a*deg2rad);
		a++;
	}
}							   
//----------------------------------------------------------------------------
//long	get_angle( float xvel, float yvel)
//{
//	long		new_angle;
// 	if	( (new_angle=(long)((atan2((double)yvel,(double)xvel))/deg2rad)) < 0 ) new_angle+=360;
//	return(new_angle);
//}
//----------------------------------------------------------------------------
//long	get_angle1( long xvel, long yvel)
//{
//	long		new_angle;
// 	if	( (new_angle=(long)((atan2((double)yvel,(double)xvel))/deg2rad)) < 0 ) new_angle+=360;
// 	return(new_angle);
//}
//----------------------------------------------------------------------------
void	pocketed_ball(long ball_num,long pock,long ang)
{ 
	long	miss_respot=NO;
   if ( ball[ball_num].active==NO) return;
   if ( ball[ball_num].active==2) return;

   ball[ball_num].active=NO;

   add_col_to_list(ball_num, ball[ball_num].x, ball[ball_num].y);

   if ( activated_lines==YES) return;
   if ( calculating_computer_shot == YES ) return;

   potted_flag=YES;

/* save ball potted , in case we want to respot it */

	miss_respot=NO;

	if	( (GAME_TYPE==UK_POOL) && (break_flag==YES) && (ball[ball_num].num  != 8) )	
    {
	  miss_respot=YES;
    }

    if	((ball[ball_num].numstruct > WHITE_BALL) && (miss_respot==NO))	{
    potted_ball[pot_count] = ball[ball_num].numstruct;
    if	(pot_count==0)	saved_ball_count=nine_ball_count;
    pot_count++;
	}

		if	(ball_num == WHITEBALL)  
      {
		 pocket_num = 100;

/*check to see if ball was pockted on left
 hand side of the screen or right */

       if	(ball_ptr[ball[ball_num].numstruct] -> world_pos.x == 0)
		   {
/* if in middle then sample played through both speakers */
		    add_sample(POT_WHITE, 0);
         }
       else
			{

		 if	(ball_ptr[ball[ball_num].numstruct] -> world_pos.x < 0)
			{
			add_sample(POT_WHITE, LEFT_PAN);
		   }
		  else
			{
         add_sample(POT_WHITE, RIGHT_PAN);
         }

		 }
       return;
		}

/*check to see if ball was pockted on left
 hand side of the screen or right */

       if	(ball_ptr[ball[ball_num].numstruct] -> world_pos.x == 0)
		  {
/* if in middle then sample played through both speakers */
			add_sample(POT_BALL, 0);
        }
       else 
        {
         if	(ball_ptr[ball[ball_num].numstruct] -> world_pos.x < 0) {
		    add_sample(POT_BALL, LEFT_PAN);
		  }
       else	
        {
         add_sample(POT_BALL, RIGHT_PAN);
        }
		}

		if	(show_shot_flag==YES)   show_panel=PANEL_ON_TIME;

      if ( SNOOKER_MODE == YES )
       {
        snooker_pot_check(ball_num);
       }
      else
       {
		 switch	(GAME_TYPE)	
       {

/* setup number of ball in pack + white ball */

	  	case	UK_POOL:
			uk_pot_check(ball_num);			
			break;

		case	US_8BALL:
			us_8ball_check(ball_num);
			break;

		case	US_POOL:
			us_pot_check(ball_num);
			break;

		case	CONTINUOUS:
			continuous_check(ball_num,pock);
			break;
			
		case	SPEED_POOL:
			potted_good_ball_flag=YES;
			speed_pot++;
			if	(speed_pot == LAST_BALL-1) start_time=NO;
			break;

		case	KILLER_POOL:
	//		potted_good_ball_flag=YES;
			break;

	  	case	NMS_POOL:
			/* set which side all other same colours must go down */

			if	( players[0].ball_type == NULL)  
          {

   /* refresh whole of screen to show coloured discs */

          num_frames_camera_still = 0;

          if	(ball[ball_num].obj_type == YELLOW)	
           {
				/* setup pocket for balls to go down */

            if	( (pock == 1) || (pock == 2) || (pock == 3) )
             {
              nms_pockets[1] = YELLOW;
              nms_pockets[2] = YELLOW;
              nms_pockets[3] = YELLOW;

              nms_pockets[0] = RED;
              nms_pockets[5] = RED;
              nms_pockets[4] = RED;
             }
				else
             {
              nms_pockets[0] = YELLOW;
              nms_pockets[5] = YELLOW;
              nms_pockets[4] = YELLOW;

              nms_pockets[1] = RED;
              nms_pockets[2] = RED;
              nms_pockets[3] = RED;
             }
           }
          else
           {
			  /* setup pocket for balls to go down */

            if	( (pock == 1) || (pock == 2) || (pock == 3) )
             {
              nms_pockets[0] = YELLOW;
              nms_pockets[5] = YELLOW;
              nms_pockets[4] = YELLOW;

              nms_pockets[1] = RED;
              nms_pockets[2] = RED;
              nms_pockets[3] = RED;
             }
            else
             {
              nms_pockets[1] = YELLOW;
              nms_pockets[2] = YELLOW;
              nms_pockets[3] = YELLOW;

              nms_pockets[0] = RED;
              nms_pockets[5] = RED;
              nms_pockets[4] = RED;
             }
			  }
			}

			/* setup ball in pocket */
			nms_pot_check(ball_num,pock);
			break;
		}

		}

/* update player stats, Shots Taken varible */

		if	(potted_good_ball_flag==YES)	
       {
		  if	(Player_Active==0)	
         {
		    player_info[left_player].potting_ratio++;
	      }
	     else	
         {
          player_info[right_player].potting_ratio++;
		   }
		 }

   if	((potted_good_ball_flag==YES) && (white_hit_cushion > 1) )	add_sample(CLAPPING, 0);
		
}			 
//----------------------------------------------------------------------------
void	continuous_check(long ball_num, long pock)
{
}
//----------------------------------------------------------------------------
void	nms_pot_check(long ball_num, long pock)
{
	ballstruct *bi;
   bi=&ball[ball_num];

    if	( players[0].ball_type == NULL )
     {
//------------------------------------
      potted_good_ball_flag=YES;

      switch	( bi -> obj_type )	
       {
	     case	RED:
	  		players[Player_Active].ball_type = RED;
	  		players[1-Player_Active].ball_type = YELLOW;
	  		break;

        case	YELLOW:
	  		players[Player_Active].ball_type = YELLOW;
	  		players[1-Player_Active].ball_type = RED;
	  		break;

        case	BLACK:
	  		potted_black_wrong_flag=YES;
	  		break;
       }

      p1_stored_pot_type=players[0].ball_type;
      p2_stored_pot_type=players[1].ball_type;

      return;
     }
	 
    potted_good_ball_flag=YES;

  //------------------------------------
    if	( players[Player_Active].ball_type == bi -> obj_type )
     {
	/* check to see if ball gone down correct side */

      if ( nms_pockets[pock] != bi -> obj_type )
		 {
        if	( bi -> obj_type == RED    )  potted_red_wrong_flag=YES;
        if	( bi -> obj_type == YELLOW )  potted_yellow_wrong_flag=YES;
		 }
	  }
	 else
     {
      if	( bi -> obj_type == BLACK  )  potted_black_wrong_flag=YES;
      if	( bi -> obj_type == RED    )  potted_red_wrong_flag=YES;
      if	( bi -> obj_type == YELLOW )  potted_yellow_wrong_flag=YES;
	  }
}
//----------------------------------------------------------------------------
void	uk_pot_check(long ball_num)
{
	ballstruct *bi;
   bi=&ball[ball_num];

   if	( ( players[0].ball_type == NULL ) && ( players[1].ball_type == NULL ) )
    {
     potted_good_ball_flag=YES;

     switch	( bi -> obj_type )	
      {
       case	RED:
        players[Player_Active].ball_type   = RED;
        players[1-Player_Active].ball_type = YELLOW;
        break;

       case	YELLOW:
        players[Player_Active].ball_type   = YELLOW;
        players[1-Player_Active].ball_type = RED;
        break;

       case	BLACK:
          potted_black_wrong_flag=YES;
        break;
      }     
      p1_stored_pot_type=players[0].ball_type;
      p2_stored_pot_type=players[1].ball_type;
     return;
    }

   if ( players[Player_Active].ball_type == bi -> obj_type )
    {
     potted_good_ball_flag=YES;
    }
   else 
    {
   /* test free ball */

     if  ( UK_FREE_BALL == NO )  
      {
       if  ( ( Two_Shots > 1 ) && ( bi -> obj_type != BLACK ) && ( used_free_ball==NO) ) 
        {
         potted_good_ball_flag=YES;
         return;
        }
		}

     if  ( bi -> obj_type == BLACK  ) potted_black_wrong_flag=YES;
     if  ( bi -> obj_type == RED    ) potted_red_wrong_flag=YES;
     if  ( bi -> obj_type == YELLOW ) potted_yellow_wrong_flag=YES;
    }
}
//----------------------------------------------------------------------------
void	us_8ball_check(long ball_num)
{
/* ignore any balls potted on break */

	ballstruct *bi;
   bi=&ball[ball_num];

	if	(bi -> obj_type == BLACK)	
    {
     if ( players[Player_Active].ball_type != bi -> obj_type )
     potted_red_wrong_flag=YES;
	 }

//   if	( break_flag == YES ) return;

   if	( players[0].ball_type == NULL )
    {
     potted_good_ball_flag=YES;
     switch	( bi -> obj_type )	
      {
       case	YELLOW:
        players[Player_Active].ball_type   = YELLOW;
        players[1-Player_Active].ball_type = RED;
        break;

       case	RED:
        players[Player_Active].ball_type   = RED;
        players[1-Player_Active].ball_type = YELLOW;
        break;

       case	BLACK:
        potted_black_wrong_flag=YES;
        break;
      }

      p1_stored_pot_type=players[0].ball_type;
      p2_stored_pot_type=players[1].ball_type;

     return;
    }

   if	( players[Player_Active].ball_type == bi -> obj_type )
    {
     potted_good_ball_flag=YES;
    }
   else 
    {
    if  ( bi -> obj_type == BLACK  )  potted_black_wrong_flag=YES;
    if  ( bi -> obj_type == RED    )  potted_red_wrong_flag=YES;
    if  ( bi -> obj_type == YELLOW )  potted_yellow_wrong_flag=YES;
   }
}
//----------------------------------------------------------------------------
void	us_pot_check(long ball_num)
{
		long	c,amount;

/* show panel with new scores */

		if	( (ball[ball_num].num == nine_ball_count) || (LOWEST_BALL_HIT_FIRST_FLAG==YES) )
       {
		  potted_good_ball_flag=YES;
        draw_twice=2;

/* add scores */
		  amount=50;

		  if	(ball[ball_num].num != nine_ball_count )
			{
		  potted_hi_ball_flag=YES;

/* multyply potted ball by number of points */ 

			amount=amount*ball[ball_num].num;

/* check to see if potted nine ball , if so then a special bonus is awarded */

			if	(ball[ball_num].num == 9) amount=500;
			
			if	(show_shot_flag==YES)
          {
           Add_Score(amount);
          }
			return;
			}

/* check to see if potted nine ball , if so then a special bonus is awarded */

			if	(ball[ball_num].num == 9) amount=500;
				  
			if	(show_shot_flag==YES) Add_Score(amount);

			if	(nine_ball_count == LAST_BALL-1) return;

			c=1;
         while	(1)	
          {
			  while	(ball[c].num != nine_ball_count)	
            {
			    c++;
			   }

			  if	(ball[c].active==YES) break;

			  c=1;
			  nine_ball_count++;
			 }
			  }
			 else 
			  {
            lowest_ball_not_hit_first=YES;
            return;
			  }

}
//----------------------------------------------------------------------------
void	snooker_pot_check(long ball_num)
{
		long	amount,count,count1;

/* show panel with new scores */

		if	(show_shot_flag==NO) return;

      potted_good_ball_flag=YES;

      count=1;
      while ( ball[count].active==NO)
       {
	     count++;
       }

      if ( count < 16)
      {
      if ( SNOOKER_SHOOT_MODE == POT_RED_BALL )
      {

	    if	(ball[ball_num].ball_colour  != RED)
        {
         potted_yellow_wrong_flag=YES;
         potted_good_ball_flag=NO;

/* potted coloured ball so award other player points */

         switch ( ball[ball_num].ball_colour )
          {
		     case YELLOW:
           amount=4;
   	     break;
    		  case BROWN:
           amount=4;
	        break;
	    	  case GREEN:
           amount=4;
	        break;
		     case BLUEBALL:
           amount=5;
	        break;
	    	  case PURPLE:
           amount=6;
	        break;
		     case BLACK:
           amount=7;
	        break;
          }
         if ( hit_red_ball_flag==NO)
          {
           Add_Score_2_other_player( amount );
          }
         return;
		  }

/* else if potted red correctly then add 1 point to your score */

        Add_Score1( 1 );
        

		}
     else
      {  

/* check potted coloured ball correctly */

      switch ( ball[ball_num].ball_colour )
		 {
/* if potted red ball instead of coloured ball then award other player 4
 points */

		  case RED:
         potted_yellow_wrong_flag=YES;
         Add_Score_2_other_player( 4 );
         return;

/* potted correct ball award yourself the relevant points */

		  case YELLOW:
         amount=2;
	      break;
		  case BROWN:
         amount=3;
	      break;
		  case GREEN:
         amount=4;
	      break;
		  case BLUEBALL:
         amount=5;
	      break;
		  case PURPLE:
         amount=6;
	      break;
		  case BLACK:
         amount=7;
	      break;
       }
      Add_Score1(amount);
     }
     }

    else
     {
/* potted all reds now pot colours in correct order */

      if ( ball[ball_num].num == count-1 )
       {
        switch ( ball[ball_num].ball_colour )
		   {
           case YELLOW:
           amount=2;
	        break;
	    	  case BROWN:
     	     amount=3;
	        break;
	   		  case GREEN:
     	     amount=4;
	        break;
	   		  case BLUEBALL:
     	     amount=5;
	        break;
	   		  case PURPLE:
     	     amount=6;
	        break;
	   		  case BLACK:
     	     amount=7;
	        break;
         }
        Add_Score1(amount);
	    }
      else
       {
         potted_yellow_wrong_flag=YES;
         potted_good_ball_flag=NO;
         switch ( ball[ball_num].ball_colour )
          {
		     case YELLOW:
           amount=4;
   	     break;
    		  case BROWN:
           amount=4;
	        break;
	    	  case GREEN:
           amount=4;
	        break;
		     case BLUEBALL:
           amount=5;
	        break;
	    	  case PURPLE:
           amount=6;
	        break;
		     case BLACK:
           amount=7;
	        break;
          }
         if ( hit_red_ball_flag==NO)
          {
           Add_Score_2_other_player( amount );
          }
       }
    }
}
//----------------------------------------------------------------------------
const double q_factor=0.15;
const double r_factor=0.05;

#define Q_FACTOR   (0.2) 
#define R_FACTOR   (0.08)


#define Z_AXIS_SPIN_FRICTION	0.1

#define MIN_ANG_VEL_Z			0.005

#define MINIMUM_SPEED_SQUARED			0.0004	/* when velocity drops below this
											value, ball is considered not 
											moving */

#define NUM_FRIC_VALS			6

#define FRIC_SPEED_SCALE	10000.0		/* increased friction occurs when ball
											speed drops below 5 cm/s */

const double friction[NUM_FRIC_VALS][2]={	{Q_FACTOR*4, R_FACTOR*4},
								{Q_FACTOR*3, R_FACTOR*3},
								{Q_FACTOR*2.5, R_FACTOR*2.5},
								{Q_FACTOR*2, R_FACTOR*2},
								{Q_FACTOR*1.5, R_FACTOR*1.5},
								{Q_FACTOR, R_FACTOR}};

/* if avz>.2 increase friction */
#define NUM_FRICTION_VALS_Z		8
#define FRIC_SPEED_SCALE_Z		1

const double angle_z_friction[NUM_FRICTION_VALS_Z]={
										Z_AXIS_SPIN_FRICTION*9.5,
										Z_AXIS_SPIN_FRICTION*9,
										Z_AXIS_SPIN_FRICTION*7,
										Z_AXIS_SPIN_FRICTION*6,
										Z_AXIS_SPIN_FRICTION*5,
										Z_AXIS_SPIN_FRICTION*5,
										Z_AXIS_SPIN_FRICTION*5,
										Z_AXIS_SPIN_FRICTION*5,

										};
struct ball_dist_st {
		double x;
		double y;
		double hypot;
	};

static struct ball_dist_st ball_dist[22][22];	/* used for distances between
													balls */

#define MAX_HITS	22

struct ball_hits_st {
	short ball1;
	short ball2;
};

static struct ball_hits_st ball_hits[MAX_HITS];


void update_ball_movement(double time_step, int ball_num)
{
	double qx, qy;				// force acting at bottom of ball
	double rx, ry;				// force acting at point 7/10 above bed height
	double a_x, a_y;			// linear accelerations
	double alpha_x, alpha_y;	// angular accelerations
	double total_velocity;
	int fric_pos;
	double	xvel,yvel;
	static int check_angle_count=0;

  if ( ball[ball_num].active==2 )
    {
     ball[ball_num].z-=0.000005;
     ball[ball_num].moving=YES;

     if ( ball[ball_num].z < -(ball[ball_num].diameter)) 
     {
      ball[ball_num].active=NO;
      ball[ball_num].moving=NO;
	  }

     return;
    }


/* Don't bother doing anything if ball isn't moving */

	if ( (ball[ball_num].moving==FALSE) or (ball[ball_num].active==FALSE)
      or (ball[ball_num].active==2)  ) 
    {
     return;
    }

/* Store previous positions */

	ball[ball_num].x_prev=ball[ball_num].x;
	ball[ball_num].y_prev=ball[ball_num].y;
	ball[ball_num].z_prev=ball[ball_num].z;

/* Calc position in array for friction */
	
	total_velocity=(ball[ball_num].vx*ball[ball_num].vx)+(ball[ball_num].vy*ball[ball_num].vy);

	if (  total_velocity<MINIMUM_SPEED_SQUARED ) {

  	if (fabs(ball[ball_num].avz)<0.03) {
			ball[ball_num].moving=FALSE;
			ball[ball_num].vx=0.0;
			ball[ball_num].vy=0.0;
			ball[ball_num].vz=0.0;
			ball[ball_num].avx=0.0;
			ball[ball_num].avy=0.0;
			ball[ball_num].avz=0.0;
			return;
  		}
  	}

	fric_pos=total_velocity*FRIC_SPEED_SCALE;

	if (fric_pos>=NUM_FRIC_VALS) fric_pos=NUM_FRIC_VALS-1;

/* Calculate forces r & q */

	qx=-(friction[fric_pos][0]*
			(ball[ball_num].vx-(ball[ball_num].avx*ball[ball_num].radius)));

	qy=-(friction[fric_pos][0]*
			(ball[ball_num].vy-(ball[ball_num].avy*ball[ball_num].radius)));

	rx=-(friction[fric_pos][1]*ball[ball_num].vx);

	ry=-(friction[fric_pos][1]*ball[ball_num].vy);

/* Calculate linear and angular accelerations */

	a_x=(qx+rx)/ball[ball_num].mass;

	a_y=(qy+ry)/ball[ball_num].mass;
		 
	alpha_x=(ball[ball_num].radius*(2*rx-10*qx))/(10*ball[ball_num].mi);

	alpha_y=(ball[ball_num].radius*(2*ry-10*qy))/(10*ball[ball_num].mi);

/* Now alter velocities with acceleration */

	ball[ball_num].avx=(alpha_x*time_step)+ball[ball_num].avx;

	ball[ball_num].avy=(alpha_y*time_step)+ball[ball_num].avy;

	ball[ball_num].vx+=a_x*time_step;

	ball[ball_num].vy+=a_y*time_step;

/* Update positions */

	if	( (xvel=(time_step*ball[ball_num].vx) ) < 0 ) xvel=-xvel;
	if	( (yvel=(time_step*ball[ball_num].vy) ) < 0 ) yvel=-yvel;

	ball[ball_num].dist_x+=xvel;
	ball[ball_num].dist_y+=yvel;

/* x */

	ball[ball_num].x+=(time_step*ball[ball_num].vx);
	ball[ball_num].ax+=(time_step*ball[ball_num].avx);

/* y */

	ball[ball_num].y+=(time_step*ball[ball_num].vy);
	ball[ball_num].ay+=(time_step*ball[ball_num].avy);

/* z */

	fric_pos=fabs(ball[ball_num].avz)*FRIC_SPEED_SCALE_Z;
	if (fric_pos>=(NUM_FRICTION_VALS_Z)) {
		fric_pos=NUM_FRICTION_VALS_Z-1;
	}
	ball[ball_num].avz-=(time_step*angle_z_friction[fric_pos]*ball[ball_num].avz);
	ball[ball_num].az+=(time_step*ball[ball_num].avz);

/* only check this every 4th time */

	check_angle_count++;
	if (check_angle_count>=4) {

		while (ball[ball_num].ax>(PIx*2)) ball[ball_num].ax-=PIx*2;
		while (ball[ball_num].ay>(PIx*2)) ball[ball_num].ay-=PIx*2;
		while (ball[ball_num].az>(PIx*2)) ball[ball_num].az-=PIx*2;

		while (ball[ball_num].ax<0.0) ball[ball_num].ax+=PIx*2;
		while (ball[ball_num].ay<0.0) ball[ball_num].ay+=PIx*2;
		while (ball[ball_num].az<0.0) ball[ball_num].az+=PIx*2;
		check_angle_count=0;
	}

	return;
}

void hit_ball(	long ball_num,long x_hit, long y_hit,
				 	double ang_z, double ang_y, double vel)
{
	matrix4x1 velocity;
	matrix4x1 ang_vel;
	matrix4x4 ang_transform;
	long	a,count;
   double tmp_angle;

/* convert butt to radians */

   ang_y=(ang_y * PI) /2048;

//   tmp_angle =(int) (ang_z-1024)&4095;

   if ( (tmp_angle=ang_z-1024.0) <0 ) tmp_angle+=4096.0;
   ang_z=(double)(tmp_angle * (PI*2))/4096;

   for (count=0;count<LAST_BALL;count++) 
	 {
	  ball[count].vx=0;
	  ball[count].vy=0;
	  ball[count].avx=0;
	  ball[count].avy=0;
    }   

	velocity[0]=vel;
	velocity[1]=0.0;
	velocity[2]=0.0;
	velocity[3]=0.0;

	ang_vel[3]=0.0;

	for	(a=0;a<4;a++)	{
	 ang_transform[0][a]=0;
	 ang_transform[1][a]=0;
	 ang_transform[2][a]=0;
	 ang_transform[3][a]=0;
	}

	rotate2(ang_z, 0, ang_y, &ang_transform);

	multiply4x1( &ang_transform, &velocity, &velocity);

 	ball[ball_num].vx=velocity[0];
 	ball[ball_num].vy=-velocity[1];

	ang_vel[0]=((vel/ball[ball_num].radius)*(-y_hit/.4))/65536;
	ang_vel[1]=0;
	ang_vel[2]=((vel/ball[ball_num].radius)*(-x_hit/.2))/65536;

	rotate2(ang_z, ang_y , 0, &ang_transform);
	multiply4x1( &ang_transform, &ang_vel, &ang_vel);

	ball[ball_num].avx=ang_vel[0];
	ball[ball_num].avy=-ang_vel[1];
	ball[ball_num].avz=ang_vel[2];

/* Ball now moving */

	ball[ball_num].moving=TRUE;

	return;
}

	
void init_ball_position(int ball_num)
{
	ball[ball_num].vx=0;
	ball[ball_num].vy=0;
	ball[ball_num].vz=0;			 

	ball[ball_num].ax=0;
	ball[ball_num].ay=0;
	ball[ball_num].az=0;

	ball[ball_num].old_ax=0;
	ball[ball_num].old_ay=0;
	ball[ball_num].old_az=0;

	ball[ball_num].avx=0;
	ball[ball_num].avy=0;
	ball[ball_num].avz=0;

	ball[ball_num].moving=FALSE;
	ball[ball_num].active=TRUE;

/* All units in meters and kg */

   if ( SNOOKER_MODE == YES ) 
    {
      ball[ball_num].diameter=0.052500;		//	dia of USA pool ball
	 }
   else
    {
	switch	( TABLESIZE_TYPE ) 
    {
     case 0:
     ball[ball_num].diameter=0.057000;		//	dia of USA pool ball
    break;
     case 1:
	  ball[ball_num].diameter=0.050700;		//	dia of UK pool ball
    break;
     case 2:
     ball[ball_num].diameter=0.057000;		//	dia of USA pool ball
    break;
	 }
	}

	ball[ball_num].diameter_squared=ball[ball_num].diameter*ball[ball_num].diameter;
	ball[ball_num].radius=ball[ball_num].diameter/2;	//	rad of USA pool ball

	if (ball_num!=WHITE_BALL) {
		ball[ball_num].mass=ball_mass;			//	mass of USA pool ball
	} else {
		ball[ball_num].mass=0.115;			//	make white ball lighter
	}

	ball[ball_num].x=0;
	ball[ball_num].y=0;
	ball[ball_num].z=ball[ball_num].radius;
	ball[ball_num].x_prev=0;
	ball[ball_num].y_prev=0;
	ball[ball_num].z_prev=0;

/* Moment of inertia */
	ball[ball_num].mi=
		2*ball[ball_num].mass*(ball[ball_num].radius*ball[ball_num].radius)/5;

	return;
}
	
static const double	cush_loss=0.95;
static const double	cush_loss_perp=0.95;
static const double	ang_cush_loss=0.1;

/* these def's are for debugging */
//#define DONT_CHECK_X_CUSH
//#define DONT_CHECK_Y_CUSH

void check_cushions_float(int ball_num, double current_time_step)
{
/* Don't bother doing anything if ball isn't moving */

	if (ball[ball_num].moving==FALSE) return;
	if (ball[ball_num].active==FALSE) return;
	if (ball[ball_num].active==2) return;

//   test_camers_in_pocket( ball_num );

#ifndef	DONT_CHECK_X_CUSH
	check_cushions_x1(ball_num, current_time_step);
	check_cushions_x4(ball_num, current_time_step);
#endif

#ifndef	DONT_CHECK_Y_CUSH
	check_cushions_y2(ball_num, current_time_step);
	check_cushions_y3(ball_num, current_time_step);
#endif

	return;
}

void check_cushions_y2(int ball_num, double current_time_step)
{
	double cush_y, y_diff, extra_time_step;


	if ( (max_cush_y2-ball[ball_num].y)<0.0) {

		cush_y=get_cush_y(3,ball[ball_num].x,0.0);
//	  	if ( (cush_y==-10) or (cush_y==10)) 
      check_all_pockets_y2(ball_num,0);	

		if (((ball[ball_num].y>cush_y))) {

		/* need to calculate difference so that extra time step can be calculated */
			y_diff=cush_y-ball[ball_num].y;
			extra_time_step=y_diff/ball[ball_num].vy;
			
			ball[ball_num].y=cush_y;
			add_col_to_list(ball_num, ball[ball_num].x, ball[ball_num].y);

			cush_rebound_y(ball_num, get_cush_ang(3, ball[ball_num].x, 0.0), 3);
		/* update time lost by repositioning the ball */
			update_ball_movement(fabs(extra_time_step),ball_num);

			// mark's cushion checks 
		    ball[ball_num].hitcush++;  
			if	(  ball[ball_num].graphic_type == WHITEBALL) white_hit_cushion=YES;
			else	cushion_hit++;



		}

	}
}

void check_cushions_x1(int ball_num, double current_time_step)
{
	double cush_x,x_diff, extra_time_step;


	if ((min_cush_x1-ball[ball_num].x)>0.0) {

		cush_x=get_cush_x(1,0.0,ball[ball_num].y);

	  	if ((cush_x==-10) or (cush_x==10))      check_all_pockets_x1(ball_num,1);	

		if ((ball[ball_num].x<cush_x)) {


			x_diff=ball[ball_num].x-cush_x;
			extra_time_step=x_diff/ball[ball_num].vx;

			ball[ball_num].x=cush_x;
			add_col_to_list(ball_num, ball[ball_num].x, ball[ball_num].y);

			cush_rebound_x(ball_num, get_cush_ang(1,0.0,ball[ball_num].y),1);
			update_ball_movement(fabs(extra_time_step),ball_num);

			// mark's cushion checks
		    ball[ball_num].hitcush++;  
			if	(  ball[ball_num].graphic_type == WHITEBALL) white_hit_cushion=YES;
			else	cushion_hit++;
		}
	}
}

void check_cushions_x4(int ball_num, double current_time_step)
{
	double cush_x, x_diff,extra_time_step;


	if ((max_cush_x4-ball[ball_num].x)<0.0) {


		cush_x=get_cush_x(4,0.0,ball[ball_num].y);
	   if ((cush_x==-10) or (cush_x==10))       check_all_pockets_x4(ball_num,3);	

		if ((ball[ball_num].x>cush_x)) {


			x_diff=ball[ball_num].x-cush_x;
			extra_time_step=x_diff/ball[ball_num].vx;

			ball[ball_num].x=cush_x;

			add_col_to_list(ball_num, ball[ball_num].x, ball[ball_num].y);

			cush_rebound_x(ball_num, get_cush_ang(4,0.0,ball[ball_num].y),4);
			update_ball_movement(fabs(extra_time_step),ball_num);

			// mark's cushion checks
		    ball[ball_num].hitcush++;  
			if	( ball[ball_num].graphic_type == WHITEBALL) white_hit_cushion=YES;
			else	cushion_hit++;
		}
	}
}
void check_cushions_y3(int ball_num, double current_time_step)
{
	double cush_y,y_diff,extra_time_step;

	if ((min_cush_y3-ball[ball_num].y)>0.0) {

      cush_y=get_cush_y(2,ball[ball_num].x,0.0);

//		if ( (cush_y==-10) or (cush_y==10))
   	check_all_pockets_y3(ball_num,2);	


		if (((ball[ball_num].y<cush_y))) {


		/* need to calculate difference so that extra time step can be calculated */
			y_diff=ball[ball_num].y-cush_y;
			extra_time_step=y_diff/ball[ball_num].vy;
			
			ball[ball_num].y=cush_y;
			add_col_to_list(ball_num, ball[ball_num].x, ball[ball_num].y);

			cush_rebound_y(ball_num, get_cush_ang(2, ball[ball_num].x, 0.0),2);

		/* update time lost by repositioning the ball */
			update_ball_movement(fabs(extra_time_step),ball_num);

			// mark's cushion checks
		    ball[ball_num].hitcush++;  
			if	(  ball[ball_num].graphic_type == WHITEBALL) white_hit_cushion=YES;
			else	cushion_hit++;
		}
	}
}

/* stop crashes when debugging */
#define BALL_DEBUG

double get_cush_ang(int bump_num, double x, double y)
{
	int  array_pos;
	double cush_ang;

	switch (bump_num) {
		case 1:
			array_pos=y/bumper_scale;
			array_pos+=NUM_CUSH_POINTS_Y/2;
		#ifdef	BALL_DEBUG
			if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_Y)) {	
				array_pos=0;
			}
		#endif		
			cush_ang=cushions_y1[array_pos].cush_ang;
			break;
		case 2:
			array_pos=x/bumper_scale;
			array_pos+=NUM_CUSH_POINTS_X/2;
		#ifdef	BALL_DEBUG
			if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_X)) {	
				array_pos=0;
			}
		#endif		
			cush_ang=cushions_x2[array_pos].cush_ang;
			break;
		case 3:
			array_pos=x/bumper_scale;
			array_pos+=NUM_CUSH_POINTS_X/2;
		#ifdef	BALL_DEBUG
			if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_X)) {	
				array_pos=0;
			}
		#endif		
			cush_ang=cushions_x3[array_pos].cush_ang;
			break;

		case 4:
			array_pos=y/bumper_scale;
			array_pos+=NUM_CUSH_POINTS_Y/2;
		#ifdef	BALL_DEBUG
			if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_Y)) {	
				array_pos=0;
			}
		#endif		
			cush_ang=cushions_y4[array_pos].cush_ang;
			break;
	}

	return cush_ang;
}

double get_cush_x(int bump_num, double x, double y)
{
	int  array_pos;
	double intp_x;

	switch (bump_num) {
		case 1:
			array_pos=y/bumper_scale;
			array_pos+=NUM_CUSH_POINTS_Y/2;
		#ifdef	BALL_DEBUG
			if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_Y)) {	
				array_pos=0;
			}
		#endif		
			intp_x=cushions_y1[array_pos].x;
			return intp_x;
			break;

		case 4:
			array_pos=y/bumper_scale;
			array_pos+=NUM_CUSH_POINTS_Y/2;
		#ifdef	BALL_DEBUG
			if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_Y)) {	
				array_pos=0;
			}
		#endif		
			intp_x=cushions_y4[array_pos].x;
			return intp_x;
			break;
	}

	return 10;		
}

double get_cush_y(int bump_num, double x, double y)
{
	int  array_pos;
	double intp_y;


	switch (bump_num) {
		case 2:
			array_pos=x/bumper_scale;
			array_pos+=NUM_CUSH_POINTS_X/2;
		#ifdef	BALL_DEBUG
			if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_X)) {	
				array_pos=0;
			}
		#endif		
			intp_y=cushions_x2[array_pos].y;
			return intp_y;
			break;

		case 3:
			array_pos=x/bumper_scale;
			array_pos+=NUM_CUSH_POINTS_X/2;
		#ifdef	BALL_DEBUG
			if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_X)) {	
				array_pos=0;
			}
		#endif		
			intp_y=cushions_x3[array_pos].y;
			return intp_y;
			break;
	}

	return 10;
}



#define CUSH_SPIN_SCALE		0.005

void cush_rebound_x(int ball_num, double angle, int cush_num)
{
	double temp_vx, temp_vy;
	double spin_ang, ang_vel, spin_vx, spin_vy;
	double cush_spin_scale=CUSH_SPIN_SCALE;
//	double total_vel;

/* convert angle to radians! arrgh! */
/* super bodge */
	angle=-angle;
	angle/=180;
	angle*=PI;

/* calculate spin transfer to velocity */

	if (cush_num==4) {
		cush_spin_scale=-cush_spin_scale;
	}

//	total_vel=sqrt((ball[ball_num].vx*ball[ball_num].vx)+(ball[ball_num].vy*ball[ball_num].vy));
		temp_vx=-((ball[ball_num].vx*cos(angle))+(ball[ball_num].vy*sin(angle)));
		temp_vy=(-(ball[ball_num].vx*sin(angle))+(ball[ball_num].vy*cos(angle)));


//	total_vel=sqrt((temp_vx*temp_vx)+(temp_vy*temp_vy));
		ball[ball_num].vx=(temp_vx*cos(-angle))+(temp_vy*sin(-angle));
		ball[ball_num].vy=(-temp_vx*sin(-angle))+(temp_vy*cos(-angle));
//	total_vel=sqrt((ball[ball_num].vx*ball[ball_num].vx)+(ball[ball_num].vy*ball[ball_num].vy));

	ang_vel=ball[ball_num].avz*cush_spin_scale;
	spin_ang=angle+PI/2;
	spin_vx=ang_vel*cos(spin_ang);
	spin_vy=ang_vel*sin(spin_ang);
	
	ball[ball_num].vx=ball[ball_num].vx*cush_loss;
	ball[ball_num].vy=ball[ball_num].vy*cush_loss_perp;
	ball[ball_num].avx=ball[ball_num].avx*ang_cush_loss;
	ball[ball_num].avy=ball[ball_num].avy*ang_cush_loss;
	ball[ball_num].avz=0;

	ball[ball_num].vx+=spin_vx;
	ball[ball_num].vy+=spin_vy;

	ball[ball_num].dist_x+=ang_cush_loss*5;
	
}

void cush_rebound_y(int ball_num, double angle, int cush_num)
{
	double temp_vx, temp_vy;
	double spin_ang, ang_vel, spin_vx, spin_vy;
	double cush_spin_scale=CUSH_SPIN_SCALE;
//	double total_vel;

/* need to subtract 90 degrees due to reflection */
	angle-=90;
/* convert angle to radians! arrgh! */
	angle/=180;
	angle*=PI;

/* calculate spin transfer to velocity */
	if (cush_num==2) {
		cush_spin_scale=-cush_spin_scale;
	}
	ang_vel=ball[ball_num].avz*cush_spin_scale;

	spin_ang=angle+PI/2;
	spin_vx=ang_vel*cos(spin_ang);
	spin_vy=ang_vel*sin(spin_ang);
	
//	total_vel=sqrt((ball[ball_num].vx*ball[ball_num].vx)+(ball[ball_num].vy*ball[ball_num].vy));
		temp_vx=((ball[ball_num].vx*cos(angle))+(ball[ball_num].vy*sin(angle)));
		temp_vy=-(-(ball[ball_num].vx*sin(angle))+(ball[ball_num].vy*cos(angle)));

//	total_vel=sqrt((temp_vx*temp_vx)+(temp_vy*temp_vy));
		ball[ball_num].vx=(temp_vx*cos(-angle))+(temp_vy*sin(-angle));
		ball[ball_num].vy=(-temp_vx*sin(-angle))+(temp_vy*cos(-angle));
//	total_vel=sqrt((ball[ball_num].vx*ball[ball_num].vx)+(ball[ball_num].vy*ball[ball_num].vy));
	ball[ball_num].dist_y+=ang_cush_loss*5;

	ball[ball_num].vx=ball[ball_num].vx*cush_loss;
	ball[ball_num].vy=ball[ball_num].vy*cush_loss_perp;
	ball[ball_num].avx=ball[ball_num].avx*ang_cush_loss;
	ball[ball_num].avy=ball[ball_num].avy*ang_cush_loss;
	ball[ball_num].avz=0;

	ball[ball_num].vx+=spin_vx;
	ball[ball_num].vy+=spin_vy;

}

double get_ball_grad(int ball_num)
{
	double grad;

	if (ball[ball_num].x_prev==0.0) return 0.0;
	if (ball[ball_num].y_prev==0.0) return 0.0;

	if ((ball[ball_num].y_prev-ball[ball_num].y)==0.0) return 0.0;

	grad=(ball[ball_num].y_prev-ball[ball_num].y)
			/(ball[ball_num].x_prev-ball[ball_num].x);

	return grad;
}

double get_ball_intercept(int ball_num, double grad)
{
	double c;

	c= ball[ball_num].y - grad*ball[ball_num].x;

	return c;
}


double find_nearest_x_dist(int ball_num)
{
	double nearest_dist=1e-40;

	/* only check against cushions for now */

	nearest_dist=fabs(max_cush_x4-ball[ball_num].x);

	if (nearest_dist>fabs(min_cush_x1-ball[ball_num].x))
		nearest_dist=fabs(min_cush_x1-ball[ball_num].x);

	return nearest_dist;
}

double find_nearest_y_dist(int ball_num)
{
	double nearest_dist=-1e40;

	/* only check against cushions for now */

	nearest_dist=fabs(max_cush_y2-ball[ball_num].y);
	if (nearest_dist>fabs(min_cush_y3-ball[ball_num].y))
		nearest_dist=fabs(min_cush_y3-ball[ball_num].y);

	return nearest_dist;
}

#define BODGE_FACTOR	80

void update_all_balls(double time_step)
{

double xd,yd;
	int loop, collision_loop;
	double time_elapsed=0.0, current_time_step;
	static double prev_time_step;

   reset_col_list();

/* make ball bals moving for test */

	while ((time_elapsed+0.00000001)<time_step) {

		current_time_step=get_max_time_step(time_step);

/* do ball collisions, if ball is hit, go to previous positions */

		if (ball_hits_pos!=0) {
			for  (loop=0; loop<LAST_BALL; loop++) {
				ball[loop].x=ball[loop].x_prev;
				ball[loop].y=ball[loop].y_prev;
			}
	  		time_elapsed-=prev_time_step;
		}


    
  		for (collision_loop=0; collision_loop<ball_hits_pos; collision_loop++) {
			ball_collision2(ball_hits[collision_loop].ball1, ball_hits[collision_loop].ball2);
		}


/* update balls and check cushions */	
		for (loop=0; loop<LAST_BALL; loop++) {
//			bgcol(63,63,63);
			update_ball_movement(current_time_step,loop);
  //			bgcol(0,0,0);

			check_cushions_float(loop,current_time_step);

		}
		time_elapsed+=current_time_step;
		prev_time_step=current_time_step;

	}
//	#ifndef	FINAL
//	pollhost();
//	#endif
	DrawScreen();
   if (end_of_frame_flag==YES ) return;

/*--------------------------*/
/* draw line from old position */

   if (activated_lines==TRUE) 
    {
     for (loop=0;loop<LAST_BALL;loop++)
      {

       if ( (loop == WHITE_BALL) && (butt > 1) && (x_hit_pos!=0) )
        {
         if ( new_count == 0 )
          {
          }

         xd=ball[loop].x-ball[loop].x_prev;
         if ( xd < 0)     xd=ball[loop].x_prev-ball[loop].x;
         yd=ball[loop].y-ball[loop].y_prev;
         if ( yd < 0)     yd=ball[loop].y_prev-ball[loop].y;

			if ( xd+yd > 0.005)
          {
           get_new_screen_pos(loop);
          }
        }
       else {
       get_new_screen_pos(loop);
       }
      }
     return;
	 }

#define	mile_frac	.001

	if	(ball[0].moving==YES)	
	{
		if	(Player_Active==0)	
		{
			player_info[left_player].ball_mileage+=mile_frac;
		}
		else	
		{
			player_info[right_player].ball_mileage+=mile_frac;
		}
	}

}

double get_max_time_step(double time_step)
{
	double max_time_step;
	double min_time_step;
//	int loop;
	int loop1, loop2;
	double min_ball_dist=1e99;
	double min_dist_ball_vel, new_time_step;
	int min_dist_ball_num=0;

	max_time_step=time_step/2;

//	min_time_step=time_step/512;

	min_time_step=time_step/4;

/* Clear array */

	for (loop1=0; loop1<LAST_BALL; loop1++) {
		for (loop2=0; loop2<LAST_BALL; loop2++) {
//			ball_dist[loop1][loop2].x=0.0;
//			ball_dist[loop1][loop2].y=0.0;
			ball_dist[loop1][loop2].hypot=0.0;
  		}
  	}

	ball_hits_pos=0;	// reset hit list

/* find smallest distance between 2 balls */


	for (loop1=0; loop1<LAST_BALL; loop1++) {

/* because we are copying values into 2 positons in array, we don't need to
	go through entire array */

		 if ( (ball[loop1].moving) && (ball[loop1].active) ) 
        for (loop2=0; loop2<LAST_BALL; loop2++) {



		 	if ((loop1!=loop2)&&(ball_dist[loop1][loop2].hypot==0.0)&&(ball[loop2].active)) {

		 		ball_dist[loop1][loop2].x=ball[loop2].x-ball[loop1].x;
		 		ball_dist[loop1][loop2].y=ball[loop2].y-ball[loop1].y;
		 		ball_dist[loop1][loop2].hypot=
   		 			ball_dist[loop1][loop2].x*ball_dist[loop1][loop2].x+ball_dist[loop1][loop2].y*ball_dist[loop1][loop2].y;

/* can fill in another part of array with values */
		 		ball_dist[loop2][loop1].x=	   ball_dist[loop1][loop2].x;
		 		ball_dist[loop2][loop1].y=	   ball_dist[loop1][loop2].y;
		 		ball_dist[loop2][loop1].hypot= ball_dist[loop1][loop2].hypot;
/* Check to see if any balls have hit each other */
				if (ball_dist[loop1][loop2].hypot<min_ball_dist) {
					min_ball_dist=ball_dist[loop1][loop2].hypot;
					min_dist_ball_num=loop1;
				}

				if (ball_dist[loop1][loop2].hypot<ball[loop1].diameter_squared) {

/* ball has hit another ball, add to list */

					ball_hits[ball_hits_pos].ball1=loop1;
					ball_hits[ball_hits_pos].ball2=loop2;
					ball_hits_pos++;
/* This is to stop the list at end of array when debugging */
					if (ball_hits_pos==LAST_BALL) ball_hits_pos--;
				//	if (ball_hits_pos==MAX_HITS) ball_hits_pos--;
				}
		 	}
		 }
	}

/* 
	temporary bodge
	calculate new step based on dist/(ball velocity)
*/
	min_dist_ball_vel=sqrt(ball[min_dist_ball_num].vx*ball[min_dist_ball_num].vx+
		ball[min_dist_ball_num].vy*ball[min_dist_ball_num].vy);

	new_time_step=fabs(sqrt(min_ball_dist)-ball[min_dist_ball_num].diameter)/(min_dist_ball_vel*2);

	while ((new_time_step<max_time_step)&&(max_time_step>min_time_step)) {
		max_time_step/=2;
	}

	return max_time_step;
}

void ball_collision2(short ball1, short ball2)
{
	double collision_ang;
	double ball1_ang, z_spin;
	const double z_spin_mul=2.0;
	const double elasticity=0.90;
	double vel_ball1, vel_ball2;
	double perp_vel_ball1, perp_vel_ball2;
	double new_vel_ball1, new_vel_ball2;
	double momentum_ball1, momentum_ball2;
	double initial_vel_diff, total_initial_momentum;
	int	vol;
   long ball_num;

   if ( ball[ball1].active == 2 ) return;
   if ( ball[ball2].active == 2 ) return;
   if ( ball[ball1].active == FALSE ) return;
   if ( ball[ball2].active == FALSE ) return;

/* calculate angle of collsion between 2 balls */
	collision_ang=atan2(ball[ball2].x-ball[ball1].x, ball[ball2].y-ball[ball1].y);

/* Calculate ang of dirn of ball1 */
	ball1_ang=atan2(ball[ball1].vx, ball[ball1].vy);

/* get ball velocities along this angle */
	vel_ball1=ball[ball1].vx*sin(collision_ang)+ball[ball1].vy*cos(collision_ang);
	vel_ball2=ball[ball2].vx*sin(collision_ang)+ball[ball2].vy*cos(collision_ang);
	initial_vel_diff=vel_ball2-vel_ball1;

/* get ball velocities at perpendicular */
	perp_vel_ball1=-ball[ball1].vy*sin(collision_ang)+ball[ball1].vx*cos(collision_ang);
	perp_vel_ball2=-ball[ball2].vy*sin(collision_ang)+ball[ball2].vx*cos(collision_ang);

/* get momentum along angle */
	momentum_ball1=vel_ball1*ball[ball1].mass;
	momentum_ball2=vel_ball2*ball[ball2].mass;
	total_initial_momentum=momentum_ball1+momentum_ball2;

/* get new velocities */

	new_vel_ball2=(total_initial_momentum-(ball[ball2].mass*elasticity*initial_vel_diff))
					/(ball[ball1].mass+ball[ball2].mass);

	new_vel_ball1=(elasticity*initial_vel_diff)+new_vel_ball2;

/* calc z spin */
	z_spin=(collision_ang-ball1_ang)*initial_vel_diff*z_spin_mul;

/* put these new velocities in ball structure */
	ball[ball1].vx=new_vel_ball1*sin(collision_ang)+perp_vel_ball1*cos(collision_ang);
	ball[ball1].vy=new_vel_ball1*cos(collision_ang)-perp_vel_ball1*sin(collision_ang);
	ball[ball1].avz+=z_spin;
	ball[ball1].moving=TRUE;

	ball[ball2].vx=new_vel_ball2*sin(collision_ang)+perp_vel_ball2*cos(collision_ang);
	ball[ball2].vy=new_vel_ball2*cos(collision_ang)-perp_vel_ball2*sin(collision_ang);
	ball[ball2].avz-=z_spin;
	ball[ball2].moving=TRUE;

	add_col_to_list(ball2, ball[ball2].x, ball[ball2].y);
	add_col_to_list(ball1, ball[ball1].x, ball[ball1].y);

/*--------------------------*/
/* update foul flags */

   if (activated_lines==YES) 
    {
     ball_hits_count++;
     return;
    }

	if	( ball[ball2].graphic_type != WHITEBALL)	ball_num=ball2;
	else	ball_num=ball1;

   if ( SNOOKER_MODE == YES )
    {
     check_hits_snooker( ball_num );
    }
   else
	 {
 	  if	(GAME_TYPE == UK_POOL) check_hitsuk( ball_num );
     if	(GAME_TYPE == US_8BALL) check_hits8ball( ball_num );
     if	(GAME_TYPE == US_POOL) check_hitsus( ball_num );
 	  if	(GAME_TYPE == NMS_POOL) check_hitsuk( ball_num );
	 }

 	missed_all_balls_flag=NO;

   if ( calculating_computer_shot == YES ) return;

/*--------------------------*/
/*--------------------------*/
/* trigger samples of balls hitting */

	vol=(ball[ball1].vx*ball[ball1].vy) *1300;
	add_sample_with_volume(ball1,BALL_HIT, 0,vol);

	vol=(ball[ball2].vx*ball[ball2].vy) *1300;
	add_sample_with_volume(ball1,BALL_HIT, 0,vol);

}

/* This is true martix rotation creator. the 1 in my3ds.c has a -ve
	in yaw for camera system in renderware */

void rotate2(double roll, double pitch, double yaw, matrix4x4 spin)
{
    double sr, sp, sy, cr, cp, cy;
    matrix4x4 mr, mp, my;

    sr=sin(roll);   cr=cos(roll);
    sy=sin(yaw);    cy=cos(yaw);
    sp=sin(pitch);  cp=cos(pitch);

    identity4x4(&mr);
    identity4x4(&my);
    identity4x4(&mp);
    
    mr[0][0] = mr[1][1] = cr;
    mr[1][0] = - (mr[0][1] = sr);

                                /* prepare pitch matrix */
    mp[1][1] = mp[2][2] = -cp;
    mp[1][2] = - (mp[2][1] = -sp);
                                /* prepare yaw matrix */
/*
    my[0][0] = my[2][2] = cy;
    my[0][2] = - (my[2][0] = sy);
*/
    my[0][0] = my[2][2] = cy;
    my[0][2] = - (my[2][0] = sy);

    multiply4x4( &mr, &my, spin );
    multiply4x4( spin, &mp, spin );

}
#if	0
/* Read in co-ord positions of cushions for table */
int init_cushions(char filename[])
{
	FILE *fp;
	char string[80];
	int bumper_num=0,  point_pos;
	int pocket_num;
	double temp;

	if((fp = fopen(filename,"ra"))==NULL)	{
		return -1;
	}
	
	while (!feof(fp)) {
		fscanf( fp, "%s", string);

		if (strcmp(string,"point:")==0) {

	/* Read in positions at which cushions start */

			fscanf(fp, "%s", string);
			point_pos=atoi(string);			
			switch (bumper_num) {
				case 1:
					fscanf(fp, "%s", string);
					cushions_y1[point_pos].x=atof(string);
					fscanf(fp, "%s", string);
					cushions_y1[point_pos].y=atof(string);
					fscanf(fp, "%s", string);
					cushions_y1[point_pos].cush_ang=atof(string);
					break;
				case 2:
					fscanf(fp, "%s", string);
					cushions_x2[point_pos].x=atof(string);
					fscanf(fp, "%s", string);
					cushions_x2[point_pos].y=atof(string);
					fscanf(fp, "%s", string);
					cushions_x2[point_pos].cush_ang=atof(string);
					break;
				case 3:
					fscanf(fp, "%s", string);
					cushions_x3[point_pos].x=atof(string);
					fscanf(fp, "%s", string);
					cushions_x3[point_pos].y=atof(string);
					fscanf(fp, "%s", string);
					cushions_x3[point_pos].cush_ang=atof(string);
					break;
				case 4:
					fscanf(fp, "%s", string);
					cushions_y4[point_pos].x=atof(string);
					fscanf(fp, "%s", string);
					cushions_y4[point_pos].y=atof(string);
					fscanf(fp, "%s", string);
					cushions_y4[point_pos].cush_ang=atof(string);
					break;
			}
			continue;
		}

		if (strcmp(string,"edge_step_value:")==0) {
			fscanf(fp, "%s", string);
			bumper_scale=atof(string);
		}
			
		if (strcmp(string,"min_value:")==0) {

	/* Read in positions at which cushions start */

			fscanf(fp, "%s", string);
			temp=atof(string);			
			switch (bumper_num) {
				case 4:
					max_cush_x4=temp;
					break;
				case 2:
					min_cush_y3=temp;
					break;
				case 3:
					max_cush_y2=temp;
					break;
				case 1:
					min_cush_x1=temp;
					break;
			}
		}

		if (strcmp(string,"bumper:")==0) {

			fscanf(fp, "%s", string);
			bumper_num=atoi(string);
		}

		if (strcmp(string,"pocket:")==0) {

	/* pocket num */

			fscanf(fp, "%s", string);
			pocket_num=atoi(string);			
			fscanf(fp, "%s", string);
			fscanf(fp, "%s", string);
			pockets[pocket_num].x=atof(string);
			fscanf(fp, "%s", string);
			fscanf(fp, "%s", string);
			pockets[pocket_num].y=atof(string);
			fscanf(fp, "%s", string);
			fscanf(fp, "%s", string);
			pockets[pocket_num].radius=(atof(string))*1.5;// + 0.061057;
		}
		if (strcmp(string,"pocket_x:")==0) {

	/* pocket num */
			fscanf(fp, "%s", string);
			pockets[pocket_num].x=atof(string);
		}
		if (strcmp(string,"pocket_y:")==0) {

	/* pocket num */
			fscanf(fp, "%s", string);
			pockets[pocket_num].y=atof(string);
		}
		if (strcmp(string,"pocket_radius:")==0) {

	/* pocket num */
			fscanf(fp, "%s", string);
			pockets[pocket_num].radius=atof(string);
		}

	}

	fclose(fp);
	return 1;
}
#endif

flag any_balls_moving(void)
{
	int loop;

	for (loop=0; loop<LAST_BALL; loop++) {
		if ( (ball[loop].moving==YES) && (ball[loop].active>0) ) return TRUE;
	}
	return FALSE;
}


void	check_all_pockets_x1(long ball_num, long bumper)
{
	double	x,y,x1,y1,hypt,dx,dy;


	x=ball[ball_num].x;
	y=ball[ball_num].y;

	x1=pockets[0].x-x;
	y1=pockets[0].y-y;
   if ( x1 < 0 ) x1=x-pockets[0].x;
   if ( y1 < 0 ) y1=y-pockets[0].y;

	x1*=x1;
	y1*=y1;
	hypt=sqrt(x1+y1);

   if ( (ball[ball_num].x < pockets[0].x) or (ball[ball_num].y > pockets[0].y))
    {
     pocketed_ball(ball_num,3,0);
	 }

/* check actual pocket */

	if	( hypt <= pockets[0].radius ) 
    {
	  pocketed_ball(ball_num,0,0);
     return;
	 }

	x1=pockets[1].x-x;
	y1=pockets[1].y-y;
   if ( x1 < 0 ) x1=x-pockets[1].x;
   if ( y1 < 0 ) y1=y-pockets[1].y;

 	x1*=x1;
	y1*=y1;
	hypt=sqrt(x1+y1);

   if ( (ball[ball_num].x < pockets[1].x) or (ball[ball_num].y < pockets[1].y))
    {
     pocketed_ball(ball_num,3,0);
	 }

	if	( hypt <= pockets[1].radius) 
    {
	  pocketed_ball(ball_num,1,0);
     return;
	 }
}

void	check_all_pockets_x4(long ball_num, long bumper)
{
	double	x,y,x1,y1,hypt;

   if ( (ball[ball_num].x > pockets[3].x) or (ball[ball_num].y < pockets[3].y))
    {
     pocketed_ball(ball_num,3,0);
	 }
//   if (LOCATION < DIFFERENT_SHAPED_TABLES_INDEX) 

	x=ball[ball_num].x;
	y=ball[ball_num].y;

	x1=pockets[4].x-x;
	y1=pockets[4].y-y;
   if ( x1 < 0 ) x1=x-pockets[4].x;
   if ( y1 < 0 ) y1=y-pockets[4].y;

	x1*=x1;
	y1*=y1;
	hypt=sqrt(x1+y1);

  if ( (ball[ball_num].x > pockets[4].x) or (ball[ball_num].y > pockets[4].y))
    {
     pocketed_ball(ball_num,3,0);
	 }

	if	( hypt <= pockets[4].radius) 
    {
	  pocketed_ball(ball_num,4,0);
     return;
	 }


	x1=pockets[3].x-x;
	y1=pockets[3].y-y;
   if ( x1 < 0 ) x1=x-pockets[3].x;
   if ( y1 < 0 ) y1=y-pockets[3].y;

	x1*=x1;
	y1*=y1;
	hypt=sqrt(x1+y1);
	if	( hypt <= pockets[3].radius) 
    {
	  pocketed_ball(ball_num,3,0);
     return;
	 }
}


void	check_all_pockets_y2(long ball_num, long bumper)
{
	double	x,y,x1,y1,hypt;



   if (LOCATION < DIFFERENT_SHAPED_TABLES_INDEX) 
   {
   if ( ball[ball_num].y > pockets[5].y)
    {
     pocketed_ball(ball_num,3,0);
	 }

   if ( (ball[ball_num].x > pockets[4].x) or (ball[ball_num].y > pockets[4].y))
    {
     pocketed_ball(ball_num,3,0);
	 }

   if ( (ball[ball_num].x < pockets[0].x) or (ball[ball_num].y > pockets[0].y))
    {
     pocketed_ball(ball_num,3,0);
	 }
  	}

	x=ball[ball_num].x;
	y=ball[ball_num].y;

	x1=pockets[0].x-x;
	y1=pockets[0].y-y;

   if ( x1 < 0) x1=x-pockets[0].x;
   if ( y1 < 0) y1=y-pockets[0].y;

	x1*=x1;
	y1*=y1;

	hypt=sqrt(x1+y1);

	if	( hypt <= pockets[0].radius/4) 
    {
	  pocketed_ball(ball_num,0,0);
     return;
	 }


	x1=pockets[5].x-x;
	y1=pockets[5].y-y;
   if ( x1 < 0 ) x1=x-pockets[5].x;
   if ( y1 < 0 ) y1=y-pockets[5].y;
	x1*=x1;
	y1*=y1;
	hypt=sqrt(x1+y1);
	if	( hypt <= pockets[5].radius) 
    {
	  pocketed_ball(ball_num,5,0);
     return;
	 }




	x1=pockets[4].x-x;
	y1=pockets[4].y-y;
   if ( x1 < 0 ) x1=x-pockets[4].x;
   if ( y1 < 0 ) y1=y-pockets[4].y;
	x1*=x1;
	y1*=y1;
	hypt=sqrt(x1+y1);
	if	( hypt <= pockets[4].radius) 
    {
	  pocketed_ball(ball_num,4,0);
     return;
	 }
}


void	check_all_pockets_y3(long ball_num, long bumper)
{
	double	x,y,x1,y1,hypt;
	long	p;


	for	(p=1; p < 4; p++)	{



   if (LOCATION < DIFFERENT_SHAPED_TABLES_INDEX) 
   {
   switch (p)
   {
   case 1:
   if ( (ball[ball_num].x < pockets[p].x) or (ball[ball_num].y < pockets[p].y))
    {
     pocketed_ball(ball_num,3,0);
	 }
   break;

   case 2:
   if ( ball[ball_num].y < pockets[p].y)
    {
     pocketed_ball(ball_num,3,0);
	 }
   break;
   case 3:
   if ( (ball[ball_num].x > pockets[p].x) or (ball[ball_num].y < pockets[p].y))
    {
     pocketed_ball(ball_num,3,0);
	 }
   break;
	}
	}

	x=ball[ball_num].x;
	y=ball[ball_num].y;

	x1=pockets[p].x-x;
	y1=pockets[p].y-y;

   if ( x1 < 0 ) x1=x-pockets[p].x;
   if ( y1 < 0 ) y1=y-pockets[p].y;

	x1*=x1;
	y1*=y1;
	hypt=sqrt(x1+y1);

	if	( hypt <= pockets[p].radius) 
    {
	  pocketed_ball(ball_num,p,0);
     return;
	 }
  }
}

void test_camers_in_pocket( long ballnum )
{
	double	x,y,x1,y1,hypt,dist;
	long	p;

   
   if ( ( ballnum == 0 ) && ( camera_in_pocket == NO ) )
    {
       x=ball[ballnum].x;
       y=ball[ballnum].y;

       for (dist  = 0 ; dist < (ball[ballnum].vx+ball[ballnum].vy) ; 
           dist += (ball[ballnum].vx+ball[ballnum].vy)*(0.057/2.0) )
        {

         x+=(0.057/2.0)*ball[ballnum].vx;
	      y+=(0.057/2.0)*ball[ballnum].vy;

         for (p=0;p<6;p++)
          {

         x1=pockets[p].x-x; 
         y1=pockets[p].y-y;

         x1*=x1;
         y1*=y1;
         hypt=sqrt(x1+y1);

         if	( hypt <= pockets[p].radius*1.2 ) 
          {
           setup_camera_in_pocket(p);
           return;
		    }
        }
	   }
	 }
}

long   check_solid_pocket(long ball_num , double oldx,double oldy)
{
	double	x,y,x1,y1,hypt;
	long	p;

	for	(p=0; p < 5; p++)	{

	x=ball[ball_num].x;
	y=ball[ball_num].y;

	x1=pockets[p].x-x;
	y1=pockets[p].y-y;

   if ( x1 < 0 ) x1=x-pockets[p].x;
   if ( y1 < 0 ) y1=y-pockets[p].y;

	x1*=x1;
	y1*=y1;
	hypt=sqrt(x1+y1);

	if	( hypt <= pockets[p].radius) 
    {
     ball[ball_num].y=oldy;
     ball[ball_num].x=oldx;
     return -1;
	 }
  }
  return 0;
}

