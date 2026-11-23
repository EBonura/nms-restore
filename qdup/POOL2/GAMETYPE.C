/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/*									    	 QD-UP      										 */
/*											PC-CD ROM											 */
/*									 For BMG	Interactive										 */
/*									 By NMS Software Ltd.									 */
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#include	"globals.h"
//#include	"gametypes.h"
#include "leveldat.c"
long	black_ball_spot;		// RESTORE: 8.24, was double
/* -------------------------------------------- */
void Setup_New_Game_Type(void)
{										 
	long	c,room;

/* reset game varibles */

   draw_twice=2;
	players[0].ball_type = NULL;
	players[1].ball_type = NULL;
	killer_turn=0;
	old_killer_turn=0;
   nine_ball_count=1;
	ball_in_hand_flag=YES;

   flash_name_1 = FLASH_NAME_TIME;
   flash_name_2 = 0;
	Player_Active=0;
	first_shot=NO;
	Two_Shots = NULL;

/* reset clock */

	time_value[0]=0;
	time_value[1]=0;

/* dont resetup current game type */

   if ( SNOOKER_MODE == YES)
    {
	  LAST_BALL = 22;
    }
   else
    {
     switch	(GAME_TYPE)	
      {

/* setup number of ball in pack + white ball */

	    case	UK_POOL:
			LAST_BALL = 16;
			break;

		 case	US_8BALL:
			LAST_BALL = 16;
			break;

		 case	US_POOL:
			LAST_BALL = 10;
			break;

		 case	CONTINUOUS:
			LAST_BALL = 16;
			break;
			
		 case	SPEED_POOL:
			LAST_BALL = 16; /* uk ball set */
			break;

		 case	KILLER_POOL:
			LAST_BALL = 16;
			break;

	  	 case	NMS_POOL:
			LAST_BALL = 16;
			break;

		 case	TRICK_SHOT:
			LAST_BALL = 16;
			break;
	   }
	 }

/* setup position to rack balls */

	if ( LOCATION <= DIFFERENT_SHAPED_TABLES_INDEX-1) room=(LOCATION*3)+((TABLESIZE_TYPE));
   else room = SPOT_ROOM(LOCATION);	// RESTORE: was DIFFERENT_SHAPED_TABLES_INDEX*3 + (LOCATION-DIFFERENT_SHAPED_TABLES_INDEX), see GLOBALS.H SPOT_ROOM

   if ( SNOOKER_MODE == YES ) room = 4*3;   /* setup snooker balls pack position */
   black_ball_spot=ball_spot_positions[room];

/*	setup balls positions for break..*/

	setup_balls();			

/* setup maximum lives dor players in killer pool */

	for	(c=0; c < MAX_KILLER_PLAYERS; c++)
			lives_left[c]=NUMBER_OF_LIVES_IN_KILLER;
}
/* -------------------------------------------- */
void	setup_balls( void )
{
	int	loop,spr_index;

	break_flag=YES;
	show_panel=PANEL_ON_TIME;

/* -------------------------------------------- */
/* Clear float ball positions */

  		for (loop=0; loop<LAST_BALL; loop++) {
			init_ball_position(loop);
		}

		ball_nominate = 1;

     if ( SNOOKER_MODE == YES)
      {
       UK_OR_US_BALL_SET=NO;
		 setup_snooker();
      }
     else
      {
		switch	(GAME_TYPE)	{

/* setup number of ball in pack + white ball */

	  	case	UK_POOL:
         UK_OR_US_BALL_SET=NO;
			setup_uk_pool();
			break;

		case	US_8BALL:
         UK_OR_US_BALL_SET=YES;
			setup_us_8ball();
			break;

		case	US_POOL:
         UK_OR_US_BALL_SET=YES;
 		 	setup_us_pool();
			break;

		case	CONTINUOUS:
         UK_OR_US_BALL_SET=YES;
			setup_us_8ball();
			break;
			
		case	SPEED_POOL:
         UK_OR_US_BALL_SET=NO;
			end_of_killer_game=NO;
			setup_uk_pool();
			break;

		case	KILLER_POOL:
         UK_OR_US_BALL_SET=YES;
			killer_pot=1;
			end_of_killer_game=NO;
			only_once = FALSE;
			setup_us_8ball();
			break;

	  	case	NMS_POOL:
         UK_OR_US_BALL_SET=NO;
			setup_uk_pool();
			break;

		case	TRICK_SHOT:
			setup_us_8ball();
			break;
		}
		}


		ball[WHITE_BALL].obj_type = WHITE;
		ball[WHITE_BALL].ball_colour = WHITE;
		ball[WHITE_BALL ].graphic_type = WHITEBALL;

		white_ball_gone_float();

/* setup stripe ball colour to be white with coloured band */

     if	( (GAME_TYPE!=UK_POOL) && (GAME_TYPE!=NMS_POOL) && (SNOOKER_MODE==NO) )	  	  
     {

      for (loop=0;loop<LAST_BALL;loop++)
       {
        if	(ball[loop].obj_type !=BLACK ) 
        {
         if	(ball[loop].graphic_type == SPOT)	ball[loop].obj_type=YELLOW;
         if	(ball[loop].graphic_type != SPOT)	ball[loop].obj_type=RED;
	     }
	    }
	   }

		for	(spr_index=0;spr_index<LAST_BALL;spr_index++)
		{
       *(balls_xy+(spr_index*2))     = ball[ spr_index ].x;
       *(balls_xy+((spr_index*2)+1)) = ball[ spr_index ].y;
       ball[spr_index].x_prev = ball[spr_index].x;
       ball[spr_index].y_prev = ball[spr_index].y;
       ball[spr_index].z_prev = ball[spr_index].z;
		}

}
/* -------------------------------------------- */
long	ball_positions[] = {
							 //------
							 RED,
							SPOT,1,
							 RED,
							-1,2,
							 YELLOW,
							SPOT,3,
							 RED,
							-1,4,
							 YELLOW,
							SPOT,5,
							 //------
							 YELLOW,
							SPOT,6,
							 RED,
							-1,7,
							 BLACK,
							SPOT_BLACK,8,
							 RED,
							-1,9,
							 //-------
							 YELLOW,
							SPOT,10,
							 RED,
							-1,11,
							 YELLOW,
							SPOT,12,
							 //--------
							 YELLOW,
							SPOT,13,
							 YELLOW,
							SPOT,14,
							 RED,
							-1,15
							 //------
							};
/* -------------------------------------------- */
long   nine_ball_positions[] = {
								YELLOWBALL,
							SPOT,1,
								BLUEBALL,
							SPOT,2,
								RED,
							SPOT,3,
								PURPLE,
							SPOT,4,
								ORANGE,
							SPOT,5,
								GREEN,
							SPOT,6,
								BROWN,
							SPOT,7,
								BLACK,
							SPOT,8,
								YELLOW,
							STRIPE,9  
							 //------
							};
/* -------------------------------------------- */
long   eight_ball_positions[] = {
								YELLOWBALL,
							SPOT,1,
								BLUEBALL,
							SPOT,2,
								RED,
							SPOT,3,
								PURPLE,
							SPOT,4,
								ORANGE,
							SPOT,5,
								GREEN,
							SPOT,6,
								BROWN,
							SPOT,7,
								BLACK,
							SPOT,8,

								YELLOWBALL,
							STRIPE,9,
								BLUEBALL,
							STRIPE,10,
								RED,
							STRIPE,11,
								PURPLE,
							STRIPE,12,
								ORANGE,
							STRIPE,13,
								GREEN,
							STRIPE,14,
								BROWN,
							STRIPE,15,

							};
/* -------------------------------------------- */
long re_order_pack[] = { 0, 10,5,7,9,15,4,1,3,12,11,2,13,6,14,8 };
long   us_ball_positions[] = {
								YELLOWBALL, //1
							SPOT,1,
								BLUEBALL,	//2
							SPOT,2,
								RED,			//3
							SPOT,3,
								PURPLE,		//4
							SPOT,4,
								ORANGE,		//5
							SPOT,5,
								GREEN,		//6
							SPOT,6,
								BROWN,		//7
							SPOT,7,
								BLACK,		//8
							SPOT,8,
								YELLOWBALL,	//9
							STRIPE,9,
								BLUEBALL,	//10
							STRIPE,10,
								RED,			//11
							STRIPE,11,
								PURPLE,		//12
							STRIPE,12,
								ORANGE,		//13
							STRIPE,13,
								GREEN,		//14
							STRIPE,14,
								BROWN,		//15
							STRIPE,15,
							};
/* -------------------------------------------- */
void	setup_uk_pool(void)
{
#define	max_rand	108
#define	default_add	118

	long	spr_index,tab_off;
	long	ang,type;
 	long	*ball_pointer;
	long	xball_start,new_ball_offset,x,y;	// RESTORE: 8.24, these were doubles
	long	yball_start;
	long	ball_offset;

	ang = 0;
	tab_off=0;
	type=0;

	ball_offset=mul64(ball[0].diameter, FIX24(1.04));
	xball_start =black_ball_spot ;
	yball_start =0;
	new_ball_offset=mul64(ball_offset, FIX24(.866));
	ball_pointer = ball_positions;

	ran1=0;
	ran2=0;

 		for	(spr_index=1; spr_index < LAST_BALL; spr_index++)
			{

			if	(spr_index-1 <= 4)
				{
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);

			if	(spr_index-1 < 4)
				{
				xball_start+=(new_ball_offset);
				yball_start+=(ball_offset/2);			
				}
				}

/* do backrow */

			if	( (spr_index-1 >= 5) && (spr_index-1 <= 8) )
				{
		  		yball_start-=(ball_offset);
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);

			}


			if	( (spr_index-1 >= 9) && (spr_index-1 <= 11) )
				{
				yball_start+=(ball_offset/2);
				xball_start-=(new_ball_offset);
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
		  		}

			if	( (spr_index-1 >= 12) && (spr_index-1 <= 13) )
				{
				yball_start+=(ball_offset/2);
				xball_start+=(new_ball_offset);
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
  				}

			if	(spr_index-1 == 14)
				{
				yball_start-=(ball_offset);
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);

				}

		ball[spr_index].x_prev=ball[spr_index].x;
		ball[spr_index].y_prev=ball[spr_index].y;

		ball[ spr_index ].obj_type = *(ball_pointer+tab_off);
		ball[ spr_index ].graphic_type = *(ball_pointer+tab_off+1);
		ball[ spr_index ].num = *(ball_pointer+tab_off+2);
		ball[ spr_index ].numstruct =spr_index;
		ball[ spr_index ].ball_colour = *(ball_pointer+tab_off);;
		tab_off+=3;

		}

/* reorder balls */

    x = ball[13].x;
    y = ball[13].y;
    ball[13].x = ball[8].x;
    ball[13].y = ball[8].y;
    ball[8].x = x;
    ball[8].y = y;

}
/* -------------------------------------------- */
void	setup_us_pool(void)
{
	long	spr_index;
	long	ang,type,tab_off;
 	long	*ball_pointer;
	long	x,y;			// RESTORE: 8.24, was double

	long	xball_start,new_ball_offset;	// RESTORE: 8.24, these were doubles
	long	yball_start;
	long	ball_offset;

	ang = 0;
	tab_off=0;
	type=0;

	yball_start =0;
	xball_start =black_ball_spot ;

	ball_pointer = nine_ball_positions;

	ran1=0;
	ran2=0;

	random_value+=FIX24(.01);	// RESTORE: 8.24
	if	(random_value > FIX24(0.05)) random_value=0;
	ball_offset=mul64(ball[0].diameter, FIX24(1.01)+random_value);
	new_ball_offset=mul64(ball_offset, FIX24(.866));

 		for	(spr_index=1; spr_index < LAST_BALL; spr_index++)
			{

			if	(spr_index-1 <= 2)
				{
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
			if	(spr_index-1 < 2)
				{
				xball_start+=(new_ball_offset);
				yball_start-=(ball_offset/2);
				}
				}

			if	( (spr_index-1 >= 3) && (spr_index-1 <= 4) )
				{
		  		xball_start+=new_ball_offset;
				yball_start+=ball_offset/2;
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
  				}


			if	( (spr_index-1 >= 5) && (spr_index-1 <= 6) )
				{
				yball_start+=ball_offset/2;
				xball_start-=new_ball_offset;
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
				}

			if	(spr_index-1 == 7)
				{
				yball_start-=ball_offset/2;
				xball_start-=new_ball_offset;

	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
			 	}

			if	(spr_index-1 == 8)
				{
				yball_start-=ball_offset/2;
				xball_start+=new_ball_offset;

	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
			 	}

		ball[spr_index].x_prev=ball[spr_index].x;
		ball[spr_index].y_prev=ball[spr_index].y;
		ball[ spr_index ].obj_type = *(ball_pointer+tab_off);
	   ball[ spr_index ].graphic_type = *(ball_pointer+tab_off+1);
		ball[ spr_index ].num = *(ball_pointer+tab_off+2);
		ball[ spr_index ].numstruct =spr_index;
	   ball[ spr_index ].ball_colour = *(ball_pointer+tab_off);

		tab_off+=3;
		}

/* reorder balls */

    x = ball[2].x;
    y = ball[2].y;
    ball[2].x = ball[5].x;
    ball[2].y = ball[5].y;
    ball[5].x = x;
    ball[5].y = y;

    x = ball[2].x_prev;
    y = ball[2].y_prev;
    ball[2].x_prev = ball[5].x_prev;
    ball[2].y_prev = ball[5].y_prev;
    ball[5].x_prev = x;
    ball[5].y_prev = y;

}
/* -------------------------------------------- */
void	setup_us_8ball(void)
{
	long	spr_index,tab_off;
	long	ang,type;
 	long	*ball_pointer,count;

	long	xball_start,new_ball_offset;	// RESTORE: 8.24, these were doubles
	long	yball_start;
	long	ball_offset;

	ang = 0;
	tab_off=0;
	type=0;

	ball_offset=mul64(ball[0].diameter, FIX24(1.05));
	xball_start =black_ball_spot ;
	yball_start =0; //whitey;
	new_ball_offset=mul64(ball_offset, FIX24(.866));

	ball_pointer = us_ball_positions;
	ran1=0;
	ran2=0;

 		for	(spr_index=1; spr_index < LAST_BALL; spr_index++)
			{

			if	(spr_index-1 <= 4)
				{
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
			if	(spr_index-1 < 4)
				{
				xball_start+=new_ball_offset;
				yball_start+=ball_offset/2;
				}
				}

			if	( (spr_index-1 >= 5) && (spr_index-1 <= 8) )
				{
		  		yball_start-=(ball_offset);
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
				}

			if	( (spr_index-1 >= 9) && (spr_index-1 <= 11) )
				{
				yball_start+=((ball_offset/2));
				xball_start-=new_ball_offset;
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
				}

			if	( (spr_index-1 >= 12) && (spr_index-1 <= 13) )
				{
				yball_start+=((ball_offset/2));
				xball_start+=new_ball_offset;

	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
				}

			if	(spr_index-1 == 14)
				{
				yball_start-=(ball_offset);
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
				}

		ball[spr_index].x_prev=ball[spr_index].x;
		ball[spr_index].y_prev=ball[spr_index].y;

		saveball[spr_index].x_prev=ball[spr_index].x;
		saveball[spr_index].y_prev=ball[spr_index].y;
			
		ball[ spr_index ].obj_type = *(ball_pointer+tab_off);
		ball[ spr_index ].graphic_type = *(ball_pointer+tab_off+1);
		ball[ spr_index ].numstruct =spr_index;
		ball[ spr_index ].num = *(ball_pointer+tab_off+2);
		ball[ spr_index ].ball_colour = *(ball_pointer+tab_off);;


		tab_off+=3;
		}


     for ( count = 1 ; count < LAST_BALL ; count++)
		{
		 ball[re_order_pack[count] ].x = saveball[ count ].x_prev;
		 ball[re_order_pack[count] ].x_prev = saveball[ count ].x_prev;

		 ball[re_order_pack[count] ].y = saveball[ count ].y_prev;
		 ball[re_order_pack[count] ].y_prev = saveball[ count ].y_prev;
      }

}
/* -------------------------------------------- */
/* -------------------------------------------- */
long	snooker_ball_positions[] = {
							 //------
							 RED,
							SPOT,1,
							 RED,
							SPOT,2,
							 RED,
							SPOT,3,
							 RED,
							SPOT,4,
							 RED,
							SPOT,5,
							 //------
							 RED,
							SPOT,6,
							 RED,
							SPOT,7,
							 RED,
							SPOT,8,
							 RED,
							SPOT,9,
							 //-------
							 RED,
							SPOT,10,
							 RED,
							SPOT,11,
							 RED,
							SPOT,12,
							 //--------
							 RED,
							SPOT,13,
							 RED,
							SPOT,14,
							 RED,
							SPOT,15,

							 BLACK,
							SPOT,16,
							 PURPLE,
							SPOT,17,
							 BLUEBALL,
							SPOT,18,
							 GREEN,
							SPOT,19,
							 BROWN,
							SPOT,20,
							 YELLOW,
							SPOT,21,
                     
							 //------
							};
void setup_snooker(void)
{

	long	spr_index,tab_off;
	long	ang,type;
 	long	*ball_pointer;
	long	xball_start,new_ball_offset;	// RESTORE: 8.24, these were doubles
	long	yball_start;
	long	ball_offset;

	ang = 0;
	tab_off=0;
	type=0;

	ball_offset=mul64(ball[0].diameter, FIX24(1.04));
	xball_start =black_ball_spot ;
	yball_start =0;
	new_ball_offset=mul64(ball_offset, FIX24(.866));
	ball_pointer = snooker_ball_positions;

	ran1=0;
	ran2=0;

 		for	(spr_index=1; spr_index < LAST_BALL; spr_index++)
			{

			if	(spr_index-1 <= 4)
				{
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);

			if	(spr_index-1 < 4)
				{
				xball_start+=(new_ball_offset);
				yball_start-=(ball_offset/2);			
				}
				}

/* do backrow */

			if	( (spr_index-1 >= 5) && (spr_index-1 <= 8) )
				{
		  		yball_start+=(ball_offset);
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);

			}


			if	( (spr_index-1 >= 9) && (spr_index-1 <= 11) )
				{
				yball_start-=(ball_offset/2);
				xball_start-=(new_ball_offset);
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
		  		}

			if	( (spr_index-1 >= 12) && (spr_index-1 <= 13) )
				{
				yball_start-=(ball_offset/2);
				xball_start+=(new_ball_offset);
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);
  				}

			if	(spr_index-1 == 14)
				{
				yball_start+=(ball_offset);
	  			ball[ spr_index ].x = (xball_start);
	  			ball[ spr_index ].y = (yball_start);

				}

		ball[spr_index].x_prev=ball[spr_index].x;
		ball[spr_index].y_prev=ball[spr_index].y;

		ball[ spr_index ].obj_type = *(ball_pointer+tab_off);
		ball[ spr_index ].graphic_type = *(ball_pointer+tab_off+1);
		ball[ spr_index ].num = *(ball_pointer+tab_off+2);
		ball[ spr_index ].numstruct =spr_index;
		ball[ spr_index ].ball_colour = *(ball_pointer+tab_off);;
		tab_off+=3;

		}

/* setup coloured balls */

/* setup black position */
    ball[16].x = FIX24(1.480);	// RESTORE: 8.24
    ball[16].y = FIX24(0.0);

/* setup pink position */
    ball[17].x = FIX24(0.9470);
    ball[17].y = FIX24(0.0);

/* setup blue position */
    ball[18].x = FIX24(0.0);
    ball[18].y = FIX24(0.0);

/* setup green position */

    ball[19].x = FIX24(-1.045);
    ball[19].y = FIX24(0.30);

/* setup brown position */

    ball[20].x = FIX24(-1.045);
    ball[20].y = FIX24(0.0);

/* setup yellow position */

    ball[21].x = FIX24(-1.045);
    ball[21].y = FIX24(-0.30);

    for (spr_index=16;spr_index<22;spr_index++)
     {
		ball[spr_index].x_prev=ball[spr_index].x;
		ball[spr_index].y_prev=ball[spr_index].y;
     }

}


