	for(iy=0;iy<height;iy++)
	{
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line

		if(((y+iy)>=clip_start_y) && ((y+iy)<clip_end_y))	// y clip?
		{
			dx+=x;
			if(safe_mode==YES)
			{
				dy=(y+iy+screen);						// y screen ptr
			}
			else
			{
				dy=(y+iy+y_mod[sy]+screen);		// y screen ptr
			}
			
			if( dx>=clip_start_x) ix=0;					// left clip?
			else ix=clip_start_x-dx;

			if( (dx+lx)<clip_end_x) dlx=lx;				// right clip?
		   	else dlx=clip_end_x-dx;

			#ifdef VIEW_COLLISION
			if(ix<dlx) screen_set_line(ix+dx+dy*640,dlx-ix,255);
			#else
			if(ix<dlx) screen_andor_line(source+i+ix,ix+dx+dy*640,dlx-ix);
			#endif
		}
		i+=lx;
	}

  	dy=y;
	sdisp=(screen+y_mod[sy]+y)*640;
	for(iy=0;iy<height;iy++)
	{					
		dx=(int) source[i++];
		lx=(int) source[i++];
		i+=lx;		
		if(((y+iy)>=clip_start_y) && ((y+iy)<clip_end_y))
		{
			if((x-sx+dx)>=clip_start_x) ix=0;
			else ix=clip_start_x-(x-sx+dx);

			if((x-sx+dx+lx)<clip_end_x) dlx=lx;
		   	else dlx=clip_end_x-(x-sx+dx);
					  
			disp=ix+x+dx;
			dest=dump_blk_buf+disp+(y_mod640[dy+sy]);
			if(ix<dlx) screen_dump_line(dest,sdisp+disp,dlx);
		}
		sdisp+=640;
		dy++;
	}


flag process_collision_with_craft()
{
	char *gfxdata;
	int ptr;
	short width,height;
	short dx,dy;
	int i,j,x,y;
	int name,frame;
	
	for(i=0;i<MAX_SPRITES;i++)
 	{
		if( sprites[i].on==TRUE
			 && sprites[i].name!=NAME_EXPLOSION
			 && sprites[i].action!=ACTION_FLASH)
		{		
			name=sprites[i].name;
			x=sprites[i].x;
			y=sprites[i].y;
			frame=sprites[i].frame;
	
			ptr=seqdata[name].map[frame];
			gfxdata=seqdata[name].data;
			dx=gfxdata[ptr+0]+(gfxdata[ptr+1]<<8);
			dy=gfxdata[ptr+2]+(gfxdata[ptr+3]<<8);
			x+=dx;
			y+=dy;
			ptr+=4;
			width=gfxdata[ptr+0]+(gfxdata[ptr+1]<<8);
			height=gfxdata[ptr+2]+(gfxdata[ptr+3]<<8);

			if(craftx>=x && craftx<(x+width) 
						&& crafty>=y && crafty<(y+height) )
					return TRUE;
		}
	}
	return FALSE;
}

void remove_sprite(sprite_sequence seq,int sprite_no,int x,int y,int sx,int sy,int screen)
{
	char *source,*dest;
	int i=0;
	short offset_x,offset_y;
	int ix,iy;
	int dx,lx,dlx;
	int disp,sdisp;
	int width,height;
	int dy;
	char c;

	if(safe_mode==YES) return;

	y-=sy;

	i=seq.map[sprite_no];
	source=seq.data;

	offset_x=source[i+0]+(source[i+1]<<8);
	offset_y=source[i+2]+(source[i+3]<<8);
	x+=(int ) offset_x;
	y+=(int ) offset_y;
	i+=4;
	width=source[i+0]+(source[i+1]<<8);
	height=source[i+2]+(source[i+3]<<8);
	i+=4;

  	dy=y;
	for(iy=0;iy<height;iy++)
	{
		dx=(int) source[i++];
		lx=(int) source[i++];
		i+=lx;
		if(((y+iy)>=clip_start_y) && ((y+iy)<clip_end_y))
		{
			disp=x+dx;
			sdisp=(dy+sy);
			disp+=(screen+y_mod[sy]+dy)*640;

			if((x-sx+dx)>=clip_start_x) ix=0;
			else ix=clip_start_x-(x-sx+dx);

			if((x-sx+dx+lx)<clip_end_x) dlx=lx;
		   	else dlx=clip_end_x-(x-sx+dx);
												  
			dest=dump_blk_buf+ix+x+dx+(y_mod640[dy+sy]);
			if(ix<dlx) screen_dump_line(dest,ix+disp,dlx);
			
		}
		dy++;
	}
}


