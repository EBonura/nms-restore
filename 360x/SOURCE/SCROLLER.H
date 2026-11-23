
#ifndef	__H_INTRO
#define	__H_INTRO

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Init_Map(BYTE *binfile,BYTE *mapfile,BYTE *palfile,BYTE *bmffile,BYTE *colfile);
void	Sort_Map(void);
void	Print_The_Map(void);
void	Do_Scroll_Download(void);
void	Get_Maptable_Data(void);
void	Update_Scroll(LONG craftx,LONG crafty);

/**************************************
*           DECLARE DEFINES           *
**************************************/

#define	XEDGECLIP	64
#define	YEDGECLIP	64

/**************************************
*         DECLARE STRUCTURES          *
**************************************/

typedef struct	{
				SHORT	xoff;
				SHORT	yoff;
				SHORT	page;
				SHORT	spare;
				}MAPDATA;

/**************************************
*          DECLARE DEFINES            *
**************************************/

extern	BYTE		*levelbin;		/* POINTER TO CURRENT LEVEL BIN */
extern	BYTE		*levelmap;		/* POINTER TO CURRENT LEVEL MAP */
extern	BYTE		*levelpal;		/* POINTER TO CURRENT LEVEL PAL */
extern	BYTE		*levelcol;		/* POINTER TO CURRENT LEVEL COLL */
extern	SHORT	colwidth;		/* RESTORE: bug fix, THE COLLISION MAP'S WIDTH (Sort_Map) */
extern	LONG		levelcolsize;		/* RESTORE: ITS LENGTH IN BYTES */

extern	BYTE		*downloadbufferx;		/* POINTER TO VERTICAL STRIP */
extern	BYTE		*downloadbuffery;		/* POINTER TO HORIZONTAL STRIP */

extern	LONG		startscreenx;		/* START X POSITION ON THIS LEVEL */
extern	LONG		startscreeny;		/* START Y POSITION ON THIS LEVEL */

extern	LONG		screenx;			/* PIXEL POSITION ACROSS MAP */
extern	LONG		screeny;			/* PIXEL POSITION DOWN MAP */

extern	LONG		scrollxoff;		/* X OFFSET FOR WHERE TO GET DATA FROM */
extern	LONG		scrollyoff;		/* X OFFSET FOR WHERE TO GET DATA FROM */

extern	SHORT	mapwidth;			/* HEIGHT OF THIS MAP */
extern	SHORT	mapheight;		/* WIDTH OF THIS MAP */

extern	MAPDATA	maptextures[21*17];		/* TEXTURE LIST FOR THE SCROLLER */

extern	LONG		spritenumber;

#endif

