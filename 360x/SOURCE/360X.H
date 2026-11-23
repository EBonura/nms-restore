
#ifndef	__H_360X
#define	__H_360X

/**************************************
*         GENERAL CONSTANTS           *
**************************************/

#define	ON		1
#define	BOTH		2
#define	OFF		0

#define	STEREO	0
#define	MONO		1

#define  TRUE		1
#define	FALSE	0

#define	YES		1
#define  NO		0

#define	PASS		1				/* PASSED */
#define	FAIL		0				/* FAILED */

#define	JAPANESE	0
#define	AMERICAN	1
#define	EUROPEAN	2

#define	ENGLISH	0
#define	FRENCH	1
#define	GERMAN	2

#define	BYTE		char
#define	UBYTE	unsigned char
#define	SHORT	short
#define	USHORT	unsigned short
#define	LONG		long
#define	ULONG	unsigned long
#define	FLAG		unsigned char

#define	INTOGAME		(240)
#define	INTOTITLE		(241)
#define	INTOSHIP		(242)
#define	INTOHIGH		(243)
#define	INTOCREDITS	(244)

#define CRASH while(screentoggle!=2)pollhost();
#define SPTRCAST

/**************************************
*         DECLARE STRUCTURES          *
**************************************/

typedef struct	{
				USHORT	control1;
				USHORT	control2;
				USHORT	control3;
				USHORT	control4;
				UBYTE	fonttype;
				UBYTE	textstuff[64];
				UBYTE	nullstring;
				}TEXT;

/**************************************
*       DATA TYPES FOR JOYPADS        *
**************************************/

#define PaDLup 		0x0010
#define PaDLdown        0x0040
#define PaDLleft        0x0080
#define PaDLright       0x0020
#define PaDRup 		0x1000
#define PaDRdown        0x4000
#define PaDRleft        0x8000
#define PaDRright       0x2000
#define PaDselect       0x0001
#define PaDlu           0x0400
#define PaDld           0x0100
#define PaDru           0x0800
#define PaDrd           0x0200
#define PaDstart        0x0008

/* RESTORE: PLATFORM (owner's decision, 2026-11-13: "the select language should accept both start and X"; "the menu should have proper X to get in, circle to get out"). In the menus Cross enters or confirms as Start does, and Circle goes back one level or leaves the menu. In-game controls and the high score name entry (where Cross types the letter) are unchanged. */
#define PaDenter        (PaDstart|PaDRdown)
#define PaDback         PaDRright

/**************************************
*    INCLUDE LIBRARY HEADER FILES     *
**************************************/

#include <r3000.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/file.h>

#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libsn.h>
#include <libcd.h>
#include <libspu.h>
#include <libsnd.h>
#include <libmath.h>
#include <asm.h>
#include <kernel.h>
#include <cardio.h>
#include <ctrller.h>

/**************************************
*            INCLUDE FILES            *
**************************************/

#include	"GLOBALS.H"


#include "FIRE.H"
#include "PROCESS.H"		/* RESTORE: NEW WORK (owner's decision): Hit_Sprite, hitsleft */
#include "PARTICLE.H"
#include "INTERUPT.H"
#include "SPRITE.H"

#include "LOADDATA.H"
#include "CONTROL.H"
#include "SCROLLER.H"
#include "LEVEL.H"
#include "PANEL.H"
#include	"SPRITE2.H"

#include "PANEL.H"
#include "GENERAL.H"
#include "FILES.H"
#include "SOUND.H"
#include "SPRLIST.H"
#include "JALLOC.H"

#include "TITLE.H"					/* INTRO STUFF */
#include "OPTIONS.H"
#include "LANGUAGE.H"
#include "SHIPDEMO.H"
#include "PLANETS.H"
#include "HIGH.H"
#include "CREDITS.H"
#include "TELETYPE.H"
#include "LOADSAVE.H"


#endif

