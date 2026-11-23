/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SATURN PROCESS SPRITES AI */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include	<sgl.h>
#include "defines.h"
#include "types.h"
#include "globals.h"
#include <libsn.h>
#include <stdio.h>
#include <math.h>

#define	PI		3.14267214
#define	coll_buf	collmap

//Nasty nasty nasty
#define	internal_message	msg
#define	level_no	Level
#define	message	stat_str

/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include "ai.dat"
#include "command.h"

/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define HIGH 1
#define LOW  0

/* Externals ******************************************************/

extern	char				stat_str[];
extern	long				craft;
extern	long				pod_no;
extern	Uint8				Level;
extern	flag				change_level;
extern	flag				escape;
extern	Uint8				*collmap;
extern	attribstruc 	sprites_attribs[];
extern	spritestruct 	sprites[];
extern	Uint16			sprites_still[];

extern	int	rand(void);
extern	void	msg(Uint8 *message);
extern	flag 	sprite_collision(sprite_sequence seq,int sprite_no,int x,int y);
extern	void 	Add_Particle_Acceleration(long x,long y,long dx,long dy,long ddx,long ddy);
extern	flag 	SHcollision_at(long x,long y);

/* ProtoTypes *****************************************************/

void		process_sprites(void);
long 		process_sprite(AI_DATATYPE *AI_DATA);

void 		set_ai_none(long i);
void 		set_ai_cycle(long i);

void 	 	clear_sprites(void);
long 	 	add_sprite(int name,int action,int x,int y);
void 	 	get_reference_point(long i,long point,long var_x,long var_y);
void 	 	adopt_inrange_child(long father,long range);
long 		find_floor(long x,long y,long max_scan);
long 		get_logic_data(long i);
unsigned long sqr_root(unsigned long val);

/* Globals ********************************************************/

long				foster_parent;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void set_ai_none(long i)
{
	sprites[i].move_ptr=ai_none;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void set_ai_cycle(long i)
{
	sprites[i].move_ptr=ai_cycle;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

long get_logic_data(long i)
{
	return ai_data[i];
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Clear SPRITE LIST */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void clear_sprites(void)
{
	Uint16 i;

	for(i=0;i<MAX_SPRITES;i++)
	{
		sprites[i].on=FALSE;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Add a sprite to SPRITE LIST */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long add_sprite(int name,int action,int x,int y)
{

	int		i,j;

	// Find a free sprite

	for(i=0;i<MAX_SPRITES;i++)
	{
		if(sprites[i].on==FALSE)	break;
	}
	
	if(i==MAX_SPRITES)
	{
		return(-1);
	}
	else
	{
			// AI data
		switch (action)
		{
			case ACTION_NONE:
				//sprites[i].move_ptr=ai_cycle;
				sprites[i].move_ptr=ai_none;
				break;
			case ACTION_CYCLE:
				sprites[i].move_ptr=ai_cycle;
				break;
			case ACTION_PINGPONG:
				sprites[i].move_ptr=ai_pingpong;
				break;
			case ACTION_PLAYONCE:
				sprites[i].move_ptr=ai_explode;
				break;
			case ACTION_PATH:
				sprites[i].move_ptr=ai_path;
				break;
			default:
				sprites[i].move_ptr=ai_cycle;
				break;
		}
			
		// Find action associated to sprite
		if(sprites_attribs[name].ai!=-1)
		{
			sprites[i].move_ptr=sprites_attribs[name].ai;
		}
		else
		{ 
			//sprintf(message,"%d AI not defined\n",name);
			//msg(message);
		}
		
		sprites[i].path=-1;
		sprites[i].node=-1;
		sprites[i].thread_ptr=-1;
		
		// Insert sprite data
		sprites[i].still=NO;
		
		j=0;
		while(sprites_still[j]!=NAME_END)
		{
			if(name==sprites_still[j])
			{
				sprites[i].still=YES;
				sprintf(stat_str,"Still sprite:%d at %d\n",name,i);
				msg(stat_str);
				break;
			}
			j++;
		}

		sprites[i].queen=-1;
		sprites[i].child=-1;
		sprites[i].parent=-1;
		sprites[i].obj[obj_children]=0;
		sprites[i].hit=FALSE;
		sprites[i].on=TRUE;
		sprites[i].visible=TRUE;
		sprites[i].shootable=sprites_attribs[name].shootable;
		sprites[i].name=name;
		sprites[i].action=action;
		sprites[i].on_hit_ptr=ai_polo;
		sprites[i].x=x;
		sprites[i].start_x=x;
		sprites[i].y=y;
		sprites[i].start_y=y;
//		sprites[i].buf_x[0]=x;
//		sprites[i].buf_x[1]=x;
//		sprites[i].buf_y[0]=y;
//		sprites[i].buf_y[1]=y;
		sprites[i].obj[obj_x]=x;
		sprites[i].obj[obj_y]=y;
		sprites[i].anim_delay_rate=4;
		sprites[i].frame=0;
		sprites[i].dying=0;
		sprites[i].obj[obj_angle]=0;
		sprites[i].obj[obj_angle2]=0;
		sprites[i].clipped=NO;
		
		sprites[i].startframe=0;
		sprites[i].endframe=seqdata[name].frames; //-2 from this is 4 phil
		
		// Bodge for spikes...
		if(name==NAME_SPIKE)
		{
			sprites[i].framecounter=8+((rand()*128)>>15);
		}
		else
		{
			sprites[i].framecounter=0;
		}
		
		sprites[i].framecounter+=rand()&3;
		
		#if 0
		for(j=0;j<2;j++)					// Set up Double Buffer Data
		{
			sprites[i].buf_x[j]=x;
			sprites[i].buf_y[j]=y;
			sprites[i].buf_frame[j]=0;
			sprites[i].buf_rem[j]=NO;
		}
		#endif

		return i;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Make a linked list of inrange sprites */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void adopt_inrange_child(long father,long range)
{
	long i;
	long x,y;

	for(i=0;i<MAX_SPRITES;i++)
	{
		if( (sprites[i].on==TRUE) && (i!=father) )
		{
			if(sprites[father].name==sprites[i].name)
			{
				if( (sprites[i].parent==-1) && (sprites[i].child==-1) )
				{
					x=sprites[father].x-sprites[i].x;
					y=sprites[father].y-sprites[i].y;
					if( ((x*x)+(y*y)) < range )
					{
						sprites[i].parent=father;
						sprites[i].queen=foster_parent;
						sprites[father].child=i;
						sprites[foster_parent].obj[obj_children]++;
						//printf("Father %i Child %i %i\n",father,i,sprites[foster_parent].obj[obj_children]);
						adopt_inrange_child(i,range);
						return;
					}
				}
			}
		}
	}
}
	
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long find_floor(long x,long y,long max_scan)
{
	long height;
	
	height=0;
	if( SHcollision_at(x,y)==TRUE )
	{
		// scan up
		while(max_scan>0)
		{
			height--;
			if( SHcollision_at(x,y+height)==FALSE ) break;
			max_scan--;
		}
	}
	else
	{
		// scan down
		while(max_scan>0)
		{
			height++;
			if( SHcollision_at(x,y+height)==TRUE ) break;
			max_scan--;
		}
		height--;
	}
	return height;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Integer Square Root (Approximation Method) */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

//Find where this is called and use slSquart instead
unsigned long sqr_root(unsigned long val)
{
	int	i;
	unsigned long	upper,lower,mid;
	unsigned long	sqr;

	upper=val;
	lower=0;

	if(upper>65536) upper=65536;

	for(i=0;i<16;i++)
	{
		mid=lower+((upper-lower)>>1);
		sqr=mid*mid;
		if(sqr==val) break;
		if(sqr>val) upper=mid;
		else lower=mid;
	}
	return(mid);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
//
// The logic processing code.
//
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void process_sprites(void )
{
	AI_DATATYPE		AI;

	for(AI.spr_i=0;AI.spr_i<MAX_SPRITES;AI.spr_i++)
	{
		if((sprites[AI.spr_i].on==TRUE))
		{
			if(sprites[AI.spr_i].move_ptr==0)
			{
				//sprintf(stat_str,"NULL AI in sprite %d \n", (int) sprites[AI.spr_i].name);
				//msg(stat_str);
				sprites[AI.spr_i].move_ptr=ai_none;
				//sprites[AI.spr_i].move_ptr=ai_pingpong;
			}

			AI.logic_index	=	sprites[AI.spr_i].move_ptr;
 			sprites[AI.spr_i].move_ptr=process_sprite(&AI);
			
			if(sprites[AI.spr_i].thread_ptr!=-1)
			{
				AI.logic_index	=	sprites[AI.spr_i].thread_ptr;
 				sprites[AI.spr_i].thread_ptr=process_sprite(&AI);
			}
		}
	}
}

/**********************************************************************/

long 		process_sprite(AI_DATATYPE *AI)
{

	AI->end_slice=FALSE;
	AI->logic=ai_data;

	while(AI->end_slice==FALSE)
	{
		AI->command=AI->logic[AI->logic_index++];
		
		if(AI->command<COMMAND_END)
		{
			AI_Command[AI->command](AI);
		}
		else
		{
		  	//sprintf(stat_str,"[%d] , Pointer:%p\n",(int) AI->command, AI_Command[AI->command]);
			//msg(stat_str);
		}
	}

	return AI->logic_index;

}

/**********************************************************************/

void	AI_COMMAND_WAIT(AI_DATATYPE *AI_DATA)
{
		AI_DATA->logic_index--;
		AI_DATA->end_slice=TRUE;
}

/**********************************************************************/

void	AI_COMMAND_STOP(AI_DATATYPE *AI_DATA)
{
				AI_DATA->end_slice=TRUE;
}

/**********************************************************************/

void	AI_COMMAND_PAUSE(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index];
				if(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]>0)
				{
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]--;
					AI_DATA->end_slice=TRUE;
					AI_DATA->logic_index--;
				}
				else AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_JMP(AI_DATATYPE *AI_DATA)
{
				AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index];
}

/**********************************************************************/

void	AI_COMMAND_CALL(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].ret_ptr=AI_DATA->logic_index;
				AI_DATA->logic_index=AI_DATA->temp1;
}

/**********************************************************************/

void	AI_COMMAND_RETURN(AI_DATATYPE *AI_DATA)
{
				AI_DATA->logic_index=sprites[AI_DATA->spr_i].ret_ptr;
}

/**********************************************************************/

void	AI_COMMAND_THREAD(AI_DATATYPE *AI_DATA)
{
				sprites[AI_DATA->spr_i].thread_ptr=AI_DATA->logic[AI_DATA->logic_index++];
}

/**********************************************************************/

void	AI_COMMAND_MOVERND(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=(rand()*AI_DATA->temp1)>>15;
}

/**********************************************************************/

void	AI_COMMAND_MOVEI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=AI_DATA->temp1;
}

/**********************************************************************/

void	AI_COMMAND_MOVEV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
}

/**********************************************************************/

void	AI_COMMAND_ADDI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]+=AI_DATA->temp1;
}

/**********************************************************************/

void	AI_COMMAND_ADDV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]+=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
}

/**********************************************************************/

void	AI_COMMAND_SUBI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]-=AI_DATA->temp1;
}

/**********************************************************************/

void	AI_COMMAND_SUBV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]-=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
}

/**********************************************************************/

void	AI_COMMAND_INDEX_ARRAY(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];			// var index
				AI_DATA->temp1=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];			// array
				if(AI_DATA->temp1>AI_DATA->logic[AI_DATA->temp2+1]) AI_DATA->temp1=0;	// validate
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->temp2+AI_DATA->temp1+2];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];			// var dest
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=AI_DATA->temp1;
}

/**********************************************************************/

void	AI_COMMAND_SHL(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		// shift
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// shift var source
				AI_DATA->temp1=(sprites[AI_DATA->spr_i].obj[AI_DATA->temp2])<<AI_DATA->temp1;
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		 		// shift var dest
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=AI_DATA->temp1;
}

/**********************************************************************/

void	AI_COMMAND_SHR(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		// shift
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// shift var source
				AI_DATA->temp1=(sprites[AI_DATA->spr_i].obj[AI_DATA->temp2])>>AI_DATA->temp1;
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];			 		// shift var dest
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=AI_DATA->temp1;
}

/**********************************************************************/

void	AI_COMMAND_MULI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		// mul1
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// mul2
				AI_DATA->temp1=sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]*AI_DATA->temp1;
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=AI_DATA->temp1;		// result

}

/**********************************************************************/

void	AI_COMMAND_MULV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		// mul1
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// mul2
				AI_DATA->temp1=sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]*sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=AI_DATA->temp1;		// result
}

/**********************************************************************/

void	AI_COMMAND_DIVI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		// divider
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// 
				if(AI_DATA->temp1!=0) AI_DATA->temp1=sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]/AI_DATA->temp1;
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// result
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=AI_DATA->temp1;
}

/**********************************************************************/

void	AI_COMMAND_DIVV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		// divider
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// 
				if(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]!=0)
					 AI_DATA->temp1=sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]/sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// result
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=AI_DATA->temp1;
}

/**********************************************************************/

void	AI_COMMAND_NEG(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=-sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
}

/**********************************************************************/

void	AI_COMMAND_ADD_CHILD(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		// sprites name
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// sprites pc
				AI_DATA->temp3=add_sprite(AI_DATA->temp1,ACTION_CYCLE,
										sprites[AI_DATA->spr_i].x,sprites[AI_DATA->spr_i].y);
				sprintf(stat_str,"Child added returned %i\n",(int)AI_DATA->temp3);
				internal_message(stat_str);
				if(sprites[AI_DATA->spr_i].child!=-1)
				{ 
					internal_message("Old child overwritten\n");
				}
				if(AI_DATA->temp3!=-1)
				{
					sprites[AI_DATA->spr_i].child=AI_DATA->temp3;
					sprites[AI_DATA->temp3].move_ptr=AI_DATA->temp2;
					sprites[AI_DATA->temp3].parent=AI_DATA->spr_i;
				}
}

/**********************************************************************/

void	AI_COMMAND_ADOPT_INRANGE_CHILD(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				if(sprites[AI_DATA->spr_i].child==-1)
				{
					foster_parent=AI_DATA->spr_i;
					sprites[AI_DATA->spr_i].obj[obj_children]=0;
					adopt_inrange_child(AI_DATA->spr_i,AI_DATA->temp1);
				}
}

/**********************************************************************/

void	AI_COMMAND_QUEEN_ORDER(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->spr_i;
				while(sprites[AI_DATA->temp2].child!=-1)
				{
					AI_DATA->temp2=sprites[AI_DATA->temp2].child;
					sprites[AI_DATA->temp2].obj[AI_DATA->temp1]=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
					//printf("From %i To %i\n",i,AI_DATA->temp2);
				}
}

/**********************************************************************/

void	AI_COMMAND_GET_QUEENS(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=sprites[AI_DATA->spr_i].queen;
				if(AI_DATA->temp2!=-1)
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=sprites[AI_DATA->temp2].obj[AI_DATA->temp1];
}

/**********************************************************************/

void	AI_COMMAND_GET_PARENTV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=sprites[AI_DATA->spr_i].parent;
				if(AI_DATA->temp2!=-1)
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=sprites[AI_DATA->temp2].obj[AI_DATA->temp1];
}

/**********************************************************************/

void	AI_COMMAND_GET_CHILDV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=sprites[AI_DATA->spr_i].child;
				if(AI_DATA->temp2!=-1)
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=sprites[AI_DATA->temp2].obj[AI_DATA->temp1];
}

/**********************************************************************/

void	AI_COMMAND_IFEQI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] == AI_DATA->temp1)
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index++];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFEQV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] == sprites[AI_DATA->spr_i].obj[AI_DATA->temp1])
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index++];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFNEI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] != AI_DATA->temp1)
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFNEV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] != sprites[AI_DATA->spr_i].obj[AI_DATA->temp1])
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFLEI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] <= AI_DATA->temp1)
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFLEV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] <= sprites[AI_DATA->spr_i].obj[AI_DATA->temp1])
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index++];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFLTI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] < AI_DATA->temp1)
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFLTV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] < sprites[AI_DATA->spr_i].obj[AI_DATA->temp1])
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFGTI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] > AI_DATA->temp1)
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFGTV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] > sprites[AI_DATA->spr_i].obj[AI_DATA->temp1])
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFGEI(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] >= AI_DATA->temp1)
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_IFGEV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				if( sprites[AI_DATA->spr_i].obj[AI_DATA->temp2] >= sprites[AI_DATA->spr_i].obj[AI_DATA->temp1])
						AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index];
				else	AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_FRAMEINC(AI_DATATYPE *AI_DATA)
{
 				if ( (sprites[AI_DATA->spr_i].framecounter--)<=0 )
				{
					sprites[AI_DATA->spr_i].framecounter=sprites[AI_DATA->spr_i].anim_delay_rate;
				 	AI_DATA->temp1=sprites[AI_DATA->spr_i].frame;
					AI_DATA->temp1++;
					if( AI_DATA->temp1>=sprites[AI_DATA->spr_i].endframe ) AI_DATA->temp1=sprites[AI_DATA->spr_i].startframe;
					sprites[AI_DATA->spr_i].frame=AI_DATA->temp1;
				}
}

/**********************************************************************/

void	AI_COMMAND_UPDATE_XY(AI_DATATYPE *AI_DATA)
{
				sprites[AI_DATA->spr_i].x=sprites[AI_DATA->spr_i].obj[obj_x];
				sprites[AI_DATA->spr_i].y=sprites[AI_DATA->spr_i].obj[obj_y];
}

/**********************************************************************/

void	AI_COMMAND_SET_FRAME(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
				//printf("Frame Set %i\n",sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]);
				/*
				if(sprites[AI_DATA->spr_i].frame>=seqdata[(sprites[AI_DATA->spr_i].name)].frames)
					sprites[AI_DATA->spr_i].frame=0;
				if(sprites[AI_DATA->spr_i].frame<0) sprites[AI_DATA->spr_i].frame=0;
				*/
}

/**********************************************************************/

void	AI_COMMAND_SETANIMRANGE(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].startframe=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
				sprites[AI_DATA->spr_i].endframe=sprites[AI_DATA->spr_i].obj[AI_DATA->temp2];
}

/**********************************************************************/

void	AI_COMMAND_GETANIMRANGE(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=sprites[AI_DATA->spr_i].startframe;
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=sprites[AI_DATA->spr_i].endframe;
}

/**********************************************************************/

void	AI_COMMAND_SETANIMNORMAL(AI_DATATYPE *AI_DATA)
{
//				sprites[AI_DATA->spr_i].startframe=sprites[AI_DATA->spr_i].normal_startframe;
//				sprites[AI_DATA->spr_i].endframe=sprites[AI_DATA->spr_i].normal_endframe;
//				sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
}

/**********************************************************************/

void	AI_COMMAND_SETANIMLEFT(AI_DATATYPE *AI_DATA)
{
//				sprites[AI_DATA->spr_i].startframe=sprites[AI_DATA->spr_i].left_startframe;
//				sprites[AI_DATA->spr_i].endframe=sprites[AI_DATA->spr_i].left_endframe;
//				sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
}

/**********************************************************************/

void	AI_COMMAND_SETANIMRIGHT(AI_DATATYPE *AI_DATA)
{
//				sprites[AI_DATA->spr_i].startframe=sprites[AI_DATA->spr_i].right_startframe;
//				sprites[AI_DATA->spr_i].endframe=sprites[AI_DATA->spr_i].right_endframe;
//	 			sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
}

/**********************************************************************/

void	AI_COMMAND_SETANIMUP(AI_DATATYPE *AI_DATA)
{
//				sprites[AI_DATA->spr_i].startframe=sprites[AI_DATA->spr_i].up_startframe;
//				sprites[AI_DATA->spr_i].endframe=sprites[AI_DATA->spr_i].up_endframe;
//		 		sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
}

/**********************************************************************/

void	AI_COMMAND_SETANIMDOWN(AI_DATATYPE *AI_DATA)
{
//				sprites[AI_DATA->spr_i].startframe=sprites[AI_DATA->spr_i].down_startframe;
//				sprites[AI_DATA->spr_i].endframe=sprites[AI_DATA->spr_i].down_endframe;
//		 		sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
}

/**********************************************************************/

void	AI_COMMAND_SETANIMWOUNDED(AI_DATATYPE *AI_DATA)
{
//				sprites[AI_DATA->spr_i].startframe=sprites[AI_DATA->spr_i].wounded_startframe;
//				sprites[AI_DATA->spr_i].endframe=sprites[AI_DATA->spr_i].wounded_endframe;
//		 		sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
}

/**********************************************************************/

void	AI_COMMAND_SETANIMDEATH(AI_DATATYPE *AI_DATA)
{
//				sprites[AI_DATA->spr_i].startframe=sprites[AI_DATA->spr_i].death_startframe;
//				sprites[AI_DATA->spr_i].endframe=sprites[AI_DATA->spr_i].death_endframe;
//				sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
}

/**********************************************************************/

void	AI_COMMAND_ANIMPLAYFORWARDONCE(AI_DATATYPE *AI_DATA)
{
				if ( (sprites[AI_DATA->spr_i].framecounter--)<=0 )
				{
					sprites[AI_DATA->spr_i].framecounter=sprites[AI_DATA->spr_i].anim_delay_rate;
				 	sprites[AI_DATA->spr_i].frame++;
				}

				if(sprites[AI_DATA->spr_i].frame<=(sprites[AI_DATA->spr_i].endframe-1))
				{ 
					AI_DATA->logic_index--;
					AI_DATA->end_slice=TRUE;
				}

				if(sprites[AI_DATA->spr_i].frame<sprites[AI_DATA->spr_i].startframe)
					sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
				if(sprites[AI_DATA->spr_i].frame>=sprites[AI_DATA->spr_i].endframe)
					sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].endframe-1;
				if(sprites[AI_DATA->spr_i].frame<0) sprites[AI_DATA->spr_i].frame=0;
}

/**********************************************************************/

void	AI_COMMAND_ANIMPLAYBACKONCE(AI_DATATYPE *AI_DATA)
{
				if ( (sprites[AI_DATA->spr_i].framecounter--)<=0 )
				{
					sprites[AI_DATA->spr_i].framecounter=sprites[AI_DATA->spr_i].anim_delay_rate;
				 	sprites[AI_DATA->spr_i].frame--;
				}
				if(sprites[AI_DATA->spr_i].frame>=sprites[AI_DATA->spr_i].startframe)
				{ 
					AI_DATA->logic_index--;
					AI_DATA->end_slice=TRUE;
				}

				if(sprites[AI_DATA->spr_i].frame<sprites[AI_DATA->spr_i].startframe)
					sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
				if(sprites[AI_DATA->spr_i].frame>=sprites[AI_DATA->spr_i].endframe)
					sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].endframe-1;

				if(sprites[AI_DATA->spr_i].frame<0) sprites[AI_DATA->spr_i].frame=0;
}

/**********************************************************************/

void	AI_COMMAND_ANIMPLAYLOOPONCE(AI_DATATYPE *AI_DATA)
{
				if ( (sprites[AI_DATA->spr_i].framecounter--)<=0 )
				{
					sprites[AI_DATA->spr_i].framecounter=sprites[AI_DATA->spr_i].anim_delay_rate;
				 	sprites[AI_DATA->spr_i].frame++;
				}

				if(sprites[AI_DATA->spr_i].frame<=(sprites[AI_DATA->spr_i].endframe-1))
				{ 
					AI_DATA->logic_index--;
					AI_DATA->end_slice=TRUE;
				}

				if(sprites[AI_DATA->spr_i].frame<sprites[AI_DATA->spr_i].startframe)
					sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
				if(sprites[AI_DATA->spr_i].frame>=sprites[AI_DATA->spr_i].endframe)
					sprites[AI_DATA->spr_i].frame=sprites[AI_DATA->spr_i].startframe;
				if(sprites[AI_DATA->spr_i].frame<0) sprites[AI_DATA->spr_i].frame=0;
}

/**********************************************************************/

void	AI_COMMAND_ANIMSPEED(AI_DATATYPE *AI_DATA)
{
				sprites[AI_DATA->spr_i].anim_delay_rate=AI_DATA->logic[AI_DATA->logic_index++];
}

/**********************************************************************/

void	AI_COMMAND_ANIMSPEEDV(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].anim_delay_rate=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
}

/**********************************************************************/

void	AI_COMMAND_INRANGE(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=sprites[AI_DATA->spr_i].x-craftx;
				AI_DATA->temp2=sprites[AI_DATA->spr_i].y-crafty;
				if(AI_DATA->logic[AI_DATA->logic_index++] > ((AI_DATA->temp1*AI_DATA->temp1)+(AI_DATA->temp2*AI_DATA->temp2)) )
					AI_DATA->logic_index=AI_DATA->logic[AI_DATA->logic_index++];
				else AI_DATA->logic_index++;
}

/**********************************************************************/

void	AI_COMMAND_BG_COLLIDE(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				#if 0
				if( sprite_collision(&seqdata[(sprites[AI_DATA->spr_i].name)],
											sprites[AI_DATA->spr_i].frame,
											sprites[AI_DATA->spr_i].x,
											sprites[AI_DATA->spr_i].y) )
				{
					AI_DATA->logic_index=AI_DATA->temp1;
				}
				#endif
}

/**********************************************************************/

void	AI_COMMAND_POINT_COLLISION(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		//x
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		//y
				AI_DATA->temp1=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
				AI_DATA->temp2=sprites[AI_DATA->spr_i].obj[AI_DATA->temp2];
				AI_DATA->temp3=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp4=coll_buf[( (AI_DATA->temp1+AI_DATA->temp2*iff_width)>>3 )];
				if( ( AI_DATA->temp4 & (1<<((7-AI_DATA->temp1)&7)) )!=0) AI_DATA->logic_index=AI_DATA->temp3;
}

/**********************************************************************/

void	AI_COMMAND_FIND_FLOOR(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		//x
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		//y
				AI_DATA->temp3=AI_DATA->logic[AI_DATA->logic_index++]; 		//max scan + return var
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp3]=find_floor(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1],
															sprites[AI_DATA->spr_i].obj[AI_DATA->temp2],
															sprites[AI_DATA->spr_i].obj[AI_DATA->temp3]);
}

/**********************************************************************/

void	AI_COMMAND_SCAN(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		//x
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		//y
				//AI_DATA->temp3=screen_clip_point(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1],sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]);
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		//x1
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		//x2
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=AI_DATA->temp3&0xffff;
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=AI_DATA->temp3>>16;
}

/**********************************************************************/

void	AI_COMMAND_DIE(AI_DATATYPE *AI_DATA)
{
				AI_DATA->end_slice=TRUE;
				AI_DATA->logic_index--;
				if(sprites[AI_DATA->spr_i].dying==0)	sprites[AI_DATA->spr_i].dying=2;
				else
				{
					sprites[AI_DATA->spr_i].dying--;
					if(sprites[AI_DATA->spr_i].dying==0)
					{ 
						//printf("Child %i\n",sprites[AI_DATA->spr_i].child);
						sprites[AI_DATA->spr_i].on=FALSE;

						// Fix linked list

					 	if(sprites[AI_DATA->spr_i].parent!=-1)
						{
							//printf("%i Has Parent\n",i);
							AI_DATA->temp1=sprites[AI_DATA->spr_i].parent;
							sprites[AI_DATA->temp1].child=sprites[AI_DATA->spr_i].child;
											
							if(sprites[AI_DATA->spr_i].child!=-1)
							{
								//printf("%i Has Child\n",i);
								AI_DATA->temp1=sprites[AI_DATA->spr_i].child;
								sprites[AI_DATA->temp1].parent=sprites[AI_DATA->spr_i].parent;
							}
						}
						if(sprites[AI_DATA->spr_i].queen!=-1)
						{
							AI_DATA->temp1=sprites[AI_DATA->spr_i].queen;
							sprites[AI_DATA->temp1].obj[obj_children]--;
						}
						if(sprites[AI_DATA->spr_i].obj[obj_children]>0)
						{
							//printf("Queen %i killed\n",i);
							//printf("Child %i\n",sprites[AI_DATA->spr_i].child);
							if(sprites[AI_DATA->spr_i].child!=-1)
							{
								//printf("Has a child\n",i);
								for(AI_DATA->temp1=0;AI_DATA->temp1<MAX_SPRITES;AI_DATA->temp1++)
								{
									if(sprites[AI_DATA->temp1].queen==AI_DATA->spr_i)
										sprites[AI_DATA->temp1].queen=sprites[AI_DATA->spr_i].child;
								}
								AI_DATA->temp1=sprites[AI_DATA->spr_i].child;
								sprites[AI_DATA->temp1].parent=-1;
								sprites[AI_DATA->temp1].obj[obj_children]=sprites[AI_DATA->spr_i].obj[obj_children];
								sprites[AI_DATA->temp1].path=sprites[AI_DATA->spr_i].path;
								sprites[AI_DATA->temp1].node=sprites[AI_DATA->spr_i].node;
								//printf("New Queen %i Children %i\n",AI_DATA->temp1,sprites[AI_DATA->spr_i].obj[obj_children]);
							}
						}
					}
				}
}

/**********************************************************************/

void	AI_COMMAND_ARRAY(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->logic_index+=AI_DATA->temp1;
}

/**********************************************************************/

void	AI_COMMAND_ADD_PARTICLE(AI_DATATYPE *AI_DATA)
{
	#if 0
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp3=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp4=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp5=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp6=AI_DATA->logic[AI_DATA->logic_index++];
				Add_Particle_Acceleration(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1],
													sprites[AI_DATA->spr_i].obj[AI_DATA->temp2],
													sprites[AI_DATA->spr_i].obj[AI_DATA->temp3],
													sprites[AI_DATA->spr_i].obj[AI_DATA->temp4],
													sprites[AI_DATA->spr_i].obj[AI_DATA->temp5],														
													sprites[AI_DATA->spr_i].obj[AI_DATA->temp6]);
	#endif
}

/**********************************************************************/

void	AI_COMMAND_ON_HIT(AI_DATATYPE *AI_DATA)
{
				sprites[AI_DATA->spr_i].on_hit_ptr=AI_DATA->logic[AI_DATA->logic_index++];
}

/**********************************************************************/

void	AI_COMMAND_SHOOTABLE_ON(AI_DATATYPE *AI_DATA)
{
				sprites[AI_DATA->spr_i].shootable=TRUE;
}

/**********************************************************************/

void	AI_COMMAND_SHOOTABLE_OFF(AI_DATATYPE *AI_DATA)
{
				sprites[AI_DATA->spr_i].shootable=FALSE;
}

/**********************************************************************/

void	AI_COMMAND_VISIBLE_ON(AI_DATATYPE *AI_DATA)
{
				sprites[AI_DATA->spr_i].visible=TRUE;
}

/**********************************************************************/

void	AI_COMMAND_VISIBLE_OFF(AI_DATATYPE *AI_DATA)
{
				sprites[AI_DATA->spr_i].visible=FALSE;
}

/**********************************************************************/

void	AI_COMMAND_SET_PRIORITY(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].priority=AI_DATA->temp1;		 
}

/**********************************************************************/

void	AI_COMMAND_GET_FLOOD_VAL(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				//sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=get_flood_val(sprites[AI_DATA->spr_i].x,sprites[AI_DATA->spr_i].y);
}

/**********************************************************************/

void	AI_COMMAND_GET_VECTOR(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				//get_flood_vector(sprites[AI_DATA->spr_i].x,sprites[AI_DATA->spr_i].y,
				//					  &sprites[AI_DATA->spr_i].obj[AI_DATA->temp1],&sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]);
}

/**********************************************************************/

void	AI_COMMAND_GET_QUEEN_ID(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=sprites[AI_DATA->spr_i].queen;
				//printf("Queen ID = %i\n",sprites[AI_DATA->spr_i].queen);
}

/**********************************************************************/

void	AI_COMMAND_GET_PARENT_ID(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=sprites[AI_DATA->spr_i].parent;
}

/**********************************************************************/

void	AI_COMMAND_GET_CHILD_ID(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=sprites[AI_DATA->spr_i].child;
}

/**********************************************************************/

void	AI_COMMAND_GET_PLAYER_ID(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=craft;
}

/**********************************************************************/

void	AI_COMMAND_GET_POD_ID(AI_DATATYPE *AI_DATA)
{							  
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=pod_no;
}

/**********************************************************************/

void	AI_COMMAND_GET_IDS_VAR(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		// ID
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// VAR wanted
				AI_DATA->temp3=AI_DATA->logic[AI_DATA->logic_index++];		// VAR dest
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp3]=sprites[(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1])].obj[AI_DATA->temp2];
}

/**********************************************************************/

void	AI_COMMAND_SET_IDS_VAR(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];		// Var source
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];		// ID
				AI_DATA->temp3=AI_DATA->logic[AI_DATA->logic_index++];		// Var Dest
				sprites[(sprites[AI_DATA->spr_i].obj[AI_DATA->temp2])].obj[AI_DATA->temp3]=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
}

/**********************************************************************/

void	AI_COMMAND_GET_ANGLE_TO_ID(AI_DATATYPE *AI_DATA)
{
		#if 0
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp1=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
				if(AI_DATA->temp1!=-1)
				{
					sprites[AI_DATA->spr_i].angle=atan2(sprites[AI_DATA->temp1].x-sprites[AI_DATA->spr_i].x,
												  sprites[AI_DATA->temp1].y-sprites[AI_DATA->spr_i].y);
					//arc_count++;
					AI_DATA->dtemp=sprites[AI_DATA->spr_i].angle;
					if(AI_DATA->dtemp<0) AI_DATA->dtemp=(PI*2)+AI_DATA->dtemp;
					sprites[AI_DATA->spr_i].obj[obj_angle]=(AI_DATA->dtemp*1024)/(PI*2);
				}
		#endif
}

/**********************************************************************/

void	AI_COMMAND_SET_VEL(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp3=AI_DATA->logic[AI_DATA->logic_index++];
				#if 0
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=asm_mul_div(sin_table[((sprites[AI_DATA->spr_i].obj[obj_angle])
															+(sprites[AI_DATA->spr_i].obj[obj_angle2]))&1023],
															 sprites[AI_DATA->spr_i].obj[AI_DATA->temp1],32768);
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp3]=asm_mul_div(cos_table[(sprites[AI_DATA->spr_i].obj[obj_angle])],
															 sprites[AI_DATA->spr_i].obj[AI_DATA->temp1],32768);
				#endif
}

/**********************************************************************/

void	AI_COMMAND_GET_DISTANCE_TO_ID(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp3=sprites[AI_DATA->spr_i].x-sprites[(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1])].x;
				AI_DATA->temp4=sprites[AI_DATA->spr_i].y-sprites[(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1])].y;
				AI_DATA->temp5=(AI_DATA->temp3*AI_DATA->temp3)+(AI_DATA->temp4*AI_DATA->temp4);
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=sqr_root(AI_DATA->temp5);
}

/**********************************************************************/

void	AI_COMMAND_GET_ROTATION_TO_ID(AI_DATATYPE *AI_DATA)
{
		#if 0
				AI_DATA->dtemp=sprites[AI_DATA->spr_i].angle;
				while(AI_DATA->dtemp<0) AI_DATA->dtemp+=PI*2;
				while(AI_DATA->dtemp>(PI*2)) AI_DATA->dtemp-=PI*2;

				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp1=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
				AI_DATA->dtemp=atan2(sprites[AI_DATA->temp1].x-sprites[AI_DATA->spr_i].x,
						   	sprites[AI_DATA->temp1].y-sprites[AI_DATA->spr_i].y);
					//arc_count++;
				//printf("Angle %f New Angle %f\n",sprites[AI_DATA->spr_i].angle,AI_DATA->dtemp);
				AI_DATA->dtemp=sprites[AI_DATA->spr_i].angle-AI_DATA->dtemp;
				//printf("Difference %f\n",AI_DATA->dtemp);
				if(AI_DATA->dtemp<0)
				{
					AI_DATA->dtemp=-AI_DATA->dtemp;
					if(AI_DATA->dtemp<PI) AI_DATA->temp1=1;
					else AI_DATA->temp1=-1;
				}
				else
				{
					if(AI_DATA->dtemp<PI) AI_DATA->temp1=-1;
					else AI_DATA->temp1=1;
				}
				//printf("Difference %f\n",AI_DATA->dtemp);
				//printf("Rotation = %i\n",AI_DATA->temp1);
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp2]=AI_DATA->temp1;
		#endif

}

/**********************************************************************/

void	AI_COMMAND_ADD_TO_ANGLE(AI_DATATYPE *AI_DATA)
{
		#if 0
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp1=sprites[AI_DATA->spr_i].obj[AI_DATA->temp1];
				/*
					sprites[AI_DATA->spr_i].angle+=(AI_DATA->temp1*PI*2)/256;
				*/
				sprites[AI_DATA->spr_i].obj[obj_angle]+=AI_DATA->temp1*4;
				sprites[AI_DATA->spr_i].obj[obj_angle]&=1023;
		#endif
}

/**********************************************************************/

void	AI_COMMAND_GET_ANGLE(AI_DATATYPE *AI_DATA)
{
	#if 0
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				/*
					while(sprites[AI_DATA->spr_i].angle<0) sprites[AI_DATA->spr_i].angle+=PI*2;
					while(sprites[AI_DATA->spr_i].angle>(PI*2)) sprites[AI_DATA->spr_i].angle-=PI*2;
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=(sprites[AI_DATA->spr_i].angle*256)/(PI*2);
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]&=255;
				*/
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=sprites[AI_DATA->spr_i].obj[obj_angle];
	#endif

}

/**********************************************************************/

void	AI_COMMAND_SET_ANGLE(AI_DATATYPE *AI_DATA)
{
	#if 0
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].angle=(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]*PI*2)/256;
				sprites[AI_DATA->spr_i].obj[obj_angle]=(AI_DATA->temp1&0xff)*4;
	#endif
}

/**********************************************************************/

void	AI_COMMAND_NEXT_NODE(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp4=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp5=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp6=AI_DATA->logic[AI_DATA->logic_index++];
				
				if(sprites[AI_DATA->spr_i].path==-1)			// No Path available
				{
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp4]=-1;			
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp5]=-1;			
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp6]=-1;
					return;
				}

				//get_path_coordinates(&sprites[AI_DATA->spr_i].node,&AI_DATA->temp1,&AI_DATA->temp2);

				AI_DATA->temp1=sprites[AI_DATA->spr_i].start_x+AI_DATA->temp1-sprites[AI_DATA->spr_i].x;
				AI_DATA->temp2=sprites[AI_DATA->spr_i].start_y+AI_DATA->temp2-sprites[AI_DATA->spr_i].y;
				AI_DATA->temp3=sqr_root( (AI_DATA->temp1*AI_DATA->temp1)+(AI_DATA->temp2*AI_DATA->temp2) );
				
				if(AI_DATA->temp3>0)
				{
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp4]=(AI_DATA->temp1*65536)/AI_DATA->temp3;
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp5]=(AI_DATA->temp2*65536)/AI_DATA->temp3;
				}
				else
				{
					//printf("Zero length path %s (%i,%i)\n",get_sprite_name(sprites[AI_DATA->spr_i].name),AI_DATA->temp1,AI_DATA->temp2);
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp4]=0;
					sprites[AI_DATA->spr_i].obj[AI_DATA->temp5]=0;
				}
				
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp6]=AI_DATA->temp3;

				// bodge... (for -2 values which mean return
				if(sprites[AI_DATA->spr_i].node<0) sprites[AI_DATA->spr_i].node=sprites[AI_DATA->spr_i].path;
			
}

/**********************************************************************/

void	AI_COMMAND_GET_SPRITES_COORDS(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp3=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp4=AI_DATA->logic[AI_DATA->logic_index++];

				#if 0
				get_sprites_coordinates(AI_DATA->spr_i,&sprites[AI_DATA->spr_i].obj[AI_DATA->temp1],
												  &sprites[AI_DATA->spr_i].obj[AI_DATA->temp2],
												  &sprites[AI_DATA->spr_i].obj[AI_DATA->temp3],
												  &sprites[AI_DATA->spr_i].obj[AI_DATA->temp4]);
				#endif
}

/**********************************************************************/

void	AI_COMMAND_ADD_POLO(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp3=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp4=AI_DATA->logic[AI_DATA->logic_index++];
				
				#if 0
				add_polo(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1],	 
							sprites[AI_DATA->spr_i].obj[AI_DATA->temp2],
							sprites[AI_DATA->spr_i].obj[AI_DATA->temp3],
							sprites[AI_DATA->spr_i].obj[AI_DATA->temp4]);
				#endif
}

/**********************************************************************/

void	AI_COMMAND_ADD_LINE(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp3=AI_DATA->logic[AI_DATA->logic_index++];
				
				#if 0
				add_a_glow_line_clipped(sprites[AI_DATA->spr_i].obj[AI_DATA->temp1],	 
												sprites[AI_DATA->spr_i].obj[AI_DATA->temp2],
												sprites[AI_DATA->spr_i].obj[AI_DATA->temp3]);
				#endif
}

/**********************************************************************/

void	AI_COMMAND_GET_RET(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=sprites[AI_DATA->spr_i].ret_ptr;
}

/**********************************************************************/

void	AI_COMMAND_SET_RET(AI_DATATYPE *AI_DATA)
{
	 	sprites[AI_DATA->spr_i].ret_ptr	=	AI_DATA->logic[AI_DATA->logic_index++];
}

/**********************************************************************/

void	AI_COMMAND_GET_REF_POINT(AI_DATATYPE *AI_DATA)
{
				//AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				//AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				//AI_DATA->temp3=AI_DATA->logic[AI_DATA->logic_index++];
				//get_reference_point(AI_DATA->spr_i,AI_DATA->temp1,AI_DATA->temp2,AI_DATA->temp3);
}

/**********************************************************************/

void	AI_COMMAND_OUTPUT_VAR(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				AI_DATA->temp2=AI_DATA->logic[AI_DATA->logic_index++];
				//debug_variable[AI_DATA->temp1]=sprites[AI_DATA->spr_i].obj[AI_DATA->temp2];
}

/**********************************************************************/

void	AI_COMMAND_GET_LEVEL(AI_DATATYPE *AI_DATA)
{
				AI_DATA->temp1=AI_DATA->logic[AI_DATA->logic_index++];
				sprites[AI_DATA->spr_i].obj[AI_DATA->temp1]=level_no;
}

/**********************************************************************/

void	AI_COMMAND_SET_LEVEL(AI_DATATYPE *AI_DATA)
{
				level_no=AI_DATA->logic[AI_DATA->logic_index++];
				change_level=TRUE;
				escape=TRUE;
}

/**********************************************************************/

