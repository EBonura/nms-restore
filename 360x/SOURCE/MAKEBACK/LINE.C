/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Line draw code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
/*
	Polygon draw
	Clip polygon?
	Find highest lines
	Draw line
	Would polygon clip be better done with sprite code?
	Could use precalculation...
*/

#include "globals.h"

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Line code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// A bit convoluted though
// This allows easy removal of line on double buffered screen
// Line is initialised with line_on = 0
// Line decrements to -2 through remove_line to delete from both buffers...

void init_line_list(void )
{
	int i,b;
	for(i=0;i<MAX_LINES;i++)
	{
		lines[i].line_on=-2;
		for(b=0;b<2;b++)
		{
			lines[i].x1[b]=-1;
			lines[i].y1[b]=-1;
			lines[i].x2[b]=-1;
			lines[i].y2[b]=-1;
		}
	}
}

void draw_lines(int buf)
{
	int i;

	for(i=0;i<MAX_LINES;i++)
	{
		if(lines[i].line_on>=0)
		{
			draw_line(255,	lines[i].x1[buf],lines[i].y1[buf],
								lines[i].x2[buf],lines[i].y2[buf]);
		}
	}
}

void remove_lines(int buf)
{
	int i;

	for(i=0;i<MAX_LINES;i++)
	{
		if(lines[i].line_on!=-2)
		{
			remove_line(   lines[i].x1[buf],lines[i].y1[buf],
								lines[i].x2[buf],lines[i].y2[buf]);
			lines[i].line_on--;
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Draw Screen Line Dx-Dy Method */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

/*
void draw_pixel(int x,int y,char c)
{
	if(safe_mode==YES)
	{
		y-=bufy[buf]; 
		//if((y>=clip_start_y) && (y<clip_end_y)
		//&& ((x-scroll_x)>=clip_start_x) && ((x-scroll_x)<clip_end_x))
		plotpixel(x-bufx[buf]+(y+logical_screen+split_height)*SVGA_logical_length,c);
	}																								  x-bufx[buf]+(y+logical_screen+split_height)*SVGA_logical_length
	else
	{
		y-=bufy[buf]; 
		//if((y>=clip_start_y) && (y<clip_end_y)
		//&& ((x-scroll_x)>=clip_start_x) && ((x-scroll_x)<clip_end_x))
		plotpixel(x+(bufy[buf]%screen_height+y+logical_screen+split_height)*SVGA_logical_length,c);
	}
}
*/

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Clip Line DX-DY method */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag clip_line(long *p_x1,long *p_y1,long *p_x2,long *p_y2)
{
	long	x1=*p_x1;
	long	y1=*p_y1;
	long	x2=*p_x2;
	long	y2=*p_y2;
	long 	x;
	long	dx,dy;
	long 	width,height;				// positive width & height
	long 	swidth,sheight;			// signed width & height
	long 	xdist,ydist;				// temp vars
	long	lowest,highest;
	flag	pt1_on_screen;
	flag	pt2_on_screen;
	
	// --- X Total Clip ---

	if( x1<x2 ) { lowest=x1; highest=x2; }
	else			{ lowest=x2; highest=x1; }
	if( highest<clip_start_x ) return FALSE;
	if( lowest >=viewable_width  ) return FALSE;

	// --- Y Total Clip ---

	if( y1<y2 ) { lowest=y1; highest=y2; }
	else			{ lowest=y2; highest=y1; }
	if( highest<clip_start_y ) return FALSE;
	if( lowest>=viewable_height ) return FALSE; 

	// --- Get Width & Height ---

	swidth=width=x2-x1;
	sheight=height=y2-y1;
	if(width<0) width=-width;
	if(height<0) height=-height;

	// ----- Is point 1 onscreen? -----

	if( (x1>=clip_start_x) &&	(x1<viewable_width)	&&
		 (y1>=clip_start_y) &&	(y1<viewable_height)	)
			pt1_on_screen=TRUE;
	else	
			pt1_on_screen=FALSE;

	// Is point 2 onscreen?

	if( (x2>=clip_start_x) &&	(x2<viewable_width)	&&
		 (y2>=clip_start_y) &&	(y2<viewable_height)	)
			pt2_on_screen=TRUE;
	else	
			pt2_on_screen=FALSE;

	// ----- Is one of the points not on the screen? -----

	if( !(pt1_on_screen==TRUE && pt2_on_screen==TRUE) )
	{

	// ************************************************
	// ----- Points off the screen? -----
	// ************************************************
	
		// Track each to edge of screen and find out if on screen

		if(x1<clip_start_x)
		{
	 		dx=clip_start_x-x1;				// X distance to edge
			dy=(sheight<<16)/width;			//	gradient of slope
			ydist=(dy*dx)>>16;				// Y distance to edge
			y1=y1+ydist;						// y position on left edge
			x1=clip_start_x;					// new X
		}

		if(x1>=viewable_width)
		{
	 		dx=x1-viewable_width;
			dy=(sheight<<16)/width;
			ydist=(dy*dx)>>16;
			y1=y1+ydist;
			x1=viewable_width-1;
		}

		if(y1<clip_start_y)
		{
	 		dy=clip_start_y-y1;				// Y distance to edge
			dx=(swidth<<16)/height;			// gradient
			xdist=(dy*dx)>>16;				// X distance to edge
			x=x1+xdist;							// X distance on edge
			if(x<clip_start_x || x>=viewable_width)	return FALSE;
			y1=clip_start_y;					// New Y
			x1=x;									// New X
		}

		if(y1>=viewable_height)
		{
	 		dy=y1-viewable_height;
			dx=(swidth<<16)/height;
			xdist=(dy*dx)>>16;
			x=x1+xdist;
			if(x<clip_start_x || x>=viewable_width)	return FALSE;
			y1=viewable_height-1;
			x1=x;
		}

		// Clip Second point (same as first point clip)

		if(x2<clip_start_x)
		{
	 		dx=clip_start_x-x2;
			dy=(sheight<<16)/width;
			ydist=(dy*dx)>>16;
			y2=y2-ydist;
			x2=clip_start_x;
		}

		if(x2>=viewable_width)
		{
	 		dx=x2-viewable_width;
			dy=(sheight<<16)/width;
			ydist=(dy*dx)>>16;
			y2=y2-ydist;
			x2=viewable_width-1;
		}

		if(y2<clip_start_y)
		{
	 		dy=clip_start_y-y2;
			dx=(swidth<<16)/height;
			xdist=(dy*dx)>>16;
			x=x2-xdist;
			if(x<clip_start_x || x>=viewable_width)	return FALSE;
			y2=clip_start_y;
			x2=x;
		}

		if(y2>=viewable_height)
		{
	 		dy=y2-viewable_height;
			dx=(swidth<<16)/height;
			xdist=(dy*dx)>>16;
			x=x2-xdist;
			if(x<clip_start_x || x>=viewable_width)	return FALSE;
			y2=viewable_height-1;
			x2=x;
		}

		*p_x1=x1;
		*p_y1=y1;
		*p_x2=x2;
		*p_y2=y2;

 	}
	return TRUE;

}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Draw Line DX-DY method */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void draw_line(unsigned char c,long x1,long y1,long x2,long y2)
{
	int 	width,height;
	int 	swidth,sheight;
	int 	x,y;
	int	dx,dy;
	long   screen_x1,screen_y1;
	long	screen_x2,screen_y2;

	screen_x1=x1-bufx[buf];
	screen_x2=x2-bufx[buf];
	screen_y1=y1-bufy[buf];
	screen_y2=y2-bufy[buf];

	if(clip_line(&screen_x1,&screen_y1,&screen_x2,&screen_y2)==FALSE) return;

	x1=screen_x1+bufx[buf];
	x2=screen_x2+bufx[buf];
	y1=screen_y1+bufy[buf];
	y2=screen_y2+bufy[buf];

	swidth=width=x2-x1;
	sheight=height=y2-y1;
	if(width<0) width=-width;
	if(height<0) height=-height;

	if(width<height)
	{
		if(height>0)
		{
			if(y1<y2) dy=1;
			else dy=-1;

			x=x1<<16;
			if(width==0) dx=0;
			else dx=(swidth<<16)/height;

			for(y=0;y<height;y++)
			{
				draw_pixel(x>>16,y1,c);
				x+=dx; y1+=dy;
			}
		}
		draw_pixel(x2,y2,c);
	}
	else
	{
		if(x1<x2) dx=1;
		else dx=-1;

		y=y1<<16; 
		if(height==0) dy=0;
		else dy=(sheight<<16)/width;
		for(x=0;x<width;x++)
		{
			draw_pixel(x1,y>>16,c);
			x1+=dx; y+=dy; 
		}
		draw_pixel(x2,y2,c);
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Remove Screen Line Dx-Dy Method */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Pixel clip is a bit slow...
// Should be on pixel list for faster (less) bank changing
// Although UNIVBE has helped with this problem...

/*
void remove_pixel(int x,int y)
{
	if(safe_mode==YES)
	{
	y-=bufy[buf];
//	if((y>=clip_start_y) && (y<clip_end_y)
//		&& ((x-scroll_x)>=clip_start_x) && ((x-scroll_x)<clip_end_x))
	plotpixel(x-bufx[buf]+(y+logical_screen+split_height)*SVGA_logical_length, 
		get_map_pixel(x,y+bufy[buf]));
	}
	else
	{
	y-=bufy[buf];
//	if((y>=clip_start_y) && (y<clip_end_y)
//		&& ((x-scroll_x)>=clip_start_x) && ((x-scroll_x)<viewable_width))
	plotpixel(x+(bufy[buf]%screen_height+y+logical_screen+split_height)*SVGA_logical_length, 
		get_map_pixel(x,y+bufy[buf]));
	}
}
*/

void remove_line(long x1,long y1,long x2,long y2)
{
	int 	width,height;
	int 	swidth,sheight;
	int 	x,y;
	int	dx,dy;
	long  screen_x1,screen_y1;
	long	screen_x2,screen_y2;

	if(safe_mode==YES && bursting==YES) return;

	screen_x1=x1-bufx[buf];
	screen_x2=x2-bufx[buf];
	screen_y1=y1-bufy[buf];
	screen_y2=y2-bufy[buf];

	if(clip_line(&screen_x1,&screen_y1,&screen_x2,&screen_y2)==FALSE) return;

	x1=screen_x1+bufx[buf];
	x2=screen_x2+bufx[buf];
	y1=screen_y1+bufy[buf];
	y2=screen_y2+bufy[buf];

	swidth=width=x2-x1;
	sheight=height=y2-y1;
	if(width<0) width=-width;
	if(height<0) height=-height;

	if(width<height)
	{
		if(height>0)
		{
			if(y1<y2) dy=1;
			else dy=-1;

			x=x1<<16;
			if(width==0) dx=0;
			else dx=(swidth<<16)/height;

			for(y=0;y<height;y++)
			{
				remove_pixel(x>>16,y1);
				x+=dx; y1+=dy;
			}
		}
		remove_pixel(x2,y2);
	}
	else
	{
		if(x1<x2) dx=1;
		else dx=-1;

		y=y1<<16;
		if(height==0) dy=0;
		else dy=(sheight<<16)/width;
		for(x=0;x<width;x++)
		{
			remove_pixel(x1,y>>16);
			x1+=dx; y+=dy;
		}
		remove_pixel(x2,y2);
	}
}
