#define	RUNLENGTH	1
#define	BITSTREAM	(1<<1)
#define	COL256		(1<<2)

#define	MAX_ICONS		5

enum
{
	C_BALLS,
	C_TABLE,
	C_PLAYERS,
	C_OPTIONS,
	C_START,
	C_SPEAKER,
	C_BAIZE,
	C_PAD,
	C_CARD,
	C_TROPHY
};


typedef	struct
{
	u_long	offset;
	u_short	width;
	u_short	height;
	Sint8		xoff;
	Sint8		yoff;
	u_short	flags;
} G_SPRITE;

long			xpoints;					// how many points along x of table
long			ypoints;					// how many points along y of table

u_short		PaletteBuffer[512];

ubyte			*BackScreen;			// Used for loading screens from disk
ubyte			*bufadd;					// used to find where to decompress icon

int			menu_frames[2][5];	// animation frames for icons

int			opt_level;				// icon menu level
int			opt_frame;				// icon selected this level
int			cursor;					// icon index for pad & window functions

u_long		*BufferPtr[ MAX_ICONS ][2];	// pointers to decompress buffers (icons)

short			svx[ MAX_ICONS ];		// abs icon x pos (vram)
short			svy[ MAX_ICONS ];		// abs icon y pos (vram)
short			spw[ MAX_ICONS ];		// abs icon width
short			sph[ MAX_ICONS ];		// abs icon height
short			spx[ MAX_ICONS ];		// abs icon x pos (screen)
short			spy[ MAX_ICONS ];		// abs icon y pos (screen)
short			spu[ MAX_ICONS ];		// icon u
short			spv[ MAX_ICONS ];		// icon v
short			tex[ MAX_ICONS ];		// icon texture page

#define		TABLEMODELS	4
#define		BALLCHOICES	3	// RESTORE: the balls icon's choices (9ball, triangle, triangle; not trick)
#define		BAIZECHOICES	3	// RESTORE: the baize icon's choices
#define		BALLMODELS	4
#define		PLAYMODELS	2

#define		MAXMODELS	(TABLEMODELS+BALLMODELS+PLAYMODELS+1)	// extra models to be added
#define		Speaker		(TABLEMODELS+BALLMODELS+PLAYMODELS)
long			TableType;
long			BaizeType;	// RESTORE: the baize icon's choice (0 blue, 1 green, 2 red); it shared TableType
long			PlayType;
long			BallType;

u_long		*ModelsPMD[ MAXMODELS ];

//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл

void	white_ball_gone_float(void);
