
void	process_sprites(void)
{
	volatile	flag end_slice;
	volatile	long i;
	volatile	long *logic;
	volatile	long logic_index;
	volatile	long command;
	volatile	long temp1,temp2,temp3,temp4;

	for(i=0;i<MAX_SPRITES;i++)
	{
		if(sprites[i].on==TRUE)
		{
			end_slice=FALSE;
			logic=ai_data; //sprites[i].move_seg;
			logic_index=sprites[i].move_ptr;

			while(end_slice==FALSE)
			{
				command=logic[logic_index++];
				switch(command)
				{
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_WAIT:
						logic_index--;
						end_slice=TRUE;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_STOP:
						end_slice=TRUE;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_JMP:
						logic_index=logic[logic_index++];
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_CALL:
						temp1=logic[logic_index++];
						sprites[i].ret_ptr=logic_index;
						logic_index=temp1;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_RETURN:
						logic_index=sprites[i].ret_ptr;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_THREAD:
						sprites[i].thread_ptr=logic[logic_index++];
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_MOVERND:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						sprites[i].obj[temp2]=(rand()*temp1)>>15;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_MOVEI:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						sprites[i].obj[temp2]=temp1;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_MOVEV:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						sprites[i].obj[temp2]=sprites[i].obj[temp1];
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_ADDI:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						sprites[i].obj[temp2]+=temp1;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_ADDV:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						sprites[i].obj[temp2]+=sprites[i].obj[temp1];
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_SUBI:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						sprites[i].obj[temp2]-=temp1;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_SUBV:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						sprites[i].obj[temp2]-=sprites[i].obj[temp1];
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_INDEX_ARRAY:
						temp1=logic[logic_index++];			// var index
						temp1=sprites[i].obj[temp1];
						temp2=logic[logic_index++];			// array
						if(temp1>logic[temp2+1]) temp1=0;	// validate
						temp1=logic[temp2+temp1+2];
						temp2=logic[logic_index++];			// var dest
						sprites[i].obj[temp2]=temp1;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_SHL:
						temp1=logic[logic_index++];		// shift
						temp2=logic[logic_index++];		// shift var source
						temp1=sprites[i].obj[temp2]<<temp1;				 
						temp2=logic_index++;			 		// shift var dest
						sprites[i].obj[temp2]=temp1;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_SHR:
						temp1=logic[logic_index++];		// shift
						temp2=logic[logic_index++];		// shift var source
						temp1=sprites[i].obj[temp2]>>temp1;
						temp2=logic_index++;			 		// shift var dest
						sprites[i].obj[temp2]=temp1;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_ADD_CHILD:
						temp1=logic[logic_index++];		// sprites name
						temp2=logic[logic_index++];		// sprites pc
						sprites[i].child=add_sprite(temp1,ACTION_CYCLE,
												sprites[i].x,sprites[i].y);
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_IFEQI:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						if( sprites[i].obj[temp2] == temp1)
								logic_index=logic[logic_index++];
						else	logic_index++;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_IFEQV:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						if( sprites[i].obj[temp2] == sprites[i].obj[temp1])
								logic_index=logic[logic_index++];
						else	logic_index++;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_IFNEI:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						if( sprites[i].obj[temp2] != temp1)
								logic_index=logic[logic_index];
						else	logic_index++;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_IFNEV:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						if( sprites[i].obj[temp2] != sprites[i].obj[temp1])
								logic_index=logic[logic_index++];
						else	logic_index++;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_FRAMEINC:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=sprites[i].anim_delay_rate;
						 	temp1=sprites[i].frame;
							temp1++;
							if( temp1>=sprites[i].endframe ) temp1=sprites[i].startframe;
							sprites[i].frame=temp1;
						}
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_UPDATE_XY:
						sprites[i].x=sprites[i].obj[obj_x];
						sprites[i].y=sprites[i].obj[obj_y];
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_ANIMPLAYFORWARDONCE:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=sprites[i].anim_delay_rate;
						 	sprites[i].frame++;
						}

						if(sprites[i].frame<=(sprites[i].endframe-1))
						{ 
							logic_index--;
							end_slice=TRUE;
						}

						if(sprites[i].frame<sprites[i].startframe)
							sprites[i].frame=sprites[i].startframe;
						if(sprites[i].frame>=sprites[i].endframe)
							sprites[i].frame=sprites[i].endframe-1;
						if(sprites[i].frame<0) sprites[i].frame=0;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_ANIMPLAYBACKONCE:
						if ( (sprites[i].framecounter--)<=0 )
						{
							sprites[i].framecounter=sprites[i].anim_delay_rate;
						 	sprites[i].frame--;
						}
						if(sprites[i].frame>=sprites[i].startframe)
						{ 
							logic_index--;
							end_slice=TRUE;
						}

						if(sprites[i].frame<sprites[i].startframe)
							sprites[i].frame=sprites[i].startframe;
						if(sprites[i].frame>=sprites[i].endframe)
							sprites[i].frame=sprites[i].endframe-1;

						if(sprites[i].frame<0) sprites[i].frame=0;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_ANIMSPEED:
						sprites[i].anim_delay_rate=logic[logic_index++];
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_ANIMSPEEDV:
						temp1=logic[logic_index++];
						sprites[i].anim_delay_rate=sprites[i].obj[temp1];
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_INRANGE:
						temp1=sprites[i].x-craftx;
						temp2=sprites[i].y-crafty;
						if(logic[logic_index++] > ((temp1*temp1)+(temp2*temp2)) )
							logic_index=logic[logic_index++];
						else logic_index++;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_BG_COLLIDE:
						temp1=logic[logic_index++];
						if( sprite_collision(seqdata[(sprites[i].name)],
													sprites[i].frame,
													sprites[i].x,
													sprites[i].y) )
						{
							logic_index=temp1;
						}
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_DIE:
						if(sprites[i].dying==0)	sprites[i].dying=2;
						else
						{
							sprites[i].dying--;
							if(sprites[i].dying==0) sprites[i].on=FALSE;
						}
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_ARRAY:
						temp1=logic[logic_index++];
						logic_index+=temp1;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_ADD_PARTICLE:
						temp1=logic[logic_index++];
						temp2=logic[logic_index++];
						temp3=logic[logic_index++];
						temp4=logic[logic_index++];
						//add_particle_no_gravity(sprites[i].obj[temp1],
						//								sprites[i].obj[temp2],
						//								sprites[i].obj[temp3],
						//								sprites[i].obj[temp4]);

						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_ON_HIT:
						sprites[i].on_hit_ptr=logic[logic_index++];
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_SHOOTABLE_ON:
						sprites[i].shootable=TRUE;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_SHOOTABLE_OFF:
						sprites[i].shootable=FALSE;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_VISIBLE_ON:
						sprites[i].visible=TRUE;
						break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
					case COMMAND_VISIBLE_OFF:
						sprites[i].visible=FALSE;
						break;
		
					}
			}

			sprites[i].move_ptr=logic_index;
		}
	}

}

