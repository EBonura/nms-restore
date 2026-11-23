/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Sprite Draw & Remove Functions */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

#include "globals.h"

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* FLIC style sprite draw routine */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */
// A struct for the sprite might be nice
// sprite_no is the frame no
// x = real x
// y = real y
// sy = scroll y
// screen = logical screen y + split height

void draw_sprite(sprite_sequence *seq,int sprite_no,int x,int y,int sx,int sy,int screen)
{
	unsigned char *source;
	short *sourcew;
	int i,j;
	int ix,iy;
	int dx,lx,tx,ax,dlx;
	int width,height;
	int dy;

	if(sprite_no>=seq->frames)
	{
		i=0;
		sprintf(message,"Sprite number too high %s\n",get_sprite_name(sprites[i].name));
		internal_message(message);
		return;
	}

	x-=sx;
	y-=sy;
	i=seq->map[sprite_no];

	if(i==-1)
	{
		internal_message("Beyond last sprite\n");
		return;
	}

	source=seq->data;
	sourcew=(short *) (source+i);

	x+=(int ) sourcew[0];
	y+=(int ) sourcew[1];
	width=(int ) sourcew[2];
	height=(int ) sourcew[3];

	i+=8;

	if( x>=viewable_width )  		return;
	if( y>=viewable_height ) 		return;
	if( (y+height)<clip_start_y ) return;
	if( (x+width) <clip_start_x )	return;
		 
	if(safe_mode==YES) dy=(y+screen)*SVGA_logical_length;	// y screen ptr
	else dy=(y+y_mod[sy]+screen)*SVGA_logical_length+sx;	// y screen ptr
		
	if( (y+height)>=viewable_height ) height=viewable_height-y;	// bottom clip

	iy=0;														// top clip
	while( (y+iy)<clip_start_y )
	{
		if(iy==height) break;
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		tx=(unsigned int) source[i++];				// type of line
		ax=(unsigned int) source[i++];				// data length of line
		i+=ax;
		dy+=SVGA_logical_length;
		iy++;
	}

	if( (x>=clip_start_x) && ((x+width)<viewable_width) ) 
	{
		asm_draw_unclipped(source+i,x+dy,height-iy);
		/*
		// x not clipped
		for(;iy<height;iy++)
		{
			dx=(unsigned int) source[i++];				// x distance to first pixel
			lx=(unsigned int) source[i++];				// length of line
			tx=(unsigned int) source[i++];
			ax=(unsigned int) source[i++];
			if(tx<2) asm_add_andor_line(source+i,x+dx+dy,lx+(tx<<8));
			else
			{
				ix=i;
				while(tx>0)
				{
					lx=(unsigned int) source[ix++];
					asm_add_andor_line(source+ix,x+dx+dy,lx+256);
					ix+=lx;
					dx+=lx;
					dx+=(unsigned int) source[ix++];
					tx--;
				}

			}
			dy+=SVGA_logical_length;
			i+=ax;
		}	
		*/
	}
	else
	{
		// x clipped (very slow)
		for(;iy<height;iy++)
		{
			dx=(unsigned int) source[i++];				// x distance to first pixel
			lx=(unsigned int) source[i++];				// length of line
			tx=(unsigned int) source[i++];
			ax=(unsigned int) source[i++];
			dx+=x;
		
			if(tx<2)
			{
				if( dx>=clip_start_x) ix=0;					// left clip?
				else ix=clip_start_x-dx;
			
				if( (dx+lx)<viewable_width) dlx=lx;				// right clip?
				else dlx=viewable_width-dx;
		
				#ifdef VIEW_COLLISION
					if(ix<dlx) screen_set_line(ix+dx+dy,dlx-ix,255);
				#else
					if(ix<dlx) asm_add_andor_line(source+i+ix,ix+dx+dy,(dlx-ix)+(tx<<8));
				#endif
			}
			else
			{
				j=i;
				while(tx>0)
				{
					lx=(unsigned int) source[j++];
	
					if( dx>=clip_start_x) ix=0;					// left clip?
					else ix=clip_start_x-dx;
			
					if( (dx+lx)<viewable_width) dlx=lx;				// right clip?
					else dlx=viewable_width-dx;
	
					if(ix<dlx) asm_add_andor_line(source+j+ix,ix+dx+dy,(dlx-ix)+256);
					j+=lx;
					dx+=lx;
					dx+=(unsigned int) source[j++];
					tx--;
				}
			}
			dy+=SVGA_logical_length;
			i+=ax;
	
		}
	}
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Remove sprite routine */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void remove_sprite(sprite_sequence *seq,int sprite_no,
						 int x,int y,int sx,int sy,int screen)
{
	unsigned char *source,*dest;
	short *sourcew;
	int i=0;
	int ix,iy;
	int dx,lx,tx,ax,dlx;
	int width,height;
	int dy;

	if ( safe_mode==YES && bursting==YES ) return;

	if(sprite_no>=seq->frames)
	{
		sprintf(message,"Sprite number too high %s\n",get_sprite_name(sprites[i].name));
		internal_message(message);
		return;
	}

	x-=sx;
	y-=sy;
	i=seq->map[sprite_no];

	if(i==-1)
	{
		internal_message("Beyond last sprite\n");
	}

	source=seq->data;
	sourcew=(short *) (source+i);

	x+=(int ) sourcew[0];
	y+=(int ) sourcew[1];
	width=(int ) sourcew[2];
	height=(int ) sourcew[3];
	i+=8;

	if( x>=viewable_width ) return;
	if( y>=viewable_height ) return;
	if( (y+height)<clip_start_y ) return;
	if( (x+width )<clip_start_x ) return;

	if(safe_mode==YES) dy=(y+screen)*SVGA_logical_length;		// y screen ptr
	else dy=(y+y_mod[sy]+screen)*SVGA_logical_length+sx;	// y screen ptr
	
	if( (y+height)>=(viewable_height) ) height=viewable_height-y;	// bottom clip

	iy=0;														// top clip
	while( (y+iy)<clip_start_y )
	{
		if(iy==height) break;
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		i++;
		ax=(unsigned int) source[i++];				// length of line
		i+=ax;
		dy+=SVGA_logical_length;
		iy++;
	}

	if( (x>=clip_start_x) && ((x+width)<viewable_width) ) 
	{
	 	// not clipped
		for(;iy<height;iy++)
		{
			dx=(unsigned int) source[i++];				// x distance to first pixel
			lx=(unsigned int) source[i++];				// length of line
			tx=(unsigned int) source[i++];				// type of line
			ax=(unsigned int) source[i++];				// length of line

			dx+=x;

			dest=dump_blk_buf+dx+sx+(y_mod640[sy+iy+y]);

			//asm_add_dump_line(dest,dx+dy,lx);
			if(tx<2)
			{
				asm_add_dump_line(dest,dx+dy,lx);
			}
			else
			{
				ix=i;
				while(tx>0)
				{
					lx=(unsigned int) source[ix++];
					asm_add_dump_line(dest,dx+dy,lx);
					ix+=lx;
					dx+=lx;
					dest+=lx;
					lx=(unsigned int) source[ix++];
					dx+=lx;
					dest+=lx;
					tx--;
				}
			}
			dy+=SVGA_logical_length;
			i+=ax;
		}
	}
	else
	{
	for(;iy<height;iy++)
	{
		// clipped
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		i++;
		ax=(unsigned int) source[i++];				// length of line

		dx+=x;

		if( dx>=clip_start_x) ix=0;					// left clip?
		else ix=clip_start_x-dx;

		if( (dx+lx)<viewable_width) dlx=lx;				// right clip?
		else dlx=viewable_width-dx;

		dest=dump_blk_buf+dx+sx+ix+(y_mod640[sy+iy+y]);

		if(ix<dlx) asm_add_dump_line(dest,ix+dx+dy,dlx-ix);
		dy+=SVGA_logical_length;
		i+=ax;
	}
	}
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Draw Lines */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void use_dump_line_list(void)
{
	long i;

	asm_set_currentband(-1);
	for(i=0;i<32;i++)
	{
		currentband=asm_dump_bank(i);
		dump_pixel_bank(i);
	}
}

void clear_dump_line_list(void)
{
	asm_init_lines();
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Sprite Collision with background */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */
// Should use background map
 
unsigned char left_byte_masks[8]={
		0x7f,
		0x3f,
		0x1f,
		0x0f,
		0x07,
		0x03,
		0x01,
		0x00
};

unsigned char right_byte_masks[9]={
		0x00,	//0
		0x80,	//1
		0xc0,	//2
		0xe0,	//3
		0xf0,	//4
		0xf8,	//5
		0xfc,	//6
		0xfe,	//7
		0xff	//8
};

flag sprite_collision(sprite_sequence *seq,int sprite_no,int x,int y)
{
	unsigned char  *source;
	short *sourcew;
	unsigned char mask;
	unsigned char bit;
	int i;
	int iy;
	int dx,lx,ax;
	int disp;
	int width,height;					

//	return FALSE;

	i=seq->map[sprite_no];
	source=seq->data;
	sourcew=(short *) (source+i);
	x+=(int ) sourcew[0];
	y+=(int ) sourcew[1];
	width =sourcew[2];
	height=sourcew[3];
	i+=8;

	for(iy=0;iy<height;iy++)
	{
		dx=(unsigned int) source[i++];		// x distance to first pixel
		lx=(unsigned int) source[i++];		// length of line
		i++;
		ax=(unsigned int) source[i++];		// length of line
		i+=ax;
		if( ((y+iy)>0) && ((y+iy)<iff_height) )
		{
			disp=x+dx+iff_width*(y+iy);
			bit=1<<(7-(disp&7));
			disp=disp>>3;
			mask=coll_buf[disp++];
			// left edge (slow bit)
			while(lx>0 && bit>0)
			{
				if( (mask & bit) !=0 ) return TRUE;
				bit=bit>>1;
				lx--;
			}
			// middle (could be faster) (could be rep scasb)
			while(lx>8)
			{
				mask=coll_buf[disp++];
				if(mask!=0) return TRUE;
				lx-=8;
			}
			// end edge
			if(lx>0)
			{	
				mask=coll_buf[disp++];
				bit=1<<7;
				if( (mask&right_byte_masks[lx])!=0 ) return TRUE;
				/*
				while(lx>0)
				{
					if( (mask & bit) !=0 ) return TRUE;
					bit=bit>>1;
					lx--;
				}
				*/
			}
			/*
			for(ix=0;ix<lx;ix++)
			{
				if(bit==0)
				{
					bit=1<<7;
					mask=coll_buf[disp++];
					// optimisation...
					if(mask==0) { ix+=8; bit=0; }
				}
				if( (mask & bit) !=0 ) return TRUE;
				bit=bit>>1;
			}
			*/
		}
	}
	return FALSE;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Clip Sprite Against Background */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */
// Very Very slow routine...
// Every line is x_clipped (this is slow)

void screen_clip_line(char *source,long dest,long x,long y,long length);

void bgclip_sprite(sprite_sequence *seq,int sprite_no,
						 int x,int y,int sx,int sy,int screen)
{
	unsigned char *source;
	short *sourcew;
	int i=0,j;
	int ix,iy;
	int dx,lx,tx,ax,dlx;
	int width,height;
	int dy;

	if(sprite_no>=seq->frames)
	{
		sprintf(message,"Sprite number too high %s\n",get_sprite_name(sprites[i].name));
		internal_message(message);
		return;
	}

	x-=sx;
	y-=sy;

	i=seq->map[sprite_no];
	if(i==-1)
	{
		internal_message("Beyond last sprite\n");
		return;
	}

	source=seq->data;
	sourcew=(short *) (source+i);

	x+=(int ) sourcew[0];
	y+=(int ) sourcew[1];
	width=(int ) sourcew[2];
	height=(int ) sourcew[3];
	i+=8;

	if( x>viewable_width ) return;
	if( y>viewable_height ) return;
	if( (y+height)<clip_start_y ) return;
	if( (x+width )<clip_start_x ) return;

	if(safe_mode==YES) dy=(y+screen)*SVGA_logical_length-sx;		// y screen ptr
	else dy=(y+y_mod[sy]+screen)*SVGA_logical_length;	// y screen ptr
	
	// *** Y CLIP ***

	if( (y+height)>=viewable_height ) height=viewable_height-y;	// bottom clip

	iy=0;														// top clip
	while( (y+iy)<clip_start_y )
	{
		if(iy==height) break;
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		tx=(unsigned int) source[i++];				// type of line
		ax=(unsigned int) source[i++];				// data length of line
		i+=ax;
		dy+=SVGA_logical_length;
		iy++;
	}

	// *** DRAW ***

	if( (x>=clip_start_x) && ((x+width)<viewable_width) ) 
	{
	for(;iy<height;iy++)
	{
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		tx=(unsigned int) source[i++];				// type of line
		ax=(unsigned int) source[i++];				// type of line
		
		dx+=x;

		if(tx<2)
		{
		 	screen_clip_line(source+i,dy,sx+dx,y+iy+sy,lx+(tx<<8));
		}
		else
		{
			j=i;
			while(tx>0)
			{		
				lx=(unsigned int) source[j++];				// length of line
			 	screen_clip_line(source+j,dy,sx+dx,y+iy+sy,lx+256);
				
				j+=lx;
				dx+=lx;
				dx+=(unsigned int) source[j++];				// x distance to first pixel

				tx--;
			}
		}																  
		dy+=SVGA_logical_length;
		i+=ax;
	}
	
	}
	else
	{
	for(;iy<height;iy++)
	{
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		tx=(unsigned int) source[i++];				// type of line
		ax=(unsigned int) source[i++];				// type of line
		
		dx+=x;

		if(tx<2)
		{
			if( dx>=clip_start_x) ix=0;					// left clip?
			else ix=clip_start_x-dx;

			if( (dx+lx)<viewable_width) dlx=lx;				// right clip?
		  	else dlx=viewable_width-dx;

			if(ix<dlx)
			 	screen_clip_line(source+i+ix,dy,sx+ix+dx,y+iy+sy,(dlx-ix)+(tx<<8));
		}
		else
		{
			j=i;
			while(tx>0)
			{		
				lx=(unsigned int) source[j++];				// length of line
				if( dx>=clip_start_x) ix=0;					// left clip?
				else ix=clip_start_x-dx;

				if( (dx+lx)<viewable_width) dlx=lx;				// right clip?
		   	else dlx=viewable_width-dx;

				if(ix<dlx)
				 	screen_clip_line(source+j+ix,dy,sx+ix+dx,y+iy+sy,(dlx-ix)+256);
				
				j+=lx;
				dx+=lx;
				dx+=(unsigned int) source[j++];				// x distance to first pixel

				tx--;
			}
		}																  
		dy+=SVGA_logical_length;
		i+=ax;
	}
	}
}

// This slows the game down a lot...
// Clipped sprites should be used sparingly

long screen_clip_point(long x,long y)
{
	long 	x1,x2;
	long	index;
	
	index=edge_offs[y];
	do
	{
		x1=edge_array[index++];
		x2=edge_array[index++];
		if( x<x1) break;					// finished?

		if( x>x1 && x<x2)	break;
	}
	while(x2<iff_width);

	return ((x2<<16)+x1);
}

void screen_clip_line(char *source,long dest,long x,long y,long length)
{
	long	xa,xb;
	long 	x1,x2;
	long	index;
	long	line_type;
	
	line_type=length&0xff00;
	length&=0xff;

	index=edge_offs[y];

	xa=x;
	xb=xa+length;

	do
	{
		x1=edge_array[index++];
		x2=edge_array[index++];
		if( xb<x1) break;					// finished?

		if( xb>x1 && xa<x2)				// line viewable?
		{
			if(xa<x1)						// left clip?
			{								
				length-=x1-xa;
				source+=x1-xa;
				xa=x1;
			}
			if(xb>x2) length-=xb-x2;	// right clip?

			if(length>0) asm_add_andor_line(source,dest+xa,length+line_type);
			length=xb-xa;
		}
	}
	while(x2<iff_width);
}


/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Make Collision Edge List */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */
// This takes far too long...
// Could put it in MAKECOLL

flag collision_at(long x,long y)
{
	unsigned char mask;
	unsigned char bit;

	mask=coll_buf[( (x+y*iff_width)>>3 )];

	bit=1<<(7-(x&7));
	
	if( (mask&bit)!=0 ) return TRUE;
	
	return FALSE;
}

long find_floor(long x,long y,long max_scan)
{
	long height;
	
	height=0;
	if( collision_at(x,y)==TRUE )
	{
		// scan up
		while(max_scan>0)
		{
			height--;
			if( collision_at(x,y+height)==FALSE ) break;
			max_scan--;
		}
	}
	else
	{
		// scan down
		while(max_scan>0)
		{
			height++;
			if( collision_at(x,y+height)==TRUE ) break;
			max_scan--;
		}
		height--;
	}
	return height;
}

void make_edge_array(void )
{
	long x,y;
	long ptr;

	ptr=0;

	//printf("X=%i\n",iff_width);
	//printf("Y=%i\n",iff_height);
	for(y=0;y<iff_height;y++)
	{
		//printf("Y=%i\n",y);
		edge_offs[y]=ptr;
		x=1;
		while(x<iff_width)
		{
			while( (x<iff_width) && (collision_at(x,y)==TRUE) ) x++;
			edge_array[ptr++]=x;
			//printf("%i,",x);
			while( (x<iff_width) && (collision_at(x,y)==FALSE) ) x++;
			edge_array[ptr++]=x;
			//printf("%i,,",x);
		}
		if( ptr>(8*4096) )
		{
			internal_message("EDGE ARRAY FULL\n");
			break;
		}
	}
	sprintf(message,"Array Size %i\n",ptr);
	internal_message(message);
}


void make_block_collision_array(void )
{
	long x,y;
	long ix,iy;
	long i;
	char block;

	collision_block_array=heap_malloc(256*256);

//	mode13();

	i=0;

	for(y=0;y<iff_height;y+=16)
	{
		for(x=0;x<iff_width;x+=16)
		{
			block=0;
			for(iy=0;iy<16;iy++)
			{
				for(ix=0;ix<16;ix+=8)
				{
					if(coll_buf[((x+ix)+(y+iy)*iff_width)>>3]!=0) block=-1;
				}
			}
			collision_block_array[i++]=block;
		}
	}
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Create Box */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void create_box(sprite_sequence *seq,long width,long height)
{
	unsigned char *source;
	short *sourcew;
	long  i,x,y;
	unsigned char c;

	seq->frames=2;
	seq->map=(long *) malloc( 8 );
	seq->data=malloc( (width+4)*height +	8);

	seq->map[0]=0;
	seq->map[1]=-1;
	
	source=seq->data;
	sourcew=(short *) source;

	i=0;

	sourcew[0]=0;	
	sourcew[1]=0;	
	sourcew[2]=width;
	sourcew[3]=height;

	i+=8;
	
	for(y=0;y<height;y++)
	{
		source[i++]=0;			//dx
		source[i++]=width;	//lx
		source[i++]=0;			//tx
		source[i++]=width;	//ax

		for(x=0;x<width;x++)
		{
			source[i++]=255;
		}
	}

	// Clear Box

	i=8;

	for(y=0;y<height;y++)
	{
		i++;						//dx
		width=source[i++];	//lx
		i++;						//tx
		i++;
		for(x=0;x<width;x++)
		{
			c=0;
			if(y==0) c=6*16;
			if(y==(height-1)) c=6*16;
			if(x==0) c=6*16;
			if(x==(width-1) ) c=6*16;
			source[i++]=c;
		}
	}
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Clear sprite box */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void clear_box(sprite_sequence *seq)
{
	unsigned char *source;
	short *sourcew;
	long  i,x,y;
	long	width,height,ax;
	unsigned char c;

	i=seq->map[0];
	source=seq->data;
	sourcew=(short *) (source+i);

	width=(long ) sourcew[2];
	height=(long ) sourcew[3];

	i+=8;

	for(y=0;y<height;y++)
	{
		i++;						//dx
		width=source[i++];	//lx
		i++;						//tx
		ax=source[i++];
		for(x=0;x<width;x++)
		{
			c=0;
			if(y==0) c=6*16;
			if(y==(height-1)) c=6*16;
			if(x==0) c=6*16;
			if(x==(width-1) ) c=6*16;
			source[i+x]=c;
		}
		i+=ax;
	}
}

void clear_box_region(sprite_sequence *seq,long start_x,long start_y,long end_x,long end_y)
{
	unsigned char *source;
	short *sourcew;
	long  i,y;
	long	width,height,ax;

	i=seq->map[0];
	source=seq->data;
	sourcew=(short *) (source+i);

	width=(long ) sourcew[2];
	height=(long ) sourcew[3];

	i+=8;

	for(y=0;y<height;y++)
	{
		i++;						//dx
		width=source[i++];	//lx
		i++;						//tx
		ax=source[i++];		//ax
		if(ax==width)
		{
			if(y>=start_y && y<end_y)
			{
				memset(source+i+start_x,0,end_x-start_x);
				/*
				for(x=0;x<width;x++)
				{
				
					c=0;
					if(x>=start_x) c++;
					if(x <end_x) c++;
					if(c==2) source[i+x]=0;
				}
				*/
			}
		}
		i+=ax;
	}
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Draw Pixel to Sprite */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void draw_pixel_to_sprite(sprite_sequence seq,long x,long y,char c)
{
	unsigned char *source;
	short *sourcew;
	long	i,iy,width,height;
	int	dx,lx,tx,ax;
		
//	i=seq.map;
	i=0;
	source=seq.data;
	sourcew=(short *) (source+i);

	width=(int ) sourcew[2];
	height=(int ) sourcew[3];

	i+=8;

	if(x>=width ) return;
	if(y>=height) return;

	for(iy=0;iy<y;iy++)
	{
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		tx=(unsigned int) source[i++];				// type of line
		ax=(unsigned int) source[i++];				// data length of line
		i+=ax;
	}
	dx=(unsigned int) source[i++];				// x distance to first pixel
	lx=(unsigned int) source[i++];				// length of line
	tx=(unsigned int) source[i++];				// type of line
	ax=(unsigned int) source[i++];				// data length of line

	source[i+x]=c;

}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Get 4 coordiantes of sprite */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void get_sprites_coordinates(long i,long *x1,long *y1,long *x2,long *y2)
{
	char 	*source;
	short *sourcew;
	int ptr;
	short width,height;
	int x,y;
	int name,frame;
	
  	name=sprites[i].name;
  	frame=sprites[i].frame;
//	printf("%i Name %i Frame %i\n",i,name,frame);
	
  	ptr=seqdata[name].map[frame];
  	source=seqdata[name].data;
	sourcew=(short *) (source+ptr);

	x=(int ) sourcew[0];
	y=(int ) sourcew[1];
	width =	sourcew[2];
	height=	sourcew[3];

	x+=sprites[i].x;
	y+=sprites[i].y;

  	*x1=x;
  	*y1=y;
	*x2=x+width;
	*y2=y+height;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Find nearest sprite (to given x,y) */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */
// Does not handle massively offset sprites

long find_nearest_sprite(long x,long y)
{
	long i;
	long dx,dy,dist;
	long best,best_dist;	

	best=-1;
	best_dist=(320*320)+(240*240);
	for(i=0;i<MAX_SPRITES;i++)
	{
		if( (sprites[i].on==TRUE) && (sprites[i].shootable==YES) )
		{
			dx=sprites[i].x-x;
			dy=sprites[i].y-y;
			dist=(dx*dx)+(dy*dy);
			if(dist<best_dist)
			{
				best_dist=dist;
				best=i;
			}
		}
	}
	return best;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* End of file */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */


