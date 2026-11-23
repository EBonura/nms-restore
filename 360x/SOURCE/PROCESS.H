
#ifndef	__H_PROCESS
#define	__H_PROCESS

/**************************************
*          DECLARE ROUTINES           *
**************************************/

extern	UBYTE	hitsleft[];			/* RESTORE: NEW WORK (owner's decision) */
FLAG	Hit_Sprite(long i);
FLAG	Placed_Shootable(long name);		/* RESTORE: OWNER'S DECISION, PROCESS.C sprites_shoot */
FLAG	Can_Die(long start);			/* RESTORE: NEW WORK (owner's decision) */

/**************************************
*          DECLARE DEFINES            *
**************************************/

/**************************************
*          DECLARE STRUCTURES         *
**************************************/

#endif

