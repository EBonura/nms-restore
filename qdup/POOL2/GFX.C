#include	"globals.h"
#ifdef	PSX
//#include	"ballgfx.h"	// RESTORE: Sony sample data, not in this repo; GFX.C uses nothing from it
#endif

ext	OBJECT	*cue_ptr;
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void InitVideo(int res)
{
#ifdef	PSX

	switch(res)
		{
		case	LO_RES:
				screen_height = FRAME_YL;
				screen_width = FRAME_XL;
				break;
		case	MED_RES:
				screen_height = FRAME_YH;
				screen_width = FRAME_XL;
				break;
		case	HI_RES:
				screen_height = FRAME_YH;
				screen_width = FRAME_XH;
				break;
		}


	ResetGraph(0);				// Reset Graphic SubSystem
	SetGraphDebug(0);			// Turn Off All Debug Messages

	InitGeom();
//	SetGeomOffset(240,272);
	SetGeomOffset(MODEL_X,MODEL_Y);
	SetGeomScreen(SCR_Z);
	SetVideoRes(res);	// Set Screen Res To 640 * 256

#else

#endif
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void SetVideoRes(int res)
{
#ifdef	PSX
	active_page = 0;
	SetDispMask(0);			// Turn Screen Off
	// RESTORE: BUG FIX. screen_height WAS SET BY InitVideo ALONE (480, ONCE AT START), SO display_screen TOOK THE 480-LINE PATH IN EVERY
	// FRAME OF THE 240-LINE GAME AS WELL. IT FOLLOWS THE MODE SET HERE.
	screen_height = (res == LO_RES) ? FRAME_YL : FRAME_YH;
	screen_width = (res == HI_RES) ? FRAME_XH : FRAME_XL;
	switch(res)
		{
		case	LO_RES:
				SetDefDrawEnv(&Display[0].draw, 0,   0,        FRAME_XL, FRAME_YL);
				SetDefDispEnv(&Display[0].disp, 0,   FRAME_YL, FRAME_XL, FRAME_YL);
				SetDefDrawEnv(&Display[1].draw, 0,   FRAME_YL, FRAME_XL, FRAME_YL);
				SetDefDispEnv(&Display[1].disp, 0,   0,        FRAME_XL, FRAME_YL);
				Display[1].disp.screen.x = Display[0].disp.screen.x = SCREEN_XL;
				Display[1].disp.screen.y = Display[0].disp.screen.y = SCREEN_YL;
				Display[1].disp.screen.h = Display[0].disp.screen.h = FRAME_YL;
				break;

		case	MED_RES:
				SetDefDrawEnv(&Display[0].draw, 0,   0,        FRAME_XL, FRAME_YH);
				SetDefDispEnv(&Display[0].disp, 0,   0, 		  FRAME_XL, FRAME_YH);
				SetDefDrawEnv(&Display[1].draw, 0,   0,		  FRAME_XL, FRAME_YH);
				SetDefDispEnv(&Display[1].disp, 0,   0,        FRAME_XL, FRAME_YH);
				Display[1].disp.screen.x = Display[0].disp.screen.x = SCREEN_XL;
				Display[1].disp.screen.y = Display[0].disp.screen.y = SCREEN_YH;
				break;

		case	HI_RES:
				SetDefDrawEnv(&Display[0].draw, 0,   0, FRAME_XH, FRAME_YH);
				SetDefDispEnv(&Display[0].disp, 0,   0, FRAME_XH, FRAME_YH);
				SetDefDrawEnv(&Display[1].draw, 0,   0, FRAME_XH, FRAME_YH);
				SetDefDispEnv(&Display[1].disp, 0,   0, FRAME_XH, FRAME_YH);
				Display[1].disp.screen.x = Display[0].disp.screen.x = SCREEN_XH;
				Display[1].disp.screen.y = Display[0].disp.screen.y = SCREEN_YH;
				break;
		}
	Display[0].draw.r0 = Display[1].draw.r0 = 0x40;
	Display[0].draw.g0 = Display[1].draw.g0 = 0x80;
	Display[0].draw.b0 = Display[1].draw.b0 = 0xc0;
	Display[0].draw.isbg = Display[1].draw.isbg = 1;

	clipw = cliph = clipd = 0;

	get_page();
	display_screen();

	SetVideoMode(VIDEO_MODE);
	ResetGraph(1);
	SetDispMask(1);
#else

#endif
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
#ifdef	PSX
void get_page(void)
{
	active_page = active_page ? 0:1;
	cdl = &Display[ active_page ];	/* this is our structure for the video ouput params */
	o_t = cdl->ot;
	ClearOTagR( o_t, OT_SIZE );
}
#else

#endif
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	AdjustClipArea( void )
{
	if(clipd != 0)
		{
		if((clipd == -1) && (clipw!=0))
			{
			clipw-=2;
			cliph-=3;
			if(clipw==0) clipd = 0;
			}

		if((clipd == 1) && (clipw!=160))
			{
			clipw+=2;
			cliph+=3;
			if(clipw==160) clipd = 0;
			}
		}

	SetClipArea( clipw, cliph );
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	SetClipArea( int cw, int ch)
{
	int	cx,cy;
	cx = 160-cw;
	cy = 240-ch;
	setRECT(&Display[0].draw.clip, cx, cy, cw*2, ch*2);
	setRECT(&Display[1].draw.clip, cx, cy, cw*2, ch*2);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void display_screen(void)
{
#ifdef	PSX
	// RESTORE: BUG FIX. THE OPTION SCREEN IS ONE 320 x 480 INTERLACED PICTURE IN ONE BUFFER, DRAWN WITH dfe 0. ON THE CONSOLE (HARDWARE
	// TESTS v2.24, ROWS B) A dfe 0 DRAW IN THE PICTURE SKIPS THE LINES OF THE FIELD ON SHOW, ALTERNATING FIELD BY FIELD, AND A DRAW IN THE
	// VERTICAL BLANK SKIPS THE EVEN LINES EVERY TIME. VSync(0) THEN DrawOTag (v0.56, 1996) STARTED EVERY PASS IN THE BLANK, SO THE
	// BACKDROP, DRAWN FIRST, NEVER REACHED THE EVEN LINES: THEY KEPT THE TITLE PICTURE AND EVERY CLOSED PANEL (THE OWNER'S PHOTOGRAPH OF
	// v0.58). NOW EACH PASS STARTS AFTER A BLANK OF THE OTHER COUNT PARITY THAN THE LAST PASS, ONCE THAT BLANK IS OVER (VSyncField): THE
	// FIELDS TAKE TURNS AT EVERY BLANK, SO PASSES START AN ODD NUMBER OF FIELDS APART. AND THE PRIMITIVES ARE SENT ONE BY ONE, HOLDING
	// BEFORE ANY THAT WOULD MEET THE NEXT BLANK UNTIL THAT BLANK IS OVER (DrawOTagField): NOTHING IS DRAWN IN A BLANK, AND HOWEVER LONG A
	// PASS TAKES ON THE CONSOLE (ITS TEXTURED DRAWS ARE ABOUT 4.7 TIMES SLOWER THAN THE EMULATOR'S), EACH PRIMITIVE IS DRAWN IN THE OTHER
	// FIELD FROM ONE PASS TO THE NEXT. GPUSTAT IS NOT READ. dfe 1 IS NO ANSWER: THE SCREEN IS REDRAWN WHOLE EVERY PASS, BACKDROP FIRST,
	// AND WITH ONE BUFFER THE TELEVISION WOULD SHOW ICONS MISSING. 240-LINE SCREENS ARE UNCHANGED.
	if(screen_height == FRAME_YH)
		{
		static int	field = 0;
#ifndef	QDUP_TEST_ONE_FIELD		// RESTORE: test build only (tests/gameplay/qdup-fields.sh, the broken reference): every pass after a blank of one parity
		field ^= 1;
#endif
		VSyncField(field);
		PutDispEnv( &Display[ active_page ].disp );
		PutDrawEnv( &Display[ active_page ].draw );
		DrawOTagField(cdl->ot+OT_SIZE-1);
		}
	else
		{
		VSync(0);
		PutDispEnv( &Display[ active_page ].disp );
		PutDrawEnv( &Display[ active_page ].draw );
		DrawOTag(cdl->ot+OT_SIZE-1);
		}
   FntFlush(-1);
#else
	slSynch();
#endif
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	show_page( void )
{
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
text	*yesnostr[2]=
{	"NO",
	"YES"
};
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
text	*eventtxt[]=
{
	"-- Unknown --",
	"Rotate Camera",
	"Ball In Hand",
	"Raise Cue Butt",
	"Zoom Camera",
	"Taking Shot",
	"Add Masse",
	"Aiming",
	"Rotate 180",
	"-- Unknown --",
	"Move Camera",
	"-- Unknown --",
	"-- Unknown --",
	"-- Unknown --",
	"-- Unknown --",
	"-- Unknown --",
	"-- Unknown --",
	"-- Unknown --",
	"-- Unknown --",
	"-- Unknown --",
	"-- Unknown --",
	"-- Unknown --"
};
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//text	*playtxt[]=
//{
//	"WHITE",
//	"YELLOW",
//	"BLUE",
//	"RED",
//	"PURPLE",
//	"ORANGE",
//	"GREEN",
//	"BROWN",
//	"BLACK",
//};
ext	text* foul_messages[];
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
// RESTORE: the 3D view: Render (MY3DS.C) makes the frames, with
// DrawScreen2D below and the 3D renderer written for the restoration.  The
// physics (update_all_balls) and the shot loop (Show_Shot) also call
// DrawScreen, to show the 2D view while the balls move; with the 3D view
// chosen those calls show nothing and the next Render shows the balls.  The
// player picks the view after START (viewchoice.c); L2 and R2 together
// switch between them during the game.  The 2D view is NMS's own.
long	view_3d = YES;		// RESTORE: see above; the view the player chose (viewchoice.c)

void	DrawScreen2D( void );
void	DrawScreen( void )
{
	if(view_3d == NO)
		{
		DrawScreen2D();
		display_screen();
		}
}

void	Print_Control_Hints( void );	// RESTORE: new/hints.c
/* RESTORE: owner's decision (2026-11-20): an aiming aid in the flat view, a deliberate addition: no version of the
 * original drew one (NMS's PS1 flat view has a cue line written and commented out above, GFX.C; its Saturn flat view
 * and the DOS 3D view show the cue itself).  The commented-out line ran from the 3D cue's position to the screen
 * centre (160,120), which points at the table's centre, not along the aim.  This one is a dotted line: short dashes,
 * two pixels wide, every AIM_DASH_STEP pixels from the cue ball's centre (so the pattern stays put while the angle
 * is held) along the shot's direction, the direction hit_ball takes (cue_angle less a quarter turn, 4096 to the
 * turn), up to AIM_LINE_LEN pixels and cut at the box of the pockets' centres, while a shot can be taken. */
#define	AIM_LINE_LEN	80
#define	AIM_DASH_START	9		// the first dash's distance from the ball's centre: clear of its 11 pixel sprite
#define	AIM_DASH_LEN	3
#define	AIM_DASH_STEP	8		// a dash every 8 pixels from the ball, so the pattern does not crawl while the angle is held
#define	AIM_DASHES	10
static	POLY_F4	aim_dash[2][AIM_DASHES];
int	aim_line_on = 0;	// RESTORE: 1 while the line is drawn (the gates read it)
int	aim_line_draws = 0;	// RESTORE: how many passes in a row it has been drawn at the same angle and ball position
static	int	aim_line_sig = -1;
/* RESTORE: owner's decision (2026-11-20, "the whole stick"): a cue stick behind the cue ball, along the aim, for the flat view,
 * on top of the line. Flat quads, no texture: a blue tip, a white ferrule, a grey shaft that thickens and a brown butt with
 * a dark end, in the 3D cue's pens (STV3D.C cue_colour: 91, 204, 197, 232, 239 of the room palette, read by
 * Baize_Felt_Load).  It draws back with cue_position (metres, 50 pixels each beyond its rest value) while Cross is held and is gone once the
 * balls move (the strike), as the line is. Cut to the screen below the hint text.  Sorted below the balls, so the ball is never covered. */
extern	ubyte	cue_rgb[5][3];
int	aim_stick_on = 0;	// RESTORE: 1 while the stick is drawn (the gates read it)
#define	STICK_PARTS	5
static	POLY_F4	stick_part[2][STICK_PARTS];
static	void	Aim_Stick_2D( int cx, int cy, long dx, long dy )
{
	static	const	ubyte	len[STICK_PARTS] = { 3, 3, 42, 22, 6 };		// tip, ferrule, shaft, butt, butt end
	static	const	ubyte	hw0[STICK_PARTS] = { 1, 1, 1, 2, 3 };		// half widths at the near and far end of each
	static	const	ubyte	hw1[STICK_PARTS] = { 1, 1, 2, 3, 3 };
	int	k, s, pull, total, t, c;
	long	u, ex, ey;
	POLY_F4	*p;
	short	*xy;

	pull = ((int)(cue_position*100.0) - 22) / 2;	// at rest cue_position is four ball diameters (STV3D.C), the 3D cue halves it as well
	if(pull < 0) pull = 0;
	if(pull > 40) pull = 40;
	s = 9 + pull;										// the tip's distance from the ball's centre
	total = 0;
	for(k = 0; k < STICK_PARTS; k++) total += len[k];
	ex = cx - (int)((dx*(s+total))>>16);				// the butt's end
	ey = cy - (int)((dy*(s+total))>>16);
	t = 65536;											// cut to the screen below the hint text
	if(ex > 316) t = ((long)(316-cx) << 16) / (int)(ex-cx);
	if(ex < 4) t = ((long)(4-cx) << 16) / (int)(ex-cx);
	if((ey > 250) && ((u = ((long)(250-cy) << 16) / (int)(ey-cy)) < t)) t = u;
	if((ey < 52) && ((u = ((long)(52-cy) << 16) / (int)(ey-cy)) < t)) t = u;
	if(t < 0) t = 0;
	for(k = 0; k < STICK_PARTS; k++)
		{
		int	w0, w1;
		int	s0 = s, s1 = s + ((len[k]*t) >> 16);
		w0 = hw0[k];
		w1 = hw1[k];
		p = &stick_part[active_page][k];
		SetPolyF4(p);
		c = (k == 0) ? 0 : (k == 1) ? 1 : (k == 2) ? 2 : (k == 3) ? 3 : 4;
		p->r0 = cue_rgb[c][0];
		p->g0 = cue_rgb[c][1];
		p->b0 = cue_rgb[c][2];
		// the corners: near end left, near end right, far end left, far end right; the back vector is -(dx,dy), the side (-dy,dx)
		xy = &p->x0;
		xy[0] = cx + (int)((-dx*s0 - dy*w0) >> 16);	xy[1] = cy + (int)((-dy*s0 + dx*w0) >> 16);
		xy[2] = cx + (int)((-dx*s0 + dy*w0) >> 16);	xy[3] = cy + (int)((-dy*s0 - dx*w0) >> 16);
		xy[4] = cx + (int)((-dx*s1 - dy*w1) >> 16);	xy[5] = cy + (int)((-dy*s1 + dx*w1) >> 16);
		xy[6] = cx + (int)((-dx*s1 + dy*w1) >> 16);	xy[7] = cy + (int)((-dy*s1 - dx*w1) >> 16);
		AddPrim( o_t++, p);
		s = s1;
		}
}

void	Aim_Line_2D( void )
{
	int	a, i, len, cx, cy, ex, ey, minx, maxx, miny, maxy, px, py;
	long	dx, dy, t, u;

	if((ball[WHITE_BALL].active != YES) || (any_balls_moving() == YES))
		{
		aim_line_on = 0;
		aim_stick_on = 0;
		aim_line_draws = 0;
		aim_line_sig = -1;
		return;
		}
	cx = ((ball[WHITE_BALL].x*25)/0x400000)+160+5;		// the sprite's centre: its corner is at sx,sy, it is 11 wide
	cy = ((ball[WHITE_BALL].y*25)/0x400000)+120+5;
	a = (cue_angle-1024)&4095;
	dx = E_Cos(a);
	dy = E_Sin(a);
	ex = cx + (int)((dx*AIM_LINE_LEN)>>16);
	ey = cy + (int)((dy*AIM_LINE_LEN)>>16);
	minx = maxx = cx;
	miny = maxy = cy;
	for(i=0;i<6;i++)		// the pockets' centres bound the table
		{
		px = (pockets[i].x*25)/0x400000;
		py = (pockets[i].y*25)/0x400000;
		px += 160;
		py += 120;
		if(i == 0 || px < minx) minx = px;
		if(i == 0 || px > maxx) maxx = px;
		if(i == 0 || py < miny) miny = py;
		if(i == 0 || py > maxy) maxy = py;
		}
	t = 65536;				// the line's fraction that lies inside the box
	if(ex > maxx) t = ((long)(maxx-cx) << 16) / (ex-cx);
	if(ex < minx) t = ((long)(minx-cx) << 16) / (ex-cx);
	if((ey > maxy) && ((u = ((long)(maxy-cy) << 16) / (ey-cy)) < t)) t = u;
	if((ey < miny) && ((u = ((long)(miny-cy) << 16) / (ey-cy)) < t)) t = u;
	if(t < 0) t = 0;
	ex = cx + (int)(((long)(ex-cx)*t) >> 16);
	ey = cy + (int)(((long)(ey-cy)*t) >> 16);
	len = (int)((AIM_LINE_LEN*t) >> 16);			// the line's length after the cut, in pixels
	for(i = 0; i < AIM_DASHES; i++)
		{
		int	s0 = AIM_DASH_START + i*AIM_DASH_STEP, s1 = s0 + AIM_DASH_LEN;
		short	*xy;
		POLY_F4	*p;

		if(s0 >= len) break;
		if(s1 > len) s1 = len;
		p = &aim_dash[active_page][i];
		SetPolyF4(p);
		setRGB0(p, 255, 255, 255);
		xy = &p->x0;							// two pixels wide (the side vector is (-dy, dx)), so no dash is a single line of either field
		xy[0] = cx + (int)((dx*s0 - dy) >> 16);	xy[1] = cy + (int)((dy*s0 + dx) >> 16);
		xy[2] = cx + (int)((dx*s0 + dy) >> 16);	xy[3] = cy + (int)((dy*s0 - dx) >> 16);
		xy[4] = cx + (int)((dx*s1 - dy) >> 16);	xy[5] = cy + (int)((dy*s1 + dx) >> 16);
		xy[6] = cx + (int)((dx*s1 + dy) >> 16);	xy[7] = cy + (int)((dy*s1 - dx) >> 16);
		AddPrim( o_t++, p);
		}
	Aim_Stick_2D(cx, cy, dx, dy);
	aim_stick_on = 1;
	a = a + 8192 * (cx + 512 * cy);
	aim_line_draws = (a == aim_line_sig) ? aim_line_draws + 1 : 1;
	aim_line_sig = a;
	aim_line_on = 1;
}
void	DrawScreen2D( void )
{
#ifdef	PSX
	static	long	temppal = 0;
	static	long	stop_printing = NO;

	int	i,c,sx,sy;
	RECT	rect;

	get_page();
	bsp = cdl->sprballs;
	bdr = cdl->drmballs;
	setRECT(&rect,0,0,0,0);

	if(PadBounce & PAD_START)
		{
		temppal++;
		temppal &= 15;
		}

//	SetLineF2(&cdl->cueline);
//	cdl->cueline.x0 = (cue_ptr->modified_pos.x*100)+160;
//	cdl->cueline.y0 = (cue_ptr->modified_pos.y*100)+120;
//	cdl->cueline.x1 =	160;
//	cdl->cueline.y1 = 120;
//	cdl->cueline.r0 = 2;
//	cdl->cueline.g0 = 2;
//	cdl->cueline.b0 = 2;
//	AddPrim( o_t++, &cdl->cueline);

	if((PadBounce & PAD_TL2) && (PadDataIn & PAD_TR2))	// RESTORE: see above; L2 and R2 together (L2 alone is undo, PAD.C)
		{
		view_3d ^= YES;
		E_SetView3D(view_3d);
		}
	if(view_3d == NO)
	{
	for(i=0;i<LAST_BALL;i++)
		{
		if(ball[i].ball_colour == 0) c = 2;
		if(ball[i].ball_colour == 1) c = 0;
		if(ball[i].ball_colour == 3) c = 1;
		if(ball[i].ball_colour == 8) c = 3;
//		sx = (160*ball[i].x);
		sx = (ball[i].x*25)/0x400000;	// RESTORE: 100*x, x 8.24 (was a double)
		sx+=160;
//		sy = (160*ball[i].y);
		sy = (ball[i].y*25)/0x400000;
		sy+=120;
		setXY0(bsp, sx,sy);
		bsp->clut = CLUTaddr(320,511)+c;
		SetDrawMode(bdr,0,0,0x85,&rect);
		SetSprt(bsp);
		SetShadeTex(bsp,1);
		SetSemiTrans(bsp,0);
		setRGB0(bsp,255,255,255);
		setWH(bsp, 11, 11);
		if(ball[i].active == YES)
			{
			AddPrim( o_t++, bsp++);
			AddPrim( o_t++, bdr++);
			}
		}

	for(i=0;i<6;i++)
		{
		sx = (pockets[i].x*25)/0x400000;	// RESTORE: 100*x, 8.24
		sy = (pockets[i].y*25)/0x400000;
		if(sx<0) sx+=12;
		if(sx>0) sx-=12;
		if(sy<0) sy+=12;
		if(sy>0) sy-=12;
		sx+=160;
		sy+=120;
		setXY0(bsp, sx,sy);
		bsp->clut = CLUTaddr(320,511)+4;
		SetDrawMode(bdr,0,0,0x85,&rect);
		SetSprt(bsp);
		SetShadeTex(bsp,1);
		SetSemiTrans(bsp,0);
		setRGB0(bsp,255,255,255);
		setWH(bsp, 11, 11);
		AddPrim( o_t++, bsp++);
		AddPrim( o_t++, bdr++);
		}
	Aim_Line_2D();	// RESTORE: owner's decision (2026-11-20): the authors' cue line, below
	}
#else

	SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,0x0000,No_Window|No_Gouraud,CL256Bnk|ECdis|MESHoff,sprNoflip);
	FIXED			Pos[4];
	int	i;
 						
	
	Pos[0]	=	160*65536;							  
	Pos[1]	=	120*65536;							  
	Pos[2]	=	170*65536;							  
	Pos[3]	=	ORIGINAL*65536;

	for (i=1;i<16;i++)
		{
		Pos[0]=(64-(160*ball[i].x))*65536;
		Pos[1]=(160-(160*ball[i].y))*65536;
		slDispSprite(Pos,&Normal,0);
		}
#endif

	if(white_ball_potted_flag == YES)
		{
		stop_printing = YES;
		}
	if((white_ball_potted_flag == NO) && (stop_printing == YES))
		{
		temppal=YES;
		}
#ifdef	QDUP_DEBUG_TEXT	// RESTORE: owner's decision (2026-11-18, "We should remove the debug text at the top"): NMS's status lines are for tool builds only (QDUP_CFLAGS=-DQDUP_DEBUG_TEXT); the foul or AIM MODE line below stays, the PC shows it in its panel (SCRDRWS.C)
	FntPrint("Player %d, Cue Angle = %d\n",Player_Active,cue_angle);
	FntPrint("Balls Moving %3s, Frames = %4d\n",yesnostr[any_balls_moving()],game_frames);
	FntPrint("Yd = %4d, Stat = %s\n",mouse_ydiff,eventtxt[event_flag]);
#endif
	if(foul_occured>0) FntPrint("%s\n",foul_messages[foul_occured]);
	if(any_balls_moving() == NO) Print_Control_Hints();	// RESTORE: owner's decision (2026-11-18): control hints in play, after the foul or AIM MODE line (new/hints.c)

}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
#ifdef	PSX
void	Show_VRAM(void)
{

	word	slidex,slidey;

	DrawSync(0);
	VSync(0);

	GsInitGraph(640,480,2,0,0);		/* SET THE MODE */
	SetDispMask(1);				/* 1=ENABLE, 0=DISABLE */
	slidex=slidey=0;
	while (1)
	{
	  	GsDefDispBuff(slidex,slidey,slidex,slidey);

		get_pad();

		if(PadDataIn & PAD_TL1) if(slidex>3) slidex-=4;
		if(PadDataIn & PAD_TR1) if(slidex<380) slidex+=4;
		if(PadDataIn & PAD_TL2) if(slidey>3) slidey-=4;
		if(PadDataIn & PAD_TR2) if(slidey<32) slidey+=4;
		if(PadBounce & PAD_SEL) break;

//		pollhost();

		VSync(0);					/* WAIT FOR VBLANK */
		GsSwapDispBuff();			/* EXCHANGE DISPLAY AND DRAWING BUFFER */
		ResetGraph(1);				/* RESET GPU */

	}
}
#endif
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
ubyte *LoadTGA(text *name)
{

	Uint16	*dst1,*dst2;
	Uint16	*src1,*src2;
	Uint16	w,h;

	Uint16 r,g,b;
	Uint16 col;
	int	i,j;

	src1 = (Uint16*)LoadFile( name );
	w=src1[6];
	h=src1[7];
	src2=src1+9;

	dst1=(Uint16*)Get_Ram_Ptr(w*h*2);
	dst2=&dst1[((h-1)*w)];

	for (j=0;j<h;j++)
		{
		for (i=0;i<w;i++)
			{
			col=(*src2++);
			b=(col&0x001f)<<10;
			g=(col&0x03e0);
			r=(col&0x7c00)>>10;
			dst2[i]=0x8000+b+g+r;
			}
		dst2-=w;
		}
	return(dst1);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл


