#ifndef	OT_LENGTH
#define	OT_LENGTH	12						// Depth Of Ordering Table
#define	OT_SIZE		(1<<OT_LENGTH)		// Size Of Ordering Table
#endif

#define	SCR_Z	1024

#define	MODEL_X	80
#define	MODEL_Y	310

#define	ICON_SIZE		60*120

#ifndef	AMERICAN
  #define  FRAME_XL     320		// Lo-Res 320 pixels wide
  #define  FRAME_YL     256		// Lo-Res 256 high.
  #define SCREEN_XL     0			// Offset display by 0 in X
  #define SCREEN_YL     16			// Offset display by 16 in Y
  #define  FRAME_XH     640		// Hi-Res 640 pixels wide
  #define  FRAME_YH     480		// Hi-Res 480 high.
  #define SCREEN_XH     0			// Offset display by 0 in X
  #define SCREEN_YH     20			// Offset display by 20 in Y
  #define VIDEO_MODE		MODE_PAL
#else
  #define  FRAME_XL     320		// Lo-Res 320 wide display.
  #define  FRAME_YL     240		// Lo-Res 240 pixels.
  #define SCREEN_XL     0			// Offset X by 0.
  #define SCREEN_YL     12			// Offset Y by 12 pixels.
  #define  FRAME_XH     640		// Hi-Res 640 wide display.
  #define  FRAME_YH     480		// Hi-Res 480 pixels.
  #define SCREEN_XH     0			// Offset X by 0.
  #define SCREEN_YH     12			// Offset Y by 12 pixels.
  #define VIDEO_MODE		MODE_NTSC
#endif

#define	MAX_BALLS		28

enum
{
	LO_RES,
	MED_RES,
	HI_RES
};

	// Drawing Environment Structure
typedef struct
{		
	DRAWENV		draw;							// drawing environment
	DISPENV		disp;							// display environment
	LINE_F2		cueline;
	SPRT			sprballs[ MAX_BALLS ];	// prims for the balls
	DR_MODE		drmballs[ MAX_BALLS ];	// ball draw modes
	short			txtballs[ MAX_BALLS ];	// ball texture pages
	u_long		ot[ OT_SIZE ];				// ordering table
} SPRLIST;

	// Display Stuff
SPRLIST		Display[2];						// Double Buffered Video
SPRLIST		*cdl;								// Current Display List
SPRT			*bsp;								// Used For Drawing Sprites
DR_MODE		*bdr;								//		"			"
short			*btx;								//		"			"
u_long		active_page;					// Current Display Page
u_long		*o_t;								// Current Ordering Table
int			screen_width;
int			screen_height;

long			game_frames;

BLK_FILL		clear;

SPRT			*osp;
DR_MODE		*odr;

SPRT			tsp;
DR_MODE		tdm;

SPRT			rsp;
DR_MODE		rdm;
SPRT			nsp;
DR_MODE		ndm;

SPRT			rsp1;
DR_MODE		rdm1;
SPRT			nsp1;
DR_MODE		ndm1;

SPRT			opt_sprs[6];					// Sprites For Option Screen
DR_MODE		opt_mode[6];					// Draw Mode "		"		 "

SPRT			spr_sprs[ MAX_ICONS ];					// Sprites For Option Screen
DR_MODE		spr_mode[ MAX_ICONS ];					// Draw Mode "		"		 "

SPRT			ref_sprs[3];					// Sprites For Option Screen
DR_MODE		ref_mode[3];					// Draw Mode "		"		 "

SPRT			txt_sprs[512];
DR_MODE		txt_mode;
SPRT			*tspr;
int			textpos;

	// Vbl Stuff
u_long		Evbl;
u_long		vblcount;

	// Protos
void			VblCallback( void );
void			VblInit( void );
void			Vblank(void);

	// Memory Card Stuff
#define		CARD1	0x00
#define		CARD2	0x10
u_long		ev0;
u_long		ev1;
u_long		ev2;
u_long		ev3;
u_long		ev10;
u_long		ev11;
u_long		ev12;
u_long		ev13;
u_long		currentcard;
	// Pad Stuff
ControllerPacket	pad1;
ControllerPacket	pad2;
u_long		PadDataIn;
u_long		PadBounce;
extern	u_long	PadPressed;	// RESTORE: BUG FIX, PAD.C
u_long		lastpad;
	// 3D Stuff
u_long		*TablePMD;
SVECTOR		grotate;
ulong			distance;
VECTOR		camera;
MATRIX		trans;
int			scaler;

int			clipw;
int			cliph;
int			clipd;


flag			joy2_is_left;
flag			joy2_is_right;
flag			joy2_is_up;
flag			joy2_is_down;
