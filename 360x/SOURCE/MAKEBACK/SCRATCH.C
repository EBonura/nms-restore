/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Structures */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Crashed in burst mode and scroll mode by pressing Z over a large distance
// Don't exactly know why though
// Could do with 2 sprite lists...
// Or one massive one...
// Chicken & egg again...
// Development cycle...
// Sprites drawn...
// Sprites grabbed...
// Put down on map...
// Given to me...
// Implemented...
// Final tweaking...
//
// Should be a module...
// Sprite system for none monster sprites to make my life easy
// To do:-
// Properties list for all sprite types
// Hit Points...
// Shootable?
// Seperate Animation Processor
// Where to go now:-
// Command Line options in grabber:-
//
// Grab with origin at middle of first frame
// Grab with origin at middle of all frames
// Grab with origin at bottom of all frames
// Grab with origin at bottom of first frame
// Grab with origin at left of all frames
// Grab with origin at left of first frame
//
// Put explosion in right place...
// Different explosion sequences...
// Map Editor...


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Other */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long bounce_x[8]={1,1,-1,-1, -1,-1, 1,1 };
long bounce_y[8]={1,-1,1,-1, 1, 1, -1,1 };

long org_list[]={
	NAME_acidspt,
	NAME_l1c5,
	NAME_END,
};

// explosion type might be good here...
// hit points etc...

		
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Variables */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Level Data */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Load SPRITE LIST */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Collided with craft? */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Pretty slow compiled

flag collided_with_craft(int name,int x,int y,int frame)
{
	char *gfxdata;
	short *gfxdataw;
	int ptr;
	short width,height;

	ptr=seqdata[name].map[frame];
	gfxdata=seqdata[name].data;
	gfxdataw=(short *) (gfxdata+ptr);
	x+=(int ) gfxdataw[0];
	y+=(int ) gfxdataw[1];
	width=gfxdataw[2];
	height=gfxdataw[3];

	if( craftx>=x && craftx<(x+width)
		 && crafty>=y && crafty<(y+height) ) return TRUE;
	return FALSE;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Get angle to craft */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Not imensley good when the ship is a distance away... 

/*
void get_angle_to_craft(int i)
{
	long x1,x2,y1,y2;
	long dx,dy,hyp;
	long tx,ty;

	x1=sprites[i].x;
	y1=sprites[i].y;

	x2=craftx;
	y2=crafty;

	tx=(x2-x1)/8;
	ty=(y2-y1)/8;
	dx=tx*256;
	dy=ty*256;

	if(tx<0) tx=-tx;
	if(ty<0) ty=-ty;

	if( (tx+ty)<256 )	hyp=sqr_root( (dx*dx) + (dy*dy) );
	else hyp=0;

	if(hyp>0)
	{	
		if(dx!=0) sprites[i].cos=(dx*256)/hyp;
		else sprites[i].cos=0;
		if(dy!=0) sprites[i].sin=(dy*256)/hyp;
		else sprites[i].sin=0;
	}
	else
	{
		sprites[i].cos=0;
		sprites[i].sin=0;
	}
}
*/

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Cycle Animation */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void inc_frame_no(long name,long *frame)
{	
	long i;
	i=*frame;
	i++;
	if(seqdata[name].map[i]==-1) i=0;
	*frame=i;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Process Collisions... */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Could do with some optimisation here

/*
void process_collisions()
{
	char *gfxdata;
	short *gfxdataw;
	int ptr;
	short width,height;
	int i,j,x,y;
	int name,frame;
	
	for(i=0;i<MAX_SPRITES;i++)
 	{
		if( (sprites[i].on==TRUE) && (sprites[i].shootable==YES) )
		{		
			name=sprites[i].name;
			x=sprites[i].x;
			y=sprites[i].y;
			frame=sprites[i].frame;
	
			ptr=seqdata[name].map[frame];
			gfxdata=seqdata[name].data;
			gfxdataw=(short *) (gfxdata+ptr);
			x+=(int ) gfxdataw[0];
			y+=(int ) gfxdataw[1];
			width=gfxdataw[2];
			height=gfxdataw[3];

			for(j=0;j<MAX_FIRE;j++)
			{
				if(fire_on[j]>0)
				{
					if(fire_bx[buf][j]>=x && fire_bx[buf][j]<(x+width) 
						&& fire_by[buf][j]>=y && fire_by[buf][j]<(y+height) )
					{
	 				 	fire_on[j]=-2;
						sprites[i].move_ptr=sprites[i].on_hit_ptr;
						break;
					}
				}
			}

		}
	}
}
*/

/*…ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕª
  ∫ Sprite to Sprite Collision                                             ∫
  ∫                                                                        ∫
  »ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕº*/
// This should be checked perhaps with just the on_screen list... it would be faster

flag collision_with(sprite_sequence *its_seq,int its_frame,int its_x,int its_y)
{
	char *gfxdata;
	short *gfxdataw;
	flag x_is_in,y_is_in;
	int ptr;
	short width,height;
	short its_width,its_height;
	int i,x,y;
	int name,frame;
	
	ptr=its_seq->map[its_frame];
	gfxdata=its_seq->data;
	gfxdataw=(short *) (gfxdata+ptr);

	its_x+=(int ) gfxdataw[0];
	its_y+=(int ) gfxdataw[1];
	its_width=(int ) gfxdataw[2];
	its_height=(int ) gfxdataw[3];

	for(i=0;i<MAX_SPRITES;i++)
 	{
		if( (sprites[i].on==TRUE) && (sprites[i].shootable==YES) )
		{		
			name=sprites[i].name;
			x=sprites[i].x;
			y=sprites[i].y;
			frame=sprites[i].frame;
	
			ptr=seqdata[name].map[frame];
			gfxdata=seqdata[name].data;
			gfxdataw=(short *) (gfxdata+ptr);

			x+=(int ) gfxdataw[0];
			y+=(int ) gfxdataw[1];
			width=gfxdataw[2];
			height=gfxdataw[3];

			x_is_in=FALSE;
			if( its_x<=x && (its_x+its_width)>(x+width) ) x_is_in=TRUE;
			if( its_x>=x && its_x<(x+width) ) x_is_in=TRUE;
			if( (its_x+its_width)>=x && (its_x+its_width)<(x+width) )
				x_is_in=TRUE;

			y_is_in=FALSE;
			if( its_y<=y && (its_y+its_height)>(y+height) ) y_is_in=TRUE;
			if( its_y>=y && its_y<(y+height) ) y_is_in=TRUE;
			if( (its_y+its_height)>=y && (its_y+its_height)<(y+height) ) 
				y_is_in=TRUE;

			if(x_is_in==TRUE && y_is_in==TRUE) return TRUE;
		}
	}
	return FALSE;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* 320x240 demo */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void illustrate_320x240(void )
{
	char 		*source,*dest;
	long		x,y;

	source=readlbm("360S.LBM");

	set_x320y240();
//	set_x640y400();

	set_palette(palette,0,256);

	// Make a 320x240 out of a 640x480 in memory...

	dest=malloc(320*240);

	for(y=0;y<240;y++)
	{
		for(x=0;x<320;x++)
		{
			dest[x+y*320]=source[x*2+y*640*2];
		}
	}

	// Copy it to the chain4...

	dump_x320y240(dest);

	free(dest);
}
