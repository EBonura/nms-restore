/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
// Saturn Level processing
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include	<stdio.h>
#include <sgl.h>
#include "defines.h"
#include "types.h"
#include	"globals.h"
//#include "names.h"

#define	MAX_SPRITES 256

/* ProtoTypes *****************************************************/

void	init_lists(void);
flag 	process_script(void);
flag 	process_line(char *source,long length);
flag 	process_linedata(Uint32 *data);
void	validate_sprite_list(void);

/* Globals ********************************************************/

//long				fsize;
char 				*script;

//spritestruct 	sprites[MAX_SPRITES];


spriteinfo		info_index[MAX_SPRITES];
attribstruc 	sprites_attribs[NAME_END];

flag				first_sprite_entry	=	TRUE;
long				sprite_entry_index	=	0;

extern	BALL ball[];

BALL init_ball[2]=
{
	{toFIXED(160),toFIXED(100),toFIXED(0),toFIXED(0),toFIXED(100)  }, 
	{toFIXED(160),toFIXED(125.7),toFIXED(0),toFIXED(0),toFIXED(50) }  
};

/* Externals ******************************************************/

extern	void 				process_sprites(void);
extern	void 				clear_sprites(void);
extern	long 				add_sprite(int name,int action,int x,int y);
extern	int				rand(void);
extern	void				ReadFile(Uint8 *name,Uint8 *target);
extern	void 				set_ai_none(long i);
extern	void 				set_ai_cycle(long i);

extern	char				*fnames_new[NAME_END];
extern	Uint32			Goffset[];
extern	Uint8				BigBuffer[];		//GFX Load area

extern	void 				msg(char *ptr);
extern	void 				clearmsg(void);
extern	Uint32			MaxSpritesLoaded;
extern	char				stat_str[];
extern	long				frac_craft_x,frac_craft_y;
extern	spritestruct 	sprites[];
extern	Uint16			sprites_still[];

/* Misc ********************************************************/

enum
{
	OFFSET_NAME=0,
	OFFSET_X,
	OFFSET_Y,
	OFFSET_NORMAL_STARTFRAME,
	OFFSET_NORMAL_ENDFRAME,
	OFFSET_DEATH_STARTFRAME,
	OFFSET_DEATH_ENDFRAME,
	OFFSET_WOUNDED_STARTFRAME,
	OFFSET_WOUNDED_ENDFRAME,
	OFFSET_UP_STARTFRAME,
	OFFSET_UP_ENDFRAME,
	OFFSET_DOWN_STARTFRAME,
	OFFSET_DOWN_ENDFRAME,
	OFFSET_LEFT_STARTFRAME,
	OFFSET_LEFT_ENDFRAME,
	OFFSET_RIGHT_STARTFRAME,
	OFFSET_RIGHT_ENDFRAME,
	OFFSET_SPEED_DELAY,
	OFFSET_HIT_POINTS,
	OFFSET_PRIORITY,
	OFFSET_DAMAGE,
	OFFSET_START_DELAY,
	OFFSET_RANDOM_STARTDELAY,
	OFFSET_INITIAL_STARTFRAME,
	OFFSET_INITIAL_VARIANCE,
	OFFSET_CLIPPED,
	OFFSET_SCORE,
	OFFSET_PLAYTYPE,
	OFFSET_SPECIAL_STARTFRAME,
	OFFSET_SPECIAL_ENDFRAME,
	OFFSET_BRANCH1,
	OFFSET_BRANCH2,
	OFFSET_BRANCH3,
	OFFSET_BRANCH4,
	OFFSET_PATH_SPEED,
	OFFSET_END
};

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Main procedure */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void place_craft(void )
{
	long i;
	
	for(i=0;i<2;i++)
	{
		ball[i].x=init_ball[i].x;
		ball[i].y=init_ball[i].y;
		ball[i].vx=init_ball[i].vx;
		ball[i].vy=init_ball[i].vy;
		ball[i].mass=init_ball[i].mass;
	}

	//default placement
	craftx=(125); crafty=(70);

	craft=-1;
	for(i=0;i<MAX_SPRITES;i++)
	{
		if(sprites[i].on==TRUE)
		{
			if(sprites[i].name==NAME_CSHIP)
			{
				craft=i;
				craftx=sprites[i].x;
				crafty=sprites[i].y;
			}
		}
	}

	if(craft==-1)
	{
	 	craft=add_sprite(NAME_CSHIP,ACTION_NONE,craftx,crafty);
	}			
	
	if(craft!=-1)
	{
		set_ai_none(craft);
	}

	frac_craft_x=craftx; frac_craft_y=crafty;
	sprites[craft].priority=5;
	sprites[craft].clipped=NO;

	sprites[craft].obj[obj_temp1]=0;

	ball[0].x=(craftx<<16);
	ball[0].y=(crafty<<16);
	ball[0].vx=0;
	ball[0].vy=0;

}

/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void	init_lists(void)
{
	Uint16	i;

	//Reset the names list
	for(i=0;i<NAME_END;i++)		
	{
		seqdata[i].data		=	(char *)0xffffffff;
		seqdata[i].frames		=	0;
		seqdata[i].offset		=	0;
	}

	seqdata[NAME_BUCK01].defined			=	YES;	
	seqdata[NAME_CHAIN3].defined			=	YES;
	seqdata[NAME_EBALL].defined			=	YES;
	seqdata[NAME_GUIDED].defined			=	YES;
	seqdata[NAME_mine2].defined			=	YES;
//	seqdata[NAME_exp1].defined				=	YES;	//shafted
	seqdata[NAME_exp2].defined				=	YES;
	seqdata[NAME_exp3].defined				=	YES;
	seqdata[NAME_exp4].defined				=	YES;
//	seqdata[NAME_exp5].defined				=	YES;	//shafted
	seqdata[NAME_exp6].defined				=	YES;
	seqdata[NAME_exp7].defined				=	YES;

}

/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

// Goes through the name array and loads the sprites which have had
// names assigned to them

flag load_level_sprites(char *fname)
{
	
	Uint16	i;
	Uint32	SizeCount	=	0;
	Uint8		*name;
	Uint8		*tgt;
	
	fsize	=	0;

	for(i=NAME_NULL;i<NAME_END;i++)
	{
		if( fnames_new[i] != (void *)-1 )		//Does this sprite have a file?
		{
			if(seqdata[i].defined==YES)			//is it in the map file
			{
				sprintf(stat_str,"NAME:%d - ",i);
				msg(stat_str);
				sprintf(stat_str,"sprites/%s",fnames_new[i]);
				name	=	stat_str;
				tgt	=	BigBuffer;
				tgt	+=	SizeCount;
				seqdata[i].data	=	(char *)(tgt);	//register offset to
															//start of sprite data

				ReadFile(name,tgt);
				SizeCount	+=	fsize;
			}
		}
	}

	if(SizeCount > LoadSize)
	{
		sprintf(stat_str,"DANGER!!! : Not enough RAM for sprites:%d\n Need %d bytes\n",(int)SizeCount,(int)(SizeCount-LoadSize));
		msg(stat_str);
	}
	else
	{
		sprintf(stat_str,">>>>>%d BYTES USED IN SPRITE DATA\n",(int)SizeCount);
		msg(stat_str);
	}
	
	return TRUE;

}

/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag process_script(void)
{
	volatile	long		NumEntries;
	Uint32	i=1,x;
	Uint32	*data	=	LowRam;

	//Set up the info index for the attributes
	for(x=0;x<MAX_SPRITES;x++)
	{
		info_index[x].normal_startframe	=	-1;
	}

	//Load the script file for the current level
	sprintf(stat_str,"levels/level%d.bmf",(Level+1));
	ReadFile(stat_str,(Uint8 *)data);
//	ReadFile("levels/level1.bmf",(Uint8 *)data);

	//get the number of sprite entries for this level
	NumEntries	=	data[0];

	//process each entry
	while(i<(NumEntries*OFFSET_PATH_SPEED))
	{
		process_linedata(data+i);
		i+=(OFFSET_END);
	}	

	sprintf(stat_str,"Active sprites:%d\n",(int)sprite_entry_index);
	msg(stat_str);

	return TRUE;
}

/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag	process_linedata(Uint32 *data)
{

	Sint32		sprite_no,i;
	long 			sprite_name;
	long 			x,y;
	long 			normal_startframe,normal_endframe;
	long 			death_startframe,death_endframe;
	long 			wounded_startframe,wounded_endframe;
	long 			up_startframe,up_endframe;
	long 			down_startframe,down_endframe;
	long 			left_startframe,left_endframe;
	long 			right_startframe,right_endframe;
	long 			speed_delay;
	long 			damage;
	long 			hit_points;
	long 			priority;
	long 			score;
	long 			start_delay;
	long 			random_startdelay;
	long 			initial_startframe;
	long 			initial_variance;
	long 			clipped;
	long 			playtype;
	long 			special_startframe;
	long 			special_endframe;
	long 			branch1,branch2,branch3,branch4;
	long 			path_speed;
	spriteinfo	*ii;


	//Get all the data for this sprite
	sprite_name			=	data[OFFSET_NAME];
	i						=	data[OFFSET_NAME];
	x						=	data[OFFSET_X]; 
	y						=	data[OFFSET_Y]; 
	normal_startframe	=	data[OFFSET_NORMAL_STARTFRAME]; 
	normal_endframe	=	data[OFFSET_NORMAL_ENDFRAME]; 
	death_startframe	=	data[OFFSET_DEATH_STARTFRAME]; 
	death_endframe		=	data[OFFSET_DEATH_ENDFRAME]; 
	wounded_startframe=	data[OFFSET_WOUNDED_STARTFRAME]; 
	wounded_endframe	=	data[OFFSET_WOUNDED_ENDFRAME]; 
	up_startframe		=	data[OFFSET_UP_STARTFRAME]; 
	up_endframe			=	data[OFFSET_UP_ENDFRAME]; 
	down_startframe	=	data[OFFSET_DOWN_STARTFRAME]; 
	down_endframe		=	data[OFFSET_DOWN_ENDFRAME]; 
	left_startframe	=	data[OFFSET_LEFT_STARTFRAME]; 
	left_endframe		=	data[OFFSET_LEFT_ENDFRAME]; 
	right_startframe	=	data[OFFSET_RIGHT_STARTFRAME]; 
	right_endframe		=	data[OFFSET_RIGHT_ENDFRAME]; 
	speed_delay			=	data[OFFSET_SPEED_DELAY]; 
	hit_points			=	data[OFFSET_HIT_POINTS]; 
	priority				=	data[OFFSET_PRIORITY]; 
	damage				=	data[OFFSET_DAMAGE]; 
	start_delay			=	data[OFFSET_START_DELAY]; 
	random_startdelay	=	data[OFFSET_RANDOM_STARTDELAY]; 
	initial_startframe=	data[OFFSET_INITIAL_STARTFRAME]; 
	initial_variance	=	data[OFFSET_INITIAL_VARIANCE]; 
	clipped				=	data[OFFSET_CLIPPED]; 
	score					=	data[OFFSET_SCORE]; 
	playtype				=	data[OFFSET_PLAYTYPE];
	special_startframe=	data[OFFSET_SPECIAL_STARTFRAME];
	special_endframe	=	data[OFFSET_SPECIAL_ENDFRAME];
	branch1				=	data[OFFSET_BRANCH1];
	branch2				=	data[OFFSET_BRANCH2];
	branch3				=	data[OFFSET_BRANCH3];
	branch4				=	data[OFFSET_BRANCH4];
	path_speed			=	data[OFFSET_PATH_SPEED];

	if(sprite_name>=NAME_END)
	{
		sprintf(stat_str,"BMF Error: BadSpriteName - %d\n",(int)sprite_name);
		msg(stat_str);
	}
	else
	{
		if(first_sprite_entry==TRUE)
		{
			first_sprite_entry=FALSE;
			sprite_entry_index=0;
		}
		else
		{
			sprite_entry_index++;
		}

		if(branch1!=-1)
		{
			sprite_no=add_sprite(i,ACTION_PATH,x,y);
		}
		else
		{
			if(playtype==0)
			{
				sprite_no=add_sprite(i,ACTION_CYCLE,x,y);
			}
			else
			{
				sprite_no=add_sprite(i,ACTION_PINGPONG,x,y);
			}
		}

		if(sprite_no!=-1)
		{
			sprites[sprite_no].framecounter=start_delay+(rand()&3);
			sprites[sprite_no].frame=normal_startframe;
			sprites[sprite_no].startframe=normal_startframe;
			sprites[sprite_no].endframe=normal_endframe;

			ii=&info_index[i];

			if(ii->normal_startframe==-1)
			{
				ii->normal_startframe	=	normal_startframe;
				ii->normal_endframe		=	normal_endframe;
				ii->left_startframe		=	left_startframe;
				ii->left_endframe			=	left_endframe;
				ii->right_startframe		=	right_startframe;
				ii->right_endframe		=	right_endframe;
				ii->up_startframe			=	up_startframe;
				ii->up_endframe			=	up_endframe;
				ii->down_startframe		=	down_startframe;
				ii->down_endframe			=	down_endframe;
				ii->wounded_startframe	=	wounded_startframe;
				ii->wounded_endframe		=	wounded_endframe;
				ii->death_startframe		=	death_startframe;
				ii->death_endframe		=	death_endframe;
			}

			sprites[sprite_no].priority	=	priority;
			sprites[sprite_no].clipped		=	clipped;
			sprites[sprite_no].map_index	=	sprite_entry_index;
			sprites[sprite_no].path			=	branch1;
			sprites[sprite_no].node			=	branch1;

		}

		if(seqdata[sprite_name].defined==NO) seqdata[sprite_name].defined=YES;
	}

	return OK;

}

/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void	validate_sprite_list(void)
{
	int	i,j,name;
	flag	found_still;

	for(i=0;i<NAME_END;i++)
	{
		//Check for sprites in the map file that I haven't loaded yet
		if(seqdata[i].defined==YES)
		{
			if(seqdata[i].data==(char *)0xffffffff)
			{
				j=0;found_still=FALSE;

				while(1)
				{
					name	=	sprites_still[j++];
					if(name==NAME_END) break;
					if(name==i)
					{
						found_still=TRUE;
						break;
					}
				}

				if(found_still==FALSE)
				{
					sprintf(stat_str,"Sprite %d defined but no data\n",i);
					msg(stat_str);
				}
			}
		}

		//Check for loaded sprites that are not used
		if(seqdata[i].defined==NO)
		{
			if(seqdata[i].data!=(char *)0xffffffff)
			{
				sprintf(stat_str,"Sprite %d not defined but HAS data\n",i);
				msg(stat_str);
			}
		}

		#if 0
		if(seqdata[i].defined==NO)
		{
			if(seqdata[i].data==(char *)0xffffffff)
			{
				sprintf(stat_str,"NON CRITICAL:No data or define for %d \n",i);
				msg(stat_str);
			}
		}
		#endif

	}
}

/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag	LoadAttributesData(void)
{
	int		x,entries;
	long		name,ai,sfx,score;
	flag		defined,shootable,bouncy;
	Uint8		*bpointer;
	Uint32	*lpointer=(Uint32 *)BigBuffer;
	Uint32	index=0;


	//Initialise the attributes list
	for(x=0;x<NAME_END;x++)
	{
		sprites_attribs[x].ai			=	-1;
		sprites_attribs[x].sfx			= 	0;
		sprites_attribs[x].score		=	0;
		sprites_attribs[x].defined		=	0;
		sprites_attribs[x].shootable	=	0;
		sprites_attribs[x].bouncy		=	0;
	}

	//Read in the attributes file
	ReadFile("bin/attribs.atr",(Uint8 *)BigBuffer);
	
	entries	=	lpointer[index++];

	for(x=0;x<entries;x++)
	{
		name		=	lpointer[(index++)];
		ai			=	lpointer[(index++)];
		sfx		=	lpointer[(index++)];
		score		=	lpointer[(index++)];

		bpointer	=	(Uint8 *)(lpointer+index);

		defined	=	bpointer[0];
		shootable=	bpointer[1];
		bouncy	=	bpointer[2];

		index++;

		if( (name>NAME_NULL) && (name<NAME_END) )
		{
			sprites_attribs[name].ai			=	ai;
			sprites_attribs[name].sfx			= 	sfx;
			sprites_attribs[name].score		=	score;
			sprites_attribs[name].defined		=	defined;
			sprites_attribs[name].shootable	=	shootable;
			sprites_attribs[name].bouncy		=	bouncy;
		}
		else
		{
			sprintf(stat_str,"Bad sprite in ATR data :%d\n",(int)name);
			msg(stat_str);
		}

	}
	
	return TRUE;
}

/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */




















	#if 0
	if(playtype==0) 
	{
		sprite_no=add_sprite(sprite_name,ACTION_CYCLE,x,y);
		ActiveSprites++;
	}
	else
	{
		sprite_no=add_sprite(sprite_name,ACTION_PINGPONG,x,y);
		ActiveSprites++;
	}

	if(sprite_no!=-1)
	{
		sprites[sprite_no].framecounter=(start_delay+(rand()&3));
		sprites[sprite_no].frame=normal_startframe;
		sprites[sprite_no].startframe=normal_startframe;
		sprites[sprite_no].endframe=normal_endframe;
		sprites[sprite_no].priority=priority;
		sprites[sprite_no].clipped=clipped;
	}
	#endif






