
#ifndef	__H_FILES
#define	__H_FILES

/**************************************
*          DECLARE ROUTINES           *
**************************************/

UBYTE	LoadHere(BYTE* name,void *buffer);
UBYTE*	LoadFile(BYTE* name);
LONG	FileInfo(UBYTE *index,BYTE *filespec,LONG *firstsec,LONG *size);
UBYTE	InitialiseFiles(void);
UBYTE	InitCD(BYTE *index,BYTE *filename);
SHORT	Stream_Vab_Body(BYTE *name,SHORT vabid);	/* RESTORE: platform */

/**************************************
*          DECLARE DEFINES            *
**************************************/

CdlFILE	fp;						/* LIBCD FILE STRUCT, INCLUDING THE LENGTH */

extern	LONG		mode;			/* THE MODE WORD FOR CDREAD */
extern	UBYTE	status;			/* 0=FAIL, RETURN VALUE FROM READING FROM CD */
extern	UBYTE	result[8];		/* RETURN VALUES FOR ACCESSING CD */

extern	LONG		lastfilelength;	/* LENGTH OF THE LAST FILE */

extern	LONG		startdatsector;	/* STARTING POINT FOR BIG .DAT FILE */
extern	LONG		firstplusdat;		/* SECTOR NUMBER OF .DAT + FIRST SECTOR */

extern	LONG		firstsector;		/* WHERE TO LOAD .DAT INDEX FILE */
extern	LONG		numberofbytes;		/* NUMBER OF BYTES TO READ */

extern	UBYTE	indexbuffer[16384];	/* 512 ENTRY INDEX FOR .DAT FILE */

#endif

