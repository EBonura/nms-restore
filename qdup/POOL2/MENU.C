#include	"globals.h"
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
extern	int	MenuXpos[][ MAX_ICONS ];
extern	int	MenuYpos[][ MAX_ICONS ];
extern	long	*MenuData[][ MAX_ICONS ];
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
ext	void	rle2raw(ubyte *source, ubyte *dest);
flag	DecodePad(void);
flag	Choose_View(void);	// RESTORE: new/viewchoice.c
void	Baize_Felt_Load(void);	// RESTORE: new/baizeicon.c
void	Title_Press_Start(void);	// RESTORE: new/viewchoice.c, the PRESS START prompt
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	end_of_killer_screen( void )
{
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	end_of_frame_screen( void )
{
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	show_game_controls( void )
{
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	quit_frame_screen( void )
{
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	Save_shot_Room( void )
{
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	draw_gfx_block(long	gfx_num,long	dest, ubyte	*source, int masked_flag, int name_index, int direction)
{
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	sync_stage( void )
{
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	reset_atract_counter( void )
{
//g   in_atract_mode=ATRACT_DELAY;
//g 	whichroom=save_status;
//g   exit_atract=0;
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	Enter_Main_Options( void )
{
#ifdef	PSX

	LoadAllWrapperGfx();
	Baize_Felt_Load();	// RESTORE: the baize colours for the rotating icon (new/baizeicon.c)
  
	InitCamera();

	opt_level = 0;
	
	opt_frame = cursor = C_START;

	SetupAllIcons();

	clipd = 1;

	// RESTORE: owner's decision (2026-11-18: "the title screen should at least have a \"press start\" to indicate we can move forward"): PRESS START under the picture while it waits (new/viewchoice.c)
	Title_Press_Start();
	// RESTORE: owner's decision (2026-11-13): Start or Cross, as Cross enters elsewhere in this menu
	while((PadBounce != PAD_START) && (PadBounce != PAD_RD)) get_pad();

	textpos = 500;


#else

	Ram_Load();
	
	fade_type=1;
	fade_val=64+32+1;
	Fade_down();
	
	BackScreen = LoadFile("gfx/optscrn.psx");
	DrawBackdrop();
	ConvertPalette( BackScreen, 256, 0, 480);
	free_last_ram_block(BackScreen);

	OptSprites = LoadFile("gfx/option.psx");
	BalSprites = LoadFile("gfx/balls.psx");

	opt_frame = 0;

	Fade_up();
#endif



	while(DecodePad()!=OK)
		{

		AnimateCurrentIcon();

		cursor = opt_level*5+opt_frame;
		DrawOptionsScreen();
		}


}

//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
int	not_built_timer = 0;	// RESTORE: frames left to show the message for an icon NMS never wrote
flag	DecodePad(void)
{
	int	i;
	flag	retval = FAIL;	// RESTORE: was unset; only C_START sets it, the other icons returned what the stack held
	
	get_pad();

	switch(cursor)
		{
		case	C_OPTIONS:
				if(PadBounce & PAD_RD)
					{
					opt_level = 1;
					opt_frame = 0;
					SetupAllIcons();
					}
				break;

		case	C_BALLS:
// RESTORE: three choices, not the four models: UK pool, 9-ball and US 8-ball
// (BallType 1, 0, 2).  The fourth model, "trick", was mapped to TRICK_SHOT,
// a mode NMS's code only reaches with a saved trick-shot file; the owner took
// it off the icon (POOL.C ball_game_type).
				if(PadBounce & PAD_LL) BallType--;
				if(PadBounce & PAD_LR) BallType++;
				if(BallType<0) BallType = BALLCHOICES-1;
				if(BallType==BALLCHOICES) BallType = 0;
				break;

		case	C_TABLE:
				if(PadBounce & PAD_LL) TableType--;
				if(TableType<0) TableType = TABLEMODELS-1;
				if(PadBounce & PAD_LR) TableType++;
				if(TableType==TABLEMODELS) TableType = 0;
				break;

// RESTORE: the baize icon shared the table icon's variable (and so cycled the
// four table models); it has its own, three choices: the three baize
// colours LEVELDAT.C's palette tables hold (blue, green, red).
		case	C_BAIZE:
				if(PadBounce & PAD_LL) BaizeType--;
				if(BaizeType<0) BaizeType = BAIZECHOICES-1;
				if(PadBounce & PAD_LR) BaizeType++;
				if(BaizeType==BAIZECHOICES) BaizeType = 0;
				break;

		case	C_PLAYERS:
				if(PadBounce & (PAD_LL|PAD_LR)) PlayType = PlayType ? 0:1;
				break;

// RESTORE: owner's decision (2026-11-13: "let's provide some feedback if a feature was never implemented, even a quick
// message on screen will do").  The speaker, pad, card and trophy icons (second page) have no code in NMS's build.
// Cross on one shows a short message in the debug font NMS's code writes its text with (FntPrint), for a moment.
		case	C_SPEAKER:
		case	C_PAD:
		case	C_CARD:
		case	C_TROPHY:
				if(PadBounce & PAD_RD) not_built_timer = 100;
				break;

		case	C_START:
				if(PadBounce & (PAD_RD|PAD_START)) retval = Choose_View();	// RESTORE: owner's decision: the 2D or 3D choice, new/viewchoice.c; FAIL (Circle) stays here
				else retval = FAIL;
				break;

		}
	if(not_built_timer > 0)	// RESTORE: see the speaker case above; the choice screen's panel (new/viewchoice.c) draws the message while this counts down
		{
		not_built_timer--;
		}
	i = opt_frame;

	// RESTORE: owner's decision (2026-11-13): Circle goes back as well as Square (NMS's)
	if((PadBounce & (PAD_RL|PAD_RR)) && (opt_level == 1))
		{
		opt_level = 0;
		opt_frame = C_OPTIONS;
		SetupAllIcons();
		}

	if(PadBounce & PAD_LU) opt_frame--;
	if(PadBounce & PAD_LD) opt_frame++;

	if(opt_frame <0) opt_frame = 4;
	if(opt_frame >4) opt_frame = 0;

	if(i!=opt_frame) DecompIcon(i);

#ifdef	QDUP_VRAM_VIEWER	// RESTORE: owner's decision (2026-11-13): the debug VRAM viewer is off in disc builds; QDUP_CFLAGS=-DQDUP_VRAM_VIEWER keeps it for tool builds
	if(PadBounce & PAD_SEL) Show_VRAM();
#endif

	return(retval);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	Ram_Load( void )
{
	int	i,j;
	balls_xy		= (long*) Get_Ram_Ptr(640);	// RESTORE: 8.24, was double
	#ifdef	PSX
	for(j=0;j<MAX_ICONS;j++)
		{
		BufferPtr[j][0] = (ubyte*) Get_Ram_Ptr(ICON_SIZE);
		BufferPtr[j][1] = (ubyte*) Get_Ram_Ptr(ICON_SIZE);
		}

	#else
	ExpandBuffer= (ubyte*) Get_Ram_Ptr(96*96);
	#endif

}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
/**************************************
*     DEPACK BITSTREAM TO TEMPRAM     *
**************************************/

void bit2raw(ubyte *source,ubyte *dest)
{

	ubyte	*src_cols; 
	long	destlen;
	ubyte	mask;

	destlen=*(u_long *)source;				/* FIRST LONG=LENGTH */
	source+=4;									/* MOVE ONTO DATA */
	src_cols=source+((destlen-1)>>3)+1;	/* FIND LIST OF COLOURS */
	while(destlen>0)							/* CHECK FOR END OF DATA */
	{
 		for (mask =0x80;mask!=0;mask>>=1)
		{
 			if ((*source & mask)!=0) *dest++=*src_cols++;
	   	 	else *dest++=0;
		}
		source++;
		destlen-=8;  
	}
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
#ifndef	PSX
void	rle2raw(ubyte *source, ubyte *dest)
{

	ubyte		temp;
	ubyte		col;
	u_long	src_len;

	src_len=*(u_long *)source;		// FIRST LONG=LENGTH
	source+=4;							// MOVE ONTO DATA
	while(src_len!=0)					// CHECK FOR END OF DATA
	{
		temp=*source++;
		if (temp>0x80)
		{
			src_len -=2;
			col   =*source++;
			for (temp=1-temp;temp!=0;temp--)	*dest++ = col;
		}
 		else
		{
			temp++;
			src_len-=temp+1;
			for (;temp!=0;temp--) *dest++=*source++;
		}
	}
}
#endif
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
char*	shorten_name(long	name_name_index)
{
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
