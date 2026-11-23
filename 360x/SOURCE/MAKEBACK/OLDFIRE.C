ADDFIRE

		/*
		for(i=0;i<MAX_FIRE;i++)
		{
			if(fire_on[i]==0)
			{
				fire_on[i]=1;
				fire_frame[i]=frame;
				fire_x[i]=x*65536;
				fire_y[i]=y*65536;
				fire_bx[0][i]=x;
				fire_by[0][i]=y;
				fire_bx[1][i]=x;
				fire_by[1][i]=y;
				fire_dx[i]=dx;
				fire_dy[i]=dy;
				break;
			}
		}
		*/

MOVEFIRE

#if 0

	for(i=0;i<MAX_FIRE;i++)
	{
		if(fire_on[i]>0)
		{
			f=fire_frame[i];
			count++;
			x=fire_x[i];
			y=fire_y[i];
			x+=fire_dx[i];
			y+=fire_dy[i];
			fire_dy[i]+=1024;
			fire_x[i]=x;
			fire_y[i]=y;
			x=x/65536;
			y=y/65536;
			fire_bx[buf][i]=x;
			fire_by[buf][i]=y;

			if(sprite_collision(seqdata[NAME_FIRE01],fire_frame[i],
								fire_bx[buf][i],fire_by[buf][i])==TRUE
			   || x<0 || x>=iff_width
			   || y<0 || y>=iff_height )
			{
			 	fire_on[i]=-2;

//				add_sprite(NAME_EXPLOSION,ACTION_PLAYONCE,x+32,y+16);
				for(j=0;j<8;j++)
				{
				rand_factor=(rand()*16)>>15;
				temp=((f-16+29+rand_factor)&63)*((pi*2)/64);
				tdx=cos(temp)*1.5;
				tdy=sin(temp)*1.5;
	
				add_particle(x+tdx,y+tdy,(int) (tdx*65536),(int) (tdy*65536));
				}
			}
		}
		else if(fire_on[i]<0) fire_on[i]++;
	}

#endif



void draw_fire(int buf)
{
//	int i,count=0;
	return;

#if 0

	for(i=0;i<MAX_FIRE;i++)
	{
		if(fire_on[i]>0)
		{
			fire_frame[i]=0;
			/*
			lines[0].line_on=0;
			lines[0].x1[buf]=fire_bx[buf][i];
			lines[0].y1[buf]=fire_by[buf][i];
			lines[0].x2[buf]=fire_bx[buf][i]+(fire_dx[i]*8)/65536;
			lines[0].y2[buf]=fire_by[buf][i]+(fire_dy[i]*8)/65536;
			*/
			/*
			draw_sprite(seqdata[NAME_FIRE01],fire_frame[i],
						fire_bx[buf][i],fire_by[buf][i],
						bufx[buf],bufy[buf],logical_screen+split_height);
			*/
			draw_sprite(seqdata[NAME_EBALL],fire_frame[i],
							fire_bx[buf][i],fire_by[buf][i],
							bufx[buf],bufy[buf],logical_screen+split_height);
			count++;
		}
	}

#endif
}

void remove_fire(int buf)
{
//	int i,count=0;
	return;

#if 0

	for(i=0;i<MAX_FIRE;i++)
	{
		if(fire_on[i]!=0)
		{
			fire_frame[i]=0;
			/*
			remove_sprite(seqdata[NAME_FIRE01],fire_frame[i],
							  fire_bx[buf][i],fire_by[buf][i],
							  bufx[buf],bufy[buf],logical_screen+split_height);
			*/
			remove_sprite(seqdata[NAME_EBALL],fire_frame[i],
							  fire_bx[buf][i],fire_by[buf][i],
							  bufx[buf],bufy[buf],logical_screen+split_height);
			count++;
		}
	}

#endif

}


