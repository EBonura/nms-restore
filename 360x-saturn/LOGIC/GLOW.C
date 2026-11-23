/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Background Warp Section */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Need clear_polo_list,add_to_polo_list,draw_polo_list functions
// Should not go from process.c
// Left to do...
// Different tables for different glow levels...
// Optimisations... (Dont clip a line if you don't have to (its slow...))
// Glow line which clips against the background...

#include	<sgl.h>
#include "defines.h"
#include "types.h"
#include "globals.h"
#include <stdio.h>
#include <libsn.h>

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

#define MAX_GLOW_LINES 512

glow	glow_line[2][MAX_GLOW_LINES];		// 12k

long current_glow[2];

#define MAX_RADIUS 128

short	polo_x1[MAX_RADIUS];				// 1K
short	polo_x2[MAX_RADIUS];

//long	buf	=	0;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void 	add_a_glow_line_clipped(long x,long y,long width);
void 	add_a_glow_line(long x,long y,long width);
void 	draw_glow_list(void );
void 	remove_glow_list(void );
void 	add_polo(long r1,long r2,long start_x,long start_y);
void 	draw_polo_clipped(long r1,long r2,long start_x,long start_y);
void 	draw_polo(long r1,long r2,long start_x,long start_y);
void 	add_circle(long radius,long start_x,long start_y);
void 	draw_circle_clipped(long r,long start_x,long start_y);
void 	draw_circle(long r,long start_x,long start_y);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void init_glow_list(void )
{
	current_glow[buf]=0;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Add a glow line */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Need a non-clip version...
// Clipping is very,very slow...
// 7 checks...

// Not working...

void add_a_glow_line_clipped(long x,long y,long width)
{
	#if 0
	long	x1,x2,y1;
	long 	i=current_glow[buf];

	if(i>=MAX_GLOW_LINES) 
	{
		internal_message("Run out of GLOW LINE space\n"); 
		return;
	}

	x1=x-bufx[buf];
	x2=x1+width;
	y1=y-bufy[buf];

	if( y1>=viewable_height ) return;
	if( y1<clip_start_y     ) return;
	if( x1>=viewable_width  ) return;
	if( x2<clip_start_x     ) return;

	if( x1<clip_start_x )
	{
		width+=x1;
		x-=x1;
	}

	if( x2>=viewable_width )
	{
		width-=(x2-viewable_width);
	}

	if(width<=0 ) return;

	glow_line[buf][i].x=x;
	glow_line[buf][i].y=y;
	glow_line[buf][i].width=width;
	current_glow[buf]++;
	#endif
}

void add_a_glow_line(long x,long y,long width)
{
	#if 0
	long 	i=current_glow[buf];

	if(i>=MAX_GLOW_LINES)
	{
		internal_message("Run out of GLOW LINE space\n");
		return;
	}

	glow_line[buf][i].x=x;
	glow_line[buf][i].y=y;
	glow_line[buf][i].width=width;
	current_glow[buf]++;
	#endif
}


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Draw Lines */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void draw_glow_list(void )
{
	#if 0
	long i,x,y,width;

	for(i=0;i<current_glow[buf];i++)
	{
		x=glow_line[buf][i].x;
		y=glow_line[buf][i].y;
		width=glow_line[buf][i].width;

		if(safe_mode==YES)
			asm_add_transparent_line(dump_blk_buf+x+y_mod640[y],
				x-bufx[buf]+(y+logical_screen+split_height-bufy[buf])*SVGA_logical_length,
											width);
					else
			asm_add_transparent_line(dump_blk_buf+x+y_mod640[y],
				x+(y+logical_screen+split_height+y_mod[(bufy[buf])]-bufy[buf])*SVGA_logical_length,
											width);
	}
	#endif
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Remove Lines */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Not clipping against background yet...

void remove_glow_list(void )
{
	#if 0
	long	i,x,y,width;

	if ( safe_mode==YES && bursting==YES ) return;

	for(i=0;i<current_glow[buf];i++)
	{
		x=glow_line[buf][i].x;
		y=glow_line[buf][i].y;
		width=glow_line[buf][i].width;
	
		if(safe_mode==YES)
			screen_dump_line(dump_blk_buf+x+y_mod640[y],
			 	x-bufx[buf]+(y+logical_screen+split_height-bufy[buf])*SVGA_logical_length,
									width);
		else
			screen_dump_line(dump_blk_buf+x+y_mod640[y],
			 	x+(y+logical_screen+split_height+y_mod[(old_scroll_y)]-old_scroll_y)*SVGA_logical_length,
									width);
	}
	#endif
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Polo */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Do a pre-check to see if any clipping is necessary...
// Do not need to calculate the second circle STUPID!
// No it doesn't even stupider

void add_polo(long r1,long r2,long start_x,long start_y)
{
	#if 0
	long x1,y1,x2;
	long sq_x1,sq_y1,sq_r1;
	long sq_x2,sq_y2,sq_r2;

	sq_r1=r1*r1;
	sq_r2=r2*r2;
	x1=r1;
	x2=r2;
	sq_x1=r1*r1;
	sq_x2=r2*r2;
	for(y1=1;y1<r1;y1++)
	{
		sq_y1=y1*y1;
		while((sq_y1+sq_x1)>sq_r1)
		{
			x1--;
			sq_x1=x1*x1;
		}
		polo_x1[y1]=x1;
	
		if(y1<=r2)
		{
			sq_y2=y1*y1;
			while((sq_y2+sq_x2)>sq_r2)
			{
				x2--;
				sq_x2=x2*x2;
			}
			polo_x2[y1]=x2;
		}
	}

	x1=start_x-bufx[buf];
	y1=start_y-bufy[buf];
	
	x2=0;
	if( (y1-r1) >=start_y   		) x2|=1;
	if( (y1+r1) < viewable_height ) x2|=2;
	if( (x1-r1) >=start_x    	   ) x2|=4;
	if( (x1+r1) < viewable_width  ) x2|=8;

	if(x2==15)	draw_polo(r1,r2,start_x,start_y);
	else			draw_polo_clipped(r1,r2,start_x,start_y);
//	draw_polo_clipped(r1,r2,start_x,start_y);
	#endif
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Call the slow clipped version of add glow line */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void draw_polo_clipped(long r1,long r2,long start_x,long start_y)
{
	#if 0
	long x1,x2,y1;
	long gap,edge;

	for(y1=1;y1<r1;y1++)
	{
		x1=polo_x1[y1];
		if(y1>r2)
		{
			add_a_glow_line_clipped(start_x-x1,start_y-y1,x1*2);
		 	add_a_glow_line_clipped(start_x-x1,start_y+y1-1,x1*2);
		}
		else
		{
			x2=polo_x2[y1];
			edge=((x1*2)-(x2*2))/2;
			gap=x2*2;
		 	add_a_glow_line_clipped(start_x-x1,start_y-y1,edge);
		 	add_a_glow_line_clipped(start_x-x1+edge+gap,start_y-y1,edge);
		 	add_a_glow_line_clipped(start_x-x1,start_y+y1-1,edge);
		 	add_a_glow_line_clipped(start_x-x1+edge+gap,start_y+y1-1,edge);
		}
	}
	#endif
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* No need to call the slow clipped version of add glow line */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void draw_polo(long r1,long r2,long start_x,long start_y)
{
	#if 0
	long x1,x2,y1;
	long gap,edge;

	for(y1=1;y1<r1;y1++)
	{
		x1=polo_x1[y1];
		if(y1>r2)
		{
		 	add_a_glow_line(start_x-x1,start_y-y1,x1*2);
		 	add_a_glow_line(start_x-x1,start_y+y1-1,x1*2);
		}
		else
		{
			x2=polo_x2[y1];
			edge=((x1*2)-(x2*2))/2;
			gap=x2*2;
		 	add_a_glow_line(start_x-x1,start_y-y1,edge);
		 	add_a_glow_line(start_x-x1+edge+gap,start_y-y1,edge);
		 	add_a_glow_line(start_x-x1,start_y+y1-1,edge);
		 	add_a_glow_line(start_x-x1+edge+gap,start_y+y1-1,edge);
		}
	}
	#endif
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Circle */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Do a pre-check to see if any clipping is necessary...


void add_circle(long radius,long start_x,long start_y)
{
	#if 0
	long x,y;
	long sq_x,sq_y,sq_radius;

	sq_radius=radius*radius;
	x=radius;
	sq_x=radius*radius;
	for(y=1;y<radius;y++)
	{
		sq_y=y*y;
		while((sq_y+sq_x)>sq_radius)
		{
			x--;
			sq_x=x*x;
		}
	 	//add_a_glow_line_clipped(start_x-x,start_y-y,x*2);
 		//add_a_glow_line_clipped(start_x-x,start_y+y-1,x*2);
		polo_x1[y]=x;
	}

	x=start_x-bufx[buf];
	y=start_y-bufy[buf];
	
	sq_x=0;
	if( (y-radius) >=start_y   		) sq_x|=1;
	if( (y+radius) < viewable_height ) sq_x|=2;
	if( (x-radius) >=start_x    	   ) sq_x|=4;
	if( (x+radius) < viewable_width  ) sq_x|=8;

	if(sq_x==15)	draw_circle			 (radius,start_x,start_y);
	else				draw_circle_clipped(radius,start_x,start_y);
	#endif
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Call the slow clipped version of add glow line */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void draw_circle_clipped(long r,long start_x,long start_y)
{
	#if 0
	long x,y;

	for(y=1;y<r;y++)
	{
		x=polo_x1[y];
		add_a_glow_line_clipped(start_x-x,start_y-y  ,x*2);
		add_a_glow_line_clipped(start_x-x,start_y+y-1,x*2);
	}
	#endif
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* No need to call the slow clipped version of add glow line */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void draw_circle(long r,long start_x,long start_y)
{
	#if 0
	long x,y;

	for(y=1;y<r;y++)
	{
		x=polo_x1[y];
		add_a_glow_line(start_x-x,start_y-y  ,x*2);
	 	add_a_glow_line(start_x-x,start_y+y-1,x*2);
	}
	#endif
}
		
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
