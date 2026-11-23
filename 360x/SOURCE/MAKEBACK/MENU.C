/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Menu System */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Going to have to do something about it...
// Want a fast forward badly
// Lets put in the windows system...
// This is shit
// System for entering numbers me thinks...
// Enter number system...
// Need to change draw menu so it can be dynamically updated
// This sucks...
// Input of text works now...
// Get it to read in & write out all values as text...
// Array for text edit, or an editable field with a ptr to a variable...
// Would be nice to have a cursor...
// This would be preferable...

#include "globals.h"

long	current_fx;
flag  edit_mode;
long	current_option;
long	no_of_options;
char	input_string[128];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Display string */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void nice_string(unsigned char pen,int start_x,int start_y,char *string)
{
	int i,ptr;
	int x,y;
	int width;
	unsigned char b,m;
	unsigned char c;
	while(*string!=0)
	{
		c=*string++;
		if(c==' ') start_x+=12;
		else
		{
		i=(int ) c;
		ptr=font_table[i];
		width=(int ) font_buffer[ptr++];
		if(width>32) width=32;
		for(x=0;x<width;x++)
		{
			c=1;
			for(i=0;i<(CHAR_HEIGHT/8);i++)
			{
				m=font_buffer[ptr++];
				b=128;
				for(y=0;y<8;y++)
				{
	    			 if((m&b)!=0) 
					 plotpixel((start_y+y+i*8)*SVGA_logical_length+start_x+x,pen);
					 b=b>>1;
				}
			}
		}
		start_x+=width+2;
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Get string length */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long nice_string_length(char *string)
{
	int i,ptr;
	int width;
	unsigned char c;
	long start_x;

	start_x=0;

	while(*string!=0)
	{
		c=*string++;
		if(c==' ') start_x+=12;
		else
		{
			i=(int ) c;
			ptr=font_table[i];
			width=(int ) font_buffer[ptr++];
			if(width>32) width=32;
			start_x+=width+2;
		}
	}
	return start_x;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Menus */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

typedef	struct
{
	char *message;
	long whattodo;
} menustruct;

enum {
		OPT_RESUME,
		OPT_SCREENSHOT,
		OPT_MENU1,
		OPT_MENU2,
		OPT_SOUNDTEST,
		OPT_PLAYSOUND,
		OPT_MENUVARS,
		OPT_ROPELENGTH,
		OPT_SHIPMASS,
		OPT_BALLMASS,
		OPT_GRAVITY,
		OPT_DEFAULTS,
		OPT_MUSICTOGGLE,
		OPT_SELECTTRACK,
		OPT_TRACKPOS,
		OPT_WEAPON,
		OPT_RESTARTLEVEL,
		OPT_QUIT
		};

char	opttext_restartlevel[]={"RESTART LEVEL"};
char	opttext_resume[]={"RESUME"};
char	opttext_screenshot[]={"SCREEN SHOT"};
char	opttext_othermenu[]={"VARIABLES"};
char	opttext_soundtest[]={"SOUNDTEST"};
char	opttext_playsound[]={"PLAY "};
char	opttext_back[]={"GO BACK"};
char	opttext_ropelength[]={"ROPE LENGTH"};
char	opttext_shipmass[]={"SHIP MASS"};
char	opttext_ballmass[]={"BALL MASS"};
char	opttext_gravity[]={"GRAVITY"};
char	opttext_defaults[]={"RESTORE DEFAULTS"};
char	opttext_cdmenu[]={"CD AUDIO"};
char	opttext_musictoggle[]={"MUSIC "};
char	opttext_selecttrack[]={"TRACK "};
char	opttext_trackpos[]={"TIME "};
char	opttext_weapon[]={"WEAPON "};
char	opttext_quit[]={"QUIT"};
char	edit_string[16+1];

menustruct ingame_options[]={
	opttext_resume			,OPT_RESUME,
	opttext_screenshot	,OPT_SCREENSHOT,
	opttext_cdmenu			,OPT_MENU2,
	opttext_othermenu		,OPT_MENUVARS,
	opttext_soundtest		,OPT_SOUNDTEST,
	opttext_weapon			,OPT_WEAPON,
	opttext_restartlevel	,OPT_RESTARTLEVEL,
	opttext_quit			,OPT_QUIT,
	NULL
};

menustruct sound_menu[]={
	opttext_back			,OPT_MENU1,
	opttext_playsound		,OPT_PLAYSOUND,
	opttext_back			,OPT_MENU1,
	NULL
};

menustruct menu2[]={
	opttext_back			,OPT_MENU1,
	opttext_ropelength	,OPT_ROPELENGTH,
	opttext_shipmass		,OPT_SHIPMASS,
	opttext_ballmass		,OPT_BALLMASS,
	opttext_gravity		,OPT_GRAVITY,
	opttext_defaults		,OPT_DEFAULTS,
	opttext_back			,OPT_MENU1,
	NULL
};

menustruct cd_options[]={
	opttext_back			,OPT_MENU1,
	opttext_musictoggle	,OPT_MUSICTOGGLE,
	opttext_selecttrack	,OPT_SELECTTRACK,
	opttext_trackpos		,OPT_TRACKPOS,
	opttext_back			,OPT_MENU1,
	NULL
};

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Save & restore previous background screen */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void save_current_screen(void)
{
	long y;

	for(y=0;y<screen_height;y++) screen_copy_line(bak_screen+y*640,physical_screen_start+y*SVGA_logical_length,640);
}

void restore_current_screen(void)
{
	long y;

	for(y=0;y<screen_height;y++) screen_dump_line(bak_screen+y*640,physical_screen_start+y*SVGA_logical_length,640);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Draw Menu */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long menux[16];

void draw_menu(menustruct *menu,long sx,long sy)
{
	static menustruct *current_menu=-1;
	static flag cursor=YES;
	char	temp_string[128];
	unsigned char pen;
	long x;
	long i=0;

	cursor^=YES+NO;
	while( menu[i].message!=NULL )
	{
		if(current_option==i) pen=255;
		else pen=255-32;

		switch (menu[i].whattodo)
		{
			case OPT_TRACKPOS:
				if(music_on==YES)	track_pos=get_CDPos();
				sprintf(temp_string,"%s %i",menu[i].message,track_pos);
				break;
			case OPT_SELECTTRACK:
				sprintf(temp_string,"%s %i",menu[i].message,selected_cdtrack);
				break;
			case OPT_PLAYSOUND:
				sprintf(temp_string,"%s %i",menu[i].message,current_fx);
				break;
			case OPT_MUSICTOGGLE:
				sprintf(temp_string,"%s %s",menu[i].message,
					music_on ? "YES":"NO" );
				break;	
			case OPT_ROPELENGTH:
				if( (menu[current_option].whattodo==OPT_ROPELENGTH) && (edit_mode==TRUE) )
				sprintf(temp_string,"%s %s%s",menu[i].message,edit_string, 
							cursor ? ".":" ");
				else
				sprintf(temp_string,"%s %i",menu[i].message,rope_length);
				break;	
			case OPT_SHIPMASS:
				if( (menu[current_option].whattodo==OPT_SHIPMASS) && (edit_mode==TRUE) )
				sprintf(temp_string,"%s %s%s",menu[i].message,edit_string, 
							cursor ? ".":" ");
				else
				sprintf(temp_string,"%s %i",menu[i].message,ship_mass);
				break;	
			case OPT_BALLMASS:
				if( (menu[current_option].whattodo==OPT_BALLMASS) && (edit_mode==TRUE) )
				sprintf(temp_string,"%s %s%s",menu[i].message,edit_string, 
							cursor ? ".":" ");
				else
				sprintf(temp_string,"%s %i",menu[i].message,ball_mass);
				break;	
			case OPT_GRAVITY:
				if( (menu[current_option].whattodo==OPT_GRAVITY) && (edit_mode==TRUE) )
				sprintf(temp_string,"%s %s%s",menu[i].message,edit_string, 
							cursor ? ".":" ");
				else
				sprintf(temp_string,"%s %i",menu[i].message,gravity_factor);
				break;	

			case OPT_WEAPON:
				sprintf(temp_string,"%s %d",menu[i].message,weapontype+1);
				break;
	
			default:
				sprintf(temp_string,"%s",menu[i].message);
				break;	
		}

		if(current_menu!=menu)
		{
			x=nice_string_length(temp_string);
			menux[i]=x;									// Fixed position for menu
		}
		else x=menux[i];
		nice_string(pen,sx+(640-x)/2,sy,temp_string);
		i++;
		sy+=32;
	}
	no_of_options=i;
	current_menu=menu;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Get Highest Pict */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

int get_highest(void )
{
	int highest=0;
	int current=0;
	DIR *dirp;
	char numstring[5];
	struct dirent *direntp;

	dirp=opendir("PICT*.LBM");
	if(dirp==NULL)	printf("No files\n");
	else
	{
		while(1)
		{
			direntp=readdir(dirp);
			if(direntp==NULL) break;
			sprintf(filename,"%s",direntp->d_name);
			strncpy(numstring,filename+4,4);
			current=atoi(numstring);
			if(current>highest) highest=current;
		}
		closedir(dirp);
	}
	return highest;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Misc */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	flag just_edited;
	long cursor_pos;

void edit_number(long *num)
{
			if(just_edited==FALSE)
			{
				flush_keys();
				memset(edit_string,0,16);
				sprintf(edit_string,"%i",*num);
				cursor_pos=strlen(edit_string);
				edit_mode=TRUE;
				just_edited=TRUE;
			}
			else
			{
				*num=atoi(edit_string);
			 	just_edited=FALSE;
			}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Process Menu */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void process_menu(void )
{
	flag	leave_menu;
	char	key;
	menustruct *current_menu;
	
	just_edited=FALSE;
	cursor_pos=0;
	input_string[0]=0;

	edit_mode=FALSE;
	leave_menu=FALSE;
	current_menu=ingame_options;
	edit_string[0]=0;

	scan_for_keysjp();
	save_current_screen();

	set_pen(0,0,0,0);

	/* Select option */

	current_option=0;

	while(leave_menu==FALSE)
	{

	while(TRUE)
	{
		draw_menu(current_menu,physical_screen_x,physical_screen_y+128);	

		scan_for_keysjp();

		if(edit_mode==TRUE)
		{
			key=get_key();
			if(key!=0)
			{
				restore_current_screen();
				if(key==8)
				{
					if(cursor_pos>0) cursor_pos--;
				}
				else if(cursor_pos<8) edit_string[cursor_pos++]=key;

				edit_string[cursor_pos]=0;
			}

		}
		else
		{
			if(keys_jp[kUARROW]==128)
			{
				if(current_option<=0) current_option=no_of_options-1;
				else current_option--;
			}
			if(keys_jp[kDARROW]==128)
			{
				current_option++;
				if(current_option>=no_of_options) current_option=0;
			}
		}

		if(keys_jp[kENTER]==128) break;
		if(keys_jp[kLARROW]==128) break;
		if(keys_jp[kRARROW]==128) break;
	}

	edit_mode=FALSE;

	/* Process selected option */

	switch(current_menu[current_option].whattodo)
	{
		case OPT_RESUME:
			leave_menu=TRUE;
			break;
		case OPT_SCREENSHOT:
			sprintf(filename,"PICT%04d.LBM",get_highest()+1);
			save_iff(filename,bak_screen,iff_palette,640,480);
			leave_menu=TRUE;
			break;
		case OPT_MENU1:
			current_option=0;
			restore_current_screen();
			current_menu=ingame_options;
			break;
		case OPT_MENU2:
			current_option=0;
			restore_current_screen();
			current_menu=cd_options;
			break;
		case OPT_MENUVARS:
			current_option=0;
			restore_current_screen();
			current_menu=menu2;
			break;
		case OPT_SOUNDTEST:
			current_option=1;
			restore_current_screen();
			current_menu=sound_menu;
			break;
		case OPT_ROPELENGTH:
			restore_current_screen();
			edit_number(&rope_length);
			break;
 		case OPT_SHIPMASS:
			restore_current_screen();
			edit_number(&ship_mass);
			break;
 		case OPT_BALLMASS:
			restore_current_screen();
			edit_number(&ball_mass);
			break;
 		case OPT_GRAVITY:
			restore_current_screen();
			edit_number(&gravity_factor);
			break;
		case OPT_MUSICTOGGLE:
			restore_current_screen();
			music_on ^=YES+NO;
			if(music_on==YES)
			{
				last_cdtrack=init_CD();
				if(selected_cdtrack>last_cdtrack) selected_cdtrack=1;
			}
			else Stop_CDTrack();
			break;
		case OPT_TRACKPOS:
			restore_current_screen();
			if(music_on==YES)	track_pos=get_CDPos();
			break;
		case OPT_SELECTTRACK:
			if(keys_jp[kLARROW]==128)
			{
				restore_current_screen();
				if(selected_cdtrack>0) selected_cdtrack--;
			}
			if(keys_jp[kRARROW]==128)
			{
				restore_current_screen();
				if(selected_cdtrack<last_cdtrack) selected_cdtrack++;
			}
			if(music_on==YES)
			{	
				Stop_CDTrack();
				Play_CDTrack(selected_cdtrack);
			}
			break;
		case OPT_PLAYSOUND:
			if(keys_jp[kLARROW]==128)
			{
				restore_current_screen();
				if(current_fx>0) current_fx--;
			}
			if(keys_jp[kRARROW]==128)
			{
				restore_current_screen();
				if(current_fx<(FX_END-1)) current_fx++;
			}
			if(current_fx<0) current_fx=0;
			if(current_fx>=FX_END) current_fx=FX_END-1;
			play_sample(0,samples_start[current_fx],samples_len[current_fx]);
			break;

		case OPT_WEAPON:
			if(keys_jp[kLARROW]==128)
			{
				restore_current_screen();
				if(weapontype>0) weapontype--;
			}
			if(keys_jp[kRARROW]==128)
			{
				restore_current_screen();
				if(weapontype<(WEAPON_END-1)) weapontype++;
			}
			if(weapontype<0) weapontype=0;
			if(weapontype>=WEAPON_END) weapontype=WEAPON_END-1;
			break;
		case OPT_RESTARTLEVEL:
			clear_sprites();
			init_fire();
			load_level_map_text(get_level_name(level_no));
			place_craft();
			validate_sprite_list();
			break;

		case OPT_QUIT:
			leave_menu=TRUE;
			escape=TRUE;
			break;
	}

	}
	
	scan_for_keysjp();
	restore_current_screen();
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of file */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
