/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/*									    	 QD-UP      										 */
/*											PC-CD ROM											 */
/*									 For BMG	Interactive										 */
/*									 By NMS Software Ltd.									 */
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

#include	"globals.h"


flag exit_end_of_frame;
flag setup_default_angle=NO, show_last_shot = NO ;
flag debounce_track=NO;
long speed_val_index,old_lines , save_9_ball_count_for_undo_shot ;
long save_Break_Flag ;
long save_ball_in_hand;
long save_draw_foul;
long save_Player_Active;
long demo_time;
long undo_shot_pos,shot_pos;
double last_mouse_ydiff;
long work_shot_position;
long save_draw_foul_shot_icons;
int save_undo_camz;

/* ---------------------------------------------------------------- */
/* include this to create new demo data file, after playing through
one frame */
//#define CREATE_NEW_DEMO
/* ------------------------------------------------------------------- */
typedef struct
        {
			long x[22];			// RESTORE: 8.24 (was 16.16 made from the doubles)
			long y[22];
			long ax[22];
			long ay[22];
			long az[22];
         long	active[22];				// is ball moving?
		  } pool_undo;

#define MAX_UNDO_LIST 300
pool_undo pool_undo_buffer[ MAX_UNDO_LIST ];

/* ------------------------------------------------------------------- */
/* end of frame data structure */
typedef struct
        {
			 double steves_angle;
          double speed_meter;
          long x_pos[22];		// RESTORE: 8.24 ball positions, were double
          long y_pos[22];

          long butt;
          long x_hit_pos;
          long y_hit_pos;
          long pot_shot;
          int  cue_angle;
          unsigned char	active[22];				// is ball moving?

		  } shot_positions_table;

#define  MAX_SHOT_STORE  100
shot_positions_table save_shots[ MAX_SHOT_STORE ];

long save_count;
/* ------------------------------------------------------------------- */
typedef struct
        {
          double	x;
          double	y;
          double	z;
          double	ax;
          double	ay;
          double	az;
          double	x_prev;
          double	y_prev;
          double	z_prev;
          long potted_flag;
          unsigned char	active;				// is ball moving?
          long  nine_ball_count;         
		  } SAVE_SHOT;

	 SAVE_SHOT *replay_ptr;
/* ------------------------------------------------------------------- */
void Run_Game(void)
{
	text	file[80];
   long  a;

/* start FUN room game track */

   setup_CD_track_for_fun_tables();

   wrapper_flag=NO;
/* clear any pressed keys from buffer */
//g   flush_keys();

/* setup control panel default to off */
   panel_scrolly=25;
   panel_flag=NO;

   end_of_frame_flag=NO;
   exit_end_of_frame=NO;
   show_replay_flag = NO;
   replay_flag=NO;

   In_Game_Ram_Init();
   frame_counter=0;
   cheat_buffer[0]=0;
   cheat_pos=0;
   camera_frame_count=0;
   start_time=NO;
   quitkey=NO;

// RESTORE: was 10.  room_palettes and poolpal_tab (LEVELDAT.C) hold 11
// entries per baize colour (potters, rackers, hall, german, snooker, moon,
// ice, beach, crypt, swamp, toy), so with 10 a green or red baize
// (TABLE_TYPE 1, 2) read every entry one place early (the first, Potters
// green, came out as the blue toy room's).  GAME.CCC, the PC version's GAME.C,
// has 11.
   NUMBER_OF_LOCATIONS=11;
   BUILD_HIRES_TEXT=YES;
   BUILD_HIRES_NUMS=YES;
   BUILD_LOW_TEXT=YES;
   BUILD_LOW_NUMS=YES;
   BUILD_LOW_TEXT_FOUL = YES;
   BUILD_HIRES_TEXT_FOUL = YES;

/* reset tracking lines */

   activated_lines=FALSE;
   reset_fouls();
   delay_after_shot=0;

/* setup ball information */

   if ( SNOOKER_MODE == YES )
    {
     cue_weight= .037;
    }
   else
    {
     cue_weight= .0245;
    }

//   cue_scale = .002;

//g	if	(RESOLUTION==HIGH)	
//g    {
//g     viewport_width=SVGA_logical_length; /* setup x resolution */
//g	  frame_rate=65.0;	 /* setup frame rate for ball machanics */
//g    }
//g	else	
//g    {
//g     viewport_width=320;	/* setup x resolution */
//g	  frame_rate=65.0;      /* setup frame rate for ball machanics */
//g	 }

	frame_rate = 50;

// RESTORE: 3D view, the owner's decision.  The break camera showed the empty
// near half of the table while every ball was off the top of the screen:
// STV3D.C (698) keeps the balls in view only when zoom_flag (General_Rules[17])
// is YES, and on the PC only the F11 key set it.  It is on, with NMS's own
// //g line above, viewport_width = 320, which the PS1 port left out and which
// zoom_out (STV3D.C, 937) compares ball positions with: at 0 every ball with
// x > 0 counted as off screen and the camera always zoomed out.
	viewport_width = 320;
	zoom_flag = YES;

/* setup if game type is a points game */

   setup_points_game();

   setup_table_friction();

/* set game to points type if in snooker mode */

   if ( SNOOKER_MODE == YES )
    {
     points_game=NO;
	 }

	height=200;

/* load in shade tables for game panel */
	
   sprintf( file, "%s//%s",drive_and_path,room_palettes[LOCATION+(TABLE_TYPE*NUMBER_OF_LOCATIONS)] );
   load_file_here(file, &pallettebuf, 768 );
   make_shade(shad_tab,100);

/* reset number of frames won */

	Frames_played[0][0]=0;
	Frames_played[0][1]=0;

/* reset score data */

	for	(a=0;a<16;a++)	players_scores[a]=0;
	for	(a=0;a<32;a++)	players_total_scores[a]=0;

/* load all in game graphics into temporary */

   load_all_game_graphics_from_disk();

/* load game sound effects */

//g   clear_loaded_samples();
//g   load_samples("sounds\\samples.txt");

/* ------------------------------ */
/* setup and play trick shot loop */
	
   if ( show_trick_shot == YES )
    {
     GAME_TYPE=TRICK_SHOTS;
	  show_andplay_trickshot_loop();
     show_trick_shot=NO;
	  return;
    }

/* ---------------------------- */
/* setup show saved replay shot */

	if	(show_saved_shot_flag==YES)
	 {
     show_saved_shot_loop();
     reset_atract_counter();
     return;
	 }

/* setup balls for current game type */

	Setup_New_Game_Type();

/* initialise Escher 3d engine */

	init_3d();

/* ------------------------------ */

   if ( DEMO_PLAY == YES )
    {
     end_of_frame_control();
     return;
	 }

/* setup cd track for this game type */

   if (DEMO_PLAY == NO)   start_game_track();
					 
/* play 1 set of number of frames set in rule book */

   Play_Single_Game();

/* --------------------------------------------- */
/* update player stats, last total score varible */

	player_info[left_player].last_total_score=players_total_scores[left_player];
	player_info[right_player].last_total_score=players_total_scores[right_player];

/* -------- */
/* end game */

	reset_atract_counter();
}
/* ------------------------------------------------------------------------ */
long	KEYBOARD=NO,keyboard_frac;
void	check_all_game_events(void)
{
#define	SPEED_UP_FRAC	12
#define	MAX_FRAC			300

long miss_it;
/* dont allow any action why camers is moving */

   if ( DEMO_PLAY == YES ) return;

/* check to see if you want to turn off foul icons */

	if	( (E_FinishedEase()==NO) ) return;

    event_flag=ROTATE_CAMERA_VIEW;

   if (getkey(kV)==YES)
    {
     if ( foul_occured != MOVE_CAMERA_MODE)
     {
	   save_move_foul=foul_occured; 
      foul_occured=MOVE_CAMERA_MODE;
	   save_foul=MOVE_CAMERA_MODE;
     }         
      event_flag=MOVE_CAMERA;
      return;
    }
   else
    {
      if ( save_foul == MOVE_CAMERA_MODE )  foul_occured= save_move_foul;
    }

	KEYBOARD=NO;

   miss_it=NO;
   if ( GAME_TYPE == CONTINUOUS )
    {
	  if ( pocket_nominated == -1 ) miss_it=YES;
    }

   if	( (replay_flag == NO) && ( nominate_colour == NO ) && (miss_it==NO))	
    {
     if	( (joy1_is_left==YES) || (getkey(kLARROW)==YES) ) { mouse_xdiff=-(abs(keyboard_frac));KEYBOARD=TRUE; }
     if	( (joy1_is_right==YES) || (getkey(kRARROW)==YES) ) {mouse_xdiff=abs(keyboard_frac);KEYBOARD=TRUE;}

     if	( (joy1_is_up==YES) || (getkey(kUARROW)==YES) ) {mouse_ydiff=-(abs(keyboard_frac));KEYBOARD=TRUE;}
     if	( (joy1_is_down==YES) || (getkey(kDARROW)==YES) ) {mouse_ydiff=abs(keyboard_frac);KEYBOARD=TRUE;}
    }

	if	(KEYBOARD==TRUE)	
    {
     if	(keyboard_frac	< MAX_FRAC) keyboard_frac+=SPEED_UP_FRAC;
    }
	else	
    {
	  if	(keyboard_frac	> 0) keyboard_frac-=SPEED_UP_FRAC*2;
	 }

	if	(mouse_buttons == LEFT_BUTTON) 
    {
     if ( foul_occured != CAMERA_ZOOM_MODE)
      {
	    save_zoom_foul = foul_occured ;
       foul_occured = CAMERA_ZOOM_MODE;     
       save_foul = CAMERA_ZOOM_MODE;     
      }
     event_flag=CAMERA_ZOOM;
     return;
	 }
   else
    {
	  if ( save_foul == CAMERA_ZOOM_MODE)   foul_occured = save_zoom_foul;
    }

   if (nominate_colour==YES) return;

/* check all link types */

	if	(LINK_OPTION!=NULL)	
    {
     if	( (Player_Active == 0) && (master==SLAVE) ) return;
     if	( (Player_Active == 1) && (master==MASTER) ) return;
    }

/* stop all cue ball action until camera ready */

   if ( shoot_mode_availble == NO ) return;
   if ( COMPUTER_TURN==YES ) return;

     if	(getkey(kE)==YES)	
       { 
 		  if ( foul_occured != APPLY_ENGLISH )
         {
		    save_english_foul=foul_occured ;
          foul_occured = APPLY_ENGLISH;
          save_foul = APPLY_ENGLISH;
         }
          event_flag=ADD_MASSE;
          return;
       }
     else
      {
	    if ( save_foul == APPLY_ENGLISH ) foul_occured = save_english_foul;
      }

/* dont allow any of these events to happen if other players turn in link */

//	if ( draw_cue == YES)
//    {

     if	( (ball_in_hand_flag==YES) && ( show_shot_flag==NO) && (getkey(kM)==YES) )
      {
       if ( foul_occured != MOVING_BALL )
        {
		   save_hand_foul=foul_occured ;
         foul_occured = MOVING_BALL;
         save_foul = MOVING_BALL;
        }
       event_flag=MOVING_BALL_IN_HAND;
       return;
		}
     else
      {
	    if (save_foul == MOVING_BALL)  foul_occured=save_hand_foul;
      }

     if	(getkey(kB)==YES)	
      {
       if ( foul_occured != RAISE_BUTT)
        {
		   save_butt_foul=foul_occured ;
         foul_occured = RAISE_BUTT;
         save_foul = RAISE_BUTT;
        }
       event_flag=RAISE_BUTT_OF_CUE;
       return;
      }
     else
      {
	    if ( save_foul == RAISE_BUTT ) foul_occured=save_butt_foul;
      }

     save_foul=0;
		
     if	( (getkey(kS)==YES) or (mouse_buttons == RIGHT_BUTTON) && (replay_flag==NO) ) 
      {
       event_flag=TAKING_SHOT;
       foul_occured = READY2_SHOOT_MODE;
       save_foul=0;
      }

    if ( (foul_occured ==  READY2_SHOOT_MODE) or 
         (foul_occured ==   AMPLIFY_SHOOT_MODE)  )
     {
      if ( amplify )
       {
        foul_occured = AMPLIFY_SHOOT_MODE;
       }
      else
       {
        foul_occured = READY2_SHOOT_MODE;
       }
	  }

  //  }


}
/* ------------------------------------------------------------------- */
void Play_Single_Game(void)
{
	long  a;

/* setup number of frames to play in tournament round */

	max_frame = NUMBER_OF_FRAMES+1;
	switch ( GAME_TYPE) 
    {
	  case KILLER_POOL:
      max_frame=1;
      break;

	  case SPEED_POOL:
      max_frame=1;
      break;
	 }

	if (TOURNAMENT_PLAY == YES )
    {
/* setup number of frames to play through the tournament */
     switch ( TOURNAMENT_ROUND )
      {
       case 0:
        max_frame=2;
		  break;
       case 1:
        max_frame=3;
		  break;
       case 2:
        max_frame=4;
		  break;
       case 3:
        max_frame=5;
		  break;
      }
    }
   
/* loop until all frames played */

	exit_frame=NO;

	while	(exit_frame==NO)	
    {

     undo_shot_pos=0;
	  reset_aiming_varibles();

     table_position=0;
     lines_on=NO;
	  oldfoul=-1;
     save_foul=-1;
     save_move_foul=-1;
     save_hand_foul=-1;
     save_zoom_foul=-1;
     save_english_foul=-1;
     save_butt_foul=-1;

	  Re_Draw_Screen();

/* run game loop, and play number of frames */

	  play_one_frame();

/* add all of score from last frame */

	  players_scores[0]+=total[0][0];	
	  total[0][0]=0;
	  players_scores[1]+=total[0][1];	
	  total[0][1]=0;

/* fade out last game correctly */

	  for	(a=0;a<12;a++)			serial_transfer();

	 if	(LINK_OPTION!=NULL)	
     {
      if (extern_framequit==YES) exit_frame=YES;
	  }

/* add 1 to players frame counter */

     Frames_played[0][Player_Active]++;
     show_replay_flag = NO;

/* setup balls for this game type */

    Setup_New_Game_Type();
    setup_default_camera_position(0,YES);

    if	((Frames_played[0][0] > max_frame-1 ) || (Frames_played[0][1] > max_frame-1 ) ) { 


	 if	(exit_frame==NO)	
     {
      add_sample(PANDAM, 0);

	   switch	(GAME_TYPE)
       {
        case	KILLER_POOL:
  		   SVGA_set_mode(0x101);
		   if	(end_of_killer_game==YES)
		   end_of_killer_screen();
		   break;

		   case	SPEED_POOL:
		   break;

		   default:

		   if ( (points_game == YES) && (SNOOKER_MODE==NO) )
          {

/* only show end of frame screen if game type uses points system */

		     end_of_frame_screen();

		     if	(exit_frame==0) 
            {
			    load_all_game_graphics_from_disk();
			   }
          }
			break;
		 }
    }

    exit_frame=YES; 

    }

    }

}
/* ------------------------------------------------------------------- */
void	play_one_frame(void)
{
/* reset flag for old message type */

	replay_avalible = NO;
   end_of_frame_flag=NO;
   shot_pos=0;

/* do game loop until all frames played */

	draw_clock=YES;
	speed_pot=0;
	game_over=NO;
   shot_position=0;

/* main game loop */

	while   (1)
    {
/* control loop for setting up shot */

     show_last_shot=NO;

     if ( (set_target_for_white()==TRUE) ) break;

     if ( (Show_Shot()==TRUE) )   break;

     save_shots[shot_position].pot_shot = potted_flag;

     if ( shot_position < MAX_SHOT_STORE)  shot_position++;

/* update all game fouls after shot */

     used_free_ball=YES;


   if (show_last_shot==NO)   
    {
       if ( update_fouls()==TRUE ) 
        {
         end_of_frame_control();
         break;
        }
    }

/* reset break flag */

    break_flag=NO;

	 if (game_over==YES) break;
	}
}
/* ------------------------------------------------------------------- */
void  end_of_frame_control(void)
{
  long	a;

  if (FRAME_REPLAYS==YES) return;

  lines_on=NO;
  activated_lines=FALSE;
  exit_end_of_frame = NO;
  shot_taken=NO;

  show_replay_flag = YES;
  end_of_frame_flag=YES;

  setup_resolution();

/* setup camera for first time */

  setup_show_potted_balls();
  replay_flag=YES;

  while   ( 1 )
	{
    show_panel = 10;

/* check replay last shot ? */

    Show_potted_balls();

/* only play one round if in demo mode */

    if ( exit_end_of_frame==YES ) break;

    }

/* make sure speed in good range */

   quit_flash_count=0;
   interlace=1;
   Handle3D();

	computer_first_shot=YES;
   show_replay_flag = NO;
	good_shot_flag=YES;
   replay_flag=NO;
   end_of_frame_flag=NO;
   setup_resolution();
   show_shot_flag = NO;

}
/* ------------------------------------------------------------------- */
flag set_target_for_white( void )
{
 	long	l,loop;
	double steves_angle;
	double tmp_angle;
	double butt_conv;

/* setup message to say in AIM MODE if no foul commited */

   if ( foul_occured == 0 ) foul_occured=AIM_MODE;

	reset_aiming_varibles();
   potted_flag=NO;

   if ( break_flag == NO  )
    {

/* re aline camera after every shot */

     if ( zoom_after_shot_flag == YES )
      {
       camera_mode=-1;
       Move_Camera_Position(ZOOM_BACK_TO_CUE_BALL);
      }
     else
      {
       camera_mode=-1;
       Move_Camera_Position(MOVE_BACK_TO_CUE_BALL);
      }
    }
   else
    {
/* setup camera position for first time */

     if  (LOCATION > DIFFERENT_SHAPED_TABLES_INDEX-1)
      {
       camera_mode=-1;
       Move_Camera_Position(ZOOM_BACK_TO_CUE_BALL);
      }
     else
      {
       setup_default_camera_position(0,NO);
      }
    }

/* check to see if computers turn */

#ifdef	QDUP_TEST_CPU_BOTH
      if	(1)	// RESTORE: test builds only (tests/qdup-ai): the computer plays both sides
#else
      if	(Player_Active == 1) 
#endif
       {
        if	(PLAYERS_SELECTED==2)	
       {
       COMPUTER_TURN=YES;
      }
    }

/* setup players ball type, for checking potted 1 of each ball sets */

   last_time_ball_type = players[0].ball_type;

/* check to see if computer is on */

   sync_stage();

/* setup players shot up */

   if	(COMPUTER_TURN==NO)   players_shot();

/* setup computer shot */

   if	(COMPUTER_TURN==YES)	 computer_shot();

   if	(quitgame==YES)  	return	TRUE;

   if	(LINK_OPTION !=NULL)	
    {
     if	(extern_quit == YES) 	return	TRUE;
    }

/* update player stats, Shots Taken varible  */

   if	(Player_Active==0)	
    {
     player_info[left_player].shots_taken ++;
    }
   else	
    {
     player_info[right_player].shots_taken ++;
    }

   if	(Player_Active==0)	
    {
     player_info[left_player].avg_shot_power=(long)speed_meter;
    }
   else	
    {
     player_info[right_player].avg_shot_power=(long)speed_meter;
    }

    show_shot_flag=1;

/* set flag to say start timer for speed pool */

    start_time=YES;

/* Update white ball speed using andy's maths */

    steves_angle= cue_angle;
    speed_meter=fabs(cue_delta_table[0]);

    replay_cue_angle = cue_angle;

/* save all shots , so that we can replay them at end of frame */

//g    save_shots[shot_position].cue_angle = cue_angle;
//g    save_shots[shot_position].steves_angle = cue_angle;
//g    save_shots[shot_position].butt = butt;
//g    save_shots[shot_position].speed_meter = speed_meter;
//g    save_shots[shot_position].x_hit_pos = x_hit_pos;
//g    save_shots[shot_position].y_hit_pos = y_hit_pos;

// RESTORE: were //g.  Show_Undo_Shot puts the camera back from these
// (GAME.C below); without them it read zeros.
    save_undo_camera_rot=	camera_rot;  
    save_undo_camera_pos=	camera_target;
    save_undo_cue_rot=	cue_rot;     
    save_undo_camz = camz;

//g   for (loop=0;loop<LAST_BALL;loop++)
//g    {
//g     save_shots[shot_position].x_pos[loop] = ball[loop].x;
//g     save_shots[shot_position].y_pos[loop] = ball[loop].y;
//g     save_shots[shot_position].active[loop] = ball[loop].active;
//g	 } 

//(steves_angle*2*PI)/360.0;

	hit_ball(WHITE_BALL, x_hit_pos, y_hit_pos, (double)steves_angle , (double)butt , DOUBLE_TO_FIX24(speed_meter));	// RESTORE: 8.24

/* save shot for replay mode */

    for	(l=0;l< LAST_BALL;l++)	
     {
      replayball[l] = ball[l];
     }

// RESTORE: were //g, with replay_avalible: the PC version (GAME.CCC, 698) has
// them on, and Show_Replay needs them to play the shot again.
	 replay_pan_angle=steves_angle;
	 replay_butt=butt;
	 replay_speed_meter=speed_meter;
	 replay_x_hit_pos=x_hit_pos;
	 replay_y_hit_pos=y_hit_pos;
  	 replay_avalible=YES;

   return	FALSE;
}
/* ------------------------------------------------------------------- */
char control_demo(void)
{
  if (demo_time == 0) return YES;

  demo_time--;
  return FALSE;
}
/* ------------------------------------------------------------------- */
/* update cue position relative to mouse movement */
void Update_Cue_Position( void )
{
static	long save_frame;
double oldsp=cue_position,cue_speed;
int speed;

   
   if ( COMPUTER_TURN == YES) return;
   if ( show_shot_flag == YES ) return;

   if ( GAME_TYPE == CONTINUOUS )
    {

/* check to see in if a pocket as been nominated, if not then dont allow
player to tack shot */

	  if ( pocket_nominated == -1 ) {
      foul_occured=CHOOSE_POCKET;

      return;
	  }
     foul_occured=0;
    }

/* check ampify shot */
   
   if ( ( (getkey_jp(kLEFTSHIFT )==YES) or (getkey_jp(kRIGHTSHIFT )==YES) ) && (DEMO_PLAY==NO) ) 
      {
       amplify^=1;

/* re-draw whole screen to erase icon if turned off */
       if (amplify == NO) refresh_whole_screen=2;
		}

   if ( amplify == NO ) cue_scale = .002;
    else cue_scale = .0045;

/* dont allow shot to be taken if camera moving */

   if ( shot_taken == YES ) return;

   speed = frames - save_frame;
   save_frame = frames;

   cue_speed= ( save_frame * mouse_ydiff ) / save_frame;

	if	(LINK_OPTION!=NULL)
    {
     if	( (Player_Active == 0) && (master==SLAVE) )	
      {
       speed_meter =	extern_cue_pan[1];
       return;
	  	}

		if	( (Player_Active == 1) && (master==MASTER) )
       {
        speed_meter = extern_cue_pan[1];
        return;
       }
    }

/* check to see if player holding down 'S' key */

	if	(event_flag!=TAKING_SHOT) return;


   if ( cue_speed > 0)
    {
     if ( cue_position < 3.0) 
      cue_position+=cue_speed * cue_scale;
	 }
   else   cue_position+=cue_speed * cue_scale;

/* save cue postions for replay last shot mode */

   cue_position_table[cue_position_table_ptr]=cue_position;
   cue_position_table_ptr++;
   if ( cue_position_table_ptr  > 199) cue_position_table_ptr=0;

   last_mouse_ydiff=cue_speed;

/* take shot at current cys setting */

   if ( (altkey==YES) && (shootkey==YES) ) 
    {
     last_mouse_ydiff=cue_position*line_length;
     cue_position=0;
	 }

/* update screen if cue moved */

	if	(oldsp != cue_position)  
    {
     refresh_whole_screen=2;
     reset_tracking_lines();
    }
}
void Update_English_On_Cue_Ball(void)
{
	long	temp,temp1;
int x_delta,y_delta,tmp;

/* dont allow player to move cue to be taken if camera moving */

   if ( show_shot_flag == YES ) return;
   if (( E_FinishedEase() == NO )) return;

	if	( event_flag!=ADD_MASSE ) return;

	temp=x_hit_pos;
	temp1=y_hit_pos;

/* get mouse x fraction for x hit pos on english */

   if ( (tmp=mouse_xdiff) < 0) tmp=-tmp;

   if ( tmp > 12)
    {
     x_delta = (mouse_xdiff * 512);
    }
   else
    {
     x_delta = (mouse_xdiff * 256);
    }

/* get mouse y fraction for y hit pos on english */

   if ( (tmp=mouse_ydiff) < 0) tmp=-tmp;
   if ( tmp > 12)
    {
     y_delta = (mouse_ydiff * 512);
    }
   else
    {
     y_delta = (mouse_ydiff * 256);
    }

/* apply new english */

   x_hit_pos+=x_delta>>3;
   y_hit_pos+=y_delta>>3;


/* check all positon on cue ball so that cue never excieds cue ball */

   if (check_ball_hypot(x_hit_pos, y_hit_pos)>64000) y_hit_pos=temp1;

	if (check_ball_hypot(x_hit_pos, y_hit_pos)>64000) x_hit_pos=temp;

	if	( (temp != x_hit_pos) || (temp1 != y_hit_pos) ) 
    {
     refresh_whole_screen=2;
     reset_tracking_lines();
    }
}
/*-----------------------------------------------------------------*/
/* add masse to cue ball */
void	 Raise_Butt_Of_Cue(void)
{
int y_delta,tmp;
	long	temp;
	temp=butt;

/* get mouse y fraction for y hit pos on english */

   if ( (tmp=mouse_ydiff) < 0) tmp=-tmp;
   if ( tmp > 12)
    {
     y_delta = -(mouse_ydiff * 2);
    }
   else
    {
     y_delta = -(mouse_ydiff * 1);
    }

   butt+=y_delta>>1;

	if	( butt < 0 ) butt=0;
	if	( butt > 1024-32 ) butt=1024-32;

	if	( temp != butt )  
    {
     last_butt = butt;
     refresh_whole_screen=2;
     reset_tracking_lines();
	 }
}
/* -------------------------------------------------------------------------*/
/* check to see if cue as hit cue ball */
flag check_hit_cue_ball(void)
{
	if	(event_flag!=TAKING_SHOT) return FALSE;

   if ( COMPUTER_TURN == YES) return FALSE;

   if ( cue_position < FIX24_TO_DOUBLE(ball[WHITE_BALL].radius) )	// RESTORE: 8.24
    {
     if ( amplify== NO) 
      {
       cue_delta_table[0] = (last_mouse_ydiff * cue_weight);
		}
     else
      {
/* calculate speed with amplify set */

       cue_delta_table[0] = (last_mouse_ydiff * (cue_weight*1.2));
		}

/* turn amplify power icon off */
     amplify=NO;
     cue_position = FIX24_TO_DOUBLE(ball[WHITE_BALL].radius);	// RESTORE: 8.24
     shot_taken=YES;
     return TRUE;
    }

   return FALSE;
}
/* ------------------------------------------------------------------- */
flag Show_Shot(void)
{
   long	speed,count,a;
   int	vol;
static	long	frames_passed;

/* play hit cue ball sound */

   vol=speed_meter*30;
   add_sample_with_volume(0,HIT_WHITE, 0,vol);

   frames_passed=frames;
   save_count=0;

   save_frame_for_delay_after_shot=frames;

   while	(1) 
    {
/* reset foul flag */
     foul_occured = 0;

     if (keys[kK]==128) game_over=YES;

/* exit game check */

/*----------------------*/
/* exit game check */

     if	( quitkey==YES  ) 
      {
       quitgame=YES;
       for	(a=0;a<12;a++)	serial_transfer();
       quit_frame_control();

       if	(exit_frame==NO) 
        {
         quitgame=NO;
	      for	(a=0;a<12;a++)	serial_transfer();
         extern_quit=NO;
        }
	    else
        {
	      quitgame=YES;
	      return	TRUE;
        }

/* check key inputs for demo play */
       if  (mouse_buttons !=0 )  
        {
         quitgame=YES;
		   return	TRUE;
        }

	    if ( (find_which_key_input1()!=NULL) )
        {
         quitgame=YES;
		   return	TRUE;
        }
		}

/* Render screen */

		 if	(quitgame==YES) break;
   
       if ( (getkey(kU)==YES) && (show_last_shot==NO) &&
            ( TOURNAMENT_PLAY == NO ) && ( GAME_TYPE != SPEED_POOL) && (GAME_TYPE != KILLER_POOL) 
/* don't allow player to undo last shot when playing the computer */
            && ( PLAYERS_SELECTED!=2) ) 	
           {
            show_last_shot=YES;
            undo_shot_pos = shot_pos;
            save_two_shots = Two_Shots;
			  }

/* update ball positions */

      if ( show_last_shot == NO) 
       {
        for (count = 0 ; count < LAST_BALL ; count++)
         {
          if ( SNOOKER_MODE == NO )
           {
            pool_undo_buffer[shot_pos].x[count] =ball[count].x;	// RESTORE: 8.24
            pool_undo_buffer[shot_pos].y[count] =ball[count].y;	// RESTORE: 8.24
            pool_undo_buffer[shot_pos].ax[count]=ball[count].ax;	// RESTORE: 16.16 angle, as FIXBALL
            pool_undo_buffer[shot_pos].ay[count]=ball[count].ay;	// RESTORE: 16.16 angle, as FIXBALL
            pool_undo_buffer[shot_pos].az[count]=ball[count].az;	// RESTORE: 16.16 angle, as FIXBALL
            pool_undo_buffer[shot_pos].active[count]=ball[count].active;
	        }
          else
           {
            pool_undo_buffer[shot_pos].x[count]=ball[count].x;	// RESTORE: 8.24
            pool_undo_buffer[shot_pos].y[count]=ball[count].y;	// RESTORE: 8.24
            pool_undo_buffer[shot_pos].active[count]=ball[count].active;
		     }
		   }

#define UNDO_FRAME_SPEED 4

        if ( save_count == 0 )
         {
          if ( shot_pos < MAX_UNDO_LIST ) shot_pos++;
          else 
           {
				printf("RUN OUT OF UNDO POSITIONS" "N");	// RESTORE: was \N, which SN's compiler read as N
           }
          
         }
        save_count++;
        save_count%=UNDO_FRAME_SPEED;
	    }

/* move balls for this frame */

		 speed=frames-frames_passed;
		 frames_passed=frames;

	game_frames=speed;

		 for (count=0;count < speed;count++)
			{
		 	if(any_balls_moving())
				{
				update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24
				}
			else count = speed;
			}

		DrawScreen();

/* skip to end of shot */

      if ( (getkey(kZ)==YES) && (GAME_TYPE != SPEED_POOL) && (show_last_shot==NO) )
        {

         while	( (any_balls_moving()==TRUE) ) 
          {
           for (count = 0 ; count < LAST_BALL ; count++)
            {
             if ( SNOOKER_MODE == NO )
              {
//g               pool_undo_buffer[shot_pos].x[count] =(long) (ball[count].x * 65536);
//g               pool_undo_buffer[shot_pos].y[count] =(long) (ball[count].y * 65536);
//g               pool_undo_buffer[shot_pos].ax[count]=(long) (ball[count].ax * 65536);
//g               pool_undo_buffer[shot_pos].ay[count]=(long) (ball[count].ay * 65536);
//g               pool_undo_buffer[shot_pos].az[count]=(long) (ball[count].az * 65536);
//g               pool_undo_buffer[shot_pos].active[count]=ball[count].active;
		        }
             else
              {
               pool_undo_buffer[shot_pos].x[count]=ball[count].x;	// RESTORE: 8.24
               pool_undo_buffer[shot_pos].y[count]=ball[count].y;	// RESTORE: 8.24
               pool_undo_buffer[shot_pos].active[count]=ball[count].active;
		        }
		      }

           if ( save_count == 0 )
            {
             if ( shot_pos < MAX_UNDO_LIST ) shot_pos++;
             else 
              {
		    		printf("RUN OUT OF UNDO POSITIONS" "N");	// RESTORE: was \N, which SN's compiler read as N
              }
            }
           save_count++;
           save_count%=UNDO_FRAME_SPEED;

           update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24
          }
		  }
/* else in undo last shot mode */
      else
       {

/* restore position's from table if in undo last shot mode */

          if ( undo_shot_pos > 0)  
           {
            if ( undo_shot_pos >= 0)  
             {
              undo_shot_pos-=1;
              for (count =0 ; count < LAST_BALL ; count++)
               {
                if ( SNOOKER_MODE == NO )
	              {
//g                  ball[count].x = (double)pool_undo_buffer[undo_shot_pos].x[count]/ 65536;
//g                  ball[count].y = (double)pool_undo_buffer[undo_shot_pos].y[count]/ 65536;
//g                  ball[count].ax= (double)pool_undo_buffer[undo_shot_pos].ax[count]/ 65536;
//g                  ball[count].ay= (double)pool_undo_buffer[undo_shot_pos].ay[count]/ 65536;
//g                  ball[count].az= (double)pool_undo_buffer[undo_shot_pos].az[count]/ 65536;
//g                  ball[count].active=pool_undo_buffer[undo_shot_pos].active[count];
//g                  ball[count].x_prev= ball[count].x;
//g                  ball[count].y_prev= ball[count].y;
//g                  saveball_lines[count]=ball[count]; //setup traking lines ball's
			        }
                else
	              {
                  ball[count].x = pool_undo_buffer[undo_shot_pos].x[count];	// RESTORE: 8.24
                  ball[count].y = pool_undo_buffer[undo_shot_pos].y[count];	// RESTORE: 8.24
                  ball[count].active=pool_undo_buffer[undo_shot_pos].active[count];
                  ball[count].x_prev= ball[count].x;
                  ball[count].y_prev= ball[count].y;
                  saveball_lines[count]=ball[count]; //setup traking lines ball's
			        }
			      }
			    }
			  }
         }

/* show  rendered frame */

	  	 Handle3D();

      if ( show_last_shot == NO)
       {
        if ( (any_balls_moving()==FALSE) ) break;
       }
		else
       {
        if (undo_shot_pos <= 0) 
         {

   /* reset all foul flags */
          camera_rot = save_undo_camera_rot;
          camera_target = save_undo_camera_pos;
          cue_rot =    save_undo_cue_rot;
          camz = save_undo_camz;
          E_EaseCamera(ease_speed);
          E_PositionCamera( &camera_target, &camera_rot, camz*256 );

          nine_ball_count = save_9_ball_count_for_undo_shot;
          Two_Shots = save_two_shots;    
          break_flag =  save_Break_Flag;
          ball_in_hand_flag =  save_ball_in_hand;
          foul_occured = save_undo_foul;
          Player_Active = save_Player_Active;
          clear_replay_panel_count=2;

          break;
         }
		 }
		}


      Handle3D();

/*------------------------------------*/

		for	(a=0;a<6;a++)	serial_transfer();

		if	(LINK_OPTION !=NULL)	
       {
        if	(extern_quit == YES) quitgame=YES;
       }

		if	(quitgame==YES) return	TRUE;
	  
		return	FALSE;
}        
/* ------------------------------------------------------------------- */
void Show_Undo_Shot(void)
{
   long	count;

/* play hit cue ball sound */

   if	(PLAYERS_SELECTED==2)		/* don't allow undo last shot while 
                                       playing the computer */
     {
      return;
     }

   if ( (TOURNAMENT_PLAY == YES) ||  (GAME_TYPE==SPEED_POOL) ) return;

   if ( getkey(kU)==0 ) return;

   undo_shot_pos = shot_pos;
   if (undo_shot_pos == 0) return;

   show_shot_flag=1;
   refresh_whole_screen=2;

   while	(1) 
    {

/* update ball positions */
      undo_shot_pos-=1;
      for (count = 0 ; count < LAST_BALL; count++)
       {
        if ( SNOOKER_MODE == NO )
         {
          ball[count].x = pool_undo_buffer[undo_shot_pos].x[count];	// RESTORE: 8.24
          ball[count].y = pool_undo_buffer[undo_shot_pos].y[count];	// RESTORE: 8.24
          ball[count].ax= pool_undo_buffer[undo_shot_pos].ax[count];	// RESTORE: 16.16 angle, as FIXBALL
          ball[count].ay= pool_undo_buffer[undo_shot_pos].ay[count];	// RESTORE: 16.16 angle, as FIXBALL
          ball[count].az= pool_undo_buffer[undo_shot_pos].az[count];	// RESTORE: 16.16 angle, as FIXBALL
          ball[count].active=pool_undo_buffer[undo_shot_pos].active[count];
          ball[count].x_prev= ball[count].x;
          ball[count].y_prev= ball[count].y;
          saveball_lines[count]=ball[count]; //setup traking lines ball's
	      }
        else
         {
          ball[count].x = pool_undo_buffer[undo_shot_pos].x[count];	// RESTORE: 8.24
          ball[count].y = pool_undo_buffer[undo_shot_pos].y[count];	// RESTORE: 8.24
          ball[count].active= pool_undo_buffer[undo_shot_pos].active[count];
          ball[count].x_prev=ball[count].x;
          ball[count].y_prev=ball[count].y;
          saveball_lines[count]=ball[count]; //setup traking lines ball's
	      }
	    }

/* show next redered frame */

     Handle3D();
     if (undo_shot_pos <= 0) 
      {
/*------------------------------------*/
       nine_ball_count = save_9_ball_count_for_undo_shot;
       Two_Shots = save_two_shots;    
       draw_foul_shot_icons = save_draw_foul_shot_icons;

       break_flag =  save_Break_Flag;
       ball_in_hand_flag =  save_ball_in_hand;
       foul_occured = save_undo_foul;
       Player_Active = save_Player_Active;

       clear_replay_panel_count=2;
       break;
      }

     clear_replay_panel_count=2;
    }

   show_shot_flag=0;

/* re aline camera after every shot */

	camera_rot = save_undo_camera_rot;
	camera_target = save_undo_camera_pos;
	cue_rot =    save_undo_cue_rot;
   camz = save_undo_camz;
   E_EaseCamera(ease_speed);
   E_PositionCamera( &camera_target, &camera_rot, camz*256 );

   refresh_whole_screen=2;
}        
/* ------------------------------------------------------------------- */
void Show_Replay(void)
{
long	l,exit=NO,speed,count;
long  save_foul;
long  save_butt=butt;
long  save_frames;
long  save_PLAYER_ACTIVE=Player_Active;

static long last_speed;

/* check to see if player want's to save this shot to disk */

  if (replay_avalible==NO) return;

  if ( getkey(kF12)==YES)
   {
    Save_shot_Room();
	}

   replay_flag=NO;

   
   if ( show_replay_flag == NO )
   {
    if	( ( show_saved_shot_flag==NO) && (GAME_TYPE!=TRICK_SHOT) )
     {
		if	( getkey(REPLAY_KEY)==0) return;
	  }
	}

   show_panel = 100;
   clear_replay_panel_count=2;
   save_foul = foul_occured;

/* reset all foul data */

   nine_ball_count = save_9_ball_count_for_undo_shot;
   Two_Shots = save_two_shots;    
   ball_in_hand_flag =  save_ball_in_hand;
   foul_occured = save_undo_foul;
   Player_Active = save_Player_Active;
   clear_replay_panel_count=2;

/* setup correct message for replay last shot */
   if ( show_saved_shot_flag == NO )
    {
     foul_occured = REPLAY_LAST_SHOT;
	 }
   else
    {
/* setup replay last saved shot */
     foul_occured = REPLAY_LAST_SAVED_SHOT;
    }

/* reset balls back to position before shot taken */

    for	(l=0;l< LAST_BALL;l++) 
	  {
	   memcpy(&saveball[l] ,&ball[l] , sizeof(ballstruct) );
	   memcpy(&ball[l] ,&replayball[l] , sizeof(ballstruct) );
	  }

/* setup camera to point at cue ball */

		show_shot_flag=NO;

#if 0
      camera_mode=-1;

     if  (LOCATION > DIFFERENT_SHAPED_TABLES_INDEX-1)
/* re aline camera after every shot */
      {
       camera_mode=-1;
       Move_Camera_Position(ZOOM_BACK_TO_CUE_BALL);
		}
     else
      {
       setup_default_camera_position(ease_speed,NO);
      }
#endif

      cue_rot.y=replay_cue_angle;
		butt=replay_butt;
		replay_flag=YES;
      activated_lines=FALSE;

#if 0
      while ( (E_FinishedEase() == NO) )
       {
	  	  Handle3D();
		 }
#endif

      count=0;
      shoot_mode=0;

      while ( count < replay_cue_position_table_ptr )
       {
        refresh_whole_screen=2;
        cue_position=cue_position_table[count];

/* setup cue in currect positon */

        if ( cue_position < FIX24_TO_DOUBLE(ball[WHITE_BALL].radius) )	// RESTORE: 8.24
         {
          cue_position = FIX24_TO_DOUBLE(ball[WHITE_BALL].radius);	// RESTORE: 8.24
    	   }

	  	  Handle3D();
        count++;

/* check to see if player want's to save this shot to disk */

		  if ( getkey(kF12)==YES)
         {
          Save_shot_Room();
			}
		 }

       show_shot_flag=YES;

/* ------------------------- */
/* show shot at end of frame */

       reset_fouls();
       show_shot_flag=YES;
       hit_ball(WHITE_BALL, replay_x_hit_pos, replay_y_hit_pos,(double) replay_pan_angle, (double) replay_butt, DOUBLE_TO_FIX24(replay_speed_meter));	// RESTORE: 8.24

/* ------------------------- */
/* main replay loop */

      last_speed=frames;
      refresh_whole_screen=2;

      while	( exit==NO )
       {

/* allow player to replay balls if they have finnished */

       if	( (any_balls_moving()==FALSE) )	
        {

/* check to see if showing last shot of match , if so then turn off replay
     panel */

         replay_flag=NO;
         show_replay_flag=NO;
		   show_panel=1;
         exit=YES;
        }
		 else
        {

			speed=frames-last_speed;
			last_speed=frames;

          for (count=0;count < speed;count++)
             update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24
		  }

		 Handle3D();

/* check to see if player want's to save this shot to disk */

		  if ( getkey(kF12)==YES)
         {
	       save_frames=frames;
          Save_shot_Room();
          frames=save_frames;
			}

       scan_for_keysjp();
       if ( (find_which_key_input()!= NULL) ) break;

		}

/* reset ball positions */

      while	( (any_balls_moving()==TRUE) )	
       {
        update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24
       }

      update_fouls();

/* reset cue position, after replay */

      show_replay_flag=NO; 

/* debounce replay selection */

      reset_aiming_varibles1();

		show_shot_flag=NO;
		replay_flag=NO;
		Player_Active=save_PLAYER_ACTIVE;
      foul_occured=save_foul;
      butt=save_butt;
      clear_replay_panel_count=2;
      draw_twice=2;

/* move camera back to cue ball */

      camera_mode=-1;
      Move_Camera_Position(MOVE_BACK_TO_CUE_BALL);

      cue_rot = camera_rot;
      cue_angle=camera_rot.y;
      fixed_camera_rot = camera_rot;
      refresh_whole_screen=2;

	  	Handle3D();
}               
/* ------------------------------------------------------------------- */
void tracking_lines_loop(void)
{
	long	   count,ready;
	double	steves_angle,tmp_speed_meter;
   double   tmp_angle;

/* if undo shot happening then no tracking lines loop */

   if ( (undo_shot_pos > 0) or (replay_flag == YES) )
    {
     if (  activated_lines==FALSE) return;

/* save ball positions ready for reseting balls */

     for (count=0;count<LAST_BALL;count++) 
      {
       ball[count]=saveball_lines[count];
      }

     activated_lines=FALSE;
     reset_fouls();
     return;
    }

   if (shot_taken == TRUE ) return;

   old_lines=lines_on;

/* check all conditions */

    if	( (trackkey==YES) && ( TOURNAMENT_PLAY == NO )
      && ( LOCATION < DIFFERENT_SHAPED_TABLES_INDEX) 

/*---------------------------------------------------*/
/* don't allow tracking lines in following game types */

      && (GAME_TYPE != CONTINUOUS) 
      && (GAME_TYPE != KILLER_POOL) ) 

/*---------------------------------------------------*/
    {

/* check to see if turned cheat lines on in Speed Pool */

     if	(PLAYERS_SELECTED!=2)		/* don't allow tracking lines while 
                                       playing the computer */
      {

       if ( GAME_TYPE == SPEED_POOL)
        {
         if ( SPEED_POOL_CHEAT_LINES == YES) 
          {
           lines_on^=1;
          }
        }
       else
        {
         lines_on^=1;
	     }
	   }  
	 }  



    if ( keys[kMINUS]==128) 
      {
       if (line_length > 256) line_length-=10;
       printf("%d\n",line_length);
      for (count=0;count<LAST_BALL;count++) 
       {
        ball[count]=saveball_lines[count];
       }

      activated_lines=FALSE;
      reset_fouls();
      }
    if ( keys[kEQUAL]==128) 
     {
      if (line_length < 256*3) line_length+=10;
       printf("%d\n",line_length);
/* save ball positions ready for reseting balls */

      for (count=0;count<LAST_BALL;count++) 
       {
        ball[count]=saveball_lines[count];
        line_set[count]=NO;
       }

      activated_lines=FALSE;
      reset_fouls();
      refresh_whole_screen=2;
     }

/* if lines turned off redraw whole screen */

    if ( (old_lines != lines_on) && (lines_on==NO) ) 
     {

/* save ball positions ready for reseting balls */

      for (count=0;count<LAST_BALL;count++) 
       {
        ball[count]=saveball_lines[count];
        line_set[count]=NO;
       }

      activated_lines=FALSE;
      reset_fouls();
      refresh_whole_screen=2;
     }

/* setup tracking lines */

   ready=YES;

/* in us pool mode only draw tracking lines if camera fully rotaed around */

   if ( (GAME_TYPE == US_POOL) && ( SNOOKER_MODE == NO) )
    {
     ready = draw_cue;
	 }

   if ( ready==YES) 
   {
	  if ( ( activated_lines==FALSE ) && ( lines_on==YES ) && (E_FinishedEase()==YES) 
            && (cue_position==FIX24_TO_DOUBLE(ball[WHITE_BALL].diameter*4)) )	// RESTORE: 8.24
      {
	 //&& (event_flag!=MOVING_BALL_IN_HAND)
       for (count=0;count<LAST_BALL;count++) 
        {
         saveball_lines[count]=ball[count];
         line_pointer_index[count]=0;
         line_pos[count]=1;
         line_set[count]=NO;
        }

      tmp_speed_meter=(cue_position*line_length) * cue_weight;

      steves_angle= cue_angle;
      new_count=0;
      hit_ball(WHITE_BALL, x_hit_pos, y_hit_pos, (double)steves_angle, ((double) butt), DOUBLE_TO_FIX24(tmp_speed_meter));	// RESTORE: 8.24
      activated_lines=TRUE;
	 }

	 if (activated_lines == TRUE) //&& (event_flag!=MOVING_BALL_IN_HAND) ) 
     {
    //  refresh_whole_screen=2;
	   build_tracking_line_points();
     } 
   }
}
/* ------------------------------------------------------------------- */
void players_shot(void)
{
 long	a,count;

 activated_lines=FALSE;
 quitgame=0;

 replay_cue_position_table_ptr=cue_position_table_ptr;
 cue_position_table_ptr=0;

  for (count=0;count<LAST_BALL;count++) 
   {
    ball[count].moving=NO;
    ball[count].vx=0;
    ball[count].vy=NO;
    ball[count].vz=NO;
	}

  show_last_shot = NO;
  name_frame_diff = frames;


  while   ( 1 )
	{
//	get_pad();

    if (keys[kK]==128)
     {
     for (count=1;count<16;count++) ball[count].active=0;
	  }

#if 0
   if (getkey(k2)==YES) game_over=YES;
   if (getkey(k4)==YES) Frames_played[0][0]+=1;
   if (getkey(k5)==YES) Frames_played[0][1]+=1;
#endif
 
/* check to see if player wants to undo last shot */

   Show_Undo_Shot();

/* check replay last shot ? */

   Show_Replay();

/* put English on cue ball */

	if	(LINK_OPTION!=NULL)	
    {

    if	( (Player_Active == 0) && (master==SLAVE) )	{
		x_hit_pos= extern_event[2];
		y_hit_pos= extern_event[3];
		butt = extern_event[4];
		}

    if	( (Player_Active == 1) && (master==MASTER) )	{
		x_hit_pos= extern_event[2];
		y_hit_pos= extern_event[3];
		butt = extern_event[4];
		}
    }

/*----------------------*/
/* exit game check */
	if	( quitkey==YES )
		{
		quitgame=YES;

		for	(a=0;a<12;a++)	serial_transfer();
          quit_frame_control();

		if	(exit_frame==NO)	{
			quitgame=NO;
			for	(a=0;a<12;a++)	serial_transfer();
		//	load_all_game_graphics_from_disk();
		//	Re_Draw_Screen();
			extern_quit=NO;
			}
			else	{
			quitgame=YES;
			return;
			}	
		}

/*----------------------*/

		if	(LINK_OPTION !=NULL)	
       {

		  for	(a=0;a<12;a++)	serial_transfer();

		  if	(extern_quit==YES)  
         {
		    quitgame=YES;
		    for	(a=0;a<12;a++)	serial_transfer();
            quit_frame_control();

		    if	(exit_frame==NO)	 
           {
			   quitgame=NO;
			   for	(a=0;a<12;a++)	serial_transfer();
			 //  load_all_game_graphics_from_disk();
			 //  Re_Draw_Screen();
			   extern_quit=NO;
			  }
			 else	
           {
			   quitgame=YES;
				return;
			  }
			}
		 }


/* render 3d scene */

      Handle3D();

/* check to see if cue hit white ball */

		if ( shot_taken==YES ) break;

	 }

/* turn off tracking lines */

    if (activated_lines==TRUE) 
     {
      for (count=0;count<LAST_BALL;count++) 
       {
        ball[count]=saveball_lines[count];
       }

      activated_lines=FALSE;
      reset_fouls();
     }

/* save all relevent foul information */

   save_9_ball_count_for_undo_shot = nine_ball_count;
   save_two_shots = Two_Shots;
   save_Break_Flag = break_flag;
   save_ball_in_hand = ball_in_hand_flag;
   save_undo_foul = foul_occured;
   save_Player_Active = Player_Active;
	save_draw_foul_shot_icons = draw_foul_shot_icons;

   show_shot_flag=1;

/* setup delay after taking shot, before player cam move camera about */

   delay_after_shot=32;

/*	serial_transfer();

	for	(a=0;a<6;a++)	serial_transfer();

	if	(quitgame==YES)  	return;

	if	(LINK_OPTION !=NULL)	{
		if	(extern_quit == YES) 	{ quitgame=YES; return;}
		}

	if	(LINK_OPTION!=NULL)	{

	if	( (Player_Active == 0) && (master==SLAVE) )	{
	  	cue_speed =	extern_cue_pan[2];
		x_hit_pos= extern_event[2];
		y_hit_pos= extern_event[3];
		butt = extern_event[4];
		}

	if	( (Player_Active == 1) && (master==MASTER) )	{
	  	cue_speed =	extern_cue_pan[2];
		x_hit_pos= extern_event[2];
		y_hit_pos= extern_event[3];
		butt = extern_event[4];
		}
	}
*/
   shot_pos=0;
	good_shot_flag=YES;
	computer_first_shot=YES;
}
/* ------------------------------------------------------------------- */
double check_ball_hypot(long x, long y)
{
	double result;

	result=sqrt((double) x*x+(double) y*y);

	return result;
}
/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
void	reset_aiming_varibles(void)
{
 long count;
      pocket_nominated=-1;

      shoot_mode_availble = NO;
      camera_in_pocket = NO;
      setup_default_angle=NO;

      lined_up_shot=NO;
      pot_count=0;
      delay_after_shot=0;
      show_shot_flag=NO;
      amplify=NO;
      undo_shot_pos=0;
//cue_position_table_ptr=0;
//      shot_pos=0;

/* swap players turn */

	  	x_hit_pos=0;
	  	y_hit_pos=0;
	  	butt=1; //reset cue but position
	  	last_butt=0; //reset cue but position

/* ------------------ */
/* reset flag for checking any object ball potted */

      ball_to_move=WHITE_BALL;

      oldevent_flag=-1;
      event_flag=-1;
      shot_taken=NO;
      cue_position=FIX24_TO_DOUBLE(ball[WHITE_BALL].diameter*4);	// RESTORE: 8.24

      last_mouse_ydiff=cue_position;

/* redraw whole screen */

      first_hit=NO;
      reset_fouls();
      shoot_mode=0;

/* save new ball position for tracking lines */

      reset_tracking_lines();
#if 0
      for (count=0;count<LAST_BALL;count++) 
       {
        saveball_lines[count]=ball[count];
       }
#endif
}
/* ------------------------------------------------------------------- */
void	reset_aiming_varibles1(void)
{
 		pot_count=0;

/* ------------------ */
/* reset flag for checking any object ball potted */

      reset_tracking_lines();
		ball_to_move=WHITE_BALL;
		shoot_mode=NO;
		shot_taken=NO;
      cue_position=FIX24_TO_DOUBLE(ball[WHITE_BALL].diameter*4);	// RESTORE: 8.24
}		
/* ------------------------------------------------------------------- */
void	start_game_track(void)
{
#if 0
/* setup cd track */
	switch	(GAME_TYPE)	
    {
	  case	UK_POOL:
	   	cd_track=ARENA;
			break;

		case	US_POOL:
			cd_track=JAZZ1;
			break;

		case	SPEED_POOL:
			cd_track=FUNK;
			break;

		case	US_8BALL:
			cd_track=JAZZ3;
			break;

		case	KILLER_POOL:
			cd_track=TOURNAMENT;
			break;
    }
#endif
}
/* ------------------------------------------------------------------- */
void    set_target_for_white_TRICK( void )
{
   long	  l;
   double  steves_angle;

/* setup players shot up */

   if ( good_trick_shot == YES)
    {
	  hit_ball(WHITE_BALL, replay_x_hit_pos, replay_y_hit_pos, (replay_pan_angle*2*PI)/360, ((double) replay_butt), DOUBLE_TO_FIX24(replay_speed_meter));	// RESTORE: 8.24
	  show_shot_flag=1;
	 }
   else 
    {
     reset_aiming_varibles1();

     players_shot_TRICK();
     show_shot_flag=1;

     steves_angle= (((cue_angle-1024)&4095) * 360)  / 4096 ;
     speed_meter=fabs(speed_meter);
     speed_meter=speed_meter*.075;

     hit_ball(WHITE_BALL, x_hit_pos, y_hit_pos, (steves_angle*2*PI)/360, ((double) butt), DOUBLE_TO_FIX24(speed_meter));	// RESTORE: 8.24

     replay_pan_angle=steves_angle;
     replay_butt=butt;
     replay_speed_meter=speed_meter;
     replay_x_hit_pos=x_hit_pos;
     replay_y_hit_pos=y_hit_pos;
     replay_avalible=YES;

     for	(l=0;l< LAST_BALL;l++)	
      {
       replayball[l] = ball[l];
	   }
    }
}
/* ------------------------------------------------------------------- */
flag Show_Shot_Trick(void)
{
   long	speed,count,l;
   int	vol;
static	long	frames_passed;

   if ( good_trick_shot == YES) 
    {
	  for (l=0;l<LAST_BALL;l++) 
     memcpy(&replayball[l] ,&ball[l] , sizeof(ballstruct) );

     Show_Replay();

     quitgame=YES;
     return	TRUE;
    }

/* play hit cue ball sound */

	vol=replay_speed_meter*30;
	add_sample_with_volume(0,HIT_WHITE, 0,vol);

   frames_passed=frames;

   while	((any_balls_moving()==TRUE)) 
	 {

/* exit game check */
/*----------------------*/
/* exit game check */

	if	( quitkey==YES ) 
	  {
 	   quitgame=YES;
 	   return	TRUE;
     }

/* calculate number of frames passed since last game loop */

	 speed = frames-frames_passed;
	 frames_passed=frames;	

	 for (count=0;count < speed;count++)
     {
      update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24
	  }

/* Render screen */

	  Handle3D();
    }
	 return	FALSE;
}        
/* ------------------------------------------------------------------- */
	long	flash_ball=0;
void	players_shot_TRICK(void)
{
	long	a,count;

   ball_to_move=1;

		while   ( 1 )
		{
/* check save setup poistion */

		if	(getkey(k2)==YES) {
		 for (a=0;a<LAST_BALL;a++) replayball[a]=ball[a];
       }

      if	(getkey(k1)==YES) Save_Trick_Shot();

/* put English on cue ball */

 		Update_English_On_Cue_Ball();

/* allow player to move cue in and out */

		Update_Cue_Position();

/* render 3d scene */
		Handle3D();

	if	( quitkey==YES ) 
			{
			break;
			}

		a=ball_to_move;

/* ------------------------- */
/* move to next ball in pack */

      if	( (getkey_jp(kN)==YES) && (flash_ball == 0) ) 
       {
        ball_to_move ++;
        ball_to_move%=LAST_BALL;

       while (ball[ ball_to_move ].active == NO) 
        {
        ball_to_move ++;
        ball_to_move%=LAST_BALL;
        }
       }

/* flash selected ball for a few frames */
		if	( (a!=ball_to_move) && (ball_to_move!=0) )	
       {
		  flash_ball=16; /* setup number of frames to flash ball */
		 }

		if	( flash_ball > 0 ) 
       {
		  ball[ ball_to_move ].active^=1;
		  flash_ball--;
		  if	( flash_ball==0 ) ball[ball_to_move].active=1;
       }
/* ------------------------- */
/* check turn ball off */

      if	( (getkey_jp(k3)==YES) && (flash_ball == 0) && ( ball_to_move != 0) ) 
       {
        count=0;
        while ( (ball[ ball_to_move ].active == NO) && ( count < LAST_BALL-1) ) {
         count++;
         ball_to_move++;
         ball_to_move%=LAST_BALL;
         if (ball_to_move==0) ball_to_move++;
         }
		   ball[ ball_to_move ].active=0;
       }

      if	( (getkey_jp(k4)==YES) && (flash_ball == 0) ) 
       {
        count=0;
        while ( (ball[ ball_to_move ].active == YES) && ( count < LAST_BALL) ) {
         count++;
         ball_to_move++;
         ball_to_move%=LAST_BALL;
         }
		   ball[ ball_to_move ].active=1;
       }

/*----------------------*/
/* check to see if cue hit white ball */

		if      (speed_meter < 0 ) break;
		}

	shot_taken=YES;
	if	(quitgame==YES)  	return;
	good_shot_flag=YES;
	quitgame=NO;
}
/* ------------------------------------------------------------------- */
void play_trickshot(void)
{
	long	l;

/* reset flag for old message type */
	break_flag=NO;
	ORDER_COUNT=1;
	show_shot_flag=NO;
/* do game loop until all frames played */

   if ( good_trick_shot == YES) 
    {
     Show_Shot_Trick();
    }
   else 
    {

	  while	( getkey(kESC)==0)
      {
/* control loop for setting up shot */
       set_target_for_white_TRICK();

       if ( getkey(kESC)==0) Show_Shot_Trick();

       for	(l=0;l< LAST_BALL;l++) 
        {
         ball[l]=replayball[l];
        }
	   }
	 }
}
/* ------------------------------------------------------------------- */
void Save_Trick_Shot(void)
{
char *file_name = "tsXX.dat";
	char	filename[80];
	long	l;
typedef struct
        {
          double x, y;
          double x_prev, y_prev;
          char active;
          long potted_flag;
		  } FRED;

	 FRED *fred_ptr;

typedef struct
        {
          double replay_pan_angle, replay_speed_meter;
          long	replay_butt;
          long replay_x_hit_pos;
          long replay_y_hit_pos;
          long replay_avalible;
          char GAME_TYPE1;
          char TABLESIZE_TYPE1;
          char LOCATION1;
          long LAST_BALL;
		  } FRED1;

	 FRED1 *fred_ptr1;


	file_name[2] =48+ (trick_name_selected / 10 );
	file_name[3] =48+ (trick_name_selected % 10 );

	sprintf( filename, "%s//%s//%s",drive_and_path,"magic",file_name );

	if	( (load_file_here(filename,save_shot_buffer,0)) !=0 ) return;

	for	(l=0;l< LAST_BALL;l++) 
		{
		fred_ptr = (FRED *) &save_shot_buffer[200];
		fred_ptr[l].x =replayball[l].x;
		fred_ptr[l].y =replayball[l].y;
		fred_ptr[l].x_prev =replayball[l].x_prev;
		fred_ptr[l].y_prev =replayball[l].y_prev;
		fred_ptr[l].active =replayball[l].active;
		fred_ptr[l].active =replayball[l].active;
	   }


	fred_ptr1 = (FRED1 *) &save_shot_buffer[100];
	fred_ptr1[0].replay_pan_angle=replay_pan_angle;
	fred_ptr1[0].replay_speed_meter=replay_speed_meter;
	fred_ptr1[0].replay_butt=replay_butt;
	fred_ptr1[0].replay_x_hit_pos=replay_x_hit_pos;
	fred_ptr1[0].replay_y_hit_pos=replay_y_hit_pos;
	fred_ptr1[0].replay_avalible=replay_avalible;
	fred_ptr1[0].LAST_BALL=LAST_BALL;
	fred_ptr1[0].GAME_TYPE1=GAME_TYPE;
	fred_ptr1[0].LOCATION1=LOCATION;
	fred_ptr1[0].TABLESIZE_TYPE1=TABLESIZE_TYPE;

/* copy name to save buffer */

//g	saveafile(filename, save_shot_buffer , SIZE_OF_SAVE_SHOT_DATA );
}
/*-----------------------------------------------------------------*/
void setup_saved_trickshot_shot(void)
{
char *file_name = "tsXX.dat";
	char	filename[80];
	long  l;
typedef struct
        {
          double x, y;
          double x_prev, y_prev;
          char active;
          long potted_flag;
		  } FRED;

	 FRED *fred_ptr;
typedef struct
        {
          double replay_pan_angle, replay_speed_meter;
          long	replay_butt;
          long replay_x_hit_pos;
          long replay_y_hit_pos;
          long replay_avalible;
          char GAME_TYPE1;
          char TABLESIZE_TYPE1;
          char LOCATION1;
          long LAST_BALL;
		  } FRED1;

	 FRED1 *fred_ptr1;

	file_name[2] = 48 + (trick_name_selected / 10);
	file_name[3] = 48 + (trick_name_selected % 10);

	sprintf( filename, "%s//%s//%s",drive_and_path,"magic",file_name );

	good_trick_shot=NO;

	if	( (load_file_here(filename,save_shot_buffer,0)) ==0 ) return;
	Setup_New_Game_Type();

	for	(l=0;l < LAST_BALL;l++)
		{
      memcpy(&replayball[l],&ball[l],sizeof(ballstruct));
		fred_ptr = (FRED *) &save_shot_buffer[200];
		replayball[l].x=fred_ptr[l].x; 
		replayball[l].y=fred_ptr[l].y; 
		replayball[l].x_prev=fred_ptr[l].x_prev; 
		replayball[l].y_prev=fred_ptr[l].y_prev; 
		replayball[l].active=fred_ptr[l].active;  
		replayball[l].active=fred_ptr[l].active;  
	}

	fred_ptr1 = (FRED1 *) &save_shot_buffer[100];
	replay_pan_angle=fred_ptr1[0].replay_pan_angle;
	replay_speed_meter=fred_ptr1[0].replay_speed_meter;
	replay_butt=fred_ptr1[0].replay_butt;
	replay_x_hit_pos=fred_ptr1[0].replay_x_hit_pos;
	replay_y_hit_pos=fred_ptr1[0].replay_y_hit_pos;
	replay_avalible=fred_ptr1[0].replay_avalible;
	LAST_BALL=fred_ptr1[0].LAST_BALL;
	GAME_TYPE=fred_ptr1[0].GAME_TYPE1;
	LOCATION=fred_ptr1[0].LOCATION1;
	TABLESIZE_TYPE=fred_ptr1[0].TABLESIZE_TYPE1;

	for	(l=0;l < LAST_BALL;l++)
     memcpy(&ball[l],&replayball[l],sizeof(ballstruct));

	good_trick_shot=YES;
}
/*-----------------------------------------------------------------*/
void  show_andplay_trickshot_loop(void)
{
		char	save_gametype;

		save_gametype=GAME_TYPE;
		load_all_game_graphics_from_disk();
		ORDER_COUNT=1;

		GAME_TYPE=TRICK_SHOT;

/* do game loop until all frames played */

		reset_aiming_varibles();

		Setup_New_Game_Type();

 	   setup_saved_trickshot_shot();

/* setup balls and cue ball to the saved shot poistion */

		init_3d();

      camera_mode=-1;
      Move_Camera_Position(ZOOM_BACK_TO_CUE_BALL);

		Re_Draw_Screen();

/* show shot */

	   play_trickshot();

		fadeout();

/* return backto front end */

		reset_atract_counter();

/* restore game type to origanal one */

	   SVGA_set_mode(0x101);
		GAME_TYPE=save_gametype;
}

/*-----------------------------------------------------------------*/
void	show_saved_shot_loop(void)
{ 
      long	save_gametype=GAME_TYPE;
      long  save_tabletype=TABLE_TYPE;
      long  save_tablesize=TABLESIZE_TYPE;
      long  save_location=LOCATION;
      long  save_res=RESOLUTION;

		break_flag=YES;
		replay_avalible = NO;

/* do game loop until all frames played */

		speed_pot=0;
		game_over=NO;

      table_position=0;
      foul_occured = 0;
      show_replay_flag = YES;

		reset_aiming_varibles();

	   setup_replay_saved_shot();

      cue_rot.y=replay_cue_angle;

		load_all_game_graphics_from_disk();

/* setup balls and cue ball to the saved shot poistion */

		init_3d();

/* show shot */
 printf("RES = %d \n",RESOLUTION);
      Re_Draw_Screen();
		Show_Replay();

/* return backto front end */

		show_saved_shot_flag=NO;
		reset_atract_counter();

/* restore game type to origanal one */

		GAME_TYPE=save_gametype;
      TABLE_TYPE = save_tabletype;
      TABLESIZE_TYPE = save_tablesize;
      LOCATION = save_location;
      RESOLUTION = save_res;
}
/*-----------------------------------------------------------------*/
void	setup_replay_saved_shot(void)
{
char *file_name = "ssX.dat";
	char	filename[80];
	long  l;
typedef struct
        {
          double x, y;
          double x_prev, y_prev;
          char active;
          long potted_flag;
		  } FRED;

	 FRED *fred_ptr;

typedef struct
        {
          double replay_pan_angle, replay_speed_meter;
          long	replay_butt;
          long replay_x_hit_pos;
          long replay_y_hit_pos;
          long replay_avalible;
          long GAME_TYPE1;
          long TABLESIZE_TYPE1;
          long TABLE_TYPE1;
          long LOCATION1;
          long LAST_BALL;
          long UK_OR_US_BALL_SET;
          long RESOLUTION1;
          VERTEX cue_rot;
          long cue_position_table_ptr;
		  } FRED1;

	 FRED1 *fred_ptr1;

	file_name[2] = slot+48;
	sprintf( filename, "%s//%s",drive_and_path, file_name );

	load_file_here(filename,save_shot_buffer,0);

/* setup replay cue data */

	fred_ptr1 = (FRED1 *) &save_shot_buffer[100];

	replay_pan_angle=fred_ptr1[0].replay_pan_angle;
	replay_speed_meter=fred_ptr1[0].replay_speed_meter;
	replay_butt=fred_ptr1[0].replay_butt;
	replay_x_hit_pos=fred_ptr1[0].replay_x_hit_pos;
	replay_y_hit_pos=fred_ptr1[0].replay_y_hit_pos;
	replay_avalible=fred_ptr1[0].replay_avalible;

	LAST_BALL=fred_ptr1[0].LAST_BALL;
	GAME_TYPE=fred_ptr1[0].GAME_TYPE1;
	LOCATION=fred_ptr1[0].LOCATION1;
	TABLESIZE_TYPE=fred_ptr1[0].TABLESIZE_TYPE1;
	TABLE_TYPE=fred_ptr1[0].TABLE_TYPE1;
   UK_OR_US_BALL_SET=fred_ptr1[0].UK_OR_US_BALL_SET;
   RESOLUTION=fred_ptr1[0].RESOLUTION1;

/* copy saved data into replay data structure */

	Setup_New_Game_Type();

   fred_ptr = (FRED *) &save_shot_buffer[200];

	for	(l=0;l < LAST_BALL;l++)
    {
     replayball[l]=ball[l];
     replayball[l].x=fred_ptr[l].x; 
     replayball[l].y=fred_ptr[l].y; 
     replayball[l].x_prev=fred_ptr[l].x_prev; 
     replayball[l].y_prev=fred_ptr[l].y_prev; 
     replayball[l].active=fred_ptr[l].active;  
     replayball[l].active=fred_ptr[l].active;  
     ball[l]=replayball[l];
    }

	memcpy(&cue_position_table[0],&save_shot_buffer[1000],200*8);

	cue_rot=fred_ptr1[0].cue_rot;
   replay_cue_angle = cue_rot.y;
   replay_cue_position_table_ptr=fred_ptr1[0].cue_position_table_ptr;

}
/*-----------------------------------------------------------------*/
void quit_frame_control(void)
{
	char c;
flag done=NO;

  QUIT_GAME_FLAG=YES;
  while   ( !done )
	{
    show_panel = 100;

    Handle3D();

/* exit end of frame loop */

	 do
	 {
	 c=get_key();

    if ( (keys[kN]==128) or (mouse_buttons==RIGHT_BUTTON) )
       {
        done=YES;
        break;
		 }

    if ( (keys[kY]==128) or (mouse_buttons==LEFT_BUTTON) )  
     {
      exit_frame=YES;
      //fadeout();
      done=YES;
      break;
     }

    } while(c!=0);

    }
/* quit with text still on screen */

  draw_twice=2;
  quit_flash_count=0;
  Handle3D();
  QUIT_GAME_FLAG=NO;

}
/* ------------------------------------------------------------------- */
void Re_Rack_Balls_with_other_gametype(void)
{
 long a;

   if ( ( GAME_TYPE == UK_POOL ) or ( GAME_TYPE == US_8BALL ) or ( GAME_TYPE == US_POOL ) )
    {
    }
   else
    {
     return;
	 }

    if ( (TOURNAMENT_PLAY == YES) or (SNOOKER_MODE == YES) ) return;

/* change game type */

    GAME_TYPE++;
    GAME_TYPE%=3;
    save_game=GAME_TYPE;

    Re_Rack_Balls();
}
/* ------------------------------------------------------------------- */
void Re_Rack_Balls(void)
{
 long a;

   if ( ( GAME_TYPE == SPEED_POOL ) or ( GAME_TYPE == KILLER_POOL ) ) return;

   if (TOURNAMENT_PLAY == YES ) return;

//	if ( break_flag == YES ) return;

/* setup balls for this game type */

    total[0][0]=0;
    total[0][1]=0;
    lines_on=NO;
    activated_lines=FALSE;
    quitgame=NO;
    foul_occured=0;
    replay_avalible = NO;
    end_of_frame_flag=NO;
    reset_aiming_varibles();
    nine_ball_count=1;
    setup_points_game();
    show_last_shot=NO;
    shot_position=0;

    Setup_New_Game_Type();

/* reset player's score's for this frame */
    for	(a=0;a<16;a++)	players_scores[a]=0;

	if ( break_flag == NO  )
    {
/* re aline camera after every shot */

     camera_mode=-1;
     Move_Camera_Position(ZOOM_BACK_TO_CUE_BALL);
	 }
   else
    {
/* setup camera position for first time */

     if  (LOCATION > DIFFERENT_SHAPED_TABLES_INDEX-1)

/* re aline camera after every shot */
      {
       camera_mode=-1;
       Move_Camera_Position(ZOOM_BACK_TO_CUE_BALL);
		}
     else     setup_default_camera_position(12,NO);

    }
}
/* -------------------------------------------- */
// RESTORE: P is the pad's Start button (PAD.C getkey).  The three loops below
// wait for it to be released, pressed and released; on the PC the keyboard
// was read by the system, here get_pad is called in each.
void pause_game(void)
{
 long tmp = foul_occured;

	if	( getkey(kP)==YES)
    {

     clear_replay_panel_count=2;
     foul_occured=2;
     draw_twice=2;

     Render();

     wait_for_vblank();
/* test for a pause key */
	  while	( getkey(kP)==YES)
	   {
		 wait_for_vblank();
		 at(0,10);
		 get_pad();	// RESTORE: the pad is read here (the PC read the key itself); see PAD.C
		 if	(getkey(kP)==0)	break;
		}

/* wait unil un-paused */
		while	(getkey(kP)==0)
			{
			wait_for_vblank();
			at(0,10);
		 get_pad();	// RESTORE: the pad is read here (the PC read the key itself); see PAD.C
			if	(getkey(kP)==YES)	break;
			}

/* clear paused status */
		while	( getkey(kP)==YES )
			{
			wait_for_vblank();
			at(0,10);
		 get_pad();	// RESTORE: the pad is read here (the PC read the key itself); see PAD.C
			if	( getkey(kP)==0)	break;
			}

   clear_replay_panel_count=2;
   foul_occured=tmp;
   draw_twice=2;
	Render();
	}
}
/* ------------------------------------------------------------------- */
int	load_file_herea( text *fname, ubyte *dest, long length );
void setup_show_potted_balls(void)
{
int tmp;

    replay_avalible = NO;
    draw_twice=2;
    Setup_New_Game_Type();

/* reset flag to draw number of shots icon */

    draw_foul_shot_icons = NO;
	 work_shot_position=0;

/* if in demo mode then load shot data */

	if ( DEMO_PLAY == YES )
    {
	  if (SNOOKER_MODE == YES)
      {
//g       tmp=load_file_herea("snkdemo.bin", &save_shots,0 );
      }
     else
      {
       if (LOCATION > DIFFERENT_SHAPED_TABLES_INDEX-1)
        {

/* load in fun table demo */

         switch ( LOCATION )
          {
    		  case 6: //beach
//g           tmp=load_file_herea("bchdemo.bin", &save_shots,0 );
           break;
    		  case 7: //coffin
//g           tmp=load_file_herea("cffdemo.bin", &save_shots,0 );
           break;
           case 9: //toy
//g           tmp=load_file_herea("toydemo.bin", &save_shots,0 );
           break;
          }   
        }
       else 
        {
         if ( TABLESIZE_TYPE == UK )
          {
//g           tmp=load_file_herea("ukdemo.bin", &save_shots,0 );
          }
         else
          {
//g           tmp=load_file_herea("usdemo.bin", &save_shots,0 );
	       }
        }
      }

     shot_position=tmp/sizeof(shot_positions_table);
     Re_Draw_Screen();
	 }
}
/* ------------------------------------------------------------------- */
/* table containing movement for cue */

double cue_movement[] = {
0.202800,0.202800,0.202800,0.204800,0.208800,0.210800,0.222800,0.232800,0.254800,
0.268800,0.294800,0.306800,0.308800,0.308800,0.302800,0.274800,0.266800,0.238800,
0.220800,0.200800,0.194800,0.192800,0.190800,0.190800,0.192800,0.194800,0.202800,
0.210800,0.220800,0.226800,0.234800,0.236800,0.238800,0.242800,0.242800,0.234800,
0.214800,0.090800,-0.027200};

void Show_potted_balls(void)
{
static long last_speed;
long	l,exit=NO,speed,count,loop,key;
long  save_foul;
long  save_frames;
double steves_angle;

   pot_count=0;

/* check to see if player want's to save this shot to disk */
   panel_flag=YES;

   clear_replay_panel_count=2;

/* setup correct message for replay last shot */

   foul_occured=0;

#ifdef CREATE_NEW_DEMO

 if ( DEMO_PLAY == NO )
   {
    if ( SNOOKER_MODE == YES ) 
     {
      saveafile("snkdemo.bin", &save_shots , sizeof( shot_positions_table )*shot_position );
	  }
    else
     {
      if (LOCATION > DIFFERENT_SHAPED_TABLES_INDEX-1)
       {
/* setup type if fun table demo */

        switch ( LOCATION )
         {
    		 case 6: //beach
          saveafile("bchdemo.bin", &save_shots , sizeof( shot_positions_table )*shot_position );
          break;

    		 case 7: //coffin
          saveafile("cffdemo.bin", &save_shots , sizeof( shot_positions_table )*shot_position );
          break;

          case 9: //toy
          saveafile("toydemo.bin", &save_shots , sizeof( shot_positions_table )*shot_position );
          break;
         }
       }
      else
       {

        if ( TABLESIZE_TYPE == UK )
         {
	       saveafile("ukdemo.bin", &save_shots , sizeof( shot_positions_table )*shot_position );
         }
        else
         {
	       saveafile("usdemo.bin", &save_shots , sizeof( shot_positions_table )*shot_position );
         }
       }
	  }
	}

#endif

/* if no balls potted in this show then dont show shot */

  while ( save_shots[work_shot_position].pot_shot == NO )
   {
    work_shot_position++;
    if (  work_shot_position > shot_position ) 
     {
      if ( DEMO_PLAY == YES)
       {
        exit_end_of_frame=YES;
        return;
    	 }
      Setup_New_Game_Type();
      work_shot_position=0;
     }
   }

/* play this shot must have potted a ball */

  steves_angle = save_shots[work_shot_position].steves_angle;
  butt =         save_shots[work_shot_position].butt ;
  speed_meter  = save_shots[work_shot_position].speed_meter ;
  x_hit_pos =    save_shots[work_shot_position].x_hit_pos;
  y_hit_pos =    save_shots[work_shot_position].y_hit_pos;

  for (loop=0;loop<LAST_BALL;loop++)
   {
    ball[loop].x =    save_shots[work_shot_position].x_pos[loop];
    ball[loop].y =    save_shots[work_shot_position].y_pos[loop];
    ball[loop].x_prev = save_shots[work_shot_position].x_pos[loop];
    ball[loop].y_prev = save_shots[work_shot_position].y_pos[loop];
    ball[loop].active = save_shots[work_shot_position].active[loop];
   }

/* setup cue from angle used in game */

  cue_rot.y=save_shots[work_shot_position].cue_angle;

  work_shot_position++;
  if (  work_shot_position > shot_position )
   {
    if ( DEMO_PLAY == YES) { exit_end_of_frame=YES; return; }
    work_shot_position=0;
    Setup_New_Game_Type();
    return;
   }


/* setup camera in a different position for every shot */

   setup_end_of_frame_camera();

/* animate cue before shot */

  count=0;
  shoot_mode=0;
  show_shot_flag=NO;
  replay_flag=YES;
  while ( count < 39 )
   {

   if ( DEMO_PLAY == YES ) panel_flag=YES;

    clear_replay_panel_count=2;
    refresh_whole_screen=2;
    cue_position=cue_movement[count];

/* setup cue in currect positon */

    if ( cue_position < FIX24_TO_DOUBLE(ball[WHITE_BALL].radius) )	// RESTORE: 8.24
     {
      cue_position = FIX24_TO_DOUBLE(ball[WHITE_BALL].radius);	// RESTORE: 8.24
     }

    Handle3D();
    count+=2;
   }

/* ------------------------- */
/* main replay loop */

   show_shot_flag=YES;
   last_speed=frames;

/* exit replay last shot */

   refresh_whole_screen=2;

   hit_ball(WHITE_BALL, x_hit_pos, y_hit_pos, (double)steves_angle , (double)butt , DOUBLE_TO_FIX24(speed_meter));	// RESTORE: 8.24

   while	( exit==NO )
    {
     clear_replay_panel_count=2;

/* allow player to replay balls if they have finnished */

     if	( (any_balls_moving()==FALSE) )	
      {
       exit=YES;
      }
     else
      {
	    speed=frames-last_speed;
		 last_speed=frames;
		 for (count=0;count < speed;count++) 
        {
         update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24
        }
	   }

	  Handle3D();

     key=find_which_key_input();

     if ( (key!=0) or ( mouse_buttons!=0) )
      {
       exit_end_of_frame=YES;
       exit=YES;
      }
	 }

/* stop all balls moving if ecaped shot */

    while	( (any_balls_moving()==TRUE) )	
     {
      update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24
	  } 
}               
/* ------------------------------------------------------------------- */
void setup_points_game(void)
{
	switch	(GAME_TYPE)
    {

	  case	US_POOL:
      if ( TOURNAMENT_PLAY == NO )
       {
        points_game=YES;
       }
      else
       {
        points_game=NO;
		 }
      break;

      default:
      points_game=NO;
      break;
    }
}
/* ------------------------------------------------------------------- */
void setup_table_friction(void)
{
	switch	( TABLESIZE_TYPE ) 
    {
     case US:
/* setup friction for table */
      switch (TABLE_FRICTION)
       {
        case 0:
        ball_mass = FIX24(0.100);	// RESTORE: 8.24
		  break;
        case 1:
        ball_mass = FIX24(0.145);	// RESTORE: 8.24
		  break;
        case 2:
        ball_mass = FIX24(0.190);	// RESTORE: 8.24
		  break;
       }
	  break;

/* uk table */

     case UK:
      switch (TABLE_FRICTION)
       {
        case 0:
/* setup friction for table */
        ball_mass = FIX24(0.100);	// RESTORE: 8.24
        break;
        case 1:
        ball_mass = FIX24(0.135);	// RESTORE: 8.24
        break;
        case 2:
        ball_mass = FIX24(0.170);	// RESTORE: 8.24
        break;
       }
	  break;
    }
}
/* ------------------------------------------------------------------- */

void setup_CD_track_for_fun_tables(void)
{
long tune;
//   if (LOCATION < DIFFERENT_SHAPED_TABLES_INDEX) return;

     switch ( LOCATION )
      {
		 case POTTERS_CLUB: 
       tune= 4;
       break;
		 case RACKERS:
       tune= 5;
       break;
		 case POOL_HALL:
       tune= 2;
       break;
		 case ARENA: 
       tune= 6;
       break;

		 case 4: //moon
       tune= 12;
       break;
		 case 5: //ice
       tune= 14;
       break;
		 case 6: //beach
       tune= 15;
       break;
		 case 7: //crypt
       tune= 16;
       break;
		 case 8: //swamp
       tune= 18;
       break;
		 case 9: //toy
       tune= 13;
       break;
      }

//g   start_new_track(tune);
}
