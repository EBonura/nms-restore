/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Window System */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Desirable enhancements:-
// Button boxes...
// Replacement simplified system to add_parameter_to_string
// which would also allow features to be editable...
// Window removement
// Window movement
// Window closing
// Button boxes/Slide Bars (events)
// File Selector...

#include "globals.h"

unsigned char *font6x8;
unsigned long *font6x8_table;
unsigned char *font6x8_data;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Window Structures*/
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define MAX_WINDOWS 			4
#define MAX_WINDOW_WIDTH	32
#define MAX_WINDOW_HEIGHT	24
#define CHARACTER_WIDTH		6
#define CHARACTER_HEIGHT	8
#define PEN_COLOUR			(6*16)
#define BACKGROUND_COLOUR	0

typedef struct  {
	flag	used;
	long	x;
	long	y;
	long	bx[2];
	long	by[2];
	long	width;
	long	height;
	unsigned char	*text;
} window;

window windows[MAX_WINDOWS];
char window_text[MAX_WINDOWS*MAX_WINDOW_WIDTH*MAX_WINDOW_HEIGHT];
char *windows_gfx;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Write Box 6x8*/
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void write_box_6x8(char *source,unsigned char *dest,long width,long height)
{
	long down,along;
	long i,y,si,di;
	unsigned char b,c,m,w;

	memset(dest,BACKGROUND_COLOUR,
			 (MAX_WINDOW_WIDTH*CHARACTER_WIDTH)*(MAX_WINDOW_HEIGHT*CHARACTER_HEIGHT));

	for(down=0;down<height;down++)
	{
		for(along=0;along<width;along++)
		{
			c=*source++;
  			if(c!=' ')
			{
				i=(int ) c;
				si=font6x8_table[i];
	  
				di=(down*CHARACTER_HEIGHT*width*CHARACTER_WIDTH)+along*CHARACTER_WIDTH;

				for(y=0;y<8;y++)
				{
					b=128;
				 	m=font6x8_data[si++];
					for(i=0;i<CHARACTER_WIDTH;i++)
					{
   					if((m&b)!=0) w=PEN_COLOUR;
						else w=BACKGROUND_COLOUR;

						dest[(di+i)]=w;
						b=b>>1;
					}
					di+=width*CHARACTER_WIDTH;
				}
			}
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Initialise Windows */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void init_windows(void)
{
	long i;

	printf("Initialising windows\n");

	font6x8=heap_load_file("GRABFONT\\OUT.CHR");
	font6x8_table=(unsigned long *) font6x8;
	font6x8_data=font6x8+256*4;

	windows_gfx=heap_malloc( (MAX_WINDOW_WIDTH*CHARACTER_WIDTH)*(MAX_WINDOW_HEIGHT*CHARACTER_HEIGHT)*4 );

	for(i=0;i<MAX_WINDOWS;i++)
	{ 
		windows[i].used=FALSE;
		windows[i].bx[0]=-1;
		windows[i].bx[1]=-1;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Create Window */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long create_window(unsigned char *source,long width,long height,long x,long y)
{
	long i,j;
	
	i=-1;

	for(j=0;j<MAX_WINDOWS;j++)
	{
		if(windows[j].used==FALSE) i=j;
	}

	i=0;								// cludge to window 0

	if(i!=-1)
	{
		windows[i].used=YES;
		windows[i].x=x;
		windows[i].y=y;

		windows[i].width =width;
		windows[i].height=height;

		windows[i].text=source;
	
		write_box_6x8(windows[i].text,
						  windows_gfx,
						  windows[i].width,
						  windows[i].height);
	}

	return i;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Release Window */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Draw Windows */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long get_screen_address(long x,long y)
{
	y-=bufy[buf];
	if(safe_mode==YES)
	{ 
		x-=(bufx[buf]);
		y+=logical_screen;
	}
	else
	{
		y+=y_mod[(bufy[buf])]+logical_screen+split_height;
	}

	return ((y*SVGA_logical_length)+x);
}

long get_removal_screen_address(long x,long y)
{
	if(safe_mode==YES) y-=bufy[buf];
	else y-=old_scroll_y;
	if(safe_mode==YES)
	{ 
		x-=bufx[buf];
		y+=logical_screen;
	}
	else
	{
		y+=(y_mod[old_scroll_y]+logical_screen+split_height);
	}

	return ((y*SVGA_logical_length)+x);
}


void draw_windows(void )
{
	char *source;
	long i,x,y,iy;
	long dest,width;

	for(i=0;i<MAX_WINDOWS;i++)
	{
		if(windows[i].used==YES)
		{
			width=windows[i].width*CHARACTER_WIDTH;
			source=windows_gfx;
			x=windows[i].x;
			y=windows[i].y;
			x+=bufx[buf];
			y+=bufy[buf];
			windows[i].bx[buf]=x;
			windows[i].by[buf]=y;

			for(iy=0;iy<(windows[i].height*CHARACTER_HEIGHT);iy++)
			{
				dest=get_screen_address(x,y);
				asm_add_andor_line(source,dest,width);
				y++;
				source+=width;
			}
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Remove Windows */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void remove_windows(void )
{
	char *source;
	long i,x,y,iy;
	long dest,width;

	for(i=0;i<MAX_WINDOWS;i++)
	{
		if(windows[i].used==YES)
		{
			if(windows[i].bx[buf]!=-1)
			{
				width=windows[i].width*CHARACTER_WIDTH;
				x=windows[i].bx[buf];
				y=windows[i].by[buf];

				for(iy=0;iy<(windows[i].height*CHARACTER_HEIGHT);iy++)
				{
					source=dump_blk_buf+x+(y_mod640[y]);
					dest=get_removal_screen_address(x,y);
					asm_add_dump_line(source,dest,width);
					y++;
				}
			}
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Create Text Window */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void copy_string(char *source,char *dest)
{
	char c;

	do
	{
		c=*source++;
		*dest++=c;
	}
	while(c!=0);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Add string to list */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void add_parameter_to_string(char *source,char *dest)
{
	char c,d;

	// Find parameter space

	do 
	{
		c=*dest;  				// -> window
		if(c=='#') break; 	// -> parameter
		dest++;
	} while(c!=0);

	// Copy parameter

	do
	{
		c=*dest;
		if(c!='#') break;
		d=*source++;
		if(d==0) break;
		*dest++=d;
	} while (c!=0);

	// Pad to end of parameter

	while (c=='#')
	{
	 	*dest++=' ';
		c=*dest;
	};
}

void add_parameters_to_string(char *source,char *dest)
{
	char parameter[MAX_WINDOW_WIDTH];
	char c;
	long si,di;
	
	// Add parameters seperated by ','s
	
	si=0;
	do
	{
		// Add parameter seperated by ',' or 0

		di=0;
		do
		{
			c=source[si++];
			if(c==',') break;
			parameter[di++]=c;
		} while(c!=0);

		parameter[di]=0;

		add_parameter_to_string(parameter,dest);

	} while(c!=0);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Use Windows */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

char window_query[]={ 		\
	"…¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬ª" \ 
	"« + Sprite Query +  ∂"	\ 
	"«                   ∂"	\ 
	"«Name:     ######## ∂"	\ 
	"«I:        ######## ∂"	\ 
	"«X:        ######## ∂"	\ 
	"«Y:        ######## ∂"	\ 
	"«Temp 1:   ######## ∂"	\ 
	"«Temp 2:   ######## ∂"	\ 
	"«Temp 3:   ######## ∂"	\ 
	"«Temp 4:   ######## ∂"	\ 
	"«Temp 5:   ######## ∂"	\ 
	"«Temp 6:   ######## ∂"	\ 
	"«Temp 7:   ######## ∂"	\ 
	"«Temp 8:   ######## ∂"	\ 
	"«Temp 9:   ######## ∂"	\ 
	"«Temp A:   ######## ∂"	\ 
/*
	"«Normal:   ######## ∂"	\
	"«Last:     ######## ∂"	\
	"«Wounded:  ######## ∂"	\
	"«Last:     ######## ∂"	\
	"«Death:    ######## ∂"	\
	"«Last:     ######## ∂"	\
*/
	"«                   ∂"	\ 
	"»¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡º" \
};

void create_sprite_query_window(long i )
{
	copy_string(window_query,window_text);

	add_parameter_to_string(get_sprite_name(sprites[i].name),window_text);

	sprintf(message,"%i",i);
	add_parameter_to_string(message,window_text);

	sprintf(message,"%i,%i",sprites[i].x,sprites[i].y);
	add_parameters_to_string(message,window_text);

	/*
	sprintf(message,"%i,%i",sprites[i].normal_startframe,sprites[i].normal_endframe);
	add_parameters_to_string(message,window_text);

	sprintf(message,"%i,%i",sprites[i].wounded_startframe,sprites[i].wounded_endframe);
	add_parameters_to_string(message,window_text);

	sprintf(message,"%i,%i",sprites[i].death_startframe,sprites[i].death_endframe);
	add_parameters_to_string(message,window_text);
	*/

	sprintf(message,"%i,%i",sprites[i].obj[obj_temp1],sprites[i].obj[obj_temp2]);
	add_parameters_to_string(message,window_text);

	sprintf(message,"%i,%i",sprites[i].obj[obj_temp3],sprites[i].obj[obj_temp4]);
	add_parameters_to_string(message,window_text);

	sprintf(message,"%i,%i",sprites[i].obj[obj_temp5],sprites[i].obj[obj_temp6]);
	add_parameters_to_string(message,window_text);

	sprintf(message,"%i,%i",sprites[i].obj[obj_temp7],sprites[i].obj[obj_temp8]);
	add_parameters_to_string(message,window_text);

	sprintf(message,"%i,%i",sprites[i].obj[obj_temp9],sprites[i].obj[obj_temp10]);
	add_parameters_to_string(message,window_text);

	create_window(window_text,21,19,160,120);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
