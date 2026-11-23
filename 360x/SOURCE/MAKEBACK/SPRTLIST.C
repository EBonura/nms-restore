/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Sprite List Handling Functions */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include "globals.h"

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SPRITE Structures */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long org_list[]={
	NAME_acidspt,
	NAME_l1c5,
	NAME_END,
};

fnames_list fnames_ian[]= {
			"NULL",
//			"SMSHIP",
//			"BULLSHIP",
			"BULLSHP2",
			"FIRE01",
			"cursor1",
			"_box",
			"bar_l",
			"bar_r",
			"pod4",
			"agmine",
			"chain3",
			"chaina",
			"chainb",
			"eball",
			"spfire",

			"GUIDED",
			"BUCK01",
			"EBALLL",
			"EBALL_M",
			"EBALL_S",
			"SMINE1",
			"SMINE2",
			"SMINE3",

			"R1_HIT",
			"R2_HIT",
			"R3_HIT",	
			"R4_HIT",
			"G1_HIT",
			"G2_HIT",
			"G3_HIT",
			"G4_HIT",
			"W1_HIT",
			"W2_HIT",
			"W3_HIT",	
			"W4_HIT",

 			"XB_FLOP1",
			"XB_FLOP2",
			"XB_FLOP3",
			"XB_FLOP4",
			"XB_FLOP5",
			"XB_FLOP6",

 			"NXA",
 			"NXB",
 			"NXC",
 			"NXD",
 			"NXE",
 			"NXF",
 			"NXG",
 			"NXH",
 			"NXI",
			
			"BORDER1",
			"SMWIRE",
			"GUIDWIRE",
			"EBWIRE",
			"MINIWIRE",
			"LASWIRE",
			"AGMWIRE",
			"BCKWIRE",

			"JELLO",

			"FLOPPY",
			"bouncer3",
			"s",
			"flawa",
			"nuray",

			"chisler",
			"stship2",
			"0pod",

			"flop2",
			"spiral3",
			"g2",
			"fork01",
			"fork02",
			"flawi",
			"mrspit",
			"jshel",
			"tomi",
			"float",

			"bug" ,
			"star",
			"starfsh",
			"turret1",
			"acidspt",
			"chapie",
			"twinspin",
			"l",
			"survrs",

			"FUEL2",
			"BOOM",
			"PLUNGER",
			"POD2",
			"SHIELD",
			"L",
			"G",

			"SPING",
			"SHELL",						 
	
			"FLOATER",
			"0HAIR",
			"BOUNCER1",
			"BOUNCER2",
			"FORK01",
			"FORK02",
			"BFIRE",
			"CPOD",
			"FLOAT",
			"MSCAT",
			"CLAW02",
			"FROG",
			"FUZZ",
			"RCK01",
			"RCK02",
			"RCK03",
			"BUG",
			"STAR",
			"TURRET1",
			"SPIRAL",
			"TENTAC1",
			"TENTAC2",
			"MINE",
			"MUSSEL",
			"RCKMORPH",
			"CHAPIE",
			"STARFSH",
			"ACIDSPT",

			"BORDSML",
			"BORDSML2",
			"BORDSML3",

			"SHIPWIRE",
			"SHIPWIRR",
			"SHIPWIR2",
			"SHIPWIR3",
			"SHIPWIR4",

			"MISSWIRE",
			"BARS01",

			"portdu",			
			"portuu",
			"portlu",
			"portru",
			"portul",
			"portur",
			"portdl",
			"portll",
			"portrl",
			"windy",

			"lmspit",
			"sperm",
			"sperm2",
			"windl2",

			"mine2",
			"windl4",
			"lazl4",
			"l42bt",

			"tentc1l5",
			"tentc2l5",			
			"turrl6",

			"fflop",
			"l2c1",
			"l2c2",
			"l2c3",
			"l2c4",
			"l2c5",
			"block1",
			"block2",
			"block3",
			"block4",
			"windtun",
			"bloba",
			"tad",
			"spfire",
			"l2rock1",

			"bordsml",
			"bordsml3",
			"overlay",
			"overly2",
			"shipwir2",

			"tubel1",
			"chapl1",
			"vinel11",
			"vines12",
			"vine13",
			"vine14",
			"vine15",
			"vine16",
			"s_l1c1",
			"s_l1c2",
			"s_l1c3",
			"s_l1c4",
			"s_l1c5",
			"tonsl1",
			"l1c1",
			"l1c2",
			"l1c3",
			"l1c4",
			"l1c5",
			"lazerl1",
			"msspit",
			"lrtad",

			"vinel21",			
			"vinel22",
			"vinel23",
			"vinel24",
			"vinel26",
			"s_l2c1",
			"s_l2c2",
			"s_l2c3",
			"s_l2c4",
			"s_l2c5",
			"l2tentl",
			"l2tentr",
			"plungl2",
			"chapl2",
			"lazerl2",
			"vtestl2",

			"vtestl4",
			"vinl41",
			"vinel42",
			"vinel43",
			"vinel44",
			"vinel46",
			"s_l4c1",
			"s_l4c2",
			"s_l4c3",
			"s_l4c4",
			"s_l4c5",
			"tubes",
			"l4rock1",
			"l4c1",
			"l4c2",
			"l4c3",
			"l4c4",
			"l4c5",
			"l4bt",
			"plungl4",
			"lazerl4",

			"vinel61",
			"vinel62",
			"vinel63",
			"vinel64",
			"vinel65",
			"vinel66",
			"s_l6c1",
			"s_l6c2",
			"s_l6c3",
			"s_l6c4",
			"s_l6c5",
			"l6c1",
			"l6c2",
			"l6c3",
			"l6c4",
			"l6c5",
			"l61bt",
			"l62bt",
			"l63bt",
		  	"l64bt",
			"l65bt",
			"l6rock1",
			"plungl6",
			"lazerl6",
			"podtemp",
			"chapl6",
			"vtestl6",
			"tonsl6",
			"tubel6",

			"vinel51",
			"vinel52",
			"vinel53",
			"vinel54",
			"vinel56",
			"s_l5c1",
			"s_l5c2",
			"s_l5c3",
			"s_l5c4",
			"s_l5c5",
			"l5rock1",
			"vtestl5",
			"l5c1",
			"l5c2",
		  	"l5c3",
			"l5c4",
			"l5c5",
			"lazerl5",
			"chapl5",
			"plungl5",
			"l51bt",
			"l52bt",
			"l53bt",
			"l54bt",
			"tonsl5",

			"vinel31",
			"vinel33",
			"vinel34",
			"vinel36",
			"s_l3c1",
			"s_l3c2",
			"s_l3c3",
			"s_l3c4",
			"s_l3c5",
			"l3rock1",
			"plungl3",
			"mussl3",
			"chapl3",
			"l3c1",
			"l3c2",
			"l3c3",
			"l3c4",
			"l3c5",
			"l31bt",
			"l32bt",
			"lazer3",
			"vtest3l",

			"l7c1",
			"l7c2",
			"l7c3",
			"l7c4",
			"l7c5",
			"l71bt",
			"l72bt",
			"l73bt",
			"l74bt",

			"l8c1",
			"l81bt",
			"l82bt",
			"l83bt",
			"l84bt",
	
		  	"l9c1",
			"l91bt",
			"l92bt",
			"l93bt",
			"l94bt",

			"lmrspit",

			"l2rock2",
			"l2rock3",

			"tubel3",
			"windl3",
			"l3rock2",
			"l3rock3",
			"l33bt",
			"turrl3",
			"tonsl3",
			"vtestfl3",
			"vinfl3_3",
			"vtestl3",

			"l4rock2",
			"l4rock3",

			"l5rock2",
			"l5rock3",

			"tentc1l6",
		  	"l6rock2",
			"l6rock3",

			"l7rock1",
			"l7rock2",
		  	"l7rock3",
			"l7rock4",

			"spermneo",
			"spermne2",
			"spermne3",

			"l8blob",
			"s_l8c3",

			"flipl8_1",
			"flipl8_d",
			"flipl8_u",
			"flipl8_r",

			"bugger",

			"donkey",
			"kong"

};			

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Return text name of sprite */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
	
char	*get_sprite_name(long i)
{
	return fnames_ian[i].fname;	
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Validate SPRITE LIST */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void validate_sprite_list(void )
{
	long	i,name;
	long	count,ptr;
	//long  *map;
	//short *sourcew;

	for(i=0;i<NAME_END;i++)
	{
 		seqdata[i].used=FALSE;

//		printf("%04i: %s\n",i,fnames_ian[i].fname);
		
	}

	for(i=0;i<MAX_SPRITES;i++)
	{
		if( sprites[i].on==TRUE )
		{
			name=sprites[i].name;
		 	if(seqdata[name].frames==-1)
			{
 				sprites[i].on=FALSE;
				printf("Error in sprite list: sprite %s not loaded\n",
							fnames_ian[name].fname);
			}
			else
			{
				seqdata[name].used=TRUE;
				sprites[i].frame=0;

				if(sprites[i].startframe<0)
				{
					sprintf(message,"Small startframe %s \n",get_sprite_name(name));
					internal_message(message);
				}
				if(sprites[i].startframe>(seqdata[name].frames-2))
				{
					sprintf(message,"Big startframe %s \n",get_sprite_name(name));
					internal_message(message);
				}
				if(sprites[i].endframe>(seqdata[name].frames-2))
				{
					sprintf(message,"Big endframe %s \n",get_sprite_name(name));
					internal_message(message);
				}

			/*
				map=seqdata[name].map;
				count=0;
				while((*map++)!=-1) count++;

				printf("NAME %s\n",fnames_ian[name].fname);
				printf("Sprite No= %i,%i,%i,%i,%i,%i\n",i,name,sprites[i].startframe,
							sprites[i].endframe,seqdata[name].frames,count);

				count--;
				for(;count>=0;count--)
				{
					ptr=seqdata[name].map[count];
					sourcew=(short *) (seqdata[name].data+ptr);
					printf("%i: %i,%i,%i,%i\n",count,sourcew[0],sourcew[1],
												  sourcew[2],sourcew[3]);
				}
			*/
			}
		}
	}
//	exit(-1);

	for(i=0;i<NAME_END;i++)
	{
		if(seqdata[i].frames>0)
		{
			if(seqdata[i].used==FALSE)
			{
				sprintf(message,"%s.SPH            is loaded unused sprite\n",fnames_ian[i].fname);
				internal_message(message);
			}		
		}	
	}

	sprintf(message,"Sprites used in level %s\n{\n",get_level_name(level_no));
	internal_message(message);
	for(i=0;i<NAME_END;i++)
	{
		if(seqdata[i].frames>0)
		{
			if(seqdata[i].used==TRUE)
			{
				sprintf(message,"\t\%s,\n",fnames_ian[i].fname);
				internal_message(message);
			}		
		}	
	}
	internal_message("}\n");

}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Load SPRITE RESOURCE */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void load_org(long name,char *fname);
void resource_load_sprite(long num,char *source);

char *sprites_res;

void load_sprites_resource(char *level_name)
{
	char *sourceb;
	long *sourcel;
	long files;
	long temp;
	long matched;
	long i,j,k;
	char name[8+1];
	
	sprintf(filename,"PHIL/%s/SPL.RES",level_name);
	sprites_res=heap_load_file(filename);

	if(sprites_res==NULL)
	{
		printf("ERROR: Unable to load sprites\n");
		SVGA_uninit();
		exit(-1);
	}

	sourcel=(long * ) sprites_res;
	files=*sourcel; 
	matched=0;
	
	for(i=1;i<NAME_END;i++)
	{
		sourceb=&fnames_ian[i].fname;
		j=0;
		while(sourceb[j]!=0)
		{
			if(sourceb[j]>='a' && sourceb[j]<='z') sourceb[j]+='A'-'a';
			j++;
		}
	
		temp=12*files+4;
		for(j=0;j<files;j++)
		{
			memcpy(name,sprites_res+4+(j*12),8);
			name[8]=0;
			if(strlen(fnames_ian[i].fname)==strlen(name))
			{
				k=strlen(fnames_ian[i].fname);
				if ( strncmp(fnames_ian[i].fname,name,k)==0 )
				{
					matched++;
					resource_load_sprite(i,sprites_res+temp);
					sprintf(filename,"PHIL/%s/%s.ORG",level_name,fnames_ian[i].fname);
					load_org(i,filename);
					memset(sprites_res+4+(j*12),0,8);		// what does this do?
				}
	 		}
			sourceb=sprites_res+4+(j*12)+8;
			sourcel=(long * ) sourceb;
			temp+=*sourcel;
		}

	}
	sprintf(message,"Matched=%i Files=%i\n",matched,files);
	internal_message(message);
}

void resource_load_sprite(long num,char *source)
{
	long i,frames;
	char 	*temp;
	short *tempw;
	long  *offsets;
	
	tempw=(short *) source;

	frames=*tempw;
	
	temp=source+2;
	offsets=(long *) temp;
	for(i=0;i<(frames-1);i++)
	{
	 	offsets[i]-=(2+frames*4);
	}

	seqdata[num].frames=frames;
	seqdata[num].map=(long *) (source+2);
	seqdata[num].data=source+2+frames*4;
}

void load_org(long name,char *fname)
{
	FILE *fp;
	long i,n;

	seqdata[name].org=NULL;
	i=0;
	while(i>=0)
	{
		n=org_list[i];
		if(n==NAME_END) break;
		if(n==name)
		{
			printf("%s loaded\n",fname);
			if( (fp=fopen(fname,"rb")) != NULL )
			{
				fclose(fp);
				seqdata[name].org=heap_load_file(fname);
				if(seqdata[name].org!=NULL) printf("Successfully\n");
			}
			break;
		}
		i++;
	}
}



/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Sprites On Screen? */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Takes a while...
// Find out which are onscreen first
// also find pre-store their onscreen (x1,y1,x2,y2)
// should use this info for remove_sprite
// Build up sorted linked list of priorities

long priority_used[16];

long nsprites_onscreen;
long onscreen_list[MAX_SPRITES];

// Scanning all the sprites takes quite a while...
// and adding their offsets

void sprites_onscreen(void)
{
	long	i,ptr;
	long	name,frame;
	long	priority;
	long	x,y;
	long	width,height;
	char	*source;
	short	*sourcew;

	memset(priority_used,0,16*4);

	nsprites_onscreen=0;

	for(i=0;i<MAX_SPRITES;i++)
	{
		sprites[i].buf_rem[buf]=NO;
		sprites[i].onscreen=FALSE;

		if( (sprites[i].on==TRUE) )
		{

		if(  (sprites[i].dying==0) &&
			 (sprites[i].visible==TRUE) )
		{
			// this has to be processed for every sprite
			// pretty time consuming...

			name=sprites[i].name;
			frame=sprites[i].frame;

			if(frame>=seqdata[name].frames)
			{
				printf("Sprite number too high %s\n",get_sprite_name(sprites[i].name));
//				exit(-1);
			}
			else
			{

				x=sprites[i].x;
				y=sprites[i].y;
				ptr=seqdata[name].map[frame];
				
				if(ptr==-1)
				{
					printf("Beyond last sprite\n");
					exit(-1);
				}

				source=seqdata[name].data;
				sourcew=(short *) (source+ptr);
				x+=(long ) sourcew[0];
				y+=(long ) sourcew[1];
				width=(long ) sourcew[2];
				height=(long ) sourcew[3];

//				sprites[i].x1=x;
				sprites[i].y1=y;
//				sprites[i].x2=x+width;
//				sprites[i].y2=y+height;
	
				x-=bufx[buf];
				y-=bufy[buf];

				if( y<viewable_height )						
				{
					if( x<viewable_width ) 	
					{
		 				if( (y+height)>clip_start_y )
						{
							if( (x+width)>clip_start_x )
							{
								priority=sprites[i].priority;
								priority_used[priority]++;
								onscreen_list[nsprites_onscreen++]=i;
								sprites[i].onscreen=TRUE;
							}
						}
					}
				}
			}
					
		}
		}
	}
}							   

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Sort sprites */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag	sprite_sorted[MAX_SPRITES];
int	sprite_order[MAX_SPRITES];

void sort_sprites(void)
{
	int i,j,k,a;
	int lowest_y,lowest_i;
	long priority;
	long y;

	sprites_onscreen();

	for(i=0;i<MAX_SPRITES;i++) sprite_sorted[i]=FALSE;
	
	k=0;

	for(priority=0;priority<16;priority++)
	{
	if(priority_used[priority]>0)
	{
		for(j=0;j<priority_used[priority];j++)	// scan all sprites in this priority set
		{
			lowest_y=-1;
			lowest_i=9999;
			for(a=0;a<nsprites_onscreen;a++)
			{
				i=onscreen_list[a];
				if( (sprites[i].priority==priority) && (sprite_sorted[i]==FALSE) )
				{
					y=sprites[i].y1;
					if(y>lowest_y)
					{
						lowest_y=y;
						lowest_i=i;
					}
				}
			}
			if(lowest_i!=9999) 
			{
				sprite_order[k++]=lowest_i;
				sprite_sorted[lowest_i]=TRUE;
			}
		}
	}							
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Draw Sprites To Screen */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
/*…ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕª
  ∫ Draw Sprites                                                           ∫
  ∫                                                                        ∫
  »ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕº*/

void draw_sprites(int buf)
{
	int i,j,x,y;
	int name,frame;

	sort_sprites();

	for(j=0;j<nsprites_onscreen;j++)
	{
		i=sprite_order[j];

		name=sprites[i].name;
		x=sprites[i].x;
		y=sprites[i].y;
		frame=sprites[i].frame;

		if(safe_mode==YES)
		{
			if(sprites[i].still==FALSE) sprites[i].buf_rem[buf]=YES;
		}
		else sprites[i].buf_rem[buf]=YES;
		sprites[i].buf_x[buf]=x;
		sprites[i].buf_y[buf]=y;
		sprites[i].buf_frame[buf]=frame;

		if(sprites[i].clipped==NO)
		draw_sprite(&seqdata[name],frame,
						x,y,
						bufx[buf],bufy[buf],
						logical_screen+split_height);
		else
		bgclip_sprite(&seqdata[name],frame,			// Takes a while
							x,y,
							bufx[buf],bufy[buf],
							logical_screen+split_height);
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Remove Sprites From Screen */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Only processes sprites previously onscreen

void remove_sprites(int buf)
{
	int i,x,y;
	int name,frame;

	for(i=0;i<MAX_SPRITES;i++)
	{
		//if(sprites[i].on==TRUE)
		if(sprites[i].buf_rem[buf]==YES)
		{
			name=sprites[i].name;
			x=sprites[i].buf_x[buf];
			y=sprites[i].buf_y[buf];
			frame=sprites[i].buf_frame[buf];

//			printf("Removed %i,%i,%i,%i,%i\n",i,name,x,y,frame);

			if(safe_mode==NO)
			{
			remove_sprite(&seqdata[name],frame,
							x,y,
							bufx[buf],old_scroll_y, //bufy[buf],
							logical_screen+split_height);
			}
			else
			{
			remove_sprite(&seqdata[name],frame,
							x,y,
							bufx[buf],bufy[buf],
							logical_screen+split_height);
			}
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */


typedef struct
{
 	int name;
	int action;
	int x;
	int y;
} levelstartstruct;


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Load SPRITE LIST */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void load_sprites(char *level_name )
{
	FILE *infile;
	int i;
	//long frames;
	//long *ptr;

	for(i=1;i<NAME_END;i++)
	{
//		at(20,0);
//		sprintf(filename,"GFX/SPRITES/%s.SPL",fnames_ian[i].fname);
		sprintf(filename,"PHIL/%s/%s.SPL",level_name,fnames_ian[i].fname);
//		at(21,0);
		printf("Loading %s\n",filename);

		seqdata[i].frames=-1;

		if( (infile=fopen(filename,"rb")) != NULL )
		{
			fclose(infile);
			printf("Opened file\n");
			printf("%i\n",i);
			load_sprite(i,filename);
		}
		else
		{
			printf("FUCKED!\n");
			
			/*
			sprintf(filename,"GFX/SPRITES/%s.MAP",fnames_ian[i].fname);
			seqdata[i].map=(int *) load_file(filename);

			ptr=(long *) seqdata[i].map;				// calc no. of frames
			frames=0;
			while(*ptr!=-1 && frames<(fsize/4)) frames++;
			seqdata[i].frames=frames;

			sprintf(filename,"GFX/SPRITES/%s.BIN",fnames_ian[i].fname);
			seqdata[i].data=load_file(filename);
			*/
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Load .SPH File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void load_sprite(long number,char *name )
{
	long i,frames;
	char *temp;
	short *tempw;
	long  *offsets;
	//char fname[80];

//	sprintf(fname,"GFX/SPRITES/%s.SPH",name);
//	sprintf(fname,"GFX/SPRITES/%s.SPL",name);
//	sprintf(fname,"PHIL/%s/%s.SPL",lname,name);

	temp=(char *) heap_load_file(name);
	tempw=(short *) temp;
	offsets=(long *) temp;

	frames=*tempw;
	//printf("Frames = %i\n",frames);
	
	offsets=(long *) (temp+2);
	for(i=0;i<(frames-1);i++)
	{
	 	offsets[i]-=(2+frames*4);
	}

	seqdata[number].start_address=temp;
	seqdata[number].frames=frames;
	seqdata[number].map=(long *) (temp+2);
	seqdata[number].data=temp+2+frames*4;
}

void add_sprites(levelstartstruct *data)
{
	int i=0;

	while(data[i].name!=NAME_END)
	{
		add_sprite(	data[i].name,
						data[i].action,
						data[i].x,
						data[i].y );
		i++;
	}
}


