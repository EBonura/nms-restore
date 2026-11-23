#include	"globals.h"
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
G_SPRITE spr_opts[] =
{
 512,   32, 68, -16, -68, 0+RUNLENGTH+COL256,	// Balls
 1788,  48,103, -25, -88, 0+RUNLENGTH+COL256,
 4476,  48, 96, -25, -88, 0+RUNLENGTH+COL256,
 7036,  56, 93, -24, -88, 0+RUNLENGTH+COL256,
 9512,  48,100, -22, -88, 0+RUNLENGTH+COL256,
 12156, 48,107, -19, -88, 0+RUNLENGTH+COL256,
 14756, 40,112, -15, -87, 0+RUNLENGTH+COL256,
 17088, 32,117, -11, -87, 0+RUNLENGTH+COL256,
 19276, 24,119,  -6, -87, 0+RUNLENGTH+COL256,
 20712, 16,119,  -9, -87, 0+RUNLENGTH+COL256,
 21776, 32,118, -16, -87, 0+RUNLENGTH+COL256,
 23712, 40,115, -22, -87, 0+RUNLENGTH+COL256,
 26196, 48,110, -25, -88, 0+RUNLENGTH+COL256,
 28712, 48,103, -25, -88, 0+RUNLENGTH+COL256,
 31408, 48, 96, -25, -88, 0+RUNLENGTH+COL256,
 33980, 56, 93, -24, -88, 0+RUNLENGTH+COL256,
 36460, 48,100, -22, -88, 0+RUNLENGTH+COL256,
 39120, 48,107, -19, -88, 0+RUNLENGTH+COL256,
 41724, 40,112, -15, -87, 0+RUNLENGTH+COL256,
 44056, 32,117, -11, -87, 0+RUNLENGTH+COL256,
 46244, 24,119,  -6, -87, 0+RUNLENGTH+COL256,
 47680, 16,119,  -9, -87, 0+RUNLENGTH+COL256,
 48744, 32,118, -16, -87, 0+RUNLENGTH+COL256,
 50680, 40,115, -22, -87, 0+RUNLENGTH+COL256,
 53164, 48,110, -25, -88, 0+RUNLENGTH+COL256,
 512,   40, 67, -20, -67, 0+RUNLENGTH+COL256,	// Table
 1604,  56,102, -30, -85, 0+RUNLENGTH+COL256,
 3948,  48,102, -24, -85, 0+RUNLENGTH+COL256,
 6260,  40, 95, -21, -83, 0+RUNLENGTH+COL256,
 8520,  40, 98, -29, -84, 0+RUNLENGTH+COL256,
 10804, 56,103, -34, -85, 0+RUNLENGTH+COL256,
 13136, 56,101, -34, -85, 0+RUNLENGTH+COL256,
 15460, 60, 93, -34, -82, 0+RUNLENGTH+COL256,
 17704, 60, 81, -34, -79, 0+RUNLENGTH+COL256,
 19884, 60, 64, -34, -73, 0+RUNLENGTH+COL256,
 21860, 60, 70, -34, -75, 0+RUNLENGTH+COL256,
 23864, 60, 85, -34, -80, 0+RUNLENGTH+COL256,
 26064, 60, 96, -34, -83, 0+RUNLENGTH+COL256,
 28348, 56,102, -30, -85, 0+RUNLENGTH+COL256,
 30688, 48,102, -24, -85, 0+RUNLENGTH+COL256,
 32992, 40, 95, -21, -83, 0+RUNLENGTH+COL256,
 35260, 40, 98, -29, -84, 0+RUNLENGTH+COL256,
 37532, 56,103, -34, -85, 0+RUNLENGTH+COL256,
 39856, 56,101, -34, -85, 0+RUNLENGTH+COL256,
 42180, 60, 93, -34, -82, 0+RUNLENGTH+COL256,
 44412, 60, 81, -34, -79, 0+RUNLENGTH+COL256,
 46592, 60, 64, -34, -73, 0+RUNLENGTH+COL256,
 48564, 60, 70, -34, -75, 0+RUNLENGTH+COL256,
 50576, 60, 85, -34, -80, 0+RUNLENGTH+COL256,
 52784, 60, 96, -34, -83, 0+RUNLENGTH+COL256,
 512,   16,80,   -8, -80, 0+BITSTREAM+COL256,	// Player
 1288,  24,119, -12, -99, 0+BITSTREAM+COL256,
 3036,  24,117, -11, -99, 0+BITSTREAM+COL256,
 4800,  24,116, -10, -99, 0+RUNLENGTH+COL256,
 6592,  24,117, -10, -99, 0+BITSTREAM+COL256,
 8416,  24,117,  -9, -99, 0+RUNLENGTH+COL256,
 10184, 16,117,  -8, -99, 0+BITSTREAM+COL256,
 11752, 16,117,  -7, -99, 0+BITSTREAM+COL256,
 13200, 16,117,  -6, -99, 0+BITSTREAM+COL256,
 14476, 16,116,  -6, -99, 0+BITSTREAM+COL256,
 15616, 16,116,  -7, -99, 0+BITSTREAM+COL256,
 16832, 16,117,  -9, -99, 0+BITSTREAM+COL256,
 18180, 24,117, -10, -99, 0+RUNLENGTH+COL256,
 19820, 24,116, -10, -99, 0+BITSTREAM+COL256,
 21556, 24,116, -11, -99, 0+BITSTREAM+COL256,
 23340, 24,116, -11, -99, 0+BITSTREAM+COL256,
 25140, 24,118, -10, -99, 0+BITSTREAM+COL256,
 26960, 24,119, -10, -99, 0+RUNLENGTH+COL256,
 28744, 16,119, -10, -99, 0+BITSTREAM+COL256,
 30360, 16,119,  -9, -99, 0+BITSTREAM+COL256,
 31840, 16,119,  -8, -99, 0+BITSTREAM+COL256,
 33156, 16,119,  -8, -99, 0+BITSTREAM+COL256,
 34348, 16,119,  -9, -99, 0+BITSTREAM+COL256,
 35596, 16,119, -10, -99, 0+BITSTREAM+COL256,
 37036, 24,119, -11, -99, 0+RUNLENGTH+COL256,
  512,  40, 78, -20, -78, 0+RUNLENGTH+COL256,	// Prefs
 2332,  60,118, -31, -98, 0+RUNLENGTH+COL256,
 5832,  60,116, -31, -97, 0+RUNLENGTH+COL256,
 9412,  60,115, -31, -97, 0+RUNLENGTH+COL256,
 13220, 56,114, -28, -97, 0+RUNLENGTH+COL256,
 17004, 56,113, -27, -97, 0+RUNLENGTH+COL256,
 20684, 56,113, -27, -97, 0+RUNLENGTH+COL256,
 24100, 56,112, -27, -96, 0+RUNLENGTH+COL256,
 27140, 56,112, -29, -96, 0+RUNLENGTH+COL256,
 30364, 56,113, -30, -97, 0+RUNLENGTH+COL256,
 33920, 56,114, -30, -97, 0+RUNLENGTH+COL256,
 37388, 56,115, -29, -97, 0+RUNLENGTH+COL256,
 41060, 56,116, -29, -97, 0+RUNLENGTH+COL256,
 44524, 60,117, -31, -97, 0+RUNLENGTH+COL256,
 47836, 60,119, -31, -98, 0+RUNLENGTH+COL256,
 51240, 56,119, -29, -98, 0+RUNLENGTH+COL256,
 54896, 56,120, -27, -99, 0+RUNLENGTH+COL256,
 58868, 56,120, -27, -99, 0+RUNLENGTH+COL256,
 62540, 56,119, -27, -98, 0+RUNLENGTH+COL256,
 65964, 56,119, -27, -98, 0+RUNLENGTH+COL256,
 69168, 56,119, -29, -98, 0+RUNLENGTH+COL256,
 72660, 56,120, -30, -99, 0+RUNLENGTH+COL256,
 76588, 56,120, -30, -99, 0+RUNLENGTH+COL256,
 80512, 56,119, -30, -98, 0+RUNLENGTH+COL256,
 84368, 56,119, -29, -98, 0+RUNLENGTH+COL256,
 512,   40, 52, -20, -52, 0+BITSTREAM+COL256,	// Start
 1948,  56, 78, -30, -65, 0+RUNLENGTH+COL256,
 4928,  56, 71, -30, -65, 0+BITSTREAM+COL256,
 7948,  60, 68, -31, -65, 0+BITSTREAM+COL256,
 10940, 56, 73, -27, -65, 0+RUNLENGTH+COL256,
 13932, 56, 80, -27, -66, 0+RUNLENGTH+COL256,
 16920, 48, 87, -19, -66, 0+RUNLENGTH+COL256,
 19604, 40, 93, -12, -67, 0+RUNLENGTH+COL256,
 21724, 24, 95,  -7, -67, 0+RUNLENGTH+COL256,
 23144, 16, 95,  -8, -67, 0+RUNLENGTH+COL256,
 24000, 32, 94, -19, -67, 0+RUNLENGTH+COL256,
 25780, 40, 90, -26, -66, 0+RUNLENGTH+COL256,
 28212, 48, 84, -31, -66, 0+RUNLENGTH+COL256,
 30968, 56, 77, -31, -66, 0+RUNLENGTH+COL256,
 33940, 56, 70, -31, -65, 0+RUNLENGTH+COL256,
 36932, 60, 68, -31, -65, 0+BITSTREAM+COL256,
 39940, 56, 74, -27, -65, 0+RUNLENGTH+COL256,
 42980, 56, 81, -27, -65, 0+RUNLENGTH+COL256,
 45996, 48, 88, -19, -65, 0+RUNLENGTH+COL256,
 48736, 32, 93, -12, -65, 0+RUNLENGTH+COL256,
 50824, 24, 96,  -7, -65, 0+RUNLENGTH+COL256,
 52320, 16, 96,  -8, -65, 0+RUNLENGTH+COL256,
 53212, 32, 95, -18, -65, 0+RUNLENGTH+COL256,
 55148, 40, 91, -25, -65, 0+RUNLENGTH+COL256,
 57648, 48, 85, -29, -65, 0+RUNLENGTH+COL256,
 512,   40, 80, -20, -80, 0+RUNLENGTH+COL256,	// Speaker
 2232,  56,119, -29, -99, 0+RUNLENGTH+COL256,
 5728,  56,117, -32, -99, 0+RUNLENGTH+COL256,
 9412,  56,116, -33,-100, 0+RUNLENGTH+COL256,
 13128, 56,113, -32,-100, 0+RUNLENGTH+COL256,
 17048, 56,118, -29,-100, 0+RUNLENGTH+COL256,
 20896, 48,120, -25,-100, 0+RUNLENGTH+COL256,
 24572, 40,119, -21, -99, 0+RUNLENGTH+COL256,
 27844, 40,115, -20, -99, 0+RUNLENGTH+COL256,
 29912, 40,116, -24, -99, 0+RUNLENGTH+COL256,
 32336, 48,120, -27,-100, 0+RUNLENGTH+COL256,
 36080, 48,119, -28,-100, 0+RUNLENGTH+COL256,
 39564, 56,115, -29,-100, 0+RUNLENGTH+COL256,
 42696, 56,113, -29,-100, 0+RUNLENGTH+COL256,
 45576, 56,115, -29, -99, 0+RUNLENGTH+COL256,
 48260, 56,117, -29, -99, 0+RUNLENGTH+COL256,
 51088, 56,118, -29, -99, 0+RUNLENGTH+COL256,
 54288, 56,117, -29, -98, 0+RUNLENGTH+COL256,
 57400, 48,115, -22, -98, 0+RUNLENGTH+COL256,
 60196, 48,112, -21, -98, 0+RUNLENGTH+COL256,
 62528, 40,108, -21, -97, 0+RUNLENGTH+COL256,
 64176, 40,109, -28, -97, 0+RUNLENGTH+COL256,
 66048, 48,113, -32, -97, 0+RUNLENGTH+COL256,
 68968, 56,117, -33, -98, 0+RUNLENGTH+COL256,
 72336, 56,118, -32, -98, 0+RUNLENGTH+COL256,
 512,   40, 80, -20, -80, 0+RUNLENGTH+COL256,	// Baize
 2204,  60,119, -30, -99, 0+RUNLENGTH+COL256,
 5676,  60,108, -30, -97, 0+RUNLENGTH+COL256,
 9208,  60, 94, -30, -94, 0+RUNLENGTH+COL256,
 12544, 60, 98, -30, -95, 0+RUNLENGTH+COL256,
 15948, 60,112, -30, -98, 0+RUNLENGTH+COL256,
 19500, 60,119, -30, -99, 0+RUNLENGTH+COL256,
 23040, 60,120, -30,-100, 0+RUNLENGTH+COL256,
 26576, 56,120, -26,-100, 0+RUNLENGTH+COL256,
 30048, 48,120, -24,-100, 0+RUNLENGTH+COL256,
 33512, 48,120, -27,-100, 0+RUNLENGTH+COL256,
 36912, 56,120, -30,-100, 0+RUNLENGTH+COL256,
 40420, 56,120, -30,-100, 0+RUNLENGTH+COL256,
 43932, 60,119, -30, -99, 0+RUNLENGTH+COL256,
 47464, 60,108, -30, -97, 0+RUNLENGTH+COL256,
 50996, 60, 94, -30, -94, 0+RUNLENGTH+COL256,
 54372, 60, 98, -30, -95, 0+RUNLENGTH+COL256,
 57792, 60,112, -30, -98, 0+RUNLENGTH+COL256,
 61344, 60,119, -30, -99, 0+RUNLENGTH+COL256,
 64900, 60,120, -30,-100, 0+RUNLENGTH+COL256,
 68432, 56,120, -26,-100, 0+RUNLENGTH+COL256,
 71912, 48,120, -24,-100, 0+RUNLENGTH+COL256,
 75368, 48,120, -27,-100, 0+RUNLENGTH+COL256,
 78780, 56,120, -30,-100, 0+RUNLENGTH+COL256,
 82240, 56,120, -30,-100, 0+RUNLENGTH+COL256,
  512,  40, 58, -20, -58, 0+RUNLENGTH+COL256,	// Pad
 1584,  56, 87, -29, -71, 0+RUNLENGTH+COL256,
 3788,  56, 80, -29, -70, 0+RUNLENGTH+COL256,
 5948,  56, 69, -29, -68, 0+RUNLENGTH+COL256,
 8068,  56, 69, -31, -68, 0+RUNLENGTH+COL256,
 10176, 56, 78, -30, -69, 0+RUNLENGTH+COL256,
 12336, 56, 86, -29, -71, 0+RUNLENGTH+COL256,
 14532, 56, 90, -29, -71, 0+RUNLENGTH+COL256,
 16744, 56, 90, -29, -71, 0+RUNLENGTH+COL256,
 18980, 48, 86, -21, -71, 0+RUNLENGTH+COL256,
 21136, 48, 77, -25, -70, 0+RUNLENGTH+COL256,
 23160, 48, 71, -29, -69, 0+RUNLENGTH+COL256,
 25096, 56, 71, -31, -67, 0+RUNLENGTH+COL256,
 27012, 56, 67, -30, -64, 0+RUNLENGTH+COL256,
 28964, 56, 60, -29, -62, 0+RUNLENGTH+COL256,
 30776, 56, 50, -29, -58, 0+RUNLENGTH+COL256,
 32428, 56, 49, -29, -58, 0+RUNLENGTH+COL256,
 34056, 56, 59, -30, -61, 0+RUNLENGTH+COL256,
 35828, 56, 67, -30, -64, 0+RUNLENGTH+COL256,
 37756, 56, 71, -29, -67, 0+RUNLENGTH+COL256,
 39716, 56, 71, -29, -69, 0+RUNLENGTH+COL256,
 41652, 48, 76, -23, -70, 0+RUNLENGTH+COL256,
 43612, 48, 85, -28, -71, 0+RUNLENGTH+COL256,
 45772, 48, 90, -30, -71, 0+RUNLENGTH+COL256,
 48004, 56, 90, -30, -71, 0+RUNLENGTH+COL256,
  512,  32, 70, -16, -70, 0+RUNLENGTH+COL256,	// Card
 2100,  48,106, -25, -89, 0+RUNLENGTH+COL256,
 4952,  48,100, -25, -88, 0+RUNLENGTH+COL256,
 7348,  48, 97, -25, -88, 0+RUNLENGTH+COL256,
 9136,  48,103, -22, -89, 0+RUNLENGTH+COL256,
 11608, 48,108, -19, -89, 0+RUNLENGTH+COL256,
 14408, 40,113, -16, -90, 0+RUNLENGTH+COL256,
 17336, 32,117, -12, -90, 0+RUNLENGTH+COL256,
 20020, 24,118,  -7, -90, 0+RUNLENGTH+COL256,
 21908, 16,118,  -9, -90, 0+RUNLENGTH+COL256,
 23348, 24,118, -16, -90, 0+RUNLENGTH+COL256,
 25640, 40,115, -21, -90, 0+RUNLENGTH+COL256,
 28556, 40,111, -24, -90, 0+RUNLENGTH+COL256,
 31572, 48,106, -25, -89, 0+RUNLENGTH+COL256,
 34720, 48,100, -25, -88, 0+RUNLENGTH+COL256,
 37732, 48, 97, -24, -88, 0+RUNLENGTH+COL256,
 40200, 48,103, -22, -89, 0+RUNLENGTH+COL256,
 43216, 48,107, -19, -89, 0+RUNLENGTH+COL256,
 46280, 40,113, -15, -90, 0+RUNLENGTH+COL256,
 49240, 32,116, -11, -90, 0+RUNLENGTH+COL256,
 51908, 24,118,  -7, -90, 0+RUNLENGTH+COL256,
 53820, 16,118,  -9, -90, 0+RUNLENGTH+COL256,
 55232, 24,118, -16, -90, 0+RUNLENGTH+COL256,
 57480, 40,115, -21, -90, 0+RUNLENGTH+COL256,
 60252, 40,111, -24, -90, 0+RUNLENGTH+COL256,
 512,   40, 76, -20, -76, 0+RUNLENGTH+COL256,	// Trophy
 2308,  56,114, -30, -95, 0+RUNLENGTH+COL256,
 6020,  48,108, -28, -95, 0+RUNLENGTH+COL256,
 9724,  56,110, -30, -95, 0+RUNLENGTH+COL256,
 13504, 56,115, -30, -95, 0+RUNLENGTH+COL256,
 17240, 48,119, -28, -95, 0+RUNLENGTH+COL256,
 20920, 48,119, -26, -95, 0+RUNLENGTH+COL256,
 24468, 48,116, -23, -95, 0+RUNLENGTH+COL256,
 28032, 48,110, -24, -95, 0+RUNLENGTH+COL256,
 31512, 48,112, -28, -95, 0+RUNLENGTH+COL256,
 35012, 48,117, -29, -95, 0+RUNLENGTH+COL256,
 38628, 48,119, -29, -95, 0+RUNLENGTH+COL256,
 42204, 48,118, -27, -95, 0+RUNLENGTH+COL256,
 45868, 56,114, -30, -95, 0+RUNLENGTH+COL256,
 49568, 48,108, -28, -95, 0+RUNLENGTH+COL256,
 53292, 48,110, -28, -95, 0+RUNLENGTH+COL256,
 57068, 56,116, -30, -95, 0+RUNLENGTH+COL256,
 60824, 48,119, -28, -95, 0+RUNLENGTH+COL256,
 64520, 48,119, -26, -95, 0+RUNLENGTH+COL256,
 68080, 48,116, -23, -95, 0+RUNLENGTH+COL256,
 71640, 48,112, -24, -96, 0+RUNLENGTH+COL256,
 75132, 48,112, -28, -95, 0+RUNLENGTH+COL256,
 78656, 48,117, -29, -95, 0+RUNLENGTH+COL256,
 82272, 48,119, -29, -95, 0+RUNLENGTH+COL256,
 85852, 48,118, -27, -95, 0+RUNLENGTH+COL256
};
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
text	*ModelNames [ MAXMODELS ] =
{
	"uktable",		// tables
	"ustable",
	"coffin",
	"square",
	"9ball",			// balls
	"triangle",
	"triangle",
	"trick",
	"head",
	"computer",
	"speaker"		// others
};
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
text	*IconNames[MAX_ICONS*2] =	// RESTORE: was [MAX_ICONS]; LoadIconFiles reads ten names (the 1996 build kept five and read MenuIcons as the other five)
{
	"balls",
	"table",
	"players",
	"prefs",
	"start",

	"speaker",
	"baize",
	"pad",
	"card",
	"trophy"
};
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
extern	G_SPRITE	spr_opts[];
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
int	MenuIcons[][ MAX_ICONS ]=
{
	   1,  26,  51,  76, 101,
	 126, 151, 176, 201, 226
};
int	MenuYpos[][ MAX_ICONS ]=
{
	 100, 179, 275, 374, 450,
	 108, 188, 276, 366, 467
};
int	MenuXpos[][ MAX_ICONS ]=		// Values To Be Changed Later
{
	  179+32, 216+32, 234+32, 253+32, 244+32,
	  179+32, 216+32, 234+32, 253+32, 244+32
};
long	*MenuData[2][ MAX_ICONS ];	// RESTORE: was [][ MAX_ICONS ], one row; LoadIconFiles fills two (the second ran over the next variables)
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
#ifdef	PSX
int	MenuVidy[][ MAX_ICONS ]=
{
	   0, 119, 256, 375,   0,
	   0, 119, 256, 346,   0
};
int	MenuVidx[][ MAX_ICONS ]=
{
	 640, 640, 640, 640, 704,
	 640, 640, 640, 640, 704
};
#endif
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	ConvertPalette( ubyte *pcpal, int num, int vx, int vy, flag trans )
{
	int	i;
	int	r,g,b;
	short	col;
	#ifdef	PSX
	RECT	rect;
	#endif

	for(i=0;i<num;i++)
		{
		r=(*pcpal++)>>4;
		g=(*pcpal++)>>4;
		b=(*pcpal++)>>4;
		#ifdef	PSX
		col = (b<<10)+(g<<5)+r;
		if((trans == YES) && (col !=0)) col|= 0x8000;
		if((trans == NO) && (col == 0)) col = 0x8000;
		if((trans == YES) && (col == 0) && (i!=0)) col = 0x8000+(1<<10);
		#else
		col = ((b<<10)+(g<<5)+r)|0x8000;
		#endif
		PaletteBuffer[i]=col;
		}
	setRECT(&rect, vx, vy, num, 1);
	LoadImage(&rect, PaletteBuffer);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	UpdateIcon( int icon )
{
 	int	opt;
	int	vx;

	if(icon == opt_frame) opt = menu_frames[opt_level][icon]+MenuIcons[opt_level][icon];
	else
		{
		opt = MenuIcons[opt_level][icon]-1;
		menu_frames[opt_level][icon]=0;
		}
	svx[ icon ]= MenuVidx[opt_level][icon]+(32*active_page);
	svy[ icon ]= MenuVidy[opt_level][icon];
	spw[ icon ]= spr_opts[ opt ].width;
	sph[ icon ]= spr_opts[ opt ].height;
	spx[ icon ]= spr_opts[ opt ].xoff+MenuXpos[opt_level][icon];
	spy[ icon ]= spr_opts[ opt ].yoff+MenuYpos[opt_level][icon];
	#ifdef	PSX
	spv[ icon ]= MenuVidy[opt_level][icon]&255;
	spu[ icon ]= 64*(active_page);
	tex[ icon ]= (svx[icon]>>6)+((MenuVidy[opt_level][icon]>>8)<<4)+0x80;
	#else

	#endif
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	DecompIcon( int icon )
{
	int	opt;
	ubyte	*image;
	#ifdef	PSX
	RECT	rect;
	#endif

	UpdateIcon( icon );

	bufadd = BufferPtr[ icon ][active_page];
	if(icon == opt_frame) opt = menu_frames[opt_level][icon]+MenuIcons[opt_level][icon];
	else opt = MenuIcons[opt_level][icon]-1;

	image = (ubyte*)(MenuData[opt_level][icon])+spr_opts[ opt ].offset;


	if(spr_opts[ opt ].flags & RUNLENGTH) rle2raw( image, bufadd );
	if(spr_opts[ opt ].flags & BITSTREAM) bit2raw( image, bufadd );
	#ifdef	PSX
	setRECT( &rect, svx[icon], svy[icon], spw[icon]/2, sph[icon]);
	LoadImage( &rect, bufadd );
	#else
// Chris put your download icon to vram here
	#endif
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	DecompAllIcons( void )
{
	int	i;

	for(i=0;i < MAX_ICONS ;i++) DecompIcon( i );
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	SetupAllIcons( void )
{
	DecompAllIcons();
	DrawOptionsScreen();
	DecompAllIcons();
	DrawOptionsScreen();
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	Choose_View_Draw( void );	// RESTORE: new/viewchoice.c
void	DrawOptionsScreen( void )
{
	#ifdef	PSX

	get_page();

	DrawSync(0);
	DrawIcons();

	AdjustClipArea();

	SetCameraPMD(&grotate, &camera, &trans, scaler);
	camera.vy+=32;
	DrawTitle();
	DrawReflect();
	DrawNumber8();
	DrawBackdrop();

	DrawInsideBall();

	DrawReflect1();
	DrawNumber81();

	Choose_View_Draw();	// RESTORE: the 2D or 3D choice panel (new/viewchoice.c), when it is up

	display_screen();

	#else
// Chris put your routines for updating screen here
	#endif

}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	AnimateCurrentIcon( void )
{
	static	int	temp = 0;

	temp++;
	temp %= 3;
	if(!temp)
		{
		menu_frames[opt_level][opt_frame]++;
		menu_frames[opt_level][opt_frame] %= 24;
		DecompIcon( opt_frame );
		}
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	LoadIconFiles( void )
{
	#ifdef	PSX
	RECT	rect;
	#endif
	int	i,j;
	text	tempname[256];

	for(i=0;i<2;i++)
		{
		for(j=0;j<MAX_ICONS;j++)
			{
			sprintf( tempname, "gfx/%s.psx", IconNames[(i*5)+j]);
			MenuData[i][j] = LoadFile( tempname );
			#ifdef	PSX
			*(MenuData[i][j])=0x0000;	// RESTORE: was a cast used as an lvalue, (short*)(*(MenuData[i][j]))=0; the 1996 build stores a long 0
			setRECT(&rect, 0,486+(i*MAX_ICONS)+j,256,1);
			LoadImage(&rect, MenuData[i][j]);
			#else
//	Chris put palette stuff here;
			#endif
			}
		}
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	LoadModelFiles( void )
{
	int	i;
	text	tempname[256];

	for(TableType=i=0;i<MAXMODELS;i++)
		{
		sprintf( tempname, "models/pmd/%s.pmd", ModelNames[i]);
		ModelsPMD[ i ] = LoadFile( tempname );
		}
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	readstr( long* addr, text* dest)
{
	text	*src;
	char	c;

	src = *(addr);
	while((c=*src++) > 0x20) *dest++ = c;
	if(c==13) src++;
	*dest++ = 0;
	*addr = src;
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
// RESTORE: returns the number in fixed point, one = 1.0 (0x1000000 for 8.24,
// 0x10000 for 16.16); was a double.  Integer only: whole part times one,
// plus the fraction digits scaled by fixdiv (24 bits, then cut to one's).
// The result equals NMS's (long)(gatof(str)*one) for every value in the
// table files.  whole starts at 0 (NMS's was unset for a number without '.').
long	gatof( text *str, long one)
{
	char	c;
	long	sgn;
	flag	frac;
	long	places;
	long	temp;
	long	whole;
	long	retval;

	sgn = places = 1;
	temp = 0;
	whole = 0;
	frac = NO;
	while((c = *(str++)) != 0)
		{
		if(c=='-') sgn = -1;
		else
			{
			if(c=='.')
				{
				whole = temp;
				temp = 0;
				frac = YES;
				}
			else
				{
				temp*=10;
				temp+=(c-'0');
				if(frac == YES) places*=10;
				}
			}
		}

	if(frac == NO) { whole = temp; temp = 0; }
	retval = whole*one + (fixdiv(temp, places) >> (24 - (one == 0x10000 ? 16 : 24)));

	return(retval*sgn);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
int	init_cushions( char *name)
{
	char	*tempfile;
	char	*tempfil1;
	char	string[80];
#ifndef	FINAL
	char	outname[128];
#endif
	int	point_pos;
	int	pocket_num,fp;
	long	temp;			// RESTORE: 8.24, was float

	int	bumper_num = 0;

	tempfile = LoadFile( name );
	tempfil1 = tempfile;

#ifndef	FINAL
	for(fp=0; fp<(strlen(name));fp++)
		{
		string[ fp ] = name[fp];
		if(name[fp] == '.')
			{
			string[fp+1]=0;
			fp = strlen(name);
			}
		}
	sprintf(outname,"%scsh",string);
#endif

	while((*tempfile)!=0)
		{
		readstr( &tempfile, string);

		if(strcmp(string,"point:")==0)
			{
			readstr( &tempfile, string);
			point_pos = atoi(string);
			switch(bumper_num)
				{
				case	1:
						readstr( &tempfile, string);
						cushions_y1[point_pos].x=gatof(string, 0x1000000);	// RESTORE: fixed point
						readstr( &tempfile, string);
						cushions_y1[point_pos].y=gatof(string, 0x1000000);
						readstr( &tempfile, string);
						cushions_y1[point_pos].cush_ang=gatof(string, 0x10000);
// RESTORE: NMS's fixed-point copy for the .csh file (gcushions_y1) went here;
// cushions_y1 is that copy now
// RESTORE: this break was on the #endif line above, where the preprocessor
// ignores it, so bumper 1 (cushions_y1) fell through into case 2: every other
// y1 point was skipped and the tokens after it went into cushions_x2[].
						break;
				case	2:
						readstr( &tempfile, string);
						cushions_x2[point_pos].x=gatof(string, 0x1000000);	// RESTORE: fixed point
						readstr( &tempfile, string);
						cushions_x2[point_pos].y=gatof(string, 0x1000000);
						readstr( &tempfile, string);
						cushions_x2[point_pos].cush_ang=gatof(string, 0x10000);
// RESTORE: NMS's fixed-point copy for the .csh file (gcushions_x2) went here;
// cushions_x2 is that copy now
						break;
				case	3:
						readstr( &tempfile, string);
						cushions_x3[point_pos].x=gatof(string, 0x1000000);	// RESTORE: fixed point
						readstr( &tempfile, string);
						cushions_x3[point_pos].y=gatof(string, 0x1000000);
						readstr( &tempfile, string);
						cushions_x3[point_pos].cush_ang=gatof(string, 0x10000);
// RESTORE: NMS's fixed-point copy for the .csh file (gcushions_x3) went here;
// cushions_x3 is that copy now
						break;
				case	4:
						readstr( &tempfile, string);
						cushions_y4[point_pos].x=gatof(string, 0x1000000);	// RESTORE: fixed point
						readstr( &tempfile, string);
						cushions_y4[point_pos].y=gatof(string, 0x1000000);
						readstr( &tempfile, string);
						cushions_y4[point_pos].cush_ang=gatof(string, 0x10000);
// RESTORE: NMS's fixed-point copy for the .csh file (gcushions_y4) went here;
// cushions_y4 is that copy now
						break;
				}
			}

		if(strcmp(string,"x_points:")==0)
			{
			readstr( &tempfile, string);
			xpoints=atoi(string);
			}

		if(strcmp(string,"y_points:")==0)
			{
			readstr( &tempfile, string);
			ypoints=atoi(string);
			}

		if(strcmp(string,"pocket_radius:")==0)
			{
			readstr( &tempfile, string);
			pockets[pocket_num].radius=(gatof(string, 0x1000000)*3)/2;	// RESTORE: 8.24, *1.5
// RESTORE: gpockets (the fixed-point copy) is pockets now
			}

		if(strcmp(string,"edge_step_value:")==0)
			{
			readstr( &tempfile, string);
			bumper_scale = gatof(string, 0x1000000);
			}

		if(strcmp(string,"min_value:")==0)
			{
			readstr( &tempfile, string);
			temp = gatof(string, 0x1000000);
			switch(bumper_num)
				{
				case	4:
						max_cush_x4=temp;
						break;
				case	2:
						min_cush_y3=temp;
						break;
				case	3:
						max_cush_y2=temp;
						break;
				case	1:
						min_cush_x1=temp;
						break;
				}
			}

		if(strcmp(string,"bumper:")==0)
			{
			readstr( &tempfile, string);
			bumper_num = atoi(string);
			}

		if(strcmp(string,"pocket:")==0)
			{
			readstr( &tempfile, string);
			pocket_num = atoi(string);
			}

		if(strcmp(string,"pocket_x:")==0)
			{
			readstr( &tempfile, string);
			pockets[pocket_num].x=gatof(string, 0x1000000);
			}

		if(strcmp(string,"pocket_y:")==0)
			{
			readstr( &tempfile, string);
			pockets[pocket_num].y=gatof(string, 0x1000000);
			}


		}
	Free_Ram_Ptr(tempfil1);
#ifndef	FINAL
	fp = OpenFileToSave( outname );
	write_long_to_disk( fp, xpoints );
	write_long_to_disk( fp, ypoints );
// RESTORE: the tables are fixed point now: written as they are (they were
// the gcushions_* and gpockets copies, *0x1000000 conversions here)
	write_long_to_disk( fp, bumper_scale );
	write_long_to_disk( fp, min_cush_x1 );
	write_long_to_disk( fp, max_cush_y2 );
	write_long_to_disk( fp, min_cush_y3 );
	write_long_to_disk( fp, max_cush_x4 );
	write_block_to_disk( fp, &pockets[0], 6*12 );
	write_block_to_disk( fp, &cushions_y1[0], ypoints*12 );
	write_block_to_disk( fp, &cushions_x2[0], xpoints*12 );
	write_block_to_disk( fp, &cushions_x3[0], xpoints*12 );
	write_block_to_disk( fp, &cushions_y4[0], ypoints*12 );
	CloseFile(fp);
#endif
}
