/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Mission Briefing */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include	"globals.h"

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Process Collisions... */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void load_fonts(void)
{
	vfont1=load_file("GRABFONT\\VFONT1.BIN");
	vfont2=load_file("GRABFONT\\VFONT2.BIN");
	vfont3=load_file("GRABFONT\\VFONT3.BIN");
	vfont4=load_file("GRABFONT\\VFONT4.BIN");
	vfont5=load_file("GRABFONT\\VFONT5.BIN");
	vfont_pal=load_file("GRABFONT\\VECTFONT.PAL");
}

void print_string_tobox(char *font,long name,int start_x,int start_y,char *string)
{
	int i,ptr;
	int x,y;
	int width;
	int y_start,y_end;
	char c;
	long *table;
	char *buffer;


	table=(long *) font;
	buffer=font+256*4;

	while(*string!=0)
	{
		c=*string++;
		if(c==' ') start_x+=12;
		else
		{
		i=(int ) c;
		ptr=table[i];
		width=(int ) buffer[ptr++];
		y_start=(int ) buffer[ptr++];
		y_end=(int ) buffer[ptr++];
		y_end+=y_start;

		for(y=y_start;y<y_end;y++)
		{
			for(x=0;x<width;x++)
			{
				c=buffer[ptr++];
				draw_pixel_to_sprite(seqdata[name],start_x+x,start_y+y,c);
				//plotpixel((start_y+y)*640+start_x+x,c);
			}
		}
		start_x+=width+1;
		}
	}
}

void print_string_24bit(char *font,int start_x,int start_y,char *string)
{
	int i,ptr;
	int x,y;
	int width;
	int y_start,y_end;
	long c,r,g,b;
	long *table;
	unsigned char *buffer;

	table=(long *) font;
	buffer=font+256*4;

	while(*string!=0)
	{
		c=*string++;
		if(c==' ') start_x+=12;
		else
		{
		i=(int ) c;
		ptr=table[i];
		width=(int ) buffer[ptr++];
		y_start=(int ) buffer[ptr++];
		y_end=(int ) buffer[ptr++];
		y_end+=y_start;

		for(y=y_start;y<y_end;y++)
		{
			for(x=0;x<width;x++)
			{
				c=buffer[ptr++];
				if(c!=0)
				{
					b=pal_32bit[c*4+0];
					g=pal_32bit[c*4+1];
					r=pal_32bit[c*4+2];
					plotpixel((start_y+y)*SVGA_logical_length+(start_x+x)*3+0,b);
					plotpixel((start_y+y)*SVGA_logical_length+(start_x+x)*3+1,g);
					plotpixel((start_y+y)*SVGA_logical_length+(start_x+x)*3+2,r);
				}
			}
		}
		start_x+=width+1;
		}
	}
}

void print_string_8bit(char *font,int start_x,int start_y,char *string)
{
	int i,ptr;
	int x,y;
	int width;
	int y_start,y_end;
	long c;
	long *table;
	unsigned char *buffer;

	table=(long *) font;
	buffer=font+256*4;

	while(*string!=0)
	{
		c=*string++;
		if(c==' ') start_x+=12;
		else
		{
		i=(int ) c;
		ptr=table[i];
		width=(int ) buffer[ptr++];
		y_start=(int ) buffer[ptr++];
		y_end=(int ) buffer[ptr++];
		y_end+=y_start;

		for(y=y_start;y<y_end;y++)
		{
			for(x=0;x<width;x++)
			{
				c=buffer[ptr++];
				if(c!=0)
				{
					plotpixel((start_y+y)*SVGA_logical_length+start_x+x+0,c);
				}
			}
		}
		start_x+=width+1;
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Display Mission Briefing Screen */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void mission_briefing(char *level_fname)
{
	char 		*tga_buf,*bak_buf,*lbm_buf;
	long i;

	return;

	clear_heap();

 	if(SVGA_is_mode_available(0x112)==YES)
 	{
		SVGA_set_mode(0x112);
		sprintf(filename,"GFX/24BIT/%s.tga",level_fname);
		tga_buf=heap_load_file(filename);
		if(tga_buf!=NULL)
		{
			read_tga_header(tga_buf);
			bak_buf=heap_malloc(return_tga_width()*return_tga_height()*3);
			decode_tga(tga_buf,bak_buf);
			lbm_buf=readlbm("GFX/24BIT/GRID.LBM");
			if(lbm_buf!=NULL)
			{
				promote_to_24bit(palette);
				for(i=0;i<480;i++)
				{
					asm_transparent_expand_to_24bit(lbm_buf+i*640,bak_buf+(i*640*3),pal_32bit,640);
					screen_dump_line(bak_buf+(i*640*3),i*SVGA_logical_length,640*3);
				}
//				load_fonts();
				
				promote_to_24bit(vfont_pal);
				print_string_24bit(vfont1,32,32,"MISSION BRIEFING:");
				print_string_24bit(vfont2,32,64,"RESCUE PODS...");
				print_string_24bit(vfont2,32,80,"...AND GET OUT");
			}			
			//free(lbm_buf);
			//free(tga_buf);
			//free(bak_buf);
		}
	}

	/*
	SVGA_set_mode(0x101);

	lbm_buf=readlbm("GFX/24BIT/GRID.LBM");
	if(lbm_buf==NULL) return;
	set_palette(palette,0,256);

	for(i=0;i<480;i++) screen_dump_line(lbm_buf+(i*640),i*SVGA_logical_length,640);

	print_string_8bit(vfont4,100,100,"HELLO WORLD");
	*/
	
	clear_heap();
}
