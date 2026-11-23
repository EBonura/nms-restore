/* RESTORE: NEW WORK (owner's decision, 2026-11-13): the choice between the
 * 2D and the 3D view when a game starts.  Not NMS's code.
 *
 * The owner: "I don't like that QDup has 2 separate 2D and 3D versions, it
 * would be nice if we could just load them into the same project, if the
 * game is identical and the only change is the play, let's add a small
 * choice screen when you start a new game so we can unify them".
 *
 * Choose_View is called by MENU.C's DecodePad when START is pressed on the
 * option screen.  It shows a small panel over the option screen (the same
 * picture, DrawOptionsScreen, which calls Choose_View_Draw just before it
 * shows the frame), in the option screen's own text: the font NMS's score
 * list uses (FONT.PSX, PrintText in PSX.C), drawn twice the size so that it
 * reads on a television.  Up and Down move between the two entries, Cross
 * (or Start) confirms and Circle goes back to the option screen, as the
 * menus do.  It returns OK to start the game, FAIL to stay on the option
 * screen.
 *
 * NMS's text tables (PSX.C FontPos, FontSize) have the letter W wrong: its
 * position is 0x58 where the picture has W at x 80 (0x50) and its width is 7
 * where the letter is 10 wide, so PrintText shows the right edge of W and a
 * piece of the L.  The panel's text uses its own copy of the entry for W.
 * The digits are magenta in the font's palette and the whole palette is
 * stored at half brightness (ConvertPalette keeps 4 bits a channel in a 5
 * bit field), so the text is drawn modulated by 255 (twice the texel) to
 * make the letters white and the digits bright.
 */
#include	"globals.h"

extern	short	FontPos[];
extern	short	FontSize[];
extern	long	view_3d;
extern	int		not_built_timer;
extern	int		opt_level, cursor;
extern	long	BallType, TableType, PlayType, BaizeType;

#define	SCALE		2
#define	MAX_GLYPHS	72		// RESTORE: 40 before the option screen's caption (below)

static	flag		choosing = NO;
static	flag		loading = NO;
// One set of primitives for each display page: the GPU may still be drawing
// the other page's frame while this one is built.
static	POLY_FT4	choice_glyph[2][MAX_GLYPHS];
static	int			choice_glyphs;
static	TILE		choice_panel[2], choice_bar[2];
static	TILE		caption_panel[2];
static	int			xscale = SCALE;	// the caption is drawn one wide, two high: the shape the 320 x 480 screen shows as square
static	u_long		*text_ot = NULL;	// RESTORE: the ordering table print_text adds to (NULL: the frame's, cdl->ot)
static	DR_MODE		restore_mode[2];
// RESTORE: owner's decision (2026-11-20): two chevrons around the value of a row that left and right change. The option
// font has no punctuation, so each is two thin slanted bars (POLY_F4), four to a page's caption: "<" and ">".
#define	CHEVRON_W	7
#define	MAX_CHEVRON_BARS	4
static	POLY_F4		chevron_bar[2][MAX_CHEVRON_BARS];
static	int			chevron_bars;
int				caption_chevrons;	// RESTORE: the chevron bars the caption was last drawn with (4 on a row that left and right change, else 0; the gates read it)

static	void	glyph_of(int c, int *u, int *v, int *w, int *h)
{
	if((c == 'W') || (c == 'w'))
		{
		*u = 0x50;				// RESTORE: NMS's table says 0x58
		*v = 0x11;
		*w = 10;				// RESTORE: NMS's table says 7
		*h = 0x11;
		return;
		}
	*u = FontPos[c] & 0xff;
	*v = FontPos[c] >> 8;
	*w = FontSize[c] & 0xff;
	*h = FontSize[c] >> 8;
}

static	int	text_width(text *s)
{
	int	c, u, v, w, h, x = 0;

	while((c = *s++) != 0)
		{
		if(c == ' ') w = 6;
		else if((c == '<') || (c == '>')) w = CHEVRON_W;
		else glyph_of(c, &u, &v, &w, &h);
		x += (w + 1) * xscale;
		}
	return(x - xscale);
}

// A chevron at x, the glyph's top at y (a glyph is 2 * 17 high here): pointing left when left is YES. Two bars meet at
// the tip; rel gives each corner's distance from the tip, mirrored for the right-pointing one.
static	void	add_chevron(int x, int y, flag left)
{
	static	const short	rel[2][4][2] = {	// x from the tip, y from the glyph's top: the upper bar, then the lower
		{ { CHEVRON_W - 3, 6 }, { CHEVRON_W, 6 }, { 0, 17 }, { 3, 17 } },
		{ { 0, 17 }, { 3, 17 }, { CHEVRON_W - 3, 28 }, { CHEVRON_W, 28 } } };
	int			k, c, px[4], py[4];
	POLY_F4		*p;

	for(k = 0; k < 2; k++)
		{
		if(chevron_bars >= MAX_CHEVRON_BARS) return;
		for(c = 0; c < 4; c++)
			{
			px[c] = x + (left ? rel[k][c][0] : CHEVRON_W - rel[k][c][0]);
			py[c] = y + rel[k][c][1];
			}
		p = &chevron_bar[active_page][chevron_bars++];
		SetPolyF4(p);
		setRGB0(p, 255, 255, 255);
		p->x0 = px[0];	p->y0 = py[0];
		p->x1 = px[1];	p->y1 = py[1];
		p->x2 = px[2];	p->y2 = py[2];
		p->x3 = px[3];	p->y3 = py[3];
		AddPrim((text_ot != NULL) ? text_ot : cdl->ot, p);
		}
}

// Draws s centred on x, its top at y, into ordering table slot 0.
static	void	print_text(text *s, int cx, int y)
{
	int	c, u, v, w, h;
	int	x = cx - text_width(s) / 2;
	POLY_FT4	*p;

	while((c = *s++) != 0)
		{
		if(c == ' ') w = 6;
		else if((c == '<') || (c == '>'))
			{
			w = CHEVRON_W;
			add_chevron(x, y, (c == '<') ? YES : NO);
			}
		else
			{
			glyph_of(c, &u, &v, &w, &h);
			if(choice_glyphs < MAX_GLYPHS)
				{
				p = &choice_glyph[active_page][choice_glyphs++];
				SetPolyFT4(p);
				SetShadeTex(p, NO);		// modulated: 255 is twice the texel
				setRGB0(p, 255, 255, 255);
				p->x0 = x;				p->y0 = y;
				p->x1 = x + w*xscale;	p->y1 = y;
				p->x2 = x;				p->y2 = y + h*SCALE;
				p->x3 = x + w*xscale;	p->y3 = y + h*SCALE;
				p->u0 = u;		p->v0 = v;
				p->u1 = u + w;	p->v1 = v;
				p->u2 = u;		p->v2 = v + h;
				p->u3 = u + w;	p->v3 = v + h;
				p->clut = CLUTaddr(0, 484);
				p->tpage = getTPage(1, 0, 512, 256);
				AddPrim((text_ot != NULL) ? text_ot : cdl->ot, p);
				}
			}
		x += (w + 1) * xscale;
		}
}

/* RESTORE: owner's decision (2026-11-18: "we should indicate that the first 3 option in the main menu should be
 * selected with left/right", and "I was able to get the table green but I don't know how I did it"). A caption at
 * the bottom left of the option screen for the icon under the cursor: its name (NMS's IconNames, COMMON.C) and value,
 * then the button that does something there (MENU.C DecodePad), every icon of both rows. Values: the balls icon's
 * three racks start 9-ball, UK pool and US 8-ball (POOL.C ball_game_type); the table icon's models are NMS's
 * uktable, ustable, coffin and square (COMMON.C ModelNames; the coffin plays the Potters table outside a test
 * build, POOL.C); the players icon's head is two players and the computer model a game against the computer
 * (POOL.C); the baize is blue, green or red (POOL.C TABLE_TYPE). The option font (FONT.PSX) has letters and digits
 * only, so no punctuation. Drawn one wide and two high: at most 26 characters, 8 pixels each, centred on x 124,
 * inside a panel from x 4 to 244 that keeps clear of the START icon. */
static	text	*caption_value( void )
{
	static	text	*balls[] = { "9 BALL", "UK POOL", "US 8 BALL" };
	static	text	*tables[] = { "UK TABLE", "US TABLE", "COFFIN", "SQUARE" };
	static	text	*baize[] = { "BLUE", "GREEN", "RED" };
	switch(cursor)
		{
		case	C_BALLS:	return(balls[BallType % 3]);
		case	C_TABLE:	return(tables[TableType % 4]);
		case	C_PLAYERS:	return(PlayType ? "COMPUTER" : "2 PLAYERS");
		case	C_BAIZE:	return(baize[BaizeType % 3]);
		default:			return("");
		}
}

int		caption_cursor = -1;	// RESTORE: the icon the caption was last drawn for (the gates read it)
int		caption_draws = 0;		// RESTORE: how many times in a row the caption has been drawn for the same cursor and values (the gates wait on it)
static	int	caption_sig = -1;
static	void	Draw_Caption( int pg )
{
	static	text	*names[] = { "BALLS", "TABLE", "PLAYERS", "OPTIONS", "START", "SPEAKER", "BAIZE", "PAD", "CARD", "TROPHY" };
	static	text	line[32];
	text	*name, *value, *act;
	int		i, n, lines, y;
	flag	adjustable = ((cursor == C_BALLS) || (cursor == C_TABLE) || (cursor == C_PLAYERS) || (cursor == C_BAIZE));

	caption_cursor = cursor;
	i = cursor + 16 * (BallType + 4 * (TableType + 8 * (PlayType + 4 * BaizeType)));
	caption_draws = (i == caption_sig) ? caption_draws + 1 : 1;
	caption_sig = i;
	chevron_bars = 0;
	name = names[cursor % 10];
	value = caption_value();
	switch(cursor)
		{
		case	C_BALLS:
		case	C_TABLE:
		case	C_PLAYERS:
		case	C_BAIZE:	act = "LEFT RIGHT CHANGES IT"; break;
		case	C_OPTIONS:	act = "CROSS OPENS MORE"; break;
		case	C_START:	act = "CROSS STARTS THE GAME"; break;
		default:			act = "NOT AVAILABLE"; break;
		}
	n = 0;
	for(i = 0; name[i] != 0; i++) line[n++] = name[i];
	if(value[0] != 0)
		{
		line[n++] = ' ';
		line[n++] = ' ';
		if(adjustable) line[n++] = '<';		// RESTORE: the chevrons (above) on the rows left and right change
		if(adjustable) line[n++] = ' ';
		for(i = 0; value[i] != 0; i++) line[n++] = value[i];
		if(adjustable) line[n++] = ' ';
		if(adjustable) line[n++] = '>';
		}
	line[n] = 0;
	lines = 2;					// RESTORE: owner's decision (2026-11-20): no "CIRCLE GOES BACK" line (Circle still goes back, MENU.C)
	xscale = 1;
	y = 476 - lines * 34;			// the last line ends by line 476 of the 480
	print_text(act, 124, y + 34);
	print_text(line, 124, y);
	xscale = SCALE;
	SetTile(&caption_panel[pg]);
	setXY0(&caption_panel[pg], 4, y - 4);
	setWH(&caption_panel[pg], 240, lines * 34 + 6);
	setRGB0(&caption_panel[pg], 0, 0, 40);
	AddPrim(cdl->ot, &caption_panel[pg]);
	caption_chevrons = chevron_bars;
}

// Called by DrawOptionsScreen just before it shows the frame.  The ordering
// table is drawn from its highest slot down and a slot from its head, so
// everything goes into slot 0 in the reverse of the order it is drawn in.
void	Choose_View_Draw( void )
{
	RECT	rect;
	int		pg, y;

	choice_glyphs = 0;
	pg = active_page;
	setRECT(&rect, 0, 0, 0, 0);
	SetDrawMode(&restore_mode[pg], ON, 0, tex[0], &rect);	// the first icon's sprite is drawn after slot 0's head
	AddPrim(cdl->ot, &restore_mode[pg]);
	if((choosing == NO) && (loading == NO)) Draw_Caption(pg);	// RESTORE: the caption, above
	if((choosing == NO) && (loading == NO) && (not_built_timer <= 0)) return;
	if(choosing == NO)	// a message: the dead icons' (not_built_timer), or PLEASE WAIT while the game loads
		{
		// PLEASE WAIT stays on the screen while the game loads, and the game then
		// sets the 256-line mode: only the top 256 lines of this frame are shown.
		// So it goes higher up than the other message.
		y = (loading == YES) ? 100 : 200;
		print_text((loading == YES) ? "PLEASE WAIT" : "NOT AVAILABLE", 160, y + 24);
		SetTile(&choice_panel[pg]);
		setXY0(&choice_panel[pg], 40, y);
		setWH(&choice_panel[pg], 240, 80);
		setRGB0(&choice_panel[pg], 0, 0, 40);
		AddPrim(cdl->ot, &choice_panel[pg]);
		return;
		}
	print_text("PLAY IN 2D OR 3D", 160, 176);
	print_text("3D TABLE", 160, 236);
	print_text("2D FLAT", 160, 296);
	SetTile(&choice_bar[pg]);
	setXY0(&choice_bar[pg], 52, (view_3d == YES) ? 226 : 286);
	setWH(&choice_bar[pg], 216, 52);
	setRGB0(&choice_bar[pg], 60, 60, 160);
	AddPrim(cdl->ot, &choice_bar[pg]);
	SetTile(&choice_panel[pg]);
	setXY0(&choice_panel[pg], 30, 150);
	setWH(&choice_panel[pg], 260, 200);
	setRGB0(&choice_panel[pg], 0, 0, 40);
	AddPrim(cdl->ot, &choice_panel[pg]);
}

// PLEASE WAIT over the option screen while the game loads: one frame on each
// display page, then the screen stays as it is.
void	Choose_View_Loading( void )
{
	loading = YES;
	DrawOptionsScreen();
	DrawOptionsScreen();
	loading = NO;
}

flag	Choose_View( void )
{
	flag	retval = FAIL;
	int		redraw = 2;		// frames still to draw: one for each display page

	choosing = YES;
	for(;;)
		{
		get_pad();
		if(PadBounce & (PAD_LU | PAD_LD))
			{
			view_3d = (view_3d == YES) ? NO : YES;
			redraw = 2;
			}
		if(PadBounce & (PAD_RD | PAD_START))
			{
			retval = OK;
			break;
			}
		if(PadBounce & PAD_RR) break;
		// NMS's display_screen shows the page it is still drawing, and the option
		// screen takes longer than a frame to draw, so a frame redrawn every vblank
		// shows its last items (this panel) missing now and then.  The picture does
		// not move here, so it is drawn once on each page and left alone.
		if(redraw > 0)
			{
			DrawOptionsScreen();
			redraw--;
			}
		else VSync(0);
		}
	choosing = NO;
	E_SetView3D(view_3d);
	return(retval);
}

/* RESTORE: owner's decision (2026-11-18: "the title screen should at least have a \"press start\" to indicate we can
 * move forward to the next screen"). MENU.C waits for Start or Cross while the title picture (PSX.C draw_title_screen,
 * GFX/FRONT.TGA loaded at 0,0, 320 x 480) is up; the option screen's frames are not shown then. So the prompt is drawn
 * straight onto the picture: PRESS START in the option font (FONT.PSX, loaded by LoadAllWrapperGfx before the wait),
 * twice the size, on a dark strip near the bottom, with its own drawing area on both fields (dfe on). */
int		title_prompts = 0;	// RESTORE: PRESS START prompts drawn (the gates read it)
int		title_loading = 0;	// RESTORE: LOADING captions drawn (the gates read it)
static	u_long	title_ot[1];
static	TILE	title_panel;
static	DR_MODE	title_mode;
static	void	title_caption( text *s )
{
	DRAWENV	env;
	RECT	rect;

	SetDefDrawEnv(&env, 0, 0, 320, 480);
	env.dfe = 1;
	PutDrawEnv(&env);
	ClearOTagR(title_ot, 1);
	choice_glyphs = 0;
	text_ot = title_ot;
	print_text(s, 160, 420);
	text_ot = NULL;
	SetTile(&title_panel);
	setXY0(&title_panel, 60, 412);
	setWH(&title_panel, 200, 50);
	setRGB0(&title_panel, 0, 0, 40);
	AddPrim(title_ot, &title_panel);
	setRECT(&rect, 0, 0, 0, 0);
	SetDrawMode(&title_mode, ON, 0, tex[0], &rect);
	AddPrim(title_ot, &title_mode);
	DrawOTag(title_ot);
	DrawSync(0);
}

void	Title_Press_Start( void )
{
	title_caption("PRESS START");
	title_prompts++;
}

/* RESTORE: the title waits while the option screen's 24 files load (LoadAllWrapperGfx), several seconds on a console, with
 * nothing to say so. LOADING goes where PRESS START will be, in the same panel, as soon as the option font is in VRAM (the
 * fourth of those files); PRESS START is drawn over it when they are all in. */
void	Title_Loading( void )
{
	title_caption("LOADING");
	title_loading++;
}
