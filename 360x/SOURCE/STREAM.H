
#ifndef	__H_STREAM
#define	__H_STREAM

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	PlayStream(BYTE *fmvname,BYTE *palname);	/* PLAY FMV FROM FILENAME */

/**************************************
*          DECLARE VARIABLES          *
**************************************/

extern	BYTE		faststreaming;		/* 2 DIFFERENT PLAY SPEEDS */
extern	BYTE		leadin;			/* LEAD IN FOR START OF THE FLIC */

#endif

