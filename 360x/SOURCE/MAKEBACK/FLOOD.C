/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Flood Code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include "globals.h"

void flood_fill(void);

long get_flood_val(long x,long y)
{
	long dx,dy;
	char val;

	dx=(x/16);
	dy=(y/16);
	dx-=flood_x-FLOOD_RADIUS;
	dy-=flood_y-FLOOD_RADIUS;

	if(dx<0) return -1;
	if(dx>=FLOOD_DIAMETER) return -1;
	if(dy<0) return -1;
	if(dy>=FLOOD_DIAMETER) return -1;

	val=flood_array[(dx+(dy*FLOOD_DIAMETER))];
	return (long) val;
}

long	vector_x[]={ 0 , 1,-1,-1, 1,-1, 0, 0, 1};
long	vector_y[]={ 0 , 1,-1, 1,-1, 0 ,-1, 1, 0};

void get_flood_vector(long x,long y,long *dx,long *dy)
{
	long i,index,best,current;

	best=99;
	index=0;
	for(i=0;i<9;i++)
	{
		current=get_flood_val(x+(vector_x[i]*FLOOD_RADIUS),
									 y+(vector_y[i]*FLOOD_RADIUS));
		if(current>0 )
		{
			if(current<best)
			{
				best=current;
				index=i;
			}
		}
	}
	*dx=vector_x[index];
	*dy=vector_y[index];
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Flood Code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void view_block_collision(void )
{
	long y;
	long flood_dest_x,flood_dest_y;
	long map_start_x,map_start_y;
	long map_end_x,map_end_y;
	//long i;

	memset(flood_array,-1,FLOOD_DIAMETER*FLOOD_DIAMETER);

	flood_x=craftx/16;
	flood_y=crafty/16;
	map_start_x=flood_x-FLOOD_RADIUS;
	map_start_y=flood_y-FLOOD_RADIUS;
	map_end_x=flood_x+FLOOD_RADIUS;
	map_end_y=flood_y+FLOOD_RADIUS;

	flood_dest_x=0;
	if(map_start_x<0)
	{
		flood_dest_x=-map_start_x;
		map_start_x=0;
	}
	flood_dest_y=0;
	if(map_start_y<0)
	{
		flood_dest_y=-map_start_y;
		map_start_y=0;
	}
	if(map_end_x>=(iff_width/16)) map_end_x=(iff_width/16)-1;
	if(map_end_y>=(iff_height/16)) map_end_y=(iff_height/16)-1;

	for(y=map_start_y;y<map_end_y;y++)
	{
		memcpy(flood_array+FLOOD_DIAMETER*flood_dest_y+flood_dest_x,
				 collision_block_array+(y*(iff_width/16))+map_start_x,
				 map_end_x-map_start_x);
		flood_dest_y++;
	}

	flood_fill();

	/*
	for(i=0;i<FLOOD_DIAMETER;i++)
	{
		if(safe_mode==YES)
		asm_add_dump_line(flood_array+FLOOD_DIAMETER*i,
					(i+logical_screen+split_height)*640,
					FLOOD_DIAMETER);
		else
		asm_add_dump_line(flood_array+FLOOD_DIAMETER*i,
					(i+y_mod[scroll_y]+logical_screen+split_height)*640+scroll_x,
					FLOOD_DIAMETER);
	}
	*/
}
 
long flood_stack_x[2][FLOOD_DIAMETER*FLOOD_DIAMETER];
long flood_stack_y[2][FLOOD_DIAMETER*FLOOD_DIAMETER];
long flood_buf;
long floods;
long flood_iter;

void flood_recurse(void );
	
void flood_fill(void)
{
	flood_buf=0;
	floods=0;
	flood_stack_x[flood_buf][floods]=FLOOD_RADIUS;
	flood_stack_y[flood_buf][floods]=FLOOD_RADIUS;
	floods++;

	for(flood_iter=0;flood_iter<FLOOD_RADIUS;flood_iter++) flood_recurse();
}

// It works

void flood_recurse(void )
{
	long i,x,y;
	long from_floods;

	from_floods=floods;
	floods=0;

	for(i=0;i<from_floods;i++)
	{
		x=flood_stack_x[flood_buf][i];
		y=flood_stack_y[flood_buf][i];
		
		if(flood_array[(x-1)+y*FLOOD_DIAMETER]==0)
		{
			flood_array[(x-1)+y*FLOOD_DIAMETER]=flood_iter;
			flood_stack_x[flood_buf^1][floods]=x-1;
			flood_stack_y[flood_buf^1][floods]=y;
			floods++;
		}
		if(flood_array[(x+1)+y*FLOOD_DIAMETER]==0)
		{
			flood_array[(x+1)+y*FLOOD_DIAMETER]=flood_iter;
			flood_stack_x[flood_buf^1][floods]=x+1;
			flood_stack_y[flood_buf^1][floods]=y;
			floods++;
		}
		if(flood_array[x+(y-1)*FLOOD_DIAMETER]==0)
		{
			flood_array[x+(y-1)*FLOOD_DIAMETER]=flood_iter;
			flood_stack_x[flood_buf^1][floods]=x;
			flood_stack_y[flood_buf^1][floods]=y-1;
			floods++;
		}
		if(flood_array[x+(y+1)*FLOOD_DIAMETER]==0)
		{
			flood_array[x+(y+1)*FLOOD_DIAMETER]=flood_iter;
			flood_stack_x[flood_buf^1][floods]=x;
			flood_stack_y[flood_buf^1][floods]=y+1;
			floods++;
		}

	}
	flood_buf^=1;
}


