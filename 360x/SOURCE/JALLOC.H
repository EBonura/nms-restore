
#ifndef	__H_JALLOC
#define	__H_JALLOC

/**************************************
*          DECLARE VARIABLES          *
**************************************/

typedef unsigned char 	uchar;
typedef unsigned long	ulong;

#define MAXJALLOCS	128

/**************************************
*         DECLARE STRUCTURES          *
**************************************/

typedef struct{
			LONG		offset;		/* WHERE THIS BLOCK STARTS, OFFSET FROM BASE */
			LONG		size;		/* SIZE OF THIS BLOCK */
			BYTE		inuse;		/* IS THIS BLOCK IN USE */
			}jallocentType;

typedef struct{
	jallocentType	blocks[MAXJALLOCS];	/* ARRAY OF CONTROL STRUCTS */
	ULONG			base;		/* BASE ADDRESS FOR GLOBAL BLOCK */
	LONG				size;		/* SIZE OF AREA */
	int				lastused;		/* NUMBER OF LAST USED BLOCK */
    int				noofactive;	/* NUMBER OF ACTIVE BLOCKS */
	}jallocType;

extern	jallocType JALLOC;

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	ShowBlocks(jallocType *jp);
void	Jalloc_Init(jallocType *jp, ulong base, long size);
UBYTE	*Jalloc_Alloc(jallocType *jp, long size);
void	Jalloc_Free(jallocType *jp, uchar *blk);
void	Jalloc_Reset(jallocType *jp);

#endif

