/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Crude Polygon System */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include	"globals.h"

long *return_edge_list_ptr(void);

unsigned char remapper[256];

void make_remapper(void)
{
	int i;

	for(i=0;i<256;i++)
	{
		if( (i+48)<255 ) remapper[i] = i+48;
		else remapper[i]=255;
	}
	asm_get_remap_table(remapper);

}

// clip left edge
// x could be less x1 or greater than x1
// to speed it up remap should not be from the screen
// even more speed use initial list from polyfill.asm

void use_edge_list(void)
{
	long	x1,x2;
	long	xa,xb;
	long	x,y;
	long	dest;
	long	length;
	long  edges;
	long *edge_list=return_edge_list_ptr();

	edges=*edge_list++;

	while(edges>0)
	{
		xa=*edge_list++;
		y=*edge_list++;
		length=*edge_list++;
		xb=xa+length;
		dest=edge_offs[y];

		x1=0;
		while(x1<iff_width)
		{
			x1=edge_array[dest++];
			x2=edge_array[dest++];
			if( xb<x1) break;			// finished?

			if( xb>x1 && xa<x2)		// line viewable?
			{
				if(xa<x1)				// left clip?
				{
					length-=x1-xa;
					xa=x1;
				}
				if(xb>x2)				// right clip?
				{
					length-=xb-x2;
				}
				if(length>0)			// draw it
				{
					x=xa;
					/*
					if(safe_mode==YES)
					mem2screen_remap_line(
						remapper,
						dump_blk_buf+x+y_mod640[y],
						x-bufx[buf]+(y+logical_screen+split_height-bufy[buf])*640,
						length);
					else
					mem2screen_remap_line(
						remapper,
						dump_blk_buf+x+y_mod640[y],
						x+(y+logical_screen+split_height
							 +y_mod[(bufy[buf])]-bufy[buf])*640,
						length);
					*/
					if(safe_mode==YES)
					asm_add_transparent_line(
						dump_blk_buf+x+y_mod640[y],
						x-bufx[buf]+(y+logical_screen+split_height-bufy[buf])*SVGA_logical_length,
						length);
					else
					asm_add_transparent_line(
						dump_blk_buf+x+y_mod640[y],
						x+(y+logical_screen+split_height
							 +y_mod[(bufy[buf])]-bufy[buf])*SVGA_logical_length,
						length);
	
				}
			}
			length=xb-xa;
		}
		edges--;
	}
}

void remove_edge_list(void)
{
	long	x1,x2;
	long	xa,xb;
	long	x,y;
	long	dest;
	long	length;
	long  edges;
	long *edge_list=return_edge_list_ptr();

	if ( safe_mode==YES && bursting==YES ) return;

	edges=*edge_list++;

	while(edges>0)
	{
		xa=*edge_list++;
		y=*edge_list++;
		length=*edge_list++;
		xb=xa+length;
		dest=edge_offs[y];

		x1=0;
		x2=0;
		while(x1<iff_width)
		{
			x1=edge_array[dest++];
			x2=edge_array[dest++];
			if( xb<x1) break;			// finished?

			if( xb>x1 && xa<x2)		// line viewable?
			{
				if(xa<x1)				// left clip?
				{
					length-=x1-xa;
					xa=x1;
				}
				if(xb>x2)				// right clip?
				{
					length-=xb-x2;
				}
				if(length>0)			// draw it
				{
					x=xa;
					if(safe_mode==YES)
					screen_dump_line(
						dump_blk_buf+x+y_mod640[y],
						x-bufx[buf]+(y+logical_screen+split_height-bufy[buf])*SVGA_logical_length,
						length);
//					screen_set_line(
//						x-bufx[buf]+(y+logical_screen+split_height-bufy[buf])*640,
//						length,255);
					else
					screen_dump_line(
						dump_blk_buf+x+y_mod640[y],
						x+(y+logical_screen+split_height
							 +y_mod[(bufy[buf])]-bufy[buf])*SVGA_logical_length,
						length);
				}
			}
			length=xb-xa;
		}
		edges--;
	}
}

void update_triangle(triangle tri)
{
	screen_tri[buf].faces=tri.faces;
	screen_tri[buf].colour=tri.colour;

	screen_tri[buf].x1=tri.x1; //-bufx[buf];
	screen_tri[buf].x2=tri.x2; //-bufx[buf];
	screen_tri[buf].x3=tri.x3; //-bufx[buf];
	screen_tri[buf].y1=tri.y1; //+logical_screen+split_height-bufy[buf];
	screen_tri[buf].y2=tri.y2; //+logical_screen+split_height-bufy[buf];
	screen_tri[buf].y3=tri.y3; //+logical_screen+split_height-bufy[buf];
}

void init_triangle(void )
{
	int i;
	for(i=0;i<2;i++)
	{
	screen_tri[i].faces=3;
	screen_tri[i].colour=0;

	screen_tri[i].x1=-1; //-bufx[buf];
	screen_tri[i].x2=-1; //-bufx[buf];
	screen_tri[i].x3=-1; //-bufx[buf];
	screen_tri[i].y1=-1; //+logical_screen+split_height-bufy[buf];
	screen_tri[i].y2=-1; //+logical_screen+split_height-bufy[buf];
	screen_tri[i].y3=-1; //+logical_screen+split_height-bufy[buf];

	}
}

triangle light1={
	3,0x20202020,
	100,150,		// TL
	400,300,		// BR
	100,300,		// BL
};

void remove_light(void )
{
	if(light_on==TRUE)
	{
		set_poly_clip(clip_start_x+bufx[buf],
						  viewable_width+bufx[buf],
						  clip_start_y+bufy[buf],
						  viewable_height+bufy[buf]);
		if(screen_tri[buf].x1!=-1)
		{
			make_poly_edge_list( (long *) &screen_tri[buf] );
			remove_edge_list();
		}
	}
}

void draw_light(void )
{
	double	temp;
	
 	if(light_on==TRUE)
 	{
 		light1.x1=craftx;
 		light1.y1=crafty;
 	   temp=(craft_angle-2)*((PI*2)/64);
 		light1.x2=craftx+cos(temp)*128;
 		light1.y2=crafty+sin(temp)*128;
 		temp=(craft_angle+4)*((PI*2)/64);
 		light1.x3=craftx+cos(temp)*128;
 		light1.y3=crafty+sin(temp)*128;

 		set_poly_clip(clip_start_x+bufx[buf],
 						  viewable_width+bufx[buf],
 						  clip_start_y+bufy[buf],
 						  viewable_height+bufy[buf]);
 
		update_triangle(light1);
		make_poly_edge_list( (long *) &screen_tri[buf] );
		use_edge_list();
	}
}
