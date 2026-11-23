/*-------------------------------------------------------------------------------------------------------------------------------- */
/*-------------------------------------------------------------------------------------------------------------------------------- */
typedef char					boolean;
typedef unsigned long		ulong;    /* unsigned 32-bit quantity */
#ifdef	PSX
typedef unsigned long		Uint32;    /* unsigned 32-bit quantity */
typedef signed long			Sint32;    /* unsigned 32-bit quantity */
typedef unsigned short		Uint16;    /* unsigned 16-bit quantity */
typedef signed short			Sint16;    /* unsigned 16-bit quantity */
typedef unsigned char		Uint8;    /* unsigned 8-bit quantity */
typedef signed char			Sint8;    /* unsigned 8-bit quantity */
#else
typedef unsigned long		u_long;    /* unsigned 32-bit quantity */
typedef unsigned short		u_short;    /* unsigned 16-bit quantity */
typedef unsigned char		u_char;    /* unsigned 8-bit quantity */
#endif
typedef short					word;	    /* signed 16-bit quantity */
typedef unsigned short		uword;    /* unsigned 16-bit quantity */
typedef char					byte;	    /* signed 8-bit quantity */
typedef unsigned char		ubyte;    /* unsigned 8-bit quantity */
typedef char					text;	    /* unsigned 8-bit quantity */
typedef unsigned char		flag;	    /* unsigned 8-bit quantity */
typedef unsigned char		FILE;	    /* unsigned 8-bit quantity  */

#ifndef	ext
#define	ext	extern
#endif

#ifndef	NULL
#define	NULL	0
#endif

/*-------------------------------------------------------------------------------------------------------------------------------- */
/*	CONSTANTS */
/*-------------------------------------------------------------------------------------------------------------------------------- */

/*
#define TRUE		1
#define FALSE		0
*/

#define OK			1
#define FAIL		0
#define NULL		0

#define YES			1
#define NO			0

#define	ON			1
#define	OFF		0

#define NEITHER		99

#define	NOT_FOUND	-1
#define	fadein				Fade_up
#define	fadeout				Fade_down

#define	at(_x_,_y_);
#define	SVGA_set_mode(_mode_);
#define	setup_SVGA_screen();
#define	setvgamode(_mode_);
#define	remap_gfx_block(_b_,_p_,_d_);
#define	remap_raw_block(_b_,_p_,_d_,_w_,_h_);
#define	screen_set_line(_d_,_w_,_c_);
#define	screen_set_line1(_d_,_w_,_c_);
#define	fix_palette_to_black();
#define	screen_dump_line(_s_,_d_,_w_);
#define	screen_remap_line1(_s_,_ss_,_d_,_dd_,_w_);

#define	find_which_key_input()	NULL
#define	find_which_key_input1()	NULL

#define	fracmul(_a_,_b_) ((_a_)*(_b_))>>16
#define	fracdiv(_a_,_b_) (((_a_)<<16)/(_b_))

#define	get_key()	NULL
#define	ltoa			itoa

// RESTORE: through FILE_IO.C's FileOpen, FileRead and FileClose, which use
// the PC link or, in a disc build, the CD (these were PCopen, PCclose, PCread)
#define	open_file_to_read( _name_ )	FileOpen( _name_, 0, 0 )
#define	close_the_read_file( _fp_ )	FileClose( _fp_ )
#define	fread(_ad_, _d_, _l_, _fp_)	FileRead( _fp_, (char*)_ad_, _l_)


#ifdef	PSX
#define	slTVOff()					SetDispMask(0)
#define	slTVOn()						SetDispMask(1)
#define	slSynch()					VSync(0)
#define	wait_for_vblank()			VSync(0)
#define	wait_for_VBE_vblank(_c_)	VSync(0)
#define	wait_for_VBE_vblank1(_c_)	VSync(0)
#define	scan_for_keys()			get_pad()
#define	scan_for_keysjp()			get_pad()


#define	CLUTaddr(_x_,_y_)	((_y_<<6)+(_x_>>4))

#define	RGB16_COLOR(_r,_g,_b) (short) ((_b <<10 ) + (_g << 5 ) + ( _r ))
#define	RGBT6_COLOR(_r,_g,_b) (short) ((_b <<10 ) + (_g << 5 ) + ( _r )) + 0x8000

#else

#define	wait_for_vblank()	slSynch()

#endif

typedef double matrix4x4[4][4];
typedef double matrix4x1[4];
