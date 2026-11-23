
#ifndef	__H_SPRITE
#define	__H_SPRITE

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Print_Sprite(SPRITE *,SHORT,SHORT,UBYTE);
SPRITE*	Find_Spr(SPRITE* spr,USHORT num);
void	Reset_Spr_Paras(void);
void	Sprite_Off(SPRITE *);
void	Init_Tpages();
UBYTE	Add_Res_Page(UBYTE *,UBYTE);
void	Reset_Cluts();
ULONG	Get_location_from_spr(SPRITE *);

/**************************************
*        GENERAL SPRITE CODE          *
**************************************/

void	Bit2raw(UBYTE *,UBYTE *);
void	Rle2raw(UBYTE *,UBYTE *);
ULONG	Find_Hole_In_VRAM(UBYTE,UBYTE);
void	Xor_VRAM_Hole(ULONG,UBYTE,UBYTE);

/**************************************
*     PRE DOWNLOADED SPRITE CODE      *
**************************************/

void	Init_Spr_List(SPRITE *,ULONG loadoffset);
void    	Init_Spr_One(SPRITE *,ULONG loadoffset);
ULONG	Add_Sprite_Frame(SPRITE *spr);
USHORT	Add_Clut16(UBYTE *palette);
USHORT	Add_Clut256(UBYTE *palette);

/**************************************
*    LIVE DOWNLOADING SPRITE CODE     *
**************************************/

void	Add_Spr_Loadoffset(SPRITE *spr,ULONG loadoffset);
void    	Init_Spr_One_Live(SPRITE *,ULONG loadoffset);
ULONG	Add_Sprite_Frame_Live(SPRITE *spr);
USHORT	Add_Clut16_Live(UBYTE *palette);
USHORT	Add_Clut256_Live(UBYTE *palette);

void	Del_Clut16_Live(USHORT clutaddr);
void	Del_Clut256_Live(USHORT clutaddr);
void	Delete_Spr_One_Live(SPRITE *,UBYTE* address);
void	Del_Sprite_Frame_Live(SPRITE *,UBYTE* address);
void	Delete_Last_Downloaded_sprites(void);

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	MAXPOLYS		3072			/* MAXIMUM POLYGONS ON SCREEN */
#define	PACKMAX2		(MAXPOLYS*24)		/* MAXIMUM POLYGON PACKET AREA */
#define	MAXTEMPSPRITES	64			/* MAXIMUM ITEMS ON LIVE DOWNLOAD */

/**************************************
*    VARIOUS FLAGS FOR SPRITE DATA    *
**************************************/

#define	COL16		0
#define	FLIP 		1
#define	COL256 		2
#define	COL16BIT 		4
#define	BITSTREAM 	8
#define	RUNLENGTH 	16
#define	FIRSTSPR 		32
#define	RESPAGE 		64
#define	LASTSPR 		128
#define	NOIMAGE		256
#define	TEMPVRAM		512
#define	DONTXORVRAM    1024
#define	MAPSPR		2048			/* UNUSED */
#define	INVRAM		4096
#define	SPLIT		8192

/**************************************
*          DECLARE VARIABLES          *
**************************************/

extern	GsOT_TAG	zsorttable[2][1<<OTLENGTH];	/* Entity of the ordering table */
extern	GsOT     	Wot[2];			/* Ordertable handlers. 2 for double buffering */
extern	PACKET	out_packet[2][PACKMAX2];

extern	USHORT	tpage[22];		/* texture page ID */
extern	ULONG	respagesloaded[22];			
extern	ULONG 	vram_slot_table[22][32];

extern	UBYTE	cluts16[320];
extern	UBYTE	cluts256[16];

extern	UBYTE	respagecnt;

extern	SHORT	scalex;
extern	SHORT	scaley;
extern	LONG		rotate;
extern	BYTE		semitrans;
extern	BYTE		rgb;
extern	BYTE		center;
extern	BYTE		flipx;
extern	BYTE		flipy;
extern	BYTE		forceZ;
extern	BYTE		oldrgb;
extern	USHORT  	doitlive;
extern	ULONG	attribute;
extern	UBYTE 	gamefirstpass;

extern	DR_ENV	enviro;
extern	DRAWENV	drawenv;

extern	SPRCTRL	sprctrl;

extern	ULONG	maxpolycount;

#endif

