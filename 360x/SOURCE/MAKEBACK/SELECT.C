/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Selection */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// To speed up 24-bit version?
// Faster expand to 24-bit function or do it to screen
// Could pre-clip lines b4 main dump
// Asteroids need compressing (do their frames repeat?)
// Real Time Conversion? or pre-stored conversion...
// Pre-stored seems mildly easier
// Lets get the asteroids to flash when you are over them
// No bank changes across lines in 24-bit mode...
// Would like a 16-bit/24-bit version...
// Furthur optimisations:- Clipped removal
// Need a function to find max physical size of each sprite

#include "globals.h"

	char *bak_buf;

long  level_selected;
long	spiral_x;
long	spiral_y;

void update_mouse(long mouse_id );
void move_to_next_asteroid(void);
void draw_asteroids(void );
void remove_sprite_lines(sprite_sequence seq,int sprite_no,int x,int y);
void remove_sprite_lines_24bit(sprite_sequence seq,int sprite_no,int x,int y);
void draw_sprite_lines_24bit(sprite_sequence seq,int sprite_no,int x,int y);
void add_24bit_line(unsigned char *source,long x,long y,long length);
void draw_sprite_list(void );
void init_sprite_list(void);
long get_sprite_id(long name,long frame,long x,long y);
void id_set_sprite(long i,long frame,long x,long y);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Structure for Asteroids */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

enum	{
			ANIM_NULL,
			ANIM_ALEUIS,
			ANIM_SELIUS,
			ANIM_CUNAE,
			ANIM_NIXA,
			ANIM_BOZAEI,
			ANIM_PROLI,
			ANIM_BUTS,
			ANIM_CURSOR,
			ANIM_END
		};
			

fnames_list fnames_anims[]= {
	"NULL",
	"ALEUIS",
	"SELIUS",
	"CUNAE",
	"NIXA",
	"BOZAEI",
	"PROLI",
	"BUTS",
	"CURSOR1"
};
  
typedef struct
{
	long id;
	long name;
	long frame;
	long frame_delay;
	long x;
	long y;
} animstruct;

animstruct anims[]={
	-1,ANIM_ALEUIS,0,0,80, 88,
	-1,ANIM_SELIUS,0,5,546,324,
	-1,ANIM_CUNAE, 0,2,382,212,
	-1,ANIM_NIXA,  0,3,296,72,
	-1,ANIM_BOZAEI,0,1,204,238,
	-1,ANIM_PROLI, 0,4,546,72,
	-1,ANIM_END,   0,0,0,0
};	

void promote_to_24bit(unsigned char *source)
{
	long i;
	unsigned long r,g,b;

	for(i=0;i<256;i++)
	{
		r=source[i*3+0]<<2;
		g=source[i*3+1]<<2;
		b=source[i*3+2]<<2;

		pal_32bit[i*4+0]=b;
		pal_32bit[i*4+1]=g;
		pal_32bit[i*4+2]=r;
		pal_32bit[i*4+3]=0;
	}

}

void load_anims(void)
{
	FILE *infile;
	long i;

	for(i=0;i<NAME_END;i++)
	{
		seqdata[i].start_address=NULL;
		seqdata[i].frames=-1;
	}

	for(i=0;i<ANIM_END;i++) anims[i].id=-1;

	for(i=1;i<ANIM_END;i++)
	{
		sprintf(filename,"GFX/SELECT/%s.SPL",fnames_anims[i].fname);
		if( (infile=fopen(filename,"rb")) != NULL )
		{
			fclose(infile);
			load_sprite(i,filename);
			asm_clear_line_list();
			draw_asteroids();
			draw_sprite_list();
			asm_dump_line_list();
		}
		else printf("File does not exist %s\n",filename);
	}
}

void release_sprites(void )
{
	long i;
	char *address;

//	for(i=0;i<NAME_END;i++)
	for(i=0;i<ANIM_END;i++)
	{
		address=seqdata[i].start_address;
		if(address!=NULL)
		{ 
			free(address);
			seqdata[i].start_address=NULL;
			seqdata[i].frames=-1;
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Load Space Ship */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void load_big_sprite(char *fname)
{
	long name;
	long i;
	int *sourcel;

	name=ANIM_END;

	seqdata[name].frames=-1;

	sprintf(filename,"%s.map",fname);
	seqdata[name].map=(int *) heap_load_file(filename);
	sprintf(filename,"%s.bin",fname);
	seqdata[name].data=heap_load_file(filename);
	if(seqdata[name].data==NULL) printf("File error on big sprite\n");

	sourcel=seqdata[name].map;
	i=0;
	while(sourcel[i]!=-1) i++;
	seqdata[name].frames=i;
}

void remove_big_sprite(long frame)
{
	char *dest;
	long name;
	long i,x,y,iy;
	long lx,dx;
	long width,height;
	char *source;
	short *sourcew;
	
	name=ANIM_END;
	x=640/2;
	y=480/2;

	if(frame>=seqdata[name].frames)
	{
		frame=frame%(seqdata[name].frames);
	}

	i=seqdata[name].map[frame];
	source=seqdata[name].data;
	sourcew=(short *) (source+i);
	
	x+=(long ) sourcew[0];
	y+=(long ) sourcew[1];
	width=(long ) sourcew[2];
	height=(long ) sourcew[3];
	i+=8;

	for(iy=0;iy<height;iy++)
	{
		sourcew=(short *) (source+i);
		dx=sourcew[0];
		lx=sourcew[1];
		i+=4;
		dest=bak_buf+x+dx+(iy+y)*640;
	 	asm_add_linked_line(dest,x+dx,y+iy,lx);
		i+=lx;
	}	
}

void draw_big_sprite(long frame)
{
	long name;
	long i,x,y,iy;
	long lx,dx;
	long width,height;
	char *source;
	short *sourcew;
	
	name=ANIM_END;
	x=640/2;
	y=480/2;

	if(frame>=seqdata[name].frames)
	{
		frame=frame%(seqdata[name].frames);
	}

	i=seqdata[name].map[frame];
	source=seqdata[name].data;
	sourcew=(short *) (source+i);
	
	x+=(long ) sourcew[0];
	y+=(long ) sourcew[1];
	width=(long ) sourcew[2];
	height=(long ) sourcew[3];
	i+=8;

	for(iy=0;iy<height;iy++)
	{
		sourcew=(short *) (source+i);
		dx=sourcew[0];
		lx=sourcew[1];
		i+=4;
		asm_add_linked_line(source+i,x+dx,y+iy,lx);
		i+=lx;
	}	
}


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Mouse Library - by Phillip Trelford */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void mouse_read_motion_counters(void )
{
	union REGS regs;
	long mickeys_x,mickeys_y;

	regs.w.ax = 0x0b;						// RETURN BUTTON PRESS DATA
 	int386( 0x33, &regs, &regs );

	mickeys_x=(long ) regs.w.cx;
	mickeys_y=(long )regs.w.dx;
	
	mouse_x+=mickeys_x;
	mouse_y+=mickeys_y;

	if(mouse_x<0) mouse_x=0;
	if(mouse_y<0) mouse_y=0;
	if(mouse_x>(640-16)) mouse_x=640-16;
	if(mouse_y>(480-16)) mouse_y=480-16;

}

/*
static struct rminfo {
	long	EDI;
	long	ESI;
	long	EBP;
	long	reserved_by_system;
	long	EBX;
	long	EDX;
	long	ECX;
	long	EAX;
	short	flags;
	uword	ES,DS,FS,GS,IP,CS,SP,SS;
} RMI;
*/

void	reset_mouse_driver( void)
{
	union REGS regs;
//	struct SREGS sregs;

	mouse_movements=0;
	last_mouse_x=0;
	last_mouse_y=0;
	mouse_x=0;
	mouse_y=0;
	mouse_dx=0;
	mouse_dy=0;
	clicked_buttons=0;
	clicked_left_button=NO;
	clicked_right_button=NO;
	clicked_middle_button=NO;
	clicked_mouse=0;
	clicked_mouse_x=0;
	clicked_mouse_y=0;
	mouse_button_status=0;
	mouse_left_button=NO;
	mouse_right_button=NO;
	mouse_middle_button=NO;

//	mouse_connected=NO;
//	return;

	/*
	printf("Reset Mouse\n");
	memset( &RMI, 0, sizeof( RMI ));
	RMI.EAX=0;
	
	memset( &sregs, 0, sizeof( sregs ));
	regs.w.ax = 0x0300;					// DPMI Issue DOS Int
	regs.h.bl = 0x33;
	regs.h.bh = 0;
	regs.w.cx = 0;
	int386x( DPMI_INT, &regs , &regs, &sregs );
	if( (RMI.EAX&0xffff) == 0 )
	*/

	regs.w.ax = 0x00;						//	RESET DRIVER AND READ STATUS
	int386( 0x33, &regs, &regs );
	if(regs.w.ax==0) 
	{
		mouse_connected=NO;
		return;
	}

	regs.w.ax = 0x07;						//	DEFINE HORIZONTAL CURSOR RANGE
	regs.w.cx = 0;
	regs.w.dx = 640;
 	int386( 0x33, &regs, &regs );
	
	regs.w.ax = 0x08;						//	DEFINE VERTICAL CURSOR RANGE
	regs.w.cx = 0;
	regs.w.dx = 480;
 	int386( 0x33, &regs, &regs );
	
	mouse_connected=YES;
}

void return_position_and_button_status(void)
{
	long i;
	union REGS regsin,regsout;
	
	last_mouse_x=mouse_x;
	last_mouse_y=mouse_y;

	if(mouse_connected==YES)
	{
		regsin.w.ax = 0x03;						// RETURN POSITION AND BUTTON STATUS
 		int386( 0x33, &regsin, &regsout );

		mouse_x=regsout.w.cx;
		mouse_y=regsout.w.dx;
		mouse_button_status=regsout.w.bx;
		if(mouse_button_status&1) mouse_left_button=YES;
		else mouse_left_button=NO;

		if(mouse_button_status&2) mouse_right_button=YES;
		else mouse_right_button=NO;

		if(mouse_button_status&4) mouse_middle_button=YES;
		else mouse_middle_button=NO;
	}
	/*
	else
	{
	*/
		if( mouse_dx>0 ) mouse_dx--;
		if( mouse_dx<0 ) mouse_dx++;
		if( mouse_dy>0 ) mouse_dy--;
		if( mouse_dy<0 ) mouse_dy++;

		if( keys[kLARROW]==128 ) mouse_dx-=2;
		if( keys[kRARROW]==128 ) mouse_dx+=2;
		if( keys[kUARROW]==128 ) mouse_dy-=2;
		if( keys[kDARROW]==128 ) mouse_dy+=2;
	
		if( mouse_dx<-8 ) mouse_dx=-8;
		if( mouse_dx>8  ) mouse_dx=8;
		if( mouse_dy<-8 ) mouse_dy=-8;
		if( mouse_dy>8  ) mouse_dy=8;

		mouse_x+=mouse_dx;
		mouse_y+=mouse_dy;

		if( mouse_x<0 ) mouse_x=0;
		if( mouse_x>=640 ) mouse_x=640-1;
		if( mouse_y<0 ) mouse_y=0;
		if( mouse_y>=480 ) mouse_y=480-1;

		
	/*
	}
	*/
	i=1;
	if(last_mouse_x!=mouse_x) i=0;
	if(last_mouse_y!=mouse_y) i=0;
	if(i==0)
	{ 
		mouse_movements=0;
		position_mouse_pointer(mouse_x,mouse_y);
	}
	else
	{
		mouse_movements++;
	}

}

// Something wrong here with left button

void return_button_press_data(void)
{
	union REGS regsin,regsout;
	
	clicked_left_button=NO;
	clicked_right_button=NO;

	if(mouse_connected==YES)
	{
 		regsin.w.ax = 0x05;						// RETURN BUTTON PRESS DATA
	 	regsin.w.bx = 0x00;
 		int386( 0x33, &regsin, &regsout );

		clicked_buttons=regsout.w.ax;
		clicked_mouse=regsout.w.bx;
		clicked_mouse_x=regsout.w.cx;
		clicked_mouse_y=regsout.w.dx;
		if(clicked_buttons&1) clicked_left_button=YES;

		if(clicked_left_button==NO)
		{
		 	regsin.w.ax = 0x05;						// RETURN BUTTON PRESS DATA
 			regsin.w.bx = 0x01;
		 	int386( 0x33, &regsin, &regsout );
	
			clicked_buttons=regsout.w.ax;
			if(clicked_buttons&2) clicked_right_button=YES;
			clicked_mouse=regsout.w.bx;
			clicked_mouse_x=regsout.w.cx;
			clicked_mouse_y=regsout.w.dx;
		}

	}
	/*
	else
	{
	*/
		if(keys_jp[kENTER]==128) clicked_mouse=1;
		clicked_left_button=YES;
		clicked_mouse_x=mouse_x;		
		clicked_mouse_y=mouse_y;
	/*
	}
	*/
}

void position_mouse_pointer(int x,int y)
{
	union REGS regsin,regsout;
	
	if(mouse_connected==YES)
	{
	regsin.w.ax = 0x04;						//	POSITION MOUSE CURSOR
	regsin.w.cx = x;
	regsin.w.dx = y;
 	int386( 0x33, &regsin, &regsout );
	}

	mouse_x=x;
	mouse_y=y;

}

flag	TrueColour;

unsigned char *massive_buf;
//unsigned char massive_buf[640*480*3];
long mptr;

long test_the_ship(void)
{
	double angle;
	long frames;
	long frame;

	frames=seqdata[ANIM_END].frames-1;

	angle=atan2(mouse_x-320,240-mouse_y);
	
	while(angle<0) angle+=PI*2;

	frame=(long ) ((angle*frames)/(PI*2));

	return frame;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Main Procedure for Selection */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void reverse_bytes(char *source)
{
	long i;
	char r,g,b;

	for(i=0;i<(640*480);i++)
 	{
		r=source[0];
		g=source[1];
		b=source[2];

		source[0]=b;
		source[1]=g;
		source[2]=r;
		source+=3;
 	}
}
	
 
	char *select_pal;

long select(void)
{
	long	last_1,this_1;
	long	i;
	long	mouse_id;
	long	button1_id;
	long	 button2_id;
	char *tga_buf;
	flag	toggle_mickey;

	toggle_mickey=NO;

	clear_heap();

	init_particle_list();
	init_line_list();
	init_sprite_list();


	TrueColour=NO;
//	if(SVGA_is_mode_available(0x112)==YES) TrueColour=YES;
	
	select_pal=heap_load_file("GFX/SELECT/ALEUIS.PAL");
	select_pal[0]=0;
	select_pal[1]=0;
	select_pal[2]=0;
	
	if(TrueColour==YES)
	{
		SVGA_set_mode(0x112);
		massive_buf=heap_malloc(640*480*3);
		asm_set_logical_length(SVGA_logical_length);
		tga_buf=heap_load_file("GFX/SELECT/MENUBAK.TGA");
//		tga_buf=heap_load_file("GFX/SELECT/BS.TGA");
		read_tga_header(tga_buf);
		bak_buf=heap_malloc(return_tga_width()*return_tga_height()*3);
		decode_tga(tga_buf,bak_buf);
		reverse_bytes(bak_buf);
		for(i=0;i<480;i++)
		{
			screen_dump_line(bak_buf+(i*640*3),i*SVGA_logical_length,640*3);
		}
		promote_to_24bit(select_pal);
	}
	else
	{
		SVGA_set_mode(0x101);
		if(safe_mode==0) set_split(0);
		asm_set_logical_length(640);
		SVGA_set_display_start(0,0,YES);
		bak_buf=readlbm("GFX/SELECT/MENU.LBM");
		clear_dump_line_list();
		screen_dump(0,0,bak_buf,640,480);
		use_dump_line_list();
 			
		set_palette(select_pal,0,256);
	}

	position_mouse_pointer(320,200);

	mouse_id=get_sprite_id(ANIM_CURSOR,0,mouse_x,mouse_y);
	button1_id=get_sprite_id(ANIM_BUTS,0,48,360);
	button2_id=get_sprite_id(ANIM_BUTS,0,96,420);

	load_anims();

//	load_big_sprite("GFX/FRONT/OVSHIP2");

	level_selected=0;

	last_1=mouse_x;
	while(TRUE)
	{
		scan_for_keysjp();

		if(keys_jp[kT]==128)
		{
			if(toggle_mickey==YES) toggle_mickey=NO;
			else toggle_mickey=YES;
		}
	
		if(keys_jp[kESC]==128)
		{
			level_selected=-1;
			break;
		}

		if(keys_jp[kTAB]==128)
		{
			move_to_next_asteroid();
			mouse_movements=0;
		}
		i=-1;
		if(keys_jp[k1]==128) i=0;
		if(keys_jp[k2]==128) i=1;
		if(keys_jp[k3]==128) i=2;
		if(keys_jp[k4]==128) i=3;
		if(keys_jp[k5]==128) i=4;
		if(keys_jp[k6]==128) i=5;
		if(i!=-1)
		{
			mouse_movements=0;
			position_mouse_pointer(anims[i].x,anims[i].y);
		}

		asm_clear_line_list();

		update_mouse(mouse_id);

		draw_asteroids();

		draw_sprite_list();

	  	this_1=test_the_ship();
//		draw_big_sprite(this_1);

//		remove_big_sprite(last_1);
		last_1=this_1;

		remove_sprite_list();

		if(TrueColour==NO) wait_vbi();
		colour_bar(0,63,63,63);
		asm_dump_line_list();
		colour_bar(0,0,0,0);
		
		if(toggle_mickey==NO)
		{
	 		return_position_and_button_status();
		}
		else
		{
			mouse_read_motion_counters();
		}
		return_button_press_data();

		if(clicked_mouse!=0) break;
	}

//	free(bak_buf);

	//release_sprites();

	if(TrueColour==YES)
	{
		SVGA_set_mode(0x101);				// bodge to 1024x768
	  	asm_set_logical_length(SVGA_logical_length);
	}

//	if(mouse_connected==NO)
//	{
//		clearkey();
//	}

	clear_heap();

	return level_selected;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Update the mouse position */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Spiralling cursor does seem like a novel idea...
 
void update_mouse(long mouse_id )
{
/*
	double angle;
	long x,y;

	if(mouse_movements>64)
	{
		if((mouse_movements&1)==0)
		{
			angle=(mouse_movements-64)&31;
			if(angle>0)	angle=(angle*PI*2)/32;
			else angle=0;
			x=(long ) (cos(angle)*(mouse_movements&31));
			y=(long ) (sin(angle)*(mouse_movements&31));
			//x=((rand()*16)>>15)-4;
			//y=((rand()*16)>>15)-4;
 			id_set_sprite(mouse_id,0,mouse_x+x,mouse_y+y);
		}
	}
	else
	{
	 	if(mouse_movements>32)
 		{
 			if(mouse_flash<=0)
	 		{
 				id_set_sprite(mouse_id,0,-99,-99);
 				mouse_flash=1;
	 		}
 			else
 			{
	 			mouse_flash--;
 				id_set_sprite(mouse_id,0,mouse_x,mouse_y);
 			}
	 	}
 		else id_set_sprite(mouse_id,0,mouse_x,mouse_y);
	}
	*/
 	id_set_sprite(mouse_id,0,mouse_x,mouse_y);
}


void move_to_next_asteroid(void)
{
	long i;
	level_selected++;
	i=-1;
	while(i!=level_selected)
	{
		i++;
		if(anims[i].name==ANIM_END)
		{
			level_selected=0;
			i=0;
		}
	}
	position_mouse_pointer(anims[i].x,anims[i].y);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Select Screen Sprite Handler */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
//	Different from game sprites
//	Line List Handler in CODE.ASM
 
#define SPRITES_MAX 16

typedef struct
{
	long name;
	long frame;
	long x;
	long y;
	long old_frame;
	long old_x;
	long old_y;
} SpriteList;

SpriteList sprite_list[SPRITES_MAX];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Initialise Sprite List */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void init_sprite_list(void)
{
	long	i;
	for(i=0;i<SPRITES_MAX;i++)
	{
		sprite_list[i].name=-1;
		sprite_list[i].frame=-1;
		sprite_list[i].x=-1;
		sprite_list[i].y=-1;
		sprite_list[i].old_frame=-1;
		sprite_list[i].old_x=-1;
		sprite_list[i].old_y=-1;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Add a sprite to sprite list and return its ID */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long get_sprite_id(long name,long frame,long x,long y)
{
	long i;
	i=0;
	while(i<SPRITES_MAX)
	{
		if(sprite_list[i].name==-1)
		{
			//printf("%i %i %i %i\n",name,frame,x,y);

			sprite_list[i].name=name;
			sprite_list[i].frame=frame;
			sprite_list[i].x=x;
			sprite_list[i].y=y;
			sprite_list[i].old_frame=frame;
			sprite_list[i].old_x=x;
			sprite_list[i].old_y=y;
			return i;
		}
		i++;
	}
	return -1;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Draw sprite list */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void draw_sprite_list(void )
{
	long i,name;

	mptr=0;
	for(i=0;i<SPRITES_MAX;i++)
	{
		name=sprite_list[i].name;
		if(name!=-1)
		{
			if(seqdata[name].frames>0)
			{
				if(TrueColour==YES)
				{
				draw_sprite_lines_24bit(seqdata[name],
										sprite_list[i].frame,
										sprite_list[i].x,
										sprite_list[i].y);
				}
				else
				{
				draw_sprite_lines(seqdata[name],
										sprite_list[i].frame,
										sprite_list[i].x,
										sprite_list[i].y);
				}
			}
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Remove sprite list */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void remove_sprite_list(void )
{
	long i;
	flag remove;

	for(i=0;i<SPRITES_MAX;i++)
	{
		if(sprite_list[i].name!=-1)
		{
			remove=FALSE;
			if(sprite_list[i].frame!=sprite_list[i].old_frame) remove=TRUE;
			if(sprite_list[i].x!=sprite_list[i].old_x) remove=TRUE;
			if(sprite_list[i].y!=sprite_list[i].old_y) remove=TRUE;

			if(remove==TRUE)
			{
			if(TrueColour==YES)
			{
			remove_sprite_lines_24bit(seqdata[(sprite_list[i].name)],
										sprite_list[i].old_frame,
										sprite_list[i].old_x,
										sprite_list[i].old_y);
			}
			else
			{
			remove_sprite_lines(seqdata[(sprite_list[i].name)],
										sprite_list[i].old_frame,
										sprite_list[i].old_x,
										sprite_list[i].old_y);
			}
			}
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Set IDed sprites frame,x & y */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void id_set_sprite(long i,long frame,long x,long y)
{
	sprite_list[i].old_x=sprite_list[i].x;
	sprite_list[i].old_y=sprite_list[i].y;
	sprite_list[i].old_frame=sprite_list[i].frame;

	sprite_list[i].frame=frame;
	sprite_list[i].x=x;
	sprite_list[i].y=y;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Mouse inside sprite */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag mouse_inside_sprite(sprite_sequence seq,long frame,long x,long y)
{
	unsigned char *source;
	short *sourcew;
	long i,width,height;
	long iy,dx,lx,ax;

	i=seq.map[frame];
	source=seq.data;
	sourcew=(short *) (source+i);

	x+=(long ) sourcew[0];
	y+=(long ) sourcew[1];
	width=(long ) sourcew[2];
	height=(long ) sourcew[3];

	i+=8;

	if(x>mouse_x) return FALSE;
	if((x+width)<mouse_x) return FALSE;

	if(y>mouse_y) return FALSE;
	if((y+height)<mouse_y) return FALSE;

	iy=0;
	while( (y+iy)<mouse_y )
	{
		if(iy==height) return FALSE;
		dx=(unsigned int) source[i++];
		lx=(unsigned int) source[i++];
		i++;
		ax=(unsigned int) source[i++];
		i+=ax;
		iy++;
	}
	x+=dx;
	if( (x<mouse_x) && ((x+lx)>mouse_x) ) return TRUE;

	return FALSE;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Animate the Asteroids */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */


void draw_asteroids(void )
{
	long name,frame,x,y,i;

	i=0;
	while(TRUE)
	{
		name=anims[i].name;
		if(name==ANIM_END) break;

		if(seqdata[name].frames>0)
		{
			frame=anims[i].frame;
			x=anims[i].x;
			y=anims[i].y;

			if(anims[i].id==-1) anims[i].id=get_sprite_id(name,frame,x,y);
		
			if(anims[i].frame_delay==0)
			{
				frame++;
				if(frame>=(seqdata[name].frames-2)) frame=0;
				anims[i].frame=frame;
				anims[i].frame_delay=2;
			}
			else anims[i].frame_delay--;

			if(mouse_inside_sprite(seqdata[name],frame,x,y)==TRUE)
			{
				frame=0;
				level_selected=i;
			}
			id_set_sprite(anims[i].id,frame,x,y);
			
		}
		i++;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* FLIC style sprite draw routine */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void draw_sprite_lines(sprite_sequence seq,int sprite_no,int x,int y)
{
	unsigned char *source;
	short *sourcew;
	int i;
	int ix,iy;
	int dx,lx,tx,dlx;
	int width,height;

	if(sprite_no>=seq.frames) sprite_no=0;

	i=seq.map[sprite_no];
	source=seq.data;
	sourcew=(short *) (source+i);

	x+=(int ) sourcew[0];
	y+=(int ) sourcew[1];
	width=(int ) sourcew[2];
	height=(int ) sourcew[3];

	i+=8;

	if( x>=clip_end_x ) return;
	if( y>=480 ) return;
	if( (y+height)<clip_start_y ) return;
	if( (x+width) <clip_start_x ) return;
		 
	if( (y+height)>=480 ) height=480-y;	// bottom clip

	iy=0;														// top clip
	while( (y+iy)<0 )
	{
		if(iy==height) break;
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		i+=lx+2;
		iy++;
	}

	if( (x>=clip_start_x) && ((x+width)<clip_end_x) ) 
	{
		// x not clipped
		for(;iy<height;iy++)
		{
			dx=(unsigned int) source[i++];				// x distance to first pixel
			lx=(unsigned int) source[i++];				// length of line
			tx=(unsigned int) source[i++];
			i++;
			asm_add_linked_line(source+i,x+dx,y+iy,lx);
			i+=lx;
		}
	}
	else
	{
		// x clipped
		for(;iy<height;iy++)
		{
			dx=(unsigned int) source[i++];				// x distance to first pixel
			lx=(unsigned int) source[i++];				// length of line
			tx=(unsigned int) source[i++];
			i++;
			dx+=x;
		
			if( dx>=clip_start_x) ix=0;					// left clip?
			else ix=clip_start_x-dx;

			if( (dx+lx)<clip_end_x) dlx=lx;				// right clip?
			else dlx=clip_end_x-dx;

			if(ix<dlx) asm_add_linked_line(source+i+ix,ix+dx,y+iy,(dlx-ix));
			i+=lx;
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* FLIC style sprite draw routine */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void draw_sprite_lines_24bit(sprite_sequence seq,int sprite_no,int x,int y)
{
	unsigned char *source;
	short *sourcew;
	int i;
	int ix,iy;
	int dx,lx,tx,dlx;
	int width,height;

	if(sprite_no>=seq.frames) sprite_no=0;

	i=seq.map[sprite_no];
	source=seq.data;
	sourcew=(short *) (source+i);

	x+=(int ) sourcew[0];
	y+=(int ) sourcew[1];
	width=(int ) sourcew[2];
	height=(int ) sourcew[3];

	i+=8;

	if( x>=clip_end_x ) return;
	if( y>=480 ) return;
	if( (y+height)<clip_start_y ) return;
	if( (x+width) <clip_start_x ) return;
		 
	if( (y+height)>=480 ) height=480-y;	// bottom clip

	iy=0;														// top clip
	while( (y+iy)<0 )
	{
		if(iy==height) break;
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		i+=lx+2;
		iy++;
	}

	if( (x>=clip_start_x) && ((x+width)<clip_end_x) ) 
	{
		// x not clipped
		for(;iy<height;iy++)
		{
			dx=(unsigned int) source[i++];				// x distance to first pixel
			lx=(unsigned int) source[i++];				// length of line
			tx=(unsigned int) source[i++];
			i++;
//			asm_add_linked_line(source+i,x+dx,y+iy,lx);
			add_24bit_line(source+i,x+dx,y+iy,lx);
			i+=lx;
		}
	}
	else
	{
		// x clipped
		for(;iy<height;iy++)
		{
			dx=(unsigned int) source[i++];				// x distance to first pixel
			lx=(unsigned int) source[i++];				// length of line
			tx=(unsigned int) source[i++];
			i++;
			dx+=x;
		
			if( dx>=clip_start_x) ix=0;					// left clip?
			else ix=clip_start_x-dx;

			if( (dx+lx)<clip_end_x) dlx=lx;				// right clip?
			else dlx=clip_end_x-dx;

			if(ix<dlx)
			{
//			 	asm_add_linked_line(source+i+ix,ix+dx,y+iy,(dlx-ix));
			 	add_24bit_line(source+i+ix,ix+dx,y+iy,(dlx-ix));
			}
			i+=lx;
		}
	}
}

void add_24bit_line(unsigned char *source,long x,long y,long length)
{
	asm_add_linked_line(&massive_buf[mptr],x*3,y,length*3);

	asm_expand_to_24bit(source,massive_buf+mptr,pal_32bit,length);
	mptr+=length*3;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Remove sprite routine using line list */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void remove_sprite_lines(sprite_sequence seq,int sprite_no,int x,int y)
{
	unsigned char *source,*dest;
	short *sourcew;
	int i=0;
	int ix,iy;
	int dx,lx,ax,dlx;
	int width,height;

	i=seq.map[sprite_no];
	source=seq.data;
	sourcew=(short *) (source+i);

	x+=(int ) sourcew[0];
	y+=(int ) sourcew[1];
	width=(int ) sourcew[2];
	height=(int ) sourcew[3];
	i+=8;

	if( x>=clip_end_x ) return;
	if( y>=480 ) return;
	if( (y+height)<clip_start_y ) return;
	if( (x+width )<clip_start_x ) return;

	if( (y+height)>=480 ) height=480-y;				// bottom clip

	iy=0;														// top clip
	while( (y+iy)<clip_start_y )
	{
		if(iy==height) break;
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		i++;
		ax=(unsigned int) source[i++];				// length of line
		i+=ax;
		iy++;
	}

	if( (x>=clip_start_x) && ((x+width)<clip_end_x) ) 
	{
	 	// not clipped
		for(;iy<height;iy++)
		{
			dx=(unsigned int) source[i++];				// x distance to first pixel
			lx=(unsigned int) source[i++];				// length of line
			i++;
			ax=(unsigned int) source[i++];				// length of line

			dx+=x;

			dest=bak_buf+dx+(iy+y)*640;
			asm_add_linked_line(dest,dx,y+iy,lx);
			i+=ax;
		}
	}
	else
	{
	for(;iy<height;iy++)
	{
		// clipped
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		i++;
		ax=(unsigned int) source[i++];				// length of line

		dx+=x;

		if( dx>=clip_start_x) ix=0;					// left clip?
		else ix=clip_start_x-dx;

		if( (dx+lx)<clip_end_x) dlx=lx;				// right clip?
		else dlx=clip_end_x-dx;

		dest=bak_buf+dx+(iy+y)*640;
		
		if(ix<dlx) asm_add_linked_line(dest,dx,y+iy,lx);
		i+=ax;
	}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Remove sprite routine using line list */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void remove_sprite_lines_24bit(sprite_sequence seq,int sprite_no,int x,int y)
{
	unsigned char *source,*dest;
	short *sourcew;
	int i=0;
	int ix,iy;
	int dx,lx,ax,dlx;
	int width,height;

	i=seq.map[sprite_no];
	source=seq.data;
	sourcew=(short *) (source+i);

	x+=(int ) sourcew[0];
	y+=(int ) sourcew[1];
	width=(int ) sourcew[2];
	height=(int ) sourcew[3];
	i+=8;

	if( x>=clip_end_x ) return;
	if( y>=480 ) return;
	if( (y+height)<clip_start_y ) return;
	if( (x+width )<clip_start_x ) return;

	if( (y+height)>=480 ) height=480-y;				// bottom clip

	iy=0;														// top clip
	while( (y+iy)<clip_start_y )
	{
		if(iy==height) break;
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		i++;
		ax=(unsigned int) source[i++];				// length of line
		i+=ax;
		iy++;
	}

	if( (x>=clip_start_x) && ((x+width)<clip_end_x) ) 
	{
	 	// not clipped
		for(;iy<height;iy++)
		{
			dx=(unsigned int) source[i++];				// x distance to first pixel
			lx=(unsigned int) source[i++];				// length of line
			i++;
			ax=(unsigned int) source[i++];				// length of line

			dx+=x;

			dest=bak_buf+dx*3+(iy+y)*640*3;
			asm_add_linked_line_clip(dest,dx*3,y+iy,lx*3);
			i+=ax;
		}
	}
	else
	{
	for(;iy<height;iy++)
	{
		// clipped
		dx=(unsigned int) source[i++];				// x distance to first pixel
		lx=(unsigned int) source[i++];				// length of line
		i++;
		ax=(unsigned int) source[i++];				// length of line

		dx+=x;

		if( dx>=clip_start_x) ix=0;					// left clip?
		else ix=clip_start_x-dx;

		if( (dx+lx)<clip_end_x) dlx=lx;				// right clip?
		else dlx=clip_end_x-dx;

		dest=bak_buf+dx*3+(iy+y)*640*3;
		
		if(ix<dlx) asm_add_linked_line_clip(dest,dx*3,y+iy,lx*3);
		i+=ax;
	}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of file */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */


