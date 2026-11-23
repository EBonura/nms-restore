/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/*									    	 QD-UP      										 */
/*											PC-CD ROM											 */
/*									 For BMG	Interactive										 */
/*									 By NMS Software Ltd.									 */
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#include "globals.h"
#include	"rules.h"
long save_frame;
void  check_ball_in_safe_place( long ballnum);
flag	update_fouls(void)
{
long i;

//g  if (	(FXS_FLAG==TRUE) && (sound_active==YES) )
//g  {
//g  for (i=0;i<8;i++)
//g  {
//g  while ((query_sample(i)==ON))
//g   {
//g    Handle3D();
//g    if (keys[kK]==128) break;
//g   }
//g  }
//g  }

	/*------------------------------
	 check foul for game type
	 ------------------------------*/

    if ( SNOOKER_MODE == YES )
     {
      us_snooker_foul();
	  }
    else
     {
		switch	(GAME_TYPE)	{

/* setup number of ball in pack + white ball */

	  	case	UK_POOL:
			uk_pool_foul();
			break;

		case	US_8BALL:
			us_8ball_foul();
			break;

		case	US_POOL:
			us_pool_foul();
			break;

		case	CONTINUOUS:
			continuous_foul();
			break;
			
		case	SPEED_POOL:
			speed_pool_foul();
			break;

		case	KILLER_POOL:
			killer_pool_foul();
			break;

	  	case	NMS_POOL:
			nms_pool_foul();
			break;
		}
	 }

/* checvk all common fouls */
	check_all_fouls();

 	reset_fouls();

	if ( GAME_TYPE == KILLER_POOL ) 
    {
     Killer_Pool_();
     clear_replay_panel_count=4;
     draw_twice=2;
    }

	if	(game_over==YES) return	TRUE;

	return	FALSE;
}											  
/*--------------------------------------------------------------------------*/						 
void	us_8ball_foul(void)
{
	long	check_ball;

	check_ball=0;

	while	( ball[check_ball].obj_type != BLACK)
    {
     check_ball++;
    }

	if	(ball[check_ball].active == YES) return;
   if ( players[Player_Active].ball_type!=BLACK )	return;
   if ( ball[WHITE_BALL].active==NO) return;

	game_over=YES;								
}
/*--------------------------------------------------------------------------*/						 
void	us_snooker_foul(void)
{
	long	check_ball;

	check_ball=0;

	while	( ball[check_ball].obj_type != BLACK)
    {
     check_ball++;
    }

	if	(ball[check_ball].active == YES) return;
   if ( ball[WHITE_BALL].active==NO) return;

/* check to see if all other balls potted, if so then game over else respot */

	check_ball=1;
	while	( ball[check_ball].active == NO )
    {
     check_ball++;
     if ( check_ball > LAST_BALL ) break;
    }

   if ( check_ball < LAST_BALL-1 ) return;

	game_over=YES;								
}
/*--------------------------------------------------------------------------*/						 
void	nms_pool_foul(void)
{
	long	check_ball;
	check_ball=0;

	while	(ball[check_ball].obj_type != BLACK) check_ball++;
	if	( ball[check_ball].active == YES) return;

   if ( NMS_RESPOT_ON_ILLEGAL_POT == NO )
    {
     if ( ball[WHITE_BALL].active==NO) return;
     if ( players[Player_Active].ball_type!=BLACK )	return;
	 }

	game_over=YES;								
}
/*--------------------------------------------------------------------------*/						 
void	uk_pool_foul(void)
{
	long	check_ball=0;

	while	(ball[check_ball].obj_type != BLACK) check_ball++;

	if	( ball[check_ball].active == YES ) return;

   foul_occured = POT_BLACK_CORRECT;
	game_over=YES;								
}
/*--------------------------------------------------------------------------*/						 
void	us_pool_foul(void)
{

/* check to see if nine ball as been pocketed, if so then game over */
  	if	( ball[9].active == NO ) 
    {
	  game_over=YES;
	 }
}
/*--------------------------------------------------------------------------*/						 
void  speed_pool_foul(void)
{
	long	c,c1;
	game_over=NO;

	/* check top see if all balls are pocketed? */

	c1=0;
	for	(c=1;c<LAST_BALL;c++)
	 {
	  if	( ball[c].active == NO) c1++;
	 }

  	if	(c1 >= LAST_BALL-1) 
    {
     end_of_killer_game=YES;
     game_over=YES;
    }

}
/*--------------------------------------------------------------------------*/						 
void	white_ball_gone(void)
{
/* setup boundary for white ball down table */

		top_bumper_value = HEAD_STRING;

		white_ball_gone_float();
}
/*--------------------------------------------------------------------------*/						 
/*							Continuous Pool Foul checks									 */
void	continuous_foul(void)
{
}
/*--------------------------------------------------------------------------*/						 
void	check_hits8ball( long ball_num )
{
	if	( first_hit == NO)	
    {
		first_hit = YES;
		 
		/* check to see if you hit black first!! */

		if	(players[Player_Active].ball_type != BLACK)	
       {
        if	(ball[ball_num].obj_type == BLACK)
         {
          hit_black_first_flag=YES; return; 
         }
       }

		if	(players[Player_Active].ball_type != NULL)  
       {
		  if	(players[Player_Active].ball_type != ball[ball_num].obj_type)
         {
          if	( ball[ball_num].obj_type == YELLOW )
           {
            hit_yellow_ball_flag=YES;
           }
          else 
           {
            hit_red_ball_flag=YES;
           }
         }
	    }
	 }
}
/*--------------------------------------------------------------------------*/						 
void  check_hits_snooker( long ball_num )
{
long count,count1,amount;

	if	( first_hit == NO)	
    {

     first_hit = YES;
		 
/* check free ball, hit any ball first fkag */

		/* check to see if you hit black first!! */

    count=1;

    while ( ball[count].active==NO)
     {
	   count++;
     }

    if ( count < 15 )
    {

     if ( SNOOKER_SHOOT_MODE == POT_RED_BALL )
      {

       if	(ball[ball_num].ball_colour != RED)
        {
         hit_red_ball_flag=YES;

/* add foul to other player */

         switch (ball[ball_num].ball_colour )
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

         Add_Score_2_other_player( amount );

        }
      }
     else
      {
       if	(ball[ball_num].ball_colour == RED)
        {
         hit_yellow_ball_flag=YES;

/* add 4 points to opponent */
         Add_Score_2_other_player( 4 );

        }
      }
    }
   else
    {
/* check to see if player has hit correct colour first */

    if ( ball[ball_num].num != count) 
       {
        hit_red_ball_flag=YES;
		 }
   }
  }
}
/*--------------------------------------------------------------------------*/						 
void	check_hitsuk( long ball_num )
{
	if	( first_hit == NO)	
    {

    first_hit = YES;
		 
/* check free ball, hit any ball first fkag */

    if	( UK_FREE_BALL==NO )
     {
	   if	( ( Two_Shots > 1 ) && ( used_free_ball == NO ) )
       {
        return;
       }
	  }

/* check to see if you hit black first!! */

		if	(players[Player_Active].ball_type != BLACK) 
       {
        if	(ball[ball_num].obj_type == BLACK)
         {
          hit_black_first_flag=YES;
          return; 
         }
       }

		if	(players[Player_Active].ball_type != NULL)  
       {
		  if	(players[Player_Active].ball_type != ball[ball_num].obj_type)
         {

          if	( ball[ball_num].obj_type == YELLOW )
           {
            hit_yellow_ball_flag=YES;
           }
          else 
           {
            hit_red_ball_flag=YES;
           }
         }
		}
	}
}
/*--------------------------------------------------------------------------*/						 
void	check_hitsus( long ball_num )
{
	if	( first_hit == NO)
    {
	  first_hit = YES;

     if	( (ball[ball_num].num == nine_ball_count)
         || (LOWEST_BALL_HIT_FIRST_FLAG==YES) )
		{
	    LOWEST_BALL_HIT_FIRST_FLAG=YES;
	    return;
		}
		else	
       {
        lowest_ball_not_hit_first=YES;
		 }
	  }
}
/*--------------------------------------------------------------------------*/						 
void 	save_replay(void)
{
	long	s;
	for	(s=0;s<10;s++)	save_time_value[s]=time_value[s];
	save_frame=frame_count;
	first_replay=YES;
}
/*--------------------------------------------------------------------------*/						 
void 	setup_replay(void)
{
	long	s;
	for	(s=0;s<10;s++)	time_value[s]=save_time_value[s];

	frame_count=save_frame;
	while(frame_count!=0)
	 {
	  frame_count--;
    }
}
/*--------------------------------------------------------------------------*/						 
void	killer_pool_foul(void)
{
	long	count,loop;
	long	x,y;			// RESTORE: 8.24, was double

/* in killer pool and all object balls have been potted, re-rack pack */

   loop=0;
   for (count=1 ; count < LAST_BALL ; count++ )
     {
      if ( ball[ count ].active == NO ) loop++;
     }

/* check re-rack ? */

  if ( loop >= (LAST_BALL-1) )
   {

    for (count=1;count<LAST_BALL;count++)
     {
	   init_ball_position(count);
      x=*(balls_xy+count*2);
      y=*(balls_xy+count*2+1);
      ball[count].x=x;
      ball[count].y=y;
      ball[count].x_prev=ball[count].x;
      ball[count].y_prev=ball[count].y;
      ball[count].z_prev=ball[count].z;
	  }
   }

	/* check to see if out of game */

	if	( lives_left[killer_turn] == 0 ) return;	

	/* check to see if white ball potted! */

	if	(break_flag == NO) {

/* check in off flag against white ball potted */

	switch	(KILLER_IN_OFF)	
    {
     case	YES:
     if	( (potted_flag == NO) || (ball[WHITE_BALL].active==NO) )
		{
		 lives_left[killer_turn]--;	
       last_killer_turn = killer_turn;
       foul_occured=LOSE_LIFE_FOUL;

		}
     break;
	
     case	NO:	
	  if	( potted_flag == NO)
		{
		 lives_left[killer_turn]--;	
       last_killer_turn = killer_turn;
       foul_occured=LOSE_LIFE_FOUL;
		}
	  break;
	 }
	}

/* if player potted black then award them an extra life */

	if	( (ball[8].active == NO) && (only_once == FALSE) ) 
		{
		only_once=TRUE;
			if	( lives_left[killer_turn] < 5) {
			lives_left[killer_turn]++;	
			}
		}

}
/*--------------------------------------------------------------------------*/						 
void	Killer_Pool_(void)
{
	long	count=0,alldead=0,alive;

	killer_turn++;
	killer_turn%=NUMBER_OF_PLAYERS_IN_KILLER;

	for	(count=0;count<NUMBER_OF_PLAYERS_IN_KILLER;count++)	
    {
	  if	(lives_left[count] > 0) 
      {
       alldead++;
       alive=count; 
      }
	 }

/* ----------------------------------- */

	if	(alldead > 1) return;

/* setup winner */
	fadeout();
	killer_winner=killer_order[alive];
	end_of_killer_game=YES;
	
	game_over=YES;
	return;
}
/*--------------------------------------------------------------------------*/						 
#define WHITE_Y		FIX24(0.0)		// RESTORE: 8.24
#define WHITE_X		FIX24(-0.550) //882	// on line
void	white_ball_gone_float(void)
{
   long room;

	init_ball_position(WHITE_BALL);

	ball[WHITE_BALL].x=WHITE_X;
	ball[WHITE_BALL].y=WHITE_Y;
	ball[WHITE_BALL].moving=FALSE;
	ball[WHITE_BALL].active=TRUE;
	ball[WHITE_BALL].z=ball[WHITE_BALL].radius;
   ball[WHITE_BALL].ball_colour = WHITE;

/* setup ball just behind head string */

	if ( LOCATION <= DIFFERENT_SHAPED_TABLES_INDEX-1) 
    {
     if (SNOOKER_MODE == 0) room=(LOCATION*3)+((TABLESIZE_TYPE));
      else
       {
        ball[WHITE_BALL].x = -(FIX24(1.045)+ball[0].diameter);	// RESTORE: 8.24
        check_ball_in_safe_place(WHITE_BALL);
        return;
       }
    }
   else room = SPOT_ROOM(LOCATION);	// RESTORE: was DIFFERENT_SHAPED_TABLES_INDEX*3 + (LOCATION-DIFFERENT_SHAPED_TABLES_INDEX), see GLOBALS.H SPOT_ROOM

   ball[WHITE_BALL].x = whiteball_spot_positions[room] ;

   check_ball_in_safe_place(WHITE_BALL);
}
/*--------------------------------------------------------------------------*/						 
void	check_all_fouls(void)
{
	long	swap_turn=NO,time,miss_it,miss_white,check_ball,good_game;


/* dont show any fouls in load saved shot mode */

  if ( ball[0].active==0) 
  {
   ball[0].active=0;
   ball[0].active=0;
   ball[0].active=0;
   ball[0].active=0;
  }

	if	(show_saved_shot_flag==YES) return;

/* test break conditions for uk pool */

   if ( (GAME_TYPE == UK_POOL) && ( break_flag == YES) )
    {

/* in uk pool, check to see if player has potted black off break, if so
 then no foul given */

     if ( potted_black_wrong_flag == YES )
      {

/* if white ball potted as well frame given to other player */
    
        if ( ball[WHITE_BALL].active==NO )
			Swap_Player();

       foul_occured = POT_BLACK_WRONG_FOUL_OTHER_PLAYER_WINS;

       return;
      }
    }

/* ----------------------------------------- */
/* check to see if potted 1 of each coloured balls, if so allow player
 to choose which colour they play */

   good_game=NO;

   if (GAME_TYPE==UK_POOL) good_game=YES;
   if (GAME_TYPE==US_8BALL) good_game=YES;

	if	( (good_game==YES) && (game_over==NO) && (last_time_ball_type == NULL) )
     {

   if ( (potted_red_wrong_flag==YES ) or (potted_yellow_wrong_flag==YES)  )
     {

		foul_occured = CHOOSE_COLOUR;

      if ( ball[WHITE_BALL].active == YES )
       { 
        hit_yellow_ball_flag=NO;
        potted_red_wrong_flag=NO;
        potted_yellow_wrong_flag=NO;
        draw_ball_type_selecter();
         potted_good_ball_flag=YES;
        return;
       }
      else
       {
         white_ball_gone();
         moved_cue_ball_to_head_string_flag=YES;
         miss_white=NO;
         ball_in_hand_flag=YES;
         foul_occured = CHOOSE_COLOUR;

//         foul_occured=POT_WHITE_WRONG_BALL_FOUL_CHOOSE_COLOUR;

			swap_turn=YES;
			Two_Shots=2;
			Swap_Player();
         num_frames_camera_still = 0;
         Render();
         num_frames_camera_still = 0;
         Render();
         hit_yellow_ball_flag=NO;
         potted_red_wrong_flag=NO;
         potted_yellow_wrong_flag=NO;
         draw_ball_type_selecter();
		  // if ( SNOOKER_MODE == NO )
         return;
        }
      }
	 }

/* ----------------------------------------- */

	ball_in_hand_flag=NO;

/* check missed all balls */

	if	(missed_all_balls_flag==YES)	
    {
     if ( SNOOKER_MODE == YES )
      {
       foul_occured = MISSED_BALL_NO_BALL_IN_HAND;
       Add_Score_2_other_player( 4 );
		}
     else
     {

      if ( GAME_TYPE == KILLER_POOL )
       {
        foul_occured = MISSED_BALL_NO_BALL_IN_HAND;

	    if	(break_flag == NO) 
        {
         last_killer_turn = killer_turn;
         foul_occured=LOSE_LIFE_FOUL;
        }
		}
     else
      {

       if ( GAME_TYPE == US_8BALL)
        {
         foul_occured = MISSED_BALL_NO_BALL_IN_HAND;
		  }
       else
        {
         foul_occured = MISSED_ALL_BALLS_FOUL;
		  }
		}

     if ( (GAME_TYPE == US_POOL) or ( GAME_TYPE == UK_POOL) or ( GAME_TYPE == NMS_POOL ) )
      {
       ball_in_hand_flag=YES;
      }
 	
    }

    swap_turn=YES;
    two_shots_flag=YES;
	 good_shot_flag=NO;
	}

/* --------------------------------------------------------- */
/* when in snooker mode check the fouling fouls or good pots */

   if ( SNOOKER_MODE == YES)
    {
     check_ball=1;
     while	( ball[check_ball].active==NO )
      {
       check_ball++;
      }

/* check to see if all reds are potted, if so then don't check */

     if ( check_ball < 16 )
      {
       if ( SNOOKER_SHOOT_MODE == POT_RED_BALL ) 
        {
/* player must pot a red mode */

         if ( potted_good_ball_flag==YES)
          {
           SNOOKER_SHOOT_MODE = POT_COLOUR_BALL;

/* allow player to select a colour */

           foul_occured = CHOOSE_COLOUR_SNOOKER;
//g           snooker_select_colour();
           foul_occured =NULL;
          }
         else
          {
           swap_turn=YES;
           respot_ball();
          }
        }
       else
/* snooker pot mode must be in pot a coloured ball mode */
        {

         SNOOKER_SHOOT_MODE = POT_RED_BALL;
         if ( potted_good_ball_flag==NO)
          {
           swap_turn=YES;
          }
         respot_ball();
        }
      }
	 }

/* --------------------------------------------------------- */

/* check white ball potted */

    moved_cue_ball_to_head_string_flag=NO; /* flag to say smoothly move
                                                camera to cue ball in head
                                                string */
	if	( ball[WHITE_BALL].active==NO )	
    {
     moved_cue_ball_to_head_string_flag=YES;
     miss_white=NO;
     ball_in_hand_flag=YES;
     two_shots_flag=YES;
     white_ball_gone();

     if	( GAME_TYPE == KILLER_POOL )	
      {
       miss_white=1-KILLER_IN_OFF;
	   }

     if	(miss_white==NO)	
      {
       foul_occured = POT_WHITE_WRONG_BALL_FOUL;
       swap_turn=YES;

       if ( (GAME_TYPE == US_POOL) or (GAME_TYPE == UK_POOL) or (GAME_TYPE == NMS_POOL) )
        ball_in_hand_flag=YES;

       good_shot_flag=NO;

/* respot any wrongly potted balls*/
       respot_ball();
      }
	 }

/* check missed rail in US pool */

	if	( (white_hit_cushion == NO) && (cushion_hit == NO) && (potted_flag==NO) && (GAME_TYPE==US_POOL) && (SNOOKER_MODE==NO) )	
    {

     if ( GAME_TYPE == US_8BALL )
      {
       foul_occured = MISSED_RAIL_US8BALL; /* set us foul with no ball in hand */
      }
     else
      {
       foul_occured = MISSED_RAIL_FOUL;
      }

     swap_turn=YES;

	if ( (GAME_TYPE == US_POOL) or ( GAME_TYPE == UK_POOL) or ( GAME_TYPE == NMS_POOL ) )
     ball_in_hand_flag=YES;

     two_shots_flag=YES;
     good_shot_flag=NO;
    }

/* check missed rail in US 8 BALL pool */

	if	( (white_hit_cushion == NO) && (cushion_hit == NO) && (potted_flag==NO) && (GAME_TYPE==US_8BALL) 
       && ( MUST_HIT_RAIL == NO ) )	
    {

     if ( GAME_TYPE == US_8BALL )
      {
       foul_occured = MISSED_RAIL_US8BALL; /* set us foul with no ball in hand */
      }
     else
      {
       foul_occured = MISSED_RAIL_FOUL;
      }

     swap_turn=YES;

	if ( (GAME_TYPE == US_POOL) or ( GAME_TYPE == UK_POOL) or ( GAME_TYPE == NMS_POOL ) )
     ball_in_hand_flag=YES;

     two_shots_flag=YES;
     good_shot_flag=NO;
    }

/* check 9 ball lowest ball not hit first */

  if ( lowest_ball_not_hit_first == YES )
   {
     game_over = NO;
     foul_occured = LOWEST_BALL_NOT_HIT_FIRST;
     swap_turn=YES;
     if ( (GAME_TYPE == US_POOL) or ( GAME_TYPE == UK_POOL) or ( GAME_TYPE == NMS_POOL ) )
      {
       ball_in_hand_flag=YES;
		}
     two_shots_flag=YES;
     good_shot_flag=NO;
     respot_ball();
   }

/* check hit black first */

	if	(hit_black_first_flag==YES)	
    {

     if ( GAME_TYPE == US_8BALL)
      {
/* setup message to save HIT BLACK FOUL, NO BALL IN HAND */
       foul_occured = HIT_BLACK_NO_BALL_IN_HAND;
      }
     else
      {
       foul_occured = HIT_BLACK_FOUL;
		}

     swap_turn=YES;
     if ( (GAME_TYPE == US_POOL) or ( GAME_TYPE == UK_POOL) or ( GAME_TYPE == NMS_POOL ) )
      {
       ball_in_hand_flag=YES;
		}
     two_shots_flag=YES;
     good_shot_flag=NO;
    }

/* check to see if hit yellow ball instead of red ball first */

	if	(hit_yellow_ball_flag==YES)	
    {

     if	(UK_OR_US_BALL_SET==NO) 
      {
       foul_occured=HIT_YELLOW_WRONG_BALL_FOUL;
      }
     else
      {
       foul_occured=HIT_SPOT_WRONG_BALL_FOUL;
      }
  
     if ( SNOOKER_MODE == YES )
      {
       foul_occured=HIT_RED_BALL_FOUL_NO_IN_HAND;
      }

     swap_turn=YES;

     if ( (GAME_TYPE == US_POOL) or ( GAME_TYPE == UK_POOL) or ( GAME_TYPE == NMS_POOL ) )
      {
       ball_in_hand_flag=YES;
		}

     two_shots_flag=YES;
     good_shot_flag=NO;
    }

/* check to see if hit red ball instead of yellow ball first */

	if	(hit_red_ball_flag==YES)	
    {
     if	(UK_OR_US_BALL_SET==NO) 
      {
       foul_occured=HIT_RED_BALL_FOUL;
      }
     else
      {
       foul_occured=HIT_STRIPE_WRONG_BALL_FOUL;
      }

     if ( SNOOKER_MODE == YES )
      {
       foul_occured=HIT_COLOURED_BALL_FIRST;
      } 

     swap_turn=YES;

     if ( (GAME_TYPE == US_POOL) or ( GAME_TYPE == UK_POOL) or ( GAME_TYPE == NMS_POOL ) )
      {
       ball_in_hand_flag=YES;
		}

     two_shots_flag=YES;
     good_shot_flag=NO;
    }

/* check potted wrong ball */

	if	(potted_red_wrong_flag==YES)	
    {

     if	(UK_OR_US_BALL_SET==NO) 
      {
       foul_occured = POT_WRONG_BALL_FOUL;
      }
     else
      {
       foul_occured = POT_9_BALL_WRONG_FOUL;
		}

     swap_turn=YES;

     if ( (GAME_TYPE == US_POOL) or ( GAME_TYPE == UK_POOL) or ( GAME_TYPE == NMS_POOL ) )
      {
       ball_in_hand_flag=YES;
		}

     two_shots_flag=YES;
     good_shot_flag=NO;

/* respot any wrongly potted balls */

     respot_ball();
    }

/* check potted wrong ball */

	if	(potted_yellow_wrong_flag==YES)	
    {

     if	(UK_OR_US_BALL_SET==NO) 
      {
       foul_occured = POT_YELLOW_WRONG;
      }
     else
      {
       foul_occured = POT_8_BALL_WRONG_FOUL;
		}
     swap_turn=YES;

     if ( (GAME_TYPE == US_POOL) or ( GAME_TYPE == UK_POOL) or ( GAME_TYPE == NMS_POOL ) )
      {
       ball_in_hand_flag=YES;
		}

     if ( SNOOKER_MODE == YES )
      {
       foul_occured = POT_COLOUR_BALL_WRONG_FOUL;
		}


     two_shots_flag=YES;
     good_shot_flag=NO;

/* respot any wrongly potted balls*/

     respot_ball();
    }

/* check potted black wrong ball */

	if	( potted_black_wrong_flag==YES )
    {

     if ( (GAME_TYPE == US_POOL) or ( GAME_TYPE == UK_POOL) or ( GAME_TYPE == NMS_POOL ) )
      {
       ball_in_hand_flag=YES;
		}

     two_shots_flag=YES;
     good_shot_flag=NO;

/* respot any wrongly potted balls*/

     respot_ball();

/* check to see if black was re-spotted, if aso then change foul message */

     foul_occured = POT_BLACK_WRONG_FOUL;
	  check_ball=0;
     while	( ball[check_ball].obj_type != BLACK ) check_ball++;

     if	( ball[check_ball].active == NO) 
      {
       foul_occured = POT_BLACK_WRONG_FOUL_OTHER_PLAYER_WINS;
		}
    }

/* check time penalty */

   if	( (GAME_TYPE==SPEED_POOL) && (SPEED_PENALTY_ON_FOULS == NO) && (swap_turn==YES) )	
    {
	  good_shot_flag=NO;
	  for	(time=0;time<15;time++)	update_timer();
	 }

/* check to see if potted any balls */

   if	(potted_hi_ball_flag + potted_good_ball_flag == NO) 
    {
     swap_turn=YES;
     if	(Two_Shots > 0)	
      {
       Two_Shots--;
		 if ( Two_Shots==0)	draw_foul_shot_icons = NO;
      }
    }

/* check two shots */

	if	(two_shots_flag==YES)	
    {
	  if	( (UK_1_SHOT_CARRY==NO) && (GAME_TYPE==UK_POOL) && ( SNOOKER_MODE == NO) )	
      {
/* check two shots on black */
       miss_it=NO;
       if	( (players[1-Player_Active].ball_type == BLACK)
              && (UK_TWO_SHOTS==YES) ) miss_it=YES;

       if	( (miss_it==NO)  && ( game_over==NO ) )
        {
			draw_foul_shot_icons = YES;
			swap_turn=YES;
			Two_Shots=2;
			Swap_Player();
		  }
		}
	}


/* check two shots */

	if	(two_shots_flag==YES)	
    {
	  if	( (GAME_TYPE==US_8BALL) && ( SNOOKER_MODE == NO) )	
      {
/* check two shots on black */

       if	(  game_over==NO )
        {
			draw_foul_shot_icons = YES;
			swap_turn=YES;
			Two_Shots=2;
			Swap_Player();
		  }
		}
	}

/* check nms pool type game for two shots */

	if	(two_shots_flag==YES)	{
	
	if	( (GAME_TYPE==NMS_POOL))	{

/* check two shots on black */
 
	miss_it=NO;
	if	(players[1-Player_Active].ball_type == BLACK ) miss_it=YES;

		if	(miss_it==NO && game_over==NO)	{

			swap_turn=YES;
			Two_Shots=2;
			Swap_Player();
			}
		}
	}

/* check ball in hand */

	if	( (ball_in_hand_flag==YES) && (game_over==NO) )
    {
	  swap_turn=YES;
    }

/* swap players turn */

	if	(swap_turn==YES)	
    {
     if	(Two_Shots == 0)	Swap_Player();
    }

}
/*--------------------------------------------------------------------------*/						 
void	respot_ball(void)
{
	long	count,loop,index,do_ball;
	long	x,y,x1,y1;		// RESTORE: 8.24, were double

/* don't respot any balls in uk pool */

   if ( GAME_TYPE == UK_POOL ) return;

/* check to see if any balls potted */
	if	(pot_count==0)	return;

/* use potted ball varible */

	index=1;

	for	(count=0;count<pot_count;count++)	
    {

     if ( SNOOKER_MODE == NO ) 
      {
	    for	(loop=1;loop<LAST_BALL;loop++)	
        {
         if	(loop != potted_ball[count])	
          {

/* get position in saved data */
           x=*(balls_xy+index*2);
           y=*(balls_xy+index*2+1);
           x1=ball[loop].x-x;
           y1=ball[loop].y-y;

/* if ball touching another ball then get next position in stored list */

           if	(fix_hypot(x1,y1) <= ball[WHITE_BALL].diameter)	// RESTORE: 8.24
            {
             index++;
             loop=1;	 

/* check to see if any space in pack free, if not  then place ball just in
front of the first ball*/

             if	(index >= LAST_BALL) 	
              {
               x=*(balls_xy+index*2);
               y=*(balls_xy+index*2+1);
               x-=ball[WHITE_BALL].diameter*2;
               loop=1000;
              }
            }
          }
        }
	   } 


   do_ball=NO;

   if ( SNOOKER_MODE == YES ) 
    {
     if ( ball[potted_ball[count]].ball_colour != RED)
      {
        index=ball[potted_ball[count]].num;

        x=*(balls_xy+index*2);
		  y=*(balls_xy+index*2+1);

        do_ball=YES;
      }
    }
   else
   {
   switch	(GAME_TYPE)
    {
     case	UK_POOL:
      if ( ball[potted_ball[count]].num  != 8) break;
      do_ball=YES;
	  	break;

	  case	US_8BALL:
/* only respot 8 ball on illegal pot */
      if ( US_8_RESPOT_ON_ILLIGAL_POT == YES )
       {
        if ( ball[potted_ball[count]].num  != 8) break;
		 }
      do_ball=YES;
	  	break;

	  case	US_POOL:

      if ( ball[potted_ball[count]].num  != 9) break;
      do_ball=YES;
	  	break;

	  case	CONTINUOUS:
	  	break;
	 		
	  case	SPEED_POOL:
	  	break;

	  case	KILLER_POOL:
	  	break;

     case  NMS_POOL:

/* only respot 8 ball on illegal pot */
      if ( NMS_RESPOT_ON_ILLEGAL_POT == YES )
       {
        if ( ball[potted_ball[count]].num  != 8) break;
		 }
      do_ball=YES;
     break;
    }
   }

/* respot ball */
   if ( do_ball == YES )
    {
     init_ball_position(potted_ball[count]);
     ball[potted_ball[count]].x=x;
     ball[potted_ball[count]].y=y;
     ball[potted_ball[count]].x_prev=ball[potted_ball[count]].x;
     ball[potted_ball[count]].y_prev=ball[potted_ball[count]].y;
     ball[potted_ball[count]].z_prev=ball[potted_ball[count]].z;

     check_ball_in_safe_place(potted_ball[count]);

    }
  }
}
/*--------------------------------------------------------------------------*/						 
void	reset_fouls(void)
{
/* reset all foul flags */

	two_shots_flag=NO;
  	missed_all_balls_flag=YES;
  	potted_good_ball_flag=NO;

  	potted_hi_ball_flag=NO;	
	potted_red_wrong_flag=NO;
	potted_black_wrong_flag=NO;
	potted_yellow_wrong_flag=NO;
 	first_foul=NO;

	hit_black_first_flag=NO;
	hit_yellow_ball_flag=NO;
	hit_red_ball_flag=NO;
   lowest_ball_not_hit_first=NO;

/* reset flag to say no balls potted */

 	LOWEST_BALL_HIT_FIRST_FLAG=NO;
 	potted_flag=NO;
	first_hit=NO;
	cushion_hit=NO;
	white_hit_cushion=NO;
}
/* -------------------------------------------- */
void	Swap_Player(void)
{
#define	MAX_PLAYERS_IN_GAME	2

	bonus_pocket = rand();
	bonus_pocket%=6;

/* exit on these conditions */

	if	(GAME_TYPE == SPEED_POOL) {
   old_player_turn = Player_Active;
   return;
	}

/* do killer pool play turns */

	if	(GAME_TYPE == KILLER_POOL)	{
	return;
	}

   used_free_ball = NO;

/* -------------------------------------------- */
	
	COMPUTER_TURN=NO;
	old_player_turn=Player_Active;
   old_turn_count=2;

/* clear and draw new score */

	Player_Active++;
	Player_Active%=MAX_PLAYERS_IN_GAME;

/* make players name flash to show turn */
   setup_flash_name();

	show_panel=PANEL_ON_TIME;
   draw_twice=2;
   if ( RESOLUTION==LOW)  refresh_whole_screen=YES;
}
/* -------------------------------------------- */
void setup_flash_name(void)
{
   if ( Player_Active == 0) 
    {
     flash_name_1=FLASH_NAME_TIME;
     flash_name_2=0;
    }
   if ( Player_Active == 1) 
    {
     flash_name_1=0;
     flash_name_2=FLASH_NAME_TIME;
    }
}
/* -------------------------------------------- */
void  check_ball_in_safe_place( long ballnum )
{
long loop,rad;
long x,y,x1,y1;		// RESTORE: 8.24, were double
int angle;

  angle=0;
  rad=1;

  for ( loop = 0 ; loop < LAST_BALL ; loop++)
   {
	 if ( loop != ballnum )
     {
       x = ball[ballnum].x;
       y = ball[ballnum].y;
		 x1 = ball[loop].x - x;
		 y1 = ball[loop].y - y;

/* test touching ball */

       if ( fix_hypot(x1,y1) <= ball[ballnum].diameter )	// RESTORE: 8.24
        {
// RESTORE: angle counts 4096 to the turn (it steps by 1024), but NMS took it
// as degrees (1024 deg, 2048 deg...); slCos/slSin of the SGL angle angle*16
// move the ball a diameter in the four directions meant
          ball[ballnum].x -= mul64(slCos((ANGLE)(angle<<4))<<8, ball[ballnum].diameter);
          ball[ballnum].y -= mul64(slSin((ANGLE)(angle<<4))<<8, ball[ballnum].diameter);

          angle+=1024;
          angle&=4095;

			 loop=0;
        }
      }  
	}

   ball[ballnum].x_prev=ball[ballnum].x;
	ball[ballnum].y_prev=ball[ballnum].y;

}

