/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SVGA screen functions library*/
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Missing function flag SVGA_mode_available(long mode)

#include "globals.h"
#include	"svga.h"
#include	"uvbelib.h"

SV_devCtx * SV_vars;

void _cdecl VBE_fatalError(char *msg)
{
	printf("%s\n", msg);
	exit(1);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Palette library */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

VBE_palette win_pal[256];

void set_palette(char *source,long index,long length)
{
	long i;

	if(use_UNIVBE==YES)
	{
		for(i=0;i<length;i++)
		{
			win_pal[index+i].red=source[i*3+0]<<2;
			win_pal[index+i].green=source[i*3+1]<<2;
			win_pal[index+i].blue=source[i*3+2]<<2;
			win_pal[index+i].alpha=0;
		}
 		SV_setPalette(index,length,win_pal,-1);
	}
	else
	{
		for(i=0;i<length;i++)
		{
			outp(0x3c8,index+i);
			outp(0x3c9,source[(i*3)+0]);
			outp(0x3c9,source[(i*3)+1]);
			outp(0x3c9,source[(i*3)+2]);
		}
	}
}

void set_pen(long pen,char r,char g,char b)
{
	if(use_UNIVBE==YES)
	{
		win_pal[pen].red=r<<2;
		win_pal[pen].green=g<<2;
		win_pal[pen].blue=b<<2;
		win_pal[pen].alpha=0;
		SV_setPalette(pen,1,win_pal,-1);
	}
	else
	{
		outp(0x3c8,pen);
		outp(0x3c9,r);
		outp(0x3c9,g);
		outp(0x3c9,b);
	}
}

void internal_message(text *string)
{
	printf(string);
}

void colour_bar(long pen,char r,char g,char b)
{
//	set_pen(pen,r,g,b);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SVGA UnInitialisation */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void  SVGA_uninit(void )
{
	if(use_UNIVBE==YES)
	{
		if(UNIVBE_installed==YES) UV_exit();
	}
	else VESA_close();
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SVGA Initialisation */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag  SVGA_init(void )
{
	flag worked;

	SVGA_currentmode=-1;

	if(use_UNIVBE==YES)
	{
		printf("UNIVBE Version\n");
		UNIVBE_installed=YES;
		worked=UV_install("",YES,YES);
		switch(worked)
		{
			case UV_ok:
				printf("UVBELib successfully installed the driver\n");
				break;
			case UV_noDetect:
				printf("Driver file is configured for standard VGA\n");
				break;
			case UV_alreadyVBE20:
				printf("VBE 2.0 detected and ignoreVBE was false\n");
				break;
			case UV_laterVersion:
				printf("A later TSR version of UniVBE is installed\n");
				break;
			case UV_lastNonFatal:
				printf("Last non fatal error code\n");
				break;
			case UV_errNotFound:
				printf("Could not find UNIVBE.DRV driver file\n");
				break;
			case UV_errNotValid:
				printf("File loaded not a valid driver file\n");
				break;
			case UV_errOldVersion:
				printf("Attempt to load older driver file version\n");
				break;
			case UV_errNoMemory:
				printf("Could not allocate memory to load driver\n");
				break;
			case UV_errNoRealMemory:
				printf("Could not allocate real mode memory\n");
				break;
			case UV_errGenerate:
				printf("Error generating UNIVBE.DRV driver file\n");
				break;
			case UV_errCheckInstall:
				printf("Check install function failed\n");
				break;
			default:
				printf("Unknown Error from UNIVBE Install\n");
				break;
		}
		
		if(worked>UV_lastNonFatal)
		{
			printf("UNIVBE Fatal error: %s\n",UV_getErrorStr(worked));
			exit(-1);
		}

		/*
		printf("UNIVBE error string: %s\n",UV_getErrorStr(worked));
		printf("UNIVBE copyright string: %s\n",UV_getCopyrightStr());
		printf("UNIVBE Major Version string: %s\n",UV_getMajorVersion());
		printf("UNIVBE Minor Version string: %s\n",UV_getMinorVersion());
		printf("UNIVBE Release Date string: %s\n",UV_getReleaseDate());
		
		printf("UNIVBE Super VGA string: %s\n",UV_getSuperVGAStr());
		printf("UNIVBE DAC string: %s\n",UV_getDACStr());
		printf("UNIVBE Clock string: %s\n",UV_getClockStr());
		*/

		/*
		if (UV_install("",YES,YES)!=TRUE)
		{
			printf( "\nCould not install UNIVBE\n" );
			UNIVBE_installed=NO;
		}
		*/

		/*
		if( SV_init()==0) worked=FALSE;
		else worked=TRUE;
		*/
		SV_vars=SV_init(NO);
		worked=TRUE;
	
	}
	else
	{
		printf("VESA Version\n");
		worked=VESA_init();
	}

	return worked;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SVGA Is Mode Available? */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag UNIVBE_is_mode_available(ushort number)
{
	ushort			*modes;
	VBE_modeInfo	mi;
	
	for (modes = SV_vars->modeList; *modes != 0xFFFF; modes++)
 	{
		if (!VBE_getModeInfo(*modes,&mi))
		continue;
	
		if( *modes==number) return YES;
	}
	return NO;	
}

flag SVGA_is_mode_available(long mode)
{
	if(use_UNIVBE==YES)
		return UNIVBE_is_mode_available(mode);
	else
		return VESA_is_mode_available(mode);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SVGA Set Mode */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag  SVGA_set_mode(long mode)
{
//	VBE_modeInfo modeInfo;
	flag worked;

	if(mode==SVGA_currentmode)
	{
		currentband=-1;
		SVGA_set_display_start(0,0,YES);
 		return OK;
	}

	if(use_UNIVBE==YES)
	{
//		VBE_getModeInfo(mode,&modeInfo);
//		vbeMdLinear	

		worked=SV_setMode(mode,YES,NO,0);
//		worked=SV_setVirtualMode(mode,640,480,YES,YES,2);
		SVGA_logical_length=SV_vars->bytesperline;
	}
	else
	{ 
		worked=VESA_init_mode(mode);
	}
	asm_set_UNIVBE_status(use_UNIVBE);
	currentband=-1;
						 
	if(worked==YES) SVGA_currentmode=mode;

	return worked;
}

void set_band(long band);
#pragma aux set_band parm caller [ebx];

void setvesaband( long band )
{
//	asm_set_band(band);
	if(use_UNIVBE==YES) asm_set_band(band);		// for UNIVBE
	else //set_band(band);								// for standard VESA
	VESA_set_band(band);

	/*
union REGS regs;
struct SREGS sregs;

//
//	SET UP REAL-MODE CALL STRUCTURE
//
	memset( &RMI, 0, sizeof( RMI ));
	RMI.EAX = 0x00004f05;						//	call service 39h ah=0x39
	RMI.EBX = 0x00000000;
	RMI.EDX = band * 64 / VESA_wingran;

//
//	USE DMPI CALL 300H TO ISSUE THE DOS INTERRUPT
//
	memset( &sregs, 0, sizeof( sregs ));
	regs.w.ax = 0x0300;
	regs.h.bl = 0x10;
	regs.h.bh = 0;
	regs.w.cx = 0;
	sregs.es = FP_SEG( &RMI );
	regs.x.edi = FP_OFF( &RMI );
	int386x( 0x31, &regs, &regs, &sregs );
	*/
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SVGA set display start */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void SVGA_set_display_start(long x,long y,flag waitVRT)
{
	if(use_UNIVBE==YES) VBE_setDisplayStart(x,y,waitVRT);
	else VESA_set_display_start(x,y,waitVRT);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* SVGA Pixel Functions */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void plotpixel(int pos,char c)
{
	char *j=(char *) screen_lin_addr;
	long newband;
	newband=(pos>>16);
	if (newband!=currentband) 
	{
		setvesaband(newband);
		currentband=newband;
	}
	j+=(pos & 0xffff);
	*j=c;
}						 

char readpixel(int pos)
{
	char *j=(char *) screen_lin_addr;
	long newband;
	newband=(pos>>16);
	if (newband!=currentband) 
	{
		setvesaband(newband);
		currentband=newband;
	}
	j+=(pos & 0xffff);
	return *j;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Screen Dump Functions */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void screen_copy_line(ubyte *destbuff,long dest,long width)
{
	char *vga=(char *) screen_lin_addr;
	long x;
	long destmod=dest&0xffff;

	if(width==0) { currentband=-1; return; }

	if((dest>>16)!=currentband)
	{
		currentband=dest>>16;
		setvesaband(currentband);
	}
	if(((dest+width)>>16)!=currentband)
	{
		x=65536-destmod;							// distance to band change
		memcpy(destbuff,vga+destmod,x);		// copy to band change
		currentband++;								// increment band
		setvesaband(currentband);				// set new band
		memcpy(destbuff+x,vga,width-x);		// copy remainder
	}
	else memcpy(destbuff,vga+destmod,width);
}

void screen_dump_line(char *source,int dest,int width)
{
	char *vga=(char *) screen_lin_addr;
	int x;
	int destmod=dest&0xffff;

	if((dest>>16)!=currentband)
	{
		currentband=dest>>16;
		setvesaband(currentband);
	}
	if(((dest+width)>>16)!=currentband)
	{
		x=65536-destmod;						// distance to band change
		memcpy(vga+destmod,source,x);		// copy to band change
		currentband++;							// increment band
		setvesaband(currentband);				// set new band
		memcpy(vga,source+x,width-x);			// copy remainder
	}
	else memcpy(vga+destmod,source,width);
}	

void screen_set_line(long dest,long width,ubyte colour)
{
	char *vga=(char *) screen_lin_addr;
	long x;
	long destmod=dest&0xffff;

	if(width==0) { currentband=-1; return; }

	if((dest>>16)!=currentband)
	{
		currentband=dest>>16;
		setvesaband(currentband);
	}
	if(((dest+width)>>16)!=currentband)
	{
		x=65536-destmod;						// distance to band change
		memset(vga+destmod,colour,x);		// copy to band change
		currentband++;							// increment band
		setvesaband(currentband);				// set new band
		memset(vga,colour,width-x);			// copy remainder
	}
	else
		memset(vga+destmod,colour,width);
}	

void screen_andor_line(char *source,int dest,int width)
{
	char *vga=(char *) screen_lin_addr;
	int x;
	int destmod=dest&0xffff;

	if((dest>>16)!=currentband)
	{
		currentband=dest>>16;
		setvesaband(currentband);
	}
	if(((dest+width)>>16)!=currentband)
	{
		x=65536-destmod;					// distance to band change
		asm_andor_line(source,vga+destmod,x);
		source+=x;

		currentband++;							// increment band
		setvesaband(currentband);				// set new band
		asm_andor_line(source,vga,width-x);
	}
	else
		asm_andor_line(source,vga+destmod,width);
}	

void screen_remap_line(char *source,int dest,int width)
{
	char *vga=(char *) screen_lin_addr;
	int x;
	int destmod=dest&0xffff;

	if((dest>>16)!=currentband)
	{
		currentband=dest>>16;
		setvesaband(currentband);
	}
	if(((dest+width)>>16)!=currentband)
	{
		x=65536-destmod;						// distance to band change
		asm_remap(vga+destmod,vga+destmod,source,x);		// copy to band change
		currentband++;							// increment band
		setvesaband(currentband);				// set new band
		asm_remap(vga,vga,source,width-x);			// copy remainder
	}
	else asm_remap(vga+destmod,vga+destmod,source,width);
}

void mem2screen_remap_line(char *table,char *source,int dest,int width)
{
	char *vga=(char *) screen_lin_addr;
	int x;
	int destmod=dest&0xffff;

	if((dest>>16)!=currentband)
	{
		currentband=dest>>16;
		setvesaband(currentband);
	}
	if(((dest+width)>>16)!=currentband)
	{
		x=65536-destmod;						// distance to band change
		asm_remap(source,vga+destmod,table,x);		// copy to band change
		currentband++;							// increment band
		setvesaband(currentband);				// set new band
		asm_remap(source+x,vga,table,width-x);			// copy remainder
	}
	else asm_remap(source,vga+destmod,table,width);
}

void dump_the_screen(int screen,int x,int y)
{
	int dest;
	int iy;

	dest=(screen+split_height)*SVGA_logical_length; 
	for(iy=y;iy<(viewable_height+y);iy++)
	{
		//screen_dump_line(dump_blk_buf+x+y_mod640[iy],dest,viewable_width);
		asm_add_dump_line(dump_blk_buf+x+y_mod640[iy],dest,viewable_width);
		dest+=SVGA_logical_length;
	}
}

void screen_dump(int start_x,int start_y,char *source,int width,int height)
{
	long y,index;
	for(y=0;y<height;y++)
	{
		index=SVGA_logical_length*(y+start_y)+start_x;
		asm_add_dump_line(source,index,width);
		//screen_dump_line(source,index,width);
		source+=width;
	}
}

/*------------------------------------------------------------------------------------------------------------------------------*/
/*	POSITION CURSOR AT Y, X TEXT POSITION																						*/
/*------------------------------------------------------------------------------------------------------------------------------*/

void at( long r, long c )
{
		union REGS regsin;
		regsin.w.ax = 0x200;
		regsin.w.bx = 0;
		regsin.w.dx = ( r << 8 ) + c;
		int386( 0x10, &regsin, &regsin );
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
