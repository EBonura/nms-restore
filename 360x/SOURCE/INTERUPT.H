
#ifndef	__H_INTERUPT
#define	__H_INTERUPT

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Init_Order_Table();			/* INITIALISE GRAPHICS ORDERING TABLE */
void	VblCallback();				/* CALLBACK FOR VBLANK */
void	VSyncCallBack(ULONG);		/* CALLBACK FOR VSYNC */
void	Start_Loop(void);			/* RUN THIS AT THE START OF THE MAIN LOOP */
void	End_Loop(void);			/* RUN THIS AT THE END OF THE MAIN LOOP */
void	Interupt_GPU_Stuff();		/* UPDATE LIVE DOWNLOADING */

/**************************************
*       GAME GENERAL CONSTANTS        *
**************************************/

#define	OTLENGTH		12

#define	SCREENWIDTH	(320)		/* WIDTH OF SCREEN */
#define	SCREENHEIGHT	(240)		/* HEIGHT OF SCREEN */

#define	SCREENOFF		SetDispMask(0);   
#define	SCREENON		SetDispMask(1);     
#define	HALT			VSync(0);

#define	MOD			%
#define	NOT			!
#define	BOR 			|
#define	LOR			||
#define	XOR			^
#define	EOR			^
#define	INT			SHORT

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	LIVEDOWNLOAD	1
#define	MAXINTERGPUS	32-1

/**************************************
*          DECLARE STRUCTURES         *
**************************************/

typedef struct{
			ULONG image;	/*0*/		/*whoops should be a *UBYTE */
			ULONG pal;	/*4*/		/*whoops should be a *USHORT */
			UBYTE x;		/*8*/  
			UBYTE y;		/*9*/
			UBYTE w;		/*10*/
			UBYTE h;		/*11*/ 
			SHORT u;		/*12*/
			SHORT v;		/*14*/
			USHORT clut;	/*16*/
			USHORT tpage;	/*18*/ 
			USHORT flags;	/*20*/
			}SPRITE;		/*22*/		/*24 bytes*/

typedef struct{
			UBYTE type;
			SPRITE *spr;
			ULONG off;
			}SPRITEQUEUE;

typedef struct{
			RECT rect;
			SPRITE *spr;
			RECT pal;
			}TEMPSPRITE;

typedef struct{
			UBYTE r,g,b;
			}RGB;

typedef struct{
			SHORT	scalex;
			SHORT	scaley;
			LONG		rotate;
			BYTE		semitrans;
			BYTE		red;
			BYTE		green;
			BYTE		blue;
			BYTE		center;
			BYTE		flipx;
			BYTE		flipy;
			BYTE		forceZ;
			BYTE 	oldrgb;
			USHORT	doitlive;
			ULONG	attribute;
			USHORT	overrideclut;
			}SPRCTRL;

/**************************************
*         DECLARE STRUCTURES          *
**************************************/

typedef struct{ULONG type;
			RECT rect;
			USHORT x;
			USHORT y;
			}MOVEIMAGE;

typedef struct{ULONG type;
			RECT rect;
			ULONG *data;
			ULONG *free;
			RECT rectpal;
			ULONG *pal;
			struct SPRITE *spr;
			}SPRITEIMAGE;

/**************************************
*          DECLARE VARIABLES          *
**************************************/

extern	MOVEIMAGE		*InteruptGPU[MAXINTERGPUS];	/* DATA FOR LIVE DOWNLOADED SPRITES */
extern	USHORT		intergpu;		/* DATA FOR INTERUPTGPU */

//extern	MOVEIMAGE 	*pointmove;		/* MOVE DATA FROM A TO B */
extern	SPRITEIMAGE	*pointsprite;		/* LOAD SPRITE FROM MEMORY */

extern	GsOT		Wot[2];			/* ORDERING TABLE HANDLERS */
extern	GsOT_TAG	zsorttable[2][1<<OTLENGTH];	/* ORDERING TABLES */

extern	ULONG 	Evbl;			/* EVENT FOR VBLANK OCCURANCE */

extern	volatile 	ULONG	vblank;		/* THIS IS THE CURRENT VBLANK */
extern	volatile 	ULONG	ovblank;		/* THIS IS THE OLD VBLANK */

extern	BYTE		screentoggle;		/* WHICH SCREEN YOU ARE CURRENTLY WORKING ON */
extern	LONG		gamecycle;		/* GAME CYCLE COUNTER, UPDATED EVERY GAME TURN */

extern	USHORT	currentlive;		/* CURRENT LIVE DOWNLOAD SPRITE */
extern	UBYTE	*lastfree;		/* CHECK FOR LIVE DOWNLOADING HAS BEEN FREED */

#endif

