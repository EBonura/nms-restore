/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/*									    	 QD-UP      										 */
/*											PC-CD ROM											 */
/*									 For BMG	Interactive										 */
/*									 By NMS Software Ltd.									 */
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#include	"globals.h"
#include	"trick.lst"

#define	SCR2	512
static	long	player_1namexpos;
static	long	player_2namexpos;
static	long	xsize;

int	hi_speed_font(int	start_x,int	start_y,char	*text, int az,int azdot,int azsize,int az1,int azdot1);
long PANEL_OFFSET;
/* -------------------------------------------------------------------------*/
enum	{	OPT_GREEN,
			OPT_RED,
			OPT_WHITE,
			OPT_YELLOW,
			OPT_ORANGE,
			OPT_PURPLE,
			OPT_BLUE,
			OPT_GREY,
			OPT_CYAN,
			OPT_DARKGREY,
			MAX_OPTIONS_COLS
		};
/* -------------------------------------------------------------------------*/
ubyte	white_contrast_320200[ 3 * 3 ]=
{
	36,	36,	41,
	63,	63,	70,
	91,	91,	100
};
/* -------------------------------------------------------------------------*/
typedef	struct	{

	ubyte	r_mlt;
	ubyte	g_mlt;
	ubyte	b_mlt;
	ubyte	pen1;
	ubyte	pen2;
	ubyte	pen3;

} pen320struct;
/*------------------------------------------------------------------------------------------------------------------------------*/
pen320struct	pen_conv_320200[ MAX_OPTIONS_COLS ]={

	{	  0, 63,  0	},	//	green
	{	 63,  0,  0	},	//	red
	{	 63, 63, 63	},	//	white
	{	 63, 63,  0	},	//	yellow
	{	 63, 31,  0	},	//	orange
	{	 63,  0, 63	},	//	purple
	{	  0,  0, 63	},	//	blue
	{	 42, 42, 42	},	//	grey
	{	  0, 63, 63	},	//	cyan
	{	 21, 21, 21	},	//	darkgrey
};
/* -------------------------------------------------------------------------*/
void	draw_640x480_screen(  char *source )
{
	int i;
	screen_buffer=source;

	for(i=0;i<480;i++)
	{
//g		asm_add_bg_line(source+i*640,i*SVGA_logical_length,640);
	}
}
/* -------------------------------------------------------------------------*/
void	draw_640x480_screen1(  char *source )
{
	int i;

	screen_buffer1=source;
	for(i=0;i<480;i++)
	{
//g	 asm_add_bg_line(source+i*640,(512+i)*SVGA_logical_length,640);
	}
}
/* -------------------------------------------------------------------------*/
#if 0
unsigned char frame_buffer[65536*4];
int bytes_saved;
void screen_dump(int start_x,int start_y,char *source,int width,int height)
{
	int y;
	long index;
	for(y=0;y<height;y++)
	{
		index=(SVGA_logical_length,640*(y+start_y))+start_x;
		screen_dump_line(source,index,width);
		source+=iff_width;
	}
}
#endif
/* ------------------------------------------------------------------------ */
void	SetupPanel(void)
{

   if	(RESOLUTION == LOW) PANEL_OFFSET=0;
      else PANEL_OFFSET=CLIPPED-16;

/* draw status panel for current resolution mode */
   if ( RESOLUTION == LOW ) 
	 {
/* draw low - res panel to 'backscreen_buffer' */
     Low_Res_Panel();
    }
   else
    {
/* draw hi - res panel to screen */
     Hi_Res_Panel();
    }
}
/* ------------------------------------------------------------------------ */
void Low_Res_Panel(void)
{
	long	width,x,y,pheight,speed;
static   long flash_demo,frame_diff;

   speed = frames - frame_diff;
   frame_diff = frames;

   if ( (speed < 0) or (speed>10) ) speed=0;

   flash_demo+=speed;
   flash_demo%=28;

/* draw number of shots left for current player */
  
   if ( (show_shot_flag == NO) && (show_saved_shot_flag==NO) )
    {
     draw_number_of_shots_icons();
    }

/* ------------------------------- */
/* draw amplify graphic if enabled */

   if ( amplify == YES )
    {
     draw_amplify();
    }

/* ---------------------------- */
/* draw icon to show if connected to a external device */

	draw_connection();

	if	( getkey_jp(kSPACE)==YES ) 
    {
	  panel_flag^=1;
     if (panel_flag==NO) show_panel=0;
    }

   if (panel_flag==NO)
    {
     if ( show_panel == 0)
		{

       if (panel_scrolly < 25)
        {
         refresh_whole_screen=YES;
         panel_scrolly+=speed*1.3;
        }
		 else  
        {
         panel_scrolly=25;
         return;
        }
      }
	  else
      {
       refresh_whole_screen=YES;

       show_panel-=speed;
       if ( show_panel<0) show_panel=0;

       if (panel_scrolly > 0) panel_scrolly-=speed*1.3;
       if (panel_scrolly < 0) panel_scrolly=0;
		}
    }
	else
    {
     if (panel_scrolly > 0) {
       refresh_whole_screen=YES;
       panel_scrolly-=speed*1.3;
		}
     if (panel_scrolly < 0)  panel_scrolly=0;
    }
   
/* setup height of translucant panel */

    x=4;
    pheight=25;

	 width=viewport_width-(x*2);

    for	(y=0;y<pheight-1;y++) 
     screen_remap_line1(shad_tab,backscreen_buffer,x+(((200-pheight+panel_scrolly)+y)*viewport_width),x+(((239-PANEL_HEIGHT-pheight)+y)*viewport_width),width);

   if ( DEMO_PLAY == YES )   
    {
 	  draw_demo();
     return;
	 }

/* ------------------------------- */
/* draw quit frame screen */

     if ( QUIT_GAME_FLAG == YES )
      {
	    draw_quit_frame();
	    return;
      }

/* ---------------------------- */
/* replay panel to 3d buffer */
   if ( show_replay_flag == YES )
    {
     draw_end_of_frame();
	  return;
    }

  if	(show_saved_shot_flag==NO) 
   {
    draw_panel_bits();
	}

/* draw last foul to panel */

  draw_last_foul();

}
/* ------------------------------- */
void Hi_Res_Panel(void)
{
	long	y,temp;

/* clear replay panel */

   currentband=-1;

   clear_replay_panel();

	temp=0;
	if	(interlace==FALSE) temp=SCREEN2;

/* ------------------------------- */
/* draw amplify graphic if enabled */

   if ( amplify == YES )
    {
     draw_amplify();
    }

/* draw number of shots left for current player */

   if ( (show_shot_flag == NO) && (show_saved_shot_flag==NO) )
    {
     draw_number_of_shots_icons();
    }

/* draw quit frame text */

   if ( QUIT_GAME_FLAG == YES )
    {
     height=CLIPPED;
     for	(y=0;y < height+16;y++) 
 	    screen_set_line(4+((y+(480-CLIPPED-(48+16)+(CLIPPED)))*viewport_width)+temp,640-8,0);
     draw_quit_frame();
	  return;
    }

/* replay panel to 3d buffer */

   if ( show_replay_flag == YES )
    {
	  draw_end_of_frame();
	  return;
    }

   if ( GAME_TYPE == SPEED_POOL )
    {
     height=CLIPPED;
     for	(y=0;y < height+16;y++) 
      screen_set_line(460+((y+(480-CLIPPED-(48+16)+(CLIPPED)))*viewport_width)+temp,180,0);
	 }

    if (old_player_turn != Player_Active)
     {

      height=CLIPPED;
      for	(y=0;y < height+16;y++) 
 	    screen_set_line(4+((y+(480-CLIPPED-(48+16)+(CLIPPED)))*viewport_width)+temp,640-8,0);

      if (old_turn_count==0) old_player_turn=Player_Active; 
       else old_turn_count--;
	  }

   draw_last_foul();

	if	(show_saved_shot_flag==YES) return;

   draw_panel_bits();

   if ( old_turn_count > 0) return;

   if ( clear_replay_panel_count > 0 ) return;
   if ( draw_twice > 0) draw_twice--;
}
/* ------------------------------------------------------------------------ */
void	make_shade(ubyte	*tab,long shad)
{
	byte	r, g, b;
	long	p, s;
	long	start=0;
	long	end=256;

	for ( s=0; s<end; s++ ) tab[ s ] = 0;

	for	(p=0;p<256;p++)
			{
					r = (pallettebuf[ p * 3 + 0 ]*shad)/255;
					g = (pallettebuf[ p * 3 + 1 ]*shad)/255;
					b = (pallettebuf[ p * 3 + 2 ]*shad)/255;
//g					tab[ p ] = (( ubyte )( find_a_close_colour(
//g						r, g, b, pallettebuf, start, end )));
		}
}
/* -------------------------------------------------------------------------*/
long	save_type;
long	flick;
void	draw_panel_bits(void)
{

/* draw snooker panel */

   if ( SNOOKER_MODE == YES )
    {
     draw_player_turn_arror();
	  snooker_panel();
     return;
    }

/* draw all pool game types panel */

	switch	(GAME_TYPE)	{
	  	case	UK_POOL:
			draw_player_turn_arror();
			uk_pool_panel();
			break;

		case	US_8BALL:
			draw_player_turn_arror();
			us_pool_panel();
			break;

		case	US_POOL:
			draw_player_turn_arror();
			nine_ball_panel();
			break;

		case	CONTINUOUS:
         continuous_panel();
			break;

		case	SPEED_POOL:
			speed_pool_panel();
			break;

		case	KILLER_POOL:
			killer_pool_panel();
			break;

	  	case	NMS_POOL:
			draw_player_turn_arror();
			nms_pool_panel();
			break;

	  	case	TRICK_SHOT:
			draw_trick_shot_name();
			break;
		}
}
/* -------------------------------------------------------------------------*/
void snooker_panel(void)
{
//g	screen_andor_line(0,0,0);
	draw_player_names();
	draw_scores_snooker();
	draw_frames_won(0);
	draw_snooker_balls();
}
/*------------------------------------------------------------------------------------------------------------------------------*/
void continuous_panel(void)
{
//g	screen_andor_line(0,0,0);
	draw_player_names();
	draw_scores_snooker();
	draw_frames_won(0);
}
/*------------------------------------------------------------------------------------------------------------------------------*/
long ball_order_table[] =  { 0,4,5,2,3,6,6,6 };
void  draw_snooker_balls(void)
{
	long	xp,temp,count,count1;
   long tmp=1;

/* setup ball type for snooker panel */

   if ( SNOOKER_SHOOT_MODE == POT_COLOUR_BALL )
    {
	  tmp = 7;
    }
   else tmp=1; /* pot red ball */

	count=1;
	count1=0;

/* check to see if tracking line are active, if so then use old ball
positions */

   if (activated_lines==FALSE) 
    {
     while ( ball[count].active==NO)
      {
	    count++;
      } 
    }
   else
    {
     while ( saveball_lines[count].active==NO)
      {
	    count++;
      } 
    }

/* check to see if all 16 red balls are potted, if so check coloured balles */
    if ( count >= 15 )
    {
     tmp = ball_order_table[count-16];
    }

	if	(Player_Active==0)	xp=4;
	else	xp=320-22-4;

	temp=0;
	if	(interlace==FALSE) temp=SCREEN2;

	if	(RESOLUTION==LOW) 
    {
     draw_gfx_block(tmp,xp+(((200-24)+panel_scrolly)*320),ball_data,0,0,COPY_TO_RAM );
    }
	else
    {
     draw_gfx_block(tmp,xp*2+((480-(24*2)-CLIPPED+PANEL_OFFSET)*SVGA_logical_length)+temp,ball_data,0,0,DUMP_TO_SCREEN );
    }
}
/* -------------------------------------------------------------------------*/
void	draw_trick_shot_name(void)
{
         long	temp;
static   long	trick_len;
	char	trick[80];

   sprintf( trick,"%s", trick_names[trick_name_selected] );

	temp=0;
	if	(interlace==TRUE	) temp=SCR2;

	if	(RESOLUTION==LOW) {
    draw_text_low( (320-trick_len)/2,200-20,&trick[3],0);
	 trick_len=len;
    }
	else	hi_speed_font((640-trick_len)/2,(480-44-CLIPPED)+PANEL_OFFSET+temp,&trick[3], 2,46,40,77,133 );
}
/* -------------------------------------------------------------------------*/
void	draw_player_turn_arror(void)
{
	long	xpos,num,temp;

	temp=0;
	if	(interlace==FALSE) temp=SCR2;

	if	(RESOLUTION==LOW)
		{
		if	(Player_Active==0) { xpos=(viewport_width/2)-(36); num=14;}
		else	{xpos=(viewport_width/2)+(26); num=13;}
	}
	else 
		{
		if	(Player_Active==0) { xpos=(640/2)-(66); num=14;}
		else	{xpos=(640/2)+(52); num=13;}
	}
   currentband=-1;
	if	(RESOLUTION==LOW)	draw_number(num,xpos,22-panel_scrolly);
	else	draw_number(num,xpos,480-46+temp-CLIPPED+PANEL_OFFSET);
}
/* -------------------------------------------------------------------------*/
void	us_pool_panel(void)
{
	long	xp,count,loop,ballgfx,temp;

#define	SPT_BAL	10
#define	STP_BAL	2
#define	BLK_BALL	9

	temp=0;
	if	(interlace==FALSE) temp=SCREEN2;

	draw_frames_won(0);
	draw_player_names();

/* if no balls potted then return and and dont dissplay graphics*/

	count=0;
  
	for	(loop=1;loop<LAST_BALL;loop++)	
    {
     if	( ball[loop].active==NO )  count++;	
	 }

	if	(count==0)
    { 
     players[0].ball_type=NULL;
     players[1].ball_type=NULL;
     return;
    }

	xp=4;

/* draw player 1 ball type */

	if	(players[0].ball_type!=NULL)	{

	count=0;

   if (activated_lines==NO)
    {

     for	(loop=1;loop<LAST_BALL;loop++)	
      {
       if	( (ball[loop].obj_type == p1_stored_pot_type) && (ball[loop].active==NO) )
        {
	      count++;	
        }
	   }

	  if	(count >= 7 ) 
      { 
       players[0].ball_type=BLACK;
      }
     else 
      {
       players[0].ball_type=p1_stored_pot_type;
      }

    }

	switch	(players[0].ball_type)	{

	case	RED:
		ballgfx=SPT_BAL;
		break;
	case	YELLOW:
		ballgfx=STP_BAL;
		break;
	case	BLACK:
		ballgfx=BLK_BALL;
		break;
	}

    if ( nominate_colour == YES )
     {
		ballgfx = 19; /* draw red ball with L */
     }

	if	(RESOLUTION==LOW) 	draw_gfx_block(ballgfx,xp+((200-24-PANEL_HEIGHT+panel_scrolly)*320),ball_data,0,0,COPY_TO_RAM );
			else 	draw_gfx_block(ballgfx,xp*2+((464-(24*2)-PANEL_HEIGHT)*SVGA_logical_length)+temp,ball_data,0,0,DUMP_TO_SCREEN );
	}

/* draw player 2 ball type */
	if	(players[1].ball_type!=NULL)	{
	xp=320-22-4;

	count=0;
	for	(loop=1;loop<LAST_BALL;loop++)	{
	if	( (ball[loop].obj_type == p2_stored_pot_type) && (ball[loop].active==NO) )
	count++;	
	}

   if (activated_lines==NO)
    { 
	  if	(count >=7) 
      { 
       players[1].ball_type=BLACK;
      }
     else 
      {
       players[1].ball_type=p2_stored_pot_type;
      }
    }

	switch	(players[1].ball_type)	{

	case	RED:
		ballgfx=SPT_BAL;
		break;
	case	YELLOW:
		ballgfx=STP_BAL;
		break;
	case	BLACK:
		ballgfx=BLK_BALL;
		break;
	}

    if ( nominate_colour == YES )
     {
		ballgfx = 20; /* draw red ball with L */
     }


	if	(RESOLUTION==LOW) 	draw_gfx_block(ballgfx,xp+((200-24-PANEL_HEIGHT+panel_scrolly)*320),ball_data,0,0,COPY_TO_RAM );
	else 	draw_gfx_block(ballgfx,xp*2+((464-(24*2)-PANEL_HEIGHT)*SVGA_logical_length)+temp,ball_data,0,0,DUMP_TO_SCREEN );
	}
}
/* -------------------------------------------------------------------------*/
void	uk_pool_panel(void)
{
	long	xp,ballgfx,count,loop,temp;
	temp=0;
	if	(interlace==FALSE) temp=SCREEN2;

#define	YEL_BAL	0
#define	RED_BAL	1

	draw_frames_won(0);
	draw_player_names();

/* if no balls potted then return and and dont dissplay graphics*/

	count=0;
	for	(loop=1;loop<LAST_BALL;loop++)	
    {
     if	( ball[loop].active==NO )	count++;	
	 }
	if	(count==0) 
    {
     players[0].ball_type=NULL;
     players[1].ball_type=NULL;
     return;
    }

	xp=4;

/* draw player 1 ball type */

	if	(players[0].ball_type!=NULL)	
    {
	  count=0;
	  for	(loop=1;loop<LAST_BALL;loop++)	
      {
	    if	( (ball[ loop ].obj_type == p1_stored_pot_type) && (ball[loop].active==NO) )
	    count++;	
		}

/* if no balls potted then return and and dont dissplay graphics*/

    if (activated_lines==NO)
     {

      if	( (count >=7) && (potted_red_wrong_flag==NO) ) 
       { 
	     players[0].ball_type=BLACK;
        if	(UK_TWO_SHOTS==YES) Two_Shots=0;
       }
      else 
       {
        players[0].ball_type=p1_stored_pot_type;
       }

     }

     switch	(players[0].ball_type)	
      {
		case	YELLOW:
			ballgfx=YEL_BAL;
			break;
		case	RED:
			ballgfx=RED_BAL;
			break;
		case	BLACK:
			ballgfx=BLK_BALL;
			break;
		}

    if ( nominate_colour == YES )
     {
		ballgfx = 17; /* draw red ball with L */
     }

	 if	(RESOLUTION==LOW) 	draw_gfx_block(ballgfx,xp+((200-24-PANEL_HEIGHT+panel_scrolly)*320),ball_data,0,0,COPY_TO_RAM );
			else 	draw_gfx_block(ballgfx,xp*2+((464-(24*2)-PANEL_HEIGHT)*SVGA_logical_length)+temp,ball_data,0,0,DUMP_TO_SCREEN );
	}

/* draw player 2 ball type */

	if	(players[1].ball_type!=NULL)	{

	xp=320-22-4;
	count=0;

	for	(loop=1;loop<LAST_BALL;loop++)	
    {
	  if	( (ball[loop].obj_type == p2_stored_pot_type) && (ball[loop].active==NO) )
	  count++;	
	 }

   if (activated_lines==NO)
    {

     if	( (count >=7) && (potted_red_wrong_flag==NO) ) 
      {
       players[1].ball_type=BLACK;
       if	(UK_TWO_SHOTS==YES) Two_Shots=0;
      }
     else 
      {
       players[1].ball_type=p2_stored_pot_type;
      }
    }

	switch	(players[1].ball_type)	{

	case	YELLOW:
		ballgfx=YEL_BAL;
		break;
	case	RED:
		ballgfx=RED_BAL;
		break;
	case	BLACK:
		ballgfx=BLK_BALL;
		break;
	}

    if ( nominate_colour == YES )
     {
		ballgfx = 18; /* draw red ball with L */
     }

	if	(RESOLUTION==LOW) 	draw_gfx_block(ballgfx,xp+((200-24-PANEL_HEIGHT+panel_scrolly)*320),ball_data,0,0,COPY_TO_RAM );
	else 	draw_gfx_block(ballgfx,xp*2+((464-(24*2)-PANEL_HEIGHT)*SVGA_logical_length)+temp,ball_data,0,0,DUMP_TO_SCREEN );
	}
}
/* -------------------------------------------------------------------------*/
void	nms_pool_panel(void)
{
	long	xp,ballgfx,count,loop,temp;

	temp=0;
	if	(interlace==FALSE) temp=SCREEN2;

	draw_frames_won(0);
	draw_player_names();
	draw_scores();


	count=0;
	for	(loop=1;loop<LAST_BALL;loop++)	
    {
     if	( ball[ loop ].active==NO )	count++;	
	 }

	if	(count==0) 
    {
     players[0].ball_type=NULL;
     players[1].ball_type=NULL;
     return;
    }

/* if no balls potted then return and and dont dissplay graphics */

	count=0;
	for	(loop=1;loop<LAST_BALL;loop++)	{
    if	( ball[loop].active==NO )	count++;	
	}

	if	(count==0)
    {
     players[0].ball_type=NULL;
     players[1].ball_type=NULL;
     return;
    }

	xp=4;

/* draw player 1 ball type */

	if	(players[0].ball_type!=NULL)	{
		count=0;
		for	(loop=1;loop<LAST_BALL;loop++)	{
		if	( (ball[loop].obj_type == players[0].ball_type) && (ball[loop].active==NO) )
		count++;	
		}

/* if no balls potted then return and and dont dissplay graphics*/

    if	(players[0].ball_type==BLACK)	count=7;

    if (activated_lines==NO)
     {
      if	( (count >=7) && (potted_red_wrong_flag==NO) ) 
       { 
        players[0].ball_type=BLACK;
       }
     }


	switch	(players[0].ball_type)	{
		case	YELLOW:
			ballgfx=YEL_BAL;
			break;
		case	RED:
			ballgfx=RED_BAL;
			break;
		case	BLACK:
			ballgfx=BLK_BALL;
			break;
		}

		if	(RESOLUTION==LOW) 	draw_gfx_block(ballgfx,xp+((200-24-PANEL_HEIGHT+panel_scrolly)*320),ball_data,0,0,COPY_TO_RAM );
			else 	draw_gfx_block(ballgfx,xp*2+((464-(24*2)-PANEL_HEIGHT)*SVGA_logical_length)+temp,ball_data,0,0,DUMP_TO_SCREEN );
	}

/* draw player 2 ball type */

	if	(players[1].ball_type!=NULL)	{

	xp=320-22-4;
	count=0;

	for	(loop=1;loop<LAST_BALL;loop++)	{
	if	( (ball[loop].obj_type == players[1].ball_type) && (ball[loop].active==NO) )
	count++;	
	}

	if	(players[1].ball_type==BLACK)	count=7;

   if (activated_lines==NO)
    {
     if	( (count >=7) && (potted_red_wrong_flag==NO) )
      { 
       players[1].ball_type=BLACK;
       if	(UK_TWO_SHOTS==YES) Two_Shots=0;
      }
    }

	switch	(players[1].ball_type)	{

	case	YELLOW:
		ballgfx=YEL_BAL;
		break;
	case	RED:
		ballgfx=RED_BAL;
		break;
	case	BLACK:
		ballgfx=BLK_BALL;
		break;
	}

	if	(RESOLUTION==LOW) 	draw_gfx_block(ballgfx,xp+((200-24-PANEL_HEIGHT+panel_scrolly)*320),ball_data,0,0,COPY_TO_RAM );
	else 	draw_gfx_block(ballgfx,xp*2+((464-(24*2)-PANEL_HEIGHT)*SVGA_logical_length)+temp,ball_data,0,0,DUMP_TO_SCREEN );
	}
}
/* -------------------------------------------------------------------------*/
void	killer_pool_panel(void)
{
	draw_player_names_killer();
	draw_killer_lives();
}
/* -------------------------------------------------------------------------*/
void	draw_killer_lives(void)
{
	long	loop,x,pcount,temp,y;

	temp=0;
	if	(interlace==FALSE) temp=SCREEN2;

	if	(RESOLUTION==LOW)	x=(viewport_width/2) - (((3*NUMBER_OF_LIVES_IN_KILLER + 5) * NUMBER_OF_PLAYERS_IN_KILLER) /2);
	else	x=(640/2) - (((6*NUMBER_OF_LIVES_IN_KILLER + 5) * NUMBER_OF_PLAYERS_IN_KILLER) /2);


	for	(pcount=0; pcount< NUMBER_OF_PLAYERS_IN_KILLER ;pcount++)	{

	for	(loop=0;loop< lives_left[pcount] ;loop++)	{

	if	(RESOLUTION==LOW)
    {
     if	(pcount == killer_turn)
      {
       for (y=0;y<10;y++)
       screen_set_line1(x+(((200-PANEL_HEIGHT)-24+panel_scrolly+y)*viewport_width), 2,9);
      }
     else
      {
       for (y=0;y<10;y++)
       screen_set_line1(x+(((200-PANEL_HEIGHT)-24+panel_scrolly+y)*viewport_width), 2,23);
      }
 
       x+=4;
    }
   else
    {
     if	(pcount == killer_turn)
      {
       for (y=0;y<20;y++)
       screen_set_line(temp+x+(((480-PANEL_HEIGHT)-48-12+y)*viewport_width), 4,9);
      }
     else
      {
       for (y=0;y<20;y++)
       screen_set_line(temp+x+(((480-PANEL_HEIGHT)-48-12+y)*viewport_width), 4,23);
      }
       x+=8;
	 }
	}

    if ( RESOLUTION==LOW)   x+=4;
      else x+=8;
  }
}
/* -------------------------------------------------------------------------*/
void	draw_player_names_killer(void)
{
	char	name[80];	
   long temp;

/* show killer player name */

   if ( foul_occured > 0) return;

	temp=0;
	if	(interlace==FALSE) temp=SCR2;

	strcpy(name,&player_names[killer_order[killer_turn]] );

	if	(RESOLUTION==LOW) 
    {

     draw_text_len(0,0,&name,0);
	  player_1namexpos=160-(len/2);
     draw_text_low(player_1namexpos,200-12-PANEL_HEIGHT+panel_scrolly,&name,0);
    }
	else 
    {
    draw_text_len(0,temp+480-47-CLIPPED+PANEL_OFFSET,&name,0);
	  player_1namexpos=(640-len)/2;
    draw_text(player_1namexpos,temp+(480-12-PANEL_OFFSET),&name,0);
	 }
}													
/* -------------------------------------------------------------------------*/
int	hi_speed_font(int	start_x,int	start_y,char	*text, int az,int azdot,int azsize,int az1,int azdot1)
{
	long	a;
	long	x,xsize;
	char	chr;
	int	len=0;
	long	dot,azpos,saveaz=az;

	while	( *text != 0 )	{

   chr	= *text++;

	x=0;

	/* find beging of charcter */

	if	( ( chr >='A' ) || ( chr ==' ' ) )	{

	if	(chr != ' ')	{

	if	( (chr >= 'O') && (azsize > 30) )
   {
	dot=azdot1;
	azpos=az1;
	az=az1;
	chr=chr-'O';
	}
	else	{
	chr=chr-'A';
	dot=azdot;
	azpos=az;
	}

	for	(a = 0 ;a < chr ;a++)	{
	while	(*(speedtxt+x+(dot*640)) == 0)	x++;
	x++;
	while	(*(speedtxt+x+(dot*640)) == 0)	x++;
	x++;				
	}

	while	(*(speedtxt+x+(dot*640)) == 0)	x++;
	xsize=1;
	while	(*(speedtxt+xsize+x+(dot*640)) == 0)	xsize++;

	for	(a=0;a< azsize;a++)	{
		
//g	screen_andor_line(speedtxt+x+((az+a)*640),start_x+((start_y+a)*SVGA_logical_length),xsize);
	}
	}
	else	xsize=26;

	start_x+=xsize+1;
	len+=xsize+1;
	dot=azdot;
	azpos=az;
	az=saveaz;

	}

	else	{	

	if	(chr==':') chr=26;
	else	chr=chr-'0';

	dot=azdot;
	azpos=az;

	for	(a = 0 ;a < chr ;a++)	{

	while	(*(speedtxt+x+(azdot1*640)) == 0)	x++;
	x++;
	while	(*(speedtxt+x+(azdot1*640)) == 0)	x++;
	x++;				
	}

	while	(*(speedtxt+x+(azdot1*640)) == 0)	x++;
	xsize=1;
	while	(*(speedtxt+xsize+x+(azdot1*640)) == 0)	xsize++;

	for	(a=0;a<azsize;a++)	{
//g	screen_andor_line(speedtxt+x+((az1+a)*640),start_x+((start_y+a)*SVGA_logical_length),xsize);
	}

	start_x+=xsize+1;
	len+=xsize+1;
	}
	}

	return	len;
}
/* -------------------------------------------------------------------------*/
void	speed_pool_panel(void)
{
	Draw_BigTimer();
	draw_player_names_speed();
}
/* -------------------------------------------------------------------------*/
void	nine_ball_panel(void)
{
//g	screen_andor_line(0,0,0);
	draw_player_names();
	draw_scores();
	draw_frames_won(0);
	draw_9_balls();
}
/* -------------------------------------------- */
void	update_timer(void)
{
	long	c,c1;
/* dont update clock if replaying last shot */

	if	(replay_flag==YES) return;

	/* check to see if all balls are pocketed? */

	c1=0;
	for	(c=1;c<LAST_BALL;c++)
    {
     if	( ball[c].active == NO) c1++;
    }

  	if	(c1 >= LAST_BALL-1) return; /* if so stop timer */

	if	( (start_time==YES) && (game_over == NO) )
    {
		if	( (time_value[0]++) >= 59) {
		time_value[1]++;
		time_value[0]=0;
		}
	 }
}
/* -------------------------------------------------------------------------*/
void    Draw_BigTimer(void)
{
	long    x,num,temp;
	char	buffer[60];
	ubyte	*time_str;
	ubyte	time_buffer[4];

#define	time_x	280
#define	time_y	20

/* only draw timer in speed pool game type */

	temp=0;
	if	(interlace==FALSE) temp=SCR2;

	if	(RESOLUTION==LOW)	x=time_x;
	else	x=time_x*2+16;

/* draw time to screen */
	time_buffer[0]='0';
	time_buffer[1]=NULL;

	time_str=ltoa(time_value[0],buffer,10);
	if	( strlen(time_str) > 1) strcpy(time_buffer,time_str);
	else	strcat(time_buffer,time_str);

/* draw seconds */
		num = (time_buffer[0])-'0';
		if	(RESOLUTION==LOW)	draw_number(num,x,time_y-panel_scrolly);
			else	draw_number(num,x,(480-44-CLIPPED+PANEL_OFFSET)+temp);

		x+=xsize+1;

		num = (time_buffer[1])-'0';
		if	(RESOLUTION==LOW)	draw_number(num,x,time_y-panel_scrolly);
			else	draw_number(num,x,(480-44-CLIPPED+PANEL_OFFSET)+temp);


/* draw colon */
	if	(RESOLUTION==LOW)	x=time_x-10;
	else	x=(time_x-5)*2+16;

	num = 10;
		if	(RESOLUTION==LOW)	draw_number(num,x,time_y-panel_scrolly);
		else	draw_number(num,x,(480-44-CLIPPED)+PANEL_OFFSET+temp);

/*----------------*/
/* draw minutes */

	if	(RESOLUTION==LOW)	x=time_x-34;
	else	x=(time_x-34)*2+16;

	time_buffer[0]='0';
	time_buffer[1]=NULL;

	time_str=ltoa(time_value[1],buffer,10);
	if	( strlen(time_str) > 1) strcpy(time_buffer,time_str);
	else	strcat(time_buffer,time_str);

		num = (time_buffer[0])-'0';
		if	(RESOLUTION==LOW)	draw_number(num,x,time_y-panel_scrolly);
			else	draw_number(num,x,(480-44-CLIPPED+PANEL_OFFSET)+temp);
		x+=xsize+1;

		num = (time_buffer[1])-'0';
		if	(RESOLUTION==LOW)	draw_number(num,x,time_y-panel_scrolly);
			else	draw_number(num,x,(480-44-CLIPPED+PANEL_OFFSET)+temp);

}
/*------------------------------------------------------------------------------------------------------------------------------*/
void    draw_9_balls(void)
{
	long	xp,temp;

#define	nine_ballx 196
#define	nine_bally 5

   if ( draw_twice == 0 ) return;

	if	(Player_Active==0)	xp=4;
	else	xp=320-22-4;

	temp=0;
	if	(interlace==FALSE) temp=SCREEN2;

	if      (GAME_TYPE != US_POOL ) return;

	if	(RESOLUTION==LOW) 	draw_gfx_block(nine_ball_count+1,xp+(((200-24)+panel_scrolly)*320),ball_data,0,0,COPY_TO_RAM );
	else 	draw_gfx_block(nine_ball_count+1,xp*2+((480-(24*2)-CLIPPED+PANEL_OFFSET)*SVGA_logical_length)+temp,ball_data,0,0,DUMP_TO_SCREEN );
}
/* -------------------------------------------------------------------------*/
void	HI_CLS(void)
{
	long	y;
   if ( RESOLUTION==LOW) return;
   screen_set_line(0,0,0);
   for (y=0;y<1024;y++)
    screen_set_line(y*SVGA_logical_length,640,0);
}
/* -------------------------------------------------------------------------*/
void	draw_scores(void)
{

   if ( foul_occured > 0) return;

   if (points_game == NO) return;

	Update_Scores();
   draw_score(0);
   draw_score(1);
}

void	draw_scores_snooker(void)
{

   if ( foul_occured > 0) return;

	Update_Scores();
   draw_score(0);
   draw_score(1);
}

void draw_score(long player)
{
	long	num,count,xpos;
	ubyte	*first_score;
	char	buffer[60];
	long	temp,a;

	if	(player==0)	xpos = 80;
	else	xpos = 200;

	temp=0;
	if	(interlace==FALSE) temp=SCR2;

	if	(RESOLUTION==HIGH) xpos=xpos*2;

   if (RESOLUTION == HIGH) 
    {
     for	(a=0;a<21;a++)	
      {
       screen_set_line(xpos-1+((a+temp+480-25-CLIPPED+PANEL_OFFSET)*SVGA_logical_length) ,80,0);
      }
	 }

	first_score=ltoa(players_scores[player],buffer,10);

	count=0;
	while	(*(first_score+count) != NULL)	{

	num=*(first_score+count)-'0';	

	if	(RESOLUTION==LOW)	draw_number(num,xpos,12-panel_scrolly);
	else	draw_number(num,xpos,temp+480-32);

	count++;							
	xpos+=xsize+1;
	}
}
/* -------------------------------------------------------------------------*/
 long hires_numtab[100];
void	draw_number(long	num,long	xpos,long y)
{
	long	x,a;
	long	ny,ns,ys,psize,index,num1;
   ubyte *txtptr;


   txtptr = txt;

	if	(RESOLUTION==LOW)
	{
	draw_number_low(num,xpos, y);
	return;
	}

   if ( BUILD_HIRES_NUMS == YES )
    {

     psize=640;//viewport_width;

     ny=54;
     ns=30;
     ys=21;

     if	(GAME_TYPE==SPEED_POOL)	
      {
       ny=201;
       ns=156;
       ys=40;
       txtptr = speedtxt;

      }
     index=0;
     for (num1=0;num1 <= 9+10; num1++)
     {
     x=0;

     for	(a = 0 ;a < num1 ;a++)	
      {
       while	(*(txtptr+x+(ny*psize)) == 0)	x++;
       x++;
       while	(*(txtptr+x+(ny*psize)) == 0)	x++;
       x++;
      }

     while	(*(txtptr+x+(ny*psize)) == 0)	x++;
     xsize=1;
     while	(*(txtptr+xsize+x+(ny*psize)) == 0)	xsize++;
     hires_numtab[index*2]=x;
     hires_numtab[index*2+1]=xsize;
     index++;
	  }

	  BUILD_HIRES_NUMS = NO;
    }

	psize=640;

	ny=54;
	ns=30;
	ys=22;

	if	(GAME_TYPE==SPEED_POOL)	{
	ny=201;
	ns=156;
	ys=40;
   txtptr = speedtxt;
	}

   x  = hires_numtab[num*2];
   xsize  = hires_numtab[num*2+1];


    for	(a=0;a<ys;a++)	
     {
      screen_dump_line(txtptr+x+((a+ns)*psize),xpos+((y+a)*viewport_width) ,xsize);
     }

}
/* -------------------------------------------------------------------------*/
void	Setup_Scores(void)
{
	long	a;

	Scores[0][0]=0;
	Scores[0][1]=0;

	for	(a=1;a<10;a++)	Scores[a][0]=-1;
	for	(a=1;a<10;a++)	Scores[a][1]=-1;
}
/*-------------------------------*/
void	Update_Scores(	void)
{
 	if	(total[0][0]>0) {
	players_scores[0]+=total[0][0];	
	total[0][0]=0;
   show_panel++;
   }

 	if	(total[0][1]>0) {
	players_scores[1]+=total[0][1];	
	total[0][1]=0;
   show_panel++;
   }
}
/*-------------------------------*/
void	Add_Score_2_other_player( long score)
{
	if	(replay_flag==YES)	return;
   if (DEMO_PLAY==YES) return;

 	total[0][1-Player_Active]+=score;
}
/*-------------------------------*/
void	Add_Score( long score)
{
   if (points_game == NO) return;
	if	(replay_flag==YES)	return;
   if (DEMO_PLAY==YES) return;

 	total[0][Player_Active]+=score;
}
/*-------------------------------*/
void	Add_Score1( long score)
{
	if	(replay_flag==YES)	return;
   if (DEMO_PLAY==YES) return;
 	total[0][Player_Active]+=score;
}
/*-------------------------------*/
	long	pen[2];
#define	LOW_COL	40

void	draw_player_names(void)
{
long	temp,speed,y;

   screen_set_line(0,0,0);

   speed = frames - name_frame_diff;
   name_frame_diff = frames;

   if ( draw_twice == 0 ) return;

	temp=0;
	if	(interlace==FALSE) temp=SCR2;


   if ( TOURNAMENT_PLAY == NO )
    {
     name_ptr = shorten_name(left_player%16);
	 }
   else
    {
     name_ptr = shorten_name(tourn_left_player);
    }

	if	(RESOLUTION==LOW)	{
     draw_text_len(player_1namexpos,0,name_ptr,0);
	  player_1namexpos=(viewport_width/2)-(48+len);
     draw_text_low(player_1namexpos,height-22+panel_scrolly,name_ptr,0);
	}
	else	{

    draw_text_len(player_1namexpos,temp+480-47-CLIPPED+PANEL_OFFSET,name_ptr,0);
	 player_1namexpos=(640/2)-((35*2)+len);
    draw_text(player_1namexpos,temp+480-47-CLIPPED+PANEL_OFFSET,name_ptr,0);
	}


   if ( TOURNAMENT_PLAY == NO )
    {
	  name_ptr = shorten_name(right_player);
	 }
   else
    {
     name_ptr = shorten_name(tourn_right_player);
    }

	if	(RESOLUTION==LOW)	{
     draw_text_len(player_1namexpos,0,name_ptr,0);
     player_2namexpos=(viewport_width/2)+(40);

     draw_text_low(player_2namexpos,height-22+panel_scrolly,name_ptr,0);
	}
	else	{ 

// draw_text_len(player_2namexpos,temp+480-47-CLIPPED+PANEL_OFFSET,name_ptr,0);
	player_2namexpos=(640/2)+(35*2);
   draw_text(player_2namexpos,temp+480-47-CLIPPED+PANEL_OFFSET,name_ptr,0);
	}
}
/* -------------------------------------------------------------------------*/
void	draw_quit_frame(void)
{
static long old_frame;
	long	temp,speed,time=8,len1;

   old_player_turn=-1;
   old_turn_count=1000;
   speed=frames-old_frame;
   quit_flash_count+=speed+1;
   time=24+speed;
   quit_flash_count%=time;
   old_frame=frames;

/* make text flash */
   if (quit_flash_count > (time/2)) return;

	temp=0;
	if	(interlace==FALSE	) temp=SCR2;

   BUILD_LOW_TEXT = YES;

	if	(RESOLUTION==LOW) 
    {
     len_text(player_1namexpos,200-20,"QUIT YES OR NO",-1);
	  player_1namexpos=(viewport_width/2)-(len/2);
     draw_text_low(player_1namexpos,200-20+panel_scrolly,"QUIT YES OR NO",-1);
    }
	else	
    {
     draw_text_len(0,480,"QUIT YES OR NO",0);
     player_1namexpos=(640/2)-(len/2);
     draw_text(player_1namexpos,temp+480-42-CLIPPED+PANEL_OFFSET,"QUIT YES OR NO",0);
    }
   BUILD_LOW_TEXT = YES;
}
/* -------------------------------------------------------------------------*/
void	draw_end_of_frame(void)
{
static long old_frame;
long	temp,speed,time=8,len1;

   old_player_turn=-1;
   old_turn_count=1000;
   speed=frames-old_frame;
   quit_flash_count+=speed+1;
   time=100+speed;
   quit_flash_count%=time;
   old_frame=frames;

   if  ( (find_which_key_input() == NULL) ) 
    {
     if (quit_flash_count > (time/1.5)) return;
    }

	temp=0;
	if	(interlace==FALSE	) temp=SCR2;

	if	(RESOLUTION==LOW) 
    {
     len_text(player_1namexpos,200-23,"FRAME REPLAY",0);
	  player_1namexpos=(viewport_width/2)-(len/2);
     draw_text_low(player_1namexpos,200-23+panel_scrolly,"FRAME REPLAY",0);

     len_text(player_1namexpos,200-11,"PRESS ANY KEY TO QUIT",0);
	  player_1namexpos=(viewport_width/2)-(len/2);
     draw_text_low(player_1namexpos,200-12+panel_scrolly,"PRESS ANY KEY TO QUIT",0);
    }
	else	
    {
/* draw end of frame message */
     if ( DEMO_PLAY == NO )
      {
       draw_text_len(player_1namexpos,0,"FRAME REPLAY",0);
       player_1namexpos=(640/2)-(len/2);
       draw_text(player_1namexpos,temp+(480-44-CLIPPED+PANEL_OFFSET),"FRAME REPLAY",0);
		}
     else
      {
/* draw demo play message */
       draw_text_len(player_1namexpos,0,"DEMO REPLAY",0);
       player_1namexpos=(640/2)-(len/2);
       draw_text(player_1namexpos,temp+(480-44-CLIPPED+PANEL_OFFSET),"DEMO REPLAY",0);
		}

     draw_text_len(player_1namexpos,0,"PRESS ANY KEY TO QUIT",0);
     player_1namexpos=(640/2)-(len/2);
     draw_text(player_1namexpos,temp+(480-20-CLIPPED+PANEL_OFFSET),"PRESS ANY KEY TO QUIT",0);
    }
}
/* -------------------------------------------------------------------------*/
void	draw_demo(void)
{
static long old_frame;
	long	temp,speed,time=8,len1;


   old_player_turn=-1;
   old_turn_count=1000;
   speed=frames-old_frame;
   quit_flash_count+=speed+1;
   time=64+speed;
   quit_flash_count%=time;
   old_frame=frames;

   if  ( (find_which_key_input() == NULL) ) 
    {
     if (quit_flash_count > (time/2)) return;
    }

	temp=0;
	if	(interlace==FALSE	) temp=SCR2;

	if	(RESOLUTION==LOW) 
    {
     len_text(player_1namexpos,200-23,"DEMO PLAY",0);
	  player_1namexpos=(viewport_width/2)-(len/2);
     draw_text_low(player_1namexpos,200-23+panel_scrolly,"DEMO PLAY",0);

     len_text(player_1namexpos,200-11,"PRESS ANY KEY TO QUIT",0);
	  player_1namexpos=(viewport_width/2)-(len/2);
     draw_text_low(player_1namexpos,200-11+panel_scrolly,"PRESS ANY KEY TO QUIT",0);

    }
	else	
    {
     draw_text_len(player_1namexpos,0,"DEMO PLAY",0);
	  player_1namexpos=(640/2)-(len/2);
     draw_text(player_1namexpos,temp+(480-44-CLIPPED+PANEL_OFFSET),"DEMO PLAY",0);

     draw_text_len(player_1namexpos,0,"PRESS ANY KEY TO QUIT",0);
	  player_1namexpos=(640/2)-(len/2);
     draw_text(player_1namexpos,temp+(480-20-CLIPPED+PANEL_OFFSET),"PRESS ANY KEY TO QUIT",0);
    }
}
/* -------------------------------------------------------------------------*/
void	draw_player_names_speed(void)
{
	long	temp;

   if ( foul_occured > 0) return;

	name_ptr = shorten_name(left_player);
	temp=0;
	if	(interlace==FALSE	) temp=SCR2;

	if	(RESOLUTION==LOW) draw_text_low(10,200-20+panel_scrolly,name_ptr,0);
	else	hi_speed_font(20,(480-44-CLIPPED+PANEL_OFFSET)+temp,name_ptr, 2,46,40,77,133 );
}
/* -------------------------------------------------------------------------*/
 long hires_txt[100],hires_nms[100];
void	draw_text(int	start_x,int	start_y,char	*text, long play_pen)
{
	long	a;
	long	x,xsize;
	char	chr,save_chr;
	long	ay,ny,ys,ns,psize,index;

   if ( BUILD_HIRES_TEXT == YES )
    {
     index=0;
     for (chr='A' ; chr <= 'Z'+1 ;chr++)
      {
       psize=640;//viewport_width;

       ay=23;
       ny=54;
       ns=30;
       ys=21;

       save_chr = chr;

       x=0;

         switch	(chr)	
          {
           case	'.':
           chr=27;
           break;

           case ' ':
           chr=26;
           break;

           default:
           chr=chr-'A';
		     break;
	       }

       for	(a = 0 ;a < chr ;a++)	
        {
         while	(*(txt+x+(ay*psize)) == 0)	x++;
         x++;
         while	(*(txt+x+(ay*psize)) == 0)	x++;
         x++;
        }

       while	(*(txt+x+(ay*psize)) == 0)	x++;
       xsize=1;
       while	(*(txt+xsize+x+(ay*psize)) == 0)	xsize++;

       hires_txt[index*2]=x;
       hires_txt[index*2+1]=xsize;
       index++;
       chr=save_chr;
      } 

/* build nums */

     index=0;
     for ( chr='0' ; chr <= '9'; chr++)
      {
       x=0;
       save_chr = chr;
       chr=chr-'0';

       for	(a = 0 ;a < chr ;a++)	
        {
         while	(*(txt+x+(ny*psize)) == 0)	x++;
         x++;
         while	(*(txt+x+(ny*psize)) == 0)	x++;
         x++;					 
        }

       while	(*(txt+x+(ny*psize)) == 0)	x++;
       xsize=1;
       while	(*(txt+xsize+x+(ny*psize)) == 0)	xsize++;

       hires_nms[index*2]=x;
       hires_nms[index*2+1]=xsize;
       index++;
       chr=save_chr;

      }
				 
	  BUILD_HIRES_TEXT = NO ;
    }


	len=0;

	while	( *text != 0 )	{

   chr	= *text++;

	x=0;

	psize=640;//viewport_width;
	if	(RESOLUTION==LOW)	{
	ay=13;
	ny=30;
	ns=17;
	ys=11;


	if	(GAME_TYPE==SPEED_POOL)	{
	psize=640;
	ay=18;
	ny=43;
	ns=21;
	ys=17;
	}

	}
	else	{
	ay=23;
	ny=54;
	ns=30;
	ys=22;
	}

	/* find beging of charcter */

	if	((chr >='A') || (chr ==' '))	
    {

     switch	(chr)	
      {
       case	'.':
       chr=27;
       break;

       case ' ':
       chr=26;
       break;
	  
       default:
		 chr=chr-'A';
		 break;
	   }

     x     = hires_txt[chr*2];
     xsize = hires_txt[chr*2+1];

     for	(a=0;a<ys;a++)	
      {
       screen_dump_line(txt+x+(a*psize),start_x+((start_y+a)*viewport_width) ,xsize);
      }

      start_x+=xsize+1;
      len+=xsize+1;

	 }
   else	
    {

     chr=chr-'0';
     x = hires_nms[chr*2];
     xsize = hires_nms[chr*2+1];

     for	(a=0;a<ys;a++)	
      {
		 screen_dump_line(txt+x+(a*psize),start_x+((start_y+a)*viewport_width) ,xsize);
      }

     start_x+=xsize+1;
	 }

	}
}
/*------------------------------------------------------------------------------------------------------------------------------*/
void	len_text(int	start_x,int	start_y,char	*text, long play_pen)
{
	long	a;
	long	x,xsize;
	char	chr;
	long	ay,ny,ys,ns,psize;

	play_pen=play_pen;

	len=0;

	while	( *text != 0 )	{

   chr	= *text++;

	x=0;

	psize=640;//viewport_width;

	if	(RESOLUTION==LOW)	{
	ay=58;
	ny=30;
	ns=44;
	ys=9;


	if	( (GAME_TYPE==SPEED_POOL) && (play_pen!=-1) )
  	{
	psize=640;
	ay=18;
	ny=43;
	ns=21;
	ys=17;
	}

	}
	else	{
	ay=23;
	ny=54;
	ns=30;
	ys=21;
	}


	/* find beging of charcter */

	if	((chr >='A') || (chr ==' '))	{

	switch	(chr)	{
	case	'.':
			chr=27;
			break;

	case ' ':
		chr=26;
		break;

	default:
		chr=chr-'A';
		break;
	}

	for	(a = 0 ;a < chr ;a++)	{

	while	(*(txt+x+(ay*psize)) == 0)	x++;
	x++;
	while	(*(txt+x+(ay*psize)) == 0)	x++;
	x++;
	}

	while	(*(txt+x+(ay*psize)) == 0)	x++;
	xsize=1;
	while	(*(txt+xsize+x+(ay*psize)) == 0)	xsize++;

	start_x+=xsize+1;
	len+=xsize+1;

	}
	else	{

	chr=chr-'0';
	for	(a = 0 ;a < chr ;a++)	{

	while	(*(txt+x+(ny*psize)) == 0)	x++;
	x++;
	while	(*(txt+x+(ny*psize)) == 0)	x++;
	x++;					 
	}

	while	(*(txt+x+(ny*psize)) == 0)	x++;
	xsize=1;
	while	(*(txt+xsize+x+(ny*psize)) == 0)	xsize++;

	start_x+=xsize+1;
	}
	}
}
/*------------------------------------------------------------------------------------------------------------------------------*/
/* draw to screen number of games won */
void	draw_frames_won(long player)
{
#define	frame_x	124
#define	frame_y	4
#define	frame_y1	22
	long	x,num,count,temp;

	temp=0;
	if	(interlace==FALSE) temp=SCR2;

   if ( draw_twice == 0 ) return;

	player=player;

	count=0;
/* draw player 1 frames won */
	x = 160-22;
	num=Frames_played[count][0];	
	if	(RESOLUTION==LOW)	draw_number(num,x,23-panel_scrolly);
	else	draw_number(num,x*2,480-47-CLIPPED+PANEL_OFFSET+temp);

/* draw player 2 frames won */

	x = 160+16;
	num=Frames_played[count][1];	
	if	(RESOLUTION==LOW)	draw_number(num,x,23-panel_scrolly);
	else	draw_number(num,x*2,480-47-CLIPPED+PANEL_OFFSET+temp);

/* draw frames to play */

	x = 160-3;
	num=max_frame;
	if	(RESOLUTION==LOW)	draw_number(num,x,23-panel_scrolly);
	else	draw_number(num,x*2,480-47-CLIPPED+PANEL_OFFSET+temp);

	x = 155-4;
	num=11;
	if	(RESOLUTION==LOW)	draw_number(num,x,23-panel_scrolly);
	else	draw_number(num,x*2,480-47-CLIPPED+PANEL_OFFSET+temp);

	x = 167-2;
	num=12;
	if	(RESOLUTION==LOW)	draw_number(num,x,23-panel_scrolly);
	else	draw_number(num,x*2,480-47-CLIPPED+PANEL_OFFSET+temp);

}
/*------------------------------------------------------------------------------------------------------------------------------*/
/* draw turbo icon */
void	draw_turbo_icon(void)
{
	long	y;

/* show if turbo shot on/off*/
	if	(turbo_shot_mode==YES)	{
	for	(y=0;y<13;y++)
	memcpy(SA+196+((22+y)*320),panel+68+((y+12)*320),13);
	}
	else	{
	for	(y=0;y<13;y++)
	memcpy(SA+196+((22+y)*320),panel+81+((y+12)*320),13);
	}
}
/*------------------------------------------------------------------------------------------------------------------------------*/
void clear_replay_panel(void)
{
	long	y,temp;

   if (RESOLUTION==LOW ) return;

   if ( clear_replay_panel_count <= 0 ) return;
   clear_replay_panel_count--;
   draw_twice=2;
	temp=0;
	if	(interlace==FALSE	) temp=SCREEN2;

   for (y=SVGA_Y_POS;y< 80 ;y++)
   screen_set_line( ((y+400)*SVGA_logical_length)+temp,640,0);
}
/*------------------------------------------------------------------------------------------------------------------------------*/
void	draw_ball_type_selecter(void)
{
	long	a;

/* load in choose box bgd graphic type */

	char	buff[768];
	memcpy(&buff,&palette,768); 
   
   nominate_colour=YES;
	show_panel=0;

/* setup default colour to potted colour */

	whichcol=0;

/* draw highlighted balls */
	count=5;
	sync_stage();

	while	( quitgame==NO )	
    { 
	  show_panel=10;
/* make players name flash to show turn */

      if	(mouse_buttons==LEFT_BUTTON) { whichcol=0; quitgame=YES;}
      if	(mouse_buttons==RIGHT_BUTTON) { whichcol=1; quitgame=YES;}

     choose_loop();

     Handle3D();     

     for	(a=0;a<12;a++)	serial_transfer();

/* setup colour from other computer */

     if	(LINK_OPTION !=NULL)	
      {
	    if	( (Player_Active == 0) && (master==SLAVE) )	whichcol = extern_whichcol;
	    if	( (Player_Active == 1) && (master==MASTER) )	whichcol = extern_whichcol;
		 if	(extern_quit == YES) quitgame=YES;
		}
	 }

/* stop players names flashing */
   flash_name_1=0;
   flash_name_2=0;
	old_player_turn=Player_Active;

	memcpy(&palette,&buff,768); 
	for	(a=0;a<12;a++)	serial_transfer();
	sync_stage();
	quitgame=NO;
	for	(a=0;a<12;a++)	serial_transfer();
   nominate_colour=NO;
   foul_occured=0;
}
/*------------------------------------------------------------------------------------------------------------------------------*/
void	choose_loop(void)
{
	switch	(whichcol)	{

	case	0:
		players[Player_Active].ball_type = RED;
		players[1-Player_Active].ball_type = YELLOW;
		break;

	case	1:
		players[Player_Active].ball_type = YELLOW;
		players[1-Player_Active].ball_type = RED;
		break;
	}

   p1_stored_pot_type=players[0].ball_type;
   p2_stored_pot_type=players[1].ball_type;
}
/*------------------------------------------------------------------------------------------------------------------------------*/
void draw_number_of_shots_icons(void)
{
	long	temp,a,tmp;

   if ( draw_foul_shot_icons == NO ) return;

	temp=0;
	if	(interlace==FALSE) temp=SCREEN2;

  if ( ( tmp = Two_Shots) <= 0) tmp=1;

  if ( Player_Active == 0 )
   {
    for ( a = 0 ; a < tmp ; a++)
     {
      if (RESOLUTION == LOW)
       {
        draw_gfx_block(0,1+(320*2)+21*a,shots_icon,0,0,COPY_TO_RAM );
       }
      else
       {
        draw_gfx_block(0,2+(SVGA_logical_length*16)+( 42*a )+temp,shots_icon,0,0,COPY_TO_SCREEN );
	    }
     }
   }
  else
   {
    for ( a = 0 ; a < tmp ; a++)
     {
      if (RESOLUTION == LOW)
       {
        draw_gfx_block(0,(320*2)+320-21-(21*a),shots_icon,0,0,COPY_TO_RAM );
       }
      else
       {
        draw_gfx_block(0,(640*16)+(SVGA_logical_length-42-(42*a) )+temp,shots_icon,0,0,COPY_TO_SCREEN );
	    }
     }
   }
}
/*------------------------------------------------------------------------------------------------------------------------------*/
void	draw_connection(void)
{
	if	(LINK_OPTION == NULL ) return;
	draw_gfx_block(0,320-60,connect_data,0,0,COPY_TO_RAM );
}
/*------------------------------------------------------------------------------------------------------------------------------*/
 long text_low[100];
void build_low_text(long play_pen)
{
	long	a;
	long	x,xsize;
	char	chr,save_chr;
	long	ay,ny,ys,ns,psize;
   long index;
   ubyte *txtptr;

   txtptr = txt;

   index=0;

   if (BUILD_LOW_TEXT == NO) return;

   for (chr='A' ; chr <= 'Z'+1 ; chr++)
   {
   save_chr=chr;

	x=0;

	psize=640;
	ay=58;
	ny=30;
	ns=44;
	ys=9;
 
	if	( (GAME_TYPE==SPEED_POOL) && (play_pen!=-1) )
    {
     ay=18;
     ny=43;
     ns=0;
     ys=17;
     txtptr = speedtxt;
	 }

	/* find beging of charcter */

	switch	(chr)	{

	case	'.':
		chr=27;
		break;

	case	'#': 
		chr=28;
		break;

	case ' ':
		chr=26;
		break;

	default:
		chr=chr-'A';
		break;
	}

	for	(a = 0 ;a < chr ;a++)	{

	while	(*(txtptr+x+(ay*psize)) == 0)	x++;
	x++;
	while	(*(txtptr+x+(ay*psize)) == 0)	x++;
	x++;
	}

	while	(*(txtptr+x+(ay*psize)) == 0)	x++;
	xsize=1;
	while	(*(txtptr+xsize+x+(ay*psize)) == 0)	     xsize++;

   xsize++;

   text_low[index*2] = x;
   text_low[index*2+1] = xsize;
   index++;
	chr=save_chr;
   }

   BUILD_LOW_TEXT = NO;

}

void	draw_text_low(int	start_x,int	start_y,char	*text, long play_pen)
{
	long	a;
	long	x,xp,xsize;
	char	chr;
	long	c;
	long	ay,ny,ys,ns,psize;
   ubyte *txtptr;

   txtptr = txt;

   build_low_text(play_pen);

	len=0;
	while	( *text != 0 )	{

   chr	= *text++;

	x=0;

	psize=640;
	ay=58;
	ny=30;
	ns=44;
	ys=10;


	if	( (GAME_TYPE==SPEED_POOL) && (play_pen!=-1) )
   {
	ay=18;
	ny=43;
	ns=0;
	ys=18;
   txtptr = speedtxt;
	}

	/* find beging of charcter */

	switch	(chr)	{

	case	'.':
		chr=27;
		break;

	case	'#':
		chr=28;
		break;

	case ' ':
		chr=26;
		break;

	default:
		chr=chr-'A';
		break;
	}

   x  = text_low[chr*2];   
   xsize  = text_low[chr*2+1];   

	for	(a=0;a<ys;a++)	
    {
//g     screen_andor_line1( txtptr+x+((ns+a)*psize), start_x+((start_y+a)*viewport_width), xsize);
	 }

	start_x+=xsize+1;
	len+=xsize+1;

	}
}
/*------------------------------------------------------------------------------------------------------------------------------*/
 num_low[100];
void	draw_number_low(long	num,long	xpos,long y)
{
	long	x,xp,a;
	long	ay,ny,ns,ys,psize,index;
	char	c,chr;
   ubyte *txtptr;

   txtptr = txt;
   if ( BUILD_LOW_NUMS == YES )
    {
     index = 0;

     for ( chr=0 ; chr <= 14 ;chr++)
	   {
       psize=640;
       ny=78;
       ns=64;
       ys=10;

      if	(GAME_TYPE==SPEED_POOL)	
       {
        ay=18;
        ny=43;
        ns=21;
        ys=17;
        txtptr = speedtxt;
       }

      x=0;

     for	(a = 0 ;a < chr ;a++)	
      {
       while	(*(txtptr+x+(ny*psize)) == 0)	x++;
       x++;
       while	(*(txtptr+x+(ny*psize)) == 0)	x++;
       x++;
      }

     while	(*(txtptr+x+(ny*psize)) == 0)	x++;
     xsize=1;
     while	(*(txtptr+xsize+x+(ny*psize)) == 0)	xsize++;
     xsize++;

	  num_low[index*2] = x;
	  num_low[index*2+1] = xsize;
     index++;
     }

	  BUILD_LOW_NUMS = NO;

    }


	psize=640;
	ny=78;
	ns=64;
	ys=10;

	if	(GAME_TYPE==SPEED_POOL)	
    {
	  ay=18;
	  ny=43;
     ns=21;
     ys=17;
        txtptr = speedtxt;
    }

	x = num_low[num*2];
	xsize = num_low[num*2+1];   

	for	(a=0;a<ys;a++) 
    {
//g     screen_andor_line1(txtptr+x+((a+ns)*psize),xpos+(((height-y)+a)*viewport_width),xsize);
	 }
}
/* -------------------------------------------------------------------------*/
void	draw_text_len(int	start_x,int	start_y,char	*text, long play_pen)
{
	long	a;
	long	x,xsize;
	char	chr;
	long	ay,ny,ys,ns,psize;


	play_pen=play_pen;

	len=0;

	while	( *text != 0 )	{

   chr	= *text++;

	x=0;

	psize=640;//viewport_width;

	if	(RESOLUTION==LOW)	{
	ay=58;
	ay=58;
	ny=30;
	ns=44;
	ys=9;

	if	(GAME_TYPE==SPEED_POOL)	
    {
     ay=18;
     ny=43;
     ns=21;
	  ys=17;
	 }

	}
	else	{
	ay=23;
	ny=54;
	ns=30;
	ys=21;
	}


	/* find beging of charcter */

	if	((chr >='A') || (chr ==' '))	{

	switch	(chr)	{
	case	'.':
			chr=27;
			break;

	case ' ':
		chr=26;
		break;

	default:
		chr=chr-'A';
		break;
	}

	for	(a = 0 ;a < chr ;a++)	{

	while	(*(txt+x+(ay*psize)) == 0)	x++;
	x++;
	while	(*(txt+x+(ay*psize)) == 0)	x++;
	x++;
	}

	while	(*(txt+x+(ay*psize)) == 0)	x++;
	xsize=1;
	while	(*(txt+xsize+x+(ay*psize)) == 0)	xsize++;

	start_x+=xsize+1;
	len+=xsize+1;

	}

	}

}
/* -------------------------------------------------------------------------*/
void draw_amplify(void)
{
	long	temp;
	temp=0;
	if	(interlace==FALSE) temp=SCREEN2;

  if ( Player_Active == 1)
   {
    if (RESOLUTION == LOW)
    	draw_gfx_block(0,1+320,amplify_icon,0,0,COPY_TO_RAM );
    else 	draw_gfx_block(0,2+(16*SVGA_logical_length)+temp,amplify_icon,0,0,DUMP_TO_SCREEN );
   }
  else
   {
    if (RESOLUTION == LOW)
    	draw_gfx_block(0,320-59+320,amplify_icon,0,0,COPY_TO_RAM );
    else 	draw_gfx_block(0,(16*SVGA_logical_length)+(SVGA_logical_length-120)+temp,amplify_icon,0,0,DUMP_TO_SCREEN );
   }

}
/* -------------------------------------------------------------------------*/
 long hires_nms_foul[100];
void	draw_text_foul(int	start_x,int	start_y,char	*text, long play_pen)
{
	long	a;
	long	x,xsize;
	char	chr,save_chr;
	long	ay,ys,psize,index;

   if ( BUILD_HIRES_TEXT_FOUL == YES )
    {
     index=0;
     for (chr='A' ; chr <= 'Z'+2 ;chr++)
      {
       psize=640;//viewport_width;

       ay=55;

       save_chr = chr;

       x=0;

         switch	(chr)	
          {
           case  ',':
	        chr=27;
           break;

           case ' ':
           chr=26;
           break;

           default:
           chr=chr-'A';
		     break;
	       }

       for	(a = 0 ;a < chr ;a++)	
        {
         while	(*(txt_foul+x+(ay*psize)) == 0)	x++;
         x++;
         while	(*(txt_foul+x+(ay*psize)) == 0)	x++;
         x++;
        }

       while	(*(txt_foul+x+(ay*psize)) == 0)	x++;
       xsize=1;
       while	(*(txt_foul+xsize+x+(ay*psize)) == 0)	xsize++;

       hires_nms_foul[index*2]=x;
       hires_nms_foul[index*2+1]=xsize;
       index++;
       chr=save_chr;
      } 

	  BUILD_HIRES_TEXT_FOUL = NO ;
    }


	len_foul=0;

	while	( *text != 0 )	
   {

   chr	= *text++;

	x=0;

	psize=640;//viewport_width;
	ys=19;

	/* find beging of charcter */

     switch	(chr)	
      {
 
       case  ',':
	    chr=27;
		 break;

       case ' ':
       chr=26;
       break;
	  
       default:
		 chr=chr-'A';
		 break;
	   }

     x     = hires_nms_foul[chr*2];
     xsize = hires_nms_foul[chr*2+1];

     for	(a=0;a<ys;a++)	
      {
       screen_dump_line(txt_foul+x+((34+a)*psize),start_x+((start_y+a)*viewport_width) ,xsize);
      }

      start_x+=xsize+1;
      len_foul+=xsize+1;
	 }
}
/* -------------------------------------------------------------------------*/
 long text_low_foul[100];
void build_low_text_foul(void)
{
	long	a;
	long	x,xsize;
	char	chr,save_chr;
	long	ay,ys,ns,psize;
   long index;

   index=0;

   if (BUILD_LOW_TEXT_FOUL == NO) return;

   for (chr='A' ; chr <= 'Z'+2 ; chr++)
   {
   save_chr=chr;

	x=0;

	psize=640;
	ns=30;
	ys=8;
   ay=40;

	/* find beging of charcter */

	switch	(chr)	{

   case  ',':
		chr=27;
		break;

	case	'#': 
		chr=28;
		break;

	case ' ':
		chr=26;
		break;

	default:
		chr=chr-'A';
		break;
	}

	for	(a = 0 ;a < chr ;a++)	{

	while	(*(txt_foul+x+(ay*psize)) == 0)	x++;
	x++;
	while	(*(txt_foul+x+(ay*psize)) == 0)	x++;
	x++;
	}

	while	(*(txt_foul+x+(ay*psize)) == 0)	x++;
	xsize=1;
	while	(*(txt_foul+xsize+x+(ay*psize)) == 0)	     xsize++;

   xsize++;

   text_low_foul[index*2] = x;
   text_low_foul[index*2+1] = xsize;
   index++;
	chr=save_chr;
   }

   BUILD_LOW_TEXT_FOUL = NO;

}

void	draw_text_low_foul(int	start_x,int	start_y,char	*text, long play_pen)
{
	long	a;
	long	x,xp,xsize;
	char	chr;
	long	c;
	long	ys,ns,psize;

   build_low_text_foul();
	len_foul=0;
	while	( *text != 0 )	{

   chr	= *text++;

	x=0;

	psize=640;
	ns=30;
	ys=8;

	/* find beging of charcter */

	switch	(chr)	{

   case  ',':
		chr=27;
		break;

	case	'#':
		chr=28;
		break;

	case ' ':
		chr=26;
		break;

	default:
		chr=chr-'A';
		break;
	}

   x  = text_low_foul[chr*2];   
   xsize  = text_low_foul[chr*2+1];   

	for	(a=0;a<ys;a++)	{
//g    screen_andor_line1(txt_foul+x+((ns+a)*psize),start_x+((start_y+a)*viewport_width),xsize);
	}

	start_x+=xsize+1;
	len_foul+=xsize+1;

	}
}
/* -------------------------------------------------------------------------*/
 text* foul_messages[] = {

    "THREE FOULS BALL IN HAND",              /* 00 */
    "HIT BLACK FIRST, BALL IN HAND",			/*	01	*/
    "PAUSED", 											/*	02	*/
    "BALL IN HAND ",									/*	03	*/
    "TIME FOUL BALL IN HAND",						/*	04	*/
    "HIT YELLOW FIRST, BALL IN HAND",	      /* 05 */
    "HIT RED FIRST, BALL IN HAND",	         /* 06 */ 
    "BALL NOT HIT, BALL IN HAND",				/*	07	*/
    "FOUL, POTTED BLACK , RESPOTTED",        /* 08 */
    "FOUL, RED POTTED, BALL IN HAND",	      /* 09 */
    "TWO SHOTS", 										/*	10	*/
    "SCRATCH, BALL IN HAND",	               /* 11 */
    "FOUL, POTTED SPOT",	                  /* 12 */
    "FOUL, POTTED STRIPE",                   /* 13 */
    "MISSED RAIL, BALL IN HAND",					/*	14	*/
    "FOUL YELLOW POTTED, BALL IN HAND",      /* 15 */
    "LOSES A LIFE",					            /*	16	*/
    "GAME OVER",										/*	17	*/
    "FOUL, HIT SPOT FIRST",                  /* 18 */
    "FOUL, HIT STRIPE FIRST",                /* 19 */
    "HIT NINE FIRST",	                     /* 20 */
    "POTTED BLACK, OTHER PLAYER WINS",       /* 21 */
    "POTTED BLACK, GAME OVER",               /* 22 */
    "LOWEST BALL NOT HIT, BALL IN HAND",     /* 23 */
    "FOUL, MISSED RAIL",					      /* 24 */
    "FOUL, BALL NOT HIT ",        				/*	25	*/
    "FOUL, BALL NOT HIT, LOSE A LIFE",			/*	26	*/
    "REPLAYING LAST SHOT",                   /* 27 */
    "REPLAYING SAVED SHOT",                  /* 28 */
    "DOUBLE POT, CHOOSE BALL SET",           /* 29 */
    "SCRATCH, DOUBLE POT, CHOOSE BALL SET",  /* 30 */
    "FOUL, HIT COLOURED BALL FIRST",         /* 31 */
    "FOUL, POTTED COLOURED BALL WRONG",      /* 32 */
    "NOMINATE POCKET,AND NOMINATE BALL",     /* 33 */
    "FOUL, HIT BLACK FIRST",         			/*	34	*/

/* in game events */

    "SHOOT MODE",                            /*	35	*/
    "AIM MODE",                              /*	36	*/
    "MOVE CAMERA MODE", 							/*	37	*/
    "AMPLIFY SHOOT MODE",							/*	38	*/
    "CAMERA ZOOM MODE",								/*	39	*/
    "APPLY ENGLISH MODE",							/*	40	*/
    "RAISE BUTT OF CUE MODE",						/*	41	*/
    "BALL IN HAND MODE",                     /*	42	*/

};

long cleared_foul;

void draw_last_foul(void)
{
 text string[80];
 long	temp,y;

 if ( foul_occured != oldfoul) 
  {
    if (RESOLUTION == HIGH) 
     {
      if (cleared_foul >= 2 ) 
       {
        cleared_foul=-1;
        oldfoul=foul_occured;
       }
      else
       {
        clear_replay_panel_count=1;
        cleared_foul++;
        temp=0;
        if	(interlace==FALSE) temp=SCREEN2;

        for (y=0;y<32;y++)
         {
          screen_set_line(64+((480-32+y)*SVGA_logical_length)+temp,640-128,0);
         }
	   }
    }
  }

 cleared_foul=0;
 if ( foul_occured==0 ) return;

 temp=0;
 if	(interlace==FALSE) temp=SCR2;

 if ( foul_occured == LOSE_LIFE_FOUL )
   {
/* check for player with no name */

	if	( strlen(&player_names[killer_order[last_killer_turn]]) <= 1)
     {
      sprintf(string,"%s %s","MAN WITH NO NAME",
	   foul_messages[ foul_occured ]);
	  }
    else
     {
/* store player's name with lose a life text */
	   sprintf(string,"%s %s",&player_names[killer_order[last_killer_turn]],
	   foul_messages[ foul_occured ]);
	  }
   }
 else  strcpy(&string , foul_messages[ foul_occured ]);

  if ( RESOLUTION==LOW )
   {
    y = 10;
   }
  else 
   {
    y = 32;
	 if ( GAME_TYPE == SPEED_POOL) y=48;
	}

/* if showing saved shot then move message up */

 if	(show_saved_shot_flag==YES) y*=2;

 if ( RESOLUTION == LOW )
   {
    draw_text_low_foul(0,201,&string,0);

	 if ( GAME_TYPE != SPEED_POOL)
     {
      draw_text_low_foul( (160-((len_foul/2))) ,(200-y)+panel_scrolly,&string,0);
     }
    else
     {
      draw_text_low_foul( (120-((len_foul/2))) ,(200-y)+panel_scrolly,&string,0);
     }
   }
  else
   {
    draw_text_foul( 0 , 490,&string,0);
    draw_text_foul( (320-((len_foul/2))),(480-y)+temp,&string,0);
   }

}
