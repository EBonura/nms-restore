/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Phillip Trelford's Read Level Data */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include	"globals.h"

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Load Level */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

fnames_list level_fnames[]={
	"LEVEL1",
	"LEVEL2",
	"LEVEL3",
	"LEVEL4",
	"LEVEL5",
	"LEVEL6",
	"LEVEL7",
	"LEVEL8",
	"LEVEL9"
};

fnames_list coll_fnames[]={
	"LEVEL1C",
	"LEVEL2C",
	"LEVEL3C",
	"LEVEL4C",
	"LEVEL5C",
	"LEVEL6C",
	"LEVEL7C",
	"LEVEL8C",
	"LEVEL9C"
};

char *get_level_name(long i )
{
	return &level_fnames[i].fname;
}

ball_st init_ball[2]={ {160,100,0,0,100},{160,125.7,0,0,50}};

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

	craftx=250; crafty=431;
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
	if(craft==-1) craft=add_sprite(NAME_CSHIP,ACTION_NONE,craftx,crafty);
	set_ai_none(craft);

	frac_craft_x=craftx; frac_craft_y=crafty;
	sprites[craft].priority=5;
	sprites[craft].clipped=NO;

	ball[0].x=craftx;
	ball[0].y=crafty;
	ball[0].vx=0;
	ball[0].vy=0;
}

void load_level(char *ffname)
{
	long  i,l1,l2;
	char	fname[128];
	unsigned char  *temp_buf;
	
	level_no=0;

	load_level_paf(ffname);

	load_level_spj_text(ffname);

	for(i=0;i<9;i++)
	{
		l1=strlen(level_fnames[i].fname);
		l2=strlen(bg_fname);
		if(l1==l2) if(strnicmp(&level_fnames[i].fname,bg_fname,l1)==0)
		{
			level_no=i;
		}
	}
	sprintf(message,"Loading Level no=%i\n",level_no);
	internal_message(message);

	load_sprites_resource(level_fnames[level_no].fname);

	load_attributes();

	load_level_map_text(ffname);

	internal_message("Loading Collision...\n");

	sprintf(fname,"GFX/LEVELS/%s.BIN",coll_fnames[level_no].fname);
	coll_buf=heap_load_file(fname);

	internal_message("Loading Palette...\n");

	sprintf(fname,"GFX/LEVELS/%s.PAL",level_fnames[level_no].fname);
	temp_buf=heap_load_file(fname);
	memcpy(iff_palette,temp_buf,256*3);
	heap_free_last();

	internal_message("Loading Blocks...\n");

	sprintf(fname,"GFX/LEVELS/%s.BIN",level_fnames[level_no].fname);
	blk_buf=heap_load_file(fname);

//	printf("Clearing block\n");
//	memset(blk_buf,0,256);

	internal_message("Loading Map...\n");

	sprintf(fname,"GFX/LEVELS/%s.MAP",level_fnames[level_no].fname);
	temp_buf=heap_load_file(fname);
							  			  
	iff_width=(temp_buf[0])+(temp_buf[1]<<8);
	iff_width=iff_width*blk_dim;
	iff_height=(temp_buf[2])+(temp_buf[3]<<8);
	iff_height=iff_height*blk_dim;
	map=(long *) &temp_buf[4];
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Script processing */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag	first_sprite_entry;
long	sprite_entry_index;

flag process_line(char *source,long length);

flag process_script(char *fname);

flag load_level_paf(char *fname)
{
	FILE *fp;

	sprintf(line,"%s.paf",fname);
	if((fp = fopen( line , "rb" )) == NULL ) return FAIL;
	else fclose(fp);
 
	path_data=heap_load_file(line);

	return OK;
}

flag load_level_spj_text(char *fname)
{
	FILE *fp;

	sprintf(line,"%s.spj",fname);
	if((fp = fopen( line , "rb" )) == NULL ) return FAIL;
	else fclose(fp);
	process_script(line);

	return OK;
}

flag load_level_map_text(char *fname)
{
	FILE *fp;

	first_sprite_entry=TRUE;
	sprintf(line,"%s.map",fname);
	if((fp = fopen( line , "rb" )) == NULL ) return FAIL;
	else fclose(fp);
	process_script(line);

	return OK;
}

flag load_level_sprites(char *fname)
{
	FILE *fp;

	sprintf(line,"%s.spj",fname);
	if((fp = fopen( line , "rb" )) == NULL ) return FAIL;
	else fclose(fp);
	process_script(line);

	sprintf(line,"%s.map",fname);
	if((fp = fopen( line , "rb" )) == NULL ) return FAIL;
	else fclose(fp);
	process_script(line);

	return TRUE;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Process Text Level Script */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag process_script(char *fname)
{
	long linestart,linelength;
	long i;
	char c;

	script=(char *) heap_load_file(fname);
	
	lineno=0;
	i=0;
	linestart=0;
	linelength=0;

	while(i<fsize)
	{
		c=script[i++];
		if( c==13 || c==10 )
		{
			if( script[i]==13 || script[i]==10 ) i++;
			if(linelength>0) strncpy(line,&script[linestart],linelength);
			line[linelength]=0;	
			//printf("Line %4i: %s\n",lineno,line);
			process_line(&script[linestart],linelength);
			lineno++;
			linestart=i;
			linelength=0;
		}
		else linelength++;
	}	

	heap_free_last();

	return TRUE;
}

enum {
  		ENTRY_SPRITE_INFO,
  		ENTRY_NUM_SPRITES,
  		ENTRY_SAVE_FORMAT_VERSION,
  		ENTRY_SPRITENAME,
  		ENTRY_SPRITEDIR,
  		ENTRY_SCRIPTDIR,
  		ENTRY_PALETTE,
		ENTRY_MAPFILE,
		ENTRY_BACKGROUND,
		ENTRY_END
		};


typedef struct
{
	char *string;
} strings;

strings entry_names[ENTRY_END]={
	"SPRITE_INFO",
	"NUM_SPRITES",
	"SAVE_FORMAT_VERSION",
	"SPRITENAME",
	"SPRITEDIR",
	"SCRIPTDIR",
	"PALETTE",
	"MAPFILE",
	"BACKGROUND",
	};

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Process Script Line */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag process_line(char *source,long length)
{
	long i,sprite_no;
	long index;
	long entry;
	long entry_length;
	long version;
	long x,y;
	char fname[8+1];
	long normal_startframe,normal_endframe;
	long death_startframe,death_endframe;
	long wounded_startframe,wounded_endframe;
	long up_startframe,up_endframe;
	long down_startframe,down_endframe;
	long left_startframe,left_endframe;
	long right_startframe,right_endframe;
	long speed_delay;
	long damage;
	long hit_points;
	long priority;
	long score;
	long start_delay;
	long random_startdelay;
	long initial_startframe;
	long initial_variance;
	long clipped;
	long playtype;
	long special_startframe;
	long special_endframe;
	long branch1,branch2,branch3,branch4;
	long path_speed;

	entry=0;
	while(entry<ENTRY_END)
	{
		entry_length=strlen(entry_names[entry].string);	
		if(length>entry_length) 
		{
			if(strncmp(entry_names[entry].string,source,entry_length)==0) break;
		}
		entry++;
	}
	length-=entry_length;
	index=entry_length;
	
	if( length<index) return FAIL;
	if( source[index++]!='=') return FAIL;

	switch (entry)
	{
		case ENTRY_SPRITE_INFO:
			if(first_sprite_entry==TRUE)
			{
				first_sprite_entry=FALSE;
				sprite_entry_index=0;
			}
			else sprite_entry_index++;
			memset(&fname,0,9);
			sscanf(&source[index],"%s",fname);
			index+=strlen(fname);
			sscanf(&source[index],"%i,%i,"			\
										 "%i,%i,"			\		
										 "%i,%i,"			\
										 "%i,%i,"			\
										 "%i,%i,"			\
										 "%i,%i,"			\
										 "%i,%i,"			\
										 "%i,%i,"			\
										 "%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,"	\
										 "%i,%i,"				\
										 "%i,%i,%i,%i,%i",
						&x,&y,
						&normal_startframe,&normal_endframe,
						&death_startframe,&death_endframe,
						&wounded_startframe,&wounded_endframe,
						&up_startframe,&up_endframe,
						&down_startframe,&down_endframe,
						&left_startframe,&left_endframe,
						&right_startframe,&right_endframe,
						&speed_delay,
						&hit_points,
						&priority,
						&damage,
						&start_delay,
						&random_startdelay,
						&initial_startframe,
						&initial_variance,
						&clipped,
						&score,
						&playtype,
						&special_startframe,&special_endframe,
						&branch1,
						&branch2,
						&branch3,
						&branch4,
						&path_speed
						);
				//printf("%s %i %i\n",fname,x,y);
				for(i=0;i<NAME_END;i++)
				{						
//					if(strnicmp(&sprite_fnames[i].fname,fname,8)==0)
					if(strnicmp(get_sprite_name(i),fname,8)==0)
					{				
						if(branch1!=-1) sprite_no=add_sprite(i,ACTION_PATH,x,y);
						else
						{
							if(playtype==0) sprite_no=add_sprite(i,ACTION_CYCLE,x,y);
							else sprite_no=add_sprite(i,ACTION_PINGPONG,x,y);
						}
						if(sprite_no!=-1)
						{
							sprites[sprite_no].framecounter=start_delay+rand()&3;
							sprites[sprite_no].frame=normal_startframe;
							sprites[sprite_no].startframe=normal_startframe;
							sprites[sprite_no].endframe=normal_endframe;

							sprites[sprite_no].normal_startframe=normal_startframe;
							sprites[sprite_no].normal_endframe=normal_endframe;

							sprites[sprite_no].left_startframe=left_startframe;
							sprites[sprite_no].left_endframe=left_endframe;

							sprites[sprite_no].right_startframe=right_startframe;
							sprites[sprite_no].right_endframe=right_endframe;

							sprites[sprite_no].up_startframe=up_startframe;
							sprites[sprite_no].up_endframe=up_endframe;

							sprites[sprite_no].down_startframe=down_startframe;
							sprites[sprite_no].down_endframe=down_endframe;

							sprites[sprite_no].wounded_startframe=wounded_startframe;
							sprites[sprite_no].wounded_endframe=wounded_endframe;

							sprites[sprite_no].death_startframe=death_startframe;
							sprites[sprite_no].death_endframe=death_endframe;

							sprites[sprite_no].priority=priority;
							sprites[sprite_no].clipped=clipped;
							sprites[sprite_no].map_index=sprite_entry_index;
					
							sprites[sprite_no].path=branch1;
							sprites[sprite_no].node=branch1;

							//printf("%i,%s,%i,%i,%i,%i,%i\n",sprite_entry_index,fname,branch1,branch2,branch3,branch4,path_speed);

						}
						break;
					}
				}
				if(i==NAME_END)
				{ 
					sprintf(message,"Unknown Sprite %s\n",fname);
					internal_message(message);
				}
				break;
		case ENTRY_SAVE_FORMAT_VERSION:
			sscanf(&source[index],"%i",&version);
			printf("Version %i\n",version);
			break;
		case ENTRY_BACKGROUND:
			memset(&fname,0,9);
			sscanf(&source[index],"%s",bg_fname);
			sprintf(message,"Background=%s\n",bg_fname);
			internal_message(message);
			length+=entry_length;
			// make this a function...
			entry_length=0;
			while(length>index)
			{
				bg_fname[entry_length]=source[index];
				if( source[index]=='.' ) bg_fname[entry_length]=0;
				entry_length++;
				if( source[index]=='\\' ) entry_length=0;
				index++;
			}
			break;
	}

	return OK;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Read an Origin File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void read_origin(char *fname)
{
	char *source=load_file(fname);
	short *sourcew;
	long	i,j;
	long  no_of_collisions;		  
	long  no_of_references;

	for(j=0;j<4;j++)
	{
	sourcew=(short * ) source;

	printf("grab number %i\n",sourcew[0]);
	printf("x origin %i\n"	 ,sourcew[1]);
	printf("y origin %i\n"   ,sourcew[2]);
	printf("\n");
	
	no_of_collisions=(long ) sourcew[3];

	printf("number of collisions %i\n",no_of_collisions);
	source+=8;
	sourcew=(short *) source;

	// What does all this mean...

	for(i=0;i<no_of_collisions;i++)
	{
		printf("Coll %i %i %i %i\n",sourcew[i*4+0],
											 sourcew[i*4+1],
											 sourcew[i*4+2],
									  		 sourcew[i*4+3]);
	}

	source+=no_of_collisions*8;
	sourcew=(short * ) source;
	printf("Extra word%i\n",sourcew[0]);
	source+=2;
	sourcew=(short * ) source;
	no_of_references=(long ) sourcew[0];
	
	printf("number of references %i\n",no_of_references);

	sourcew++;
	for(i=0;i<no_of_references;i++)
	{
		printf("X %i,Y %i\n",sourcew[0],sourcew[1]);
		sourcew+=2;
	}

	source=(char *) sourcew;
	}
}
