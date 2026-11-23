/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Mike here's my simple sprite draw for my format... Phil */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

typedef struct {
	int	*map;
	char	*data;
} sprite_sequence ;

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* FLIC style sprite draw routine */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void draw_sprite(sprite_sequence seq,int sprite_no,int x,int y,int sx,int sy,int screen)
{
	unsigned char *source;
	short *sourcew;
	int i;
	int ix,iy;
	int dx,lx,dlx;
	int disp;
	int width,height;
	int dy;

	if( (x-sx)>clip_end_x ) return;
	if( (y-sy)>clip_end_y ) return;
	y-=sy;

	i=seq.map[sprite_no];
	source=seq.data;
	sourcew=(short *) (source+i);

	x+=(int ) sourcew[0];								// x offset 
	y+=(int ) sourcew[1];								// y offset
	width=(int ) sourcew[2];							// yes it's the width
	height=(int ) sourcew[3];							// yes it's the height

	i+=8;

	if(  (y+height)<clip_start_y ) return;
	if( (x-sx+width)<clip_start_x ) return;

	for(iy=0;iy<height;iy++)
	{
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line

		if(((y+iy)>=clip_start_y) && ((y+iy)<clip_end_y))	// y clip?
		{
			if(safe_mode==YES)
			{
				dy=(y+iy+screen)*640;		// y screen ptr
				disp=x+dx+dy-sx;				// +x
			}
			else
			{
				dy=(y+iy+y_mod[sy]+screen)*640;		// y screen ptr
				disp=x+dx+dy;									// +x
			}
			
			if((x-sx+dx)>=clip_start_x) ix=0;					// left clip?
			else ix=clip_start_x-(x-sx+dx);

			if((x-sx+dx+lx)<clip_end_x) dlx=lx;				// right clip?
		   	else dlx=clip_end_x-(x-sx+dx);

			#ifdef VIEW_COLLISION
			if(ix<dlx) screen_set_line(ix+disp,dlx-ix,255);
			#else
			if(ix<dlx) screen_andor_line(source+i+ix,ix+disp,dlx-ix);
			#endif
		}
		i+=lx;
	}
}
