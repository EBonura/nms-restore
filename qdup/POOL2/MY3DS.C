/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/*									    	 QD-UP      										 */
/*											PC-CD ROM											 */
/*									 For BMG	Interactive										 */
/*									 By NMS Software Ltd.									 */
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#include	"globals.h"
#include	"my3ds.h"
//#include "dll.h"
//#include	"ail.h"

//#define  SAVE_A_DEMO

/* -------------------------------------------------------------------------*/
/* re-draw 3d after trashing it for a menu */
void	Re_Draw_Screen( void )
{
	char	file[80];

	fadeout();
   draw_twice=2;

	if	(RESOLUTION==LOW)	
    {
/* setup 320x200 screen mode for lores game play */
	  wait_for_VBE_vblank(0);
     setvgamode(0x13);
	  fix_palette_to_black();
    }
   else 
    {
     setup_SVGA_screen();
    }

/* clear all video ram out */

	HI_CLS();

/* load in game palette */

   sprintf( file, "%s//%s",drive_and_path,room_palettes[LOCATION+(TABLE_TYPE*NUMBER_OF_LOCATIONS)] );
	load_file_here(file, &palette, 768 );

/* fix pen 0 to black */

	palette[0]=0;
	palette[1]=0;
	palette[2]=0;

	num_frames_camera_still = 0;

   mouse_xdiff=0;
   mouse_ydiff=0;
   mouse_buttons=0;

   setup_felt_palette();

/* draw scene twice so that it is tidy on both screens */

   num_frames_camera_still = 0;

   name_frame_diff = frames;
//g	Render();
//g	Render();

/* clear mouse controls */

   update_mouse_xy();
   mouse_xdiff=0;
   mouse_ydiff=0;
   mouse_buttons=0;

	fadein();
}
/* ------------------------------------------------------------------------ */
void setup_felt_palette(void)
{
/* setup felt palettes */

   switch	(TABLE_TYPE)
    {
     case	BLUE_BAIZE:
/* blue baize */
     palette[(3*2)+2]-=2;
     palette[(3*3)+2]-=3;
     break;

     case	RED_BAIZE:
/* red baize */
     palette[3*2]-=2;
     palette[3*3]-=3;
     break;

     case	GREEN_BAIZE:
/* green baize */
     palette[3*2+1]-=1;
     palette[3*3+1]+=1;
     break;
    }

/* change colour of carpet */

   switch (LOCATION)
    { 
	 case POTTERS_CLUB:  
     palette[3*5]-=1;
     palette[3*6]+=1;
     palette[3*5+2]-=1;
     palette[3*6+2]+=1;
    break;

	 case RACKERS:  

#if 0
     palette[3*5]-=2;
     palette[3*6]+=1;
     palette[3*5+2]-=1;
     palette[3*6+2]+=2;
#endif
    break;


	 case ARENA:  
     palette[3*5+1]-=1;
     palette[3*6+1]+=1;
     palette[3*5+2]-=1;
     palette[3*6+2]+=1;
     break;

	 case POOL_HALL:  
     palette[3*5+2]-=1;
     palette[3*6+2]+=1;
     break;
   }
}
/* ------------------------------------------------------------------------ */
void	Handle3D(void )
{ 	
	long	save_f,y;
   char filename[80];

/* save a screen to disk */

//g	if (getkey(k0)==YES)
//g     {
//g      bak_screen = backscreen_buffer;
//g	   for(y=0;y<480;y++) screen_copy_line(bak_screen+y*640,y*SVGA_logical_length,640);
//g    	sprintf(filename,"PICT%04d.LBM",get_highest()+1);
//g		save_iff(filename,bak_screen,palette,640,480);
//g	  }

/* ------------------------------------------------ */

	check_all_camera_positions();

	scan_for_keysjp();

/* check to see if any cheat mode was entered */

   scan_for_cheat_modes();

/* play backgroun sound effects */

	background_samples();

/* ------------------------------------------------ */

//g	loop_cd_music_track( cd_track );

   if ( COMPUTER_TURN==NO ) 	update_mouse_xy();

/* update world objects according to frame rate */

//  turn_off_pocket_cameras();

  if	( (E_FinishedEase()==YES) ) 
   {
    check_all_game_events();

  if (DEMO_PLAY==NO)
  {

#ifdef SAVE_A_DEMO
  *(demo_buffer+demo_pos)    =(word) mouse_xdiff;
  *(demo_buffer+demo_pos+4)  =(word) mouse_ydiff;
  *(demo_buffer+demo_pos+8)  =(word) mouse_buttons;
  *(demo_buffer+demo_pos+12) =(word) event_flag;
  demo_pos+=16;

  if (getkey(k1)==YES)  saveafile( "demo1.DAT",demo_buffer, demo_pos*4);
  if (getkey(k2)==YES)  saveafile( "demo2.DAT",demo_buffer, demo_pos*4);
#endif
  }
  else
   {
    mouse_xdiff       =(int) *(demo_buffer+demo_pos);
    mouse_ydiff       =(int) *(demo_buffer+demo_pos+4);
    event_flag        =(word) *(demo_buffer+demo_pos+12); 
    demo_pos+=16;
   }

    if ( (COMPUTER_TURN==NO) && (QUIT_GAME_FLAG==NO) )   
     {
      DoCamera();
      Update_Cue_Position();
      Update_English_On_Cue_Ball();
     }

    if ( (check_hit_cue_ball()== YES) ) return;

	}

/* render scene to screen */

	Render();

/* check pause game */

	save_f=frames;
	pause_game();
	frames=save_f;

/* ------------------------------------------------ */

	if	(LINK_OPTION!=NULL)	
    {
     serial_transfer();

     if	(extern_event[0] == MOVING_BALL_IN_HAND)	
      {
       if	( (Player_Active == 0) && (master==SLAVE) )	
        {
         ball[ball_to_move].x=(long)(extern_cue_ball[0]*0x1000000);	// RESTORE: ball 8.24; the link data stay doubles
         ball[ball_to_move].y=(long)(extern_cue_ball[1]*0x1000000);
        }

       if	( (Player_Active == 1) && (master==MASTER) )	
        {
         ball[ball_to_move].x=(long)(extern_cue_ball[0]*0x1000000);	// RESTORE: ball 8.24; the link data stay doubles
         ball[ball_to_move].y=(long)(extern_cue_ball[1]*0x1000000);
        }
      }
	 }
}
/* ------------------------------------------------------------------------ */
void	Render(void)
{

/* call steves 3d */

   if ( (panel_flag==YES) && (RESOLUTION==LOW) )  refresh_whole_screen=YES;

// RESTORE: the 3D view shows the table in 3D through the PS1 renderer
// written for the restoration (escher/, not NMS's; NMS's PS1 build has no
// 3D view, ENGINE.C stubs Escher's render calls and DrawScreen's 2D view is
// what it shows).  DrawScreen2D starts the frame (next page, cleared
// ordering table, the debug text, the 2D view when L2 has turned it on),
// draw_3d renders into the same ordering table, then the frame is shown.
	if(view_3d == YES)
		{
		DrawScreen2D();
		E_SetOrderingTable(cdl->ot, OT_SIZE);
		draw_3d();
		display_screen();
		}
	else
		{
		DrawScreen();	// the 2D view: draws and shows the frame itself
		draw_3d();
		}
	SetupPanel();
	if ( nominate_colour == YES)
    {
     choose_loop();
	 }

	if	(RESOLUTION==HIGH)
    {
     interlace+=1;
     interlace%=2;
     setup_SVGA_screen();
    }
	else
    {
     if(view_3d == NO) wait_for_VBE_vblank(0);	// RESTORE: the 2D view waits as the 2D build did; the 3D view has shown its frame already (below)
// RESTORE: 3D view: the PC waited for the VGA retrace here to copy its back
// buffer to the screen (the memcpy below); on the PS1 that wait is a second
// VSync each frame, and the 3D build has synced and shown its frame already
// (display_screen above), so it leaves the wait out.
//g     memcpy( (char *) 0xa0000, backscreen_buffer, 64000);
    }
}
/* ------------------------------------------------------------------------ */
void	show_render(void)
{
	wait_for_VBE_vblank(0);
	draw_3d();
	SetupPanel();

//	dump_x320y240(viewport_screen_buffer);
}
/* ------------------------------------------------------------------------ */
void identity4x4(matrix4x4 matx)
{
    int x,y;

    for (x=0;x<4;x++) {

        for (y=0;y<4;y++) {
            if (y==x) matx[x][y]=1;
            else matx[x][y]=0;
        }
      }
    
    return;
}
/* -------------------------------------------------------------------------*/
void multiply4x4(matrix4x4 matx1, matrix4x4 matx2, matrix4x4 result)
{
    matrix4x4 tempresult;
    int x,y;

    for (y=0;y<4;y++) {
        for (x=0;x<4;x++) {
            tempresult[x][y]=matx1[0][y]*matx2[x][0]+matx1[1][y]*matx2[x][1]+
                        matx1[2][y]*matx2[x][2]+matx1[3][y]*matx2[x][3];
        }
    }

/*  Copy Temp matrix into final matrix */

    for (y=0;y<4;y++) {
        for (x=0;x<4;x++) {
            result[x][y]=tempresult[x][y];
        }
    }

    return;
}
/* -------------------------------------------------------------------------*/
void multiply4x1(matrix4x4 matx1, matrix4x1 matx2, matrix4x1 result)
{
    matrix4x1 tempresult;
    int y;

    for (y=0;y<4;y++) {
            tempresult[y]=matx1[0][y]*matx2[0]+matx1[1][y]*matx2[1]+
                        matx1[2][y]*matx2[2]+matx1[3][y]*matx2[3];
    }
/*  Copy Temp matrix into final matrix */
    for (y=0;y<4;y++) {
            result[y]=tempresult[y];
    }

    return;
}
/* -------------------------------------------------------------------------*/
void rotate(double roll, double pitch, double yaw, matrix4x4 spin)
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
    mp[1][1] = mp[2][2] = cp;
    mp[1][2] = - (mp[2][1] = sp);
                                /* prepare yaw matrix */
    my[0][0] = my[2][2] = cy;
    my[0][2] = - (my[2][0] = -sy);

    multiply4x4( &mr, &my, spin );
    multiply4x4( spin, &mp, spin );
}
/* -------------------------------------------- */
void  scan_for_cheat_modes(void)
{
  char c;
  long a;
  int str_len;

#define MAX_CHEAT 50

	str_len=strlen(&cheat_buffer);

   if ( str_len > 20) 
    {
     cheat_pos=0;
	  for (a=0;a< MAX_CHEAT;a++) cheat_buffer[a]=0;
    }

	c=find_which_key_input();
 
//g   if ( c!='@') add_key( &cheat_buffer , c);

  if (getkey_jp(kENTER)==YES)
   {
    cheat_pos=0;
    for (a=0;a< MAX_CHEAT ;a++) cheat_buffer[a]=1;
	}

//   printf("%s\n",cheat_buffer);

/* check cheat codes */

   switch ( GAME_TYPE )
    {

	  case SPEED_POOL:

     if ( strcmp( "BATMAN FOREVER" , &cheat_buffer ) == 0 )
      {
		 SPEED_POOL_CHEAT_LINES=YES;
       lines_on=YES;
	    for (a=0;a< MAX_CHEAT;a++) cheat_buffer[a]=1;
      }

     break;

    }
}
/* -------------------------------------------- */
void	background_samples(void)
{
static long	background_status;

	switch	(background_status)
	{

     case	0:
		add_sample(COUGH1, 0);	
		break;

	  case	100:
		add_sample(FART_SOUND, 0);	
		break;

     case	662:
		add_sample(COUGH1, 0);	
		break;

     case	1449:
		add_sample(COUGH2, 0);	
		break;

     case	934:
		add_sample(COUGH2, 0);	
		break;
	 }

	background_status++;
	background_status%=1850;
}

