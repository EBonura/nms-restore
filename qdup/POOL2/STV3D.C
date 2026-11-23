/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/*									    	 QD-UP      										 */
/*											PC-CD ROM											 */
/*									 For BMG	Interactive										 */
/*									 By NMS Software Ltd.									 */
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* ------------------------------ */
/*   ball colour order */
/* ------------------------------ */
// spot ball coloures //
//  1 - Yellow
//  2 - Blue
//  3 - Red
//  4 - Purple
//  5 - Orange
//  6 - Green
//  7 - Brown
//  8 - Black

// striped ball colours //

//  9 - Yellow
// 10 - Blue
// 11 - Red
// 12 - Purple
// 13 - Orange
// 15 - Green
// 16 - Brown
/* ------------------------------ */
#include	"globals.h"
//#include	"uvbelib.h"
//#include "svga.h"
#include	"resapi.h"

#define  LOWEST_CAMERA_ANGLE 30

//#define NO_TABLE
//#define CHECK_CAMERA_POSITION

HRESOURCE	resfile;
long num_lines_drawn;
flag newballs=NO;
void RenderBasicScene(int toscr );

void Fast_Haaaa_DrawAllBalls(int toscr);
void Draw_Camera_View_Point( char *buffer_s );
void turn_on_diamonds(long loop);
void draw_tracking_lines( char *buffer );
void Draw_Head_String( char *backscreen);
char VerticesSame(VERTEX *v1, VERTEX *v2);
int  pixel_aspect;
OBJECT *cue_spot;

flag  hires_numbers=YES;
flag  used_hires=NO;
flag  oldnumber_res=-1;
static  flag old_view,changed_view;

VERTEX uk_table_camera_in_pockets[] = {
       { 0 , 1527, 0 } ,
       { -193118 , 6489, 95917 } ,

       { 0 , 574, 0 } ,
       { -192026 , 6489, -95301 } ,

       { 0 , 4075 , 0 } ,
       { -349 , 6489, -97769 } ,

       { 0 , 3613 , 0 } ,
       { 193369 , 6489, -95226 } ,

       { 0 , 2566 , 0 } ,
       { 192758 , 6489, 95162 } ,

       { 0 , 2024 , 0 } ,
       { 157 , 6489, 97742 } ,
};

/* ------------------------------ */
/* UK table type camera positions */
VERTEX special_camera_positions[] = {
		{1024, 2046, 0},			/* over head view */
		{-829, 7296,155659},
		
		{145,1128,0},			  	/* bottom left corner view */
		{-713524,388503,367928},

		{145,920,0},			  	/* bottom right corner view */
		{-713524,388503,-367928},
		
		{145,3218,0},				/* top left corner view */
		{821450,388503,367928},

		{145,2285,0},				/* top right corner view */
		{821450,388503,-367928},

		{385,1022,0},			   /* default view position for table */
		{-155280, 6489 , -1663},

		{255,1025,0},			   /* start position for camera */
		{-217600,7296,0}
};
/* ------------------------------ */
/* us table type camera positions */

VERTEX special_us_camera_positions[] = {
		{1024, 2046, 0},			/* over head view */
		{-829, 7296,155659},
		
		{145,1128,0},			  	/* bottom left corner view */
		{-713524,388503,367928},

		{145,920,0},			  	/* bottom right corner view */
		{-713524,388503,-367928},
		
		{145,3218,0},				/* top left corner view */
		{821450,388503,367928},

		{145,2285,0},				/* top right corner view */
		{821450,388503,-367928},

		{313,1023,0},			   /* default view position for table */
		{-298994, 7296 , -398},

		{255,1025,0},			   /* start position for camera */
		{-217600,7296,0}
};
	
#define	OVER_HEAD_CAMERA  2
#define	CORNER1_CAMERA    3
#define	CORNER2_CAMERA	   4
#define	CORNER3_CAMERA    5
#define	CORNER4_CAMERA    6
#define	STARTUP_CAMERA    7
#define	LINE_UP_SHOT      8
#define  MOVE_CAMERA_VIEWPOINT 200

char   quit;
char   *poolptr;

OBJECT *dummy_pointer;
OBJECT *dummy_pointer1;
OBJECT *bumper_ptr[6];
OBJECT *diamond_ptr[18];
OBJECT *sidebumper_ptr[6];
OBJECT *sidebumperpock_ptr[6];
OBJECT *look_at_ptr, *cue_ball_ptr, *cue_ptr, *ball_ptr1, *stripe_ptr[22] ;
OBJECT *ball_numbers[22] , *cross_ptr;
OBJECT *line1,*line2;
OBJECT *line3,*line4;
OBJECT *corners,*surface;
OBJECT *uscorners[6];

VERTEX save_look_at;

int    temp2, divider, zoom;
int	 bumper_near[6];
VERTEX bumper_pos[6];
int    x_frac,y_frac,z_frac;
int    linesort;

flag zoom_out(int ball_num);
flag move_camera_point_flag;
// --------------------------------------------------------------------------
void CollideCueWithBalls(void)
{
#define AngleRangeConstant 7500000

// As far as I know, the cue is 1060mm long

#define CueLength 1060

int i, ball_angle, dx, dz, angle_range, delta_angle, raise_angle;
int cueangle;
int distance; 
long last_butt_tmp = last_butt;
long reach;		// RESTORE: 3D view, fixed point (below)

  cueangle = (cue_angle + 2048) & 4095;
  highest_raise_angle = 0;
  reach = CueLength * 256 + FIX24_TO_WORLD(dbl_to_fix24(cue_position)) / 2;	// RESTORE: (CueLength + cue_position*500) * 256

  for (i = 1; i < LAST_BALL; i++)
  {

    if (ball[i].active > 0)
    {
      // Find radian angle of ball

      dx = - (ball_ptr[i]->modified_pos.x - ball_ptr[0]->modified_pos.x);
      dz = (ball_ptr[i]->modified_pos.z   - ball_ptr[0]->modified_pos.z);

// RESTORE: the 3D view runs this every frame the cue butt is raised, so it
// is in fixed point there: NMS's angle is atan2(dx, dz) (its three cases
// below), here from fix_atan2 (65536 a turn) in Escher units; the distance
// from fix_sqrt on 1/128 world units.
      ball_angle = (-(fix_atan2(dx, dz) >> 4)) & 4095;
      distance = (fix_sqrt((dx>>7)*(dx>>7) + (dz>>7)*(dz>>7)) >> 12) << 7;

		if ( (distance < reach) && (distance>0) )
		{

  		  angle_range = AngleRangeConstant / distance;
		  
		  // is cue colliding with ball
		  
		  delta_angle = (cueangle - ball_angle) & 4095;
		  
		  if (delta_angle >= 2048)
		  {
          delta_angle -= 4096;
		  }
		  
		  if ((delta_angle >= - angle_range) && (delta_angle <= angle_range))
		  {
		  
		    // we have a collision. Now convert delta angle to fraction across
          // collision range with 11 bits fraction (-2048..2047)
		  
		    delta_angle = (delta_angle << 10) / angle_range;
          delta_angle &= 4095;
		  
		    // using this value, get the sin which will determine how much to
          // raise the cue. Use our constant to adjust the raise angle
		  
		    raise_angle = E_Cos(  delta_angle);
          raise_angle = (raise_angle * angle_range) >> 16;
		  
		    // convert to degrees
		  
		//    raise_angle = (raise_angle * 90) >> 10;
		  
		    // update highest so far
		  
		    if (raise_angle > last_butt_tmp)
          {
            highest_raise_angle = raise_angle;
           last_butt_tmp = highest_raise_angle;
		    }
		  }
		}
	 }
  }

  if ( last_butt_tmp > highest_raise_angle)  return;

//   if ( highest_raise_angle-5 < 0) return;

   butt = highest_raise_angle;

  refresh_whole_screen=2;
}
// --------------------------------------------------------------------------
void GetBallPtrs(void)
{
int i,x,col,count1;

char *ball_nums = "0";
char *ball_nums1 = "10";
char *stripe_name = "stripe0X";
char *spot_name = "spot0X";
char *pock_name = "pocketX";
OBJECT *tmp1;
OBJECT *tmp;
static int    save_colour[22];
 long loop;

/* colour outer box to black */

   tmp1=E_GetObjectPtr( "outerbox" );
 
   if (tmp1)
    {
     col = poolptr[2560 + 0];

     for (x=0;x < tmp1 -> num_planes ;x++)
      {
       tmp1 -> planes[x].colour = col * 0x01010101;
      }
     tmp1 -> priority=96 << 24;
	 }

    for (i = 1; i < 10; i++)
     {
      ball_nums[0] = ( i % 10 )+48;
      ball_numbers[i] = E_GetObjectPtr( ball_nums );

      if (ball_numbers[i])
       {
        col = poolptr[2560 + 0];
        for (x=0;x < ball_numbers[i] -> num_planes ;x++)
         {
          ball_numbers[i] -> planes[x].colour = col * 0x01010101;
         }
       }
     } 

    for (i = 10; i < 16; i++)
     {
      ball_nums1[1] = ( i % 10 )+48;
      ball_numbers[i] = E_GetObjectPtr( ball_nums1 );
      if (ball_numbers[i])
       {
        col = poolptr[2560 + 0];
        for (x=0; x < ball_numbers[i] -> num_planes ;x++)
        ball_numbers[i] -> planes[x].colour = col * 0x01010101;
       }
     } 

/* get cue spot pointer */

  ball_ptr[0] = E_GetObjectPtr("cuespot");
  cue_spot = ball_ptr[0];

  if (cue_spot)
   {
    col = poolptr[2560 + 16];
    for (x=0; x < ball_ptr[0] -> num_planes ;x++)
     {
      cue_spot -> planes[x].colour = col * 0x01010101;
     }
   }

/* get spot ball pointers */

  for (i = 1; i < 22; i++)
   {
    spot_name[5] = ( (i-1) % 10 )+48;
    spot_name[4] = ( (i-1) / 10 )+48;
    ball_ptr[i] = E_GetObjectPtr( spot_name);
   } 

/* get stripe pointers */

  for (i = 9; i < 16; i++)
   {
    stripe_name[7] = (i-9 )+48;

    stripe_ptr[i] = E_GetObjectPtr( stripe_name);

    if (stripe_ptr[i])
    {
/* setup stripe colours */

    switch	( i-8 )
     {
      case	 BLACK:
      col=0;
      break;

      case	YELLOWBALL:
      col=7*16;
      break;

      case	PURPLE:
      col=5*16;
      break;

      case	BROWN:
      col=6*16;
      break;

      case	BLUEBALL:
      col=16;
      break;

      case	ORANGE:
      col=3*16;
      break;

      case	GREEN:	
      col=2*16;
      break;

      case	WHITE:
      col=128;
      break;

      case	STRIPE:
      col=128;
      break;			  

      case	RED:
      col=4*16;
      break;
     }

    col = poolptr[2560 + col];

    for (x=0;x < stripe_ptr[i] -> num_planes ;x++)
    stripe_ptr[i] -> planes[x].colour = col * 0x01010101;
	}
  }

  ball_ptr1 = E_GetObjectPtr( "dummy4" );
  cue_ball_ptr = ball_ptr[0];

  cross_ptr = E_GetObjectPtr( "cross" );

  surface = E_GetObjectPtr( "surface" );
  if (surface) surface -> priority = 15 << 24;

  for ( count1=1;count1<7;count1++)
   {
    pock_name[6] = ( count1 % 10 )+48;
    tmp = E_GetObjectPtr( pock_name );
    if (tmp) tmp -> priority = 16 << 24;
   }
  

/* turn off ball number textures */

     col = poolptr[2560 + (15)];

  for (count1 = 1 ; count1 < 22; count1++)
   {
    if (ball_ptr[count1])
     {
      for (count=0;count < ball_ptr[ count1 ] -> num_planes; count++)
       {
        ball_ptr[count1] -> planes[count].plane_type = plane_plain;
        ball_ptr[count1] -> planes[count].colour = col * 0x01010101;
       }
     }
   }

#if 0
   if ( (hires_numbers == NO) && (used_hires==YES) && (oldnumber_res!=hires_numbers))
    {
     oldnumber_res=hires_numbers;
	  for (count1 = 1 ; count1 < 22; count1++)
      {
       if (ball_ptr[count1])
		 {
        for (count=0;count < ball_ptr[count1] -> num_planes; count++)
         {
          ball_ptr[count1] -> planes[count].plane_type = plane_texture;
          ball_ptr[count1] -> planes[count].colour = save_colour[count1];
	      }
	    }
	   }
    }
#endif
}
// --------------------------------------------------------------------------
OBJECT *corner_ptr[6];
OBJECT *d_ptr[9];

void GetBumperPtrs(void)
{
int i;
char *diamond_name = "diamondXX";
char *ball_name = "bumperX";
char *ball_name1 = "sideX";
OBJECT *tmp;
char *name = "ldummyX";


/* draw 'D' on snooker table */

   for (i=1;i<10;i++)
    {
     name[6] = ( i % 10 )+48;
     if (d_ptr) d_ptr[i-1] = E_GetObjectPtr(name);
	 }

  corners = E_GetObjectPtr( "corners" );
  if (corners) corners -> priority = 15 << 24;

 if	( TABLESIZE_TYPE == UK) {

  for (i = 1; i < 7; i++)
  {
    ball_name[6] = 48 + i ;
    ball_name1[4] = 48 + i ;

    bumper_ptr[i-1] = E_GetObjectPtr( ball_name);

    sidebumper_ptr[i-1] = E_GetObjectPtr( ball_name1);
	}
  }

/* setup us bumper side set */
  else 
   {

/* get pointers to diamonds */

  for (i = 1; i < 19; i++)
   {
    if (i > 9) 
     {
      diamond_name[7] = 48 + (i / 10 ) ;
      diamond_name[8] = 48 + (i % 10 ) ;
     }
    else
     {
      diamond_name[7] = 48 + (i % 10 ) ;
      diamond_name[8] = NULL;
	  }

    diamond_ptr[i-1] = E_GetObjectPtr(diamond_name);
    if (diamond_ptr[i-1] ) diamond_ptr[i-1] -> priority = 4 << 24;
	}

    for (i = 1; i < 7; i++)
     {
      ball_name[6] = 48 + i ;
      bumper_ptr[i-1] = E_GetObjectPtr( ball_name );
     }
      sidebumper_ptr[0] = E_GetObjectPtr( "endleft" );
      sidebumper_ptr[1] = E_GetObjectPtr( "endbottom" );
      sidebumper_ptr[2] = E_GetObjectPtr( "endtop" );
      sidebumper_ptr[3] = E_GetObjectPtr( "endright" );

     }

/* swamp US bumper's around */

     tmp = bumper_ptr[5];
     bumper_ptr[5] = bumper_ptr[2];
     bumper_ptr[2] = tmp;


   sidebumperpock_ptr[0] = E_GetObjectPtr( "newcorner1" );
   sidebumperpock_ptr[1] = E_GetObjectPtr( "newcorner2" );
   sidebumperpock_ptr[2] = E_GetObjectPtr( "newcorner3" );
   sidebumperpock_ptr[3] = E_GetObjectPtr( "newcorner4" );


  for (i=0;i<6;i++) 
   {
    if (bumper_ptr[i]) bumper_ptr[i]-> priority = 14 << 24;
    if (sidebumper_ptr[i]) sidebumper_ptr[i] -> priority = 13 << 24;
    if (sidebumperpock_ptr[i]) sidebumperpock_ptr[i] -> priority = 11 << 24;
	}

/* get pointers to corner pockets */

  if ( TABLESIZE_TYPE == US )
   {
    uscorners[0] = E_GetObjectPtr( "corner1" );
    if (uscorners[0]) uscorners[0] -> priority = 15 << 24;
    uscorners[1] = E_GetObjectPtr( "corner2" );
    if (uscorners[1]) uscorners[1] -> priority = 15 << 24;
    uscorners[2] = E_GetObjectPtr( "corner3" );
    if (uscorners[2]) uscorners[2] -> priority = 15 << 24;
    uscorners[3] = E_GetObjectPtr( "corner4" );
    if (uscorners[3]) uscorners[3] -> priority = 15 << 24;
    uscorners[4] = E_GetObjectPtr( "corner5" );
    if (uscorners[4]) uscorners[4] -> priority = 15 << 24;
    uscorners[5] = E_GetObjectPtr( "corner6" );
    if (uscorners[5]) uscorners[5] -> priority = 15 << 24;
   }

    tmp = E_GetObjectPtr( "side" );
    if (tmp) tmp -> priority = 8 << 24;

}
// ----------------------------------------------------------------------------
void Load( char *filename, void *address, int length)
{
	PCDISKFP;

	if (( fp = open_file_to_read( filename )) == PCERRORFP ) {
//   printf( "Unable to open file %s\n",filename );
   return ;
	}

//	filelength = get_file_length( fp );

	read_block_from_disk( address, length, fp );
	close_the_read_file( fp );

//  printf( filename);

/*
int file1;
unsigned int length_read;

  if (_dos_open( filename, O_RDONLY, &file1 ) != 0 ) {
    printf( "Unable to open file %s\n",filename );
  } else {
    _dos_read( file1, (void *) address, length, &length_read);
    _dos_close( file1 );
  }

  */
}
// ----------------------------------------------------------------------------
void setup_camera_in_pocket(long pocket)
{
/* check to see if a ball is abount to be pocketed, if so then place camera
 in pocket */

     camera_in_pocket = YES;

     camera_rot = uk_table_camera_in_pockets[pocket*2];
     camera_target = uk_table_camera_in_pockets[pocket*2+1];
     camz = 115;

     E_EaseCamera(ease_speed/2);
     E_PositionCamera( &camera_target, &camera_rot, camz*256 );

}
// ----------------------------------------------------------------------------
void turn_off_pocket_cameras(void)
{
 if (show_shot_flag==YES) return; 

/*
	 camera_pos = save_camera_pos;
	 camera_rot = save_camera_rot;
    camz =  save_camz;

    E_EaseCamera(ease_speed);
    E_PositionCamera( &camera_target, &camera_rot, camz*256 );
*/
    camera_in_pocket = NO;
}
// ----------------------------------------------------------------------------
   int   max_zoom,bad;
#define NEAREST_CAMZ 250
void DoCamera(void)
{
  long ball_num,speed_delay;
int x_delta,y_delta,tmp;
long	temp;

/* ------------------------ */

#define	MAX_NEW_ZOOM	6000 
#define	MAX_NEW_ZOOM1	7000  // when room turned off

   if (detail_level == LOW) max_zoom=MAX_NEW_ZOOM1;
     else max_zoom=MAX_NEW_ZOOM;

   if (LOCATION > DIFFERENT_SHAPED_TABLES_INDEX-1) max_zoom = 1000 * 20;

   if ( SNOOKER_MODE == YES ) max_zoom = 1000 * 7;

/* ---------------------- */
/* test to see if camera out of range if so,
 bring camera back to a good level */

   if ( (camz > max_zoom) && (camera_mode==ZOOM_BACK_TO_CUE_BALL) )
    {
     camz = max_zoom;
     E_EaseCamera(ease_speed);
     E_PositionCamera( &camera_target, &camera_rot, camz*256 );
     return;
    }

   if ( (show_shot_flag==YES) && (zoom_flag==YES) )
    {

/* check to see if cue ball is off screen, if so then zoom camera out until
 ball is visable */

     ball_num=0;
     if ( (zoom_out(ball_num)==TRUE) ) return;

/* check to see if nine ball off screen, if so then zoom camera out until
 ball is visable */

     if ( GAME_TYPE == US_POOL) 
      {
       ball_num = Select_Ball_To_Pot();
       if ( (zoom_out(ball_num)==TRUE) ) return;
		}
	 }


/* ------------------------ */
/* dont allow player to move camera for a while, after taking shot */

   if ( delay_after_shot > 0) 
    {
     speed_delay = frames - save_frame_for_delay_after_shot;
     save_frame_for_delay_after_shot=frames;
     delay_after_shot-=speed_delay;
     return;
	 }

/* ------------------------ */
/* check to see if player moving camera view point */

   if ( event_flag==MOVE_CAMERA ) 
    {
     shoot_mode=YES;
     Move_Camera_View_Point();
	 }
   else
   {

	if	(event_flag==TAKING_SHOT)	
    {
     shoot_mode=YES;
	 }

/* ---------------------------- */
/* rotate room about the x axis */

     x_delta = mouse_xdiff ;

     if ( (abs(mouse_xdiff)) < 4)
      {
       if (mouse_xdiff > 0) x_delta =1;
       if (mouse_xdiff < 0) x_delta =-1;
      }

/* --------------------- */
     y_delta = mouse_ydiff;

     if ( (abs(mouse_ydiff)) < 8)
      {
       if (mouse_ydiff > 0)  y_delta = 2 ;
       if (mouse_ydiff < 0)  y_delta = -2 ;
      }

/* --------------------- */

     if	( (getkey(kLEFTCTRL)==YES) or (getkey(kRIGHTCTRL)==YES) )
      {
       if (mouse_xdiff > 0) x_delta =1;
       if (mouse_xdiff < 0) x_delta =-1;

       if (mouse_ydiff > 0) y_delta =1;
       if (mouse_ydiff < 0) y_delta =-1;

      }

     if	(event_flag==ROTATE_CAMERA_VIEW)	
      {

       if (FLIP_CONTROLS==NO) 
        {
           camera_rot.y-= x_delta ;
        }
       else
        {
           camera_rot.y+= x_delta ;
        }


/* ------------------------ */
/* tilt room */

     if (FLIP_CONTROLS==NO) 
      {
         camera_rot.x -= y_delta;
      }
     else
      {
         camera_rot.x += y_delta;
      }
     check_camera_limets();

    }

/* ------------------------ */
/* zoom camera position */
     temp=camz;
     if	(event_flag == CAMERA_ZOOM)	
      {

       if (FLIP_CONTROLS==NO) 
        {
  	      if	( (camz+=y_delta * 2 )  > max_zoom) 
          {
           camz=max_zoom;
          }
        }
       else
        {
  	      if	( (camz-=y_delta * 2 )  > max_zoom) 
          {
           camz=max_zoom;
          }
        }
      }

      if	( camz < NEAREST_CAMZ ) camz= NEAREST_CAMZ;

/* ------------------------ */

  }/* from check moving camera */

  if ( replay_flag == YES ) return;

  if ( shoot_mode==0 )
   {
    if ( (E_FinishedEase()==YES) ) 
     {  
      if	(camera_mode==ZOOM_BACK_TO_CUE_BALL) 
       {
        if	(COMPUTER_TURN==NO)
         {
          cue_angle=camera_rot.y;
         }
        if	(camera_mode==ZOOM_BACK_TO_CUE_BALL) 
         {
          fixed_camera_rot = camera_rot;
          fixed_camera_pos = camera_pos;
	      }
       }
      else
       {
        if	(COMPUTER_TURN==NO) 
         cue_angle=fixed_camera_rot.y;
       }
        cue_rot = camera_rot;
        fixed_cue_rot = camera_rot;
     }
   }
/* ------------------------ */

	if	(event_flag==MOVING_BALL_IN_HAND)	
    {
  	  MoveBall_new();
	 }

    if	(event_flag==RAISE_BUTT_OF_CUE)	
     {
      Raise_Butt_Of_Cue();
      CollideCueWithBalls();
      return;
     }

/* raise cue butt over balls in the way */
//g   CollideCueWithBalls();
}
// ---------------------------------------------------------------------------
void PutCamera(VERTEX look_at)
{
VERTEX temp_vertex;

	if	(show_shot_flag==NO)	{
	save_look_at.x = look_at.x;
	save_look_at.y = look_at.y;
	save_look_at.z = look_at.z;

	}
	else	{
	 look_at.x =save_look_at.x;
	 look_at.y =save_look_at.y;
	 look_at.z =save_look_at.z;
	}

  temp_vertex.x = 0;
  temp_vertex.y = 2000;
  temp_vertex.z = -(camz*256);

  E_RotateVertex( camera_rot.x, camera_rot.y, camera_rot.z, &temp_vertex);

  camera_pos.x = temp_vertex.x + look_at.x;
  camera_pos.y = temp_vertex.y + look_at.y;
  camera_pos.z = temp_vertex.z + look_at.z;
}
// ----------------------------------------------------------------------------
flag zoom_out(int ball_num)
{
#define DISTANCE_TO_ZOOM_OUT_IF_BALL_OFF_SCREEN 3000

VERTEX ball_info;
   int bad,height;

/* if camera allready moving then exit */

	if	( (E_FinishedEase()==NO) ) {
    return FALSE;
   }

/* if camera zoomed out at maximum distance then exit */

   if (camz >= max_zoom) return FALSE;

/* check all conditions of ball zoom */

   if (mouse_buttons == LEFT_BUTTON)   return FALSE;
   if (ball[ball_num].active == FALSE) return FALSE;
   if (ball[ball_num].moving == FALSE) return FALSE;

/* get screen 2d position of current ball */

   bad = E_GetScreenPos( ball_ptr[ball_num], &ball_info );

/* setup height of screen in current resolution */

   if (RESOLUTION==HIGH) height = 480-(CLIPPED*2);
   else height = 200;

/* check to see if ball is off screen */

// RESTORE: 3D view: the same tests in fixed point, run every frame of a
// shot: pixels (8.8) against twice the diameter in metres (8.24 >> 16), the
// units NMS compared.
   if ( (bad!=0) || (((ball_info.x>>4)<<8) < ((ball[0].diameter*2)>>16)) ||
        (((ball_info.x>>4)<<8) > (viewport_width<<8)-((ball[0].diameter*2)>>16)) ||
        (((ball_info.y>>4)<<8) < ((ball[0].diameter*2)>>16)) ||
        (((ball_info.y>>4)<<8) > (height<<8)-((ball[0].diameter*2)>>16)) )
    {

/* add distance to camera z position */

     camz+=DISTANCE_TO_ZOOM_OUT_IF_BALL_OFF_SCREEN;

/* check max range */

     if (camz > max_zoom) camz = max_zoom;

/* set speed of ease */

     E_EaseCamera(6);

/* update new camera position */

     E_PositionCamera( &camera_target, &camera_rot, camz*256 );
     return TRUE;
    }

    return FALSE;;
}
// ----------------------------------------------------------------------------
void ReplaceExtension(char *filename, char *extension)
{
int i, z;

  i = 0;

  while(1)
  {
    if (filename[i] == 0)
    {
      break;
    }
    if (filename[i] == '.')
    {
      for (z = 0; z < 3; z++)
      {
        filename[i+1+z] = extension[z];
      }
      break;
    }
    i++;
  }

}
// ----------------------------------------------------------------------------
// This routine searches the current directory for files txtbnk*.raw and loads
// them into texture memory

void LoadTextures( text *path)
{
char *filename = "txtbnkXX.raw";
int i;
char *load_address;
	char	file[80];
	
  load_address = textures+(65536*4);

  for (i = 4; i < 31; i++)
  {
    filename[6] = 48 + (i / 10);
    filename[7] = 48 + (i % 10);
    sprintf( file, "%s//%s//%s",drive_and_path,path,filename );

//    reply = _dos_findfirst( file , _A_SUBDIR, &fileinfo);

  //  if (reply == 0)
  //  {
      if (file) Load( file, load_address, 65536);
  //  }

    load_address += 65536;
  }
}
void LoadBallTextures( text *path)
{
char *filename = "txtbnkXX.raw";
int i;
char *load_address;
	char	file[80];
	
  load_address = textures;

  for (i = 0; i < 4; i++)
  {
    filename[6] = 48 + (i / 10);
    filename[7] = 48 + (i % 10);
    sprintf( file, "%s//%s//%s",drive_and_path,path,filename );

//    reply = _dos_findfirst( file , _A_SUBDIR, &fileinfo);

  //  if (reply == 0)
  //  {
      if (file) Load( file, load_address, 65536);
  //  }

    load_address += 65536;
  }
}
// ----------------------------------------------------------------------------
void PositionCue(void)
{
 	long	cuebutt,tmp;

VERTEX temp_vertex;

   if	( (camera_mode!=ZOOM_BACK_TO_CUE_BALL) && ( replay_flag == NO) )
    {
     PositionCue1();
     return;
	 }

  // Make vector pointing back along z axis

  temp_vertex.x = x_hit_pos/12;
  temp_vertex.y = -(y_hit_pos/12);
  temp_vertex.z = -FIX24_TO_WORLD(dbl_to_fix24(cue_position)/2 + FIX24(.016));	// RESTORE: 3D view, fixed point (every frame)

  // Rotate this by camera's rotation

  if ( (tmp = (1024-(butt+32))) > 1024) tmp=1024;
  cuebutt=1024 - tmp;

  E_RotateVertex( cuebutt, cue_rot.y, cue_rot.z, &temp_vertex);

  // Add this vector onto position of cue ball

  temp_vertex.x += cue_ball_ptr->modified_pos.x;
  temp_vertex.y += cue_ball_ptr->modified_pos.y;
  temp_vertex.z += cue_ball_ptr->modified_pos.z;

  // And move the cue to this position and rotate

  E_MoveObject( temp_vertex.x, temp_vertex.y, temp_vertex.z, cue_ptr);
  E_RotateObject( cuebutt, cue_rot.y, cue_rot.z, cue_ptr);

  E_RotateObject( ((cuebutt)&4095)-(y_hit_pos/100), cue_rot.y-(x_hit_pos/100), cue_rot.z, cue_spot );
}
// ----------------------------------------------------------------------------
void PositionCue1(void)
{
 	long	cuebutt,tmp;

VERTEX temp_vertex;
  // Make vector pointing back along z axis


  temp_vertex.x = x_hit_pos/12;
  temp_vertex.y = -(y_hit_pos/12);
  temp_vertex.z = -FIX24_TO_WORLD(dbl_to_fix24(cue_position)/2 + FIX24(.016));	// RESTORE: 3D view, fixed point (every frame)

  // Rotate this by camera's rotation

  if ( (tmp = (1024-(butt+32))) > 1024) tmp=1024;

  cuebutt=1024 - tmp;

  E_RotateVertex( cuebutt, fixed_camera_rot.y, fixed_camera_rot.z, &temp_vertex);

  // Add this vector onto position of cue ball

  temp_vertex.x += cue_ball_ptr->modified_pos.x;
  temp_vertex.y += cue_ball_ptr->modified_pos.y;
  temp_vertex.z += cue_ball_ptr->modified_pos.z;

  // And move the cue to this position and rotate

  E_MoveObject( temp_vertex.x, temp_vertex.y, temp_vertex.z, cue_ptr);

  E_RotateObject( cuebutt, fixed_camera_rot.y, fixed_camera_rot.z, cue_ptr);

  E_RotateObject( ((cuebutt)&4095)-(y_hit_pos/100), fixed_cue_rot.y-(x_hit_pos/100), fixed_cue_rot.z, cue_spot );

}
// ----------------------------------------------------------------------------
 VERTEX cross;
// RESTORE: was (512*1024).  E_Load (ESCHER.S) takes 539,300 bytes of this
// memory for the Potters table and the balls (measured: AllocMem's last
// block, plane_data_mem, ended 15,012 bytes past 512 KB, over balls_xy and
// the 3D build's brightness table).  Sized to what E_Load takes; the
// owner's decision.  RAM.C and STV3D.C both define it.
#define	MODEL_MEM	632196	// RESTORE: was 539300 (the US Potters table); E_Load's mem_used after loading, measured: Potters US 539,300, Potters UK 569,888, Crypt 520,852, Square 632,196 (SQUARE.ESH, 142 KB).  The largest, as the tables' choice is made at the option screen
void	init_3d(void)
{
	long	t1,t2,t3,t4,t5,t6,t7,t8;
	long	room;
	long	loop,tmp;
	text	file[80];

   changed_view=NO;

/* point backscreen uffer at front end ram */

   oldnumber_res = 255;
   ball_to_move=0;
	ease_speed=8;

/* get pointer to level */

   if ( SNOOKER_MODE == NO) tmp=TABLESIZE_TYPE;
   else tmp=2;

	if ( LOCATION <=DIFFERENT_SHAPED_TABLES_INDEX-1) room=(LOCATION*15)+(tmp*5);
   else room = 15*DIFFERENT_SHAPED_TABLES_INDEX + (LOCATION-DIFFERENT_SHAPED_TABLES_INDEX)*5;

	DrawSync(0);	// RESTORE: 3D view: the 3D renderer's primitives live in this memory
   memset(steves_memory,0,MODEL_MEM);
	E_Init( steves_memory, MODEL_MEM);

/* load in mesh */

//g   sprintf( file, "%s//%s",drive_and_path,mesh_table[room] );
   sprintf( file, "%s",mesh_table[room] ); //g
   E_AddMesh( file );

   if ( SNOOKER_MODE == YES )
    {
//g     sprintf( file, "%s//%s//%s//%s",drive_and_path, "balls",mesh_table[room+4],"ballscsn.esh" );
	sprintf( file,"%s\\ballscsn.esh",mesh_table[room+4] );
    }
   else
   {
//     sprintf( file, "%s//%s//%s//%s",drive_and_path, "balls",mesh_table[room+4],"ballscue.esh" );
	sprintf( file,"%s\\ballscue.esh",mesh_table[room+4] );
   }

   E_AddMesh( file );

	E_Load();

	E_LoadMapData();

/* load in brightness table */

//g   sprintf( file, "%s//%s",drive_and_path,mesh_table[room+2] );
//g	load_file_here(file,brightness_table,0);
// RESTORE: 3D view: the PC version loaded the brightness table here (the //g
// lines were left out of the PS1 port, which drew no 3D); the PS1 3D
// renderer written for the restoration colours planes with it.
   sprintf( file, "%s",mesh_table[room+2] );
	load_file_here(file,brightness_table,8192);
   E_UseBrightnessTable(brightness_table);

/* load textures for scene */

//g   load_3d_textures();
   poolptr = E_PoolTabPtr();

/*	in taken shot then turn cue off*/

   E_Reset2DLines();

/* turn on or off felt+floor textures */

   if (LOCATION <= DIFFERENT_SHAPED_TABLES_INDEX-1) E_EnableFelt( ON );
    else E_EnableFelt( 0 );

   load_characters();

/* load poolpal.tab */

   if ( SNOOKER_MODE==YES)
    {
//g     sprintf( file, "%s//%s//%s",drive_and_path,mesh_table[room+4],"snkpal.tab" );
    }
   else
    {
//g     sprintf( file, "%s//%s",drive_and_path,poolpal_tab[LOCATION+(TABLE_TYPE*NUMBER_OF_LOCATIONS)] );
	 }

//g	load_file_here(file,poolptr,0);
// RESTORE: 3D view: load poolpal.tab and the room palette as the PC version
// did (Re_Draw_Screen loaded the palette); the 3D renderer needs both.
   if ( SNOOKER_MODE==NO )
    {
     sprintf( file, "tables\\%s",poolpal_tab[LOCATION+(TABLE_TYPE*NUMBER_OF_LOCATIONS)] );
	  load_file_here(file,poolptr,0);
    }
   sprintf( file, "tables\\%s",room_palettes[LOCATION+(TABLE_TYPE*NUMBER_OF_LOCATIONS)] );
	load_file_here(file,palette,768);

//g   setup_vesa();

/* load in bumper information */

//g   sprintf( file, "%s//%s",drive_and_path,mesh_table[room+3] );

   sprintf( file, "%s",mesh_table[room+3] );
	init_cushions(file);

/* make all dummy objects invisable */

	cue_ptr = E_GetObjectPtr("cue");
	if (cue_ptr) cue_ptr -> priority = 1 << 24 ;
// RESTORE: 3D view: BALLSCUE.EMP (May 1996) is older than BALLSCUE.ESH
// (July) and colours the cue's planes with running debug numbers.  The cue's
// colours are in NMS's files all the same: POTTRUS3.ESH (9 May 1996) and
// SQUARE.ESH (29 Apr) each hold a "cue" object with the same counts as
// BALLSCUE.ESH's (59 vertices, 56 lines, 28 planes) and byte-identical LINE
// records, so its planes map one to one; POTTRUS3.EMP and SQUARE.EMP give
// the same colour for each of the 28 planes (brightness 31 in all, as in
// BALLSCUE.EMP).  Caveat: POTTRUS3.EMP predates POTTBLUE.PAL (27 May), so
// the palette entries are NMS's numbers read in the palette that came later.
// Completion: replaces an earlier choice of ours (one brown, 232, everywhere).
	if (cue_ptr && cue_ptr -> num_planes == 28)
	{
		static const unsigned char cue_colour[28] = {
			239,239,239,239,239,239,197,196,197,20,204,21,90,90,
			90,90,90,90,232,17,239,239,239,18,239,239,91,91 };
		for (loop=0;loop < 28 ;loop++) cue_ptr -> planes[loop].colour = cue_colour[loop] * 0x01010101;
	}

	E_SelectObject("cue");
	E_MakeMobileCompound();
	E_SelectObject("spot*");
	E_MakeMobileCompound();
	E_SelectObject("cuespot*");
	E_MakeMobileCompound();

	E_SelectObject("stripe*");
	E_MakeMobileCompound();

	E_SelectObject("1");
	E_MakeMobileCompound();
	E_SelectObject("2");
	E_MakeMobileCompound();
	E_SelectObject("3");
	E_MakeMobileCompound();
	E_SelectObject("4");
	E_MakeMobileCompound();
	E_SelectObject("5");
	E_MakeMobileCompound();
	E_SelectObject("6");
	E_MakeMobileCompound();
	E_SelectObject("7");
	E_MakeMobileCompound();
	E_SelectObject("8");
	E_MakeMobileCompound();
	E_SelectObject("9");
	E_MakeMobileCompound();
	E_SelectObject("10");
	E_MakeMobileCompound();
	E_SelectObject("11");
	E_MakeMobileCompound();
	E_SelectObject("12");
	E_MakeMobileCompound();
	E_SelectObject("13");
	E_MakeMobileCompound();
	E_SelectObject("14");
	E_MakeMobileCompound();
	E_SelectObject("15");
	E_MakeMobileCompound();

/* first two dummy objects are used for head string */

	E_SelectObject("dummy1");
	E_MakeDummy();
	E_SelectObject("dummy2");
	E_MakeDummy();

/* second two dummy pointers are used for tracking lines */

	E_SelectObject("dummy3");
	E_MakeMobile();
	E_SelectObject("dummy4");
	E_MakeMobile();
	E_SelectObject("dummy5");
 	E_MakeMobile();
	E_SelectObject("dummy6");
	E_MakeMobile();


	E_SelectObject("ldummy1");
	E_MakeDummy();
	E_SelectObject("ldummy2");
	E_MakeDummy();
	E_SelectObject("ldummy3");
	E_MakeDummy();
	E_SelectObject("ldummy4");
	E_MakeDummy();
	E_SelectObject("ldummy5");
	E_MakeDummy();
	E_SelectObject("ldummy6");
	E_MakeDummy();
	E_SelectObject("ldummy7");
	E_MakeDummy();
	E_SelectObject("ldummy8");
	E_MakeDummy();
	E_SelectObject("ldummy9");
	E_MakeDummy();

   GetBallPtrs();

//	if ( (LOCATION <= DIFFERENT_SHAPED_TABLES_INDEX-1) || (LOCATION==9) )
//    {
     GetBumperPtrs();
     GetBumperExtents();
// 	 }

   E_SelectObject("*");
   E_EnableObjects( 0);


   if ( SNOOKER_MODE == YES )
    {
     for	(loop=0;loop<LAST_BALL;loop++)
      {
       if ( ball_ptr[loop] )
        {
         ball_ptr[loop] -> radius=6720;
         E_ScaleObject( ball_ptr[loop], 73534);
        }
      }
    }
	else
    {

	switch	( TABLESIZE_TYPE )
   {

/* setup us ball size */

   case US:

	for	(loop=0;loop<16;loop++)
    {
     if ( ball_ptr[loop] !=NULL )
	  {
      ball_ptr[loop] -> radius=7296;

/* scale mesh */

      E_ScaleObject( ball_ptr[loop], 73534);

      if ( loop > 8)
        {
         if (stripe_ptr[loop]) E_ScaleObject( stripe_ptr[loop], 73534);
        }
      if ( loop > 0)
       {
        if (ball_numbers[loop]) E_ScaleObject( ball_numbers[loop], 73534);
       }
     }
    }
    break;

/* uk table */

   case UK:

	 for	(loop=0;loop<16;loop++)
     {
      if ( ball_ptr[loop] )
       {
        ball_ptr[loop] -> radius=6502;

/* scale mesh */

        E_ScaleObject( ball_ptr[loop], 65536);
        if ( loop > 8)
         {
          if (stripe_ptr[loop]) E_ScaleObject( stripe_ptr[loop], 65536);
         }
        if ( loop > 0)
         { 
          if (ball_numbers[loop]) E_ScaleObject( ball_numbers[loop], 65536);
         }
       }
      }
     break;
   }
   }

   setup_resolution();

/* setup positions for head strings */

   dummy_pointer=E_GetObjectPtr("dummy1");
   dummy_pointer1=E_GetObjectPtr("dummy2");

/* setup head string 3d positions */

   if (LOCATION > DIFFERENT_SHAPED_TABLES_INDEX-1)
    {
     switch ( LOCATION )
      {
		 case 4: //moon
       dummy_pointer -> modified_pos.x = -FIX24_TO_WORLD(max_cush_x4-FIX24(0.2));	// RESTORE: 8.24
       dummy_pointer1 -> modified_pos.x =-FIX24_TO_WORLD(max_cush_x4-FIX24(0.2));	// RESTORE: 8.24
       dummy_pointer -> modified_pos.z = FIX24_TO_WORLD(min_cush_y3-FIX24(0.3));	// RESTORE: 8.24
       dummy_pointer1 -> modified_pos.z = FIX24_TO_WORLD(max_cush_y2+FIX24(0.3));	// RESTORE: 8.24
       break;
		 case 5: //ice
       break;
		 case 6: //beach
       dummy_pointer -> modified_pos.x = -FIX24_TO_WORLD(max_cush_x4-FIX24(0.2));	// RESTORE: 8.24
       dummy_pointer1 -> modified_pos.x =-FIX24_TO_WORLD(max_cush_x4-FIX24(0.2));	// RESTORE: 8.24
       dummy_pointer -> modified_pos.z = FIX24_TO_WORLD(min_cush_y3-FIX24(0.4));	// RESTORE: 8.24
       dummy_pointer1 -> modified_pos.z = FIX24_TO_WORLD(max_cush_y2+FIX24(0.4));	// RESTORE: 8.24
       break;
		 case 7: //crypt
       dummy_pointer -> modified_pos.x = -FIX24_TO_WORLD(max_cush_x4-FIX24(0.4));	// RESTORE: 8.24
       dummy_pointer1 -> modified_pos.x =-FIX24_TO_WORLD(max_cush_x4-FIX24(0.4));	// RESTORE: 8.24
       dummy_pointer -> modified_pos.z = FIX24_TO_WORLD(min_cush_y3-FIX24(0.3));	// RESTORE: 8.24
       dummy_pointer1 -> modified_pos.z = FIX24_TO_WORLD(max_cush_y2+FIX24(0.3));	// RESTORE: 8.24
       break;
		 case 8: //swamp
       dummy_pointer -> modified_pos.x = -FIX24_TO_WORLD(max_cush_x4-FIX24(0.1));	// RESTORE: 8.24
       dummy_pointer1 -> modified_pos.x =-FIX24_TO_WORLD(max_cush_x4-FIX24(0.1));	// RESTORE: 8.24
       dummy_pointer -> modified_pos.z = (-1.0)*256000;
       dummy_pointer1 -> modified_pos.z =FIX24_TO_WORLD(max_cush_y2+FIX24(0.3));	// RESTORE: 8.24
       break;
		 case 9: //toy
       dummy_pointer -> modified_pos.x = -FIX24_TO_WORLD(max_cush_x4-FIX24(0.2));	// RESTORE: 8.24
       dummy_pointer1 -> modified_pos.x =-FIX24_TO_WORLD(max_cush_x4-FIX24(0.2));	// RESTORE: 8.24
       dummy_pointer -> modified_pos.z = FIX24_TO_WORLD(min_cush_y3-FIX24(0.1));	// RESTORE: 8.24
       dummy_pointer1 -> modified_pos.z = FIX24_TO_WORLD(max_cush_y2+FIX24(0.1));	// RESTORE: 8.24
       break;
      }
    }
   else
    {
     if ( SNOOKER_MODE == NO )
      {
	    switch ( TABLESIZE_TYPE ) 
        {
         case US:
         dummy_pointer -> modified_pos.x = -FIX24_TO_WORLD(max_cush_x4-FIX24(0.58));	// RESTORE: 8.24
	 	   dummy_pointer1 -> modified_pos.x = -FIX24_TO_WORLD(max_cush_x4-FIX24(0.58));	// RESTORE: 8.24
         dummy_pointer -> modified_pos.z = FIX24_TO_WORLD(min_cush_y3-FIX24(0.1));	// RESTORE: 8.24
         dummy_pointer1 -> modified_pos.z = FIX24_TO_WORLD(max_cush_y2+FIX24(0.1));	// RESTORE: 8.24
         break;

         case UK:
         dummy_pointer -> modified_pos.x = -FIX24_TO_WORLD(max_cush_x4-FIX24(0.2));	// RESTORE: 8.24
         dummy_pointer1 -> modified_pos.x =-FIX24_TO_WORLD(max_cush_x4-FIX24(0.2));	// RESTORE: 8.24
         dummy_pointer -> modified_pos.z = FIX24_TO_WORLD(min_cush_y3-FIX24(0.1));	// RESTORE: 8.24
         dummy_pointer1 -> modified_pos.z = FIX24_TO_WORLD(max_cush_y2+FIX24(0.1));	// RESTORE: 8.24
         break;
        }
      }
    }

/* set priority of lines in scene */

   linesort = 11 << 24;

   setup_default_camera_position(0,NO);

   line1 = E_GetObjectPtr( "dummy3" );
   line2 = E_GetObjectPtr( "dummy4" );

   line3 = E_GetObjectPtr( "dummy5" );
   line4 = E_GetObjectPtr( "dummy6" );

   quit = 0;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void setup_resolution(void)
{

   if ( end_of_frame_flag == NO )
    {
	  zoom=928+200;
    }
   else
    {
	  zoom=750;
    }

	if	(RESOLUTION==LOW)   
		{

		E_SetScreenSize(320,200);
      E_SetScanlineLength(320);
		E_SetScreenCentre(160,145);
		E_SetPixelAspect(54613);
      pixel_aspect  = 54613;

		E_SetZoom( (zoom*65536)/320 );
	   E_SetZoneBank( 0);
		}

	if	(RESOLUTION==HIGH)
		{
		E_SetScreenSize(640,480-(CLIPPED*2));
      E_SetScanlineLength(SVGA_logical_length);
		E_SetScreenCentre(320,276);
		E_SetPixelAspect( 65536 );
      pixel_aspect  = 65536;
		E_SetZoom((zoom*65536)/320);
		}
}
// ----------------------------------------------------------------------------
char VerticesSame(VERTEX *v1, VERTEX *v2)
{
  if ((v1->x == v2->x) && (v1->y == v2->y) && (v1->z == v2->z))
  {
    return( 1);
  } else {
    return( 0);
  }
}
// ----------------------------------------------------------------------------
flag test_camera_still(void)
{
VERTEX camera_rot_tmp = camera_rot;
VERTEX camera_pos_tmp = camera_pos;
flag temp = NO;
static flag first_move;

   E_GetCameraPos(&camera_pos_tmp);
   E_GetCameraRot(&camera_rot_tmp);
	if ( (VerticesSame( &camera_pos_tmp, &old_camera_pos)) && (VerticesSame( &camera_rot_tmp, &old_camera_rot)))
	{
	if (first_move == NO) temp = YES;
   first_move = YES;
	} else {
	first_move = NO;
   temp = NO;
	}
	return temp;   
}
// ----------------------------------------------------------------------------
flag cue_behind_cueball;
void	draw_3d(void)
{
int	a,loop,y,pheight;

  long frame_count;
	long	temp;

 camera_rot.z=0;
 camera_rot_tmp = camera_rot;
 camera_pos_tmp = camera_pos;

/* reset counter for number of lines drawn */

//  E_SetDetail( 0 );

  num_lines_drawn=0;

  if ( keys[k7]==128)   E_EnableFelt( 0 );
  if ( keys[k8]==128)   E_EnableFelt( 1 );

#ifdef CHECK_CAMERA_POSITION

  if	(getkey(kSPACE)==YES)	{
	at(0,0);
	printf("%d , %d , %d \n",camera_rot.x,camera_rot.y,camera_rot.z);
	printf("%d , %d , %d \n",camera_target.x,camera_target.y,camera_target.z);
	printf("%d \n",camz);
  }

   if ( getkey_jp(kF7)==YES)
    {
	  newballs^=1;
    }

#endif

/* don't check cue priority in demo play mode */

   if ( (cue_ptr -> world_pos.z > ball_ptr[WHITE_BALL] -> world_pos.z) && (DEMO_PLAY==NO) )
    {
     cue_behind_cueball=YES;
	 }
   else
    {
     cue_behind_cueball=NO;
	 }

/* update all 3d ball positions for rendering */

	UpdateBallPositions();

	if (RESOLUTION==HIGH)
   {
    E_SetZoneBank( interlace );
	}

/* make camera look at cue ball if lining up shot */

   if ( (E_FinishedEase() == YES) )
    {

     E_EaseCamera(0);

     E_PositionCamera( &camera_target, &camera_rot, camz*256 );

	  if ( shoot_mode==2 ) shoot_mode=NO;

/* line up cue with next ball to pot , only in nine ball game type */

     if ( (lined_up_shot==NO) && ( GAME_TYPE==US_POOL) && (COMPUTER_TURN==NO) && (camera_mode == ZOOM_BACK_TO_CUE_BALL) && (SNOOKER_MODE==NO) )
      {
       Move_Camera_Position(LINE_UP_SHOT);
       lined_up_shot=YES;
		}
     else
      {

/* turn cue on once the camera has finished easing, after lining up shot */

       if ( (show_shot_flag==NO ) && ( nominate_colour==NO) ) 
        {
         draw_cue=YES;
         shoot_mode_availble = YES;
        }
       else 
        {
         shoot_mode_availble = NO;
         draw_cue=NO;
		  }

/* turn off blue aiming icon if cue is not drawn */

       if ( draw_cue==NO  ) cue_spot -> visible=NO;
       else  cue_spot -> visible=YES;

/* turn cue off if camera at lowest point */

       if (camera_rot.x <= LOWEST_CAMERA_ANGLE)  draw_cue=NO;
	   }
    }

 
   E_UpdateCamera();
	PositionCue();

/* get actual positons of camera from E_Ease routine */

   E_GetCameraPos(&camera_pos_tmp);
   E_GetCameraRot(&camera_rot_tmp);

   setup_detail_level_of_room();

	if ( (VerticesSame( &camera_pos_tmp, &old_camera_pos)) && (VerticesSame( &camera_rot_tmp, &old_camera_rot)))
	{
     num_frames_camera_still++;
	} else {
     num_frames_camera_still = 0;
     reset_tracking_lines();
	}

   tracking_lines_loop();

   if ( activated_lines == TRUE ) 
    {
     num_frames_camera_still = 0;
#if 0
     if  (RESOLUTION == LOW )
      {
       if	( panel_scrolly == 25 )
        {
         E_SelectObject("*");
         E_EnableObjects(0);
	     }
	   } 
     else
      {
       E_SelectObject("*");
       E_EnableObjects(0);
	   }
#endif
	 }

	old_camera_pos = camera_pos_tmp;
	old_camera_rot = camera_rot_tmp;

/* turn off objects in room according to detail level */

   if ( SNOOKER_MODE == NO )
    {
     for	(loop=0;loop<16;loop++)	ball_ptr[loop] -> enabled=0;
//g	  for	(loop=8;loop<16;loop++)	stripe_ptr[loop] -> enabled=0;
	  for	(loop=9;loop<16;loop++)	stripe_ptr[loop] -> enabled=0;
	 }
   else
    {
     for	(loop=0;loop<LAST_BALL;loop++)	ball_ptr[loop] -> enabled=0;
    }

//   if (LOCATION <= DIFFERENT_SHAPED_TABLES_INDEX-1)
//    {
     CalcBumperVisibility();
     for (a=0;a<6;a++) if(bumper_ptr[a]) bumper_ptr[a] -> enabled =0;
     for (a=0;a<6;a++) if(sidebumper_ptr[a]) sidebumper_ptr[a] -> enabled =0;
//	 }

// RESTORE: 3D view: the PC kept a clean copy of the room to draw only the
// balls over it while the camera was still (the branch below); the PS1
// rebuilds its ordering table every frame, so the whole scene is drawn
// every frame.
	num_frames_camera_still = 0;
	if	(num_frames_camera_still < 2)	{
	num_frames_still=0;

	turn_on_far_bumpers();

/* turn cue off */

	cue_ptr -> enabled =NO;

/* render all objects in room */

	if (RESOLUTION==HIGH)
   {

/* ------------------------ */
/* test to see if player is moving
 camera view position off cue ball */

     Draw_Camera_View_Point( backscreen_buffer ); 

     if (activated_lines == TRUE)
      draw_tracking_lines(backscreen_buffer);

     Draw_Head_String( backscreen_buffer );

     E_RenderToScreen(1,0);

     E_Reset2DLines();

	} else {


/* ------------------------ */
/* test to see if player is moving
 camera view position off cue ball */

     Draw_Camera_View_Point( backscreen_buffer ); 

     if (activated_lines == TRUE)
      draw_tracking_lines(backscreen_buffer);

     Draw_Head_String( backscreen_buffer );

     E_RenderToBuffer( backscreen_buffer, 1 , 0);

     E_Reset2DLines();
	}


#if 0
	if (RESOLUTION==HIGH)
    {
     E_RenderToScreen(1 , 0);
	 }
   else 
    {
     E_RenderToBuffer( backscreen_buffer, 1 , 0);
    }

/* de-select all bumpers */

   E_SelectObject("*");
	E_EnableObjects(0);
#endif

   E_SelectObject("*");
	E_EnableObjects(0);

/* render balls to screen + stripes+spots */

   if ( cue_behind_cueball==YES )
    {
     if	( (draw_cue==NO) || (shot_taken==YES) )	cue_ptr -> enabled =NO;
	  else	
      {
       cue_ptr -> enabled =1;
      } 
    if (RESOLUTION==HIGH)
     {
      E_RenderToScreen(1 , 0);
	  }
    else 
     {
      E_RenderToBuffer( backscreen_buffer, 1 , 0);
     }

     cue_ptr -> enabled =NO;

    }

   if ( newballs == YES)
    {
     Fast_Haaaa_DrawAllBalls(1);
    }
   else
    {
     DrawAllBalls(1);
    }

/* render bumpers correctly */

   E_SelectObject("*");
	E_EnableObjects(0);

	turn_on_near_bumpers();
   turn_on_diamonds(0);

   if ( SNOOKER_MODE == YES )
    {
     E_SelectObject("side");
     E_EnableObjects( 1);
	 }

   if ( cue_behind_cueball==NO )
    {
     if	( (draw_cue==NO) || (shot_taken==YES) )	cue_ptr -> enabled =NO;
	  else	
      {
       cue_ptr -> enabled =1;
      } 
    }

	if (RESOLUTION==HIGH)
    {
     E_RenderToScreen(0 , 0);
	 }
   else 
    {
     E_RenderToBuffer( backscreen_buffer, 0 , 0);
	 }

	}

/* ------------------------------------ */
/* render just balls onto static screen */

	else	{
	// We must make sure that all the various buffers contain the right images
   // so the first couple of still frames are spent rendering here and there

   E_Reset2DLines();

	if (RESOLUTION==HIGH)
	{
	  // In hires, we render the clean scene to both video buffers and the
     // 2nd backcreen buffer

     if (num_frames_still <2)
     {

/* de-select all bumpers */

       turn_on_far_bumpers();

       /* render all objects in room */

       if (num_frames_still == 0)
       {

         Draw_Head_String( backscreen_buffer1 );


/* ------------------------ */
/* test to see if player is moving
 camera view position off cue ball */

         Draw_Camera_View_Point( backscreen_buffer1 ); 

         RenderBasicScene(0);
         E_Reset2DLines();
       }

       frame_count=frames;

       Draw_Head_String( backscreen_buffer1 );

       RenderBasicScene(1 );
       E_Reset2DLines();

		 frames=frame_count;

       E_SelectObject("*");
	    E_EnableObjects(0);

     }
	} else {


     // In low res we render to both backscreens
     if (num_frames_still == 0)
	  {

/* de-select all bumpers */

       Draw_Head_String( backscreen_buffer1 );

       Draw_Camera_View_Point( backscreen_buffer1 ); 

		 RenderBasicScene(0);
       E_Reset2DLines();

       E_SelectObject("*");
	    E_EnableObjects(0);

	    turn_on_far_bumpers();


       frame_count=frames;
       E_RenderToBuffer( backscreen_buffer1, 0 , 0);

       E_SelectObject("*");
	    E_EnableObjects(0);

		 memcpy( backscreen_buffer, backscreen_buffer1, 64000);

//       cue_ptr -> enabled =0;
		 frames=frame_count;
	  }
	}

	if (RESOLUTION==HIGH)
	{
     if	(num_frames_still >=2 )
     {
       E_CopyRamToVga(backscreen_buffer1);
     }

	} else {

     if (num_frames_still > 0)
     {
       // In low res, we clean up the primary backscreen using the previous frame's
       // zones to copy from the clean buffer into the main buffer

	  	 E_CopyRamToRam( backscreen_buffer1, backscreen_buffer);

   	if	( (panel_scrolly > 0) && (panel_scrolly < 26) )
		 {
 	       for	(y=0;y<panel_scrolly;y++)
		    memcpy(backscreen_buffer+((y+(200-panel_scrolly))*320),backscreen_buffer1+((y+(200-panel_scrolly))*320),320);
		 }

	  }
	}

   if (refresh_whole_screen>0) 
	 {

     if (RESOLUTION==LOW) 
      {

       memcpy( backscreen_buffer, backscreen_buffer1, 64000);
       refresh_whole_screen=NO;
      }
	  else 
      {
       temp=0;
       if	(interlace==FALSE) temp=SCREEN2;

        for (y=0; y < 480-(CLIPPED*2) ; y++)
        screen_dump_line(backscreen_buffer1+((y)*640),((y+SVGA_Y_POS)*SVGA_logical_length)+temp,640);

       refresh_whole_screen--;
	   }
	 }


   E_SelectObject("*");
	E_EnableObjects(0);


#ifdef NO_TABLE
   return;
#endif

/* render balls to screen + stripes+spots */

	// Render near bumpers to screen

   if ( newballs == YES )
    {
     Fast_Haaaa_DrawAllBalls(0);
    }
   else
    {
     DrawAllBalls(0);
	 }

   E_SelectObject("*");
	E_EnableObjects(0);

   cue_ptr -> enabled =0;
	turn_on_near_bumpers();
   turn_on_diamonds(0);


   if ( cue_behind_cueball==NO )
    {
     if	( (draw_cue==NO) || (shot_taken==YES) )	cue_ptr -> enabled =NO;
	  else	
      {
       cue_ptr -> enabled =1;
      } 
    }

   if ( SNOOKER_MODE == YES )
    {
     E_SelectObject("side");
     E_EnableObjects( 1);
	 }

	if (RESOLUTION==HIGH)
	{
    E_RenderToScreen(0 , 0);
	} else {
     E_RenderToBuffer( backscreen_buffer, 0 ,0);
	}

   E_Reset2DLines();

	// increment counter for num frames still
   num_frames_still++;
	}
}
// ----------------------------------------------------------------------------
void	RenderBasicScene(int	toscr)
{
	if	(toscr == 1) E_RenderToScreen(1 , 0);
	if	(toscr == 0) E_RenderToBuffer(backscreen_buffer1,1 , 0);
}
// ----------------------------------------------------------------------------
void ResetObjectRotation(OBJECT *obj)
{
int i;
VERTEX *modified, *source;

  obj->rotation.x = 0;
  obj->rotation.y = 0;
  obj->rotation.z = 0;

  modified = obj->modified_vertex;
  source   = obj->source_vertex;

  for (i = 0; i < obj->num_vertices; i++)
   {
	 *source = *modified;
    source++;
    modified++;
   }
}
// ----------------------------------------------------------------------------
void LocalRotateObject(int x_rotate, int y_rotate, int z_rotate, OBJECT *obj)
{
  ResetObjectRotation( obj);
  E_RotateObject( x_rotate, y_rotate, z_rotate, obj);
}
// ----------------------------------------------------------------------------
void	UpdateBallPositions(void)
{
int	loop,zpos;
VERTEX rotate_vertex;

  for	(loop=0;loop<LAST_BALL;loop++)	
	{

   if ( activated_lines == FALSE) 
    {
     zpos = FIX24_TO_WORLD(ball[loop].z);	// RESTORE: 8.24
     if ( ball[loop].active==FALSE ) zpos=1000*256000;

      E_MoveObject( FIX24_TO_WORLD(ball[loop].x), zpos, FIX24_TO_WORLD(ball[loop].y), ball_ptr[loop] );	// RESTORE: 8.24

     if ( SNOOKER_MODE == NO)
      {
       if (loop> 8) E_MoveObject( FIX24_TO_WORLD(ball[loop].x), zpos, FIX24_TO_WORLD(ball[loop].y), stripe_ptr[loop] );	// RESTORE: 8.24
       if (loop> 0) E_MoveObject( FIX24_TO_WORLD(ball[loop].x), zpos, FIX24_TO_WORLD(ball[loop].y), ball_numbers[loop] );	// RESTORE: 8.24
      }
	 }
	 else
     {
      zpos = FIX24_TO_WORLD(saveball_lines[loop].z);	// RESTORE: 8.24
      if ( saveball_lines[loop].active==FALSE ) zpos=1000*256000;
      E_MoveObject( FIX24_TO_WORLD(saveball_lines[loop].x), (int)(zpos), FIX24_TO_WORLD(saveball_lines[loop].y), ball_ptr[loop] );	// RESTORE: 8.24
     }
  
    if (loop!=WHITE_BALL)
     {
     if	(activated_lines==FALSE) 
      {
		 rotate_vertex.x = -slMulFX(ball[loop].ax - ball[loop].old_ax, FIX16(4096/(2*3.141592654)))/65536;	// RESTORE: 16.16 radians to 4096 a turn
	    rotate_vertex.y =  slMulFX(ball[loop].ay - ball[loop].old_ay, FIX16(4096/(2*3.141592654)))/65536;	// RESTORE: 16.16 radians to 4096 a turn
		 rotate_vertex.z =  slMulFX(ball[loop].az - ball[loop].old_az, FIX16(4096/(2*3.141592654)))/65536;	// RESTORE: 16.16 radians to 4096 a turn
      }
	  else
      {
	    rotate_vertex.x = -slMulFX(saveball_lines[loop].ax - saveball_lines[loop].old_ax, FIX16(4096/(2*3.141592654)))/65536;	// RESTORE: 16.16 radians to 4096 a turn
	    rotate_vertex.y =  slMulFX(saveball_lines[loop].ay - saveball_lines[loop].old_ay, FIX16(4096/(2*3.141592654)))/65536;	// RESTORE: 16.16 radians to 4096 a turn
	    rotate_vertex.z =  slMulFX(saveball_lines[loop].az - saveball_lines[loop].old_az, FIX16(4096/(2*3.141592654)))/65536;	// RESTORE: 16.16 radians to 4096 a turn
      }
     rotate_vertex.x &= 4095;
	  rotate_vertex.y &= 4095;
	  rotate_vertex.z &= 4095;

	  LocalRotateObject( rotate_vertex.y, rotate_vertex.z, rotate_vertex.x, ball_ptr[loop]);
     if ( SNOOKER_MODE == NO)
      {
	    if (loop > 8) LocalRotateObject( rotate_vertex.y, rotate_vertex.z, rotate_vertex.x, stripe_ptr[loop]);
	    if (loop > 0) LocalRotateObject( rotate_vertex.y, rotate_vertex.z, rotate_vertex.x, ball_numbers[loop]);
		}

    if	(activated_lines==FALSE) 
     {
      ball[loop].old_ax=ball[loop].ax;
      ball[loop].old_ay=ball[loop].ay;
      ball[loop].old_az=ball[loop].az;
     }
    else
     {
      saveball_lines[loop].old_ax=saveball_lines[loop].ax;
      saveball_lines[loop].old_ay=saveball_lines[loop].ay;
      saveball_lines[loop].old_az=saveball_lines[loop].az;
     }
    }
   }
}
// ----------------------------------------------------------------------------
void GetBumperExtents(void)
{
int i,num_bumps;
VERTEX min, max;

  num_bumps=4;
  for (i = 0; i < num_bumps; i++)
  {

    if ( bumper_ptr[i] ) E_GetObjectExtents( bumper_ptr[i], &min, &max);

    switch( i)
    {
      case(0) : bumper_pos[i].x = max.x; bumper_pos[i].x = max.x; break;
      case(1) : bumper_pos[i].x = max.x; bumper_pos[i].z = max.z; break;
      case(2) : bumper_pos[i].x = min.x; bumper_pos[i].z = min.z; break;
      case(3) : bumper_pos[i].x = min.x; bumper_pos[i].z = min.z; break;
    }
//    printf("bumper pos = %d\n", bumper_pos[i]);
  }
}
// ----------------------------------------------------------------------------
void CalcBumperVisibility(void)
{
VERTEX	*camerapos;
int i,num_bumps;

//  if ( (LOCATION > DIFFERENT_SHAPED_TABLES_INDEX-1) && (LOCATION < 9) ) return;

  camerapos=&camera_pos;
  E_GetCameraPos(camerapos);

  num_bumps=4;


  for (i = 0; i < num_bumps; i++)
   {
	 bumper_near[i] = 1;

    switch( i)
     {
      case(0) : if (camerapos ->x > bumper_pos[0].x) bumper_near[0] = 0; break;
      case(1) : if (camerapos ->z > bumper_pos[1].z) bumper_near[1] = 0; break;
      case(2) : if (camerapos ->z < bumper_pos[2].z) bumper_near[2] = 0; break;
      case(3) : if (camerapos ->x < bumper_pos[3].x) bumper_near[3] = 0; break;
     }
   }
}
// ----------------------------------------------------------------------------
void turn_on_diamonds(long loop)
{
 int i;
 if	( TABLESIZE_TYPE == 1) return;
#ifdef NO_TABLE
   return;
#endif

/* turn on all diamonds */

  for (i=0 ; i < 18; i++) 
   {
    if (diamond_ptr[i]) diamond_ptr[i] -> enabled=1;
	}
}
// ----------------------------------------------------------------------------
void	turn_on_far_bumpers(void)
{
	int	loop,num_bumps;

#ifdef NO_TABLE
   return;
#endif

  num_bumps=4;

	for	(loop=0;loop<num_bumps;loop++)	{

		if	(bumper_near[loop]==0) {

			if (bumper_ptr[loop]) bumper_ptr[loop] -> enabled=1;


	  		if (sidebumper_ptr[loop]) sidebumper_ptr[loop] -> enabled=1;


			if	(loop==1) 
          {
           if (bumper_ptr[5]) bumper_ptr[5] -> enabled=1;
          }
	 		if	(loop==2) 
          {
           if (bumper_ptr[4]) bumper_ptr[4] -> enabled=1;
			 }

			}
	}
}
// ----------------------------------------------------------------------------
void	turn_on_near_bumpers(void)
{
	int	loop,num_bumps;

#ifdef NO_TABLE
   return;
#endif

//  if ( (LOCATION > DIFFERENT_SHAPED_TABLES_INDEX-1) && (LOCATION < 9) ) return;
    num_bumps=4;

	for	(loop=0;loop<num_bumps;loop++)	{

		if	(bumper_near[loop]==1) {

			if ( bumper_ptr[loop]) bumper_ptr[loop]  -> enabled=1;
	 	 	if ( sidebumper_ptr[loop]) sidebumper_ptr[loop]-> enabled=1;

/* turn on pockets to cover balls in pockets problem */

         switch ( loop )
          {

           case 0:
	  		  if (sidebumperpock_ptr[1-1]) sidebumperpock_ptr[1-1] -> enabled=1;
	  		  if (sidebumperpock_ptr[2-1]) sidebumperpock_ptr[2-1] -> enabled=1;
           break;

           case 1:
	  		  if (sidebumperpock_ptr[2-1]) sidebumperpock_ptr[2-1] -> enabled=1;
	  		  if (sidebumperpock_ptr[3-1]) sidebumperpock_ptr[2-1] -> enabled=1;
           break;


           case 2:
	  		  if (sidebumperpock_ptr[1-1]) sidebumperpock_ptr[1-1] -> enabled=1;
	  		  if (sidebumperpock_ptr[4-1]) sidebumperpock_ptr[4-1] -> enabled=1;
           break;

           case 3:
	  		  if (sidebumperpock_ptr[3-1]) sidebumperpock_ptr[3-1] -> enabled=1;
	  		  if (sidebumperpock_ptr[4-1]) sidebumperpock_ptr[4-1] -> enabled=1;
           break;

			 }


			if	(loop==1) 
          {
           if (bumper_ptr[5]) bumper_ptr[5] -> enabled=1;
          }
		 	if	(loop==2) 
          {
           if ( bumper_ptr[4]) bumper_ptr[4] -> enabled=1;
          }

			}

	}


}
// ----------------------------------------------------------------------------
void DrawAllBalls(int toscr)
{
int   ball_num,miss_stripes_and_spots=NO;
int   ball_active[22],col,zone_radius,count,count1;
int	squish,shadow_offset, zone_bottom_add;
VERTEX ball_info[22];


 	long	 loop;
	long object_order[22];
	long temp_pos2, sort_drop_out;
	unsigned char changed=TRUE;
	long	SHADOW_COLOUR;

#if 0
   if (getkey(kF11)==YES) hires_numbers=NO;
   if (getkey(KF12)==YES) hires_numbers=YES;
#endif



	E_ClearZones();

#ifdef NO_TABLE
   return;
#endif

/* setup shadow colour for balls */
 
	SHADOW_COLOUR=16*9;

	squish= E_Sin(camera_rot.x);

/* Now fill up order list */

	for (loop=0; loop<LAST_BALL; loop++) object_order[loop]=loop;

 	sort_drop_out=0;

/* Do bubble sort */

	while (changed) {
		changed=FALSE;

		for (loop=1; loop<LAST_BALL; loop++) {

			if (ball_ptr[object_order[loop]]->world_pos.z > ball_ptr[object_order[loop-1]]->world_pos.z) {
				temp_pos2=object_order[loop];
				object_order[loop]=object_order[loop-1];
				object_order[loop-1]=temp_pos2;
				changed=TRUE;
			}
		}
	}


    for (ball_num = 0; ball_num < LAST_BALL; ball_num++)
     {
      E_WorldRotateObject( ball_ptr[ object_order[ball_num] ] );
      ball_active[object_order[ball_num] ] = E_GetScreenPos( ball_ptr[object_order[ball_num]], &ball_info[object_order[ball_num]] );
     }

  for (ball_num = 0; ball_num < LAST_BALL; ball_num++)
  {
	 if ( ball_active[ object_order[ball_num]] == 0) 
    {
			
		zone_bottom_add = (squish * ball_info[ object_order[ball_num]].z) >> 16;

      zone_radius=(ball_info[ object_order[ball_num]].z * 18) >> 4;

      E_AddZone( (ball_info[ object_order[ball_num]].x - zone_radius) >> 4,
                 (ball_info[ object_order[ball_num]].y - zone_radius) >> 4,
                 (ball_info[ object_order[ball_num]].x + zone_radius) >> 4,
                 (ball_info[ object_order[ball_num]].y + (zone_radius + zone_bottom_add)) >> 4 );
	 }
  }

  E_MergeZones();
  E_BuildChunks();

  if (RESOLUTION==HIGH)
  {
    if (toscr == 1)
    {
		// Camera moving. We must download zones from screen so that we can
      // draw balls on top of them
      E_CopyVgaToRam( backscreen_buffer);
    } else {
		// Camera still. Copy zones from clean buffer to ball-draw buffer
      E_CopyRamToRam( backscreen_buffer1, backscreen_buffer);
    }
  } else {
    if (toscr==0)
    {
		// Camera still. Copy zones from clean buffer to get clean background
      E_CopyRamToRam( backscreen_buffer1, backscreen_buffer);
	 }
   }

/* render cue behind cue ball */

    if ( cue_behind_cueball==YES )
     {
      if	( (draw_cue==NO) || (shot_taken==YES) )	cue_ptr -> enabled =NO;
	   else	
       {
        cue_ptr -> enabled =1;
       } 

      if (RESOLUTION==HIGH)
       {
        E_RenderToScreen(0 , 0);
       } else {
        E_RenderToBuffer( backscreen_buffer, 0 ,0);
	    }

      if (toscr == 1)
       {
        E_RenderToBuffer(backscreen_buffer,0 , 0);
       } 	 
       else
       {
      E_RenderToBuffer(backscreen_buffer,0 , 0);
     }

    cue_ptr -> enabled =0;
   }

/* now render the balls */

  for (ball_num = 0; ball_num < LAST_BALL; ball_num++)
  {

/* render cue behind cue ball */
 
  if ( ball_active[ object_order[ball_num]] == 0) 
   {

    if ( ball[object_order[ball_num] ].graphic_type == STRIPE)
     {
      col=128;
	  }
    else
     {
    switch	( ball[object_order[ball_num] ].ball_colour )
     {
      case	 BLACK:
      col=0;
      break;

      case	YELLOWBALL:
      col=7*16;
      break;

      case	PURPLE:
      col=5*16;
      break;

      case	BROWN:
      col=6*16;
      break;

      case	BLUEBALL:
      col=16;
      break;

      case	ORANGE:
      col=3*16;
      break;

      case	GREEN:	
      col=2*16;
      break;

      case	WHITE:
      col=128;
      break;

      case	STRIPE:
      col=128;
      break;			  

      case	RED:
      col=4*16;
      break;
     }
     }

    shadow_offset = (E_Cos(camera_rot.x)
						 *ball_info[ object_order[ball_num]].z ) >> 16;

    if (RESOLUTION==LOW)
     {
      shadow_offset = (shadow_offset * 54613) >> 16;
	  }

      E_DrawBall( ball_info[ object_order[ball_num]].x >> 1, 
                  ((ball_info[ object_order[ball_num]].y+shadow_offset) >> 1)+6 ,
                  ((ball_info[ object_order[ball_num]].z)*3) >> 3,
                  backscreen_buffer,
						SHADOW_COLOUR ,
						squish , 0
						);

      E_DrawBall( ball_info[ object_order[ball_num]].x >> 1, 
                  (ball_info[ object_order[ball_num]].y >> 1)+6,
                  ball_info[ object_order[ball_num]].z >> 1,
                  backscreen_buffer,
						col ,
						65536 , 1
						);

/* check to see if spots and stripes needed */

   miss_stripes_and_spots=NO;


   if ( SNOOKER_MODE == YES)
    {

/* turn on aiming spot */

     if  ( object_order[ball_num]==0 ) 
      {
       ball_ptr[object_order[ball_num]] -> enabled = 1;
      }
    }
	else
    {

     if ( (UK_OR_US_BALL_SET == NO ) && (object_order[ball_num]!=0) &&
        ( object_order[ball_num]!=8 ) ) miss_stripes_and_spots=YES;

   if ( miss_stripes_and_spots == NO )
	 {

/* turn on vertex numbers */

     if ( (object_order[ball_num] > 0) && (hires_numbers==YES) ) {
      ball_numbers[object_order[ball_num]] -> enabled = 1;
     }


/* turn on ball spots */

     ball_ptr[object_order[ball_num]] -> enabled = 1;

/* turn on striped objects */

     if ( object_order[ball_num] > 8 )
     stripe_ptr[object_order[ball_num]] -> enabled = 1;
    }

	 }

/* render stripes-spots */
   if (toscr == 1)
    {
     E_RenderToBuffer(backscreen_buffer,0 , 0);
    }
     else
     {
     if (ball_num==0) {
      E_RenderToBuffer(backscreen_buffer,1 , 0);
      }
     else {
       E_RenderToBuffer(backscreen_buffer,0 , 0);
      }

     }

/* turn off vertex numbers */

     if (object_order[ball_num] > 0)
      {
       if (ball_numbers[object_order[ball_num]])  ball_numbers[object_order[ball_num]]-> enabled = 0;
		}

	 if (ball_ptr[object_order[ball_num]]) ball_ptr[object_order[ball_num]] -> enabled = 0;

     if ( object_order[ball_num] > 8 )
      {
       if (stripe_ptr[object_order[ball_num]]) stripe_ptr[object_order[ball_num]] -> enabled = 0;
      }
   }


  }

  if (RESOLUTION==HIGH)
  {
    // In hires mode, we copy the zones back up to the video buffer. In low
    // res modes, we copy the entire buffer forward anyway so we dont need
    // to copy the zones
    E_CopyRamToVga( backscreen_buffer);
  }
}

// ----------------------------------------------------------------------------
void Fast_Haaaa_DrawAllBalls(int toscr)
{
int   ball_num,miss_stripes_and_spots=NO;
int   ball_active[16],col,zone_radius,count,count1;
int	squish,shadow_offset, zone_bottom_add;
VERTEX ball_info[16];

static int    save_colour[16];

 	long	 loop;
	long object_order[16];
	long temp_pos2, sort_drop_out;
	unsigned char changed=TRUE;
	long	SHADOW_COLOUR;

/* turn off ball number textures */

   if ( (hires_numbers == YES) && (oldnumber_res!=hires_numbers))
    {
     oldnumber_res=hires_numbers;

     col = poolptr[2560 + (15)];

	  for (count1 = 1 ; count1 < LAST_BALL; count1++)
      {
       save_colour[count1] = ball_ptr[ count1 ] -> planes[0].colour;
		 
       for (count=0;count < ball_ptr[ count1 ] -> num_planes; count++)
        {
         ball_ptr[count1] -> planes[count].plane_type = plane_plain;
         ball_ptr[count1] -> planes[count].colour = col * 0x01010101;
	     }
	   }
      used_hires=YES;
	  }

   if ( (hires_numbers == NO) && (used_hires==YES) && (oldnumber_res!=hires_numbers))
    {
     oldnumber_res=hires_numbers;
	  for (count1 = 1 ; count1 < LAST_BALL; count1++)
      {
		 
       for (count=0;count < ball_ptr[count1] -> num_planes; count++)
        {
         ball_ptr[count1] -> planes[count].plane_type = plane_texture;
         ball_ptr[count1] -> planes[count].colour = save_colour[count1];
	     }
	   }
    }

	E_ClearZones();

/* setup shadow colour for balls */
 
	SHADOW_COLOUR=16*9;

	squish= E_Sin(camera_rot.x);

/* Now fill up order list */

	for (loop=0; loop<LAST_BALL; loop++) object_order[loop]=loop;

 	sort_drop_out=0;

/* Do bubble sort */

	while (changed) {
		changed=FALSE;

		for (loop=1; loop<LAST_BALL; loop++) {

			if (ball_ptr[object_order[loop]]->world_pos.z > ball_ptr[object_order[loop-1]]->world_pos.z) {
				temp_pos2=object_order[loop];
				object_order[loop]=object_order[loop-1];
				object_order[loop-1]=temp_pos2;
				changed=TRUE;
			}
		}
	}

  for (ball_num = 0; ball_num < LAST_BALL; ball_num++)
  {
   E_WorldRotateObject( ball_ptr[ object_order[ball_num] ] );
   ball_active[object_order[ball_num] ] = E_GetScreenPos( ball_ptr[object_order[ball_num]], &ball_info[object_order[ball_num]] );
  }

  for (ball_num = 0; ball_num < LAST_BALL; ball_num++)
  {
	 if ( ball_active[ object_order[ball_num]] == 0) 
    {
			
		zone_bottom_add = (squish * ball_info[ object_order[ball_num]].z) >> 16;

      zone_radius=(ball_info[ object_order[ball_num]].z * 18) >> 4;

      E_AddZone( (ball_info[ object_order[ball_num]].x - zone_radius) >> 4,
                 (ball_info[ object_order[ball_num]].y - zone_radius) >> 4,
                 (ball_info[ object_order[ball_num]].x + zone_radius) >> 4,
                 (ball_info[ object_order[ball_num]].y + (zone_radius + zone_bottom_add)) >> 4 );
	 }
  }

  E_MergeZones();
  E_BuildChunks();

  if (RESOLUTION==HIGH)
  {
    if (toscr == 1)
    {
		// Camera moving. We must download zones from screen so that we can
      // draw balls on top of them
      E_CopyVgaToRam( backscreen_buffer);
    } else {
		// Camera still. Copy zones from clean buffer to ball-draw buffer
      E_CopyRamToRam( backscreen_buffer1, backscreen_buffer);
    }
  } else {
    if (toscr==0)
    {
		// Camera still. Copy zones from clean buffer to get clean background
      E_CopyRamToRam( backscreen_buffer1, backscreen_buffer);
	 }
   }

  for (ball_num = 0; ball_num < LAST_BALL; ball_num++)
   {
    if ( ball_active[ object_order[ball_num]] == 0) 
     {
      switch	( ball[object_order[ball_num] ].ball_colour )
       {
        case	 BLACK:
        col=0;
        break;

        case	YELLOWBALL:
        col=7*16;
        break;

        case	PURPLE:
        col=5*16;
        break;

        case	BROWN:
        col=6*16;
        break;

        case	BLUEBALL:
        col=16;
        break;

        case	ORANGE:
        col=3*16;
        break;

        case	GREEN:	
        col=2*16;
        break;

        case	WHITE:
        col=128;
        break;

        case	STRIPE:
        col=128;
        break;			  

        case	RED:
        col=4*16;
        break;
       }

      shadow_offset = (E_Cos(camera_rot.x)
						 *ball_info[ object_order[ball_num]].z ) >> 16;

      if (RESOLUTION==LOW)
       {
        shadow_offset = (shadow_offset * 54613) >> 16;
	    }
/*
      E_DrawBall( ball_info[ object_order[ball_num]].x >> 1, 
                  ((ball_info[ object_order[ball_num]].y+shadow_offset) >> 1)+6 ,
                  ((ball_info[ object_order[ball_num]].z)*3) >> 3,
                  backscreen_buffer,
						SHADOW_COLOUR ,
						squish , 0
						);

      E_DrawBall( ball_info[ object_order[ball_num]].x >> 1, 
                  (ball_info[ object_order[ball_num]].y >> 1)+6,
                  ball_info[ object_order[ball_num]].z >> 1,
                  backscreen_buffer,
						col ,
						65536 , 1
						);
*/

	   E_ModifyBall( ball_info[ object_order[ball_num]].x >> 1,
		             (ball_info[ object_order[ball_num]].y >> 1) + 6,
		              ball_info[ object_order[ball_num]].z >> 1,
						  col,
                    1,
                    1,
                    ball_ptr[ object_order[ball_num]] -> world_pos.z + ball_ptr[ object_order[ball_num]] -> priority,
						  65536,
                    ball_num );

/* check to see if spots and stripes needed */

      miss_stripes_and_spots=NO;

      if ( (UK_OR_US_BALL_SET == NO ) && (object_order[ball_num]!=0) && (object_order[ball_num]!=8)  ) 
       miss_stripes_and_spots=YES;

      if ( miss_stripes_and_spots == NO )
       {
/* turn on vertex numbers */

        if ( (object_order[ball_num] > 0) && (hires_numbers==YES) ) 
         {
          ball_numbers[object_order[ball_num]] -> enabled = 1;
         }

/* turn on ball spots */

        ball_ptr[object_order[ball_num]] -> enabled = 1;

/* turn on striped objects */

        if ( object_order[ball_num] > 8 )
        stripe_ptr[object_order[ball_num]] -> enabled = 1;
		 }

     } else
        { // if ball active
         E_ModifyBall( 0, 0, 0, 0, 0, 0, 0, 0, ball_num);
        }

   } // for ball_num....

/* render stripes-spots */

   if (toscr == 1)
    {
     E_RenderToBuffer(backscreen_buffer,0 , 1);
    }
     else
     {
      if (ball_num==0) 
       {
        E_RenderToBuffer(backscreen_buffer,1 , 1);
       }
      else 
       {
        E_RenderToBuffer(backscreen_buffer,0 , 1);
       }
     }

#if 0
/* turn off vertex numbers */
     if (object_order[ball_num] > 0)
      ball_numbers[object_order[ball_num]] -> enabled = 0;

	 ball_ptr[object_order[ball_num]] -> enabled = 0;
     if ( object_order[ball_num] > 8 )
    stripe_ptr[object_order[ball_num]] -> enabled = 0;
   }
  }
#endif
  if (RESOLUTION==HIGH)
  {
    // In hires mode, we copy the zones back up to the video buffer. In low
    // res modes, we copy the entire buffer forward anyway so we dont need
    // to copy the zones
    E_CopyRamToVga( backscreen_buffer);
  }
}
/* -------------------------------------------------------------------------*/
//	cue_ptr -> enabled =NO;
void	MoveBall_new(void)
{
// RESTORE: positions, distances and steps 8.24, angle an SGL angle (were
// doubles, the angle in degrees)
   long	x,y,stepx,stepy,head_string_pos,loop,xadd,yadd;	
   ANGLE	angle;
   long	oldx,oldy,oldstepx,oldstepy,diameter;
   long		a,bad,ball_num;
	//renerware varibles //
   long      xDelta=0;
   long      yDelta=0,dx,dy,dx1,dy1;
   VERTEX		temp_vertex;
	long cush_x;
	long cush_y;

   reset_tracking_lines();

/* check ball against boundarys of table */

/* stop ball in hand being moved past hea string off break */

	switch (TABLESIZE_TYPE ) 
    {
     case US:
     head_string_pos=FIX24(0.58);
     break;

     case UK:
     head_string_pos=FIX24(0.2);
     break;

     case 2:
     head_string_pos=FIX24(0.18);
     break;
	 }

	switch	( TABLESIZE_TYPE ) {

   case US:
/* us table edges */

   xadd = FIX24(0.004450);
   yadd = FIX24(0.00455);
   break;

/* uk table edges */
   case UK:
   xadd = FIX24(0.00840);
   yadd = FIX24(0.00495);
   break;
   }

// Make translation vector for un-rotated camera

  switch ( TABLESIZE_TYPE )
   {
    case US:
    temp_vertex.x = mouse_xdiff ;
    temp_vertex.z =- mouse_ydiff ;
    temp_vertex.y = 0;
    break;

    case UK:
    temp_vertex.x = mouse_xdiff ;
    temp_vertex.z =- mouse_ydiff ;
    temp_vertex.y = 0;
    break;
	}

	// Rotate translation vector by camera's y rotation
   E_RotateVertex( 0, camera_rot.y, 0, &temp_vertex);

	// change into doubles and scale down to meters

   xDelta = (temp_vertex.x * 32768) / 5;	// RESTORE: /2560 m in 8.24
   yDelta = (temp_vertex.z * 32768) / 5;

	// xDelta and yDelta now hold the values to add to balls' positions

 //-----------------------------------------------------------------------//
 //		if camera is in OVER HEAD MODE then dont allow movement of ball	  //
 //-----------------------------------------------------------------------//

   oldx=ball[ball_to_move].x;
   oldy=ball[ball_to_move].y;

   ball[ball_to_move].x+=xDelta;
   ball[ball_to_move].y+=yDelta;

   stepx=oldx;
   stepy=oldy;

 /* check ball against all other balls on table */

   x=ball[ball_to_move].x-oldx;
   y=ball[ball_to_move].y-oldy;
   angle=(ANGLE)fix_atan2(y,x);	// RESTORE: SGL angle (was degrees, 0..360)

   loop =fix_hypot(x,y) ;	//loop counter for lenght moeed.	// RESTORE: 8.24

   diameter=ball[ball_to_move].diameter;

   while	(loop > 0)
    {
     oldstepx=stepx;
     oldstepy=stepy;
     stepx+=mul64(slCos(angle)<<8, FIX24(.0003)); //move ball to new position for checking	// RESTORE: 8.24
     stepy+=mul64(slSin(angle)<<8, FIX24(.0003)); 
     loop-=FIX24(.0003);

     bad=NO;

     for	(a = 0; a < LAST_BALL;a++)
      {
       if	( (a != ball_to_move) && (ball[a].active==TRUE) )
        {
         x = stepx-ball[a].x  ;//check ball to move against all balls
         y = stepy-ball[a].y  ;//in pack
         if	(fix_hypot(x,y) <= diameter ) bad=YES; //check balls	// RESTORE: 8.24 (was x*=x, y*=y, sqrt)
        }
      }

     if	(bad == NO)
      {
       ball[ball_to_move].x=stepx;	//move ball to new position
       ball[ball_to_move].y=stepy;	//if no ball in way

     if	( (break_flag==YES) && ( ball_to_move==WHITE_BALL ) ) 
       {
        if	( (WORLD_TO_FIX24(dummy_pointer -> modified_pos.x))-(ball[ball_to_move].x) < 0)
         {
          ball[ball_to_move].x =WORLD_TO_FIX24(dummy_pointer -> modified_pos.x);
          check_ball_in_safe_place( ball_to_move );
          stepx=ball[ball_to_move].x;
          stepy=ball[ball_to_move].y;
         }
	    }

   if ( (GAME_TYPE != US_POOL) && (break_flag==NO) )
    {

/* in Uk pool game, Check to see if player trying to move cue ball below
   head string if so then move cue ball above head string */

/* check to see if cue ball is below head string ? */

     if	( (WORLD_TO_FIX24(dummy_pointer -> modified_pos.x))-ball[ball_to_move].x < 0)
      {
/* move cue ball above head string */

        ball[ball_to_move].x = WORLD_TO_FIX24(dummy_pointer -> modified_pos.x);

        ball[ball_to_move].x_prev=ball[ball_to_move].x;
        ball[ball_to_move].y_prev=ball[ball_to_move].y;
        camera_target.x= FIX24_TO_WORLD(ball[WHITE_BALL].x);
        camera_target.z= FIX24_TO_WORLD(ball[WHITE_BALL].y);
 
/* smoothly move camera to cue bal in head string */

        if ( moved_cue_ball_to_head_string_flag == NO )
         {
          check_ball_in_safe_place( ball_to_move );

          ball[ball_to_move].x_prev=ball[ball_to_move].x;
          ball[ball_to_move].y_prev=ball[ball_to_move].y;
          camera_target.x= FIX24_TO_WORLD(ball[WHITE_BALL].x);
          camera_target.z= FIX24_TO_WORLD(ball[WHITE_BALL].y);

	       E_EaseCamera(ease_speed);
          E_PositionCamera( &camera_target, &camera_rot, camz*256 );
          moved_cue_ball_to_head_string_flag=YES;
	      }

       stepx=ball[ball_to_move].x;
       stepy=ball[ball_to_move].y;
	   }
    }
/* --------------------- */
/* check cue ball in 'D' */

   if ( SNOOKER_MODE == YES )
    {

     dx=WORLD_TO_FIX24(dummy_pointer -> modified_pos.x);	// RESTORE: 8.24
	  dx1=ball[ball_to_move].x-dx;
     dy=0;
	  dy1=ball[ball_to_move].y-dy;
     if ( dx1 < 0)
      {
	    dx1=dx-ball[ball_to_move].x;
      }
     if ( dy1 < 0)
      {
	    dy1=dy-ball[ball_to_move].y;
      }

     if ( fix_hypot(dy1,dx1) > FIX24(.292))	// RESTORE: 8.24
      {
       loop=-1;
       ball[ball_to_move].x=oldstepx;	//reset ball to old position, so that we can check every pixel!
       ball[ball_to_move].y=oldstepy;
		}

    }

      }
      else	
       {
        loop=-1;
        ball[ball_to_move].x=oldstepx;	//reset ball to old position, so that we can check every pixel!
        ball[ball_to_move].y=oldstepy;
        break;				//if collision then exit loop
       }

/* check boundary's of table */

      ball_num=ball_to_move;

    	if ((min_cush_x1-ball[ball_num].x)>0) {

      if ( (check_solid_pocket(ball_num,oldstepx,oldstepy)==-1) )  loop=-1;
		cush_x=get_cush_x(1,0,ball[ball_num].y);

 		if ((ball[ball_num].x<cush_x)) {

      loop=-1;
   	ball[ball_num].x=cush_x;

	   }
	   }

	   if ((max_cush_x4-ball[ball_num].x)<0) {
      if ( (check_solid_pocket(ball_num,oldstepx,oldstepy)==-1) )  loop=-1;
	 	cush_x=get_cush_x(4,0,ball[ball_num].y);

		if ((ball[ball_num].x>cush_x)) {
      loop=-1;
      ball[ball_num].x=cush_x;

      }
      }


      if ((min_cush_y3-ball[ball_num].y)>0) {

      if ( (check_solid_pocket(ball_num,oldstepx,oldstepy)==-1) )  loop=-1;

	    cush_y=get_cush_y(2,ball[ball_num].x,0);
	    if (((ball[ball_num].y<cush_y))) {
       loop=-1;
	    ball[ball_num].y=cush_y;
       }
      }

      if ( (max_cush_y2-ball[ball_num].y)<0) {
      if ( (check_solid_pocket(ball_num,oldstepx,oldstepy) ==-1) )  loop=-1;

		cush_y=get_cush_y(3,ball[ball_num].x,0);

		if (((ball[ball_num].y>cush_y))) {
      loop=-1;
      ball[ball_num].y=cush_y;
     }
     }

    }

   ball[ball_to_move].x_prev=ball[ball_to_move].x;
   ball[ball_to_move].y_prev=ball[ball_to_move].y;

/* if not in uk pool, must be US 9 Ball , allow player to move ball any where
 on the table */

   if ( camera_mode == MOVE_CAMERA_VIEWPOINT )
    {
     camera_mode = ZOOM_BACK_TO_CUE_BALL ;
     camera_target.x= FIX24_TO_WORLD(ball[WHITE_BALL].x);
     camera_target.y= FIX24_TO_WORLD(ball[WHITE_BALL].radius);
     camera_target.z= FIX24_TO_WORLD(ball[WHITE_BALL].y);

     E_EaseCamera(ease_speed);
     E_PositionCamera( &camera_target, &camera_rot, camz*256 );
    }

 /* smoothly move camera backto cue ball when ball in hand */

   if ( camera_mode == ZOOM_BACK_TO_CUE_BALL )
    {
     camera_target.x= FIX24_TO_WORLD(ball[WHITE_BALL].x);
     camera_target.y= FIX24_TO_WORLD(ball[WHITE_BALL].radius);
     camera_target.z= FIX24_TO_WORLD(ball[WHITE_BALL].y);

	 } 
   else
    {
     num_frames_camera_still = 0;
    }

}
/* ------------------------------------------------------------------------ */
void	Move_Camera_Position( long camera_status)
{
	int	angle,ball_loop;
	double	x,y;
VERTEX	*table_ptr;

      if	( TABLESIZE_TYPE == 1) {
       table_ptr = special_camera_positions;
		 }
      else
       {
       table_ptr = special_us_camera_positions;
       }

/* check to see if trying to select same camera angle */

		if	(camera_mode == camera_status) return;

		camera_mode=camera_status;

		switch	(camera_status)
		{
/* -------------------------------------------- */

			case	ZOOM_BACK_TO_CUE_BALL:

         if ( activated_lines == FALSE) 
     		 {
		   	camera_target.x= FIX24_TO_WORLD(ball[WHITE_BALL].x);	// RESTORE: 8.24
		   	camera_target.y= FIX24_TO_WORLD(ball[WHITE_BALL].radius);	// RESTORE: 8.24
		   	camera_target.z= FIX24_TO_WORLD(ball[WHITE_BALL].y);	// RESTORE: 8.24
			 }
         else
          {
		   	camera_target.x= FIX24_TO_WORLD(saveball_lines[WHITE_BALL].x);	// RESTORE: 8.24
		   	camera_target.y= FIX24_TO_WORLD(saveball_lines[WHITE_BALL].radius);	// RESTORE: 8.24
		   	camera_target.z= FIX24_TO_WORLD(saveball_lines[WHITE_BALL].y);	// RESTORE: 8.24
          }
         switch	( TABLESIZE_TYPE ) 
          {
           case US:
           camz = DEFAULT_ZOOM*2;
           camera_rot.x = 128;
           break;

           case UK:
           camz = DEFAULT_ZOOM*1.5;
           camera_rot.x = 128;
           break;
			 }

			E_EaseCamera(ease_speed);
         E_PositionCamera( &camera_target, &camera_rot, camz*256 );

			break;

/* -------------------------------------------- */

			case	MOVE_BACK_TO_CUE_BALL:

         if ( activated_lines == FALSE) 
     		 {
		   	camera_target.x= FIX24_TO_WORLD(ball[WHITE_BALL].x);	// RESTORE: 8.24
		   	camera_target.y= FIX24_TO_WORLD(ball[WHITE_BALL].radius);	// RESTORE: 8.24
		   	camera_target.z= FIX24_TO_WORLD(ball[WHITE_BALL].y);	// RESTORE: 8.24
			 }
         else
          {
		   	camera_target.x= FIX24_TO_WORLD(saveball_lines[WHITE_BALL].x);	// RESTORE: 8.24
		   	camera_target.y= FIX24_TO_WORLD(saveball_lines[WHITE_BALL].radius);	// RESTORE: 8.24
		   	camera_target.z= FIX24_TO_WORLD(saveball_lines[WHITE_BALL].y);	// RESTORE: 8.24
          }
         switch	( TABLESIZE_TYPE ) 
          {
           case US:
           camera_rot.x = 128;
           break;

           case UK:
           camera_rot.x = 128;
           break;
			 }

			E_EaseCamera(ease_speed);
         E_PositionCamera( &camera_target, &camera_rot, camz*256 );
    		camera_mode=ZOOM_BACK_TO_CUE_BALL;
			break;


/* -------------------------------------------- */
/* setup camera in overhead mode */

			case	OVER_HEAD_CAMERA:
         camera_mode=-1;

			camera_rot = table_ptr[0];
			camera_target = table_ptr[1];
         switch	( TABLESIZE_TYPE ) 
          {
           case US:
           camz=4974;
           break;

           case UK:
           camz=3900;
           camera_target.z -= 20000;
           break;
			 }

         if ( SNOOKER_MODE == YES )   
          {
           camz+=2276+(6000-4794);
			 }
/* setup high resolution offset for uk and us tables */

         if (RESOLUTION == HIGH)
           camera_target.z -= 80000;

			E_EaseCamera(ease_speed);
         E_PositionCamera( &camera_target, &camera_rot, camz*256 );

  //	 		camera_mode=ZOOM_BACK_TO_CUE_BALL+200;
  //	 		camera_status=ZOOM_BACK_TO_CUE_BALL+200;
			break;									  

/* -------------------------------------------- */

			case	CORNER1_CAMERA:
         camera_mode=-1;
         switch	( TABLESIZE_TYPE ) 
          {
           case US:
           camz=6000;
           break;

           case UK:
           camz=4000;
           break;
			 }
         if ( SNOOKER_MODE == YES )   
          {
           camz+=2276;
			 }
			camera_target.x =0;
			camera_target.y =0;
			camera_target.z =0;
			camera_rot = table_ptr[2];

			E_EaseCamera(ease_speed);
         E_PositionCamera( &camera_target, &camera_rot, camz*256 );
			break;

/* -------------------------------------------- */

			case	CORNER2_CAMERA:
         camera_mode=-1;
         switch	( TABLESIZE_TYPE ) 
          {
           case US:
           camz=6000;
           break;

           case UK:
           camz=4000;
           break;
			 }
         if ( SNOOKER_MODE == YES )   
          {
           camz+=2276;
			 }
			camera_target.x =0;
			camera_target.y =0;
			camera_target.z =0;
			camera_rot = table_ptr[4];
			E_EaseCamera(ease_speed);
         E_PositionCamera( &camera_target, &camera_rot, camz*256 );
			break;

/* -------------------------------------------- */

			case	CORNER3_CAMERA:
         camera_mode=-1;
         switch	( TABLESIZE_TYPE ) 
          {
           case US:
           camz=6000;
           break;

           case UK:
           camz=4000;
           break;
			 }
         if ( SNOOKER_MODE == YES )   
          {
           camz+=2276;
			 }
			camera_target.x =0;
			camera_target.y =0;
			camera_target.z =0;
			camera_rot = table_ptr[6];
			E_EaseCamera(ease_speed);
         E_PositionCamera( &camera_target, &camera_rot, camz*256 );
			break;

/* -------------------------------------------- */

			case	CORNER4_CAMERA:
         camera_mode=-1;

         switch	( TABLESIZE_TYPE ) 
          {
           case US:
           camz=6000;
           break;

           case UK:
           camz=4000;
           break;
			 }
         if ( SNOOKER_MODE == YES )   
          {
           camz+=2276;
			 }

			camera_target.x =0;
			camera_target.y =0;
			camera_target.z =0;
			camera_rot = table_ptr[8];
			E_EaseCamera(ease_speed);
         E_PositionCamera( &camera_target, &camera_rot, camz*256 );
			break;

/* -------------------------------------------- */

			case	LINE_UP_SHOT:

			/* get angle to next ball to pot */


			ball_loop=1;
			while	(nine_ball_count != ball[ball_loop].num ) ball_loop++;

         x=ball[WHITE_BALL].x-ball[ball_loop].x;
         y=ball[WHITE_BALL].y-ball[ball_loop].y;
          
         if	( (angle=(int)((atan2((double)y,(double)x))/deg2rad)) < 0 )  angle+=360;
			if	( (angle += 90) >= 360) angle-=360;
			angle=360-angle;

			camera_rot.y = (angle * 4096) / 360;

         E_PositionCamera( &camera_target, &camera_rot, camz*256 );
			E_EaseCamera(ease_speed);
         cue_rot = camera_rot;

	 		camera_mode=ZOOM_BACK_TO_CUE_BALL;
	 		camera_status=ZOOM_BACK_TO_CUE_BALL;
         break;
      }
}
/* ------------------------------------------------------------------------ */
void setup_camera_after_shot(void)
{

    camera_rot=cue_rot;

    shoot_mode=2;
    cue_position=FIX24_TO_DOUBLE(ball[WHITE_BALL].diameter*4);	// RESTORE: 8.24

    E_EaseCamera(ease_speed);
    E_PositionCamera( &camera_target, &camera_rot, camz*256 );
    E_UpdateCamera();
}
/* ------------------------------------------------------------------------ */
VERTEX   save_rot, save_cuerot;
void	check_all_camera_positions(void)
{
VERTEX  tmp_rot = camera_rot,tmp_cuerot = cue_rot ;
char c;

   altkey=NO;
   shootkey=NO;
   trackkey=NO;
   quitkey=NO;

   if (getkey(kLEFTALT)==YES) altkey=YES;
   if (getkey(kS)==YES) shootkey=YES;

   do
   {
    c=get_key();

/* show game type rules */

   if ( end_of_frame_flag==NO)
    {
     if ( c=='D'	)
      {
       replay_flag=YES; // setup flag to stop clock 
//g       show_game_rules();		// menu.c ?????
       replay_flag=NO; // setup flag to start clock 
      }

/* show in game controls option */

     if ( c=='H' )
      {
       replay_flag=YES; // setup flag to stop clock 
       show_game_controls();
       replay_flag=NO; // setup flag to start clock 
      }
    }

   if ( (c=='?') && (SNOOKER_MODE == NO) && (PLAYERS_SELECTED!=2) )
      //check tab key only in pool games alse not in tournament section or
      //when computer is playimg
    {
     replay_flag=YES; // setup flag to stop clock 
//g     book_of_game_rules();		// menu.c????

/* setup table friction in case player changed it in rule book */
     setup_table_friction();
     replay_flag=NO; // setup flag to start clock 
	 }


/* check quit frame */

    if ( c=='Q' ) quitkey=YES;
    if ( c=='{' ) quitkey=YES;

    if (c=='T') trackkey=YES;

 	if	( (c=='!') && (camera_mode != OVER_HEAD_CAMERA) )
     {
		Move_Camera_Position(OVER_HEAD_CAMERA);
      changed_view = YES;
	  }

 	if	( (c=='"') && (camera_mode !=CORNER1_CAMERA) ) 
     {
		Move_Camera_Position(CORNER1_CAMERA);
      changed_view = YES;
	  }

 	if	( (c=='œ') && (camera_mode !=CORNER2_CAMERA) ) 
     {
		Move_Camera_Position(CORNER2_CAMERA);
      changed_view = YES;
	  }

 	if	( (c=='$') && (camera_mode !=CORNER3_CAMERA) ) 
     {
		Move_Camera_Position(CORNER3_CAMERA);
      changed_view = YES;
	  }

 	if	( (c=='%') && (camera_mode !=CORNER4_CAMERA) ) 
     {
		Move_Camera_Position(CORNER4_CAMERA);
      changed_view = YES;
	  }

 	if	( (event_flag == MOVE_CAMERA) && (camera_mode != MOVE_CAMERA_VIEWPOINT) ) 
      {
       camera_mode = MOVE_CAMERA_VIEWPOINT;
       changed_view = YES;
      }

     if ( changed_view != old_view)
      {
       save_rot=tmp_rot;
       save_cuerot=tmp_cuerot;
       old_view = changed_view;
		}

   if ( (c=='A') && (camera_mode !=ZOOM_BACK_TO_CUE_BALL) ) 
      {
       refresh_whole_screen=2;
       changed_view=NO;
       camera_rot = save_rot;
       cue_rot = save_cuerot;
		 Move_Camera_Position(ZOOM_BACK_TO_CUE_BALL);
       setup_camera_after_shot();
       foul_occured=AIM_MODE;
       return;
      }

     if	( (c=='A') && (shoot_mode==YES) && (camera_mode==ZOOM_BACK_TO_CUE_BALL) ) 
      {
       setup_camera_after_shot();
       foul_occured=AIM_MODE;
	   }

#if 0
 	if	( (c=='^') && (camera_mode !=ZOOM_BACK_TO_CUE_BALL) ) 
      {
       changed_view=NO;
       camera_rot = save_rot;
       cue_rot = save_cuerot;
		 Move_Camera_Position(ZOOM_BACK_TO_CUE_BALL);
      }
#endif

 	if	(c=='^') //F6
    {
     Re_Rack_Balls_with_other_gametype();
    }


/* check to see if player wants to re-rack balls in this frame */

   if ( c=='*' ) //F8
    {
     Re_Rack_Balls();
	 }

/* ------------------------ */
/* change detail level of scene */

  if ( DEMO_PLAY == NO)
   {
    if (c=='(')  //F9
     {
      detail_level ^= 1;
      save_res = detail_level;
      num_frames_camera_still = 0; 
     }
	}

/* ------------------------ */
/* test to see if obect ball or cue ball off screen, if so then zoom camera
back to show shot */

   if ( (c=='_') && (DEMO_PLAY == NO) ) zoom_flag^=1; //F11

   if ( c=='&' ) 	//F7
    {
     check_change_resolution();
	 }

/* check to see if player wants to stop camera zooming back to cue ball
   after every shot taken */

   if ( c==')' )	 //F10
    {
     zoom_after_shot_flag^=1;
     if ( zoom_after_shot_flag == YES )
      {
       camera_mode=-1;
       Move_Camera_Position(ZOOM_BACK_TO_CUE_BALL);
		}
	 }

   } while( c!=0 );

}
/* ------------------------------------------------------------------------ */
	int	old_scr_x,old_scr_y,new_scr_x,new_scr_y;

// ----------------------------------------------------------------------------
// the pentium can multiply two 32-bit numbers together to produce a 64-bit
// result but watcom only keeps the bottom 32-bits. This routine multiplies
// the two numbers together then shifts the result right by 16 bits. This way
// we can multiply a 32-bit number by a 16-bit cosine without losing significant
// bits through overflow

//extern int fracmul( int a, int b);
//
//#pragma aux fracmul =	\
//  "imul edx"		\
//  "shrd eax,edx,16"	\
//  modify [edx]		\
//  value [eax]		\
//  parm [eax] [edx];
//
// ----------------------------------------------------------------------------
//extern int fracdiv( int a, int b);
//
//#pragma aux fracdiv =	\
//  "cdq"			\
//  "shld edx,eax,16"	\
//  "shl eax,16"		\
//  "idiv ebx"		\
//  modify [edx]		\
//  value [eax]		\
//  parm [eax] [ebx];
//
/* ------------------------------------------------------------------------ */
void	Draw_Head_String( char *backscreen )
{
OBJECT	*dummy_pointer;
OBJECT	*dummy_pointer1;
long i;

#ifdef NO_TABLE
   return;
#endif

    num_lines_drawn=0;

/* get pointers to dummy objects */

//   if (LOCATION > DIFFERENT_SHAPED_TABLES_INDEX-1) return;

   linesort = 15 << 24;

   dummy_pointer=E_GetObjectPtr("dummy1");
   dummy_pointer1=E_GetObjectPtr("dummy2");

   E_WorldRotateObject(dummy_pointer);
   E_WorldRotateObject(dummy_pointer1);

	Draw_Line( backscreen , 9 , dummy_pointer,  dummy_pointer1 );

   if ( SNOOKER_MODE == NO ) return;

   for ( i = 0; i < 8; i++)
	 {
     E_WorldRotateObject(d_ptr[i]);
     E_WorldRotateObject(d_ptr[i+1]);
     Draw_Line( backscreen , 9 , d_ptr[i],  d_ptr[i+1] );
	 }
}
/* ------------------------------------------------------------------------ */
#define	MAX_LINES 700
void	Draw_Line( char *backscreen , char col , OBJECT *dummy, OBJECT *dummy1)
{
OBJECT	*dummy2;
VERTEX	ball_info;
int		x,y,x1,y1, fraction;

    if ( num_lines_drawn >= MAX_LINES ) return;
    num_lines_drawn++;

/* check to see if both points not on screen */

	if	( (dummy -> world_pos.z < 2560) && (dummy1 -> world_pos.z < 2560) ) return;

/* check to see if point 1 is below point 2, if so then swap thaem around */

	if (dummy->world_pos.z > dummy1->world_pos.z)
    {
     dummy2 = dummy;
     dummy = dummy1;
     dummy1 = dummy2;
	 }

	if (dummy->world_pos.z < 2048)
    {
	  fraction = fracdiv( 2048 - dummy->world_pos.z, dummy1->world_pos.z - dummy->world_pos.z);

	  dummy->world_pos.x += fracmul( dummy1->world_pos.x - dummy->world_pos.x, fraction);
	  dummy->world_pos.y += fracmul( dummy1->world_pos.y - dummy->world_pos.y, fraction);
     dummy->world_pos.z = 2048;
    }

/* get screen (2d x,y) of point 1 */

	E_GetScreenPos( dummy, &ball_info );

	x= ball_info.x >> 1;
	y= ball_info.y >> 4;

/* get screen (2d x,y) of point 2 */

	E_GetScreenPos( dummy1, &ball_info );

	x1= ball_info.x >> 1;
	y1= ball_info.y >> 4;

/* draw line in black */

   E_Draw2DLine(x,y,x1,y1,col,linesort);
}
/* ------------------------------------------------------------------------ */
void	Draw_Line_cross( char *backscreen , char col , OBJECT *dummy, OBJECT *dummy1, int xoffset,int yoffset)
{
OBJECT	*dummy2;
VERTEX	ball_info;
int		x,y,x1,y1, fraction;

/* check to see if both points not on screen */

	if	( (dummy -> world_pos.z < 2560) && (dummy1 -> world_pos.z < 2560) ) return;

/* check to see if point 1 is below point 2, if so then swap thaem around */

	if (dummy->world_pos.z > dummy1->world_pos.z)
    {
     dummy2 = dummy;
     dummy = dummy1;
     dummy1 = dummy2;
	 }

	if (dummy->world_pos.z < 2048)
    {
	  fraction = fracdiv( 2048 - dummy->world_pos.z, dummy1->world_pos.z - dummy->world_pos.z);

	  dummy->world_pos.x += fracmul( dummy1->world_pos.x - dummy->world_pos.x, fraction);
	  dummy->world_pos.y += fracmul( dummy1->world_pos.y - dummy->world_pos.y, fraction);
     dummy->world_pos.z = 2048;
    }

/* get screen (2d x,y) of point 1 */

	E_GetScreenPos( dummy, &ball_info );

	x= ball_info.x >> 1;
	y= ball_info.y >> 4;

/* get screen (2d x,y) of point 2 */

	E_GetScreenPos( dummy1, &ball_info );

	x1= ball_info.x >> 1;
	y1= ball_info.y >> 4;

/* draw line in black */

   xoffset = (xoffset * (1748<<3)) / (camz) ;

   yoffset = (yoffset * 1748) / (camz) ;

   yoffset = (yoffset * pixel_aspect) >> 16;

   linesort = 11 << 24;
   E_Draw2DLine(x-xoffset,y-yoffset,x1+xoffset,y1+yoffset,col,linesort);
}
/* ------------------------------------------------------------------------ */


void get_new_screen_pos(long loop)
{
 long loop1;
 long count;

/* setup cue ball lines for masse shot */

   if ( num_lines_drawn >= MAX_LINES ) return;

   if ( (loop == WHITE_BALL) && (butt > 1) && (x_hit_pos!=0) )
    {

    if ( ball[loop].active==NO) return; 

/* check for deflection angle in between frame update */

    if ( ball_col_list_pos > 0)
     {
      count=0;
      while ( count < ball_col_list_pos )
       {
		  if ( ball_col[ count ].ball_num == loop )
         {
          line_pointers[loop][ line_pointer_index[loop] ].x =ball_col[count].x;
          line_pointers[loop][ line_pointer_index[loop] ].y =ball_col[count].y;
          line_pointers[loop][ line_pointer_index[loop] ].z =ball[loop].radius;
          line_pointer_index[loop]++;
          ball_col[count].ball_num = 255; /* mark as bad ball */
         }
		  count++;
       }
     }

     line_pointers[loop][ line_pointer_index[loop] ].x =ball[loop].x ;
     line_pointers[loop][ line_pointer_index[loop] ].y =ball[loop].y ;
     line_pointers[loop][ line_pointer_index[loop] ].z =ball[loop].radius;
     line_pointer_index[loop]++;
     line_set[loop]=NO;
	  return;
    }

/* store position of line */

  if ( line_pointer_index[loop] == 0 )
   {

/* stop pixel being plotted if ball pocketed */

    if ( ball[loop].active==NO) return; 

    line_pointers[loop][ line_pointer_index[loop] ].x =ball[loop].x ;
    line_pointers[loop][ line_pointer_index[loop] ].y =ball[loop].y ;
    line_pointers[loop][ line_pointer_index[loop] ].z =ball[loop].radius;
    line_pointer_index[loop]++;
    line_set[loop]=NO;
   }

/* check for deflection angle in between frame update */

   if ( ball_col_list_pos > 0)
    {
     count=0;

     while ( count < ball_col_list_pos )
      {
		 if ( ball_col[ count ].ball_num == loop )
        {
         line_pointers[loop][ line_pointer_index[loop] ].x =ball_col[count].x;
         line_pointers[loop][ line_pointer_index[loop] ].y =ball_col[count].y;
         line_pointers[loop][ line_pointer_index[loop] ].z =ball[loop].radius;
         line_pointer_index[loop]++;
         ball_col[count].ball_num = 255; /* mark as bad ball */
         line_set[loop]=NO;
        }
		 count++;
      }
	 }

    if ( ball[loop].moving==YES)      line_set[loop]=NO;

    if  ( (ball[loop].active==NO) or ( ball[loop].moving==NO) )
     {
      if ( line_set[loop]==NO )
       {
        line_pointers[loop][ line_pointer_index[loop] ].x =ball[loop].x ;
        line_pointers[loop][ line_pointer_index[loop] ].y =ball[loop].y ;
        line_pointers[loop][ line_pointer_index[loop] ].z =ball[loop].radius;
        line_pointer_index[loop]++;
		 }
      line_set[loop]=YES;
    }
}
/* ------------------------------------------------------------------------ */
void	draw_tracking_lines( char *buffer)
{
	int	loop,lineloop,col;
flag miss;
/* get ball colour */

  num_lines=0;

/* only draw line once all balls have stoped moving */

  if ( activated_lines == FALSE) return;

//  if ( (any_balls_moving()==TRUE) ) return;

/* -------------------------------------------- */

  line1 -> enabled = 0;
  line2 -> enabled = 0;

  for (lineloop=0; lineloop < LAST_BALL; lineloop++ )
	{
	 miss=NO;
    if ( lineloop > 0 )
     {
/* check to see if ball as stopped moving and also check to see if ball
 active (not potted) */

//		if ( (ball[lineloop].moving==YES) or (ball[lineloop].active==NO) ) miss=YES;

     }

	if (miss==NO) 
   {
/* draw line list */

    switch	( ball[lineloop].ball_colour )
     {
      case	 BLACK:
      col=0;
      break;

      case	YELLOWBALL:
      col=7*16;
      break;

      case	PURPLE:
      col=5*16;
      break;

      case	BROWN:
      col=6*16;
      break;

      case	BLUEBALL:
      col=16;
      break;

      case	ORANGE:
      col=3*16;
      break;

      case	GREEN:	
      col=2*16;
      break;

      case	WHITE:
      col=128;
      break;

      case	STRIPE:
      col=128;
      break;			  

      case	RED:
      col=4*16;
      break;
     }

       if	( line_pointer_index[lineloop] >= 2 ) 
        {

        line1 -> enabled = 1;
        line2 -> enabled = 1;

         for (loop=1 ; loop < line_pointer_index[lineloop]; loop++ )
          { 

             linesort = 12 << 24;

/* check to see if pointer in table yet */

             E_MoveObject(   FIX24_TO_WORLD(line_pointers[lineloop][loop].x) , (double) 0 ,	// RESTORE: 8.24
                             FIX24_TO_WORLD(line_pointers[lineloop][loop].y)  , line1 );	// RESTORE: 8.24

             E_MoveObject(   FIX24_TO_WORLD(line_pointers[lineloop][loop-1].x) , (double)0,	// RESTORE: 8.24
                             FIX24_TO_WORLD(line_pointers[lineloop][loop-1].y) , line2 );	// RESTORE: 8.24

             E_WorldRotateObject(line1);
             E_WorldRotateObject(line2);

/* draw line */

             Draw_Line( buffer , 9 , line1 , line2 );



/* check to see if pointer in table yet */

             E_MoveObject(   FIX24_TO_WORLD(line_pointers[lineloop][loop].x) , FIX24_TO_WORLD(line_pointers[lineloop][loop].z) ,	// RESTORE: 8.24
                             FIX24_TO_WORLD(line_pointers[lineloop][loop].y)  , line1 );	// RESTORE: 8.24

             E_MoveObject(   FIX24_TO_WORLD(line_pointers[lineloop][loop-1].x) , FIX24_TO_WORLD(line_pointers[lineloop][loop-1].z),	// RESTORE: 8.24
                             FIX24_TO_WORLD(line_pointers[lineloop][loop-1].y) , line2 );	// RESTORE: 8.24

             E_WorldRotateObject(line1);
             E_WorldRotateObject(line2);

/* draw line */
             linesort = 11 << 24;

             Draw_Line( buffer , col/16 , line1 , line2 );

			    num_lines++;
				}
	       }
      }
    }
}
/* ------------------------------------------------------------------------ */
void	build_tracking_line_points(void)
{
  long  count;

#define BALL_HIT_DROP_OUT 5 //number of ball hits before droping out of loop

   if ( activated_lines==TRUE )
    {
//     if ((any_balls_moving()==FALSE) ) return;

     ball_hits_count=0;

     for (count=0;count< 100 ;count++)
      {
 
       update_all_balls(FIX24_ONE/frame_rate);	// RESTORE: 8.24

       if ( ball_hits_count > BALL_HIT_DROP_OUT) return;

      } 
	 }
}
/* ------------------------------------------------------------------------ */
void setup_ai_camera(void)
{
	camera_target = special_us_camera_positions[11];
	camera_pos = special_us_camera_positions[11];
	camera_rot = special_us_camera_positions[10];

	switch	( TABLESIZE_TYPE ) 
    {
     case US:
     camz = 3144 ;
	  break;

     case UK:
     camz = 2110 ;
     camera_target = special_camera_positions[11];
	  camera_rot = special_camera_positions[10];
	  break;
    }

	E_EaseCamera(ease_speed);
   E_PositionCamera( &camera_target, &camera_rot, camz*256 );
}
/* ------------------------------------------------------------------------ */
 OBJECT *pocket_pods[6];
 OBJECT *pocket_pods1[6];
void  setup_detail_level_of_room(void)
{
 long a,x;

   E_SelectObject("ukleg02");
   E_EnableObjects( 1);
   E_SelectObject("pocket*");
   E_EnableObjects( 1);
   E_SelectObject("surface");
   E_EnableObjects( 1);

 
   switch (detail_level)
	{
   case LOW:

   switch (LOCATION)
    {
     case 4:
     moon_room();
     break;

     case 5:
     moon_room();
     break;

     case 6:
     beach_room();
     break;

     case 7:
     coffin_room();
     break;

     case 9:
     toy_room();
     break;

     default:
     normal_rooms();   
     break;
	 }

   break;
   
   case HIGH:

   E_SelectObject("*");
   E_EnableObjects( 1);

   for (a=0;a<6;a++) if(bumper_ptr[a]) bumper_ptr[a] -> enabled =0;
   for (a=0;a<6;a++) if(sidebumper_ptr[a]) sidebumper_ptr[a] -> enabled =0;
	cue_ptr -> enabled =0;

/* in continuouse pool allow player to select pocket, and ball */

   if ( LOCATION != 9)//toy room
   {
   E_SelectObject("outer*");
   E_EnableObjects( 0);
	}

   break;
   }

   pocket_pods1[0] = E_GetObjectPtr( "pocket1col" );
   pocket_pods1[1] = E_GetObjectPtr( "pocket2col" );
   pocket_pods1[2] = E_GetObjectPtr( "pocket3col" );
   pocket_pods1[3] = E_GetObjectPtr( "pocket4col" );
   pocket_pods1[4] = E_GetObjectPtr( "pocket5col" );
   pocket_pods1[5] = E_GetObjectPtr( "pocket6col" );

   pocket_pods[0] = E_GetObjectPtr( "pocket1" );
   pocket_pods[1] = E_GetObjectPtr( "pocket2" );
   pocket_pods[2] = E_GetObjectPtr( "pocket3" );
   pocket_pods[3] = E_GetObjectPtr( "pocket4" );
   pocket_pods[4] = E_GetObjectPtr( "pocket5" );
   pocket_pods[5] = E_GetObjectPtr( "pocket6" );

   E_SelectObject("stripe*");
   E_EnableObjects( 0);
   E_SelectObject("spot*");
   E_EnableObjects( 0);
   E_SelectObject("diamond*");
   E_EnableObjects( 0);

   for (a=0;a<16;a++)
    {
     if (ball_numbers[a]) ball_numbers[a] -> enabled = 0;
	 }

   E_SelectObject("10");
   E_EnableObjects( 0);

/* turn off trcking lines dummy objects */

   if (line1) line1 -> visible = 0;
   if (line2) line2 -> visible = 0;
   if (line3) line3 -> visible = 0;
   if (line4) line4 -> visible = 0;

   if (line1) line1 -> enabled = 0;
   if (line2) line2 -> enabled = 0;
   if (line3) line3 -> enabled = 0;
   if (line4) line4 -> enabled = 0;

	for (x=0;x<6;x++) 
    {
     if (pocket_pods[x]) pocket_pods[x]-> enabled =1;
     if (pocket_pods1[x]) pocket_pods1[x]-> enabled =0;
	 }

   setup_nms_pocket_graphics();
   setup_continuous_pocket();
   Select_Pocket_And_Ball();

   if (cross_ptr) cross_ptr -> enabled = NO;

#ifdef NO_TABLE

   E_SelectObject("corner*");
   E_EnableObjects( 0);
   E_SelectObject("ukleg02");
   E_EnableObjects( 0);
   E_SelectObject("pocket*");
   E_EnableObjects( 0);
   E_SelectObject("surface");
   E_EnableObjects( 0);
   E_SelectObject("shadow");
   E_EnableObjects( 0);

/* turn off lights in pool hall over table */

/* dont draw other table legs in low detail, for pool hall room */
   if (LOCATION !=3)
    {
     E_SelectObject("Leg0*");
     E_EnableObjects( 0);
    }
   else
    {
     E_SelectObject("ukleg01");
     E_EnableObjects( 0);
    }
#endif

   if ( TABLESIZE_TYPE == US)
    {
     E_SelectObject("light01");
     E_EnableObjects( 0);
    }
   else
    {
     E_SelectObject("light02");
     E_EnableObjects( 0);
	 }

}
/* ------------------------------------------------------------------------ */

void setup_continuous_pocket(void)
{
long x1,col,tmp1=pocket_selected,count,disc_col;
double angle,angle1,x,y;

	if	( DEMO_PLAY == YES ) return;
//   if ( (GAME_TYPE != US_8BALL) && (GAME_TYPE != CONTINUOUS ) ) return;
   if ( GAME_TYPE != CONTINUOUS ) return;
   if ( show_shot_flag == YES ) return;


   if ( pocket_nominated != -1) return;

/* setup colour of disk */

   switch ( TABLE_TYPE )
    {
     case RED_BAIZE:
      disc_col = 8;   
      break;
     case GREEN_BAIZE:
      disc_col = 8;  
      break;
     case BLUE_BAIZE:
      disc_col = 8;   
      break;
    }

/* get pocket in view */

   angle1 = (cue_angle-1024)&4095;
   angle1 = (double)(angle1 / 4096) * 360;

   angle1 =360-angle1;

   count=0;
   while (count < 6)
    {
     x = pockets[count].x - ball[0].x ;
     y = pockets[count].y - ball[0].y ;

     if ( (angle=((double)atan2((double)y,(double)x)/deg2rad)) < 0 ) angle+=360;
	  if ( ( angle = angle-angle1 ) < 0) angle+=360;

/* check to see if pocket in range */
     if ( angle > 180 ) angle=360-angle;
     if ( angle < 10  ) break;

     count++;
	 }

/* check to see if in range of any pockets,
  if not then leave on last pocket */

   if (count < 6 ) pocket_selected = count;
   pocket_selected %=6;

/* re-render whole scene if new pocket selected */

   if (tmp1 != pocket_selected)  
    {

/* re-colour all pockets to black */

     num_frames_camera_still = 0;
     col = poolptr[2560 + 0];
     for (count=0; count<6 ;count++)
      {
       for (x1=0;x1 < pocket_pods[count] -> num_planes ;x1++)
        pocket_pods[count] -> planes[x1].colour = col * 0x01010101;
	   }
	 }

/* chose selected pocket */

   if ( mouse_buttons == LEFT_BUTTON) pocket_nominated = pocket_selected;

/* don't high-light pocket if taken shot */

   if ( show_shot_flag == YES ) return;

/* high-light pocket */

	pocket_pods[pocket_selected] -> enabled = YES;

   col = poolptr[2560 + disc_col*16];
   for (x1=0;x1 < pocket_pods[pocket_selected] -> num_planes ;x1++)
    pocket_pods[pocket_selected] -> planes[x1].colour = col * 0x01010101;
}
/* ------------------------------------------------------------------------ */
void setup_nms_pocket_graphics(void)
{
OBJECT *tmp;
long x,col,leftcol,rightcol;

#define RED_POCK 4
#define YELLOW_POCK 7

   if (GAME_TYPE != NMS_POOL) return;

   if	( players[0].ball_type == NULL) return;

     if ( nms_pockets[0] == YELLOW)
      {
       rightcol = RED_POCK;
       leftcol = YELLOW_POCK;
      }
     else
      {
       leftcol = RED_POCK;
       rightcol = YELLOW_POCK;
      }

   tmp = E_GetObjectPtr( "pocket1" );
	tmp -> enabled = YES;
   col = poolptr[2560 + leftcol*16];
   for (x=0;x < tmp -> num_planes ;x++)
    tmp -> planes[x].colour = col * 0x01010101;

   tmp = E_GetObjectPtr( "pocket6" );
	tmp -> enabled = YES;
   col = poolptr[2560 + leftcol*16];
   for (x=0;x < tmp -> num_planes ;x++)
    tmp -> planes[x].colour = col * 0x01010101;

   tmp = E_GetObjectPtr( "pocket5" );
	tmp -> enabled = YES;
   col = poolptr[2560 + leftcol*16];
   for (x=0;x < tmp -> num_planes ;x++)
    tmp -> planes[x].colour = col * 0x01010101;

/* ----------------------------------------------- */

   tmp = E_GetObjectPtr( "pocket2" );
	tmp -> enabled = YES;
   col = poolptr[2560 + rightcol*16];
   for (x=0;x < tmp -> num_planes ;x++)
    tmp -> planes[x].colour = col * 0x01010101;

   tmp = E_GetObjectPtr( "pocket3" );
	tmp -> enabled = YES;
   col = poolptr[2560 + rightcol*16];
   for (x=0;x < tmp -> num_planes ;x++)
    tmp -> planes[x].colour = col * 0x01010101;

   tmp = E_GetObjectPtr( "pocket4" );
	tmp -> enabled = YES;
   col = poolptr[2560 + rightcol*16];
   for (x=0;x < tmp -> num_planes ;x++)
    tmp -> planes[x].colour = col * 0x01010101;
}
/* ------------------------------------------------------------------------ */
void normal_rooms(void)
{

   E_SelectObject("*");
   E_EnableObjects( 0);
   E_SelectObject("outer*");
   E_EnableObjects( 1);

	if	( TABLESIZE_TYPE == 1) {

   E_SelectObject("cornes*");
   E_EnableObjects( 1);

/* dont draw other table legs in low detail, for pool hall room */

   if (LOCATION !=POOL_HALL)
    {
     E_SelectObject("Leg0*");
     E_EnableObjects( 1);
    }
   else
    {
     E_SelectObject("UKLeg01");
     E_EnableObjects( 1);
     E_SelectObject("UKLeg02");
     E_EnableObjects( 1);
    }

   E_SelectObject("pocket*");
   E_EnableObjects( 1);
   E_SelectObject("surface");
   E_EnableObjects( 1);
   }

   else {
   E_SelectObject("top*");
   E_EnableObjects( 1);
   E_SelectObject("bott*");
   E_EnableObjects( 1);
   E_SelectObject("diam*");
   E_EnableObjects( 1);

   E_SelectObject("leg1");
   E_EnableObjects( 1);
   E_SelectObject("leg2");
   E_EnableObjects( 1);
   E_SelectObject("leg3");
   E_EnableObjects( 1);
   E_SelectObject("leg4");
   E_EnableObjects( 1);

/* dont draw other table legs in low detail, for pool hall room */

   if (LOCATION != POOL_HALL)
    {
     E_SelectObject("leg0*");
     E_EnableObjects( 1);
    }

   E_SelectObject("surface*");
   E_EnableObjects( 1);
   }

   E_SelectObject("pocket1col");
   E_EnableObjects( 0);
   E_SelectObject("pocket2col");
   E_EnableObjects( 0);
   E_SelectObject("pocket3col");
   E_EnableObjects( 0);
   E_SelectObject("pocket4col");
   E_EnableObjects( 0);
   E_SelectObject("pocket5col");
   E_EnableObjects( 0);
   E_SelectObject("pocket6col");
   E_EnableObjects( 0);
   E_SelectObject("corner*");
   E_EnableObjects( 1);
}
/* ------------------------------------------------------------------------ */
void moon_room(void)
{
   E_SelectObject("*");
   E_EnableObjects( 0);
   E_SelectObject("outer*");
   E_EnableObjects( 1);

   E_SelectObject("bumper*");
   E_EnableObjects( 1);
   E_SelectObject("central*");
   E_EnableObjects( 1);
   E_SelectObject("foot*");
   E_EnableObjects( 1);
   E_SelectObject("leg*");
   E_EnableObjects( 1);
   E_SelectObject("mainbody*");
   E_EnableObjects( 1);
   E_SelectObject("side*");
   E_EnableObjects( 1);
   E_SelectObject("surface*");
   E_EnableObjects( 1);


   E_SelectObject("pocket1col");
   E_EnableObjects( 0);
   E_SelectObject("pocket2col");
   E_EnableObjects( 0);
   E_SelectObject("pocket3col");
   E_EnableObjects( 0);
   E_SelectObject("pocket4col");
   E_EnableObjects( 0);
   E_SelectObject("pocket5col");
   E_EnableObjects( 0);
   E_SelectObject("pocket6col");
   E_EnableObjects( 0);
}
/* ------------------------------------------------------------------------ */
void beach_room(void)
{
   E_SelectObject("outer*");
   E_EnableObjects( 1);

   E_SelectObject("bumper*");
   E_EnableObjects( 1);
   E_SelectObject("surface*");
   E_EnableObjects( 1);
   E_SelectObject("side*");
   E_EnableObjects( 1);
   E_SelectObject("pockets*");
   E_EnableObjects( 1);
   E_SelectObject("bucketbit*");
   E_EnableObjects( 1);
}
/* ------------------------------------------------------------------------ */
void toy_room(void)
{
   E_SelectObject("endleft*");
   E_EnableObjects( 1);
   E_SelectObject("endright*");
   E_EnableObjects( 1);
   E_SelectObject("endtop*");
   E_EnableObjects( 1);
   E_SelectObject("endbottom*");
   E_EnableObjects( 1);
   E_SelectObject("tabletop*");
   E_EnableObjects( 1);
   E_SelectObject("bumper*");
   E_EnableObjects( 1);
   E_SelectObject("pocket*");
   E_EnableObjects( 1);
   E_SelectObject("corner*");
   E_EnableObjects( 1);
   E_SelectObject("surface*");
   E_EnableObjects( 1);
   E_SelectObject("outer*");
   E_EnableObjects( 1);
}
/* ------------------------------------------------------------------------ */
void coffin_room(void)
{
   E_SelectObject("*");
   E_EnableObjects( 1);

   E_SelectObject("torch*");
   E_EnableObjects( 0);
   E_SelectObject("wall*");
   E_EnableObjects( 0);
   E_SelectObject("ceiling*");
   E_EnableObjects( 0);
   E_SelectObject("floor*");
   E_EnableObjects( 0);

   E_SelectObject("surface*");
   E_EnableObjects( 1);
   E_SelectObject("pocket1");
   E_EnableObjects( 0);
   E_SelectObject("pocket2");
   E_EnableObjects( 0);
   E_SelectObject("pocket3");
   E_EnableObjects( 0);
   E_SelectObject("pocket4");
   E_EnableObjects( 0);
   E_SelectObject("pocket5");
   E_EnableObjects( 0);
   E_SelectObject("pocket6");
   E_EnableObjects( 0);
}
/* ------------------------------------------------------------------------ */
void   Select_Pocket_And_Ball(void)
{
static	long	a,new_flash,flash,loop;
static  long save_frames;
   long speed;

   speed=frames-save_frames;
   save_frames = frames;

   if ( (show_shot_flag==YES) || (GAME_TYPE!=CONTINUOUS) ) return;

/* change ball */

       a= -1;

      if	( getkey_jp(kN)==YES ) 
       {
        ball_to_pocket++;
        ball_to_pocket%=LAST_BALL;

        while (ball[ball_to_pocket].active==NO)
         {
          ball_to_pocket++;
          ball_to_pocket%=LAST_BALL;
			}
       }

/* dont allow player to select cue ball */
      if (ball_to_pocket==0) ball_to_pocket++;

/* flash selected ball for a few frames */

		if	 ( (a!=ball_to_pocket) && (flash_ball1<=0) )
       {
		  flash_ball1=126; /* setup number of frames to flash ball */
		 }

	   for (loop=0;loop<LAST_BALL;loop++) {
        if (ball[loop].active==NO) ball[ loop ].active=1;
		 }

		if	( flash_ball1 > 0 ) 
       {
        if ( flash_ball1%10 < 4)    flash^=1;

		  ball[ ball_to_pocket ].active=flash;
		  flash_ball1-=speed;

		  if	( flash_ball1 <=0 ) 
         {
          ball[ball_to_pocket].active=1;
         }
       }

   num_frames_camera_still = 0;
}
/* ------------------------------------------------------------------------ */
void load_3d_textures(void)
{
   char file[80];
	long	room;

	if ( LOCATION <=DIFFERENT_SHAPED_TABLES_INDEX-1) room=(LOCATION*15)+((TABLESIZE_TYPE)*5);
   else room = 15*DIFFERENT_SHAPED_TABLES_INDEX + (LOCATION-DIFFERENT_SHAPED_TABLES_INDEX)*5;

/* load in texture bank */

   sprintf( file,"%s",mesh_table[room+4] );

	LoadTextures( file );

   sprintf( file,"%s//%s","balls",mesh_table[room+4] );
	LoadBallTextures(file);

   E_SetTexture(textures);
}
/* ------------------------------------------------------------------------ */
void reset_tracking_lines(void)
{
 long count;

   if ( activated_lines == TRUE ) 
    {

     for (count=0;count<LAST_BALL;count++) 
      {
       line_pointer_index[count]=0;
       ball[count]=saveball_lines[count];
      }

     activated_lines=FALSE;
     reset_fouls();
    }
}
/* ------------------------------------------------------------------------ */
void setup_default_camera_position( int ease_speed_value, flag point_at_cue_ball)
{
VERTEX	*table_ptr;
   long room,loop;
/* setup default camera position */

	 for (loop=0;loop<4;loop++)
	  if ( sidebumper_ptr[loop]) sidebumper_ptr[loop]-> enabled=1;


	if ( LOCATION <= DIFFERENT_SHAPED_TABLES_INDEX-1) room=(LOCATION*3)+((TABLESIZE_TYPE));
   else room = SPOT_ROOM(LOCATION);	// RESTORE: was DIFFERENT_SHAPED_TABLES_INDEX*3 + (LOCATION-DIFFERENT_SHAPED_TABLES_INDEX), see GLOBALS.H SPOT_ROOM

   if ( SNOOKER_MODE == YES ) room=4*3;

   if	( TABLESIZE_TYPE != 0) 
    {
     table_ptr = special_camera_positions;
 	 }
   else
    {
     table_ptr = special_us_camera_positions;
    }

   lined_up_shot=YES;
   camera_mode=ZOOM_BACK_TO_CUE_BALL;

/* setup default position for camera */

   camera_rot = table_ptr[12];
   camera_target = table_ptr[13];

	switch	( TABLESIZE_TYPE ) 
    {
     case US:
     camz = 3504;

/* setup camera zoom for snooker table */
     if ( SNOOKER_MODE == YES )   camz = 4752;
     camera_rot.x = 256;
	  break;

     case UK:
     camz = 2468;
     camera_rot.x = 234;
	  break;
    }

   if ( point_at_cue_ball == YES )
    {

/* point camera at cue ball */
     camera_target.x= FIX24_TO_WORLD(ball[0].x);//whiteball_spot_positions[room] *256000;	// RESTORE: 8.24
    }
	else
    {

/* point camera at default camera position */
     if ( SNOOKER_MODE == NO )
      {
       camera_target.x= FIX24_TO_WORLD(whiteball_spot_positions[room]);	// RESTORE: 8.24
		}
     else
      {
/* point at special snooker position, as it is not stored in spot position 
table */
       camera_target.x=-FIX24_TO_WORLD(FIX24(1.045)+ball[0].diameter);	// RESTORE: 8.24
      }

    }
/* setup cue ball position across table */

   camera_target.y= FIX24_TO_WORLD(ball[WHITE_BALL].radius);	// RESTORE: 8.24
   camera_target.z= 0*256000;

/* move camera smoothly to this position */

   E_EaseCamera(0);
   E_PositionCamera( &camera_target, &camera_rot, camz*256 );
   E_UpdateCamera();
   E_UpdateCamera();

/* make cue point along cameras field of view */

   cue_rot = camera_rot;
   E_EaseCamera(ease_speed_value);
}
// ----------------------------------------------------------------------------
void	setup_draw_3d_cue(void)
{
	int loop,a;
	// We must make sure that all the various buffers contain the right images
   // so the first couple of still frames are spent rendering here and there

   setup_detail_level_of_room();
	for	(loop=0;loop<16;loop++)	ball_ptr[loop] -> enabled=0;
	for	(loop=8;loop<16;loop++)	stripe_ptr[loop] -> enabled=0;

/* turn cue off */

	cue_ptr -> enabled =NO;

/* render all objects in room */

   if (LOCATION <= DIFFERENT_SHAPED_TABLES_INDEX-1)
    {
     CalcBumperVisibility();
     for (a=0;a<6;a++) if(bumper_ptr[a]) bumper_ptr[a] -> enabled =0;
     for (a=0;a<6;a++) if(sidebumper_ptr[a]) sidebumper_ptr[a] -> enabled =0;
	 }

	if (RESOLUTION==HIGH)
   {
     Draw_Head_String( backscreen_buffer );

     if (activated_lines == TRUE)
      draw_tracking_lines(backscreen_buffer);

     E_RenderToScreen(1,0);

     E_Reset2DLines();

	} else {

     Draw_Head_String( backscreen_buffer );

     if (activated_lines == TRUE)
      draw_tracking_lines(backscreen_buffer);

     E_RenderToBuffer( backscreen_buffer, 1,0);

     E_Reset2DLines();
	}

/* de-select all bumpers */

   E_SelectObject("*");
	E_EnableObjects(0);

	turn_on_far_bumpers();

	if (RESOLUTION==HIGH)
    {
     E_RenderToScreen(0 ,0);
	 }
   else 
    {
     E_RenderToBuffer( backscreen_buffer, 0 , 0);
    }

   E_SelectObject("*");
	E_EnableObjects(0);

/* render balls to screen + stripes+spots */

   DrawAllBalls(1);

/* render bumpers correctly */

   E_SelectObject("*");
	E_EnableObjects(0);

	turn_on_near_bumpers();
   turn_on_diamonds(0);

	if (RESOLUTION==HIGH)
    {
     E_RenderToScreen(0 ,0);
	 }
   else 
    {
     E_RenderToBuffer( backscreen_buffer, 0 ,0);
	 }

	SetupPanel();
   memcpy( backscreen_buffer1, backscreen_buffer, 64000);

}
// ----------------------------------------------------------------------------
long cuepos=0;
double cue_tab[] = { 1,2,4,6,10,13,16,18,22,18,16,13,10,6,4,2,0 };
void	draw_3d_cue(void)
{
	//cue_rot.y = cue_angle;

   cue_position = (cue_tab[cuepos])*0.008;

   if ( cue_position == 0 ) cuepos=0;
   cuepos++;

	if (RESOLUTION==HIGH)
   {
    E_SetZoneBank( interlace );
	}

   E_PositionCamera( &camera_target, &camera_rot, camz*256 );
   E_UpdateCamera();

   E_SelectObject("*");
	E_EnableObjects(0);
   E_RenderToBuffer( backscreen_buffer, 1,0);

   camera_mode = ZOOM_BACK_TO_CUE_BALL;

	PositionCue();

/* ------------------------------------ */
/* render just balls onto static screen */
   memcpy( backscreen_buffer, backscreen_buffer1, 64000);

	cue_ptr -> enabled =1;
//   E_WorldRotateObject( cue_ptr );

	if (RESOLUTION==HIGH)
	{
    E_RenderToScreen(0,0);
	} else {
     E_RenderToBuffer( backscreen_buffer, 1,0);
	}

	if	(RESOLUTION==HIGH)
    {
     interlace+=1;
     interlace%=2;
     setup_SVGA_screen();
    }
	else
    {
//     wait_for_VBE_vblank(0);
     memcpy( (char *) 0xa0000, backscreen_buffer, 64000);
    }
}
// ----------------------------------------------------------------------------
void	Move_Camera_View_Point(void)
{
int  xDelta=0;
int  yDelta=0;
VERTEX temp_vertex;
static flag old_cross;

/* check to see if drawing camera view point */

  if ( event_flag == MOVE_CAMERA )  {
    old_cross = YES;
    num_frames_camera_still = 0;
	}
  else
   {
    if (old_cross == NO) return;
    num_frames_camera_still = 0;
    old_cross = NO;
    return;
   }

	// Make translation vector for un-rotated camera

   temp_vertex.x = mouse_xdiff * 256;
   temp_vertex.z =- mouse_ydiff * 256;
   temp_vertex.y = 0;

	// Rotate translation vector by camera's y rotation

   E_RotateVertex( 0, camera_rot.y, 0, &temp_vertex);

	// change into doubles and scale down to meters

   xDelta = ( temp_vertex.x) ;// 256000;
   yDelta = ( temp_vertex.z) ;// 256000;

	camera_target.y=.012 * 256000;
   camera_target.x +=xDelta;
   camera_target.z +=yDelta;

/* check bumper boundary's */

	if	( FIX24_TO_WORLD(max_cush_x4)-(camera_target.x) <0.0) camera_target.x =FIX24_TO_WORLD(max_cush_x4);	// RESTORE: 8.24
	if	( FIX24_TO_WORLD(min_cush_x1)-(camera_target.x) >0.0) camera_target.x =FIX24_TO_WORLD(min_cush_x1);	// RESTORE: 8.24
	if	( FIX24_TO_WORLD(max_cush_y2)-(camera_target.z) <0.0) camera_target.z =FIX24_TO_WORLD(max_cush_y2);	// RESTORE: 8.24
	if	( FIX24_TO_WORLD(min_cush_y3)-(camera_target.z) >0.0) camera_target.z =FIX24_TO_WORLD(min_cush_y3);	// RESTORE: 8.24

//   E_MoveObject( (int)(camera_target.x), (int)camera_target.y, (int)(camera_target.z), cross_ptr );
}
// ----------------------------------------------------------------------------
void	Draw_Camera_View_Point( char *buffer )
{
#define CROSS_SIZE 8 /* in pixels */

/* only draw cross if player holding down the 'V' key */

  if ( event_flag == MOVE_CAMERA )
   {
    line3 -> enabled = 1;
    line4 -> enabled = 1;
	}
  else
   {
    line3 -> enabled = 0;
    line4 -> enabled = 0;
    return;
	}

	// Rotate translation vector by camera's y rotation

   E_MoveObject( (int)(camera_target.x) , (int)camera_target.y, (int)(camera_target.z), line3 );
   E_MoveObject( (int)(camera_target.x) , (int)camera_target.y, (int)(camera_target.z), line4 );
   E_WorldRotateObject(line3);
   E_WorldRotateObject(line4);

   if ( RESOLUTION == LOW)
     Draw_Line_cross( buffer , 8 , line3 , line4 ,CROSS_SIZE,0);
   else
     Draw_Line_cross( buffer , 8 , line3 , line4 ,CROSS_SIZE*1.5,0);

   E_MoveObject( (int)(camera_target.x) , (int)camera_target.y, (int)(camera_target.z), line3 );
   E_MoveObject( (int)(camera_target.x) , (int)camera_target.y, (int)(camera_target.z), line4 );
   E_WorldRotateObject(line3);
   E_WorldRotateObject(line4);
   if ( RESOLUTION == LOW)
     Draw_Line_cross( buffer , 8 , line3 , line4, 0 , CROSS_SIZE );
	else
     Draw_Line_cross( buffer , 8 , line3 , line4, 0 , CROSS_SIZE*1.5 );

} 
// ----------------------------------------------------------------------------
void check_change_resolution(void)
{
/* swap resolution flag */

   if ( end_of_frame_flag==YES) return;

   RESOLUTION^=1;

/* re-build in game font tables */

   BUILD_HIRES_TEXT=YES;
   BUILD_HIRES_NUMS=YES;
   BUILD_LOW_TEXT=YES;
   BUILD_LOW_NUMS=YES;
   NUMBER_OF_LOCATIONS=11;	// RESTORE: was 10, see GAME.C
   BUILD_LOW_TEXT_FOUL = YES;
   BUILD_HIRES_TEXT_FOUL = YES;
   draw_twice=2;

/* setup screen sizes */

	if	(RESOLUTION==HIGH)	
    {
     viewport_width=SVGA_logical_length; /* setup x resolution */
	  frame_rate=60.0;	 /* setup frame rate for ball machanics */
     height = 480-(CLIPPED*2);
    }
	else	
    {
     viewport_width=320;	/* setup x resolution */
	  frame_rate=70.0;      /* setup frame rate for ball machanics */
     height = 200;
	 }

   load_all_game_graphics_from_disk();

   setup_resolution();

/* load all game graphics into ram */

	fix_palette_to_black();

	if	(RESOLUTION==LOW)	
    {
/* setup 320x200 screen mode for lores game play */
     setvgamode(0x13);
    }
   else 
    {
/* setup 640x480 mode */
     SVGA_set_mode(0x101);
     setup_SVGA_screen();
    }

   clear_replay_panel_count=4;
	num_frames_camera_still = 0;

	HI_CLS();
	Render();
	if ( RESOLUTION == HIGH ) Render();
   setpallette ( palette,256 );
   save_resolution=RESOLUTION;
}
// ----------------------------------------------------------------------------
void LoadTextures2( text *path)
{
char *filename = "txtbnkXX.raw";
int i;
char *load_address;
	char	file[80];
	
  load_address = textures;

  for (i = 0; i < 31; i++)
  {
    filename[6] = 48 + (i / 10);
    filename[7] = 48 + (i % 10);
    sprintf( file, "%s//%s//%s",drive_and_path,path,filename );

//    reply = _dos_findfirst( file , _A_SUBDIR, &fileinfo);

  //  if (reply == 0)
  //  {
      if (file) Load( file, load_address, 65536);
  //  }

    load_address += 65536;
  }
}
OBJECT *tv_ptr;

// ----------------------------------------------------------------------------
void	init_3d_for_tournament(void)
{
	int   vesa_shift_amount;
   int   vesa_granularity;
	text	file[80];

   In_Game_Ram_Init();

   changed_view=NO;

/* point backscreen uffer at front end ram */

   oldnumber_res = 255;
   ball_to_move=0;
	ease_speed=8;

/* get pointer to level */

	E_Init( steves_memory, 1448576 );

/* load in mesh */

   sprintf( file, "%s//%s",drive_and_path,"tvtots\\tv.esh");
   E_AddMesh( file );
	E_Load();
	E_LoadMapData();

   E_SetTexture(textures);
   LoadTextures2( "tvtots" );

   poolptr = E_PoolTabPtr();
  
/*	in taken shot then turn cue off*/

   E_Reset2DLines();

/* turn on or off felt+floor textures */

   E_EnableFelt( 0 );

	vesa_shift_amount = 0;
   vesa_granularity = VESA_wingran;

	while (vesa_granularity < 64)
   {
     vesa_granularity = vesa_granularity << 1;
     vesa_shift_amount++;
	}

	E_SetVesaShift( vesa_shift_amount);
	zoom=928+500;

  	E_SetScreenSize(640,480);
   E_SetScanlineLength(SVGA_logical_length);
  	E_SetScreenCentre(320,200);
  	E_SetPixelAspect( 65536 );
   pixel_aspect  = 65536;
  	E_SetZoom((zoom*65536)/320);
}
// ----------------------------------------------------------------------------
void	init_3d_for_copyright(void)
{
	int   vesa_shift_amount;
   int   vesa_granularity;
	text	file[80];

   In_Game_Ram_Init();

   changed_view=NO;

/* point backscreen uffer at front end ram */

   oldnumber_res = 255;
   ball_to_move=0;
	ease_speed=8;

/* get pointer to level */

	E_Init( steves_memory, 1448576 );

/* load in mesh */

   sprintf( file, "%s//%s",drive_and_path,"right\\cuedup.esh");
   E_AddMesh( file );
	E_Load();
	E_LoadMapData();

   E_SetTexture(textures);
   LoadTextures2( "right" );

   poolptr = E_PoolTabPtr();
  
/*	in taken shot then turn cue off*/

   E_Reset2DLines();

/* turn on or off felt+floor textures */

   E_EnableFelt( 0 );

	vesa_shift_amount = 0;
   vesa_granularity = VESA_wingran;

	while (vesa_granularity < 64)
   {
     vesa_granularity = vesa_granularity << 1;
     vesa_shift_amount++;
	}

	E_SetVesaShift( vesa_shift_amount);

	zoom=2200;

  	E_SetScreenSize(640,480);
   E_SetScanlineLength(SVGA_logical_length);
  	E_SetScreenCentre(320,220);
  	E_SetPixelAspect( 65536 );
   pixel_aspect  = 65536;
  	E_SetZoom((zoom*65536)/320);
}
// ----------------------------------------------------------------------------
   int zangle,zadd;

void	draw_3d_for_tournament(void)
{

#define GRAVITY 64

    camera_target.x=0;
    camera_target.y=0;
    camera_target.z=0;

    camera_rot.x=0;
    camera_rot.z=0;

    camz=7900*256;

/* make tv move in and out of screen */

    zangle+=6;
    zangle%=360;

    zadd = (int)(sin(zangle*deg2rad)*10000);
    camz+=(zadd*128);

    camera_rot.y+=100;
    camera_rot.y&=4095;

    E_SelectObject("*");
    E_EnableObjects(0);
    E_SelectObject("TV");
    E_EnableObjects( 1);

    E_EaseCamera(0);
    E_PositionCamera( &camera_target, &camera_rot, camz );


    E_UpdateCamera();

    E_RenderToScreen(1,0);
}
// ----------------------------------------------------------------------------
void	draw_3d_for_copyright(void)
{

#define GRAVITY 64

    camera_target.x=0;
    camera_target.y=0;
    camera_target.z=0;

    camera_rot.x=100;
    camera_rot.z=0;

    camz=3000*256;

/* make tv move in and out of screen */

    E_SelectObject("*");
    E_EnableObjects(0);
    E_SelectObject("outer*");
    E_EnableObjects(0);

    E_SelectObject("cuedup");
    E_EnableObjects(1);

    E_EaseCamera(0);
    E_PositionCamera( &camera_target, &camera_rot, camz );

    E_UpdateCamera();

    E_RenderToScreen(1,0);
}
// ----------------------------------------------------------------------------
void	load_all_game_graphics_from_disk(void)
{
   long room;
	char	buff[768];
	text	file[80];

	memcpy(&buff,&palette,768); 

	foul=0;

   In_Game_Ram_Init();

   txt_foul = foul_data;
   speedtxt = ball_data;

/* open level palette */

   room = (TABLE_TYPE*NUMBER_OF_LOCATIONS)+LOCATION;

   sprintf( file, "%s//%s",drive_and_path,room_palettes[room] );
	load_file_here(file, &palette, 768 );

/* load all game graphics in low or high res */

	if	(RESOLUTION==LOW) 	
    {
     resfile=RES_open("bgd1.res");
     RES_load_file_here("bgd\\tmp\\ampliflo.bgd",amplify_icon,0);

     if ( SNOOKER_MODE == NO )
      {
       RES_load_file_here("bgd\\tmp\\loballs.bgd",ball_data,0);
      }
     else
      {
       RES_load_file_here("bgd\\tmp\\snalo.bgd",ball_data,0);
		}

     RES_load_file_here("bgd\\tmp\\whitelo.bgd",shots_icon,0);
     RES_close(resfile );

     resfile=RES_open("rws.res");
  
/* ---------------------------------- */
/* remap all raw file types * to game palette */

     RES_load_file_here("raw\\foulfon2.pal", remap_palette, 0 );
     RES_load_file_here("raw\\foulfon2.raw", txt_foul ,0 );
     remap_raw_block(0,remap_palette, txt_foul ,640,80);

/* --------------------- */

     if	(GAME_TYPE == SPEED_POOL)
      {
       RES_load_file_here("raw\\speedlo.raw",speedtxt,0);
	   }

       RES_load_file_here("raw\\loresfnt.raw",txt,0);

     RES_load_file_here("raw\\loresfnt.pal", remap_palette, 0 );
     remap_raw_block(0,remap_palette, txt ,640,80);

     RES_close(resfile );

     resfile=RES_open("bgd.res");
//   RES_load_file_here("bgd\\connctlo.bgd",connect_data,0);

     if (GAME_TYPE == KILLER_POOL)
      RES_load_file_here("bgd\\killerlo.bgd",killer_gfx,0);
 
     RES_close(resfile );
    }
	else 
    {
/* load all in game graphics in hi-res */

     resfile=RES_open("bgd1.res");
     RES_load_file_here("bgd\\tmp\\amplify.bgd",amplify_icon,0);
     RES_load_file_here("bgd\\tmp\\whitehi.bgd",shots_icon,0);

     if	(GAME_TYPE != SPEED_POOL)
      {
       if ( SNOOKER_MODE == NO )
        {
         RES_load_file_here("bgd\\tmp\\hiballs.bgd",ball_data,0);
        }
       else
        {
         RES_load_file_here("bgd\\tmp\\snahi.bgd",ball_data,0);
		  }
		}
     RES_close(resfile );

     resfile=RES_open("rws.res");

     RES_load_file_here("raw\\foulfnt.pal",remap_palette ,0 );
     RES_load_file_here("raw\\foulfnt.raw",txt_foul,0);
     remap_raw_block(0,remap_palette, txt_foul ,640,80);

     if	(GAME_TYPE == SPEED_POOL)
      {
       RES_load_file_here("raw\\speedhi.raw",speedtxt,0);
	   }

     RES_load_file_here("raw\\hiresfnt.raw",txt,0);

     RES_load_file_here("raw\\hiresfnt.pal", remap_palette, 0 );
     remap_raw_block(0,remap_palette, txt ,640,280);

     RES_close(resfile );

     resfile=RES_open("bgd.res");
//     RES_load_file_here("bgd\\connctlo.bgd",connect_data,0);

     if ( GAME_TYPE== KILLER_POOL)
       RES_load_file_here("bgd\\killerhi.bgd",killer_gfx,0);

     RES_close(resfile );
	 }


/* remap all bgd data types to this level palette */
	resfile=RES_open("bgd1.res");   

	if (RESOLUTION==LOW) {
/* remap number of shots icon */


   if  ( GAME_TYPE == SPEED_POOL)
    remap_raw_block(0,remap_palette, speedtxt ,640,280);

     palette[0]=0;
     palette[1]=0;
     palette[2]=0;
     RES_load_file_here("bgd\\tmp\\whitelo.pal", remap_palette, 768 );
     remap_gfx_block(0,remap_palette, shots_icon );

     if ( SNOOKER_MODE == NO )
      {
       if  ( GAME_TYPE != SPEED_POOL)
        {
         RES_load_file_here("bgd\\tmp\\loballs.pal", remap_palette, 768 );
         remap_gfx_block(20,remap_palette, ball_data);
        }
      }
     else
      {
       RES_load_file_here("bgd\\tmp\\snalo.pal", remap_palette, 768 );
       remap_gfx_block(7,remap_palette, ball_data);
      }


	  RES_load_file_here("bgd\\tmp\\ampliflo.pal", remap_palette, 768 );
     palette[0]=0x3f;
     palette[2]=0x3f;
     remap_gfx_block(0,remap_palette, amplify_icon);
     palette[0]=0;
     palette[2]=0;
    }
	else {

   if  ( GAME_TYPE == SPEED_POOL)
    {
     remap_raw_block(0,remap_palette, speedtxt ,640,280);
	 }
/* remap number of shots icon */

     palette[0]=0;
     palette[1]=0;
     palette[2]=0;
     RES_load_file_here("bgd\\tmp\\whitehi.pal", remap_palette, 768 );
     remap_gfx_block(0,remap_palette, shots_icon );

     if ( SNOOKER_MODE == NO )
      {
       if ( GAME_TYPE !=SPEED_POOL)
        {
         RES_load_file_here("bgd\\tmp\\hiballs.pal", remap_palette, 768 );
         remap_gfx_block(20,remap_palette, ball_data);
        }
      }	 
     else
      {
       RES_load_file_here("bgd\\tmp\\snahi.pal", remap_palette, 768 );
       remap_gfx_block(7,remap_palette, ball_data);
      }	 

	  RES_load_file_here("bgd\\tmp\\amplify.pal", remap_palette, 768 );
     palette[0]=0x3f;
     palette[2]=0x3f;
     remap_gfx_block(0,remap_palette, amplify_icon);
     palette[0]=0;
     palette[2]=0;
	 }
	RES_close(resfile );

/* ---------------------------------- */

	resfile=RES_open("bgd.res");   

	RES_load_file_here("bgd\\grenbais.pal", remap_palette, 768 );
//	remap_gfx_block(0,remap_palette, connect_data );

	if ( GAME_TYPE == KILLER_POOL) 
     remap_gfx_block(2,remap_palette, killer_gfx );

/* ---------------------------------- */

	RES_close(resfile );

/* remap palette back to original palette */

	memcpy(&palette,&buff,768); 
}

// ----------------------------------------------------------------------------
void setup_vesa(void)
{
	int   vesa_shift_amount;
   int   vesa_granularity;

	vesa_shift_amount = 0;
   vesa_granularity = VESA_wingran;

	while (vesa_granularity < 64)
   {
     vesa_granularity = vesa_granularity << 1;
     vesa_shift_amount++;
	}

	E_SetVesaShift( vesa_shift_amount);
}
// ----------------------------------------------------------------------------
VERTEX camera_positions[] = {
    {361 , 545 , 0 },
    {-50898 , 3072 , -87786},
    {4188,0,0} ,
    {430 , 3659 , 0},
    {103201 , 3072 , -87823 },
    {4188,0,0 },
    {453 , 2665 , 0 },
    {116728 , 3072 , 87237 },
    { 4188 ,0,0} ,
    {523 , 1710 , 0 },
    {-101875 , 3072 , 87989 },
    {4188 ,0,0},
    {758 , 2052 , 0 },
    {-1241 , 3072 , 87989 },
    {3502 ,0,0},
    {848 , 4094 , 0 },
    {5549 , 3072 , -88014 },
    {3502 ,0,0},
    {423 , 1033 , 0 },
    {-169322 , 3072 , -5148 },
    {2539 ,0,0},
    {419 , 3076 , 0 },
    {184830 , 3072 , 10373 },
    {2539 ,0,0},
    {523 , 1710 , 0 },
    {-101875 , 3072 , 87989 },
    {4188 ,0,0},
    {361 , 545 , 0 },
    {-50898 , 3072 , -87786},
    {4188,0,0} ,
};
// ----------------------------------------------------------------------------
VERTEX camera_positions_us[] = {
{486 , 693 , 0 },
{-217101 , 3072 , -76264 },
{3983 ,0,0},
{486 , 693 , 0 },
{-217101 , 3072 , -76264 },
{3983 ,0,0},
{426 , 3074 , 0 },
{262549 , 3072 , -10095 },
{3194 ,0,0},
{230 , 1262 , 0 },
{-282278 , 3072 , 124835 },
{1962 ,0,0},
{591 , 4089 , 0 },
{-5403 , 3072 , -136516 },
{4993 ,0,0},
{638 , 2046 , 0 },
{4754 , 3072 , 57817 },
{4993 ,0,0},
{606 , 2614 , 0 },
{189809 , 3072 , 147094 },
{4999 ,0,0},
{315 , 3433 , 0 },
{179898 , 3072 , -48849 },
{3895 ,0,0},
{158 , 2596 , 0 },
{287381 , 3072 , 134140 },
{866 ,0,0},
{219 , 3591 , 0 },
{287362 , 3072 , -113929 },
{1327 ,0,0}
};
// ----------------------------------------------------------------------------
/* cnooker table camers shots */

VERTEX camera_positions_snooker[] = {
{290 , 1041 , 0 },
{-357682 , 3072 , -1460 },
{3766 ,0,0},
{325 , 1410 , 0 },
{-437231 , 3072 , 212096 },
{4652 ,0,0},
{393 , 662 , 0 },
{-423224 , 3072 , -200226 },
{4925 ,0,0 },
{485 , 1 , 0 },
{-8375 , 3072 , -212472 },
{6883 ,0,0},
{385 , 3079 , 0 },
{371881 , 3072 , 2543 },
{3795 ,0,0},
{527 , 2042 , 0 },
{-1570 , 3072 , 212096 },
{6568 ,0,0},
{366 , 2656 , 0 },
{311347 , 3072 , 136059 },
{6568 ,0,0},
{485 , 1 , 0 },
{-8375 , 3072 , -212472 },
{6883 ,0,0},
{325 , 1410 , 0 },
{-437231 , 3072 , 212096 },
{4652 ,0,0},
};
// ----------------------------------------------------------------------------
//coffin table camera position's 
VERTEX camera_positions_coffin[] = {
{339 , 1006 , 0 },
{-427165 , 3072 , 6398 },
{5410 ,0,0 },
{ 234 , 3239 , 0 },
{486227 , 3072 , -130569 },
{5164 ,0,0 },
{312 , 2962 , 0 },
{377552 , 3072 , 130569 },
{5164 ,0,0},
{384 , 3103 , 0 },
{486227 , 3072 , -64696 },
{6924 ,0,0},
{993 , 26 , 0 },
{-47081 , 3072 , -130569 },
{8002 ,0,0},
{384 , 3103 , 0 },
{486227 , 3072 , -64696 },
{6924 ,0,0},
{384 , 3103 , 0 },
{486227 , 3072 , -64696 },
{6924 ,0,0},
{ 234 , 3239 , 0 },
{486227 , 3072 , -130569 },
{5164 ,0,0 },
};


long next_cam;
void setup_end_of_frame_camera(void)
{
VERTEX	*table_ptr;
long room;

/* setup default camera position */

   next_cam++;
   next_cam%=7;

   if ( LOCATION == 7)
    {
     table_ptr = camera_positions_coffin;
    }
   else
    {
     if ( SNOOKER_MODE == YES )
      {
       table_ptr = camera_positions_snooker;
	   }
     else
      {
       if	( TABLESIZE_TYPE == UK) 
        {
         table_ptr = camera_positions;
        }
       else
        {
         table_ptr = camera_positions_us;
        }
	   }
	 }

   lined_up_shot=YES;
   camera_mode=ZOOM_BACK_TO_CUE_BALL;

/* setup default position for camera */

   camera_rot = table_ptr[next_cam*3];
   camera_target = table_ptr[ (next_cam*3)+1];

   if ( TABLESIZE_TYPE == UK )
    {
     camz = table_ptr[(next_cam*3)+2].x-1000;
	 }
   else
    {
     camz = table_ptr[(next_cam*3)+2].x-400;

     if ( RESOLUTION == HIGH)
      {
       camz = table_ptr[(next_cam*3)+2].x-400;
      }
	 }

   E_EaseCamera(ease_speed);
   E_PositionCamera( &camera_target, &camera_rot, camz*256 );
   while ( (E_FinishedEase() == NO) )
    {
 	  Handle3D();
	 }
}
// ----------------------------------------------------------------------------
void check_camera_limets(void)
{
/* limet y rotation */

     if (camera_rot.x <= LOWEST_CAMERA_ANGLE)   
      {
       camera_rot.x = LOWEST_CAMERA_ANGLE;
      } 

     if (camera_rot.x >= 1024)   
      {
       camera_rot.x = 1024;
      } 

     camera_rot.x &=4095;
         
     if ((camera_rot.x > 1024) && (camera_rot.x < 2048))
      {
       camera_rot.x = 1024;
      }

     if ((camera_rot.x > 2048) && (camera_rot.x < 3072))
      {
       camera_rot.x = 3072;
      } 

/* check limets on x */

     camera_rot.y&=4095;

}
// ----------------------------------------------------------------------------
long	conversion[] ={  1,3,4,5,12,6,2,8,7,11,9,13,10,15,14,16 };
void  load_characters(void)
{

char *filename = "charXX.raw";
int i,start,left,right;
char *load_address;
char path[80];
	char	file[80];

/* setup location data */

  switch (LOCATION)
  {
   case POTTERS_CLUB:
   load_address = textures+(65536*8);
   strcpy(&path,"POTMEN");
   start=8;
   break;

   case RACKERS:
   load_address = textures+(65536*15);
   strcpy(&path,"CLUBMEN");
   start=15;
   break;

   case POOL_HALL:
   load_address = textures+(65536*14);
   strcpy(&path,"HALLMEN");
   start=14;
   break;


   case ARENA:
   load_address = textures+(65536*11);
   strcpy(&path,"GEMMEN");
   start=11;
   break;

   default:
   return;
  }

   left = conversion[left_player];
   right = conversion[right_player%16];

   filename[4] = 48 + (left / 10);
   filename[5] = 48 + (left % 10);

   sprintf( file, "%s//%s//%s",drive_and_path,path,filename );
   if (file) Load( file, load_address, 65536);
   load_address += 65536;

   filename[4] = 48 + (right / 10);
   filename[5] = 48 + (right % 10);
   sprintf( file, "%s//%s//%s",drive_and_path,path,filename );
   if (file) Load( file, load_address, 65536);
   load_address += 65536;

}

