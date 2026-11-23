void screen_clip_line(char *source,long dest,long x,long y,long length);

void clip_sprite(sprite_sequence seq,int sprite_no,
						 int x,int y,int sx,int sy,int screen)
{
	unsigned char *source,*dest;
	short *sourcew;
	int i=0;
	int ix,iy;
	int dx,lx,dlx;
	int width,height;
	int dy;

	if ( safe_mode==YES && bursting==YES ) return;

	x-=sx;
	y-=sy;
	if( x>clip_end_x ) return;
	if( y>clip_end_y ) return;

	i=seq.map[sprite_no];
	source=seq.data;
	sourcew=(short *) (source+i);

	x+=(int ) sourcew[0];
	y+=(int ) sourcew[1];
	width=(int ) sourcew[2];
	height=(int ) sourcew[3];
	i+=8;

	if( (y+height)<clip_start_y ) return;
	if( (x+width )<clip_start_x ) return;

	if(safe_mode==YES) dy=(y+screen)*640-sx;		// y screen ptr
	else dy=(y+y_mod[sy]+screen)*640;	// y screen ptr
	
	if( (y+height)>clip_end_y ) height=clip_end_y-y;	// bottom clip

	for(iy=0;iy<height;iy++)
	{
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line

		if( (y+iy)>=clip_start_y )						// top clip?
		{
			dx+=x;

			if( dx>=clip_start_x) ix=0;					// left clip?
			else ix=clip_start_x-dx;

			if( (dx+lx)<clip_end_x) dlx=lx;				// right clip?
		   else dlx=clip_end_x-dx;

			if(ix<dlx) screen_clip_line(source+i+ix,dy,sx+ix+dx,y+iy+sy,dlx-ix);
		}																  
		dy+=640;
		i+=lx;
	}
}

void screen_clip_line(char *source,long dest,long x,long y,long length)
{
	long	xa,xb;
	long 	x1,x2;
	long	index;

	if(y<0) return;
	if(y>=iff_height) return;
	if(x<0) return;
	if(x>iff_width) return;

	index=edge_offs[y];

	xa=x;
	xb=xa+length;

	x1=0;
	while(x1<iff_width)
	{
		x1=edge_array[index++];
		x2=edge_array[index++];
		if( xb<x1) break;			// finished?

		if( xb>x1 && xa<x2)		// line viewable?
		{
			if(xa<x1)				// left clip?
			{
				length-=x1-xa;
				source+=x1-xa;
				xa=x1;
			}
			if(xb>x2)				// right clip?
			{
				length-=xb-x2;
			}
			if(length>0)			// draw it
			{
				x=xa;
				//screen_set_line(dest+x,length,255);
				screen_andor_line(source,dest+x,length);
				//source+=length;
			}
		length=xb-xa;
		}
	}
}
