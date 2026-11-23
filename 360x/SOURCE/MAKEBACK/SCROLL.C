/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Edge Update Functions */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include "globals.h"
//#define VIEW_COLLISION true

void force_scroll(void);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Horizontal Scroll Code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

char *build_vertical_line(int x,int y)
{
	char	*source,*dest;
	int   ptr;
 	int	ix,iy,iiy;
	int	mx,my;
	int	i,j,loop;
	unsigned char bit;

//	return dump_blk_buf;

	dest=dump_blk_buf+x;

	i=0;

	ix=x%blk_dim;
	iiy=(y%blk_dim);
	iy=iiy*blk_dim;
	mx=(x/blk_dim);
	my=(y/blk_dim)*(iff_width/blk_dim);

	ptr=(i+y)*iff_width+x;
	bit=1<<(7-(ptr&7));
	ptr=ptr>>3;
	
	/* First Vertical Tile */

	source=blk_buf+(map[mx+my]*blk_size)+ix+iy;
	for(loop=0;loop<(blk_dim-iiy);loop++)
	{
		j=y_mod640[(i+y)];
		dest[j]=*source;
		i++;
		source+=blk_dim;
		#ifdef VIEW_COLLISION
		if( (coll_buf[ptr] & bit) !=0)
		{
	 		if( ((y+i)&1)!=(x&1) ) dest[j]=0;
		}
		ptr+=iff_width/8;
		#endif
	}

	/* Subsequent Vertical Tiles */

	while(i<viewable_height)
	{
		my+=(iff_width/blk_dim);
		source=blk_buf+(map[mx+my]*blk_size)+ix;
		for(loop=0;loop<blk_dim;loop++)
		{
			if(i==viewable_height) break;			// dodgey bodge
			j=y_mod640[(i+y)];
			dest[j]=*source;
			//if( j==0 ) dest[j]=255;					// bug is here
			i++;
			source+=blk_dim;
			#ifdef VIEW_COLLISION
			if( (coll_buf[ptr] & bit) !=0)
			{
				if( ((y+i)&1)!=(x&1) ) dest[j]=0;
			}
			ptr+=iff_width/8;
			#endif
		}
	}
	return dest;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Incredibly slow */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Too many checks & too many bank changes
// Only does one strip at a time...
// Remember double buffer causes twice as many copies +
// Quadruple buffer doubles this again...
// Re-write this in code tonight me thinks...
// Could be well fast...
// One line wrong

void scroll_left(int screen,int x,int y)
{
	//char *vga=(char *) screen_lin_addr;
	//long band;
	//long dest;
	//int i,iy;
	char *source,*savesource;

	currentband=-1;

	savesource=source=build_vertical_line(x,y);

	if(safe_mode==NO)
	{
	left_scroll++;
	
	/*
	iy=y_mod640[y];
	dest=(y_mod[y]+screen+split_height)*640+x;
	for(i=0;i<screen_height;i++)
	{
		band=(dest>>16);
		if (band!=currentband) { setvesaband(band); currentband=band; }
		vga[dest&0xffff]=source[iy];
		//if(i==0) vga[dest&0xffff]=255;
		//if(iy==0) vga[dest&0xffff]=255;
		dest+=640; iy+=640;
		if(iy==(screen_height*640))
			{ iy=0; dest=(screen+split_height)*640+x; }
	}
	source=savesource;
	iy=y_mod640[y];
	dest=(y_mod[y]+screen+split_height+screen_height)*640+x;
	for(i=0;i<screen_height;i++)
	{
		band=(dest>>16);
		if (band!=currentband) { setvesaband(band); currentband=band; }
		vga[dest&0xffff]=source[iy];
		//if(i==0) vga[dest&0xffff]=255;
		//if(iy==0) vga[dest&0xffff]=255;
		dest+=640; iy+=640;
		if(iy==(screen_height*640))
			{ iy=0; dest=(screen+split_height+screen_height)*640+x; }
	}
	*/

	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Incredibly slow scroll right */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define add_constant (screen_height*640)

void scroll_right(int screen,int x,int y)
{
	//char *vga=(char *) screen_lin_addr;
	//long band;
	//int dest;
	//int i,iy;
	char *source,*savesource;

	currentband=-1;

	x+=viewable_width;
	
	savesource=source=build_vertical_line(x,y);

	if(safe_mode==NO)
	{
	right_scroll++;
	
	/*
	iy=y_mod640[y];
	dest=(y_mod[y]+screen+split_height)*640+x;
	for(i=0;i<screen_height;i++)
	{
		band=(dest>>16);
		if (band!=currentband) { setvesaband(band); currentband=band; }
		vga[dest&0xffff]=source[iy];
		dest+=640; iy+=640;
		if(iy==(screen_height*640))
		{
			iy=0;
			dest=(screen+split_height)*640+x;	
		}
	}
	source=savesource;
	iy=y_mod640[y];
	dest=(y_mod[y]+screen+split_height+screen_height)*640+x;
	for(i=0;i<screen_height;i++)
	{
		band=(dest>>16);
		if (band!=currentband) { setvesaband(band); currentband=band; }
		vga[dest&0xffff]=source[iy];
		dest+=640; iy+=640;
		if(iy==(screen_height*640))
		{	
			iy=0;
			dest=(screen+split_height+screen_height)*640+x;
		}
	}
	*/
	}
}

void add_horiz_scroll(long screen,long x,long y )
{
	long i;
	char *source;
	long count;

	if( (left_scroll-right_scroll)==0 ) return;

	if(left_scroll>0)
	{
		count=left_scroll;
	}
	if(right_scroll>0)
	{
		count=right_scroll;
		x-=right_scroll;
		x+=viewable_width;
	}
	source=dump_blk_buf+x;
	for(i=0;i<viewable_height;i++)
	{
		asm_add_dump_line(source+y_mod640[y+i],
								(screen+split_height+y_mod[y+i])*SVGA_logical_length+x,count);
		asm_add_dump_line(source+y_mod640[y+i],
								(screen+split_height+screen_height+y_mod[y+i])*SVGA_logical_length+x,count);

	}
}


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Vertical Scroll Code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Get Map Pixel is a shit idea...

void rep_movsd(char *dest,char *source,int lengthdiv4);

#pragma aux rep_movsd parm caller [edi] [esi] [ecx] = \
	"rep movsd" \
	modify [ecx esi edi];

char *build_horizontal_line(int x,int y,int length)
{
	char *dest;
	int ix,iy;
	int mx,my;
	int i;
	#ifdef VIEW_COLLISION
	unsigned char mask;
	unsigned char bit;
	#endif
	
	dest=dump_blk_buf+y_mod640[y]+x;

	i=0;

	ix=x%blk_dim;
	iy=(y%blk_dim)*blk_dim;
	mx=(x/blk_dim);
	my=(y/blk_dim)*(iff_width/blk_dim);
	memcpy(dest+i,blk_buf+(map[mx+my]*blk_size)+ix+iy,blk_dim-ix);
	i+=(blk_dim-ix);
	while(i<length)
	{
		++mx;
		if((i+16)<length)
			memcpy(dest+i,blk_buf+(map[mx+my]*blk_size)+iy,blk_dim);
		else
			memcpy(dest+i,blk_buf+(map[mx+my]*blk_size)+iy,length-i);
		i+=16;
	}

	#ifdef VIEW_COLLISION
	iy=x+y*iff_width;
	bit=1<<(7-(iy&7));
	iy=iy>>3;
	mask=coll_buf[iy++];
	for(i=0;i<length;i++)
	{
		if(bit==0)
		{
			bit=1<<7;
			mask=coll_buf[iy++];
			//mask=255;
		}				
		if( (y&1)==((x+i)&1) )
			if( (mask & bit) !=0 ) dest[i]=0;
		bit=bit>>1;
	}
	#endif
	return dest;
}

void scroll_down(int screen,int x,int y)
{
	int dest;
	char *source=build_horizontal_line(x,y+viewable_height,viewable_width);

	if(safe_mode==NO)
	{
	dest=y_mod[y]+screen+split_height;
	//screen_dump_line(source,dest*640+x,640);
	asm_add_dump_line(source,dest*SVGA_logical_length+x,viewable_width);
	dest=y_mod[y]+screen_height+screen+split_height;
	//screen_dump_line(source,dest*640+x,640);
	asm_add_dump_line(source,dest*SVGA_logical_length+x,viewable_width);
	}
}	

void scroll_up(int screen,int x,int y)
{
	int dest;
	char *source=build_horizontal_line(x,y,viewable_width);

	if(safe_mode==NO)
	{
	dest=y_mod[y]+screen+split_height;
	//screen_dump_line(source,dest*640+x,640);
	asm_add_dump_line(source,dest*SVGA_logical_length+x,viewable_width);
	dest=y_mod[y]+screen_height+screen+split_height;
	//screen_dump_line(source,dest*640+x,640);
	asm_add_dump_line(source,dest*SVGA_logical_length+x,viewable_width);
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Update Scroll XY */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Need special case in burst scroll for when ship goes to fast...  
// Burst scroll area could do with being bigger for higher resolutions...
// Could also burst in steps of 8 in these resolutions...

void update_scroll_xy(long craftx,long crafty)
{
	long old_scroll_x,old_scroll_y;	
	long vx,vy;

	old_scroll_x=scroll_x;
	old_scroll_y=scroll_y;

	scroll_x+=burst_x;
	scroll_y+=burst_y;

		// Burst scroll ready...
		vx=craftx-scroll_x-(viewable_width/2);
		if(vx>(viewable_width/6))
		{
			if(burst_scroll==TRUE) scroll_x=craftx-(viewable_width/2)+viewable_width/8;
			else scroll_x=craftx-(viewable_width/2)-(viewable_width/6);
		}
		if(vx<(-viewable_width/6))
		{
			if(burst_scroll==TRUE) scroll_x=craftx-(viewable_width/2)-viewable_width/8;
			else scroll_x=craftx-(viewable_width/2)+(viewable_width/6);
		}
		vy=crafty-scroll_y-(viewable_height/2);
		if(vy>(screen_height/6))
		{
			if(burst_scroll==TRUE) scroll_y=crafty-(viewable_height/2)+viewable_height/8;
			else scroll_y=crafty-(viewable_height/2)-(viewable_height/6);
		}
		if(vy<(-viewable_height/6))
		{
			if(burst_scroll==TRUE) scroll_y=crafty-(viewable_height/2)-viewable_height/8; //+(screen_height/8);
			else scroll_y=crafty-(viewable_height/2)+(viewable_height/6);
		}

//	if(safe_mode==NO) scroll_x=scroll_x;

	if(scroll_x>(iff_width-viewable_width)) scroll_x=iff_width-viewable_width;
	if(scroll_x<0) scroll_x=0;
	if(scroll_y<0) scroll_y=0;
	if(scroll_y>(iff_height-viewable_height)) scroll_y=iff_height-viewable_height;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Burst Scroll */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	if(burst_scroll==TRUE)
	{
		
		burst_x=scroll_x-old_scroll_x;
		burst_y=scroll_y-old_scroll_y;

		scroll_x=old_scroll_x;
		scroll_y=old_scroll_y;

		if(burst_x>0)
		{
			if(burst_x>4)
			{
				scroll_x+=4;
				burst_x-=4;
			}
			else
			{
			 	scroll_x+=burst_x;
				burst_x=0;
			}
		}
		else
		{
			if(burst_x<-4)
			{
				scroll_x-=4;
				burst_x+=4;
			}
			else
			{
			 	scroll_x+=burst_x;
				burst_x=0;
			}
		}

		if(burst_y>0)
		{
			if(burst_y>4)
			{
				scroll_y+=4;
				burst_y-=4;
			}
			else
			{
			 	scroll_y+=burst_y;
				burst_y=0;
			}
		}
		else
		{
			if(burst_y<-4)
			{
				scroll_y-=4;
				burst_y+=4;
			}
			else
			{
			 	scroll_y+=burst_y;
				burst_y=0;
			}
		}

	 	if(scroll_x>(iff_width-viewable_width)) scroll_x=iff_width-viewable_width;
		if(scroll_x<0) scroll_x=0;
		if(scroll_y<0) scroll_y=0;
		if(scroll_y>(iff_height-viewable_height)) scroll_y=iff_height-viewable_height;

	}

//	force_scroll();

}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Force Scroll */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long get_map_block(long x,long y)
{
	long mx,my;

	mx=x>>4;
	my=(y/blk_dim)*(iff_width/blk_dim);

	return map[mx+my];
}

void force_scroll(void)
{
	long i,count;
	long x,y;
	char string[32];

//	clear_box(&seqdata[NAME_BOX]);
//	sprintf(string,"NO FORCE");


	// Check left edge

	x=scroll_x;
	y=scroll_y;
	count=0;
	for(i=0;i<(viewable_height/16);i++)
	{
		if(get_map_block(x,y)==0) count++;
		y+=16;
	}
	if(count>0)
	{
		scroll_x=(x+15)&0xfff0;
		sprintf(string,"F LEFT");
	}
	
	// Check right edge

	x=scroll_x+viewable_width-1;
	y=scroll_y;
	count=0;
	for(i=0;i<(viewable_height/16);i++)
	{
		if(get_map_block(x,y)==0) count++;
		y+=16;
	}
	if(count>0)
	{
		scroll_x=(x&0xfff0)-viewable_width;
		sprintf(string,"F RIGHT");
	}

	// Check top edge

	x=scroll_x;
	y=scroll_y;
	count=0;
	for(i=0;i<(viewable_width/16);i++)
	{
		if(get_map_block(x,y)==0) count++;
		x+=16;
	}
	if(count>0)
	{
		scroll_y=(y+15)&0xfff0;
		burst_y=0;
		sprintf(string,"F UP");
	}


	// Check bottom edge

	x=scroll_x;
	y=scroll_y+viewable_height-1;
	count=0;
	for(i=0;i<(viewable_width/16);i++)
	{
		if(get_map_block(x,y)==0) count++;
		x+=16;
	}
	if(count>0)
	{
		scroll_y=(y&0xfff0)-viewable_height;
		burst_y=0;
		sprintf(string,"F DOWN");
	}

//	sprintf(string,"%i %i",count,(viewable_height/16));

//	print_string_tobox(vfont4,NAME_BOX,2,2,string);
//	sprintf(string,"%i %i",scroll_x,scroll_y);
//	print_string_tobox(vfont4,NAME_BOX,2,24,string);

}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Make Y Modulus Table */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void make_y_mod_table(void)
{
	int i;
	for(i=0;i<4096;i++)
	{
		y_mod[i]=i%viewable_height;
		y_mod640[i]=y_mod[i]*viewable_width;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Initialize scroll variables */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void init_scroll(void )
{
	char *source;
	long i,iy;

	if(safe_mode==YES) split_height=0;
	else split_height=96;

	physical_screen=0;
	physical_screen_x=0;
	physical_screen_y=0;
	physical_screen_start=0;

	buf=0;
	bursting=NO;
	burst_x=0; burst_y=0;

	physical_screen=0;

	viewable_width=640;
	viewable_height=480-96;
   if(safe_mode==YES)
	{
 		logical_screen=512;
		viewable_height=480-96;
	}
	else
	{
		viewable_height=480-96;
		logical_screen=viewable_height*2;
	}
	if(full_screen==YES)
	{ 
		viewable_width=800;
		viewable_height=600;
		logical_screen=640;
		/*
		viewable_width=1024; 
		viewable_height=768;
		logical_screen=768;
		*/
	}
	screen_xor=logical_screen;

	make_y_mod_table();

	if(safe_mode==NO) set_split(screen_height-1);

	set_vga_address(0,split_height);

	currentband=-1;
	if(safe_mode==YES)
	{
		for(iy=screen_height;iy<480;iy++)
			screen_set_line((split_height+iy)*SVGA_logical_length,640,0);

		for(iy=0;iy<viewable_height;iy++)
		{
			source=build_horizontal_line(0,iy,viewable_width);
			for(i=0;i<2;i++)
			{
				screen_dump_line(source,(split_height+(screen_xor*i)+iy)*SVGA_logical_length,viewable_width);
			}
		}
	}
	else
	{
		for(iy=0;iy<split_height;iy++)
			screen_set_line(iy*SVGA_logical_length,640,0);

		for(iy=0;iy<screen_height;iy++)
		{
			source=build_horizontal_line(0,iy,640);
			for(i=0;i<4;i++)
			screen_dump_line(source,(split_height+(screen_height*i)+iy)*SVGA_logical_length,640);
		}
	}

	last_scroll_x=scroll_x;
	last_scroll_y=scroll_y;
	old_scroll_x=scroll_x;
	old_scroll_y=scroll_y;
	bufx[0]=scroll_x; bufy[0]=scroll_y;
	bufx[1]=scroll_x; bufy[1]=scroll_y;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Set Scroll Position */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
 
void set_scroll_position(void )
{		
		buf^=1;
		if(safe_mode==YES)
		{
 			logical_screen^=screen_xor;
			physical_screen^=screen_xor;
			
			SVGA_set_display_start(0,physical_screen+split_height,YES);
			physical_screen_start=(physical_screen+split_height)*SVGA_logical_length;
			physical_screen_x=0;
			physical_screen_y=physical_screen+split_height;
		}
		else
		{
			logical_screen^=screen_xor;
			physical_screen^=screen_xor;
			//i=set_vga_address(scroll_x-(scroll_x&0),physical_screen+split_height+(scroll_y%screen_height));
			SVGA_set_display_start(scroll_x-(scroll_x&0),physical_screen+split_height+(scroll_y%screen_height),YES);
			physical_screen_start=scroll_x+(physical_screen+split_height+(scroll_y%screen_height))*640;
			physical_screen_x=scroll_x;
			physical_screen_y=physical_screen+split_height+(scroll_y%screen_height);
		}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Update Scroll Edge */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
 
void update_scroll_edge(void )
{
	old_scroll_x=bufx[buf];
	old_scroll_y=bufy[buf];

	left_scroll=0;
	right_scroll=0;
	while(scroll_x<bufx[buf])
		  scroll_left(logical_screen,--bufx[buf],bufy[buf]);

	while(scroll_x>bufx[buf])
		  scroll_right(logical_screen,bufx[buf]++,bufy[buf]);

	while(scroll_y>bufy[buf])
		scroll_down(logical_screen,bufx[buf],bufy[buf]++);

	while(scroll_y<bufy[buf])
		scroll_up(logical_screen,bufx[buf],--bufy[buf]);

	if(safe_mode==NO) add_horiz_scroll(logical_screen,scroll_x,scroll_y);

	if(safe_mode==YES)
	{
		bursting=NO;
		// This doesn't work if the scroll has been forced...
		// to the edge...
		if(old_scroll_x!=scroll_x || old_scroll_y!=scroll_y)
		{
			bursting=YES;
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

