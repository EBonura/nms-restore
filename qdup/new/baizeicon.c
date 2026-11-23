/* RESTORE: NEW WORK (owner's decision, 2026-11-20: "when we change the colour of the table let's make the rotating
 * icon show the color selected, currently it stays blue").  Not NMS's code.
 *
 * The baize icon (second page of the option screen) turns the table's model (PSX.C DrawInsideBall: ModelsPMD[
 * TableType]).  The model's felt and cushions are flat and Gouraud triangles in blue.  Here the blue ones take the
 * colour the baize choice plays with: the felt entry (pen 1) of the Potters room palette the game loads for that
 * colour, the very file STV3D.C's init_3d reads (LEVELDAT.C room_palettes, POTTBLUE.PAL, POTTGREN.PAL, POTTRED.PAL),
 * so the icon cannot disagree with the table.  The files are read once, when the option screen opens.
 *
 * A vertex colour is the felt's when its blue is high and well above red and green (the models' blues are (87,94,237),
 * (56,65,234), (23,32,214) and (0,0,179)).  It keeps its lightness: the part of it that is white (the lesser of red and
 * green) stays, the rest of the blue is the palette's colour scaled by how much blue there was.  The model's
 * original colours are kept (the packets exist twice, one for each display page, and both are changed), put back
 * when the cursor leaves the baize icon, so the table icon is as it was.
 */
#include	"globals.h"

extern	long	BaizeType, TableType;
extern	u_long	*ModelsPMD[];
extern	flag	load_file_here( text *fname, ubyte *dest, long length );

#define	PALS_PER_COLOUR	11		// GAME.C: NUMBER_OF_LOCATIONS, set once a game starts; room_palettes holds this many a colour
#define	MAX_FELT		320		// the most blue colour words in one table model (UKTABLE: 298), one copy of the packets

// The 3D view's cue pens (STV3D.C cue_colour): tip 91, ferrule 204, shaft 197, butt 232, butt end 239 of the Potters room
// palette (the same in all three), for the flat view's cue stick (GFX.C Aim_Stick_2D); defaults until the file is read.
ubyte	cue_rgb[5][3] = { { 12, 12, 207 }, { 255, 255, 255 }, { 166, 166, 166 }, { 77, 48, 32 }, { 16, 8, 8 } };
static	const	ubyte	cue_pens[5] = { 91, 204, 197, 232, 239 };

static	ubyte	felt_rgb[BAIZECHOICES][3];	// the felt of blue, green, red (8 bits)
static	flag	felt_known = NO;

static	u_short	snap_at[MAX_FELT];		// the colour word's index in the model
static	u_long	snap_was[MAX_FELT];		// its original value
static	ubyte	snap_words[MAX_FELT];	// the packet's size in words: the other display page's copy is this far on
static	int		snap_n;
static	u_long	*snap_model = NULL;		// the model the snapshot is of, NULL when none (and nothing is changed)
static	int		shown = -1;				// the colour the model carries, -1 the original

void	Baize_Felt_Load( void )
{
	static	const text	*dir = "tables\\";
	text	file[128];
	ubyte	pal[768];
	int		i, k;

	for(i = 0; i < BAIZECHOICES; i++)
		{
		sprintf(file, "%s%s", dir, room_palettes[i * PALS_PER_COLOUR]);
		felt_rgb[i][0] = 0;
		felt_rgb[i][1] = 0;
		felt_rgb[i][2] = 63 << 2;
		if(load_file_here(file, pal, 768) == YES)
			{
			for(k = 0; k < 3; k++) felt_rgb[i][k] = (pal[3 + k] << 2) | (pal[3 + k] >> 4);	// pen 1, 6 bits a channel to 8
			if(i == 0)
				{
				int	c;
				for(c = 0; c < 5; c++)
					for(k = 0; k < 3; k++) cue_rgb[c][k] = (pal[3 * cue_pens[c] + k] << 2) | (pal[3 * cue_pens[c] + k] >> 4);
				}
			}
		}
	felt_known = YES;
}

static	flag	is_felt( u_long c )
{
	int	r = c & 255, g = (c >> 8) & 255, b = (c >> 16) & 255;

	return((b >= 170) && (b - ((r > g) ? r : g) >= 100));
}

// The words of every untextured packet's colours, per packet type (the low three bits of the group's type).
static	const	ubyte	colour_words[4][4] = { { 1, 0, 0, 0 }, { 1, 0, 0, 0 }, { 1, 3, 5, 0 }, { 1, 3, 5, 7 } };
static	const	ubyte	packet_words[4] = { 5, 6, 7, 9 };		// F3, F4, G3, G4

// Calls fn(model, word index, packet words) for each colour word of an untextured packet in the model.
static	void	walk_felt( u_long *model, void (*fn)(u_long *, int, int) )
{
	long	*w = (long *)model;
	long	*p = w + 4;
	int		i, j, k, nobj = w[3];

	for(i = 0; i < nobj; i++)
		{
		int	nptr = *p++;
		for(j = 0; j < nptr; j++)
			{
			long	*g = w + (*p++ >> 2);
			int		type = (*g >> 16) & 15, count = *g & 0xffff, ty = type & 7;
			int		words, nv, verts;

			if(ty < 4) continue;
			words = packet_words[ty - 4];
			nv = (ty & 1) ? 4 : 3;
			verts = (type & 8) ? nv : 2 * nv;
			g++;
			for(k = 0; k < count; k++)
				{
				int	c;
				for(c = 0; (c < 4) && ((c == 0) || colour_words[ty - 4][c]); c++)
					fn(model, (int)((u_long *)g - model) + colour_words[ty - 4][c], words);
				g += 2 * words + verts;
				}
			}
		}
}

static	void	snap_one( u_long *model, int at, int words )
{
	if((snap_n < MAX_FELT) && is_felt(model[at]))
		{
		snap_at[snap_n] = at;
		snap_was[snap_n] = model[at];
		snap_words[snap_n] = words;
		snap_n++;
		}
}

void	Baize_Felt_Restore( void )
{
	int	i;

	if(snap_model == NULL) return;
	for(i = 0; i < snap_n; i++)
		{
		snap_model[snap_at[i]] = snap_was[i];
		snap_model[snap_at[i] + snap_words[i]] = snap_was[i];
		}
	snap_model = NULL;
	shown = -1;
}

// Called by DrawInsideBall each frame the cursor is on the baize icon, before the model is drawn.
void	Baize_Felt_Show( u_long *model, int choice )
{
	int		i, w, c, r, g, b, lo;
	ubyte	*t;

	if(felt_known == NO) return;
	if(snap_model != model)
		{
		Baize_Felt_Restore();
		snap_n = 0;
		walk_felt(model, snap_one);
		snap_model = model;
		}
	if(shown == choice) return;
	t = felt_rgb[choice % BAIZECHOICES];
	for(i = 0; i < snap_n; i++)
		{
		w = snap_was[i];
		r = w & 255;
		g = (w >> 8) & 255;
		b = (w >> 16) & 255;
		lo = (r < g) ? r : g;
		c = (w & 0xff000000);			// the command byte, if this is the first colour of the packet
		r = lo + (t[0] * (b - lo)) / 252;
		g = lo + (t[1] * (b - lo)) / 252;
		b = lo + (t[2] * (b - lo)) / 252;
		if(r > 255) r = 255;
		if(g > 255) g = 255;
		if(b > 255) b = 255;
		snap_model[snap_at[i]] = c | (b << 16) | (g << 8) | r;
		snap_model[snap_at[i] + snap_words[i]] = snap_model[snap_at[i]];
		}
	shown = choice;
}
