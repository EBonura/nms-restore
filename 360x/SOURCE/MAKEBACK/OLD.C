flag sprite_collision(sprite_sequence seq,int sprite_no,int x,int y)
{
	unsigned char  *source;
	short *sourcew;
	unsigned char mask;
	unsigned char bit;
	int i;
	int iy;
	int dx,lx;
	int disp;
	int width,height;					

//	return FALSE;

	i=seq.map[sprite_no];
	source=seq.data;
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
		i+=lx;
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















/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* OLD AND SHIT SPRITE PROCESS */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void old_process_sprites()
{
	int i,x,y,x8,y8;
	int name,frame;
	int action;
	int temp;
	for(i=0;i<MAX_SPRITES;i++)
	{
		if(sprites[i].on==TRUE)
		{
			name=sprites[i].name;
			x=sprites[i].x;
			y=sprites[i].y;
			x8=sprites[i].x8;
			y8=sprites[i].y8;
			frame=sprites[i].frame;
			action=sprites[i].action;

			switch(action)
			{

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Do nothing */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_NONE:
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Play Animation Once*/
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_PLAYONCE:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=4;
						 	frame++;
							if(seqdata[name].map[frame]==-1)
							{
					 			sprites[i].dying=2;
					 			sprites[i].action=ACTION_DYING;
	     					   frame=0;
							}
							sprites[i].frame=frame;
						}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Cycle Animation */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_CYCLE:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=4;
						 	frame++;
							if(frame==sprites[i].endframe) frame=sprites[i].startframe;
							sprites[i].frame=frame;
						}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Spiral Animation */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_SPIRAL:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=4;
						 	frame++;
							if(seqdata[name].map[frame+1]==-1) frame=0;
							sprites[i].frame=frame;
						}
						break;


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Random Delay on first frame Cycle Animation */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_RND_CYCLE:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=4;
						 	frame++;
							if(seqdata[name].map[frame]==-1)
							{
								frame=0;
								sprites[i].framecounter=8+(rand()*64)>>15;
							}
							sprites[i].frame=frame;
						}
						break;
		
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Ping Pong Animation */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_PINGPONG:
						temp=1;
						
						if( sprites[i].framecounter>0)
						{ sprites[i].framecounter--; temp=1; }

						if( sprites[i].framecounter<0)
						{ sprites[i].framecounter++; temp=-1; }

						if ( sprites[i].framecounter==0 )
						{
						 	frame+=temp;
							//if(frame<1)
							if(frame<sprites[i].startframe)
							{
								frame=sprites[i].startframe;
								temp=temp*-1;
							}
							//if(seqdata[name].map[frame+1]==-1)
							if(frame>=sprites[i].endframe)
							{
								frame--;
								temp=temp*-1;
							}
							sprites[i].framecounter=4*temp;
							sprites[i].frame=frame;
						}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Fuel Wait to be picked up */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_PICKUP_FUEL:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=4;
						 	frame++;
							if(seqdata[name].map[frame+1]==-1) frame=0;
							sprites[i].frame=frame;
						}
					 	if(collided_with_craft(name,x,y,frame)==TRUE)
				 		{
				 			sprites[i].dying=2;
				 			sprites[i].action=ACTION_DYING;
				 			ships_fuel_add=64;
				 		}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Shield Wait to be picked up */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_PICKUP_SHIELD:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=4;
						 	frame++;
							if(seqdata[name].map[frame+1]==-1) frame=0;
							sprites[i].frame=frame;
						}
					 	if(collided_with_craft(name,x,y,frame)==TRUE)
				 		{
				 			sprites[i].dying=2;
				 			sprites[i].action=ACTION_DYING;
				 			ships_shield_add=64;
				 		}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Shield Wait to be picked up */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_PICKUP_GRAVITY:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=4;
						 	frame++;
							if(seqdata[name].map[frame+1]==-1) frame=0;
							sprites[i].frame=frame;
						}
					 	if(collided_with_craft(name,x,y,frame)==TRUE)
				 		{
				 			sprites[i].dying=2;
				 			sprites[i].action=ACTION_DYING;
				 			ships_gravity=2048;
				 		}
						break;


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Pod Wait for ship inrange */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_POD_WAIT:
						temp=sqr_root( ((x-craftx)*(x-craftx))
										  +((y-crafty)*(y-crafty)) );
						if(temp<50) 
						{
							sprites[i].lastaction=action;
							sprites[i].action=ACTION_PICKUP_POD;
							sprites[i].flashing=64;
						}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Pod Pick up */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Shit or what

				case ACTION_PICKUP_POD:
						temp=sqr_root( ((x-craftx)*(x-craftx))
										  +((y-crafty)*(y-crafty)) );
						if(temp>50)
						{
							sprites[i].action=sprites[i].lastaction;
							sprites[i].visible=TRUE;
						}
						else
						{
							if( (sprites[i].flashing&3)==0)
							{
								if(sprites[i].visible==TRUE) sprites[i].visible=FALSE;
								else sprites[i].visible=TRUE;
							}
							sprites[i].flashing--;
						}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Pod Fall */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_FALL:
						x8=x8+sprites[i].dx;
						y8=y8+sprites[i].dy;
						sprites[i].dy+=1024;
						x=x8>>16;
						y=y8>>16;
						if(sprite_collision(seqdata[(sprites[i].name)],
													frame,x,y)==FALSE)
						{
							sprites[i].x=x;
							sprites[i].y=y;
							sprites[i].x8=x8;
							sprites[i].y8=y8;
						}
						else
						{
						 	sprites[i].action=ACTION_POD_WAIT;
						}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Gravity (for Catherine Wheel) */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_GRAVITY:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=2;
						 	frame++;
							if(seqdata[name].map[frame+1]==-1) frame=0;
							sprites[i].frame=frame;
						}
						x8=x8+sprites[i].dx;
						y8=y8+sprites[i].dy;
						sprites[i].dy+=2048;
						x=x8>>16;
						y=y8>>16;
						if(sprite_collision(seqdata[(sprites[i].name)],
													frame,x,y)==FALSE)
						{
							sprites[i].x=x;
							sprites[i].y=y;
							sprites[i].x8=x8;
							sprites[i].y8=y8;
						}
						else
						{
						 	sprites[i].action=ACTION_FLASH;
							sprites[i].flashing=32;
						 	sprites[i].lastaction=ACTION_DYING;
							sprites[i].dying=2;
						}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Home in on ship... */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_HOME:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=4;
						 	frame++;
							if(seqdata[name].map[frame]==-1)
							{
								frame=0;
								sprites[i].framecounter=8+(rand()*64)>>15;
							}
							sprites[i].frame=frame;
						}
						if(sprites[i].frame>2 && sprites[i].framecounter&1==1)
						{
							if(sprites[i].x<craftx) x++;
							if(sprites[i].x>craftx) x--;	
							if(sprites[i].y<crafty) y++;
							if(sprites[i].y>crafty) y--;
							if(sprite_collision(seqdata[(sprites[i].name)],frame,x,y)==FALSE)
							{
								sprites[i].x=x;
								sprites[i].y=y;
							}
						}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Float in on ship... */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Should bounce off each other

				case ACTION_ASTEROID:
						if(sprites[i].hit==TRUE)
						{
				 			sprites[i].dying=2;
				 			sprites[i].action=ACTION_DYING;
							switch (name)
							{
								case NAME_RCK1:
									add_sprite(NAME_RCK2,ACTION_ASTEROID,sprites[i].x8/256,sprites[i].y8/256);
									add_sprite(NAME_RCK2,ACTION_ASTEROID,sprites[i].x8/256,sprites[i].y8/256);
									break;
								case NAME_RCK2:
									add_sprite(NAME_RCK3,ACTION_ASTEROID,sprites[i].x8/256,sprites[i].y8/256);
									add_sprite(NAME_RCK3,ACTION_ASTEROID,sprites[i].x8/256,sprites[i].y8/256);
									add_sprite(NAME_RCK3,ACTION_ASTEROID,sprites[i].x8/256,sprites[i].y8/256);
									break;
							}
						 	break;
						}

						// Initialiser
						if(sprites[i].dx==0 && sprites[i].dy==0)
						{
							temp=sprites[i].dir;
							sprites[i].dx=bounce_x[temp]*128-(rand()&31);
							sprites[i].dy=bounce_y[temp]*128-(rand()&31);
						}
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=2;
						 	frame++;
							if(seqdata[name].map[frame+1]==-1) frame=0;
							sprites[i].frame=frame;
						}
					  	x8+=sprites[i].dx;
					  	y8+=sprites[i].dy;
					  	x=x8+sprites[i].dx;
					  	y=y8+sprites[i].dy;
					  	if(sprite_collision(seqdata[(sprites[i].name)],frame,x/256,y/256)==TRUE)
					  	{
					  		temp=sprites[i].dir;
					  		temp++; temp&=7;
					  		sprites[i].dir=temp;
					  		sprites[i].dx=bounce_x[temp]*192-(rand()&127);
					  		sprites[i].dy=bounce_y[temp]*192-(rand()&127);
					  	}
					  	else
					  	{
					  		sprites[i].x8=x8;
					  		sprites[i].y8=y8;
					  	}
						sprites[i].x=sprites[i].x8/256;
						sprites[i].y=sprites[i].y8/256;
						break;


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Float in on ship... */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_FLOAT:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=2;
						 	frame++;
							if(seqdata[name].map[frame]==-1)
							{
								frame=0;
								sprites[i].framecounter=8+(rand()*64)>>15;
							}
							sprites[i].frame=frame;
						}
						if(sprites[i].frame>2 && sprites[i].framecounter&1==1)
						{
							if(sprites[i].x<craftx) x++;
							if(sprites[i].x>craftx) x--;	
							if(sprites[i].y<crafty) y++;
							if(sprites[i].y>crafty) y--;
							if(sprite_collision(seqdata[(sprites[i].name)],frame,x,y)==FALSE)
							{
								sprites[i].x=x;
								sprites[i].y=y;
							}
						}
						if(frame==50)
						{
							// Lets get some catherine wheels out...

							temp=add_sprite(NAME_SPIRAL,ACTION_GRAVITY,x,y);
							if(temp==-1) break;
							sprites[temp].x8=x<<16;
							sprites[temp].y8=y<<16;
							sprites[temp].dx=(16384-rand())*8;
							sprites[temp].dy=-rand()*4;
						}
						// would be better if they were more predictable...
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Home in on ship... */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Could do with a velocity,acceleration + acceleration of acceleration
// i.e 1,.5,-.1 to make it speed up and then slow down

				case ACTION_HOMELINE:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=4;
						 	frame++;
							if(seqdata[name].map[frame]==-1)
							{
								frame=0;
								sprites[i].framecounter=8+(rand()*64)>>15;
								get_angle_to_craft(i);
							}
							sprites[i].frame=frame;
						}
						if(sprites[i].frame>2 )
						{
							x8+=(sprites[i].cos);
							y8+=(sprites[i].sin);
							x=x8/256;
							y=y8/256;

							if(sprite_collision(seqdata[(sprites[i].name)],frame,x,y)==FALSE)
							{
								sprites[i].x=x;
								sprites[i].y=y;
								sprites[i].x8=x8;
								sprites[i].y8=y8;
							}
						}
						break;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* FLASH */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_FLASH:
						sprites[i].hit=TRUE;
						if( (sprites[i].flashing&3)==0)
						{
							if(sprites[i].visible==TRUE) sprites[i].visible=FALSE;
							else sprites[i].visible=TRUE;
						}
						sprites[i].flashing--;
						if(sprites[i].flashing<=0) 
						{
							sprites[i].action=sprites[i].lastaction;
							sprites[i].visible=TRUE;
						}
						break;
					
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Dying (removeing from both buffers) */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

				case ACTION_DYING:
						sprites[i].dying--;
						if(sprites[i].dying==0) sprites[i].on=FALSE;
						break;
			}
			// won't work in fast mode if scroll wraps

			if( sprites[i].buf_rem[buf]==YES && safe_mode==YES)
			{

			if( (sprites[i].buf_x[buf]==x) &&
				 (sprites[i].buf_y[buf]==y) &&
				 (sprites[i].buf_frame[buf]==frame) ) sprites[i].buf_rem[buf]=NO;
			else sprites[i].buf_rem[buf]=YES;

			//sprites[i].buf_rem[buf]=YES;
			}

		}
	}
}
